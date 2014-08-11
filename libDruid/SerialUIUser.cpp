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


			try {
				// get the expected size of the program strings string

				ctrl_strings.version_num = boost::lexical_cast<float>(ctrl_strings.version);
				DRUID_DEBUG2("Got prog strings version", ctrl_strings.version_num);

			} catch (boost::bad_lexical_cast &) {
				DRUID_DEBUG2("Weird cast attempt for version", ctrl_strings.version);
				ctrl_strings.version_num = 1.0;
			}

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


			ctrl_strings.terminate_gui = "~N/A~";
			ctrl_strings.more_stream = "~N/A~";
			if (ctrl_strings.version_num >= 1.11)
			{


				if (sepList.size() >= 14)
				{
					if (sepList[12].size())
						ctrl_strings.more_stream = sepList[12];


					if (sepList[13].size())
						ctrl_strings.terminate_gui = sepList[13];

				} else if (sepList.size() >= 13)
				{
					// no stream setup... only terminate_gui...
					if (sepList[13].size())
						ctrl_strings.terminate_gui = sepList[12];

				}



			} else {

				// older version of SerialUI
				if (sepList.size() >= 13 && sepList[12].size())
					ctrl_strings.more_stream = sepList[12];


			}


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
		DRUID_DEBUG2("terminate_gui", ctrl_strings.terminate_gui);
		DRUID_DEBUG2("eot_str", ctrl_strings.eot_str);

	}
	return ctrl_strings;
}

