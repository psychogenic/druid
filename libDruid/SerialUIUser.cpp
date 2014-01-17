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

#include "libDruid/ExternalIncludes.h"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/find.hpp>
#include <boost/algorithm/string/erase.hpp>
#include <boost/regex.hpp>
#include <unistd.h>

#include <SUIStrings.h>
#include "libDruid/SerialUIUser.h"
#include "libDruid/SerialGUIConfig.h"


namespace DRUID {
void LockedLastMessage::setTo(const DRUIDString & val)
{

	DRUID_DEBUG("LockedLastMessage::setTo locking");
	message_mutex.lock();
	DRUID_DEBUG("LockedLastMessage::setTo locked");
	last_message = val;
	message_mutex.unlock();
	DRUID_DEBUG("LockedLastMessage::setTo unlocked");

}

DRUIDString LockedLastMessage::get()
{

	DRUID_DEBUG("LockedLastMessage::get locking");
	message_mutex.lock();
	DRUID_DEBUG("LockedLastMessage::get locked");
	DRUIDString retVal(last_message);
	message_mutex.unlock();
	DRUID_DEBUG("LockedLastMessage::get unlocked");
	return retVal;


}



void LockedLastMessage::clear()
{

	DRUID_DEBUG("LockedLastMessage::clear locking");
	message_mutex.lock();
	DRUID_DEBUG("LockedLastMessage::clear locked");
	last_message.clear();
	message_mutex.unlock();
	DRUID_DEBUG("LockedLastMessage::clear unlocked");

}

SerialUIUser::SerialUIUser() : SerialUser(),
		message_rcvd(false),
		last_rcvdcheck_len(0),
		eot_checks_enabled(true),
		auto_replace_last_message(true),
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
			auto_replace_last_message(true),
			eot_str(SUI_SERIALUI_PROG_ENDOFTRANSMISSION)
{
}

SerialUIUser::~SerialUIUser() {


}

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

			if (sepList.size() >= 13 && sepList[12].size())
				ctrl_strings.more_stream = sepList[12];
			else
				ctrl_strings.more_stream = "~N/A~";

			eot_str = ctrl_strings.eot_str;
		}


		DRUID_DEBUG2("version", ctrl_strings.version);
		DRUID_DEBUG2("up_key", ctrl_strings.up_key);
		DRUID_DEBUG2("exit_key", ctrl_strings.exit_key);
		DRUID_DEBUG2("error", ctrl_strings.error);
		DRUID_DEBUG2("help_key", ctrl_strings.help_key);
		DRUID_DEBUG2("prefix_command", ctrl_strings.prefix_command);
		DRUID_DEBUG2("prefix_submenu", ctrl_strings.prefix_submenu);
		DRUID_DEBUG2("help_sep", ctrl_strings.help_sep);
		DRUID_DEBUG2("more_str", ctrl_strings.more_str);
		DRUID_DEBUG2("more_stream", ctrl_strings.more_stream);
		DRUID_DEBUG2("more_num", ctrl_strings.more_num);
		DRUID_DEBUG2("prompt_str", ctrl_strings.prompt_str);
		DRUID_DEBUG2("eot_str", ctrl_strings.eot_str);

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

		if (! incoming_message.size())
		{
			// nothing to do right now...
			usleep(200000);
			continue;
		}

		inc_msg = incoming_message;
		DRUIDString::iterator iter = inc_msg.begin();
		while ((!expectedSize) && iter != inc_msg.end()) {

			// expectedSize not yet set and we're not at the end of the incoming msg either

			if (!haveStart) {
				// still searching for the first num char
				if ((*iter) >= '0' && (*iter) <= '9') {
					// got it, this is our messagesize beginning
					haveStart = true;
					progMsgSizeIter = iter;

				}
			} else {
				// already have the start, need to find the last num char

				if (iter != progMsgSizeIter // size will be > 10, so we only get here if we're not still on the first num char
						&& ((*iter) < '0' || (*iter) > '9')) {

					// the iter is no longer pointing to a num
					// looks like we have our expected size...

					// put our size in a string
					std::string sizeBuf;
					sizeBuf.reserve(iter - progMsgSizeIter);
					std::copy(progMsgSizeIter, iter,
							std::back_inserter(sizeBuf));
					progMsgStartIter = iter;

					try {
						// get the expected size of the program strings string
						prefixSize = (progMsgStartIter - inc_msg.begin());
						expectedSize = boost::lexical_cast<unsigned int>(
								sizeBuf);

						// also account for any chars at the beginning
						// expectedSize += ;

						DRUID_DEBUG2("Prog mode ret string should have length",
								expectedSize);


					} catch (boost::bad_lexical_cast &) {
						DRUID_DEBUG2("Weird cast attempt for sizeBuf", sizeBuf);
					}
				}
			}

			iter++;
		}

		if (expectedSize && inc_msg.size() >= (prefixSize + expectedSize)) {

			DRUID_DEBUG2("We've got enough chars for the program strings message:", expectedSize);

			// now, we have the expected size set and have an incoming message
			// of at least that many chars

			std::string progRetStr; // will hold our program strings
			std::string leftOvers;

			// reserve enough space in our prog strings string
			progRetStr.reserve(prefixSize + expectedSize + 5);

			DRUIDString::iterator progIterEnd = progMsgStartIter + expectedSize;
			std::copy(progMsgStartIter, progIterEnd,
					std::back_inserter(progRetStr));

			if (progIterEnd == inc_msg.end()) {
				inc_msg = "";
			} else {
				DRUIDString newIncoming;
				newIncoming.reserve(inc_msg.end() - progIterEnd);
				std::copy(progIterEnd, inc_msg.end(),
						std::back_inserter(newIncoming));

				inc_msg = newIncoming;
			}

			incoming_message = inc_msg;

			eot_checks_enabled = true;

			DRUID_DEBUG2("program strings str now -->", progRetStr);
			DRUID_DEBUG2("Incoming message now -->", incoming_message);

			return setupProgModeStrings(progRetStr);

		}



		usleep(80000);
	} while ( time(NULL) <= maxTime && !modeEntered);

	eot_checks_enabled = true;
	return ctrl_strings;
}

