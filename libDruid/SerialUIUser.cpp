/*
 * SerialUIUser.cpp -- SerialUIUser implementation.
 *
 *	Druid4Arduino Copyright (C) 2013 Pat Deegan, psychogenic.com
 *	http://flyingcarsandstuff.com/projects/druid4arduino/
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ExternalIncludes.h"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/find.hpp>
#include <boost/algorithm/string/erase.hpp>
#include <boost/regex.hpp>
#include <unistd.h>

#include <SUIStrings.h>
#include "SerialUIUser.h"
#include "SerialGUIConfig.h"


namespace DRUID {

SerialUIUser::SerialUIUser() : SerialUser(),
		message_rcvd(false),
		last_rcvdcheck_len(0),
		eot_checks_enabled(true),
		eot_str(SUI_SERIALUI_PROG_ENDOFTRANSMISSION)
{


}

SerialUIUser::SerialUIUser(boost::asio::io_service& io_service,
		unsigned int baud, const DRUIDString& device) :
			SerialUser(io_service, baud, device),
			required_input(InputType_None),
			message_rcvd(false),
			last_rcvdcheck_len(0),
			eot_checks_enabled(true),
			eot_str(SUI_SERIALUI_PROG_ENDOFTRANSMISSION)
{
}

SerialUIUser::~SerialUIUser() {


}

#include <boost/regex.hpp>
SerialUIControlStrings SerialUIUser::setupProgModeStrings(DRUIDString & progRetStr)
{
	// TODO:FIXME
	if (progRetStr.size())
	{
		DRUIDString stringSeps(DRUIDString("([^") + (*(progRetStr.begin())) +"]+)");
		boost::regex stringSepRegex(stringSeps, boost::regex::normal);
		SerialUserStringList sepList;

		boost::regex_split(std::back_inserter(sepList), progRetStr, stringSepRegex);

		DRUID_DEBUG2("Program settings string", progRetStr);
		if (sepList.size() >= 12)
		{
			ctrl_strings.version = sepList[0];
			ctrl_strings.up_key = sepList[1];
			ctrl_strings.exit_key = sepList[2];
			ctrl_strings.error = sepList[3];
			ctrl_strings.help_key = sepList[4];
			ctrl_strings.prefix_command = sepList[5];
			ctrl_strings.prefix_submenu = sepList[6];
			ctrl_strings.help_sep = sepList[7];
			ctrl_strings.more_str = sepList[8];
			ctrl_strings.more_num = sepList[9];
			ctrl_strings.prompt_str = sepList[10];
			ctrl_strings.eot_str = sepList[11];

			eot_str = ctrl_strings.eot_str;
		}
	}
	return ctrl_strings;
}
SerialUIControlStrings SerialUIUser::enterProgramMode()
{

	static const DRUIDString command_mode_program(SUI_STRINGS_MODE_PROGRAM);
	clear();

	eot_checks_enabled = false;
	this->send(command_mode_program);

	time_t maxTime = time(NULL) + SERIALGUI_MAX_RESPONSE_DELAY_SECONDS;
	usleep(300000);


	DRUIDString::iterator progMsgSizeIter;
	DRUIDString::iterator progMsgStartIter;
	bool modeEntered = false;
	bool haveStart = false;
	unsigned int prefixSize = 0;
	unsigned int expectedSize = 0;

	DRUIDString inc_msg;
	inc_msg.reserve(500);
	do {
			if (incoming_message.size())
			{

				inc_msg = incoming_message;
				DRUIDString::iterator iter = inc_msg.begin();
				while ( (!expectedSize) && iter != inc_msg.end())
				{

					if (! haveStart)
					{
						if ( (*iter) >= '0' && (*iter) <= '9')
						{
							haveStart = true;
							progMsgSizeIter = iter;

						}
					} else {

						if (iter != progMsgSizeIter && ( (*iter) < '0' || (*iter) > '9'))
						{
							// looks like we have our expected size...
							std::string sizeBuf;
							sizeBuf.reserve(iter - progMsgSizeIter);
							std::copy(progMsgSizeIter, iter, std::back_inserter(sizeBuf));
							progMsgStartIter = iter;

							try
							{

								prefixSize = (progMsgStartIter - inc_msg.begin());
								expectedSize = boost::lexical_cast<unsigned int>(sizeBuf);

								// also account for any chars at the beginning
								// expectedSize += ;


								DRUID_DEBUG2("Prog mode ret string should have length", expectedSize);
							} catch(boost::bad_lexical_cast &)
							{
								DRUID_DEBUG2("Weird cast attempt for sizeBuf", sizeBuf);
							}
						}
					}

					iter++;
				}

				if (expectedSize && inc_msg.size() >= (prefixSize + expectedSize))
				{

					std::string progRetStr;
					std::string leftOvers;
					progRetStr.reserve(prefixSize + expectedSize + 5);

					DRUIDString::iterator progIterEnd = progMsgStartIter + expectedSize;
					std::copy(progMsgStartIter, progIterEnd, std::back_inserter(progRetStr));

					if (progIterEnd == inc_msg.end())
					{
						inc_msg = "";
					} else {
						DRUIDString newIncoming;
						newIncoming.reserve(inc_msg.end() - progIterEnd);
						std::copy(progIterEnd, inc_msg.end(), std::back_inserter(newIncoming));

						inc_msg = newIncoming;
					}

					incoming_message = inc_msg;


					eot_checks_enabled = true;

					DRUID_DEBUG2("Incoming message now -->", incoming_message);


					return setupProgModeStrings(progRetStr);

				}

			}

			usleep(200000);
		} while ( time(NULL) <= maxTime && !modeEntered);

	eot_checks_enabled = true;
	return ctrl_strings;
}

void SerialUIUser::serialReceived(char* buffer, size_t bytes_transferred) {

	static const size_t eot_str_len = eot_str.length();

	// DRUID_DEBUG2("SerialUIUser::serialReceived", bytes_transferred);
	if (bytes_transferred)
	{

		for (size_t i=0; i< bytes_transferred; i++)
		{
			DRUID_DEBUGVERBOSE(buffer[i]);
		}
		incoming_message.append(buffer, bytes_transferred);

		if (eot_checks_enabled) {
			DRUIDString::iterator findIter = std::search(
					incoming_message.begin(), incoming_message.end(),
					eot_str.begin(), eot_str.end());

			if (findIter != incoming_message.end()) {
				DRUID_DEBUG("Found EOT marker!");

				// clear the last message
				last_message.clear();
				// copy the newly arrived message to last_message
				last_message.reserve(findIter - incoming_message.begin());
				std::copy(incoming_message.begin(), findIter,
						std::back_inserter(last_message));

				DRUID_DEBUG2("LAST MESSAGE IS", last_message);

				findIter += eot_str_len;

				if (findIter != incoming_message.end()) {
					DRUIDString newIncoming;
					newIncoming.reserve(incoming_message.end() - findIter);
					std::copy(findIter, incoming_message.end(),
							std::back_inserter(newIncoming));

					incoming_message = newIncoming;
					DRUID_DEBUG2("Incoming overflow", incoming_message);
				}
			}
			// DRUID_DEBUG(last_message);
		}
	}


}

bool SerialUIUser::messageReceived() {

	if (last_message.size())
	{
		message_rcvd = true;
		checkIfMessageWasError();
		checkIfRequiresInput();

	}

	return message_rcvd;

}
void SerialUIUser::flushReceiveBuffer() {
	incoming_message.clear();
}
void SerialUIUser::lastMessageClear() {
	last_message.clear();
	message_rcvd = false;
	last_rcvdcheck_len = 0;
}

const DRUIDString& SerialUIUser::lastMessageRef() {
	return last_message;
}

DRUIDString SerialUIUser::lastMessage() {
	return last_message;
}


std::string SerialUIUser::truncatePromptFrom(std::string & msg)
{

	const std::string regexp_str(DRUIDStdString("^(.+?") + ctrl_strings.prompt_str + ").*?$");
	boost::regex prompt_finder_regexp(regexp_str);

	return boost::regex_replace(msg, prompt_finder_regexp,
			std::string(""), boost::match_any | boost::format_all);

}
void SerialUIUser::checkIfMessageWasError()
{
	static const DRUIDString errorPrefix_str(SUI_SERIALUI_MESSAGE_ERROR_PREFIX);
	const DRUIDString errorGeneric_str(ctrl_strings.error);

	// make a copy, last_message can change asynchronously
	DRUIDString msg(last_message);

	if (! boost::algorithm::find_first(msg, errorGeneric_str))
	{
		// no error, fuggetaboudit
		return;
	}

	// get rid of "ERROR:"
	boost::algorithm::erase_first(msg, errorPrefix_str);
	boost::algorithm::erase_first(msg, errorGeneric_str);

	// set the last_message to the "cleaned up" version
	last_message = msg;
	SerialUserStringList errMsgList = lastMessageAsList();

	std::string errMsg;
	errMsg.reserve(msg.size());
	for (SerialUserStringList::iterator iter = errMsgList.begin();
			iter != errMsgList.end(); iter++) {
		errMsg += truncatePromptFrom(*iter);
	}

	serialError(errMsg);


}
void SerialUIUser::checkIfRequiresInput()
{
	const DRUIDString moreStringDataPrompt(ctrl_strings.more_str);
	const DRUIDString moreNumericDataPrompt(ctrl_strings.more_num);
	const boost::regex reqInputRegex(DRUIDString("^((")
			+ moreStringDataPrompt + ")|(" + moreNumericDataPrompt + "))$");

	required_input = InputType_None;


	SerialUserStringList linesOfInterest = this->lastMessageAsList();
	for (SerialUserStringList::iterator iter = linesOfInterest.begin();
				iter != linesOfInterest.end(); iter++)
	{
		boost::smatch what;
		if (boost::regex_match(*iter, what, reqInputRegex))
		{
			if (what[2].length())
			{
				required_input = InputType_String;
				return;
			} else if (what[3].length())
			{
				required_input = InputType_Numeric;
				return;
			}
		}
	}

	return;
}

bool SerialUIUser::upMenuLevel()
{

	const std::string upLevel(ctrl_strings.up_key);

	return this->sendAndReceive(upLevel);


}

} /* namespace DRUID */
