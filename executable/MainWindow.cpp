/*
 * MainWindow.cpp -- main window implementation. Pretty hairy.
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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/lexical_cast.hpp>
#include <wx/config.h>
#include <wx/url.h>
#include <wx/hyperlink.h>
#include "MainWindow.h"
#include "SUIWindow.h"
#include "Colours.h"
#include "SettingsDialog.h"
#include "EmbeddedImages.h"
#include "CallbackData.h"
#include "Config.h"
#include "HelpDialog.h"
#include "Resources.h"




namespace DRUID {


#define MAINWINDOW_DYNAMICMENU_ELEMENTID_START		200

#ifdef PLATFORM_WINDOWS
#define MAINWINDOW_CONNECTION_CLOSE_DELAY_MS			1500
#define MAINWINDOW_CONNECTION_STARTCRAWL_DELAY_MS		1500
#else
#define MAINWINDOW_CONNECTION_CLOSE_DELAY_MS			500
#define MAINWINDOW_CONNECTION_STARTCRAWL_DELAY_MS		800
#endif


enum
{
    ID_Quit = 10,
    ID_ReInit,
    ID_Help,
    ID_About,
    ID_SerialPort,
    ID_Baud,
    ID_Settings,
    ID_UploadStreamRate_Conservative,
    ID_UploadStreamRate_Standard,
    ID_UploadStreamRate_Fast,
    ID_UploadStreamRate_Reckless,
    ID_UploadStreamRate_Custom,
    ID_Output_Clear,
    ID_Output_Export,
    ID_PingTimer,
    ID_AutoInitTimer,
    ID_SiteButton,
    ID_RawInput,
    ID_ToggleRawInput,
    ID_MenuCrawlTimer,
    ID_CheckForUpdates,
    ID_AutoCheckForUpdates,

    ID_StateTrackingPeriod_Short,
    ID_StateTrackingPeriod_Standard,
    ID_StateTrackingPeriod_Long,
    ID_StateTracking_Disable,

};



MainWindow::MainWindow(const wxString& title, const wxPoint& pos,
		const wxSize& size) :
		wxFrame((wxFrame*) (NULL), -1, title, pos, size,
				wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL|wxNO_FULL_REPAINT_ON_RESIZE) ,
		menu_id_counter(MAINWINDOW_DYNAMICMENU_ELEMENTID_START),
		baud_rate(0),
		serial_port(""),
		suiwin_currently_showing((SUIWindow*)NULL),
		suiWinListSizer((wxBoxSizer*)NULL),
		awaiting_input(false),
		executing_request(false),
		last_interaction(0),
		current_menu_depth(0),
		upload_rate_delay_factor(UPLOAD_RATE_DELAYFACTOR_STANDARD)

{

	wxImage::AddHandler(new wxPNGHandler);
	initialize_images();
	initialize_help_strings();

	wxColour whitecol(DRUID_COLOUR_WHITE);
	wxColour lightFg(DRUID_COLOUR_FOREGROUND_LIGHT);
	wxColour darkFg(DRUID_COLOUR_FOREGROUND_DARK);
	wxColour lighBg(DRUID_COLOUR_BACKGROUND_LIGHT);
	wxColour darkBg(DRUID_COLOUR_BACKGROUND_DARK);


	SetBackgroundColour(darkBg);
	SetForegroundColour(lightFg);



	wxIcon logoIcon = getDRUIDIcon();
	SetIcon(logoIcon);

	/*
	SetIcon(wxIcon(wxT("logo.png")));
	*/

	/*
	up_icon = wxBitmap(wxT("up.png"), wxBITMAP_TYPE_PNG);
	cmd_icon = wxBitmap(wxT("command.png"), wxBITMAP_TYPE_PNG);
	submenu_icon = wxBitmap(wxT("submenu.png"), wxBITMAP_TYPE_PNG);
	*/

	up_icon = wxBitmap(*(_img_up));
	cmd_icon = wxBitmap(*_img_command);
	submenu_icon = wxBitmap(*_img_submenu);


	autoInitTimer = new wxTimer(this, ID_AutoInitTimer);
	autoInitTimer->Start(700, true);

	pingTimer = new wxTimer(this, ID_PingTimer);
	crawlMenusTimer = new wxTimer(this, ID_MenuCrawlTimer);


	// mainPanel = new wxPanel(this, wxID_ANY);

	// create menus
	buildMenus();
	// create toolbar
	buildToolbar();

	suiWinListSizer = new wxBoxSizer(wxVERTICAL);

	resetSUIWindows();
	SUIWindow * buttonWindow = getSUIWindow(DEFAULT_SUIWINDOW_UID);

	wxBoxSizer *topsizer = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer * commandInputSizer = new wxBoxSizer(wxHORIZONTAL);


	wxBoxSizer *bottomSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *outputSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *outputButtonsSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer *subsizer = new wxBoxSizer(wxVERTICAL);

	// suiWinListSizer->Add(buttonWindow, 2, wxEXPAND );

	// This is done with ScrollRate/VirtualSize in MyCanvas ctor now,
	// both should produce identical results.
	//m_canvas->SetScrollbars( 10, 10, 50, 100 );

	subsizer->Add(toolbar, 0, wxEXPAND);
	commandInputSizer->Add(suiWinListSizer, 3, wxEXPAND);

	input = new InputPanel(this, this);

	input->acceptInput(false);

	wxBoxSizer * inputSizer = new wxBoxSizer(wxVERTICAL);


	trackedstate = new TrackedStatePanel(this);

	inputSizer->Add(input, 1, wxEXPAND);
	inputSizer->Add(trackedstate, 1, wxEXPAND);

	commandInputSizer->Add(inputSizer, 1, wxEXPAND);

	subsizer->Add(commandInputSizer, 3, wxEXPAND);


	wxSize rawInputMinSize(280, 30);
	wxSize rawInputMaxSize(800, 30);
	raw_input = new wxTextCtrl(this, ID_RawInput, wxEmptyString,
			wxDefaultPosition, rawInputMinSize, wxTE_PROCESS_ENTER);

	raw_input->SetMinSize(rawInputMinSize);
	raw_input->SetMaxSize(rawInputMaxSize);
	raw_input->SetForegroundColour(darkFg);

	wxBoxSizer *rawInputSizer = new wxBoxSizer(wxHORIZONTAL);

	raw_input_label = new wxStaticText(this, wxID_ANY, wxT(" >") );
	rawInputSizer->Add(raw_input_label, 0, wxALIGN_CENTER );

    rawInputSizer->Add(raw_input,
			wxSizerFlags(1).Expand().Border(wxALL, 5));




    wxFont txtCtrlFont(12, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

    // wxFont(int pointSize, wxFontFamily family, int style, wxFontWeight weight,);

    bottomSizer->Add(rawInputSizer,
    			wxSizerFlags(0).Expand().Border(wxRIGHT, 120));

	outputTextCtrl = new wxTextCtrl(this, wxID_ANY, wxT(""),
			wxDefaultPosition, wxSize(280, 90), wxTE_MULTILINE);

	outputTextCtrl->SetForegroundColour(darkFg);
	outputTextCtrl->SetFont(txtCtrlFont);
	outputSizer->Add(outputTextCtrl, wxSizerFlags(5).Expand().Border(wxALL, 5));



	outputButtonsSizer->Add(new wxButton(this, ID_Output_Clear, wxT("Clear")),
			wxSizerFlags(0).Expand().Border(wxALL, 5));
	outputButtonsSizer->Add(new wxButton(this, ID_Output_Export, wxT("Export")),
			wxSizerFlags(0).Expand().Border(wxALL, 5));



	wxBitmap my_icon(*_img_logo);



#ifdef PLATFORM_LINUX
	wxBitmapButton * siteLogo = new wxBitmapButton(this, ID_SiteButton, my_icon);
	siteLogo->SetBitmapFocus(my_icon);
	siteLogo->SetBitmapLabel(my_icon);
	siteLogo->SetLabel(wxT("Druid"));
#else
	// windows can't seem to handle the bitmap button correctly, so we
	// just use a static bimap
	wxStaticBitmap * siteLogo = new wxStaticBitmap(this, ID_SiteButton, my_icon);

#endif

	siteLogo->SetBackgroundColour(darkBg);
	siteLogo->SetToolTip(wxT(DRUID4ARDUINO_APP_NAME));


	outputButtonsSizer->Add(siteLogo, wxSizerFlags(1).Expand().Border(wxALL, 5));

	outputSizer->Add(outputButtonsSizer,
			wxSizerFlags(1).Expand().Border(wxALL, 5));

	bottomSizer->Add(outputSizer, 1, wxEXPAND);
	subsizer->Add(bottomSizer, 2, wxEXPAND);

	topsizer->Add(subsizer, 1, wxEXPAND);

	showSUIWindow(buttonWindow);

	SetSizer(topsizer);

	// by default, hide all this
	raw_input_label->Show(false);
	raw_input->Show(false);



	// SetSizer(topsizer);
	CreateStatusBar();
	SetStatusText(wxT("Welcome to Druid4Arduino!"));


}


