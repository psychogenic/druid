/*
 * SettingsDialog.h -- The serial port/baud rate settings dialog.
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

#ifndef SETTINGSDIALOG_H_
#define SETTINGSDIALOG_H_
#include "Widgets.h"
#include <map>
#include <SerialDruid.h>

namespace DRUID {

typedef struct SettingsDetailsStruct {
	int userAction;
	bool valid;
	PortName portName;
	wxString portStr;
	unsigned int baud;
} SettingDetails;

typedef std::map<wxString, unsigned int> BaudStrToBaudRateMap;

#define SERIALGUI_SETTINGSDIAL_WIDTH	400
#define SERIALGUI_SETTINGSDIAL_HEIGHT	180
#define SERIALGUI_SETTINGSDIAL_HEIGHT_FIRSTTIME		SERIALGUI_SETTINGSDIAL_HEIGHT + 220

class SettingsDialog : public wxDialog {
public:
	SettingsDialog(const wxString & title);


	void setValues(PortName & serial, unsigned int baud);

	SettingDetails getDetails(bool firstTimeUse=false);

	virtual ~SettingsDialog();

private:
	wxComboBox * port_selector;
	wxChoice * baud_selector;
	wxStaticText * port_sel_str;
	wxStaticText * greetings_text;
	wxCheckBox * get_counted_check;
	wxStaticText * count_me_text;

	static BaudStrToBaudRateMap baud_map;
	static void setupBaudMap();



};

} /* namespace DRUID */
#endif /* SETTINGSDIALOG_H_ */
