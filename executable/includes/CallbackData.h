/*
 * CallbackData.h -- class defs for our callback user data.
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

#ifndef CALLBACKDATA_H_
#define CALLBACKDATA_H_


#include <libDruid/SerialDruid.h>
#include "Widgets.h"

namespace DRUID {


class ButtonClickUserData : public wxObject {

public:
	ButtonClickUserData(DRUID::MenuItemPtr aMenuItem) : wxObject() { menu_item = aMenuItem;}

	DRUID::MenuItemPtr menu_item;
};

class UpLevelClickUserData: public wxObject {
public:
	UpLevelClickUserData(DRUID::MenuPtr aMenu) : wxObject() { cur_menu = aMenu;}

	DRUID::MenuPtr cur_menu;
};


class MenuItemClickUserData: public wxObject {
public:
	MenuItemClickUserData(DRUID::MenuPtr aMenu,
			DRUID::MenuItemPtr aMenuItem) : wxObject() { in_menu = aMenu; menu_item = aMenuItem; }

	DRUID::MenuPtr in_menu;
	DRUID::MenuItemPtr menu_item;
};


} /* namespace DRUID */


#endif /* CALLBACKDATA_H_ */