void MainWindow::touchLastInteraction()
{
	last_interaction = time(NULL);

}

void MainWindow::buildMenus()
{

	// FILE
	wxMenu* menuFile = new wxMenu;
	menuFile->Append(ID_ReInit, wxT("&Re Initialize"));
	menuFile->AppendSeparator();
	menuFile->Append(ID_CheckForUpdates, wxT("&Update Check"));
	auto_update_checks_toggle = menuFile->AppendCheckItem(ID_AutoCheckForUpdates, wxT("&Automatic Update Checks"));
	menuFile->Append(ID_Quit, wxT("E&xit"));


	loadConfig();

	if (automatic_update_checks)
		auto_update_checks_toggle->Check(true);
	else
		auto_update_checks_toggle->Check(false);


	// EDIT

	wxMenu* menuEdit = new wxMenu;
	menuEdit->Append(ID_Settings, wxT("Port &Settings"));

	wxMenu * menuUploadRate = new wxMenu;
	wxMenuItem * upload_rate_conservative = menuUploadRate->AppendRadioItem(ID_UploadStreamRate_Conservative, wxT("&Conservative"), wxT("Slow but sure uploads"));
	wxMenuItem * upload_rate_standard = menuUploadRate->AppendRadioItem(ID_UploadStreamRate_Standard, wxT("&Standard"), wxT("Standard upload rate"));
	wxMenuItem * upload_rate_fast = menuUploadRate->AppendRadioItem(ID_UploadStreamRate_Fast, wxT("&Fast"), wxT("Fast upload rate"));
	wxMenuItem * upload_rate_reckless = menuUploadRate->AppendRadioItem(ID_UploadStreamRate_Reckless, wxT("&Reckless"), wxT("Really fast upload rate"));
	wxMenuItem * upload_rate_custom; // may not be needed
	switch (upload_rate_delay_factor)
	{
	case UPLOAD_RATE_DELAYFACTOR_CONSERVATIVE:
		upload_rate_conservative->Check(true);
		break;
	case UPLOAD_RATE_DELAYFACTOR_STANDARD:
		upload_rate_standard->Check(true);
		break;

	case UPLOAD_RATE_DELAYFACTOR_FAST:
		upload_rate_fast->Check(true);
		break;
	case UPLOAD_RATE_DELAYFACTOR_RECKLESS:
		upload_rate_reckless->Check(true);
		break;

	default:
		upload_rate_custom = menuUploadRate->AppendRadioItem(ID_UploadStreamRate_Custom, wxT("C&ustom"),
				wxT("Custom upload rate"));
		upload_rate_custom->Check(true);
		break;
	}
	menuEdit->Append(wxID_ANY, wxT("&Upload Rate"), menuUploadRate);



	wxMenu * menuStateTrackingPeriod = new wxMenu;
		wxMenuItem * ping_period_short = menuStateTrackingPeriod->AppendRadioItem(ID_StateTrackingPeriod_Short, wxT("&Short"), wxT("Short Period, fast state refresh"));
		wxMenuItem * ping_period_standard = menuStateTrackingPeriod->AppendRadioItem(ID_StateTrackingPeriod_Standard, wxT("S&tandard"), wxT("Standard wake-up calls/refresh"));
		wxMenuItem * ping_period_long = menuStateTrackingPeriod->AppendRadioItem(ID_StateTrackingPeriod_Long, wxT("&Long"), wxT("Slow refresh, low activity"));
		wxMenuItem * ping_disabled = menuStateTrackingPeriod->AppendRadioItem(ID_StateTracking_Disable, wxT("&Disable"), wxT("No state tracking"));

		switch (tracking_interval_seconds)
		{
		case TRACKING_PERIOD_SHORT:
			ping_period_short->Check(true);
			break;
		case TRACKING_PERIOD_STANDARD:
			ping_period_standard->Check(true);
			break;
		case TRACKING_PERIOD_LONG:
			ping_period_long->Check(true);
			break;
		case TRACKING_PERIOD_DISABLED:
			ping_disabled->Check(true);
			break;
		default:

			break;
		}
		menuEdit->Append(wxID_ANY, wxT("&Tracking Refresh Interval"), menuStateTrackingPeriod);









	raw_input_toggle = menuEdit->AppendCheckItem(ID_ToggleRawInput, wxT("&Raw Input"), wxT("Display the raw input field"));

	// HELP
	wxMenu* menuHelp = new wxMenu;
	menuHelp->Append(ID_Help, wxT("&Help..."));
	menuHelp->AppendSeparator();
	menuHelp->Append(ID_About, wxT("&About..."));

	deviceMenu = new wxMenu;



	// Entire menu
	menuBar = new wxMenuBar;
	menuBar->Append(menuFile, wxT("&File"));
	menuBar->Append(menuEdit, wxT("&Edit"));
	menuBar->Append(deviceMenu, wxT("&Device"));
	menuBar->Append(menuHelp, wxT("&Help"));


	SetMenuBar(menuBar);


}
void MainWindow::buildToolbar()
{



	/*
	wxBitmap exit_icon(wxT("exit.png"), wxBITMAP_TYPE_PNG);
	wxBitmap refresh_icon(wxT("refresh.png"), wxBITMAP_TYPE_PNG);
	wxBitmap help_icon(wxT("help.png"), wxBITMAP_TYPE_PNG);
	wxBitmap prop_icon(wxT("properties.png"), wxBITMAP_TYPE_PNG);
	*/

	wxBitmap exit_icon(*_img_exit);
	wxBitmap refresh_icon(*_img_refresh);
	wxBitmap help_icon(*_img_help);
	wxBitmap prop_icon(*_img_properties);


	toolbar = new wxToolBar(this, wxID_ANY);
	toolbar->AddTool(ID_Settings, prop_icon, wxT("Settings"));
	toolbar->AddTool(ID_ReInit, refresh_icon, wxT("Re-Initialize"));
	toolbar->AddTool(ID_Help, help_icon, wxT("Help"));
	toolbar->AddSeparator();
	toolbar->AddTool(ID_Quit, exit_icon, wxT("Exit application"));
	toolbar->Realize();
}


void MainWindow::resetSUIWindows()
{

	if (suiWinListSizer) {
		suiWinListSizer->Clear(true);
	}


	uid_to_window.clear();

    SUIWindow * defWin = newSUIWindow(DEFAULT_SUIWINDOW_UID, 0);
    if (defWin)
    {
    	defWin->setText(wxT("Connecting to device... one moment please"));
    	enableSUIWindow(DEFAULT_SUIWINDOW_UID);
    } else {
    	// TODO:FIXME report serious error!
    }
}


