/*
 * SUIWindow.cpp -- serialUI user interface window implementation.
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
#include "SUIWindow.h"
#include "Colours.h"
namespace DRUID {



BEGIN_EVENT_TABLE( SUIWindow, wxScrolledWindow)

END_EVENT_TABLE()


SUIWindow::SUIWindow( wxWindow *parent, unsigned int firstTabbableId )
    : wxScrolledWindow( parent, -1, wxDefaultPosition, wxDefaultSize,
                        wxSUNKEN_BORDER|wxTAB_TRAVERSAL|wxScrolledWindowStyle ),
                        text((wxStaticText*)NULL),
                        first_tab_id(firstTabbableId)

{


	wxColour whitecol(DRUID_COLOUR_WHITE);
	wxColour lightFg(DRUID_COLOUR_FOREGROUND_LIGHT);
	wxColour lighBg(DRUID_COLOUR_BACKGROUND_LIGHT);
	wxColour darkBg(DRUID_COLOUR_BACKGROUND_DARK);

	SetForegroundColour(whitecol);
	SetBackgroundColour(lighBg);

    // Set the rate we'd like for scrolling.

    SetScrollRate( 5, 5 );

    // Populate a sizer with a 'resizing' button and some
    // other static decoration

    inner_sizer = new wxBoxSizer(wxVERTICAL );


    // Then use the sizer to set the scrolled region size.

    SetSizer( inner_sizer );
}
void SUIWindow::setText(wxString txt)
{
	clear();

	inner_sizer->Add( new wxStaticText( this, wxID_ANY, txt),
	                    0,
	                    wxALIGN_CENTER );
	this->Layout();
}
void SUIWindow::clear()
{
	inner_sizer->Clear(true);


}
void SUIWindow::display()
{
	Show(true);
	if (first_tab_id)
	{
		wxWindow * win = FindWindow(first_tab_id);

		if (win)
			win->SetFocus();
	}
}

void SUIWindow::setError(wxString err)
{
	SetBackgroundColour( wxT("RED") );
	setText(err);
}
} /* namespace DRUID */
