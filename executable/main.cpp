/*
 * main.cpp -- driver implementation.
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


#include <libDruid/SerialDruid.h>

#ifdef POSIX
#include <termios.h>
#include <unistd.h>
#endif
#include "App.h"

#ifdef POSIX
termios stored_settings;
tcgetattr(0, &stored_settings);

termios new_settings = stored_settings;
new_settings.c_lflag &= (~ICANON);
tcsetattr(0, TCSANOW, &new_settings);
#endif


IMPLEMENT_APP(DRUID::App)


// tcsetattr(0, TCSANOW, &stored_settings);