SUIWindow * MainWindow::newSUIWindow(DRUID::MenuUID forUID, unsigned int firstTabbableId)
{
	MenuUIDToSUIWindowMap::iterator findIter = uid_to_window.find(forUID);

	if (findIter != uid_to_window.end())
	{
		// already exists!
		return (SUIWindow*)NULL;
	}

	SUIWindow * aWin = new SUIWindow(this, firstTabbableId);
	uid_to_window[forUID] = aWin;

	suiWinListSizer->Add(aWin, 2, wxEXPAND);

	aWin->Hide();

	return aWin;
}
SUIWindow * MainWindow::getSUIWindow(DRUID::MenuUID uid)
{
	MenuUIDToSUIWindowMap::iterator findIter = uid_to_window.find(uid);

	if (findIter == uid_to_window.end()) {
		// not found!
		return (SUIWindow*) NULL;
	}

	return (*findIter).second;

}

bool MainWindow::showSUIWindow(SUIWindow * suiwin)
{

	suiwin_currently_showing = suiwin;
	suiwin_currently_showing->display();



	return true;

}

bool MainWindow::enableSUIWindow(DRUID::MenuUID uid)
{
	bool foundWindow = false;

	for (MenuUIDToSUIWindowMap::iterator iter = uid_to_window.begin();
			iter != uid_to_window.end();
			iter++)
	{
		if ((*iter).first == uid)
		{
			foundWindow = true;
			showSUIWindow((*iter).second);
		} else {
			((*iter).second)->Show(false);
		}
	}


	suiwin_currently_showing->Layout();
	Layout();
	return foundWindow;

}
SUIWindow * MainWindow::currentlyEnabledSUIWindown()
{
	return suiwin_currently_showing;
}



void MainWindow::OnReInit(wxCommandEvent& event) {

	doReInit();
}

void MainWindow::doReInit()
{
	static DRUID::ConnectionPackagePtr nullConnection;

	resetStatusBar();

	trackedstate->clearDisplay();

	if (automatic_update_checks && (rand() % 127) > 109)
		availableUpdateCheck(true);



	// if we already have a connection, close that down and come back later
	if (connection)
	{
		pingTimer->Stop();
		connection->destroy();
		connection = nullConnection;

		SetStatusText(wxT("Closing current connection"));
		autoInitTimer->Start(MAINWINDOW_CONNECTION_CLOSE_DELAY_MS, true);

		return;

	}


	// no matter what, if we get here we (no longer?) have a connection and
	// need to init one...
	if (!loadConfig())
	{
		// first time use
		if (! doGetSettings(true))
		{
			currentlyEnabledSUIWindown()->setError(wxT("Did not specify serial port, please do so prior to initializing."));
			SetStatusText(wxT("Could not automatically set serial port"));
			return;

		}

	}

	// figure out--or request--the port to connect to
	DRUID::SerialEnumerator sEnumerator;

	if (serial_port.size() && sEnumerator.portExists(serial_port))
	{
		DRUID4ARDUINO_DEBUG("Connecting");
		wxString connMsg(wxT("Connecting to "));
		connMsg += DRUID_STDSTRING_TOWX(serial_port);
		SetStatusText(connMsg);
	} else {

		DRUID::PortNameList ports = sEnumerator.listPorts();

		if (! ports.size())
		{
			currentlyEnabledSUIWindown()->setError(wxT("Cannot automatically determine serial port -- specify please."));
			SetStatusText(wxT("Could not automatically set serial port"));
			return;
		} else if (ports.size() > 1)
		{
			currentlyEnabledSUIWindown()->setError(wxT("Cannot automatically determine serial port -- multiple options available, please select."));

			SetStatusText(wxT("Could not automatically set serial port"));

		} else {
			// 1 port avail... try that one.
			serial_port = ports[0];
			std::string statusTxt("Automatic connection to port ");
			statusTxt += serial_port;
			SetStatusText(DRUID_STDSTRING_TOWX(statusTxt));
		}
	}


	// ok, actually get the connection
	connection = DRUID::Util::getConnection(baud_rate, serial_port);

	if (! connection)
	{
		currentlyEnabledSUIWindown()->setError(wxT("Could not create new connection package"));
		DRUID4ARDUINO_DEBUG("Could not create new connection package");
		SetStatusText(wxT("Could not create new connection package"));
		return;
	}

	if (! connection->active())
	{

		wxString errMsg(wxT("Could not connect to "));
		errMsg += DRUID_STDSTRING_TOWX(serial_port);
		errMsg += wxT("\r\nEnsure a valid SerialUI device is connected, and confirm settings.");
		currentlyEnabledSUIWindown()->setError(errMsg);
		SetStatusText(errMsg);
		return;

	}

	// we have an active connection now


	crawlMenusTimer->Start(MAINWINDOW_CONNECTION_STARTCRAWL_DELAY_MS, true);


	return;
}

void MainWindow::OnMenuCrawlTimer(wxTimerEvent & event)
{
	// our crawl timer has triggered, time to gets goin'

	static const std::string clStr("\r");
	static const std::string eolStr("\n");

	// wake up the device.
	DRUID::SerialUIUserPtr serial_user = connection->serialUser();
	serial_user->send(eolStr);
	// PLATFORM_SLEEP(1);


	if(connection->ping(3, false))
	{
		DRUID4ARDUINO_DEBUG2("Device seems to be alive on ", serial_port);

		//= ;
		std::string inBuf(serial_user->incomingBuffer());

		std::string name;
		std::string::iterator findIter = std::search(inBuf.begin(), inBuf.end(),
				clStr.begin(), clStr.end());

		if (findIter == inBuf.end())
		{
			findIter = std::search(inBuf.begin(), inBuf.end(),
				eolStr.begin(), eolStr.end());
		}

		if (findIter != inBuf.end()) {

			// copy the newly arrived message to last_message
			name.reserve(findIter - inBuf.begin());
			std::copy(inBuf.begin(), findIter, std::back_inserter(name));
		} else {
			name = inBuf;
		}

		prog_name = DRUID_STDSTRING_TOWX(name);
		resetStatusBar();

	} else {

		wxString errMsg(wxT("No response to ping for 3 seconds on "));
		errMsg += DRUID_STDSTRING_TOWX(serial_port);
		SetStatusText(errMsg);
	}



	// parse the menu
	if (serial_user->isConnected())
	{



		resetSUIWindows();
		if (topLevelMenu)
		{
			// clear out the window menu and its callbacks

			for (unsigned int elementId=MAINWINDOW_DYNAMICMENU_ELEMENTID_START;
					elementId < menu_id_counter; elementId++)
			{
				this->Disconnect(elementId, wxEVT_COMMAND_MENU_SELECTED);
			}

			for (unsigned int i=0; i < topLevelMenu->size(); i++)
			{
				// TODO: must we destroy the returned wxMenuItem?
				deviceMenu->Remove(deviceMenu->FindItemByPosition(0));
			}


		}

		parser.setControlStrings(serial_user->enterProgramMode());
		executing_request = true;
		topLevelMenu = parser.crawl(serial_user);

		if (topLevelMenu)
		{
			// ok, we have our menu structure
			parser.dumpMenus();
			buildAllDeviceMenus(topLevelMenu);

		} else {
			// TODO:FIXME return error
			currentlyEnabledSUIWindown()->setError(wxT("Parser could not crawl menus\r\nEnsure a valid SerialUI device is connected."));
			SetStatusText(wxT("Parser could not crawl menus"));
		}

		serial_user->clear();
		executing_request = false;
		pingTimer->Start(200, false);
	} else {
		SetStatusText(wxT("Serial connection failure"));
	}


}


void MainWindow::buildAllDeviceMenus(DRUID::MenuPtr topLevelMenu)
{
	menu_id_counter = MAINWINDOW_DYNAMICMENU_ELEMENTID_START;


	buildDeviceMenu(topLevelMenu, deviceMenu);

	enableSUIWindow(topLevelMenu->uid());

	/*currentlyEnabledSUIWindown()->innerSizer()->Layout();
	currentlyEnabledSUIWindown()->Layout();
	*/
	this->Layout();


}

