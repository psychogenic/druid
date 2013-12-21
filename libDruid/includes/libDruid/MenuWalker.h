/*
 * MenuWalker.h
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

#ifndef MENUWALKER_H_
#define MENUWALKER_H_

#ifdef HOHOHO
namespace DRUID {

#include "libDruid/Menu.h"

class MenuWalker {
public:
	MenuWalker();
	virtual ~MenuWalker();

	void traverse(MenuPtr topLevelMenu);

	virtual void itemCommand(MenuPtr menu, unsigned int index, DRUIDString key, DRUIDString help);
	virtual void itemSubmenu(MenuPtr menu, MenuPtr subMenu, const DRUIDString & name, unsigned int index);
};

} /* namespace DRUID */

#endif
#endif /* MENUWALKER_H_ */
