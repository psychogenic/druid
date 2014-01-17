/*
 * SerialUser.cpp -- SerialUser implementation.
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
#include <inttypes.h>
#include <iostream>
#include <boost/regex.hpp>
#include <unistd.h>

#include "libDruid/SerialUser.h"
#include "libDruid/SerialGUIConfig.h"

namespace DRUID {



SerialUser::SerialUser() : SerialListener(),
		awaiting_response(false),
		serial_connection(NULL),
		has_error(false),
		err_message("")
{

}

SerialUser::SerialUser(boost::asio::io_service& io_service, unsigned int baud,
		const DRUIDString& device) : SerialListener(),
		awaiting_response(false),
		serial_connection(NULL),
		has_error(false),
		err_message("")
{
	this->connect(io_service, baud, device);
}

SerialUser::~SerialUser() {

	disconnect();

}

void SerialUser::disconnect()
{
	SerialConn * ser_con = serial_connection;
	serial_connection = NULL;


	if (ser_con) {
		if (ser_con->isOnline()) {
			ser_con->close();
		}

		delete ser_con;
	}

}


bool SerialUser::connect(boost::asio::io_service& io_service, unsigned int baud,
		const DRUIDString& device)
{
	static const DRUIDString errCouldNotCreate("Could not create SerialConn object!");

	has_error = false;

	if (serial_connection)
		disconnect();

	serial_connection = new SerialConn(this, io_service, baud, device);

	if (! serial_connection)
	{
		serialError(errCouldNotCreate);
		return false;
	}

	if (has_error)
		return false;


	return true;
}

bool SerialUser::isConnected()
{
	if (! serial_connection)
		return false;

	return serial_connection->isOnline();

}

void SerialUser::serialError(const DRUIDString & errMsg)
{
	has_error = true;
	err_message = errMsg;
	std::cerr << "ERROR MESSAGE:" << err_message;

}

void SerialUser::serialReceived(char * buffer, size_t bytes_transferred)
{
	DRUID_DEBUG("SerialUser::serialReceived()");
	std::cout.write(buffer, bytes_transferred);

}

bool SerialUser::send(const DRUIDString& aString, bool addTerminator) {
	static const char stringTerminator = '\n';
	static const DRUIDString errNoConn("No serial connection, connect() first!");

	if (! serial_connection)
	{
		serialError(errNoConn);
		return false;
	}

	DRUID_DEBUG2("Sending string to serial", aString);
	serial_connection->send(aString);

	if (addTerminator)
	{
		DRUID_DEBUG("Adding terminator");

		serial_connection->write(stringTerminator);
	}

	return true;
}


bool SerialUser::send(uint8_t * buffer, size_t len) {

	static const DRUIDString errNoConn("No serial connection, connect() first!");

	if (! serial_connection)
	{
		serialError(errNoConn);
		return false;
	}

	DRUID_DEBUG2("Sending raw buffer to serial, len ", len);
	serial_connection->send(buffer, len);

	return true;
}
void SerialUser::clear()
{
	clearError();
	flushReceiveBuffer();
	lastMessageClear();

}
bool SerialUser::sendAndReceive(const DRUIDString& sendMessage, bool addTerminator, time_t timeoutSeconds) {
	static const DRUIDString timeoutError(DRUID4ARDUINO_ERROR_SENDANDRCV_TIMEOUT);

	clear();

	awaiting_response = true;
	this->send(sendMessage, addTerminator);
	time_t start_time = time(NULL);

	bool haveResponse = false;
	do {
		haveResponse = this->messageReceived();
		usleep(200);
	} while ( (!haveResponse) && (!has_error) &&
			((time(NULL) - start_time) < timeoutSeconds));

	awaiting_response = false;
	if (has_error)
		return false;

	if (! haveResponse)
	{
		serialError(timeoutError);
	}

	DRUID_DEBUG2("Message received from serial", this->lastMessageRef());

	return true;




}

bool SerialUser::send(const char aChar) {
	static const DRUIDString errNoConn("No serial connection, connect() first!");

	if (! serial_connection)
	{
		serialError(errNoConn);
		return false;
	}

	serial_connection->write(aChar);

	return true;

}

SerialUserStringList SerialUser::lastMessageAsList()
{
	const DRUIDString stringSeps("([^\\r\\n]+)");
	boost::regex stringSepRegex(stringSeps, boost::regex::normal);
	SerialUserStringList retList;

	DRUID_DEBUG("SerialUser::lastMessageAsList()");

	// in fast applications, access the lastMessageRef can cause
	// race conditions... just make a copy right away, even if it
	// costs us a bit of init time for each access.
	DRUIDString opStr(this->lastMessage());
	if (! opStr.length())
	{

		DRUID_DEBUG("lastMessage is empty!");
		return retList;


	}

	boost::regex_split(std::back_inserter(retList), opStr, stringSepRegex);

	DRUID_DEBUG2("lastMessage has #lines:", retList.size());

	return retList;

}

} /* namespace DRUID */