void MainWindow::buildDeviceMenu(DRUID::MenuPtr sguiMenu, wxMenu * intoMenu)
{
	wxColour darkFg(DRUID_COLOUR_FOREGROUND_DARK);

	wxSize btnMinSize(60, 40);
	wxSize btnMaxSize(230, 180);

	MenuHotKeysUsedMap hotKeys;

	SUIWindow* buttonWindow = newSUIWindow(sguiMenu->uid(), menu_id_counter);

	wxBoxSizer * menuSizer = new wxBoxSizer(wxVERTICAL);


	DRUID::MenuPtr parentMenu = sguiMenu->parent();
	unsigned int numRows = sguiMenu->size();

	if (parentMenu)
	{
		numRows += 2;
	}

	wxGridSizer * btnSizer = new wxGridSizer(numRows, 2, 0, 0);




	btnSizer->SetMinSize(400, 200);

	if (! parentMenu)
	{
		menuSizer->Add(new wxStaticText( buttonWindow, wxID_ANY, wxT("  ") ),
				                    0,
				                    wxALIGN_CENTER );
	} else {

		menuSizer->Add(new wxStaticText( buttonWindow, wxID_ANY, DRUID_STDSTRING_TOWX(sguiMenu->name())),
		                    0,
		                    wxALIGN_CENTER );

		wxString upStr(wxT("Back to "));
		upStr += DRUID_STDSTRING_TOWX(parentMenu->name());
		wxButton * btn = new wxButton(buttonWindow, menu_id_counter++, upStr);
		btn->SetHelpText(upStr);
		btn->SetForegroundColour(darkFg);
		btn->SetMinSize(btnMinSize);
		btn->SetMaxSize(btnMaxSize);


		btn->Connect(wxID_ANY, wxEVT_COMMAND_BUTTON_CLICKED,
							wxCommandEventHandler(MainWindow::OnUpMenu),
									new UpLevelClickUserData(sguiMenu),
									this
							);


		wxBoxSizer * btnLabelSizer = new wxBoxSizer(wxHORIZONTAL);
		btnLabelSizer->Add(new wxStaticBitmap(buttonWindow, wxID_ANY, up_icon),
				wxSizerFlags(0).Align(wxALIGN_CENTER));

		btnLabelSizer->Add(btn, wxSizerFlags(1).Expand().Align(wxALIGN_CENTER));

		btnSizer->Add(btnLabelSizer, wxSizerFlags(1).Expand().Align(wxALIGN_CENTER));
		btnSizer->Add(new wxStaticText(buttonWindow, wxID_ANY, wxT(" ")),
						wxSizerFlags(3).Align(wxALIGN_CENTER).Border(
								wxLEFT, 5));

		btnSizer->AddSpacer(0);
		btnSizer->AddSpacer(0);

	}

	for (unsigned int i = 0; i < sguiMenu->size(); i++)
	{
		unsigned int elementId = menu_id_counter++;

		DRUID::MenuItemPtr menuItem = sguiMenu->item(i);


		wxString labelWxStr(menuItem->key.c_str(), wxConvUTF8);
		wxString helpWxStr(menuItem->help.c_str(), wxConvUTF8);

		std::string itemKey = menuItemKeyToHotKey(menuItem->key, &hotKeys);
		wxMenuItem * mItem = new wxMenuItem(intoMenu, elementId,
				DRUID_STDSTRING_TOWX(itemKey), helpWxStr);

		wxButton * btn = new wxButton(buttonWindow, elementId, labelWxStr);
		btn->SetHelpText(helpWxStr);
		btn->SetForegroundColour(darkFg);
		btn->SetMinSize(btnMinSize);
		btn->SetMaxSize(btnMaxSize);

		wxBoxSizer * btnLabelSizer = new wxBoxSizer(wxHORIZONTAL);

		if (!i)
			btn->SetFocus();

		if (menuItem->type == DRUID::MenuItem_Command) {
			// connect to callbacks for commands

			btnLabelSizer->Add(new wxStaticBitmap(buttonWindow, wxID_ANY, cmd_icon),
					wxSizerFlags(0).Align(wxALIGN_CENTER));

			this->Connect(elementId, wxEVT_COMMAND_MENU_SELECTED,
					wxCommandEventHandler(MainWindow::OnExecuteMenuItem),
					new MenuItemClickUserData(sguiMenu, menuItem), this);

			btn->Connect(wxID_ANY, wxEVT_COMMAND_BUTTON_CLICKED,
					wxCommandEventHandler(MainWindow::OnRunCommand),
					new ButtonClickUserData(menuItem), this);

		} else if (menuItem->type == DRUID::MenuItem_Submenu) {
			btnLabelSizer->Add(new wxStaticBitmap(buttonWindow, elementId, submenu_icon),
					wxSizerFlags(0).Align(wxALIGN_CENTER));

			wxMenu * theSubMenu = new wxMenu(DRUID_STDSTRING_TOWX(menuItem->subMenu->name()));
			mItem->SetSubMenu(theSubMenu);

			btn->Connect(wxID_ANY, wxEVT_COMMAND_BUTTON_CLICKED,
					wxCommandEventHandler(MainWindow::OnEnterSubMenu),
					new ButtonClickUserData(menuItem), this);


			buildDeviceMenu(menuItem->subMenu, theSubMenu);

		} else {
			// ??
		}

		btn->SetMinSize(wxSize(70, 35));

		btnLabelSizer->Add(btn,
						wxSizerFlags(1).Expand().Align(
								wxALIGN_CENTER));
		btnSizer->Add(btnLabelSizer, wxSizerFlags(1).Expand().Align(
				wxALIGN_CENTER));

		wxStaticText * helptxt = new wxStaticText(buttonWindow, wxID_ANY, helpWxStr);
		helptxt->Wrap(225);

		btnSizer->Add(helptxt,
				wxSizerFlags(3).Align(wxALIGN_CENTER_VERTICAL).Border(wxLEFT, 5));



		intoMenu->Append(mItem);
	}

	menuSizer->Add(btnSizer, wxSizerFlags(1).Border(wxALL & ~wxBottom, 5));

	buttonWindow->innerSizer()->Add(menuSizer, wxSizerFlags(1).Border(wxALL & ~wxBottom, 5));

}

std::string MainWindow::menuItemKeyToHotKey(std::string & key, MenuHotKeysUsedMap * used_keys)
{
	std::string retKey;
	retKey.reserve(key.length() + 1);

	std::string curChar;
	std::string lowerCurChar;

	for (std::string::iterator iter = key.begin(); iter != key.end(); iter++) {

		curChar.clear();
		curChar += (*iter);
		lowerCurChar = curChar;
		boost::algorithm::to_lower(lowerCurChar);
		if (used_keys->find(lowerCurChar) == used_keys->end()) {
			// got it -- this key has never been used

			// note it -- don't re-use it
			(*used_keys)[lowerCurChar] = true;

			// set it -- not it in the return string by prefixing '&'
			retKey += "&";

			// copy the char, and the rest of the string, into our result string
			std::copy(iter, key.end(), std::back_inserter(retKey));

			// and return it immediately -- our work here is done!
			return retKey;

		}

		// get here: this hot-key already used here...
		// add it to the return key and go to next
		retKey += curChar;
	}

	return retKey;

}

void MainWindow::resetStatusBar()
{

	SetStatusText(prog_name);

}


void MainWindow::OnAutoInitTimer(wxTimerEvent & event)
{
	doReInit();
}
void MainWindow::OnPingTimer(wxTimerEvent & event)
{


	if (! (executing_request || awaiting_input))
	{

		DRUID::SerialUIUserPtr serial_user = connection->serialUser();
		DRUIDString inbufStr = serial_user->getAndClearBufferedMessage();
		if (inbufStr.size())
		{
			DRUID_DEBUG2("******** PING appending buffer: ", inbufStr);

			outputTextCtrl->AppendText(DRUID_STDSTRING_TOWX(inbufStr));
		}

		if (serial_user->messageReceived())
		{
			inbufStr = serial_user->lastMessage();
			if (inbufStr.size())
			{
				serial_user->lastMessageClear();
				DRUID_DEBUG2("******** PING appending lastMessage: ", inbufStr);

				outputTextCtrl->AppendText(DRUID_STDSTRING_TOWX(inbufStr));
			}
		}

		time_t time_now = time(NULL);

		int ping_interval = (tracking_interval_seconds == TRACKING_PERIOD_DISABLED) ? TRACKING_PERIOD_STANDARD : tracking_interval_seconds;
		if (time_now - last_interaction >= ping_interval)
		{
			sendPing(serial_user);
			last_interaction = time_now;

		}

	}

}
bool MainWindow::useStateTracking()
{
	return (tracking_interval_seconds != TRACKING_PERIOD_DISABLED);
}
void MainWindow::sendPing(DRUID::SerialUIUserPtr serial_user)
{
	if (connection->active())
	{
		if (executing_request)
			return;

		executing_request = true;
		bool state_track = useStateTracking();
		connection->ping(1, state_track);
		if (state_track && serial_user->numTrackedVariables())
		{
			SUIUserIdxToTrackedStateVariablePtr updatedStates = serial_user->updatedTrackedVariables();
			trackedstate->updateDisplay(updatedStates);
		}
		executing_request = false;
	}
}
void MainWindow::OnQuit(wxCommandEvent&) {
	doQuit();

}

