/*
 * App.cpp -- App implementation.
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
#include "MainWindow.h"
#include "App.h"
#include "Config.h"

namespace DRUID {

#ifdef PLATFORM_LINUX
#define DEF_WIN_WIDTH	680
#define DEF_WIN_HEIGHT	550
#else
#define DEF_WIN_WIDTH	680
#define DEF_WIN_HEIGHT	610
#endif

bool App::OnInit()
{



	srand(time(0));


	MainWindow *frame = new MainWindow(_T(DRUID4ARDUINO_APP_NAME),  wxPoint(100, 75), wxSize(DEF_WIN_WIDTH, DEF_WIN_HEIGHT) );
    frame->Show( true );
    SetTopWindow( frame );

    return true;
}


} /* namespace DRUID */
