/**
    This file is part of Settings++,
    Copyright (C) 2007
    Original work by Kloot
    cross-plattform/UI adaptation and currently maintained by koshi (Ren� Milk)
    visit http://spring.clan-sy.com/phpbb/viewtopic.php?t=12104
    for more info/help

    Settings++ is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Settings++ is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Settings++.  If not, see <http://www.gnu.org/licenses/>.
**/

// Autohost
// LODScale=1.000
// LODScaleReflection=1.000
// LODScaleRefraction=1.000
// LODScaleShadow=1.000
// MapArchives=1									// multiple maps per archive?
// MetalMapPalette=0
//
// FIXME: VSync Off=-1?



#ifndef DEFS_HPP
#define DEFS_HPP

#define PWIN_SIZE_X  895
#define PWIN_SIZE_Y  830
#define CWIN_SIZE_X  580
#define CWIN_SIZE_Y  360

// ParentWin menu-item IDs
#define ID_MENUITEM_SAVE  50
#define ID_MENUITEM_RESET  51
#define ID_MENUITEM_QUIT  52
#define ID_MENUITEM_DISABLE_WARN 53
#define ID_MENUITE_MODE 60
#define ID_MENUITEM_EXPERT 62
#define ID_MENUITEM_SIMPLE 61

// ParentWin TextInput IDs
#define ID_RES_CHOICES_LBOX_X 1111
#define ID_RES_CHOICES_LBOX_Y 1112


// ParentWin Rendering Options slider IDs
#define ID_RO_SLI_0 100
#define ID_RO_SLI_1 101
#define ID_RO_SLI_2 102
#define ID_RO_SLI_3 103
#define ID_RO_SLI_4 104
#define ID_RO_SLI_5 105
#define ID_RO_SLI_6 106
#define ID_RO_SLI_7 107
#define ID_RO_SLI_8 108

// ParentWin Video Options checkbox IDs
#define ID_WINDOWP_VO_CBOX_0 250
#define ID_WINDOWP_VO_CBOX_1 251
#define ID_WINDOWP_VO_CBOX_2 252

// ParentWin Video Options radiobutton IDs
#define ID_WINDOWP_VO_RBUT_0 280
#define ID_WINDOWP_VO_RBUT_1 281

// ParentWin Video Options slider IDs
#define ID_VO_SLI_0 200

// tab_simple controls
#define ID_SIMPLE_QUAL_CBX		390
#define ID_SIMPLE_DETAIL_CBX	391
#define ID_SIMPLE_MODE_CBX		392


// UI Options checkbox IDs
#define ID_WINDOWP_UI_CBOX_1  360
#define ID_WINDOWP_UI_CBOX_2  361
#define ID_WINDOWP_UI_CBOX_3  362
#define ID_WINDOWP_UI_CBOX_4  363
#define ID_WINDOWP_UI_CBOX_5  364
#define ID_WINDOWP_UI_CBOX_6  365
#define ID_WINDOWP_UI_CBOX_7  366
#define ID_WINDOWP_UI_CBOX_8  367
#define ID_WINDOWP_UI_CBOX_9  368
#define ID_WINDOWP_UI_CBOX_10 369
#define ID_WINDOWP_UI_CBOX_11 370
#define ID_WINDOWP_UI_CBOX_12 371
#define ID_WINDOWP_UI_CBOX_13 372
#define ID_WINDOWP_UI_CBOX_14 373

//QUALITY OPTIONS checkbox IDs
#define ID_WINDOWP_QA_CBOX_0  300
#define ID_WINDOWP_QA_CBOX_1  301
#define ID_WINDOWP_QA_CBOX_2  302
#define ID_WINDOWP_QA_CBOX_3  303
#define ID_WINDOWP_QA_CBOX_4  304
#define ID_WINDOWP_QA_CBOX_5  305
#define ID_WINDOWP_QA_CBOX_6  306
#define ID_WINDOWP_QA_CBOX_7  307
#define ID_WINDOWP_QA_CBOX_8  308
#define ID_WINDOWP_QA_CBOX_9  309

#define ID_WINDOWP_WR_COMBOX  320

// ParentWin Audio Options slider IDs
#define ID_AO_SLI_0 400
#define ID_AO_SLI_1 401
#define ID_AO_SLI_2 402

