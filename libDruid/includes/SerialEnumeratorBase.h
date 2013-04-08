/*
 * SerialEnumeratorBase.h -- base class for serial port enumerators
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

#ifndef SERIALENUMERATORBASE_H_
#define SERIALENUMERATORBASE_H_

#include "ExternalIncludes.h"

namespace DRUID {

typedef std::string PortName;
typedef std::vector<PortName> PortNameList;

class SerialEnumeratorBase {
public:
	SerialEnumeratorBase();
	virtual ~SerialEnumeratorBase();

	virtual bool portExists(const PortName & pName);

	virtual PortNameList listPorts() = 0;


};

} /* namespace DRUID */
#endif /* SERIALENUMERATORBASE_H_ */
