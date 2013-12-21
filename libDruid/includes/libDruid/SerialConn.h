/*
 * SerialConn.h -- low level serial connection
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

#ifndef SERIALCONN_H_
#define SERIALCONN_H_

#include "libDruid/ExternalIncludes.h"
#include "libDruid/SerialListener.h"

#define DRUID_SERIALCONN_MAX_READ_LEN	512
namespace DRUID {

typedef boost::asio::serial_port	boost_serial;

class SerialConn {
public:
	// SerialConn();

	SerialConn(SerialListener * listener,
			boost::asio::io_service& io_service, unsigned int baud, const DRUIDString& device);
	virtual ~SerialConn();

	void send(const DRUIDString & aString);

	void write(const char msg);

	void close();

	bool isOnline() ;

private:

	void read_begin(void);
	void read_done(const boost::system::error_code& error, size_t bytes_transferred);

	void do_write_string(const DRUIDString & msg);

	void do_write(const char msg);
	void write_begin(void);
	void write_done(const boost::system::error_code& error);

	void do_close(const boost::system::error_code& error);


	void setOptions( unsigned int baudrate, 
		boost_serial::flow_control::type flowtype = boost_serial::flow_control::none, 
		boost_serial::parity::type partype = boost_serial::parity::none,
 
		boost_serial::stop_bits::type stoptype = boost_serial::stop_bits::one,
		unsigned int char_size = 8);


	boost::asio::serial_port * port() { return &serialPort;} // the serial port this instance is connected to
private:
	bool online; // remains true while this object is still operating
	boost::asio::io_service& sconn_ioserv; // the main IO service that runs this connection
	boost::asio::serial_port serialPort; // the serial port this instance is connected to
	char msg_inbuffer[DRUID_SERIALCONN_MAX_READ_LEN]; // data read from the socket
	std::deque<char> msgs_outbound; // buffered write data
	SerialListener * listener;

};

} /* namespace DRUID */
#endif /* SERIALCONN_H_ */
