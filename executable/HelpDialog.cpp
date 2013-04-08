/*
 * HelpDialog.cpp -- online help dialog implementation.
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

#include "HelpDialog.h"

namespace DRUID {

enum {
	ID_HTMLWin=300,
};

HelpDialog::HelpDialog() :
		wxDialog(NULL, -1, _T("Help"), wxDefaultPosition, wxSize(SERIALGUI_HELPDIALOG_WIDTH, SERIALGUI_HELPDIALOG_HEIGHT),
		wxDEFAULT_DIALOG_STYLE | wxOK | wxCANCEL| wxICON_QUESTION | wxRESIZE_BORDER) {

	wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);

	wxHtmlWindow * html = new wxHtmlWindow(this, ID_HTMLWin);

	html->SetPage(*_help_en_html);

	vbox->Add(html, wxSizerFlags(1).Align(wxALIGN_CENTER).Expand().Border(wxALL, 10));


	wxButton *okButton = new wxButton(this, wxID_OK, wxT("OK"), wxDefaultPosition,
			wxSize(70, 30));
	vbox->Add(okButton, wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 10));

	SetSizer(vbox);
}

HelpDialog::~HelpDialog() {

}


void HelpDialog::OnLinkClick(wxHtmlLinkEvent& evt)
{
	wxHtmlLinkInfo info = evt.GetLinkInfo();

	wxLaunchDefaultBrowser(info.GetHref());
};

BEGIN_EVENT_TABLE(HelpDialog, wxDialog)
EVT_HTML_LINK_CLICKED(ID_HTMLWin,  HelpDialog::OnLinkClick)
END_EVENT_TABLE()


} /* namespace DRUID */
