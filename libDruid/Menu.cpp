/*
 * Menu.cpp -- Menu implementation.
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

#include "Menu.h"

namespace DRUID {

MenuUID Menu::menu_uid_counter = 100;

Menu::Menu(const DRUIDString & menuName, MenuPtr parent, const DRUIDString & parentKey) :
		menu_name(menuName),
		parent_key(parentKey)
{
	parent_menu = parent;
	init();
}

Menu::Menu(const DRUIDString& menuName) : menu_name(menuName){
	init();
}

void Menu::init()
{
	menu_uid = menu_uid_counter++;
}

MenuUID Menu::uid()
{
	return menu_uid;
}

Menu::~Menu()
{
	if (parent_menu)
	{

		// release the parent, to break circular references
		MenuPtr emptyPtr;
		parent_menu = emptyPtr;

	}

}

bool Menu::hasParent()
{
	if (parent_menu)
		return true;

	return false;
}
MenuPtr Menu::parent()
{
	return parent_menu;
}

void Menu::setParent(MenuPtr parent)
{
	parent_menu = parent;
}

void Menu::clear()
{
	menu_items.clear();
}


const DRUIDString & Menu::name()
{
	return menu_name;
}
void Menu::setName(const DRUIDString & setTo)
{

	menu_name = setTo;

}


MenuItemPtr Menu::operator[](const DRUIDString & key)
{
	return item(key);
}
MenuItemPtr Menu::operator[](const unsigned int i)
{
	return item(i);
}

MenuItemPtr Menu::item(const unsigned int i)
{

	MenuItemPtr emptyPtr;
	if (i >= menu_items.size())
	{
		return emptyPtr;
	}

	return menu_items.at(i);

}
MenuItemPtr Menu::item(const DRUIDString & key)
{
	MenuItemPtr emptyPtr;
	for (MenuItemList::iterator iter = menu_items.begin();
			iter != menu_items.end();
			iter++)
	{
		if ((*iter)->key == key)
		{
			// gotcha!
			return (*iter);
		}
	}

	return emptyPtr;
}
MenuItemList * Menu::items()
{
	return &menu_items;
}


MenuItem::MenuItem(MenuItemType item_type):
		type(item_type)
{
}

MenuItem::~MenuItem() {



}




} /* end namespace DRUID */