void MainWindow::OnAbout(wxCommandEvent& evt)
{

	wxString msg(wxT("Druid4Arduino v1.3.1 Copyright (C) 2013,2014 Pat Deegan, psychogenic.com.\r\n\r\n"));

	msg += wxT("This program comes with ABSOLUTELY NO WARRANTY;\r\n");
	msg += wxT("This is free software, and you are welcome to redistribute it ");
	msg += wxT("under certain conditions (see the accompanying LICENSE file or refer to the GNU GPL v.3)\r\n\r\n");

	msg += wxT("http://flyingcarsandstuff.com/projects/druid4arduino/");

	wxMessageBox(msg,
                 wxT("About Druid4Arduino"), wxOK | wxICON_INFORMATION );


}

void MainWindow::availableUpdateCheck(bool showOnlyPositives)
{

	char curVersionStr[] = DRUID4ARDUINO_VERSION_STRING();


	wxMessageDialog * msgDial = NULL;

	wxString versionURLStr(wxT(DRUID4ARDUINO_LATESTVERSION_URL));
	wxURL versionURL(versionURLStr);


	wxString dialogTitle(wxT("Version Check"));
	if (versionURL.GetError() != wxURL_NOERR)
	{
		if (! showOnlyPositives)
			msgDial = new wxMessageDialog(this, wxT("Could not connect to check version"),
					dialogTitle, wxOK | wxICON_ERROR);
	} else {
		// no error connecting...
		DRUID4ARDUINO_DEBUG("Connecting to verion #...");
		wxInputStream *in_stream;

		in_stream = versionURL.GetInputStream();

		if (! in_stream)
		{

			if (! showOnlyPositives)
				msgDial = new wxMessageDialog(this, wxT("Could not read (remote) version info"),
					dialogTitle, wxOK | wxICON_ERROR);

		} else {
			char buf[20];
			while (! in_stream->Eof())
			{
				in_stream->Read(buf, 20);
			}

			if (strncmp(curVersionStr, buf, strlen(curVersionStr)) == 0)
			{

				if (! showOnlyPositives)
					msgDial = new wxMessageDialog(this, wxT("Druid4Arduino is up to date!"),
						dialogTitle, wxOK | wxICON_INFORMATION);
			} else {

				wxString notifMsg(wxT("A newer version is available, upgrade to "));
				notifMsg += wxString(buf, wxConvUTF8);
				notifMsg += wxT(" at ");
				notifMsg += wxString(_T(DRUID4ARDUINO_SITE_URL));

				msgDial = new wxMessageDialog(this, notifMsg,
						dialogTitle, wxOK | wxICON_EXCLAMATION);
			}
		}

	} // end if we could get the URL

	if (msgDial != NULL)
		msgDial->ShowModal();
}
void MainWindow::OnCheckForUpdates(wxCommandEvent& evt)
{

	availableUpdateCheck(false);

}

void MainWindow::moveToTopLevelMenu()
{
	DRUID::SerialUIUserPtr serial_user = connection->serialUser();
	while (current_menu_depth && serial_user->upMenuLevel())
	{
		current_menu_depth--;
	}
}
void MainWindow::OnExecuteMenuItem(wxCommandEvent& event)
{

	MenuItemClickUserData * userData = (MenuItemClickUserData*)(event.m_callbackUserData);

	DRUID::SerialUIUserPtr serial_user = connection->serialUser();

	if (userData->menu_item->type != DRUID::MenuItem_Command)
	{
		currentlyEnabledSUIWindown()->setError(wxT("OnExecuteMenuItem called with a sub-menu item?"));
		return;
	}

	moveToTopLevelMenu();
	enableSUIWindow(topLevelMenu->uid());

	DRUID::MenuPtr curMenu = userData->in_menu;
	if (curMenu->parent())
	{

		std::list<DRUIDString> menuKeyList;
		//  has parent (i.e. is a sub-menu)


		do {

			menuKeyList.push_front(curMenu->parentKey());


			curMenu = curMenu->parent();
		} while (curMenu);

		// now go down the menu tree
		for (std::list<DRUIDString>::iterator iter = menuKeyList.begin();
				iter != menuKeyList.end();
				iter++)
		{
			if (!serial_user->sendAndReceive((*iter)))
			{
				if (serial_user->hasError())
				{

					wxString errMsg(serial_user->errorMessage().c_str(), wxConvUTF8);
					SetStatusText(errMsg);
				} else {
					wxString errMsg(wxT("Problem encountered entering sub-menu:"));
					errMsg += DRUID_STDSTRING_TOWX((*iter));
					SetStatusText(errMsg);
				}

				return;
			} else {

				current_menu_depth++;
			}
		}

	}

	enableSUIWindow(userData->in_menu->uid());
	executeCommand(userData->menu_item->key);






}

void MainWindow::OnUpMenu(wxCommandEvent& event)
{

	if (awaiting_input)
	{
		showAwaitingInputDialog();
		return;
	}


	UpLevelClickUserData * userData = (UpLevelClickUserData*)(event.m_callbackUserData);

	DRUID::SerialUIUserPtr serial_user = connection->serialUser();


	touchLastInteraction();
	if (! doUpMenu(serial_user))
		return;


	DRUIDString lastMsg(serial_user->lastMessage());
	DRUID_DEBUG2("******** executing command appending buffer: ", lastMsg);

	outputTextCtrl->AppendText(DRUID_STDSTRING_TOWX(lastMsg));
	enableSUIWindow(userData->cur_menu->parent()->uid());

	SetStatusText(DRUID_STDSTRING_TOWX(userData->cur_menu->parent()->name()));

}


bool MainWindow::doUpMenu(DRUID::SerialUIUserPtr serial_user)
{
	if ( (! serial_user->upMenuLevel()) || serial_user->hasError())
		{

			wxString errMsg(serial_user->errorMessage().c_str(), wxConvUTF8);
			currentlyEnabledSUIWindown()->setError(errMsg);
			return false;
		}


	if (current_menu_depth)  {
		current_menu_depth--;
	}

	return true;
}
void MainWindow::OnEnterSubMenu(wxCommandEvent& event)
{


	if (awaiting_input)
	{
		showAwaitingInputDialog();
		return;
	}

	ButtonClickUserData * userData = (ButtonClickUserData*)(event.m_callbackUserData);
	if (userData->menu_item->type != DRUID::MenuItem_Submenu)
	{
		currentlyEnabledSUIWindown()->setError(wxT("OnChangeEnterSubMenu called without a sub-menu item?"));
		return;
	}

	if (executeCommand(userData->menu_item->key))
	{
		current_menu_depth++;
		enableSUIWindow(userData->menu_item->subMenu->uid());
		SetStatusText(DRUID_STDSTRING_TOWX(userData->menu_item->subMenu->name()));
	}


}
void MainWindow::OnRunCommand(wxCommandEvent& event)
{


	if (awaiting_input)
	{
		showAwaitingInputDialog();
		return;
	}

	ButtonClickUserData * userData = (ButtonClickUserData*)(event.m_callbackUserData);


	executeCommand(userData->menu_item->key);
}



