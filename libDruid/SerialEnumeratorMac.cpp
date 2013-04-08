/*
 * SerialEnumeratorMac.cpp -- Mac serial enum implementation. UNTESTED.
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

#include "SerialEnumeratorMac.h"
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#ifdef PLATFORM_MAC
namespace DRUID {

SerialEnumeratorMac::SerialEnumeratorMac() : SerialEnumeratorBase() {


}

SerialEnumeratorMac::~SerialEnumeratorMac() {

}

PortNameList SerialEnumeratorMac::listPorts()
{

	PortNameList retList;


	boost::filesystem::path devPath("/dev");
	if (! boost::filesystem::exists(devPath))
	{
		return retList;
	}

	std::string cu("cu.");
	std::string tty("tty.");

	try {

		if (boost::filesystem::is_directory(devPath)) {
			for (boost::filesystem::directory_iterator iter =
					boost::filesystem::directory_iterator(devPath);
					iter != boost::filesystem::directory_iterator(); iter++) {
				boost::filesystem::directory_entry ent = (*iter);

				boost::filesystem::path fullPath = ent.path();

				std::string fName(fullPath.filename().string());

				if (boost::starts_with(fName, cu) || boost::starts_with(fName, tty))
				{
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