void SerialUIUser::serialReceived(char* buffer, size_t bytes_transferred) {


	// DRUID_DEBUG2("SerialUIUser::serialReceived", bytes_transferred);
	if (! bytes_transferred)
	{
		// nothing to do
		return ;
	}

#ifdef SERIALGUI_DEBUG_ENABLE
	for (size_t i=0; i< bytes_transferred; i++)
	{
		DRUID_DEBUGVERBOSE(buffer[i]);
	}
#endif

	// append this data to the incoming msg
	last_msg.lock();
	incoming_message.append(buffer, bytes_transferred);

	last_msg.unlock();

	if (eot_checks_enabled)
		checkForLastMessage();

}

void SerialUIUser::checkForLastMessage()
{

	static const size_t eot_str_len = eot_str.length();

	// am checking for EOTs, so do it:

	DRUIDString::iterator findIter = std::search(incoming_message.begin(),
			incoming_message.end(), eot_str.begin(), eot_str.end());

	if (findIter == incoming_message.end()) {
		// No EOT found
		return;
	}

	DRUID_DEBUG("Found EOT marker!");

	DRUID_DEBUG("SerialUIUser::serialReceived locking");
	last_msg.lock();
	DRUID_DEBUG("SerialUIUser::serialReceived locked");

	// Get safe (locked) access to raw message
	DRUIDString newContents;
	newContents.reserve(findIter - incoming_message.begin());
	// actually copy the data from incomming to new contents container
	std::copy(incoming_message.begin(), findIter,
				std::back_inserter(newContents));


	DRUID_DEBUG2("LAST MESSAGE IS", newContents);


	// clear the last message
	DRUIDString & dirAccessLastMsg = last_msg.directAccess();
	if (auto_replace_last_message)
		dirAccessLastMsg = newContents;
	else
		dirAccessLastMsg += newContents;

	findIter += eot_str_len; // skip it...

	if (findIter != incoming_message.end()) {
		// we have extra stuff after the EOT, stash it for later.
		DRUIDString newIncoming;
		newIncoming.reserve(incoming_message.end() - findIter);
		std::copy(findIter, incoming_message.end(),
				std::back_inserter(newIncoming));

		incoming_message = newIncoming;
		DRUID_DEBUG2("Incoming overflow", incoming_message);
	}


	last_msg.unlock(); // stay away from "dirAccessLastMsg" from now on
	DRUID_DEBUG("SerialUIUser::serialReceived unlocked");


}