#define MW_RETURNFROM_EXECUTECOMMAND(val)		\
	executing_request = false; \
	return (val);

bool MainWindow::executeCommand(const DRUIDString & command)
{
	static const wxString runStatus(wxT("Executing command"));
	static const wxString inputRequired(wxT("Input Required..."));
	static const wxString integerInputRequired(wxT("Integer input Required..."));
	static const wxString numericInputRequired(wxT("Numeric input Required..."));
	static const wxString runStatusDone(wxT(" completed"));

	SetStatusText(runStatus);


	uint8_t wait_attempts=0;
	while (executing_request && wait_attempts++ < 5)
		usleep(1000);


	touchLastInteraction();
	DRUID::SerialUIUserPtr serial_user = connection->serialUser();


	DRUID_DEBUG2("Doing send+rcv for", command);

	executing_request = true;
	if ( (! serial_user->sendAndReceive(command)) || serial_user->hasError())
	{
		wxString errMsg(DRUID_STDSTRING_TOWX(serial_user->errorMessage()));

		SetStatusText(errMsg);
		MW_RETURNFROM_EXECUTECOMMAND(false);

	}

	awaiting_input = false;

	wxString cmdName(DRUID_STDSTRING_TOWX(command));
	if (serial_user->inputRequired())
	{

		serial_user->flushReceiveBuffer();
		serial_user->setAutoReplaceLastMessage(false);

		{ // may have more than a single input to enter, so we do/while it...

			DRUID_DEBUG("Need input...");
			oldFocus = this->FindFocus();
			awaiting_input = true;
			DRUID::UserInputType reqType = serial_user->inputRequiredType();

			// get the name from the prompt, if possible.
			if (serial_user->inputRequiredPromptString().size()) {
				cmdName =
						DRUID_STDSTRING_TOWX(serial_user->inputRequiredPromptString());
			}

			bool streamSuccess;
			switch (reqType) {
			case DRUID::InputType_Numeric:
				DRUID_DEBUG("Numeric input required");
				SetStatusText(numericInputRequired);
				break;

			case DRUID::InputType_Integer:
				DRUID_DEBUG("Numeric (int) input required");
				SetStatusText(integerInputRequired);
				break;

			case DRUID::InputType_Stream:

				DRUID_DEBUG("Want a stream (file) upload");

				// little bit different, this one... we need to select a file and stream its contents in...

				streamSuccess = sendFileStream();
				// outputTextCtrl->AppendText(DRUID_STDSTRING_TOWX(serial_user->lastMessage()));

				awaiting_input = false;
				MW_RETURNFROM_EXECUTECOMMAND(streamSuccess);
				break;

			default:
				DRUID_DEBUG("String input required");
				SetStatusText(inputRequired);
				break;

			}


			input->acceptInput(true, cmdName, reqType);


		}

	} else {

		wxString stat(cmdName);
		stat += runStatusDone;
		SetStatusText(stat);
		if (serial_user->messageReceived())
		{
			outputTextCtrl->AppendText(DRUID_STDSTRING_TOWX(serial_user->lastMessage()));

			serial_user->lastMessageClear();
		}


		serial_user->setAutoReplaceLastMessage(true);

		if (serial_user->requestedTerminate())
		{

			SetStatusText(wxT("GUI Termination requested"));

			// TODO: FIXME -- show user we aren't just dying but terminating on purpose.
			doQuit();

			MW_RETURNFROM_EXECUTECOMMAND(true);

		}


		if (useStateTracking() && serial_user->numTrackedVariables())
		{
			last_interaction = 0;
		}

	}


	MW_RETURNFROM_EXECUTECOMMAND(true);


}

#define MAINWINDOW_SENDSTREAM_BUFFER_SIZE		24

bool MainWindow::sendFileStream()
{
	const DRUIDString cancelStreaming("0");
	uint8_t sendBuffer[MAINWINDOW_SENDSTREAM_BUFFER_SIZE];
	std::map<wxString, wxString> wildcardMap;
	wxString extension, defaultDir, defaultFile, wildcards;


	// setup wildcards and defaults, partially based on last uploaded file.
	wildcardMap[_T("txt")] = wxT("Text (*.txt)|*.txt");
	wildcardMap[_T("csv")] = wxT("CSV (*.csv)|*.csv");
	wildcardMap[_T("wav")] = wxT("WAVE (*.wav)|*.wav");
	wildcardMap[_T("html")] = wxT("HTML (*.html)|*.html");

	if (last_uploaded_filepath.Length())
	{
		wxFileName fname(last_uploaded_filepath);

		defaultDir = fname.GetPath();
		defaultFile = fname.GetFullName();
		extension = fname.GetExt().Lower();

		if (extension.Length())
		{
			// see if this extension is present
			std::map<wxString, wxString>::iterator findExtIter = wildcardMap.find(extension);
			if (findExtIter == wildcardMap.end())
			{
				// nope, not here... add it
				wildcardMap[extension] = extension + wxT(" files (*.") + extension + _T(")|*.") + extension ;
			}
		}

	}

	for (std::map<wxString, wxString>::iterator wcIter = wildcardMap.begin();
			wcIter != wildcardMap.end(); wcIter++)
	{
		wildcards += (*wcIter).second + _T("|");
	}
	// add the all files option
	wildcards += wxT("All files|*.*");


	DRUID_DEBUG2("WILDCARDS IS ", wildcards.mb_str());


	DRUID::SerialUIUserPtr serial_user = connection->serialUser();


	wxFileDialog * selectFileDialog = new wxFileDialog(this,
			wxT("Select file to use for upload"), defaultDir,
			defaultFile, wildcards,
			wxFD_OPEN | wxFD_FILE_MUST_EXIST);



	if (selectFileDialog->ShowModal() != wxID_OK) {
		serial_user->sendAndReceive(cancelStreaming, true);
		return false;
	}



	wxString fileName = selectFileDialog->GetPath();

	configUpdateLastUploadedFilePath(fileName);


	wxFile fileHandle;
	if (!fileHandle.Open(fileName.c_str(), wxFile::read)) {
		wxString errMsg(wxT("Could not open file "));
		errMsg += fileName;
		wxMessageDialog *dial = new wxMessageDialog(NULL, errMsg,
				wxT("Can't read file"), wxOK | wxICON_ERROR);
		dial->ShowModal();
		serial_user->sendAndReceive(cancelStreaming, true);
		return false;
	}

	wxFileOffset fileLen = fileHandle.Length();

	if (fileLen < 1)
	{
		// forget this
		serial_user->sendAndReceive(cancelStreaming, true);
		return false;
	}

	DRUIDString lenAsStr;
	try {
		lenAsStr = boost::lexical_cast<DRUIDString>(fileLen);
	} catch (boost::bad_lexical_cast & e)
	{
		// could not cast string??
		serial_user->sendAndReceive(cancelStreaming, true);
		return false;
	}

	// seems it worked... send len:
	serial_user->send(lenAsStr, true);


	size_t curPos = 0;

	serial_user->setAutoReplaceLastMessage(false);
	serial_user->checkForLastMessage();
	// outputTextCtrl->AppendText(DRUID_STDSTRING_TOWX(serial_user->lastMessage()));
	serial_user->flushReceiveBuffer();
	bool curEOTChecks = serial_user->eotChecks();
	serial_user->setEotChecks(false);
	serial_user->setAutoReplaceLastMessage(false);

	DRUID_DEBUG2("Reading file of len ", fileLen);

	// wait a while between each send, to allow for sending and processing
	// set a wait time, per byte, related to baud rate

	size_t waitTimePerByte = upload_rate_delay_factor * 8 * 1000000  / baud_rate ;

	DRUID_DEBUG2("Will wait between block sends (us/byte):", waitTimePerByte);


	wxString statusSuffixStr(_T("/") + DRUID_STDSTRING_TOWX(lenAsStr));
	wxString statusPosStr;
	{
		wxBusyCursor wait;
		DRUIDString curPosStr;

		while (curPos < fileLen) {
			DRUID_DEBUG2("*** reading from pos ", curPos);

			size_t numToRead =
					(MAINWINDOW_SENDSTREAM_BUFFER_SIZE < (fileLen - curPos)) ?
							MAINWINDOW_SENDSTREAM_BUFFER_SIZE :
							(fileLen - curPos);

			size_t numRead = fileHandle.Read(sendBuffer, numToRead);
			if (numRead < 1) {
				break;
			}
			serial_user->send(sendBuffer, numRead);
			usleep(waitTimePerByte * numRead);

			curPos += numRead;

			try {
				curPosStr = boost::lexical_cast<DRUIDString>(curPos);
				statusPosStr = DRUID_STDSTRING_TOWX(curPosStr) + statusSuffixStr;
				SetStatusText(statusPosStr);

			} catch (boost::bad_lexical_cast & e)
			{

			}


			wxYield();
			touchLastInteraction();

		}
	}

	statusPosStr += _T("  Complete !");
	SetStatusText(statusPosStr);

	fileHandle.Close();

	serial_user->setEotChecks(curEOTChecks);

	// serial_user->sendAndReceive("", true);
	serial_user->checkForLastMessage();
	outputTextCtrl->AppendText(DRUID_STDSTRING_TOWX(serial_user->lastMessage()));


	serial_user->setAutoReplaceLastMessage(true);

	return true;
}

