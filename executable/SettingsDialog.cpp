/*
 * SettingsDialog.cpp -- settings dialog implementation.
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

#include <SerialDruid.h>
#include <wx/url.h>
#include <iostream>
#include "Config.h"

#include "SettingsDialog.h"
namespace DRUID {

enum {
	ID_PortSelector=20,
	ID_BaudSelector,
	ID_GetCounted,
};
BaudStrToBaudRateMap SettingsDialog::baud_map;

void SettingsDialog::setupBaudMap()
{
	if (baud_map.size())
		return;

	baud_map[wxT("  4800")] = 4800;
	baud_map[wxT("  9600")] = 9600;
	baud_map[wxT(" 14400")] = 14400;
	baud_map[wxT(" 19200")] = 19200;
	baud_map[wxT(" 28800")] = 28800;
	baud_map[wxT(" 38400")] = 38400;
	baud_map[wxT(" 57600")] = 57600;
	baud_map[wxT("115200")] = 115200;

}



SettingsDialog::SettingsDialog(const wxString & title) :
		wxDialog(NULL, -1, title, wxDefaultPosition, wxSize(SERIALGUI_SETTINGSDIAL_WIDTH, SERIALGUI_SETTINGSDIAL_HEIGHT),
				wxDEFAULT_DIALOG_STYLE | wxOK | wxCANCEL| wxICON_QUESTION | wxRESIZE_BORDER) {


	wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *buttonBox = new wxBoxSizer(wxHORIZONTAL);

	// put the port/baud selectors in a grid
	wxGridSizer * choicesGrid = new wxGridSizer(2, 2, 0, 0);

	// prepare the port selector combo box
	SerialEnumerator sEnum;
	PortNameList pNames = sEnum.listPorts();
	wxArrayString port_choices;
	for (PortNameList::iterator pIter = pNames.begin();
			pIter != pNames.end();
			pIter++) {
		port_choices.Add(DRUID_STDSTRING_TOWX((*pIter)));
	}
	wxString defaultChoice;
	if (pNames.size())
	{
		defaultChoice = port_choices[0];
	}

	port_selector = new wxComboBox(this, ID_PortSelector, defaultChoice,
			wxDefaultPosition, wxSize(180, 30), port_choices, wxCB_DROPDOWN);

	port_sel_str = new wxStaticText(this, wxID_ANY, wxT("Serial Port"));
	choicesGrid->Add(port_sel_str, 0, wxALIGN_CENTER);
	choicesGrid->Add(port_selector, wxSizerFlags(1).Align(wxALIGN_CENTER_VERTICAL)
			.Align(wxALIGN_LEFT).Border(wxALL, 10));

	// prepare the baud rate choices

	setupBaudMap();
	wxArrayString baud_choices;
	for (BaudStrToBaudRateMap::iterator iter = baud_map.begin();
			iter != baud_map.end(); iter++) {
		baud_choices.Add((*iter).first);
	}
	defaultChoice = baud_choices[0];
	// TODO: how to set default choice?
	baud_selector = new wxChoice(this, ID_BaudSelector, wxDefaultPosition, wxDefaultSize,
			baud_choices);



	choicesGrid->Add(new wxStaticText(this, wxID_ANY, wxT("Baud Rate")), 0, wxALIGN_CENTER);
	choicesGrid->Add(baud_selector, wxSizerFlags(1).Align(wxALIGN_CENTER_VERTICAL)
			.Align(wxALIGN_LEFT).Border(wxALL, 10));


	wxButton *okButton = new wxButton(this, wxID_OK, wxT("OK"), wxDefaultPosition,
			wxSize(70, 30));
	wxButton *closeButton = new wxButton(this, wxID_CANCEL, wxT("Cancel"),
			wxDefaultPosition, wxSize(70, 30));

	buttonBox->Add(okButton, 1);
	buttonBox->Add(closeButton, 1, wxLEFT, 5);


	wxBoxSizer *greetsBox = new wxBoxSizer(wxHORIZONTAL);
	greetings_text = new wxStaticText(this, wxID_ANY, wxT("Welcome to Druid4Arduino!  Please specify the settings to use."));
	greetings_text->Wrap(350);
	greetsBox->Add(greetings_text, wxSizerFlags(1).Align(wxALIGN_CENTER).Border(wxALL, 10));


	wxBoxSizer *countedBox = new wxBoxSizer(wxHORIZONTAL);
	get_counted_check = new wxCheckBox(this, ID_GetCounted, wxT(""));
	get_counted_check->SetValue(true);

	count_me_text = new wxStaticText(this, wxID_ANY,
			wxT("Count me! We're curious: leave this box checked to give us an idea of how many people are using the program, and on which OS.\r\n\r\nNO personal info will be sent.")
			);
	count_me_text->Wrap(320);

	countedBox->Add(get_counted_check, wxSizerFlags(0).Align(wxALIGN_LEFT).Border(wxALL, 5));
	countedBox->Add(count_me_text, wxSizerFlags(2).Align(wxALIGN_LEFT).Border(wxALL, 10));

	vbox->Add(greetsBox, wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 0));
	vbox->Add(choicesGrid, wxSizerFlags(1).Expand()
			.Align(wxALIGN_CENTER).Border(wxALL, 10));
	vbox->Add(countedBox, wxSizerFlags(1).Expand()
			.Align(wxALIGN_CENTER).Border(wxALL, 10));
	vbox->Add(buttonBox, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);


	SetSizer(vbox);


}

SettingsDialog::~SettingsDialog() {
}

void SettingsDialog::setValues(PortName & serial, unsigned int baud)
{
	if  (serial.size())
	{
		port_selector->SetValue(DRUID_STDSTRING_TOWX(serial));
	}

	for (BaudStrToBaudRateMap::iterator iter = baud_map.begin();
			iter != baud_map.end();
			iter++)
	{
		if ((*iter).second == baud)
		{
			baud_selector->SetSelection(baud_selector->FindString((*iter).first));
		}
	}

	return;

}
SettingDetails SettingsDialog::getDetails(bool firstTimeUse) {
	static PortName emptyPortName("");
	SettingDetails retDets;

	port_selector->SetFocus();

	if (firstTimeUse) {
		setValues(emptyPortName, 9600);
		greetings_text->Show(true);
		get_counted_check->Show(true);
		count_me_text->Show(true);
		SetSize(SERIALGUI_SETTINGSDIAL_WIDTH, SERIALGUI_SETTINGSDIAL_HEIGHT_FIRSTTIME);

	} else {
		count_me_text->Show(false);
		greetings_text->Show(false);
		get_counted_check->Show(false);
	}


	retDets.valid = false;
	retDets.userAction = ShowModal();

	if (retDets.userAction != wxID_OK)
	{
		Destroy();
		return retDets;
	}


	wxString selPort = port_selector->GetValue();

	PortName selPortName(selPort.mb_str());

	SerialEnumerator sEnum;
	PortNameList pNames = sEnum.listPorts();
	if ( ! (selPortName.size() && sEnum.portExists(selPortName)) )
	{
		port_sel_str->SetLabel(wxT("You MUST select or enter a Serial Port"));
		port_sel_str->Wrap(120);
		return getDetails(firstTimeUse);

	}


	wxString selBaud = baud_selector->GetString(baud_selector->GetSelection());

	BaudStrToBaudRateMap::iterator findIter = baud_map.find(selBaud);
	if (findIter != baud_map.end())
	{

		retDets.valid = true;
		retDets.baud = (*findIter).second;
		retDets.portStr = selPort;
		retDets.portName = selPortName;

	}

	if (firstTimeUse && get_counted_check->GetValue())
	{
		wxString counterURLStr(wxT(SERIALGUI_COUNTER_URL));
		wxURL counterURL(counterURLStr);
		if (counterURL.GetError() == wxURL_NOERR)
		{
			SERIALGUI_DEBUG("Connecting to counter...");
			wxInputStream *in_stream;

			in_stream = counterURL.GetInputStream();
			if (in_stream)
			{
				char buf[255];
				while (! in_stream->Eof())
				{
					in_stream->Read(buf, 255);
				}
			}

		} else {
			SERIALGUI_DEBUG("Could not connect to counter, aborting.");
		}
	}


	Destroy();
	return retDets;
}


} /* namespace DRUID */
