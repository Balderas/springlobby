/**
c    This file is part of springsettings,
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

#include "tab_quality_video.h"
#include "se_utils.h"
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/event.h>
#include <wx/defs.h>
#include <wx/slider.h>
#include <wx/checkbox.h>
#include <wx/stattext.h>
#include <wx/checkbox.h>
#include <wx/radiobut.h>
#include <wx/combobox.h>

#include "Defs.hpp"

void tab_quality_video::initVideoSizer(wxFlexGridSizer* sizer) {


	// i < "sizeof"(VO_CBOX)
	for (int i = 0; i < ctrl_vo_Boxes_size; i++) {
		ctrl_vo_Boxes[i] = new wxCheckBox(this, VO_CBOX[i].id, (VO_CBOX[i].lbl));
		ctrl_vo_Boxes[i]->SetToolTip(VO_CBOX[i].tTip[0]);
		sizer->Add(ctrl_vo_Boxes[i], 0, wxTOP, (i == 0)? 10: 0);
	}

	ctrl_x_res = new wxTextCtrl(this, ID_RES_CHOICES_LBOX_X,_T(""), WX_DEF_P, wxSize(60, 20), 0);
	ctrl_y_res = new wxTextCtrl(this, ID_RES_CHOICES_LBOX_Y, _T(""), WX_DEF_P, wxSize(60, 20), 0);
	ctrl_x_res->SetToolTip(RC_TEXT[0].tTip[0]);
	ctrl_y_res->SetToolTip(RC_TEXT[1].tTip[0]);


	wxSizer* subSizer = new wxBoxSizer(wxHORIZONTAL);

	subSizer->Add(ctrl_x_res, 0, wxALIGN_LEFT, 10);
	subSizer->Add(5, 0, 0);
	subSizer->Add(new wxStaticText(this, -1, wxT("x")), 0, wxTOP | wxBOTTOM, 0);
	subSizer->Add(5, 0, 0);
	subSizer->Add(ctrl_y_res, 0, wxALIGN_RIGHT, 10);

	wxSizer* subSizer2 = new wxBoxSizer(wxVERTICAL);
	subSizer2->Add(new wxStaticText(this, -1, _("Screen Resolution")), 1, wxTOP|wxEXPAND , 15);
	subSizer2->Add(subSizer);
	sizer->Add(subSizer2);
	sizer->Add(0,5,0);

}

void tab_quality_video::updateControls(int what_to_update)
{
	if (what_to_update == UPDATE_ALL)
		{
			//the rest
			for (int i = 5; i < 7; i++) {
						ctrl_qa_Boxes[i]->SetValue(intSettings[QA_CBOX[i].key]);
			}

			for (int i = 0; i < ctrl_vo_Boxes_size; i++) {
				ctrl_vo_Boxes[i]->SetValue(intSettings[VO_CBOX[i].key]);
			}
		}

	if (what_to_update == UPDATE_VIDEO_MODE || what_to_update == UPDATE_ALL)
	{
		ctrl_x_res->SetValue(wxString::Format( _("%d"),intSettings[RC_TEXT[0].key]));
		ctrl_y_res->SetValue(wxString::Format( _("%d"),intSettings[RC_TEXT[1].key]));
	}
	if (what_to_update == UPDATE_QA_BOXES || what_to_update == UPDATE_ALL)
	{
		//option 5-7 are not on presets
		for (int i = 0; i < 5; i++) {
			ctrl_qa_Boxes[i]->SetValue(intSettings[QA_CBOX[i].key]);
		}
		for (int i = 8; i < ctrl_qa_Boxes_size; i++) {
			ctrl_qa_Boxes[i]->SetValue(intSettings[QA_CBOX[i].key]);
		}

		int waterOptIndex = 0;
		int waterSetting = intSettings[WR_COMBOX[0].key];
		switch (waterSetting)
		{
		case 0:
		case 1: waterOptIndex = waterSetting; break;
		case 2: waterOptIndex = 3; break;
		case 3: waterOptIndex = 2; break;
		case 4: waterOptIndex = 4; break;
		}
		ctrl_waterQ_CBox->SetValue(WR_COMBOX_CHOICES[waterOptIndex]);

		int useFSAA = intSettings[VO_SLI_EXT[0].key];
		int FSAALev = intSettings[VO_SLI[0].key];
		ctrl_fsaa_slider->SetValue((useFSAA == 1)? FSAALev: 0);

		switch (intSettings[VO_RBUT[0].key]) {
		case 16: { ctrl_z_radio1->SetValue(1); } break;
		case 24: { ctrl_z_radio2->SetValue(1); } break;
		}
	}


}

void tab_quality_video::initQualitySizer(wxFlexGridSizer* sizer)
{
	sizer->Add(new wxStaticText(this, -1, _("If an option needs special hardware to work\n"
												"it will be mentioned in the tooltip.")), 1, wxTOP|wxEXPAND , 10);

	// i < 8 with High resolution LOS textures
	// i < 7 without
	for (int i = 0; i < ctrl_qa_Boxes_size-3; i++) {
		ctrl_qa_Boxes[i] = new wxCheckBox(this, QA_CBOX[i].id, (QA_CBOX[i].lbl));
		//ctrl_qa_Boxes[i]->SetValue(configHandler->GetSpringConfigInt(QA_CBOX[i].key,fromString(QA_CBOX[i].def)));
		ctrl_qa_Boxes[i]->SetToolTip(QA_CBOX[i].tTip[0]);
		sizer->Add(ctrl_qa_Boxes[i], 0, wxTOP, (i == 0)? 5: 0);
	}

	wxSizer* subSizer = new wxBoxSizer(wxVERTICAL);
	subSizer->Add(new wxStaticText(this, -1, _("Water Quality")), 0, wxTOP| wxEXPAND, 10);
	ctrl_waterQ_CBox = new wxComboBox(this, ID_WINDOWP_WR_COMBOX, WR_COMBOX_CHOICES[0], wxDefaultPosition, wxSize(220,21),
			5,WR_COMBOX_CHOICES,wxCB_DROPDOWN|wxCB_READONLY);
	ctrl_waterQ_CBox->SetToolTip(WR_COMBOX[0].tTip[0]);
	subSizer->Add(ctrl_waterQ_CBox, 0, wxBOTTOM, 5);

	sizer->Add(subSizer,0, wxBOTTOM, 5);

}

void tab_quality_video::initAASizer(wxFlexGridSizer* sizer){
	for (int i = 8; i < ctrl_qa_Boxes_size; i++) {
		ctrl_qa_Boxes[i] = new wxCheckBox(this, QA_CBOX[i].id, (QA_CBOX[i].lbl));
		//ctrl_qa_Boxes[i]->SetValue(configHandler->GetSpringConfigInt(QA_CBOX[i].key,fromString(QA_CBOX[i].def)));
		ctrl_qa_Boxes[i]->SetToolTip(QA_CBOX[i].tTip[0]);
		sizer->Add(ctrl_qa_Boxes[i], 0, wxTOP, (i == 8)? 10: 0);
	}
	wxSizer* subsizer = new wxBoxSizer(wxVERTICAL);

	ctrl_fsaa_slider = new wxSlider(this, VO_SLI[0].id,  0, 0, 16, WX_DEF_P, WX_SLI_S, SLI_STYLE, WX_DEF_V);
	ctrl_fsaa_slider->SetToolTip(VO_SLI[0].tTip[0]);
	subsizer->Add(new wxStaticText(this, -1, (VO_SLI[0].lbl)), 0, wxTOP|wxEXPAND, 10);
	subsizer->Add(ctrl_fsaa_slider, 0, wxALIGN_LEFT|wxBOTTOM|wxEXPAND, 5);
	sizer->Add(subsizer);
}

void tab_quality_video::initZBufferSizer(wxFlexGridSizer* sizer)
{
	//z-buffer

	sizer->Add(new wxStaticText(this, -1, _("Resolution in bit")), 0, wxTOP ,15);

	ctrl_z_radio1 = new wxRadioButton(this, VO_RBUT[0].id, (VO_RBUT[0].lbl), WX_DEF_P, WX_DEF_S, wxRB_GROUP, WX_DEF_V);
	ctrl_z_radio2 = new wxRadioButton(this, VO_RBUT[1].id, (VO_RBUT[1].lbl), WX_DEF_P, WX_DEF_S, 0, WX_DEF_V);

	ctrl_z_radio1->SetToolTip(VO_RBUT[0].tTip[0]);
	ctrl_z_radio2->SetToolTip(VO_RBUT[1].tTip[0]);

	sizer->Add(ctrl_z_radio1, 0, wxTOP, 0);
	sizer->Add(ctrl_z_radio2, 0, wxBOTTOM,  10);

}

tab_quality_video::tab_quality_video(wxWindow *parent, wxWindowID id , const wxString &title , const wxPoint& pos ,
		const wxSize& size, long style)
: abstract_panel(parent, id , title , pos , size, style) {
	ctrl_qa_Boxes = new wxCheckBox*[ctrl_qa_Boxes_size];
	ctrl_vo_Boxes = new wxCheckBox*[ctrl_vo_Boxes_size];
	 parentSizer = new wxFlexGridSizer(3,0,0);
	 leftSizer = new wxFlexGridSizer(1,15,0);
	 middleSizer = new wxFlexGridSizer(1,15,0);
	 rightSizer = new wxFlexGridSizer(1,15,0);//for info
	 SizerA = new wxFlexGridSizer(1,15,10);
	 SizerB = new wxFlexGridSizer(1,15,10);
	 SizerC = new wxFlexGridSizer(1,15,10);
	 SizerD = new wxFlexGridSizer(1,5,10);
	 boxA = new wxStaticBoxSizer(wxVERTICAL ,this,_("Render Quality Options"));
	 boxB = new wxStaticBoxSizer(wxVERTICAL ,this,_("Video Mode Options"));
	 boxC = new wxStaticBoxSizer(wxVERTICAL ,this,_("Anti-Aliasing Options"));
	 boxD = new wxStaticBoxSizer(wxVERTICAL ,this,_("Z-/Depth-Buffer"));
	SizerA->AddGrowableCol(0);
	SizerB->AddGrowableCol(0);
	SizerC->AddGrowableCol(0);
	SizerD->AddGrowableCol(0);

	initVideoSizer(SizerB);
	initZBufferSizer(SizerD);
	initQualitySizer(SizerA);
	initAASizer(SizerC);

	SizerA->Fit(this);
	SizerA->SetSizeHints(this);
	SizerB->Fit(this);
	SizerB->SetSizeHints(this);
	SizerC->Fit(this);
	SizerC->SetSizeHints(this);
	SizerD->Fit(this);
	SizerD->SetSizeHints(this);

	boxA->Add(SizerA,1,wxEXPAND);
	boxB->Add(SizerB,1,wxEXPAND);
	boxC->Add(SizerC,1,wxEXPAND);
	boxD->Add(SizerD,1,wxEXPAND);
	leftSizer->Add(boxB,1,wxEXPAND|wxALL,5);
	rightSizer->Add(boxC,1,wxEXPAND|wxALL,5);
	middleSizer->Add(boxA,1,wxEXPAND|wxALL,5);
	leftSizer->Add(boxD,1,wxEXPAND|wxALL,5);
	parentSizer->Add(leftSizer,0,wxALL|wxEXPAND,10);
	parentSizer->Add(middleSizer,0,wxALL|wxEXPAND,10);
	parentSizer->Add(rightSizer,0,wxALL|wxEXPAND,10);

	updateControls(UPDATE_ALL);

	SetSizer(parentSizer, true); // true --> delete old sizer if present


}

tab_quality_video::~tab_quality_video(void) {

}

BEGIN_EVENT_TABLE(tab_quality_video, abstract_panel)
	EVT_SLIDER(wxID_ANY,            tab_quality_video::OnSliderMove)
	EVT_TEXT(ID_RES_CHOICES_LBOX_X, tab_quality_video::OnTextUpdate)
	EVT_TEXT(ID_RES_CHOICES_LBOX_Y, tab_quality_video::OnTextUpdate)
	EVT_CHECKBOX(wxID_ANY,          tab_quality_video::OnCheckBoxTick)
	EVT_RADIOBUTTON(wxID_ANY,       tab_quality_video::OnRadioButtonToggle)
	//EVT_IDLE(                       tab_quality_video::update)
EVT_COMBOBOX(ID_WINDOWP_WR_COMBOX, 		tab_quality_video::OnComboBoxChange)
END_EVENT_TABLE()
