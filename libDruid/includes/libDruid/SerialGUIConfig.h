/*
 * SerialGUIConfig.h  -- configs and defines
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

#ifndef DRUID4ARDUINOCONFIG_H_
#define DRUID4ARDUINOCONFIG_H_

#include "libDruid/ExternalIncludes.h"

//
// #define DRUID4ARDUINO_DEBUG_ENABLE
// #undef DRUID4ARDUINO_DEBUG_ENABLE

#define DRUID4ARDUINO_VERSION_MAJOR			1
#define DRUID4ARDUINO_VERSION_MINOR			2
#define DRUID4ARDUINO_VERSION_PATCHLEVEL	5

#define druidstr(s) #s
#define xdruidstr(s) druidstr(s)
#define DRUID4ARDUINO_VERSION_STRING()		 xdruidstr(DRUID4ARDUINO_VERSION_MAJOR) "." xdruidstr(DRUID4ARDUINO_VERSION_MINOR) "." xdruidstr(DRUID4ARDUINO_VERSION_PATCHLEVEL)


#define DRUID4ARDUINO_MAX_RESPONSE_DELAY_SECONDS		6

#define DRUID4ARDUINO_ERROR_SENDANDRCV_TIMEOUT			"Timeout awaiting response"


#if (defined(__GNUWIN32__) || defined(__MINGW32_MAJOR_VERSION))
#	ifndef PLATFORM_WINDOWS
#		define PLATFORM_WINDOWS
#	endif
#endif


#if  !(defined(PLATFORM_LINUX) || defined(PLATFORM_MAC) || defined(PLATFORM_WINDOWS))
#	error "You MUST define the platform for which we are building using PLATFORM_{LINUX,MAC,WINDOWS}"
#endif

#ifdef DRUID4ARDUINO_DEBUG_ENABLE
#include <iostream>
#define DRUID_DEBUG(msg) std::cerr << msg << std::endl;
#define DRUID_DEBUG2(msg1, msg2) std::cerr << msg1 << ":" << msg2 << std::endl;
#define DRUID_DEBUGVERBOSE(msg) std::cerr << msg;

#else
#define DRUID_DEBUG(msg) ;
#define DRUID_DEBUG2(msg1, msg2) ;
#define DRUID_DEBUGVERBOSE(msg) ;

#endif


#ifdef PLATFORM_LINUX
#	define PLATFORM_SLEEP(amount)		sleep(amount)
#else
#	ifdef PLATFORM_WINDOWS
#		define PLATFORM_SLEEP(amount)		usleep((amount * 1000000))
#	else
#		define PLATFORM_SLEEP(amount)		sleep(amount)
#	endif
#endif


#define DRUID4ARDUINO_SITE_URL "http://flyingcarsandstuff.com/projects/druid4arduino/"
#define DRUID4ARDUINO_LATESTVERSION_URL		"http://flyingcarsandstuff.com/downloads/druid-latest.txt"

#ifdef PLATFORM_LINUX
#define DRUID4ARDUINO_COUNTER_URL		"http://flyingcarsandstuff.com/projects/SerialGUI/counter?os=Linux"
#endif
#ifdef PLATFORM_MAC
#define  DRUID4ARDUINO_COUNTER_URL		"http://flyingcarsandstuff.com/projects/SerialGUI/counter?os=Mac"
#endif
#ifdef PLATFORM_WINDOWS
#define  DRUID4ARDUINO_COUNTER_URL		"http://flyingcarsandstuff.com/projects/SerialGUI/counter?os=Win&rid="
#endif


#endif /* DRUID4ARDUINOCONFIG_H_ */
