/*
 * SerialConn.cpp -- SerialConn implementation.
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
#include "libDruid/SerialGUIConfig.h"
#include "libDruid/SerialConn.h"

#include <stdlib.h>
#include <unistd.h>

namespace DRUID {


SerialConn::SerialConn(SerialListener * aListener,
		boost::asio::io_service& io_service, unsigned int baud,
		const DRUIDString& device) :
		online(false), sconn_ioserv(io_service),
		serialPort(io_service, device),
		listener(aListener)
{
#ifdef PLATFORM_WINDOWS
	PLATFORM_SLEEP(0.5);
#endif

	if (! serialPort.is_open()) {
		DRUID_DEBUG("SerialConn: could not OPEN serial port.");

		std::stringstream errStr;
		errStr  << "Could not open serial port:" << device << std::endl;
		DRUIDString errorMessage = errStr.str();
		listener->serialError(errorMessage);
		return;
	}


	setOptions(baud);

	online = true;
	read_begin();
}


void SerialConn::setOptions( unsigned int baudrate, boost_serial::flow_control::type flowtype, 
	boost_serial::parity::type partype, 
	boost_serial::stop_bits::type stoptype,
	unsigned int char_size)
{
	boost_serial::baud_rate baud_option(baudrate);
	serialPort.set_option(baud_option);

	serialPort.set_option(boost_serial::flow_control(flowtype));
	serialPort.set_option(boost_serial::parity(partype));
	serialPort.set_option(boost_serial::stop_bits(stoptype));
	serialPort.set_option(boost_serial::character_size(char_size));

}

SerialConn::~SerialConn()
{
	if (online)
		close();


}
void SerialConn::write(const char achar)
{
	if (online) {
		sconn_ioserv.post(boost::bind(&SerialConn::do_write, this, achar));
	}
}

void SerialConn::send(const DRUIDString & aString)
{
	if (online)
	{
		sconn_ioserv.post(boost::bind(&SerialConn::do_write_string, this, aString));
	}

	return;
}

void SerialConn::send(uint8_t * rawBytesBuffer, size_t len)
{
	if (online)
	{
		sconn_ioserv.post(boost::bind(&SerialConn::do_write_raw, this, rawBytesBuffer, len));
	}

	return;
}


void SerialConn::close()
{
	DRUID_DEBUG("CLOSING");
	if (! sconn_ioserv.stopped())
	{
		sconn_ioserv.post(
			boost::bind(&SerialConn::do_close, this,
					boost::system::error_code()));

		usleep(10000);
		sconn_ioserv.stop();
		online = false;
	}
}

bool SerialConn::isOnline() // return true if the socket is still active
{
	return online;
}


void SerialConn::read_begin(void) {
	serialPort.async_read_some(boost::asio::buffer(msg_inbuffer, DRUID_SERIALCONN_MAX_READ_LEN),
			boost::bind(&SerialConn::read_done, this,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
}

void SerialConn::read_done(const boost::system::error_code& error,
		size_t bytes_transferred) { // the asynchronous read operation has now completed or failed and returned an error
	if (error) {
		do_close(error);
	} else {
		// read completed, so process the data
		// sconn_ioserv.post(boost::bind(&SerialListener::serialReceived, listener, msg_inbuffer, bytes_transferred));

		listener->serialReceived(msg_inbuffer, bytes_transferred);
		read_begin(); // start waiting for another asynchronous read again
	}

}

void SerialConn::do_write(const char msg) { // callback to handle write call from outside this class
	bool was_empty = msgs_outbound.empty(); // is there anything currently being written?
	msgs_outbound.push_back(msg); // store in write buffer
	if (was_empty) // if nothing is currently being written, then start
		write_begin();
}


void SerialConn::do_write_raw(uint8_t * rawByteBuffer, size_t len)
{
	bool was_empty = msgs_outbound.empty();
	for (size_t i=0; i <len; i++)
	{

		msgs_outbound.push_back(rawByteBuffer[i]);
		usleep(200);
	}

	if (was_empty)
		write_begin();

}

void SerialConn::do_write_string(const DRUIDString & msg)
{
	bool was_empty = msgs_outbound.empty();
	for (size_t i=0; i < msg.length(); i++)
	{
		DRUID_DEBUGVERBOSE('X');
		DRUID_DEBUGVERBOSE(msg[i]);
		msgs_outbound.push_back((uint8_t)(msg.at(i)));
		usleep(200);
	}
	if (was_empty)
		write_begin();
}


void SerialConn::write_begin(void) { // Start an asynchronous write and call write_done when it completes or fails

	//
	if (! msgs_outbound.empty())
	{

		DRUID_DEBUGVERBOSE(msgs_outbound.front());
		boost::asio::async_write(serialPort,
			boost::asio::buffer(&msgs_outbound.front(), 1),
			boost::bind(&SerialConn::write_done, this,
					boost::asio::placeholders::error));

	}

}

void SerialConn::write_done(const boost::system::error_code& error) { // the asynchronous read operation has now completed or failed and returned an error

	// DRUID_DEBUGVERBOSE("\nWrite_done\n");
	if (error) {
		do_close(error);
	} else {
		// write completed, so send next write data
		msgs_outbound.pop_front(); // remove the completed data
		if (!msgs_outbound.empty())
			write_begin();
	}

}

void SerialConn::do_close(const boost::system::error_code& error) { // something has gone wrong, so close the socket & make this object inactive

	DRUID_DEBUG("DO_CLOSE()");
	if (error == boost::asio::error::operation_aborted) // if this call is the result of a timer cancel()
		return; // ignore it because the connection cancelled the timer

	/*
	std::stringstream errStream;
	if (error)
		errStream << "Error: " << error.message() << std::endl; // show the error message
	else
		errStream << "Error: Connection did not succeed.\n";


	listener->serialError(errStream.str());
	*/

	try {
		serialPort.close();
	} catch (std::exception & e)
	{
		std::cerr << "Exception caught while closing serial port: " << e.what() << "\n";
	}

	online = false;
}

} /* namespace DRUID */
