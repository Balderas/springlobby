/**
    This file is part of springsettings,
    Copyright (C) 2007
    Original work by Kloot
    cross-plattform/UI adaptation and currently maintained by koshi (Ren� Milk)
    visit http://spring.clan-sy.com/phpbb/viewtopic.php?t=12104
    for more info/help

    springsettings is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with springsettings.  If not, see <http://www.gnu.org/licenses/>.
**/

#ifndef __SETTINGS_FRAME_h__
#define __SETTINGS_FRAME_h__


#include <wx/frame.h>
class wxNotebook;
class tab_simple;
class tab_ui;
class tab_render_detail;
class tab_quality_video;
class audio_panel;
class debug_panel;
class abstract_panel;
class wxMenu;
class wxCommanEvent;
class wxCloseEvent;
class wxString;
class wxWindow;
class wxPoint;
class wxSize;
class wxFlexGridSizer;
class wxNotebookEvent;
class PathOptionPanel;
class wxIcon;

class settings_frame : public wxFrame
{
	private:
		DECLARE_EVENT_TABLE();
		
	public:
		settings_frame(wxWindow *parent, wxWindowID id = 1, const wxString &title = wxT("Project2"), const wxPoint& pos = wxDefaultPosition, 
				const wxSize& size = wxDefaultSize, long style =  wxCAPTION | wxSYSTEM_MENU | wxMINIMIZE_BOX | wxCLOSE_BOX);
		virtual ~settings_frame();
		void handleExternExit();
		void switchToExpertMode();
		void buildGuiFromErrorPanel();
		
	private:
		tab_simple* simpleTab;
		tab_ui* uiTab;
		audio_panel* audioTab;
		debug_panel* debugTab;
		tab_render_detail* detailTab;
		tab_quality_video* qualityTab;
									
		wxMenu* menuFile;
		wxMenu* menuMode;
		wxWindow* parentWindow;
		wxNotebook *notebook;
		wxFlexGridSizer *book_sizer;
		wxFlexGridSizer* book_sizer2;
		PathOptionPanel* pathOpt_panel;
		
		wxIcon* settingsIcon;
		
		void OnNBchange(wxNotebookEvent&);
		void initMenuBar();
		void handleExit();
		bool alreadyCalled;
		
		enum
		{
			////GUI Enum Control ID Start
			ID_UI = 1013,
			ID_QUALITY_VIDEO = 1012,
			ID_RENDER_DETAIL = 1011,
			ID_SIMPLE = 1010,
			ID_DEBUG = 1009,
			ID_MOUSE = 1008,
			ID_AUDIO = 1007,
			ID_GENERAL = 1006,
			ID_VIDEO = 1005,
			ID_RENDERING = 1004,
			ID_OPTIONS = 1003,
			////GUI Enum Control ID End
			ID_DUMMY_VALUE_ //don't remove this value unless you have other enum values
		};
		void OnClose(wxCloseEvent& event);
		void OnMenuChoice(wxCommandEvent& event);
		void CreateGUIControls();
		void resetSettings();
		void updateAllControls();
		
};


#endif
