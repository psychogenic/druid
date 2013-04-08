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

#include "SerialUser.h"
#include "SerialUIControlStrings.h"


namespace DRUID {

typedef enum SerialUIUserInputTypeEnum {
	InputType_None=0,
	InputType_String,
	InputType_Numeric,
	InputType_Integer,
	InputType_Date
} UserInputType ;


class SerialUIUser: public SerialUser {
public:
	SerialUIUser();
	SerialUIUser(boost::asio::io_service& io_service, unsigned int baud,
				const DRUIDString& device);
	virtual ~SerialUIUser();


	virtual bool messageReceived();
	virtual void lastMessageClear();

	// a ref and a copy version, see SerialUser.h but in short: boost::python.
	virtual DRUIDString lastMessage();
	virtual const DRUIDString & lastMessageRef();


	size_t incomingBufferSize() { return incoming_message.size(); }
	const DRUIDString & incomingBuffer() { return incoming_message;}



	virtual void serialReceived(char * buffer, size_t bytes_transferred);
	virtual void flushReceiveBuffer();

	bool inputRequired() { return required_input != InputType_None;}
	UserInputType inputRequiredType() { return required_input; }

	bool upMenuLevel();

	const DRUIDString & endOfTransmissionString() { return eot_str; }
	void setEndOfTransmissionString(const DRUIDString  & str) { eot_str = str;}

	SerialUIControlStrings enterProgramMode();
	SerialUIControlStrings setupProgModeStrings(DRUIDString & progRetStr);


private:

	void checkIfMessageWasError();
	void checkIfRequiresInput();
	std::string truncatePromptFrom(std::string & msg);


	UserInputType required_input;
	bool awaiting_response;
	bool message_rcvd;
	size_t last_rcvdcheck_len;
	DRUIDString incoming_message;
	DRUIDString last_message;
	bool eot_checks_enabled;
	DRUIDString eot_str;
	SerialUIControlStrings ctrl_strings;

};

typedef boost::shared_ptr<SerialUIUser> SerialUIUserPtr;

} /* namespace DRUID */
#endif /* SERIALUIUSER_H_ */
