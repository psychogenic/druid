/*
 * SerialUser.h -- friendlier use of SerialConn.
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

#ifndef SERIALUSER_H_
#define SERIALUSER_H_

#include <vector>
#include <string>


#include "libDruid/ExternalIncludes.h"

#include "libDruid/SerialGUIConfig.h"
#include "libDruid/SerialListener.h"
#include "libDruid/SerialConn.h"

#include <boost/shared_ptr.hpp>

namespace DRUID {

typedef std::vector<DRUIDString>	SerialUserStringList;


class SerialUser: public SerialListener {
public:

	SerialUser();
	SerialUser(boost::asio::io_service& io_service, unsigned int baud,
			const DRUIDString& device);
	virtual ~SerialUser();



	/* context-dependent interface */
	virtual bool messageReceived() = 0;
	virtual void lastMessageClear() = 0;

	// boost::python doesn't like references to internals,
	// so we'll use
	// lastMessageRef here, and have a lastMessage() that
	// returns a copy, for export to the module
	virtual DRUIDString lastMessage() = 0;
	virtual const DRUIDString & lastMessageRef() = 0;
	SerialUserStringList lastMessageAsList();




	bool sendAndReceive(const DRUIDString & sendMessage, bool addTerminator=true, time_t timeoutSeconds=SERIALGUI_MAX_RESPONSE_DELAY_SECONDS);


	/* SerialListener interface */
	virtual void serialError(const DRUIDString & errMsg);
	virtual void serialReceived(char * buffer, size_t bytes_transferred);
	virtual void flushReceiveBuffer() = 0;

	/* control of serial conn */
	bool connect(boost::asio::io_service& io_service, unsigned int baud,
			const DRUIDString& device);
	bool isConnected();
	void disconnect();

	bool send(const DRUIDString & aString, bool addTerminator=true);
	bool send(const char aChar);
	bool send(uint8_t * buffer, size_t len);


	/* error reporting */

	bool hasError() { return has_error;}
	const DRUIDString & errorMessage() { return err_message;}
	void clearError() { has_error = false;}


	void clear(); // clear everything


private:

	bool awaiting_response;
	SerialConn * serial_connection;
	bool has_error;
	DRUIDString err_message;
};

typedef boost::shared_ptr<SerialUser> SerialUserPtr;

} /* namespace DRUID */
#endif /* SERIALUSER_H_ */