bool SerialUIUser::messageReceived() {

	if (last_msg.size())
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
	last_msg.clear();
	message_rcvd = false;
	last_rcvdcheck_len = 0;
}

const DRUIDString& SerialUIUser::lastMessageRef() {
	return last_msg.directAccess();
}

DRUIDString SerialUIUser::lastMessage() {
	return last_msg.get();
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
	DRUIDString msg(last_msg.get());

	if (! boost::algorithm::find_first(msg, errorGeneric_str))
	{
		// no error, fuggetaboudit
		return;
	}

	// get rid of "ERROR:"
	boost::algorithm::erase_first(msg, errorPrefix_str);
	boost::algorithm::erase_first(msg, errorGeneric_str);

	// set the last_message to the "cleaned up" version
	last_msg.setTo(msg);
	SerialUserStringList errMsgList = lastMessageAsList();

	std::string errMsg;
	errMsg.reserve(msg.size());
	for (SerialUserStringList::iterator iter = errMsgList.begin();
			iter != errMsgList.end(); iter++) {
		errMsg += truncatePromptFrom(*iter);
	}

	serialError(errMsg);


}

bool SerialUIUser::inputRequired(bool forceCheck) {

	if (forceCheck)
		checkIfRequiresInput();

	return required_input != InputType_None;
}

void SerialUIUser::checkIfRequiresInput()
{
	DRUID_DEBUG("SerialUIUser::checkIfRequiresInput()");

	const DRUIDString moreStringDataPrompt(ctrl_strings.more_str);
	const DRUIDString moreNumericDataPrompt(ctrl_strings.more_num);
	const DRUIDString moreStreamDataPrompt(ctrl_strings.more_stream);
	const DRUIDString regexStr("^(.*)(("
							+ moreStringDataPrompt
							+ ")|("
							+ moreNumericDataPrompt
							+ ")|("
							+ moreStreamDataPrompt
							+ "))\\s*$");
	const boost::regex reqInputRegex(regexStr, boost::regex::perl);

	required_input = InputType_None;

	DRUID_DEBUG2("searching for moredata", moreStringDataPrompt);
	DRUID_DEBUG2("searching for morenumdata", moreNumericDataPrompt);
	DRUID_DEBUG2("searching for morestreamdata", moreStreamDataPrompt);
	DRUID_DEBUG2("using regex\n\t", regexStr);

	DRUIDString lastLine;

	SerialUserStringList linesOfInterest = this->lastMessageAsList();
	for (SerialUserStringList::iterator iter = linesOfInterest.begin();
				iter != linesOfInterest.end(); iter++)
	{
		boost::smatch what;
		// DRUID_DEBUG2("checking line", *iter);
		DRUID_DEBUGVERBOSE("checking line:");
		DRUID_DEBUGVERBOSE(*iter);
		if (boost::regex_match(*iter, what, reqInputRegex))
		{
			DRUID_DEBUG("got a match...");
			required_input_prompt = "";
			if (what[1].length())
			{
				// prompt prefix:
				required_input_prompt = what[1];
			} else if (lastLine.size() > 2) {
				required_input_prompt = lastLine;

			}

			if (what[3].length())
			{
				required_input = InputType_String;
				DRUID_DEBUG("Need string input...");

				return;
			} else if (what[4].length())
			{
				required_input = InputType_Numeric;
				DRUID_DEBUG("Need num input...");
				return;
			} else if (what[5].length())
			{
				required_input = InputType_Stream;
				DRUID_DEBUG("Need stream input...");
				return;
			}

		} else {

			DRUID_DEBUG("NO match.");
		}

		lastLine = *iter;

	}

	return;
}

bool SerialUIUser::upMenuLevel()
{

	const std::string upLevel(ctrl_strings.up_key);

	return this->sendAndReceive(upLevel);


}

} /* namespace DRUID */
