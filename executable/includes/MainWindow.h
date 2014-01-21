/*
 * MainWindow.h -- our ginourmous main window.
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

#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_
#include <map>
#include <string>
#include "Config.h"
#include "SUIWindow.h"
#include "InputPanel.h"
#include "Widgets.h"
#include "HelpStrings.h"

namespace DRUID {

typedef std::map<std::string, bool> MenuHotKeysUsedMap;
typedef std::map<DRUID::MenuUID, SUIWindow *> MenuUIDToSUIWindowMap;

#define DEFAULT_SUIWINDOW_UID	0
#define MAXIMUM_IDLE_SECS		12

#define UPLOAD_RATE_DELAYFACTOR_CONSERVATIVE		80
#define UPLOAD_RATE_DELAYFACTOR_STANDARD			35
#define UPLOAD_RATE_DELAYFACTOR_FAST				20
#define UPLOAD_RATE_DELAYFACTOR_RECKLESS			5



class MainWindow  : public wxFrame, public InputPanelReceiver {

public:
	MainWindow(const wxString& title, const wxPoint& pos, const wxSize& size);

    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnSettings(wxCommandEvent& event);

    void OnToggleRawInput(wxCommandEvent& event);
    void OnSelectUploadRateConservative(wxCommandEvent& event);
    void OnSelectUploadRateStandard(wxCommandEvent& event);
    void OnSelectUploadRateFast(wxCommandEvent& event);
    void OnSelectUploadRateReckless(wxCommandEvent& event);

    void OnHelp(wxCommandEvent& event);

    void OnReInit(wxCommandEvent& event);

    void OnOutputClear(wxCommandEvent& event);
    void OnOutputExport(wxCommandEvent& event);

    void OnSiteClick(wxCommandEvent& event);

    void OnPingTimer(wxTimerEvent & event);
    void OnAutoInitTimer(wxTimerEvent & event);
    void OnMenuCrawlTimer(wxTimerEvent & event);


	void OnRawInputEnter(wxCommandEvent& event);

    void resetStatusBar();


	virtual void inputReceivedFromPanel(wxString input);

private:

	void doReInit();
    void buildMenus();
    void buildToolbar();

    void buildAllDeviceMenus(DRUID::MenuPtr topLevelMenu);
    void buildDeviceMenu(DRUID::MenuPtr sguiMenu, wxMenu * intoMenu);
    void touchLastInteraction();

    void showAwaitingInputDialog();

    bool doGetSettings(bool first_time_user=false);


    std::string menuItemKeyToHotKey(std::string & key, MenuHotKeysUsedMap * used_keys);


    SUIWindow * newSUIWindow(DRUID::MenuUID forUID, unsigned int firstTabbableId);
    SUIWindow * getSUIWindow(DRUID::MenuUID uid);
    bool enableSUIWindow(DRUID::MenuUID uid);
    bool showSUIWindow(SUIWindow * suiwin);
    SUIWindow * currentlyEnabledSUIWindown();
    void resetSUIWindows();

    void OnEnterSubMenu(wxCommandEvent& event);
    void OnUpMenu(wxCommandEvent& event);
    void OnRunCommand(wxCommandEvent& event);

    void OnExecuteMenuItem(wxCommandEvent& event);



    void moveToTopLevelMenu();
    bool executeCommand(const DRUIDString & command);

    void saveConfig();
    bool loadConfig();
    void configUpdateLastUploadedFilePath(wxString & filepath);

    bool sendFileStream();


    unsigned int menu_id_counter;

    wxMenuBar* menuBar;
    wxMenu* deviceMenu;
    wxMenuItem * raw_input_toggle;
    wxToolBar *toolbar;
    InputPanel * input;
	wxTextCtrl * raw_input;
	wxStaticText * raw_input_label;
    wxTextCtrl * outputTextCtrl;
    wxTimer * autoInitTimer;
    wxTimer * pingTimer;
    wxTimer * crawlMenusTimer;
    wxWindow * oldFocus;



    wxString prog_name;
    unsigned int baud_rate;
    std::string serial_port;

    DRUID::ConnectionPackagePtr connection;
    DRUID::MenuPtr topLevelMenu;

    DRUID::MenuParser parser;
    MenuUIDToSUIWindowMap uid_to_window;
    SUIWindow * suiwin_currently_showing;
    wxBoxSizer * suiWinListSizer;
    bool awaiting_input;
    time_t last_interaction;
    unsigned int current_menu_depth;

    wxBitmap up_icon;
    wxBitmap cmd_icon;
    wxBitmap submenu_icon;
    unsigned int upload_rate_delay_factor;
    wxString last_uploaded_filepath;


    DECLARE_EVENT_TABLE();
};

} /* namespace DRUID */
#endif /* MAINWINDOW_H_ */
