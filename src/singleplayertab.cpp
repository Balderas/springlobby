/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */


#include <wx/intl.h>
#include <wx/sizer.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/checkbox.h>

#include "singleplayertab.h"
#include "mapctrl.h"
#include "mapgridctrl.h"
#include "utils.h"
#include "uiutils.h"
#include "ui.h"
#include "iunitsync.h"
#include "addbotdialog.h"
#include "server.h"

#ifndef HAVE_WX26
#include "aui/auimanager.h"
#endif

#include "settings++/custom_dialogs.h"

#include "springunitsynclib.h"

BEGIN_EVENT_TABLE(SinglePlayerTab, wxPanel)

  EVT_CHOICE( SP_MAP_PICK, SinglePlayerTab::OnMapSelect )
  EVT_CHOICE( SP_MOD_PICK, SinglePlayerTab::OnModSelect )
  EVT_BUTTON( SP_ADD_BOT , SinglePlayerTab::OnAddBot )
  EVT_BUTTON( SP_START , SinglePlayerTab::OnStart )
  EVT_BUTTON( SP_RESET , SinglePlayerTab::OnReset )
  EVT_CHECKBOX( SP_RANDOM, SinglePlayerTab::OnRandomCheck )

END_EVENT_TABLE()


SinglePlayerTab::SinglePlayerTab(wxWindow* parent, Ui& ui, MainSinglePlayerTab& msptab):
  wxScrolledWindow( parent, -1 ),
  m_ui( ui ),
  m_battle( ui, msptab )
{
  #ifndef HAVE_WX26
  GetAui().manager->AddPane( this, wxLEFT, _T("singleplayertab") );
  #endif

  wxBoxSizer* m_main_sizer = new wxBoxSizer( wxVERTICAL );

  //m_minimap = new MapCtrl( this, 100, &m_battle, ui, false, false, true, true );
  m_minimap = new MapGridCtrl( this, 100, &m_battle, ui );
  m_minimap->SetToolTip( TE(_("You can drag the sun/bot icon around to define start position.\n "
                           "Hover over the icon for a popup that lets you change side, ally and bonus." )) );
  m_main_sizer->Add( m_minimap, 1, wxALL|wxEXPAND, 5 );

  wxBoxSizer* m_ctrl_sizer = new wxBoxSizer( wxHORIZONTAL );

  m_map_lbl = new wxStaticText( this, -1, _("Map:") );
  m_ctrl_sizer->Add( m_map_lbl, 0, wxALL, 5 );

  m_map_pick = new wxChoice( this, SP_MAP_PICK );
  m_ctrl_sizer->Add( m_map_pick, 1, wxALL, 5 );

//  m_select_btn = new wxButton( this, SP_BROWSE_MAP, _T("..."), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
//  m_ctrl_sizer->Add( m_select_btn, 0, wxBOTTOM|wxRIGHT|wxTOP, 5 );

  m_mod_lbl = new wxStaticText( this, -1, _("Mod:") );
  m_ctrl_sizer->Add( m_mod_lbl, 0, wxALL, 5 );

  m_mod_pick = new wxChoice( this, SP_MOD_PICK );
  m_ctrl_sizer->Add( m_mod_pick, 1, wxALL, 5 );


//  m_ctrl_sizer->Add( 0, 0, 1, wxEXPAND, 0 );

  m_addbot_btn = new wxButton( this, SP_ADD_BOT, _("Add bot..."), wxDefaultPosition, wxSize(80, CONTROL_HEIGHT), 0 );
  m_ctrl_sizer->Add( m_addbot_btn, 0, wxALL, 5 );

  m_main_sizer->Add( m_ctrl_sizer, 0, wxEXPAND, 5 );

  m_buttons_sep = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
  m_main_sizer->Add( m_buttons_sep, 0, wxLEFT|wxRIGHT|wxEXPAND, 5 );

  wxBoxSizer* m_buttons_sizer = new wxBoxSizer( wxHORIZONTAL );

// see http://trac.springlobby.info/ticket/649
//  m_reset_btn = new wxButton( this, SP_RESET, _("Reset"), wxDefaultPosition, wxSize(80, CONTROL_HEIGHT), 0 );
//  m_buttons_sizer->Add( m_reset_btn, 0, wxALL, 5 );

  m_buttons_sizer->Add( 0, 0, 1, wxEXPAND, 0 );

  m_random_check = new wxCheckBox( this, SP_RANDOM, _("Random start positions") );
  m_buttons_sizer->Add( m_random_check, 0, wxALL, 5 );

  m_start_btn = new wxButton( this, SP_START, _("Start"), wxDefaultPosition, wxSize(80, CONTROL_HEIGHT), 0 );
  m_buttons_sizer->Add( m_start_btn, 0, wxALL, 5 );

  m_main_sizer->Add( m_buttons_sizer, 0, wxEXPAND, 5 );

  SetScrollRate( 3, 3 );
  this->SetSizer( m_main_sizer );
  this->Layout();

  ReloadMaplist();
  ReloadModlist();

}


