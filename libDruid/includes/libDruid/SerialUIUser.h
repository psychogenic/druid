/*
 * SerialUIUser.h -- higher level interface to the serial connection,
 * makes assumptions based on the fact we are using a SerialUI-enabled
 * device.
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

#ifndef SERIALUIUSER_H_
#define SERIALUIUSER_H_

#include "libDruid/SerialUser.h"
#include "libDruid/SerialUIControlStrings.h"

#include <boost/thread/mutex.hpp>

namespace DRUID {

typedef enum SerialUIUserInputTypeEnum {
	InputType_None=0,
	InputType_String,
	InputType_Numeric,
	InputType_Integer,
	InputType_Stream,
	InputType_Date
} UserInputType ;

typedef struct LockedLastMessageStruct {

	void setTo(const DRUIDString & val);
	DRUIDString get();
	void clear();

	inline void lock() { message_mutex.lock();}
	inline void unlock() { message_mutex.unlock();}
	inline DRUIDString & directAccess() { return last_message;}
	inline size_t size() { return last_message.size();}


private:
	DRUIDString last_message;
	boost::mutex message_mutex;
} LockedLastMessage;

class SerialUIUser: public SerialUser {
public:
	SerialUIUser();
	SerialUIUser(boost::asio::io_service& io_service, unsigned int baud,
				const DRUIDString& device);
	virtual ~SerialUIUser();

	bool autoReplaceLastMessage() { return auto_replace_last_message;}
	void setAutoReplaceLastMessage(bool setTo) { auto_replace_last_message = setTo;}


	bool eotChecks() { return eot_checks_enabled;}
	void setEotChecks(bool setTo) { eot_checks_enabled = setTo;}

	virtual bool messageReceived();
	virtual void lastMessageClear();

	// a ref and a copy version, see SerialUser.h but in short: boost::python.
	virtual DRUIDString lastMessage();
	virtual const DRUIDString & lastMessageRef();


	void checkForLastMessage();
	size_t incomingBufferSize() { return incoming_message.size(); }
	const DRUIDString & incomingBuffer() { return incoming_message;}



	virtual void serialReceived(char * buffer, size_t bytes_transferred);
	virtual void flushReceiveBuffer();

	bool inputRequired(bool forceCheck=false) ;
	UserInputType inputRequiredType() { return required_input; }
	const DRUIDString & inputRequiredPromptString() { return required_input_prompt; }

	bool upMenuLevel();

	const DRUIDString & endOfTransmissionString() { return eot_str; }
	void setEndOfTransmissionString(const DRUIDString  & str) { eot_str = str;}

	SerialUIControlStrings enterProgramMode();
	SerialUIControlStrings setupProgModeStrings(DRUIDString & progRetStr);


	bool requestedTerminate();
	bool haveBufferedMessage();
	DRUIDString getAndClearBufferedMessage();

private:

	void checkIfMessageWasError();
	void checkIfRequiresInput();


	std::string truncatePromptFrom(std::string & msg);


	UserInputType required_input;
	DRUIDString required_input_prompt;

	bool awaiting_response;
	bool message_rcvd;
	size_t last_rcvdcheck_len;
	DRUIDString incoming_message;
	bool eot_checks_enabled;
	bool auto_replace_last_message;
	DRUIDString eot_str;


	LockedLastMessage last_msg;
	SerialUIControlStrings ctrl_strings;

};

typedef boost::shared_ptr<SerialUIUser> SerialUIUserPtr;

} /* namespace DRUID */
#endif /* SERIALUIUSER_H_ */
