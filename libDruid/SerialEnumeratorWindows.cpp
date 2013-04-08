/*
 * SerialEnumeratorWindows.cpp -- Windows Serial Enumerator implementation,
 * VERY dumb.
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


#include "SerialEnumeratorWindows.h"


#ifdef PLATFORM_WINDOWS

namespace DRUID {

SerialEnumeratorWindows::SerialEnumeratorWindows() : SerialEnumeratorBase() {


}

SerialEnumeratorWindows::~SerialEnumeratorWindows() {

}


bool SerialEnumeratorWindows::portExists(const PortName & pName)
{
	return true;

}
PortNameList SerialEnumeratorWindows::listPorts()
{

	PortNameList retList;
	retList.push_back("COM1");
	retList.push_back("COM2");
	retList.push_back("COM3");
	retList.push_back("COM4");
	return retList;
}

#if 0
// have no clue... doesn't work with MinGW anyway...
#define COMBUFSIZE	7
PortNameList SerialEnumeratorWindows::listPorts()
{


	PortNameList retList;
    char comBufName[7];
    DWORD size;

	for (int i = 1; i < 256; i++)
	{

		// allocate and reset
	    COMMCONFIG CommConfig;
	    size = sizeof CommConfig;


		snprintf(comBufName, COMBUFSIZE, "COM%d", i);
		if (GetDefaultCommConfig( comBufName, &CommConfig, &size)
		           || size > sizeof CommConfig)
		{
			PortName nameStr(comBufName, COMBUFSIZE);
			retList.push_back(nameStr);
		}
	}
	return retList;

}

#endif


} /* namespace DRUID */

#endif
