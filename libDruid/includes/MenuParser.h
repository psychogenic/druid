/*
 * MenuParser.h - crawl a SerialUI menu to build a representation
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

#ifndef MENUPARSER_H_
#define MENUPARSER_H_

#include "ExternalIncludes.h"
#include "SerialUser.h"
#include "Menu.h"
#include "SerialUIControlStrings.h"

namespace DRUID {


class MenuParser {
public:
	MenuParser();
	virtual ~MenuParser();

	void setControlStrings(SerialUIControlStrings ctrl);

	MenuPtr crawl(SerialUserPtr serialUser);

	void dumpMenus();


private:


	void dumpMenu(MenuPtr menu, unsigned int depth=0);

	bool parse(SerialUserPtr user, MenuPtr menu);

	MenuPtr top_level_menu;

	SerialUIControlStrings ctrl_strings;

	static const DRUIDString command_mode_program;
	static const DRUIDString command_mode_user;

	static const DRUIDString command_help;
	static const DRUIDString command_up;
	static const DRUIDString command_exit;

	static const DRUIDString prefix_command;
	static const DRUIDString prefix_submenu;
	static const DRUIDString help_sep;


};

} /* namespace DRUID */
#endif /* MENUPARSER_H_ */
