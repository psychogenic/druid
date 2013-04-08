/*
 * SerialEnumeratorLinux.h -- Linux serial enumerator, relies on presence of
 * /dev/serial.
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

#ifndef SERIALENUMERATORLINUX_H_
#define SERIALENUMERATORLINUX_H_

#include "SerialEnumeratorBase.h"

namespace DRUID {

class SerialEnumeratorLinux : public SerialEnumeratorBase {
public:
	SerialEnumeratorLinux();
	virtual ~SerialEnumeratorLinux();

	virtual PortNameList listPorts();

};

} /* namespace DRUID */
#endif /* SERIALENUMERATORLINUX_H_ */