// ParentWin Debug Options checkbox IDs
#define ID_WINDOWP_DO_CBOX_0 540
#define ID_WINDOWP_DO_CBOX_1 541

// ParentWin Debug Options slider IDs
#define ID_DO_SLI_0 500

// ParentWin Water Rendering radiobutton IDs
#define ID_WINDOWP_WR_RBUT_0 550
#define ID_WINDOWP_WR_RBUT_1 551
#define ID_WINDOWP_WR_RBUT_2 552
#define ID_WINDOWP_WR_RBUT_3 553

// ChildWin Mouse Options checkbox IDs
#define ID_WINDOWC_MO_CBOX_0 650
#define ID_WINDOWC_MO_CBOX_1 651

// ChildWin Mouse Options radiobutton IDs
#define ID_WINDOWC_MO_RBUT_0 680
#define ID_WINDOWC_MO_RBUT_1 681
#define ID_WINDOWC_MO_RBUT_2 682
#define ID_WINDOWC_MO_RBUT_3 683
#define ID_WINDOWC_MO_RBUT_4 684

// ChildWin Mouse Options slider IDs
#define ID_MO_SLI_0 660
#define ID_MO_SLI_1 661
#define ID_MO_SLI_2 662
#define ID_MO_SLI_3 663
#define ID_MO_SLI_4 664



#include <wx/string.h>
#include <string>
#include <iostream>
//#include <fstream>
#include <sstream>

struct Control {
    const wxString lbl;
    const wxString key;
    int id;
    const std::string def; //default value for confighandler, not control
    const wxString tTip [1];
};

//inline wxString _S (const std::string str)
//{
//	return wxString(str.c_str(),*wxConvCurrent);
//}
//
//template<typename Type> void toString(std::string& s, Type t) {
//	//using namespace std;
//			std::stringstream ss;
//			ss << t;
//			ss >> s;
//}
//
//template<typename Type> Type fromString(const std::string& s) {
//        std::stringstream ss;
//        ss << s;
//        Type r;
//        ss >> r;
//        return r;
//}

//TODO find reasonable default values
//TODO is max_texture stages obsolte?
const Control RO_SLI[9] = {
	{_T("shadow-map size"),              wxT("ShadowMapSize"),       ID_RO_SLI_0,	"2048", {_T("placeholder")}	},
	{_T("tree view-distance"),           wxT("TreeRadius"),          ID_RO_SLI_1,	"3000", {_T("placeholder")}	},
	{_T("terrain detail"),               wxT("GroundDetail"),        ID_RO_SLI_2,	"120", {_T("placeholder")}	},
	{_T("unit detail"),                  wxT("UnitLodDist"),         ID_RO_SLI_3,	"600", {_T("placeholder")}	},
	{_T("grass detail"),                 wxT("GrassDetail"),         ID_RO_SLI_4,	"10", {_T("placeholder")}	},
	{_T("ground decals"),                wxT("GroundDecals"),        ID_RO_SLI_5,	"120", {_T("placeholder")}	},
	{_T("unit icon distance"),           wxT("UnitIconDist"),        ID_RO_SLI_6,	"1000", {_T("placeholder")}	},
	{_T("max simultaneous particles"),     wxT("MaxParticles"),        ID_RO_SLI_7,	"20000"	, {_T("placeholder")}},
	{_T("max texture stages (SM3)"), wxT("SM3MaxTextureStages"), ID_RO_SLI_8,	"20"	, {_T("placeholder")}}
};

const Control VO_CBOX[3] = {
	{_T("run full-screen"),                wxT("Fullscreen"),              ID_WINDOWP_VO_CBOX_0,	"1", {_T("placeholder")}},
	{_T("dual-screen mode"),               wxT("DualScreenMode"),          ID_WINDOWP_VO_CBOX_1,	"0", {_T("placeholder")}},
	{_T("enable v-sync"),                  wxT("VSync"),                   ID_WINDOWP_VO_CBOX_2,	"1", {_T("placeholder")}},
};

const Control VO_RBUT[2] = {
	{_T("16-bit Z-buffer"), wxT("DepthBufferBits"), ID_WINDOWP_VO_RBUT_0,	"16", {_T("placeholder")}},
	{_T("24-bit Z-buffer"), wxT("DepthBufferBits"), ID_WINDOWP_VO_RBUT_1,	"16", {_T("placeholder")}}
};