void MainWindow::OnOutputClear(wxCommandEvent& event)
{

	outputTextCtrl->Clear();


}
void MainWindow::OnOutputExport(wxCommandEvent& event)
{
	wxString saveCancelStat(wxT("Export cancelled"));


	if (outputTextCtrl->GetNumberOfLines() < 2)
	{

		wxMessageDialog *dial = new wxMessageDialog(NULL, wxT("No content to export..."),
						wxT("Nothing to do"), wxOK | wxICON_ERROR);
		dial->ShowModal();
		return;
	}


	wxFileDialog * saveFileDialog = new wxFileDialog(this,
			wxT("Select file to use for export"), wxEmptyString,
			wxEmptyString, wxT("Text files (*.txt)|*.txt"),
			wxFD_SAVE | ~wxFD_FILE_MUST_EXIST);

	if (saveFileDialog->ShowModal() != wxID_OK) {

		SetStatusText(saveCancelStat);
		return;
	}

	wxString fileName = saveFileDialog->GetPath();
	wxString statusTxt(wxT("Exporting to "));
	statusTxt += fileName;
	SetStatusText(statusTxt);

	wxFile fileHandle;

	/*
	if (fileHandle.Exists(fileName.c_str())) {
		// file exists!


		wxMessageDialog *dial = new wxMessageDialog(NULL,
				wxT("File exists, overwrite?"), wxT("File exists"),
				wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION);
		if (dial->ShowModal() == wxID_NO) {
			SetStatusText(saveCancelStat);
			return;

		}

	}
	*/



	if (!fileHandle.Open(fileName.c_str(), wxFile::write)) {
		wxString errMsg(wxT("Could not write to "));
		errMsg += fileName;
		wxMessageDialog *dial = new wxMessageDialog(NULL, errMsg,
				wxT("Can't write to file"), wxOK | wxICON_ERROR);
		dial->ShowModal();
		return;
	}

	for (long int i = 0; i < outputTextCtrl->GetNumberOfLines(); i++) {
		fileHandle.Write(outputTextCtrl->GetLineText(i));
		fileHandle.Write(wxT("\r\n"));
	}

	fileHandle.Close();


	SetStatusText(wxT("Output Exported"));



}

void MainWindow::inputReceivedFromPanel(wxString rcvdInput)
{
	static const wxString errorGeneric(wxT("Error sending input"));
	static const wxString inputReceived(wxT("Input sent"));

	std::string cmd(rcvdInput.mb_str());


	input->acceptInput(false);
	awaiting_input = false;


	if ( executeCommand(cmd))
	{

		SetStatusText(inputReceived);
	} else {

		// input->acceptInput(true);
		DRUID::SerialUIUserPtr serial_user = connection->serialUser();
		if (serial_user->hasError())
		{
			input->setError(DRUID_STDSTRING_TOWX(serial_user->errorMessage()));
		} else {
			SetStatusText(errorGeneric);
		}

		input->clearInputText();

	}


	// DRUID::SerialUIUserPtr serial_user = connection->serialUser();
	// serial_user->checkForLastMessage();
	// outputTextCtrl->AppendText(DRUID_STDSTRING_TOWX(serial_user->lastMessage()));

	if (oldFocus)
		oldFocus->SetFocus();


}

void MainWindow::showAwaitingInputDialog()
{

	  wxMessageDialog *dial = new wxMessageDialog(NULL,
	      wxT("Still awaiting input..."), wxT("Exclamation"),
	      wxOK | wxICON_EXCLAMATION);
	  dial->ShowModal();
	  input->sendFocusToTextArea();
}


bool MainWindow::doGetSettings(bool first_time_user)
{
	SettingsDialog * dial = new SettingsDialog(wxT("Settings"));

	if (baud_rate) {
		dial->setValues(serial_port, baud_rate);

	}
	SettingDetails dets = dial->getDetails(first_time_user);



	if (dets.valid) {
		bool updateInit = false;

		if (baud_rate != dets.baud || serial_port != dets.portName) {
			updateInit = true;
		}

		serial_port = dets.portName;
		baud_rate = dets.baud;

		saveConfig();

		return updateInit;
	}


	return false;

}
void MainWindow::OnSettings(wxCommandEvent& event)
{
	if (doGetSettings())
	{
		doReInit();
	}

}

void MainWindow::OnToggleRawInput(wxCommandEvent& event)
{
	bool  curSetting = false;
	if (raw_input_toggle->IsChecked())
	{
		curSetting = true;
	}
	//  raw_input_toggle->Check(curSetting);
	raw_input_label->Show(curSetting);
	raw_input->Show(curSetting);

	Layout();
}

void MainWindow::OnAutoUpdateCheck(wxCommandEvent& event)
{
	bool curSetting = false;
	if (auto_update_checks_toggle->IsChecked())
		curSetting = true;

	automatic_update_checks = curSetting;
	saveConfig();

}


void MainWindow::OnSelectUploadRateConservative(wxCommandEvent& event)
{

	upload_rate_delay_factor = UPLOAD_RATE_DELAYFACTOR_CONSERVATIVE;
	saveConfig();

}
void MainWindow::OnSelectUploadRateStandard(wxCommandEvent& event)
{

	upload_rate_delay_factor = UPLOAD_RATE_DELAYFACTOR_STANDARD;
	saveConfig();
}
void MainWindow::OnSelectUploadRateFast(wxCommandEvent& event)
{


	wxMessageDialog *dial = new wxMessageDialog(NULL,
			wxT("This upload rate may cause transmission failures if the device isn't fast enough to process the data."),
			wxT("Notice"), wxOK | wxICON_EXCLAMATION);
	dial->ShowModal();

	upload_rate_delay_factor = UPLOAD_RATE_DELAYFACTOR_FAST;
	saveConfig();

}


void MainWindow::OnSelectUploadRateReckless(wxCommandEvent& event)
{


	wxMessageDialog *dial = new wxMessageDialog(NULL,
			wxT("This upload rate will probably cause transmission failures unless the device is really fast."),
			wxT("Warning"), wxOK | wxICON_EXCLAMATION);
	dial->ShowModal();

	upload_rate_delay_factor = UPLOAD_RATE_DELAYFACTOR_RECKLESS;
	saveConfig();

}




void MainWindow::OnSelectStateTrackingPeriodShort(wxCommandEvent& event)
{

	tracking_interval_seconds = TRACKING_PERIOD_SHORT;
	saveConfig();
}
void MainWindow::OnSelectStateTrackingPeriodStandard(wxCommandEvent& event)
{

	tracking_interval_seconds = TRACKING_PERIOD_STANDARD;
	saveConfig();
}
void MainWindow::OnSelectStateTrackingPeriodLong(wxCommandEvent& event)
{

	tracking_interval_seconds = TRACKING_PERIOD_LONG;
	saveConfig();
}


