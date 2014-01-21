/*
 * Config.h -- A few global config defs.
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

#ifndef CONFIG_H_
#define CONFIG_H_

//include <SerialDruid.h>


#define DRUID4ARDUINO_VERSION			1
#define DRUID4ARDUINO_SUBVERSION		2

#define DRUID4ARDUINO_APP_NAME				"Druid4Arduino"
#define DRUID4ARDUINO_CONFIG_BAUDRATE				"/Settings/BaudRate"
#define DRUID4ARDUINO_CONFIG_SERIALPORT				"/Settings/SerialPort"
#define DRUID4ARDUINO_CONFIG_UPLOADDELAYFACTOR		"/Settings/UploadDelayRate"
#define DRUID4ARDUINO_CONFIG_LASTUPLOADFILEPATH		"/History/LastUpload"


#ifdef DRUID4ARDUINO_DEBUG_ENABLE
#include <iostream>
#define DRUID4ARDUINO_DEBUG(msg)			std::cerr << msg << std::endl;
#define DRUID4ARDUINO_DEBUG2(msg, msg2)		std::cerr << msg << ":" << msg2 << std::endl;
#else
#define DRUID4ARDUINO_DEBUG(msg)			;
#define DRUID4ARDUINO_DEBUG2(msg, msg2)		;
#endif


#define DRUID_STDSTRING_TOWX(str)		wxString(str.c_str(), wxConvUTF8)


#endif /* CONFIG_H_ */