const Control VO_SLI[1] = {
	{_T("full-screen anti-aliasing samples"), wxT("FSAALevel"), ID_VO_SLI_0,	"0", {_T("placeholder")}}
};

const Control VO_SLI_EXT[1] = {
	{_T(""), wxT("FSAA"), -1, "0"}
};


const Control AO_SLI[3] = {
	{_T("maximum simultaneous sounds"), wxT("MaxSounds"),            ID_AO_SLI_0,	"16" , {_T("placeholder")}},
	{_T("global sound volume"),   wxT("SoundVolume"),                ID_AO_SLI_1,	"100", {_T("placeholder")}},
	{_T("unit reply volume"),     wxT("UnitReplySoundVolume"),       ID_AO_SLI_2,	"80" , {_T("placeholder")}}
	
};

const Control QA_CBOX[10] = {
    {_T("shadows (slow)"),                                  wxT("Shadows"),             ID_WINDOWP_QA_CBOX_0,	"0", {_T("placeholder")}},
	{_T("3D trees (needs Vertex_Program)"),                 wxT("3DTrees"),             ID_WINDOWP_QA_CBOX_1,	"1", {_T("placeholder")}},
	{_T("high-resolution clouds (needs Fragment_Program)"), wxT("AdvSky"),              ID_WINDOWP_QA_CBOX_2,	"0", {_T("placeholder")}},
	{_T("dynamic clouds (slow)"),                           wxT("DynamicSky"),          ID_WINDOWP_QA_CBOX_3,	"0", {_T("placeholder")}},
	{_T("reflective units (needs Fragment_Program)"),       wxT("AdvUnitShading"),      ID_WINDOWP_QA_CBOX_4,	"0", {_T("placeholder")}},
	{_T("never use shaders when rendering SM3 maps"),       wxT("SM3ForceFallbackTex"), ID_WINDOWP_QA_CBOX_5,	"1", {_T("placeholder")}},
	{_T("enable LuaShaders support"),                       wxT("LuaShaders"),          ID_WINDOWP_QA_CBOX_6,	"1", {_T("placeholder")}},
	{_T("high-resolution LOS textures"),                    wxT("HighResLos"),          ID_WINDOWP_QA_CBOX_7,	"1", {_T("placeholder")}},
    {_T("draw smooth points"),                              wxT("SmoothPoints"),        ID_WINDOWP_QA_CBOX_8,	"0", {_T("placeholder")}},
	{_T("draw smooth lines"),                               wxT("SmoothLines"),         ID_WINDOWP_QA_CBOX_9,	"0", {_T("placeholder")}},
};
	

const Control UI_CBOX[14] = {
	{_T("enable LuaUI widgets"),                            wxT("LuaUI"),               ID_WINDOWP_UI_CBOX_1,	"1", {_T("placeholder")}},
	
	{_T("draw commands on mini-map"),                       wxT("MiniMapDrawCommands"), ID_WINDOWP_UI_CBOX_2,	"1", {_T("placeholder")}},
	{_T("draw icons on mini-map"),                          wxT("MiniMapIcons"),        ID_WINDOWP_UI_CBOX_3,	"1", {_T("placeholder")}},
	{_T("draw markers on mini-map"),                        wxT("MiniMapMarker"),       ID_WINDOWP_UI_CBOX_4,	"1", {_T("placeholder")}},
	{_T("mini-map on left (single screen)"), 				wxT("MinimapOnLeft"),       ID_WINDOWP_UI_CBOX_5,	"1", {_T("placeholder")}},
	{_T("mini-map on left (dual screen)"),   				wxT("DualScreenMiniMapOnLeft"), ID_WINDOWP_UI_CBOX_6,"1", {_T("placeholder")}},
	{_T("simplified mini-map colors"),                      wxT("SimpleMiniMapColors"), ID_WINDOWP_UI_CBOX_7,	"0", {_T("placeholder")}},

	{_T("team-colored nanospray"),                          wxT("TeamNanoSpray"),       ID_WINDOWP_UI_CBOX_8,	"1", {_T("placeholder")}},
	{_T("colorized elevation map"),                         wxT("ColorElev"),           ID_WINDOWP_UI_CBOX_9,	"1", {_T("placeholder")}},
	
	{_T("show in-game clock"),                              wxT("ShowClock"),           ID_WINDOWP_UI_CBOX_10,	"1", {_T("placeholder")}},
	{_T("show in-game player information"),                 wxT("ShowPlayerInfo"),      ID_WINDOWP_UI_CBOX_11,	"1", {_T("placeholder")}},
	{_T("show in-game framerate"),                          wxT("ShowFPS"),             ID_WINDOWP_UI_CBOX_12,	"0", {_T("placeholder")}},
//TODO is there even a reason that it should be disabled?
	{_T("fix rendering on alt-tab"),                        wxT("FixAltTab"),           ID_WINDOWP_UI_CBOX_13,	"1", {_T("placeholder")}},
	{_T("disallow helper AI's"),                            wxT("NoHelperAIs"),         ID_WINDOWP_UI_CBOX_14,	"0", {_T("placeholder")}}
};


