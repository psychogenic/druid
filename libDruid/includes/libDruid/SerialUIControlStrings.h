/*
 * SerialUIControlStrings.h -- storage for various control strings as
 * reported by SerialUI.
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

#ifndef SERIALUICONTROLSTRINGS_H_
#define SERIALUICONTROLSTRINGS_H_

#include "libDruid/SerialGUIConfig.h"
#include "libDruid/ExternalIncludes.h"


namespace DRUID {


typedef struct SerialUIControlStringsStruct {
	DRUIDString version;
	DRUIDString up_key;
	DRUIDString exit_key;
	DRUIDString error;
	DRUIDString help_key;
	DRUIDString prefix_command;
	DRUIDString prefix_submenu;
	DRUIDString help_sep;
	DRUIDString more_str;
	DRUIDString more_num;
	DRUIDString prompt_str;
	DRUIDString eot_str;

} SerialUIControlStrings ;


} /* namespace DRUID */

#endif /* SERIALUICONTROLSTRINGS_H_ */
