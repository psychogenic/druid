/*
 * InputPanel.cpp -- input panel implementation.
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
#include <libDruid/SerialDruid.h>
#include "Config.h"
#include "InputPanel.h"
#include "Colours.h"
namespace DRUID {

enum
{
    ID_TextInput = 25,
    ID_Submit,
    ID_ResetColour,

};

InputPanel::InputPanel(wxWindow *parent, InputPanelReceiver * receiver) : wxPanel( parent, -1, wxDefaultPosition, wxDefaultSize,
        wxSUNKEN_BORDER|wxTAB_TRAVERSAL), rcvr(receiver)
{


	wxColour whitecol(DRUID_COLOUR_WHITE);
	wxColour darkFg(DRUID_COLOUR_FOREGROUND_DARK);
	wxColour lighBg(DRUID_COLOUR_BACKGROUND_LIGHT);
	wxColour darkBg(DRUID_COLOUR_BACKGROUND_DARK);

	SetForegroundColour(whitecol);
	SetBackgroundColour(darkBg);


	wxSize txtMinSize(120, 30);
	wxSize txtMaxSize(350, 30);
	wxSize btnMinSize(80, 30);
	wxSize btnMaxSize(100, 40);

	resetColourTimer = new wxTimer(this, ID_ResetColour);

	topsizer = new wxBoxSizer(wxVERTICAL);
	errsizer = new wxBoxSizer(wxVERTICAL);
	inputsizer = new wxBoxSizer(wxHORIZONTAL);


	// a little spacer text
	topsizer->Add(new wxStaticText(this,wxID_ANY, wxT("")), 0, wxALIGN_CENTER );

	titleTxt = new wxStaticText(this, wxID_ANY, wxT("Input"));
	topsizer->Add(titleTxt, 0, wxALIGN_CENTER );

	txt_input = new wxTextCtrl(this, ID_TextInput, wxEmptyString, wxDefaultPosition, txtMinSize, wxTE_PROCESS_ENTER);
	txt_input->SetMinSize(txtMinSize);
	txt_input->SetMaxSize(txtMaxSize);

	inputPrefixTxt = new wxStaticText(this, wxID_ANY, wxT("\"\""));
	inputsizer->Add(inputPrefixTxt, wxSizerFlags(0)
			.Align(wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL)
			.Border(wxLEFT, 10));

	inputsizer->Add(txt_input, wxSizerFlags(1)
			.Align(wxALIGN_CENTER).Border(wxRIGHT, 10)
			.Border(wxLEFT, 2));

	topsizer->Add(inputsizer, wxSizerFlags(2).Expand().Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));



	wxButton * btn = new wxButton(this, ID_Submit, wxT("OK"));
	btn->SetForegroundColour(darkFg);
	btn->SetMinSize(btnMinSize);
	btn->SetMaxSize(btnMaxSize);
	topsizer->Add(btn, wxSizerFlags(1).Align(wxALIGN_CENTER_HORIZONTAL | wxALIGN_TOP).Border(wxALL, 5));


	wxStaticText * spacerTxt = new wxStaticText(this, wxID_ANY, wxT(""));

	errTxt = new wxStaticText(this, wxID_ANY, wxT(" "));

	errsizer->Add(spacerTxt, 0, wxALIGN_CENTER );
	errsizer->Add(errTxt, 0, wxALIGN_CENTER );
	topsizer->Add(errsizer, wxSizerFlags(1).Align(wxALIGN_CENTER).Border(wxALL, 10));

	topsizer->Add(new wxStaticText(this, wxID_ANY, wxT(" ")), wxSizerFlags(3).Expand());


	SetSizer(topsizer);


}


void InputPanel::notifyUser()
{
	wxColour darkFg(DRUID_COLOUR_FOREGROUND_DARK);
	wxColour lighBg(DRUID_COLOUR_BACKGROUND_LIGHT);


	SetForegroundColour(darkFg);
	SetBackgroundColour(lighBg);
	colourToggleCounter = 5;

	resetColourTimer->Start(100, false);
}
void InputPanel::OnResetColourTimer(wxTimerEvent & event)
{
	wxColour darkFg(DRUID_COLOUR_FOREGROUND_DARK);
	wxColour lighBg(DRUID_COLOUR_BACKGROUND_LIGHT);
	wxColour whitecol(DRUID_COLOUR_WHITE);
	wxColour darkBg(DRUID_COLOUR_BACKGROUND_DARK);

	if (colourToggleCounter)
	{
		if (colourToggleCounter % 2)
		{

			SetForegroundColour(darkFg);
			SetBackgroundColour(lighBg);
		} else {

			SetForegroundColour(whitecol);
			SetBackgroundColour(darkBg);
		}

		colourToggleCounter--;

	} else {

		// we're done
		SetForegroundColour(whitecol);
		SetBackgroundColour(darkBg);
		resetColourTimer->Stop();
	}


	Layout();


}

void InputPanel::setError(wxString errMsg)
{
	errTxt->SetLabel(errMsg);
	errTxt->Wrap(250);

	Layout();
}
void InputPanel::acceptInput(bool setTo, wxString commandName,
		DRUID::UserInputType input_type)
{

	static wxString emptyErr(wxT(""));

	if (setTo)
	{

		setError(emptyErr);
		titleTxt->SetLabel(commandName);

		if (input_type==InputType_Numeric || input_type==InputType_Integer)
		{
			DRUID4ARDUINO_DEBUG("Accepting NUMERIC input");

			inputPrefixTxt->SetLabel(wxT("#"));
			/*
			inputsizer->Detach(inputPrefixTxt);
			inputPrefixTxt->Destroy();
			inputPrefixTxt = new wxStaticText(this, wxID_ANY, wxT("#"));
			inputsizer->Prepend(inputPrefixTxt, 0, wxALIGN_RIGHT);
			*/

			txt_input->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &input_value));
		} else if (input_type == InputType_Stream)
		{
			DRUID4ARDUINO_DEBUG("Looking for stream input... need a file");
			DRUID4ARDUINO_DEBUG("SHOULD HAVE BEEN HANDLED OUTSIDE OF HERE");
			return;


		} else {
			DRUID4ARDUINO_DEBUG("Accepting ANY input");


			inputPrefixTxt->SetLabel(wxT("\"\""));

			/*
			inputsizer->Detach(inputPrefixTxt);

			inputPrefixTxt->Destroy();
			inputPrefixTxt = new wxStaticText(this, wxID_ANY, wxT("\"\""));
			inputsizer->Prepend(inputPrefixTxt, 0, wxALIGN_RIGHT);
			*/
			txt_input->SetValidator(wxTextValidator(wxFILTER_NONE, &input_value));
		}


		notifyUser();

		this->Enable(true);
		txt_input->SetFocus();
	} else {
		titleTxt->SetLabel(wxT("Input"));
		txt_input->Clear();
		this->Enable(false);
	}


	Layout();
}


void InputPanel::sendFocusToTextArea()
{

	txt_input->SetFocus();
}
void InputPanel::clearInputText()
{
	txt_input->Clear();


}

void InputPanel::OnEnter(wxCommandEvent& event)
{
	sendToReceiver();
}
void InputPanel::OnSubmit(wxCommandEvent& event)
{
	sendToReceiver();
}

void InputPanel::sendToReceiver()
{
	if (Validate()) {
		txt_input->GetValidator()->TransferFromWindow();
		if (rcvr) {
			if (input_value.size()) {

				txt_input->SetValue(input_value);
				rcvr->inputReceivedFromPanel(input_value);
			}
		}
	}

}
BEGIN_EVENT_TABLE(InputPanel, wxPanel)
	EVT_TEXT_ENTER(ID_TextInput, InputPanel::OnEnter)
    EVT_BUTTON(ID_Submit, InputPanel::OnSubmit)
    EVT_TIMER(ID_ResetColour, InputPanel::OnResetColourTimer)
END_EVENT_TABLE()

} /* namespace DRUID */
