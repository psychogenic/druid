/*
 * Util.cpp -- Util and connection package implementation.
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
#include <unistd.h>

#include "libDruid/SerialGUIConfig.h"
#include "libDruid/Util.h"

namespace DRUID {



UtilConnectionPackage::UtilConnectionPackage(unsigned int baud_rate,
		const DRUIDString& device) :
		is_active(false),
		io_service(NULL),
		serial_thread(NULL)
{

	static DRUID::SerialUIUserPtr nullSUserPtr;


	// create new service/serial user/thread
	io_service = new boost::asio::io_service();
	if (! io_service)
		return;


	serial_user = SerialUIUserPtr(new DRUID::SerialUIUser());

	if (! serial_user)
	{
		destroyIOService();
		return;
	}

	try {

		serial_user->connect(*io_service, baud_rate, device);

	} catch (std::exception & e)
	{
		std::cerr << "Exception caught while connecting to serial port: " << e.what() << "\n";


		destroyIOService();

		DRUID::SerialUIUserPtr emptyPtr;

		serial_user = emptyPtr;

		return;
	}

	serial_thread = new boost::thread(boost::bind(&boost::asio::io_service::run, io_service));

	if (! serial_thread)
	{

		destroyIOService();
		serial_user = nullSUserPtr;

	}

	// phew, made it!

	is_active = true;

#ifdef PLATFORM_WINDOWS
	PLATFORM_SLEEP(0.5);
#endif


}

UtilConnectionPackage::~UtilConnectionPackage() {
	if (is_active)
		destroy();

}


void UtilConnectionPackage::destroyIOService()
{
	if (!io_service)
		return;

	if (!io_service->stopped()) {
		io_service->stop();
	}

	delete io_service;
	io_service = NULL;
}

void UtilConnectionPackage::destroy() {

	static DRUID::SerialUIUserPtr nullSerUserPtr;

	is_active = false;



	// stop the service
	if (io_service) {
		if (! io_service->stopped())
		{
			io_service->stop();
		}
	}

	if (serial_user)
	{
		serial_user->disconnect();
	}


	// join that thread and kill it
	if (serial_thread) {
		// sleep(1);
		serial_thread->join();
		// delete serial_thread;
		serial_thread = NULL;
	}


#ifdef NOTSUREIFINEEDTODOTHISORWHAT

	// kill the service
	if (io_service) {
		delete io_service;
		io_service = NULL;
	}
#endif

	serial_user = nullSerUserPtr;

}

bool UtilConnectionPackage::ping(long maxDelaySeconds)
{
	if (! is_active)
		return false;

	time_t max_time = time(NULL) + maxDelaySeconds;
	size_t bufSize = serial_user->incomingBufferSize();
	serial_user->send('\r');
	serial_user->send('\n');

	bool waitingForResp = true;
	do {
		usleep(100000);
		if (serial_user->incomingBufferSize() != bufSize)
		{
			waitingForResp = false;
		}
		DRUID_DEBUGVERBOSE('.');
	} while (waitingForResp && (time(NULL) < max_time));

	DRUID_DEBUGVERBOSE('\n');

	return !waitingForResp;




}

ConnectionPackagePtr Util::getConnection(unsigned int baud_rate,
		const DRUIDString& device) {
	static ConnectionPackagePtr emptyPtr;

	ConnectionPackagePtr conn_package(new UtilConnectionPackage(baud_rate, device));

	if (! (conn_package && conn_package->active()) )
	{
		return emptyPtr;
	}

	return conn_package;
}


Util::Util() {


}

Util::~Util() {

}


} /* namespace DRUID */