const Control MO_SLI[5] = {
	{_T("Overhead Camera scroll speed"),    wxT("OverheadScrollSpeed"),    ID_MO_SLI_0,	"10", {_T("placeholder")}},
	{_T("Rotatable overhead Camera scroll speed"), wxT("RotOverheadScrollSpeed"), ID_MO_SLI_1,	"0" , {_T("placeholder")}},
	{_T("Total War Camera scroll speed"),          wxT("TWScrollSpeed"),          ID_MO_SLI_2,	"0" , {_T("placeholder")}},
	{_T("First Person Camera scroll speed"),         wxT("FPSScrollSpeed"),         ID_MO_SLI_3,	"0" , {_T("placeholder")}},
	{_T("Free Camera scroll speed"),          wxT("CamFreeScrollSpeed"),     ID_MO_SLI_4,	"0" , {_T("placeholder")}}
};

const Control MO_SLI_EXT[5] = {
	{_T(""), wxT("OverheadEnabled"), -1,	"1", {_T("placeholder")}},
	{_T(""), wxT("RotOverheadEnabled"), -1,	"0", {_T("placeholder")}},
	{_T(""), wxT("TWEnabled"), -1,		"0", {_T("placeholder")}},
	{_T(""), wxT("FPSEnabled"), -1,		"0", {_T("placeholder")}},
	{_T(""), wxT("CamFreeEnabled"), -1,	"0", {_T("placeholder")}}
};


const Control DO_SLI[1] = {
	{_T("console verbose level (0=min,10=max)"), wxT("VerboseLevel"), ID_DO_SLI_0,	"0", {_T("placeholder")}}
};

const Control DO_CBOX[2] = {
	{_T("catch AI exceptions"),        wxT("CatchAIExceptions"), ID_WINDOWP_DO_CBOX_0,	"0", {_T("placeholder")}},
	{_T("send debug info to console"), wxT("StdoutDebug"),       ID_WINDOWP_DO_CBOX_1,	"0", {_T("placeholder")}}
};


const Control WR_COMBOX[1] = {
		{_T("basic"),                  wxT("ReflectiveWater"), ID_WINDOWP_WR_COMBOX,	"1", {_T("placeholder")}}
	/*{_T("reflective",              "ReflectiveWater", ID_WINDOWP_WR_COMBOX_1,	"1"},
	{_T("reflective + refractive", "ReflectiveWater", ID_WINDOWP_WR_COMBOX_2,	"1"},
	{_T("dynamic",                 "ReflectiveWater", ID_WINDOWP_WR_COMBOX_3,	"1"}*/
};

const wxString WR_COMBOX_CHOICES[4] = {
	wxT("basic"), wxT("reflective"), wxT("reflective + refractive"), wxT("dynamic")
};

const Control MO_CBOX[2] = {
	{_T("invert mouse y-axis"),             wxT("InvertMouse"),      ID_WINDOWC_MO_CBOX_0,	"1", {_T("placeholder")}},
	{_T("mini-map 3-button mouse support"), wxT("MiniMapFullProxy"), ID_WINDOWC_MO_CBOX_1,	"1", {_T("placeholder")}}
};