SinglePlayerTab::~SinglePlayerTab()
{

}


void SinglePlayerTab::UpdateMinimap()
{
  //m_minimap->UpdateMinimap();
}


void SinglePlayerTab::ReloadMaplist()
{
  m_map_pick->Clear();

  wxArrayString maplist= usync().GetMapList();
  //maplist.Sort(CompareStringIgnoreCase);

  size_t nummaps = maplist.Count();
  for ( size_t i = 0; i < nummaps; i++ ) m_map_pick->Insert( RefineMapname(maplist[i]), i );

  m_map_pick->Insert( _("-- Select one --"), m_map_pick->GetCount() );
  if ( m_battle.GetHostMapName() != wxEmptyString ) {
    m_map_pick->SetStringSelection( RefineMapname( m_battle.GetHostMapName() ) );
    if ( m_map_pick->GetStringSelection() == wxEmptyString ) SetMap( m_mod_pick->GetCount()-1 );
  } else {
    m_map_pick->SetSelection( m_map_pick->GetCount()-1 );
    m_addbot_btn->Enable(false);
  }
}


void SinglePlayerTab::ReloadModlist()
{
  m_mod_pick->Clear();

  wxArrayString modlist= usync().GetModList();
  //modlist.Sort(CompareStringIgnoreCase);

  size_t nummods = modlist.Count();
  for ( size_t i = 0; i < nummods; i++ ) m_mod_pick->Insert( modlist[i], i );

  m_mod_pick->Insert( _("-- Select one --"), m_mod_pick->GetCount() );

  if ( !m_battle.GetHostModName().IsEmpty() ) {
    m_mod_pick->SetStringSelection( m_battle.GetHostModName() );
    if ( m_mod_pick->GetStringSelection() == wxEmptyString ) SetMod( m_mod_pick->GetCount()-1 );
  } else {
    m_mod_pick->SetSelection( m_mod_pick->GetCount()-1 );
  }
}


void SinglePlayerTab::SetMap( unsigned int index )
{
	//m_ui.ReloadUnitSync();
  m_addbot_btn->Enable( false );
  if ( index >= m_map_pick->GetCount()-1 ) {
    m_battle.SetHostMap( wxEmptyString, wxEmptyString );
  } else {
    try {
      UnitSyncMap map = usync().GetMapEx( index );
      m_battle.SetHostMap( map.name, map.hash );
      m_addbot_btn->Enable( true );
    } catch (...) {}
  }
  //m_minimap->UpdateMinimap();
  m_battle.SendHostInfo( IBattle::HI_Map_Changed ); // reload map options
  m_map_pick->SetSelection( index );
}


void SinglePlayerTab::SetMod( unsigned int index )
{
	//m_ui.ReloadUnitSync();
  if ( index >= m_mod_pick->GetCount()-1 ) {
    m_battle.SetHostMod( wxEmptyString, wxEmptyString );
  } else {
    try {
      UnitSyncMod mod = usync().GetMod( index );
      m_battle.SetLocalMod( mod );
      m_battle.SetHostMod( mod.name, mod.hash );
    } catch (...) {}
  }
  //m_minimap->UpdateMinimap();
  m_battle.SendHostInfo( IBattle::HI_Restrictions ); // Update restrictions in options.
  m_battle.SendHostInfo( IBattle::HI_Mod_Changed ); // reload mod options
  m_mod_pick->SetSelection( index );
}


