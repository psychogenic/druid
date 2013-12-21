/*
 * SerialEnumeratorLinux.cpp -- Linux serial enumerator implementation.
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

#include "libDruid/SerialEnumeratorLinux.h"
#include <boost/filesystem.hpp>



#ifdef PLATFORM_LINUX

namespace DRUID {

SerialEnumeratorLinux::SerialEnumeratorLinux() : SerialEnumeratorBase()
{

}

SerialEnumeratorLinux::~SerialEnumeratorLinux()
{

}

PortNameList SerialEnumeratorLinux::listPorts()
{
	PortNameList retList;


	boost::filesystem::path serialPath("/dev/serial");
	if (! boost::filesystem::exists(serialPath))
	{
		return retList;
	}

	boost::filesystem::path serialByIdPath("/dev/serial/by-id");
	if (! boost::filesystem::exists(serialByIdPath))
	{
		return retList;
	}
	try {

		if (boost::filesystem::is_directory(serialByIdPath)) {
			for (boost::filesystem::directory_iterator iter =
					boost::filesystem::directory_iterator(serialByIdPath);
					iter != boost::filesystem::directory_iterator(); iter++) {
				boost::filesystem::directory_entry ent = (*iter);


				if (boost::filesystem::is_symlink(ent.symlink_status())) {
					boost::filesystem::path loc =
							boost::filesystem::read_symlink(ent.path());


					boost::filesystem::path fullPath = serialByIdPath/loc;
					fullPath.normalize();
					PortName pName(fullPath.string());
					retList.push_back(pName);
				}
			}
		}
	} catch (const boost::filesystem::filesystem_error& ex) {
		std::cerr << ex.what() << std::endl;

	}

	return retList;

}

} /* namespace DRUID */


#endif
