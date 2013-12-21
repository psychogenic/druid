/*
 * Menu.h -- represents a menu or sub-menu
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

#ifndef MENU_H_
#define MENU_H_


#include "libDruid/ExternalIncludes.h"
#include "libDruid/SerialUser.h"
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

namespace DRUID {

// forward decl
class MenuItem;
typedef boost::shared_ptr<MenuItem> MenuItemPtr;
typedef std::vector<MenuItemPtr> MenuItemList;

class Menu; // forward decl

typedef boost::shared_ptr<Menu> MenuPtr;
// typedef boost::weak_ptr<Menu> MenuWeakPtr;

typedef unsigned int MenuUID;

class Menu  {
public:
	Menu(const DRUIDString & menuName, MenuPtr parent, const DRUIDString & parentKey);
	Menu(const DRUIDString & menuName);
	~Menu();

	MenuUID uid();

	void clear();

	const DRUIDString & name();
	void setName(const DRUIDString & setTo);


	size_t size() { return menu_items.size(); }
	MenuItemPtr operator[](const DRUIDString & key);
	MenuItemPtr operator[](const unsigned int i);


	MenuItemPtr item(const DRUIDString & key);
	MenuItemPtr item(const unsigned int i);
	MenuItemList * items();

	bool hasParent();
	MenuPtr parent();
	const DRUIDString & parentKey() { return parent_key;}
	void setParent(MenuPtr parent);


private:

	static MenuUID menu_uid_counter;
	void init();

	DRUIDString menu_name;
	MenuPtr parent_menu;
	DRUIDString parent_key; // key used to access this menu
	MenuItemList menu_items;
	MenuUID menu_uid;
};

typedef enum MenuItemTypeEnum {
	MenuItem_Command=0,
	MenuItem_Submenu
} MenuItemType;



class MenuItem {

public:
	MenuItem(MenuItemType type);
	~MenuItem();

	MenuItemType type;
	DRUIDString key;
	DRUIDString help;
	MenuPtr subMenu;

} ;

} /* end namespace DRUID */


#endif /* MENU_H_ */