bool SinglePlayerTab::ValidSetup()
{
  if ( (unsigned int)m_mod_pick->GetSelection() >= m_mod_pick->GetCount()-1 ) {
    wxLogWarning( _T("no mod selected") );
    customMessageBox(SL_MAIN_ICON, _("You have to select a mod first."), _("Gamesetup error") );
    return false;
  }

  if ( (unsigned int)m_map_pick->GetSelection() >= m_map_pick->GetCount()-1 ) {
    wxLogWarning( _T("no map selected") );
    customMessageBox(SL_MAIN_ICON, _("You have to select a map first."), _("Gamesetup error") );
    return false;
  }

  if ( m_battle.GetNumBots() == 1 )
  {
      wxLogWarning(_T("trying to start sp game without bot"));
      if ( customMessageBox(SL_MAIN_ICON, _("Continue without adding a bot first?.\n The game will be over pretty fast.\n "),
                _("No Bot added"), wxYES_NO) == wxNO )
        return false;
  }
  return true;
}


void SinglePlayerTab::OnMapSelect( wxCommandEvent& event )
{
  unsigned int index = (unsigned int)m_map_pick->GetCurrentSelection();
  SetMap( index );
}


void SinglePlayerTab::OnModSelect( wxCommandEvent& event )
{
  unsigned int index = (unsigned int)m_mod_pick->GetCurrentSelection();
  SetMod( index );
}


void SinglePlayerTab::OnAddBot( wxCommandEvent& event )
{
  if ( m_battle.GetNumBots() > 15 )
  {
    customMessageBoxNoModal( SL_MAIN_ICON, _("Spring only supports up to 16 different teams"), _("Num players error"), wxICON_EXCLAMATION );
    return;
  }
  AddBotDialog dlg( this, m_battle, true );
  if ( dlg.ShowModal() == wxID_OK ) {
    int x = 0, y = 0, handicap = 0;
    m_battle.GetFreePosition( x, y );
    wxColour col = m_battle.GetFreeColour( NULL );
    int i = m_battle.AddBot( m_battle.GetFreeAlly(), x, y, handicap, dlg.GetAI() );
    BattleBot* bot = m_battle.GetBot( i );
    ASSERT_LOGIC( bot != 0, _T("bot == 0") );
    bot->bs.colour = col;
    //m_minimap->UpdateMinimap();
  }
}


void SinglePlayerTab::OnStart( wxCommandEvent& event )
{
  if ( m_ui.IsSpringRunning() ) {
    wxLogWarning(_T("trying to start spring while another instance is running") );
    customMessageBoxNoModal(SL_MAIN_ICON, _("You cannot start a spring instance while another is already running"), _("Spring error"), wxICON_EXCLAMATION );
    return;
  }

  if ( ValidSetup() ) m_ui.StartSinglePlayerGame( m_battle );
}


void SinglePlayerTab::OnRandomCheck( wxCommandEvent& event )
{

    if ( m_random_check->IsChecked() ) m_battle.CustomBattleOptions().setSingleOption( _T("startpostype"), i2s(IBattle::ST_Random), OptionsWrapper::EngineOption );
    else m_battle.CustomBattleOptions().setSingleOption( _T("startpostype"), i2s(IBattle::ST_Pick), OptionsWrapper::EngineOption );
    m_battle.SendHostInfo( IBattle::HI_StartType );

}

void SinglePlayerTab::Update( const wxString& Tag )
{
  long type;
  Tag.BeforeFirst( '_' ).ToLong( &type );
  wxString key = Tag.AfterFirst( '_' );
  wxString value = m_battle.CustomBattleOptions().getSingleValue( key, (OptionsWrapper::GameOption)type);
  long longval;
  value.ToLong( &longval );
  if ( type == OptionsWrapper::PrivateOptions )
  {
    if ( key == _T("mapname") )
    {
      m_addbot_btn->Enable( false );
      try
      {
        m_map_pick->SetSelection( usync().GetMapIndex( m_battle.GetHostMapName() ) );
        UpdateMinimap();
        m_addbot_btn->Enable( true );
      } catch (...) {}
    }
  }
}

void SinglePlayerTab::UpdatePresetList()
{
}

void SinglePlayerTab::OnReset( wxCommandEvent& event )
{

}