void SerialUIUser::exitProgramMode()
{
	static const DRUIDString command_mode_user(SUI_STRINGS_MODE_USER);

	this->send(command_mode_user);

}
SerialUIControlStrings SerialUIUser::enterProgramMode()
{

	static const DRUIDString command_mode_program(SUI_STRINGS_MODE_PROGRAM);
	clear();

	eot_checks_enabled = false;
	this->send(command_mode_program);

	time_t maxTime = time(NULL) + DRUID4ARDUINO_MAX_RESPONSE_DELAY_SECONDS;
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

#ifdef DRUID4ARDUINO_DEBUG_ENABLE
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
bool SerialUIUser::haveBufferedMessage()
{

	static const size_t eot_str_len = eot_str.length();

	DRUID_DEBUG("Checking for bufferered message...");
	if (incoming_message.size() < 10)
	{
		DRUID_DEBUG2("... too short", incoming_message);

		return false; // no worth my time
	}


	DRUIDString::iterator findIter = std::search(incoming_message.begin(),
			incoming_message.end(), eot_str.begin(), eot_str.end());

	if (findIter != incoming_message.end()) {
		// has EOT, leave it alone...
		DRUID_DEBUG("has EOT.");
		return false;
	}


	DRUID_DEBUG("Have bufferered message!");
	return true;
}

DRUIDString SerialUIUser::getAndClearBufferedMessage()
{
	DRUIDString retStr;
	// last_msg.lock();
	if (haveBufferedMessage())
	{

		retStr = incoming_message;
		incoming_message.clear();
		// last_msg.unlock();
	}
	DRUID_DEBUG2("Returning bufferered message", retStr);
	return retStr;
}

void SerialUIUser::checkForLastMessage()
{

	static const size_t eot_str_len = eot_str.length();


	if (! incoming_message.size())
		return;

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
		checkIfMessageContainsStateTracking();
		checkIfRequiresInput();
		if (requestedTerminate())
			exit(0);

		/*
		DRUIDString new_line("\r\n");

		SerialUserStringList msgList = lastMessageAsList();
		DRUIDString new_last_msg;
		new_last_msg.reserve(last_msg.size());

		for (SerialUserStringList::iterator iter = msgList.begin(); iter !=
				msgList.end(); iter++)
		{
			std::string cleanVal = truncatePromptFrom(*iter);
			if (cleanVal.size())
			{
				new_last_msg.append(cleanVal + new_line);
			}
		}
		if (! new_last_msg.size())
		{
			last_msg.clear();
			return false;
		}

		last_msg.setTo(new_last_msg);
		*/


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


	const std::string regexp_str(DRUIDStdString("^(.*?") + ctrl_strings.prompt_str + ")\\s*$");
	boost::regex prompt_finder_regexp(regexp_str, boost::regex::normal);

	return boost::regex_replace(msg, prompt_finder_regexp,
			std::string(""), boost::match_any | boost::format_all);

}

void SerialUIUser::checkIfMessageContainsStateTracking()
{
	static const DRUIDString trackedStatePrefix_str(SUI_SERIALUI_TRACKEDSTATE_PREFIX_PROG);

	// !~TS~42#i#red# 0#i#green# 0#i#blue# 0#b#enable#0#
	static const DRUIDString trackedStateRegexStr(DRUIDString("^")
			+ SUI_SERIALUI_TRACKEDSTATE_PREFIX_PROG + "(\\d+)(.)(.+)$");
	static  boost::regex trackedStateRegex(trackedStateRegexStr);
	static  boost::regex whitespaceRegex(DRUIDString("\\s+"));


	SerialUserStringList msgList, cleanedMsg, trackedDataList;
	DRUID_DEBUG("checkIfMessageContainsStateTracking()");
	// make a copy, last_message can change asynchronously
	{
		DRUIDString msg(last_msg.get());

		if (! boost::algorithm::find_first(msg, trackedStatePrefix_str))
		{
			// no tracking, fuggetaboudit
			DRUID_DEBUG2("Not found", trackedStatePrefix_str);
			return;
		}
	}

	msgList = lastMessageAsList();

	last_msg.lock();
	DRUID_DEBUG2("Found marker", trackedStatePrefix_str);


	for (SerialUserStringList::iterator iter = msgList.begin(); iter != msgList.end(); iter++)
	{
		
		DRUID_DEBUG2("checking line", *iter);

		boost::smatch what;
		if (! boost::regex_match(*iter, what, trackedStateRegex))
		{

			DRUID_DEBUG("not here");
			cleanedMsg.push_back(*iter);
			continue;
		}

		// gotta match
		trackedDataList.clear();
		uint8_t i=0;
		const DRUIDString stringSeps(DRUIDString("([^") + DRUIDString(what[2]) + "]+)");
		boost::regex stringSepRegex(stringSeps, boost::regex::normal);


		DRUID_DEBUG2("MARKER LINE, splitting with", stringSeps);
		DRUIDString tContents(what[3]);
		boost::regex_split(std::back_inserter(trackedDataList), tContents, stringSepRegex);

		DRUID_DEBUG2("SPLIT INTO ", trackedDataList.size());
		for (uint8_t i=0; i<trackedDataList.size(); i+=3)
		{


			DRUIDString type = trackedDataList[i];
			DRUIDString name = trackedDataList[i+1];
			DRUIDString val = trackedDataList[i+2];

			val = boost::regex_replace(val, whitespaceRegex,
					std::string(""), boost::match_any | boost::format_all);

			DRUID_DEBUG("State tracking:")
			DRUID_DEBUG2("\ttype", type);
			DRUID_DEBUG2("\tname", name);
			DRUID_DEBUG2("\tvalue", val);

			if (! (type.size() && name.size() && val.size()))
			{
				continue;
				// TODO: Output error
			}

			SUIUserNameToTrackedStateVariable::iterator tFindIter = trackedVariablesMap.find(name);
			if (tFindIter == trackedVariablesMap.end())
			{

				// not yet present
				trackedVariablesMap[name].name = name;
				switch (type.at(0))
				{
				case 'b':
					// boolean
					trackedVariablesMap[name].type = SUI::SUITracked_Bool;

					break;
				case 'i':

					trackedVariablesMap[name].type = SUI::SUITracked_UInt;

					break;

				case 'f':

					trackedVariablesMap[name].type = SUI::SUITracked_Float;
					break;
				}
			}

			trackedVariablesMap[name].last_val = val;

			switch (trackedVariablesMap[name].type) {

			case SUI::SUITracked_Bool:
				trackedVariablesMap[name].val_bool = val.at(0) == '1' ? true : false;
				break;
			case SUI::SUITracked_UInt:

				try {
					// get the expected size of the program strings string

					trackedVariablesMap[name].val_int =  boost::lexical_cast<unsigned long>(val);

				} catch (boost::bad_lexical_cast &) {
					DRUID_DEBUG2("Weird cast attempt for tracked int",val);
					trackedVariablesMap[name].val_int = 0;
				}
				break;

			case SUI::SUITracked_Float:

				try {
					// get the expected size of the program strings string

					trackedVariablesMap[name].val_int =  boost::lexical_cast<float>(val);

				} catch (boost::bad_lexical_cast &) {
					DRUID_DEBUG2("Weird cast attempt for tracked float",val);
					trackedVariablesMap[name].val_float = 0;
				}
				break;
			}



		}
		
	}


	last_msg.unlock();
	last_msg.clear();
	last_msg.lock();
	DRUIDString end_line("\r\n");
	for(SerialUserStringList::iterator iter=cleanedMsg.begin(); iter != cleanedMsg.end(); iter++)
	{
		std::string strVal = truncatePromptFrom(*iter);
		if (strVal.size())
		{
			last_msg.directAccess().append(*iter);
			last_msg.directAccess().append(end_line);
		}
	}
	last_msg.unlock();






}

SUIUserIdxToTrackedStateVariablePtr SerialUIUser::updatedTrackedVariables()
{
	SUIUserIdxToTrackedStateVariablePtr retMap;


	for (SUIUserNameToTrackedStateVariable::iterator iter=trackedVariablesMap.begin();
			iter != trackedVariablesMap.end();
			iter++)
	{

		SUIUserTrackedState * tstate = &((*iter).second);
		if ( tstate->last_val != tstate->known_val)
		{

			tstate->known_val = tstate->last_val;

			retMap[tstate->idx] = tstate;
		}
	}

	return retMap;


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

bool SerialUIUser::requestedTerminate()
{

	DRUID_DEBUG("SerialUIUser::requestedTerminate()");


	const DRUIDString regexStr("^(.*)(" + ctrl_strings.terminate_gui +")\\s*$");
	const boost::regex reqTerminateRegex(regexStr, boost::regex::perl);



	SerialUserStringList linesOfInterest = this->lastMessageAsList();
	for (SerialUserStringList::iterator iter = linesOfInterest.begin();
				iter != linesOfInterest.end(); iter++)
	{
		boost::smatch what;
		// DRUID_DEBUG2("checking line", *iter);
		DRUID_DEBUGVERBOSE("checking line:");
		DRUID_DEBUGVERBOSE(*iter);
		if (boost::regex_match(*iter, what, reqTerminateRegex))
		{
			DRUID_DEBUG("got a match...");

			if (what[2].length())
			{
				return true;
			}
		}
	}

	return false;

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
