/*
 * Util.h -- basic utilities and SerialConn/boost-stuff package.
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

#ifndef UTIL_H_
#define UTIL_H_

#include "ExternalIncludes.h"
#include <boost/shared_ptr.hpp>
#include <string>

#include "SerialGUIConfig.h"
#include "ExternalIncludes.h"
#include "SerialUIUser.h"

namespace DRUID {

class UtilConnectionPackage
{
public:
	UtilConnectionPackage(unsigned int baud_rate, const DRUIDString & device);
	~UtilConnectionPackage();
	void destroy();


	bool ping(long maxDelaySeconds=2);
	bool active() { return is_active;}
	SerialUIUserPtr serialUser() { return serial_user;}

private:
    bool is_active;
    boost::asio::io_service * io_service;
    boost::thread * serial_thread;
    SerialUIUserPtr serial_user;

    void destroyIOService();

};

typedef boost::shared_ptr<UtilConnectionPackage> ConnectionPackagePtr;

class Util {
public:
	Util();
	virtual ~Util();

	/* DEADBEEF
	static std::string wideToString(const std::wstring & aWstr);
	static std::wstring stringToWide(const std::string & aStr);
	*/

	static ConnectionPackagePtr	 getConnection(unsigned int baud_rate, const DRUIDString & device);

};

} /* namespace DRUID */
#endif /* UTIL_H_ */
