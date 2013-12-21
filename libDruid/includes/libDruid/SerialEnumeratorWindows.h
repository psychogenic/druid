/*
 * SerialEnumeratorWindows.h -- windows serial port enumerator.
 * I have no idea how to do this, just spits out COM[1..4].
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

#ifndef SERIALENUMERATORWINDOWS_H_
#define SERIALENUMERATORWINDOWS_H_

#include "libDruid/SerialEnumeratorBase.h"

namespace DRUID {

class SerialEnumeratorWindows : public SerialEnumeratorBase {
public:
	SerialEnumeratorWindows();
	virtual ~SerialEnumeratorWindows();


	virtual bool portExists(const PortName & pName);
	virtual PortNameList listPorts();


};

} /* namespace DRUID */
#endif /* SERIALENUMERATORWINDOWS_H_ */
