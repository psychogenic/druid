/*
 * InputPanel.h -- main window panel used to request user input.
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

#ifndef InputPanel_H_
#define InputPanel_H_

#include "Widgets.h"
namespace DRUID {


class InputPanelReceiver {

public:
	InputPanelReceiver() {}
	virtual ~InputPanelReceiver() {}

	virtual void inputReceivedFromPanel(wxString input) = 0;

};

class InputPanel : public wxPanel {
public:
	InputPanel(wxWindow *parent, InputPanelReceiver * receiver);


	void setReceiver(InputPanelReceiver * receiver) {rcvr = receiver;}

	void sendFocusToTextArea();
	void clearInputText();

	void acceptInput(bool setTo, wxString commandName=_T(""),
			DRUID::UserInputType input_type=DRUID::InputType_String);
	void setError(wxString errMsg);



	void OnEnter(wxCommandEvent& event);
	void OnSubmit(wxCommandEvent& event);
	void OnResetColourTimer(wxTimerEvent & event);
    DECLARE_EVENT_TABLE()

private:
	void sendToReceiver();
	void notifyUser();

	InputPanelReceiver * rcvr;
	wxTextCtrl * txt_input;
	wxBoxSizer *topsizer;
	wxBoxSizer *inputsizer;
	wxBoxSizer *errsizer;
	wxStaticText * titleTxt;
	wxStaticText * errTxt;
	wxStaticText * inputPrefixTxt;
	wxTimer * resetColourTimer;
	unsigned int colourToggleCounter;



	wxString input_value;


};

} /* namespace DRUID */
#endif /* InputPanel_H_ */
