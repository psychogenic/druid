/*
 * MenuParser.cpp -- MenuParser implementation.
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


#include "libDruid/SerialGUIConfig.h"
#include "libDruid/MenuParser.h"

#include <time.h>
#include <boost/regex.hpp>
#include <SUIConfig.h>
#include <SUIStrings.h>

namespace DRUID {


const DRUIDString MenuParser::command_mode_program(SUI_STRINGS_MODE_PROGRAM);
const DRUIDString MenuParser::command_mode_user(SUI_STRINGS_MODE_USER);


const DRUIDString MenuParser::command_help(SUI_SERIALUI_HELP_KEY);
const DRUIDString MenuParser::command_up(SUI_SERIALUI_UP_KEY);
const DRUIDString MenuParser::command_exit(SUI_SERIALUI_UP_KEY);

const DRUIDString MenuParser::prefix_command(SUI_SERIALUI_KEYHELP_COMMAND_PREFIX_PROG);
const DRUIDString MenuParser::prefix_submenu(SUI_SERIALUI_KEYHELP_SUBMENU_PREFIX_PROG);
const DRUIDString MenuParser::help_sep(SUI_SERIALUI_KEYHELP_SEP_PROG);


MenuParser::MenuParser() {

	top_level_menu = MenuPtr(new Menu(SUI_SERIALUI_TOP_MENU_NAME));

	ctrl_strings.help_key = command_help;
	ctrl_strings.up_key = command_up;
	ctrl_strings.exit_key = command_exit;
	ctrl_strings.prefix_command = prefix_command;
	ctrl_strings.prefix_submenu = prefix_submenu;
	ctrl_strings.help_sep = help_sep;
}

MenuParser::~MenuParser() {
}

void MenuParser::setControlStrings(SerialUIControlStrings ctrl)
{
	ctrl_strings = ctrl;
}

MenuPtr MenuParser::crawl(SerialUserPtr user) {

	// static const DRUIDString;

	top_level_menu->clear();
	DRUID_DEBUG("Crawler start!");

	/* DEADBEEF...
	if (! user->sendAndReceive(command_mode_program))
	{
		DRUID_DEBUG("Could not send program mode command");
		return top_level_menu;
	}
	*/

	if (! parse(user, top_level_menu))
	{
		DRUID_DEBUG("MenuParser::parse() error");
		return top_level_menu;
	}


	return top_level_menu;
}

void MenuParser::dumpMenus()
{
	dumpMenu(top_level_menu);
}

void MenuParser::dumpMenu(MenuPtr menu, unsigned int depth)
{
	for (unsigned int i=0; i < depth; i++)
				std::cout << "\t";
	std::cout << "*** " << menu->name() << std::endl;

	for (MenuItemList::iterator mIter = menu->items()->begin();
			mIter != menu->items()->end();
			mIter++)
	{
		for (unsigned int i=0; i < depth; i++)
			std::cout << "\t";

		bool isSubMenu = false;
		if ((*mIter)->type == MenuItem_Submenu)
		{
			isSubMenu = true;
			std::cout << "+";
		} else {
			std::cout << "*";
		}

		std::cout << (*mIter)->key ;
		if ((*mIter)->help.length())
		{
			std::cout << " (" << (*mIter)->help << ")";
		}

		std::cout << std::endl;
		if (isSubMenu)
		{
			dumpMenu((*mIter)->subMenu, depth+1);
		}
	}

}

bool MenuParser::parse(SerialUserPtr user, MenuPtr menu)
{
	const DRUIDStdString up_command(ctrl_strings.up_key);
	const DRUIDStdString prefix_command(ctrl_strings.prefix_command);
	const DRUIDStdString prefix_submenu(ctrl_strings.prefix_submenu);
	const DRUIDStdString help_sep(ctrl_strings.help_sep);
	const DRUIDStdString prompt_str(ctrl_strings.prompt_str);
	// ^((CMD:(.+?)(HLP(.+?))?)|(SMN:(.+?)(HLP(.+?))?))$
	// 2: whole command
	// 3: command key
	// 4: whole cmd help
	// 5: cmd help string
	// 6: whole submenu
	// 7: submenu key
	// 8: whole submenu help
	// 9: submenu help string
	// 10: whole prompt str
	// 11: prompt name (sub-menu name)
	static const std::string regexp_str(DRUIDStdString("^((")
				+ prefix_command + "(.+?)(" + help_sep + "(.+?))?)|("
				+ prefix_submenu + "(.+?)(" + help_sep + "(.+?))?)|((.+?)" + prompt_str + "\\s*))$"
	);
	static boost::regex expression(regexp_str);



	if (! user->sendAndReceive(ctrl_strings.help_key))
	{
		DRUID_DEBUG("MenuParser::parse--didn't get resp back for 'help' request");
		return false;
	}

	SerialUserStringList linesOfInterest = user->lastMessageAsList();

	for (SerialUserStringList::iterator iter = linesOfInterest.begin();
			iter != linesOfInterest.end(); iter++) {

		DRUID_DEBUG2("parse() checking line: ", *iter);

		boost::smatch what;
		if (boost::regex_match(*iter, what, expression)) {
			DRUID_DEBUG("is of interest...");

			if (what[3].length())
			{
				// is a command...
				DRUID_DEBUG2("Got a command", what[3]);
				// add item
				MenuItemPtr cmdItem(new MenuItem(MenuItem_Command));
				cmdItem->key = what[3];
				cmdItem->help = what[5];

				menu->items()->push_back(cmdItem);

			} else if (what[7].length())
			{
				// is a submenu...
				DRUID_DEBUG2("Got a sub-menu", what[7]);
				// add item and parse
				MenuItemPtr submenuItem(new MenuItem(MenuItem_Submenu));
				submenuItem->key = what[7];
				submenuItem->help = what[9];

				// create the new sub-menu Menu, telling it about its parent in the c'tor
				submenuItem->subMenu = MenuPtr(new Menu(submenuItem->key, menu, submenuItem->key));
				if (! submenuItem->subMenu)
				{
					DRUID_DEBUG2("Could not create a new menu object for sub-menu", submenuItem->key);
					return false;
				}




				menu->items()->push_back(submenuItem);

			} else if (what[11].length())
			{
				// this is our current menu name...
				menu->setName(what[11]);
			}

		} // end if this was a line of interest
	} // end loop over all lines from message

	// ok, we've got all the items for this menu, now we do all the submenus...
	for (MenuItemList::iterator itemIter = menu->items()->begin();
			itemIter != menu->items()->end();
			itemIter++)
	{
		// MenuItem & itm = (*i);
		if ((*itemIter)->type == MenuItem_Submenu && (*itemIter)->subMenu)
		{
			// need to crawl the submenu

			// go in there
			if (user->sendAndReceive((*itemIter)->key))
			{
				// ok, cd'ed into the sub-menu
				// now we parse that one...

				this->parse(user, (*itemIter)->subMenu);


				if (! user->sendAndReceive(up_command))
				{
					DRUID_DEBUG("Could not come back up out of parsing sub-menu??");
					return false;
				}
			} // end if we managed to enter the sub-menu

		} // end if this is a sub-menu

	} // end loop over each menu item.


	return true;
}


} /* namespace DRUID */
