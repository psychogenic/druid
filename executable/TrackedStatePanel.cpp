/*
 * TrackedStatePanel.cpp
 *
 *  Created on: Aug 10, 2014
 *      Author: malcalypse
 */

#include "Config.h"
#include "TrackedStatePanel.h"
#include "Colours.h"

namespace DRUID {


TrackedStatePanel::TrackedStatePanel(wxWindow *parent) : wxPanel( parent, -1, wxDefaultPosition, wxDefaultSize,
        wxSUNKEN_BORDER|wxTAB_TRAVERSAL)
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


	topsizer = new wxBoxSizer(wxVERTICAL);
	vsizer = new wxBoxSizer(wxVERTICAL);


	// a little spacer text
	// topsizer->Add(new wxStaticText(this,wxID_ANY, wxT("")), 0, wxALIGN_CENTER );
	topsizer->Add(vsizer);

	SetSizer(topsizer);


}

void TrackedStatePanel::updateDisplay(SUIUserIdxToTrackedStateVariablePtr & trackedVarData)
{


	wxColour whitecol(DRUID_COLOUR_WHITE);
	wxColour darkFg(DRUID_COLOUR_FOREGROUND_DARK);

	for (SUIUserIdxToTrackedStateVariablePtr::iterator iter= trackedVarData.begin();
			iter != trackedVarData.end();
			iter++)
	{

		uint8_t idx = ((*iter).first);
		TrackedStatePanelEntriesMap::iterator findIter = tracked_entries.find(idx);
		if (findIter == tracked_entries.end())
		{
			// not yet present...
			tracked_entries[idx].tracked_state = (*iter).second;


			tracked_entries[idx].label = new wxStaticText(this, wxID_ANY, DRUID_STDSTRING_TOWX(((*iter).second)->name));
			switch (((*iter).second)->type)
			{
			case SUI::SUITracked_Bool:
				tracked_entries[idx].value = NULL;
				break;
			case SUI::SUITracked_UInt:
				/* fall through */
			case SUI::SUITracked_Float:
				tracked_entries[idx].value =
						new wxStaticText(this, wxID_ANY, DRUID_STDSTRING_TOWX(((*iter).second)->last_val));

			}

			if (tracked_entries[idx].value != NULL)
			{
				wxBoxSizer * hsizer = new wxBoxSizer(wxHORIZONTAL);
				hsizer->Add(tracked_entries[idx].label, wxSizerFlags(1).Align(wxALIGN_LEFT).Border(
						wxLEFT | wxRIGHT, 10).Expand());
				hsizer->Add(tracked_entries[idx].value, wxSizerFlags(1).Align(wxALIGN_RIGHT).Border(
						wxLEFT | wxRIGHT, 10).Expand());
				vsizer->Add(hsizer, wxSizerFlags(2).Expand().Align(wxALIGN_CENTER).Border(wxALL, 2));
			} else {
				vsizer->Add(tracked_entries[idx].label, wxSizerFlags(2).Expand().Align(wxALIGN_RIGHT).Border(
												wxLEFT , 10));
			}


		}

		switch (((*iter).second)->type)
		{

		case SUI::SUITracked_Bool:
			tracked_entries[idx].label->SetForegroundColour( tracked_entries[idx].tracked_state->val_bool ? whitecol : darkFg);
			break;
		case SUI::SUITracked_UInt:
			/* fall through */
		case SUI::SUITracked_Float:

			tracked_entries[idx].value->SetLabel(DRUID_STDSTRING_TOWX(tracked_entries[idx].tracked_state->last_val));

		}
	}

	Layout();

}

void TrackedStatePanel::clearDisplay()
{
	vsizer->Clear(true); // clear and delete windows
	tracked_entries.clear();
	Layout();
}

} /* namespace DRUID */


