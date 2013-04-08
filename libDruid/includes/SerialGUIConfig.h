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

#ifndef SERIALGUICONFIG_H_
#define SERIALGUICONFIG_H_

#include "ExternalIncludes.h"

// #define SERIALGUI_DEBUG_ENABLE
#undef SERIALGUI_DEBUG_ENABLE


#define SERIALGUI_MAX_RESPONSE_DELAY_SECONDS		6

#define SERIALGUI_ERROR_SENDANDRCV_TIMEOUT			"Timeout awaiting response"


#if (defined(__GNUWIN32__) || defined(__MINGW32_MAJOR_VERSION))
#	ifndef PLATFORM_WINDOWS
#		define PLATFORM_WINDOWS
#	endif
#endif


#if  !(defined(PLATFORM_LINUX) || defined(PLATFORM_MAC) || defined(PLATFORM_WINDOWS))
#	error "You MUST define the platform for which we are building using PLATFORM_{LINUX,MAC,WINDOWS}"
#endif

#ifdef SERIALGUI_DEBUG_ENABLE
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

#ifdef PLATFORM_LINUX
#define SERIALGUI_COUNTER_URL		"http://flyingcarsandstuff.com/projects/SerialGUI/counter?os=Linux"
#endif
#ifdef PLATFORM_MAC
#define  SERIALGUI_COUNTER_URL		"http://flyingcarsandstuff.com/projects/SerialGUI/counter?os=Mac"
#endif
#ifdef PLATFORM_WINDOWS
#define  SERIALGUI_COUNTER_URL		"http://flyingcarsandstuff.com/projects/SerialGUI/counter?os=Win&rid="
#endif


#endif /* SERIALGUICONFIG_H_ */
