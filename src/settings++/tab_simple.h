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

#ifndef __TAB_SIMPLE_h__
#define __TAB_SIMPLE_h__

#include "tab_abstract.h"

class wxComboBox;
//class wxCheckBox;
//class wxTextCtrl;
class wxSlider;
class wxFlexGridSizer;
//class wxRadioButton;
class wxString;
//class wxCommandEvent;
class wxWindow;
class wxPoint;
class wxSize;
class wxCloseEvent;
class wxStaticText;
class wxStaticBoxSizer;
class wxButton;
class settings_frame;

class tab_simple : public abstract_panel
{
 	
	public:
		tab_simple(settings_frame* _origin, wxWindow *parent, wxWindowID id = 1, const wxString &title = wxT("Project2"), 
				const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);
		virtual ~tab_simple();

		
		void updateControls(int);
		void saveCbxChoices();
		void setTabs(abstract_panel* ,abstract_panel* );
		void OnComboBoxChange(wxCommandEvent& event);

    protected:
    	void getSetUpResolutionCBX();
        void OnClose(wxCloseEvent& event);
        void OnButtonClick(wxCommandEvent& event);
		void CreateGUIControls();
		void initOptSizer(wxFlexGridSizer*);
		void initInfoSizer(wxFlexGridSizer*);
		void initButSizer(wxSizer* );
		wxComboBox* renderQuality_CBX;
		wxComboBox* renderDetail_CBX;
		wxComboBox* videoMode_CBX;
		wxSlider* audioVolume_SLI;
		wxButton* goExpert_BUT;
		wxStaticText* infoText;
		
		//dirty
		abstract_panel* detailTab;
		abstract_panel* qualityTab;
		
		wxSizer* parentSizer;	
		
		wxFlexGridSizer* leftSizer ;
		wxFlexGridSizer* rightSizer;
		wxFlexGridSizer* Sizer_CBX ;
		wxFlexGridSizer* Sizer_info;
		wxSizer* Sizer_BUT;
		
		wxStaticBoxSizer* boxA ;
		wxStaticBoxSizer* boxB ;
		settings_frame* origin;
		
		DECLARE_EVENT_TABLE()
};   
    
#endif    

