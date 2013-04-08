/*
 * SUIWindow.h -- The SerialUI interface window.
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

#ifndef SUIWINDOW_H_
#define SUIWINDOW_H_

#include "Widgets.h"
namespace DRUID {


class SUIWindow : public wxScrolledWindow
{
public:

    SUIWindow( wxWindow *parent , unsigned int firstTabbableId);

    wxBoxSizer * innerSizer() { return inner_sizer; }

    void setText(wxString txt);
    void setError(wxString errorMsg);


    void clear();

    void display();


private:

    wxStaticText * text;
    wxBoxSizer * inner_sizer;
    unsigned int first_tab_id;



    DECLARE_EVENT_TABLE()
};



} /* namespace DRUID */
#endif /* SUIWINDOW_H_ */