void MainWindow::OnSelectStateTrackingDisable(wxCommandEvent& event)
{

	tracking_interval_seconds = TRACKING_PERIOD_DISABLED;
	saveConfig();
}



void MainWindow::saveConfig()
{
	static wxString baudRateStr(wxT(DRUID4ARDUINO_CONFIG_BAUDRATE));
	static wxString serialPortStr(wxT(DRUID4ARDUINO_CONFIG_SERIALPORT));
	static wxString uploadDelayFactorStr(wxT(DRUID4ARDUINO_CONFIG_UPLOADDELAYFACTOR));
	static wxString autoUpdateChecksStr(wxT(DRUID4ARDUINO_CONFIG_AUTOUPDATECHECKS));
	static wxString pingTimerChecksStr(wxT(DRUID4ARDUINO_CONFIG_PINGTIMERSECONDS));

	static std::string rCtor("moc.cinegohcysp ,nageeD taP )C( thgirypoC");

	  wxConfig *config = new wxConfig(wxT(DRUID4ARDUINO_APP_NAME));
	  wxString str;
	  if (! config->Read(wxT("creator"), &str))
	  {
		  // never created
		  std::string ctor = rCtor;
		  std::reverse(ctor.begin(), ctor.end());
		  config->Write(wxT("creator"), DRUID_STDSTRING_TOWX(ctor));
	  }
	  config->Write(baudRateStr, (int)baud_rate);
	  config->Write(serialPortStr, DRUID_STDSTRING_TOWX(serial_port));
	  config->Write(uploadDelayFactorStr, (int)upload_rate_delay_factor);
	  config->Write(autoUpdateChecksStr, automatic_update_checks);
	  config->Write(pingTimerChecksStr, tracking_interval_seconds);


	  delete config;
}
bool MainWindow::loadConfig()
{
	static wxString baudRateStr(wxT(DRUID4ARDUINO_CONFIG_BAUDRATE));
	static wxString serialPortStr(wxT(DRUID4ARDUINO_CONFIG_SERIALPORT));
	static wxString uploadDelayFactorStr(wxT(DRUID4ARDUINO_CONFIG_UPLOADDELAYFACTOR));
	static wxString lastUploadedFilePathStr(wxT(DRUID4ARDUINO_CONFIG_LASTUPLOADFILEPATH));
	static wxString autoUpdateChecksStr(wxT(DRUID4ARDUINO_CONFIG_AUTOUPDATECHECKS));
	static wxString pingTimerChecksStr(wxT(DRUID4ARDUINO_CONFIG_PINGTIMERSECONDS));


	wxConfig *config = new wxConfig(wxT(DRUID4ARDUINO_APP_NAME));
	wxString str;
	if (!config->Read(wxT("creator"), &str)) {
		delete config;
		return false;
	}

	if (!config->Read(baudRateStr, (int *)&baud_rate)) {
		delete config;
		return false;
	}
	if (!config->Read(serialPortStr, &str)) {
		delete config;
		return false;
	}
	serial_port = std::string(str.mb_str());


	if (! config->Read(uploadDelayFactorStr, (int *)&upload_rate_delay_factor))
	{
		upload_rate_delay_factor = UPLOAD_RATE_DELAYFACTOR_STANDARD;
	}

	config->Read(lastUploadedFilePathStr, &last_uploaded_filepath);

	if (! config->Read(autoUpdateChecksStr, &automatic_update_checks))
	{
		automatic_update_checks = true;
	}

	if (! config->Read(pingTimerChecksStr, &tracking_interval_seconds))
	{
		tracking_interval_seconds = TRACKING_PERIOD_STANDARD;
	}


	delete config;
	return true;

}

void MainWindow::configUpdateLastUploadedFilePath(wxString & filepath)
{
	static wxString lastUploadedFilePathStr(wxT(DRUID4ARDUINO_CONFIG_LASTUPLOADFILEPATH));
	if (! filepath.Length())
		return;

	last_uploaded_filepath = filepath;

	wxConfig *config = new wxConfig(wxT(DRUID4ARDUINO_APP_NAME));

	config->Write(lastUploadedFilePathStr, last_uploaded_filepath);

	delete config;

}


void MainWindow::OnSiteClick(wxCommandEvent& event)
{
	static const wxString siteURL(wxT(DRUID4ARDUINO_SITE_URL));

	wxLaunchDefaultBrowser(siteURL);
}


void MainWindow::OnHelp(wxCommandEvent& event)
{
	static bool seenHelp = false;

	if (! seenHelp)
	{

		wxImage::AddHandler(new wxJPEGHandler);
		wxImage::AddHandler(new wxGIFHandler);
		seenHelp = true;
	}
	HelpDialog dia;
	dia.ShowModal();


}
void MainWindow::OnRawInputEnter(wxCommandEvent& event)
{

	wxString curVal = raw_input->GetValue();
	if (curVal.size())
	{
		DRUIDString cmd(curVal.mb_str());
		executeCommand(cmd);
	}

	raw_input->Clear();
}


void MainWindow::doQuit()
{

	if (connection.get() && connection->active())
	{
		uint8_t wait_count = 0;
		while (executing_request && wait_count++ < 3)
			PLATFORM_SLEEP(1);

		executing_request = true; // lock out other comm events

		DRUID::SerialUIUserPtr serial_user = connection->serialUser();

		doUpMenu(serial_user);  // always do it once...
		if (current_menu_depth)
		{

			uint8_t failCount = 0;
			while (current_menu_depth && (doUpMenu(serial_user) || failCount++ < 3))
			{
				;
			}

		}

		serial_user->exitProgramMode();
		executing_request = false;

	}

	Close(true);

}

BEGIN_EVENT_TABLE(MainWindow, wxFrame)
    EVT_MENU(ID_Settings,  MainWindow::OnSettings)
    EVT_MENU(ID_AutoCheckForUpdates, MainWindow::OnAutoUpdateCheck)
    EVT_MENU(ID_Quit,  MainWindow::OnQuit)
    EVT_MENU(ID_About, MainWindow::OnAbout)
    EVT_MENU(ID_Help, MainWindow::OnHelp)
    EVT_MENU(ID_ReInit, MainWindow::OnReInit)
    EVT_MENU(ID_CheckForUpdates, MainWindow::OnCheckForUpdates)
    EVT_MENU(ID_Settings, MainWindow::OnSettings)
    EVT_MENU(ID_ToggleRawInput, MainWindow::OnToggleRawInput)
    EVT_MENU(ID_UploadStreamRate_Conservative, MainWindow::OnSelectUploadRateConservative)
    EVT_MENU(ID_UploadStreamRate_Standard, MainWindow::OnSelectUploadRateStandard)
    EVT_MENU(ID_UploadStreamRate_Fast, MainWindow::OnSelectUploadRateFast)
    EVT_MENU(ID_UploadStreamRate_Reckless, MainWindow::OnSelectUploadRateReckless)
    EVT_MENU(ID_StateTrackingPeriod_Short, MainWindow::OnSelectStateTrackingPeriodShort)
    EVT_MENU(ID_StateTrackingPeriod_Standard, MainWindow::OnSelectStateTrackingPeriodStandard)
    EVT_MENU(ID_StateTrackingPeriod_Long, MainWindow::OnSelectStateTrackingPeriodLong)
    EVT_MENU(ID_StateTracking_Disable, MainWindow::OnSelectStateTrackingDisable)
    EVT_BUTTON(ID_Output_Clear, MainWindow::OnOutputClear)
    EVT_BUTTON(ID_Output_Export, MainWindow::OnOutputExport)
    EVT_BUTTON(ID_SiteButton, MainWindow::OnSiteClick)
    EVT_TIMER(ID_PingTimer, MainWindow::OnPingTimer)
    EVT_TIMER(ID_AutoInitTimer, MainWindow::OnAutoInitTimer)
    EVT_TIMER(ID_MenuCrawlTimer, MainWindow::OnMenuCrawlTimer)
	EVT_TEXT_ENTER(ID_RawInput, MainWindow::OnRawInputEnter)

END_EVENT_TABLE()

} /* namespace DRUID */
