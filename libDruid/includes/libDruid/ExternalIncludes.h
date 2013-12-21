/*
 * ExternalIncludes.h
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

#ifndef LIBDRUID_EXTERNALINCLUDES_H_
#define LIBDRUID_EXTERNALINCLUDES_H_

#ifndef BOOST_THREAD_USE_LIB
// required for mingw32 linking of thread libs
#define BOOST_THREAD_USE_LIB
#endif 
#include <boost/thread.hpp>

#include <boost/bind.hpp>
#include <boost/asio.hpp>
// include <boost/asio/serial_port.hpp>
#include <boost/lexical_cast.hpp>
// include <boost/date_time/posix_time/posix_time_types.hpp>
#include <iostream>
#include <string>

#include <deque>
#include <vector>



typedef std::string DRUIDStdString;

typedef std::string DRUIDString;


#endif /* EXTERNALINCLUDES_H_ */