const Control MO_RBUT[5] = {
	{_T("OH (overhead)"),            wxT("CamMode"), ID_WINDOWC_MO_RBUT_0,	"1", {_T("placeholder")}},
	{_T("ROH (rotatable overhead)"), wxT("CamMode"), ID_WINDOWC_MO_RBUT_1,	"1", {_T("placeholder")}},
	{_T("TW (total war)"),           wxT("CamMode"), ID_WINDOWC_MO_RBUT_2,	"1", {_T("placeholder")}},
	{_T("FP (first-person)"),        wxT("CamMode"), ID_WINDOWC_MO_RBUT_3,	"1", {_T("placeholder")}},
	{_T("FC (free camera)"),         wxT("CamMode"), ID_WINDOWC_MO_RBUT_4,	"1", {_T("placeholder")}}
};

const Control RC_TEXT[2] = {
	{_T(""), wxT("XResolution"), -1,	"1024", {_T("placeholder")}},
	{_T(""), wxT("YResolution"), -1,	"768" , {_T("placeholder")}}
};



#define SLI_STYLE (wxSL_LABELS | wxSL_AUTOTICKS)
#define WX_DEF_V wxDefaultValidator
#define WX_DEF_P wxPoint(-1, -1)
#define WX_DEF_S wxSize(-1, -1)
#define WX_SLI_S wxSize(200, -1)

/** not used
#define NUM_DEFAULTS 88
const char DEFAULTS[NUM_DEFAULTS][64] = {
	"3DTrees=1",
	"AdvSky=0",
	"AdvUnitShading=0",
	"FSAA=0",
	"FSAALevel=0",
	"ReflectiveWater=1",
	"MaxParticles=20000",
	"DynamicSky=0",
	"DepthBufferBits=16",
	"StencilBufferBits=1",
	"GrassDetail=10",
	"GroundDecals=100",
	"GroundDetail=120",
	"Shadows=0",
	"ShadowMapSize=2048",
	"SM3MaxTextureStages=20",
	"SM3ForceFallbackTex=1",
	"TreeRadius=3000",
	"UnitIconDist=1000",
	"UnitLodDist=600",
	"VSync=1",
	"XResolution=1024",
	"YResolution=768",

	"BuildIconsFirst=0",
	"ColorElev=1",
	"DualScreenMode=0",
	"FixAltTab=1",
	"Fullscreen=0",
	"FixAltTab=1",
	"HighResLos=1",
	"NoHelperAIs=0",
	"TeamNanoSpray=1",
	"SmoothLines=0",
	"SmoothPoints=0",
	"ShowClock=1",
	"ShowPlayerInfo=1",
	"ShowFPS=0",

	"StdoutDebug=0",
	"CatchAIExceptions=0",
	"VerboseLevel=0",

	"CamMode=1",
	"CamTimeExponent=4.0",
	"CamTimeFactor=1.0",
	"DoubleClickTime=200",

	"FontCharFirst=32",
	"FontCharLast=223",
	"FontFile=Luxi.ttf",

	"InfoConsoleGeometry=0.25 0.95 0.40 0.20",
	"TooltipGeometry=0.00 0.00 0.40 0.1",
	"TooltipOutlineFont=0",
	"InfoMessageTime=400",
	"InvertMouse=1",
	"InvertQueueKey=0",
	"WindowedEdgeMove=1",

	"LuaGaia=1",
	"LuaRules=1",
	"LuaShaders=1",
	"LuaUI=1",

	"MapArchives=1",
	"MetalMapPalette=0",

	"MiniMapButtonSize=16",
	"MiniMapCursorScale=-0.5",
	"MiniMapDrawCommands=1",
	"MiniMapFullProxy=1",
	"MiniMapGeometry=2 2 200 200",
	"MiniMapIcons=1",
	"MiniMapMarker=1",
	"MiniMapUnitExp=0.25",
	"MiniMapUnitSize=2.5",
	"MinimapOnLeft=1",
	"DualScreenMiniMapOnLeft=1",
	"SimpleMiniMapColors=0",

	"OverheadEnabled=1",
	"OverheadScrollSpeed=10",
	"RotOverheadEnabled=0",
	"RotOverheadScrollSpeed=0",
	"RotOverheadMouseScale=0.002",
	"TWEnabled=0",
	"TWScrollSpeed=0",
	"FPSEnabled=0",
	"FPSScrollSpeed=0",
	"FPSMouseScale=0.002",
	"CamFreeEnabled=0",
	"CamFreeScrollSpeed=0",
	"ScrollWheelSpeed=25",

	"MaxSounds=16",
	"SoundVolume=100",
	"UnitReplySoundVolume=80"
};
**/

#endif
