/* Copyright (C) 2007, 2008 The SpringLobby Team. All rights reserved. */
//
// Class: BattleRoomTab
//

#include <wx/splitter.h>
#include <wx/intl.h>
#include <wx/combobox.h>
#include <wx/stattext.h>
#include <wx/statline.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/listctrl.h>
#include <wx/sizer.h>
#include <wx/msgdlg.h>
#include <wx/settings.h>
#include <wx/colordlg.h>
#include <wx/colour.h>
#include <wx/log.h>
#include <wx/bmpcbox.h>
#include <wx/image.h>

#include <stdexcept>

#include "battleroomtab.h"
#include "ui.h"
#include "iunitsync.h"
#include "user.h"
#include "battle.h"
#include "utils.h"
#include "battleroomlistctrl.h"
#include "chatpanel.h"
#include "mapctrl.h"
#include "uiutils.h"
#include "addbotdialog.h"
#include "server.h"
#include "iconimagelist.h"
#include "settings++/custom_dialogs.h"
#include "autobalancedialog.h"
#include "settings.h"
#include "Helper/colorbutton.h"

#ifndef HAVE_WX26
#include "aui/auimanager.h"
#endif

#if wxUSE_TOGGLEBTN
#include <wx/tglbtn.h>
#endif

BEGIN_EVENT_TABLE(BattleRoomTab, wxPanel)

    EVT_BUTTON ( BROOM_START, BattleRoomTab::OnStart )
    EVT_BUTTON ( BROOM_LEAVE, BattleRoomTab::OnLeave )
    EVT_BUTTON ( BROOM_ADDBOT, BattleRoomTab::OnAddBot )

    EVT_CHECKBOX( BROOM_IMREADY, BattleRoomTab::OnImReady )
    EVT_CHECKBOX( BROOM_SPEC, BattleRoomTab::OnImSpec )
    EVT_COMBOBOX( BROOM_TEAMSEL, BattleRoomTab::OnTeamSel )
    EVT_COMBOBOX( BROOM_ALLYSEL, BattleRoomTab::OnAllySel )
    EVT_BUTTON( BROOM_COLOURSEL, BattleRoomTab::OnColourSel )
    EVT_COMBOBOX( BROOM_SIDESEL, BattleRoomTab::OnSideSel )

    EVT_COMBOBOX( BROOM_PRESETSEL, BattleRoomTab::OnPresetSel )

    #if  wxUSE_TOGGLEBTN
    EVT_TOGGLEBUTTON( BROOM_AUTOHOST, BattleRoomTab::OnAutoHost )
    EVT_TOGGLEBUTTON( BROOM_AUTOLOCK, BattleRoomTab::OnAutoLock )
    EVT_TOGGLEBUTTON( BROOM_LOCK, BattleRoomTab::OnLock )
    #else
    EVT_CHECKBOX( BROOM_AUTOHOST, BattleRoomTab::OnAutoHost )
    EVT_CHECKBOX( BROOM_AUTOLOCK, BattleRoomTab::OnAutoLock )
    EVT_CHECKBOX( BROOM_LOCK, BattleRoomTab::OnLock )
    #endif

    EVT_BUTTON( BROOM_MANAGE_MENU, BattleRoomTab::OnShowManagePlayersMenu )

    EVT_MENU( BROOM_LOCK_BALANCE, BattleRoomTab::OnLockBalance )
    EVT_MENU ( BROOM_BALANCE, BattleRoomTab::OnBalance )
    EVT_MENU ( BROOM_FIXID, BattleRoomTab::OnFixTeams )
    EVT_MENU ( BROOM_FIXCOLOURS, BattleRoomTab::OnFixColours )

END_EVENT_TABLE()

template < int N, int S = 1 >
class MyStrings : public wxArrayString
{
    public:
    MyStrings()
    {
        for ( int i = S; i <= N; ++i)
            Add( TowxString(i) );
    }
};

const MyStrings<16> team_choices;

BattleRoomTab::BattleRoomTab( wxWindow* parent, Ui& ui, Battle& battle ) :
        wxScrolledWindow( parent, -1 ),m_ui(ui), m_battle(battle)
{
#ifndef HAVE_WX26
    GetAui().manager->AddPane( this, wxLEFT, _T("battleroomtab") );
#endif
    // Create all widgets
    m_splitter = new wxSplitterWindow( this, -1, wxDefaultPosition, wxSize(100, 60) );

    UserBattleStatus& myself = m_battle.GetMe().BattleStatus();

    m_player_panel = new wxPanel( m_splitter , -1 );
    m_team_sel = new wxComboBox( m_player_panel, BROOM_TEAMSEL, _T("1"), wxDefaultPosition, wxSize(50,CONTROL_HEIGHT), team_choices );
    m_team_sel->SetToolTip(TE(_("Players with the same team number share control of their units.")));
    m_ally_sel = new wxComboBox( m_player_panel, BROOM_ALLYSEL, _T("1"), wxDefaultPosition, wxSize(50,CONTROL_HEIGHT), team_choices );
    m_ally_sel->SetToolTip(TE(_("Players with the same ally number work together to achieve victory.")));
    m_color_sel = new ColorButton( m_player_panel, BROOM_COLOURSEL, myself.colour, wxDefaultPosition, wxSize(-1,CONTROL_HEIGHT) );
    m_color_sel->SetToolTip(TE(_("Select a color to identify your units in-game")));
    m_side_sel = new wxBitmapComboBox( m_player_panel, BROOM_SIDESEL, _T(""), wxDefaultPosition, wxSize(-1,CONTROL_HEIGHT) );
    m_side_sel->SetToolTip(TE(_("Select your faction")));
    m_spec_chk = new wxCheckBox( m_player_panel, BROOM_SPEC, _("Spectator"), wxDefaultPosition, wxSize(-1,CONTROL_HEIGHT) );
    m_spec_chk->SetToolTip(TE(_("Spectate (watch) the battle instead of playing")));
    m_ready_chk = new wxCheckBox( m_player_panel, BROOM_IMREADY, _("I'm ready"), wxDefaultPosition, wxSize(-1,CONTROL_HEIGHT) );
    m_ready_chk->SetToolTip(TE(_("Click this if you are content with the battle settings.")));

    try
    {
        wxArrayString sides = usync().GetSides( m_battle.GetHostModName() );
        for ( int i = 0; i < sides.GetCount(); i++ )
        {
            m_side_sel->Append( sides[i], icons().GetBitmap( icons().GetSideIcon( m_battle.GetHostModName(), i ) ) );
        }
    }
    catch (...) {}

    m_team_lbl = new wxStaticText( m_player_panel, -1, _("Team") );
    m_ally_lbl = new wxStaticText( m_player_panel, -1, _("Ally") );
    m_color_lbl = new wxStaticText( m_player_panel, -1, _("Color") );
    m_side_lbl = new wxStaticText( m_player_panel, -1, _("Side") );

    m_map_lbl = new wxStaticText( this, -1, RefineMapname( battle.GetHostMapName() ) );
    m_size_lbl = new wxStaticText( this, -1, _T("") );
    m_wind_lbl = new wxStaticText( this, -1, _T("") );
    m_tidal_lbl = new wxStaticText( this, -1, _T("") );

    m_minimap = new MapCtrl( this, 162, &m_battle, m_ui, true, true, true, false );
    m_minimap->SetToolTip(TE(_("A preview of the selected map.  You can see the starting positions, or (if set) starting boxes.")));

    m_players = new BattleroomListCtrl( m_player_panel, battle, m_ui );
    m_chat = new ChatPanel( m_splitter, m_ui, battle );

    m_command_line = new wxStaticLine( this, -1, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );

    m_leave_btn = new wxButton( this, BROOM_LEAVE, _("Leave"), wxDefaultPosition, wxSize(-1,CONTROL_HEIGHT) );
    m_leave_btn->SetToolTip(TE(_("Leave the battle and return to the battle list")));
    m_start_btn = new wxButton( this, BROOM_START, _("Start"), wxDefaultPosition, wxSize(-1,CONTROL_HEIGHT) );
    m_start_btn->SetToolTip(TE(_("Start the battle")));

    m_manage_players_btn = new wxButton( this, BROOM_MANAGE_MENU, _("Player Management"), wxDefaultPosition, wxSize(-1,CONTROL_HEIGHT) );
    m_manage_players_btn->SetToolTip(TE(_("Various functions to make team games simplers to setup")));

    m_addbot_btn = new wxButton( this, BROOM_ADDBOT, _("Add Bot..."), wxDefaultPosition, wxSize(-1,CONTROL_HEIGHT) );
    m_addbot_btn->SetToolTip(TE(_("Add a computer-controlled player to the game")));
    #if wxUSE_TOGGLEBTN
    m_autolock_chk = new wxToggleButton( this, BROOM_AUTOLOCK , _("Autolock on start"), wxDefaultPosition , wxSize( -1, CONTROL_HEIGHT), 0 );
    #else
    m_autolock_chk = new wxCheckBox( this, BROOM_AUTOLOCK, _("Autolock on start"), wxDefaultPosition, wxSize(-1,CONTROL_HEIGHT) );
    #endif
    m_autolock_chk->SetToolTip(TE(_("Automatically locks the battle when the game starts and unlock when it's finished.")));
    m_autolock_chk->SetValue( sett().GetLastAutolockStatus() );

    #if wxUSE_TOGGLEBTN
    m_lock_chk = new wxToggleButton( this, BROOM_LOCK , _("Locked"), wxDefaultPosition , wxSize( -1,CONTROL_HEIGHT ), 0 );
    #else
    m_lock_chk = new wxCheckBox( this, BROOM_LOCK, _("Locked"), wxDefaultPosition, wxSize(-1,CONTROL_HEIGHT) );
    #endif
    m_lock_chk->SetToolTip(TE(_("Prevent additional players from joining the battle")));
    #if wxUSE_TOGGLEBTN
    m_autohost_chk = new wxToggleButton( this, BROOM_AUTOHOST , _("Autohost"), wxDefaultPosition , wxSize( -1,CONTROL_HEIGHT ), 0 );
    #else
    m_autohost_chk = new wxCheckBox( this, BROOM_AUTOHOST, _("Autohost"), wxDefaultPosition, wxSize(-1,CONTROL_HEIGHT) );
    #endif
    m_autohost_chk->SetToolTip(TE(_("Toggle autohost mode.  This allows players to control your battle using commands like '!balance' and '!start'.")));

    m_manage_users_mnu = new wxMenu();

    m_lock_balance_mnu = new wxMenuItem( m_manage_users_mnu, BROOM_LOCK_BALANCE, _( "Lock Balance" ), _("When activated, prevents anyone but the host to change team and ally"), wxITEM_CHECK );
    m_manage_users_mnu->Append( m_lock_balance_mnu );
    m_lock_balance_mnu->Check( false );

    wxMenuItem* m_balance_mnu = new wxMenuItem( m_manage_users_mnu, BROOM_BALANCE, _( "Balance alliances" ), _("Automatically balance players into two or more alliances") );
    m_manage_users_mnu->Append( m_balance_mnu );

    wxMenuItem* m_fix_colours_mnu = new wxMenuItem( m_manage_users_mnu, BROOM_FIXCOLOURS, _( "Fix colours" ), _("Make player colors unique") );
    m_manage_users_mnu->Append( m_fix_colours_mnu );

    wxMenuItem* m_fix_team_mnu = new wxMenuItem( m_manage_users_mnu, BROOM_FIXID, _( "Balance teams" ), _("Automatically balance players into control teams, by default none shares control") );
    m_manage_users_mnu->Append( m_fix_team_mnu );

    m_options_preset_sel = new wxComboBox( this, BROOM_PRESETSEL, sett().GetModDefaultPresetName( m_battle.GetHostModName() ), wxDefaultPosition, wxDefaultSize,  sett().GetPresetList(), wxCB_READONLY );
    m_options_preset_sel->SetToolTip(TE(_("Load battle preset")));

    m_opts_list = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_NO_HEADER|wxLC_REPORT );
    m_opts_list->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
    m_opts_list->SetFont( wxFont( 8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_LIGHT ) );
    wxListItem col;

    col.SetText( _("Option") );
    m_opts_list->InsertColumn( 0, col );
    col.SetText( _("Value") );
    m_opts_list->InsertColumn( 1, col );

    long pos = 0;

    m_opts_list->InsertItem( pos, _("Size") );
    m_opt_list_map[ _("Size") ] = pos++;
    m_opts_list->InsertItem( pos , _("Windspeed") );
    m_opt_list_map[ _("Windspeed") ] = pos++;
    m_opts_list->InsertItem( pos, _("Tidal strength") );
    m_opt_list_map[ _("Tidal strength") ] = pos++;

    m_opts_list->InsertItem( pos++, wxEmptyString );
    pos = AddMMOptionsToList( pos++, OptionsWrapper::EngineOption );
    m_opts_list->InsertItem( pos++, wxEmptyString );
    pos = AddMMOptionsToList( pos, OptionsWrapper::ModOption );
    m_opts_list->InsertItem( pos++, wxEmptyString );
    m_map_opts_index = pos;
    pos = AddMMOptionsToList( pos, OptionsWrapper::MapOption );


    // Create Sizers
    m_players_sizer = new wxBoxSizer( wxVERTICAL );
    m_player_sett_sizer = new wxBoxSizer( wxHORIZONTAL );
    m_info_sizer = new wxBoxSizer( wxVERTICAL );
    m_top_sizer = new wxBoxSizer( wxHORIZONTAL );
    m_buttons_sizer = new wxBoxSizer( wxHORIZONTAL );
    //m_info1_sizer = new wxBoxSizer( wxHORIZONTAL );
    m_main_sizer = new wxBoxSizer( wxVERTICAL );

    // Put widgets in place
    m_player_sett_sizer->Add( m_team_lbl, 0, wxEXPAND | wxALL, 2 );
    m_player_sett_sizer->Add( m_team_sel, 0, wxEXPAND | wxALL, 2 );
    m_player_sett_sizer->Add( m_ally_lbl, 0, wxEXPAND | wxALL, 2 );
    m_player_sett_sizer->Add( m_ally_sel, 0, wxEXPAND | wxALL, 2 );
    m_player_sett_sizer->Add( m_color_lbl, 0, wxEXPAND | wxALL, 2 );
    m_player_sett_sizer->Add( m_color_sel, 0, wxEXPAND | wxALL, 2 );
    m_player_sett_sizer->Add( m_side_lbl, 0, wxEXPAND | wxALL, 2 );
    m_player_sett_sizer->Add( m_side_sel, 0, wxEXPAND | wxALL, 2 );
    m_player_sett_sizer->Add( m_spec_chk, 0, wxEXPAND | wxALL, 2 );
    m_player_sett_sizer->Add( m_ready_chk, 0, wxEXPAND | wxALL, 2 );

    m_players_sizer->Add( m_players, 1, wxEXPAND );
    m_players_sizer->Add( m_player_sett_sizer, 0, wxEXPAND );

    m_player_panel->SetSizer( m_players_sizer );

    m_splitter->SplitHorizontally( m_player_panel, m_chat );

    //m_info1_sizer->Add( m_wind_lbl, 1, wxEXPAND );
    //m_info1_sizer->Add( m_size_lbl, 1, wxEXPAND );

    m_info_sizer->Add( m_minimap, 0, wxEXPAND );
    m_info_sizer->Add( m_map_lbl, 0, wxEXPAND );
    //m_info_sizer->Add( m_info1_sizer, 0, wxEXPAND );
    //m_info_sizer->Add( m_tidal_lbl, 0, wxEXPAND );
    m_info_sizer->Add( m_opts_list, 1, wxEXPAND | wxTOP, 4 );
    m_info_sizer->Add( m_options_preset_sel, 0, wxEXPAND | wxTOP, 4 );


    m_top_sizer->Add( m_splitter, 1, wxEXPAND | wxALL, 2 );
    m_top_sizer->Add( m_info_sizer, 0, wxEXPAND | wxALL, 2 );

    m_buttons_sizer->Add( m_leave_btn, 0, wxEXPAND | wxALL, 2 );
    m_buttons_sizer->AddStretchSpacer();
    m_buttons_sizer->Add( m_addbot_btn, 0, wxEXPAND | wxALL, 2 );
    m_buttons_sizer->Add( m_autohost_chk, 0, wxEXPAND | wxALL, 2 );
    m_buttons_sizer->Add( m_autolock_chk, 0, wxEXPAND | wxALL, 2 );
    m_buttons_sizer->Add( m_lock_chk, 0, wxEXPAND | wxALL, 2 );
    m_buttons_sizer->Add( m_manage_players_btn, 0, wxEXPAND | wxALL, 2 );
    m_buttons_sizer->Add( m_start_btn, 0, wxEXPAND | wxALL, 2 );

    m_main_sizer->Add( m_top_sizer, 1, wxEXPAND );
    m_main_sizer->Add( m_command_line, 0, wxEXPAND );
    m_main_sizer->Add( m_buttons_sizer, 0, wxEXPAND );

    m_splitter->SetMinimumPaneSize( 240 );

    for ( UserList::user_map_t::size_type i = 0; i < battle.GetNumUsers(); i++ )
    {
        m_players->AddUser( battle.GetUser( i ) );
    }

    if ( !IsHosted() )
    {
        m_options_preset_sel->Disable();
        m_options_preset_sel->SetToolTip(TE(_("Only the host can change the game options")));

        m_start_btn->Disable();
        m_start_btn->SetToolTip(TE(_("Only the host can start the battle.")));

        m_manage_players_btn->Disable();
        m_manage_players_btn->SetToolTip(TE(_("Only the host can use those functions.")));

        m_lock_chk->Disable();
        m_lock_chk->SetToolTip(TE(_("Only the host can lock the game.")));

        m_autohost_chk->Disable();
        m_autohost_chk->SetToolTip(TE(_("Only the host can toggle autohost mode.")));

        m_autolock_chk->Disable();
        m_autolock_chk->SetToolTip(TE(_("Only the host can lock the game.")));
    }
    else
    {
        m_battle.SetImReady ( true );
        m_ready_chk->Disable();
    }

    UpdateBattleInfo( wxString::Format( _T("%d_mapname"), OptionsWrapper::PrivateOptions ) );
    UpdateBattleInfo();

    SetScrollRate( 3, 3 );
    SetSizer( m_main_sizer );
    Layout();
    unsigned int widthfraction = m_opts_list->GetClientSize().GetWidth() / 3;
    m_opts_list->SetColumnWidth( 0, widthfraction * 2 );
    m_opts_list->SetColumnWidth( 1, widthfraction );

}


BattleRoomTab::~BattleRoomTab()
{
#ifndef HAVE_WX26
    if (GetAui().manager)GetAui().manager->DetachPane( this );
#endif
}


bool BattleRoomTab::IsHosted()
{
    return m_battle.IsFounderMe();
}

wxString _GetStartPosStr( IBattle::StartType t )
{
    switch ( t )
    {
    case IBattle::ST_Fixed:
        return _("Fixed");
    case IBattle::ST_Random:
        return _("Random");
    case IBattle::ST_Choose:
        return _("Boxes");
    case IBattle::ST_Pick:
        return _("Pick");
    default:
        return _T("?");
    };
}


wxString _GetGameTypeStr( IBattle::GameType t )
{
    switch ( t )
    {
    case IBattle::GT_ComContinue:
        return _("Continue");
    case IBattle::GT_ComEnds:
        return _("End");
    case IBattle::GT_Lineage:
        return _("Lineage");
    default:
        return _T("?");
    };
}


void BattleRoomTab::UpdateBattleInfo()
{
    m_lock_chk->SetValue( m_battle.IsLocked() );
    m_minimap->UpdateMinimap();
}


void BattleRoomTab::UpdateBattleInfo( const wxString& Tag )
{
    long index = m_opt_list_map[ Tag ];
    OptionsWrapper::GameOption type = (OptionsWrapper::GameOption)s2l(Tag.BeforeFirst( '_' ));
    wxString key = Tag.AfterFirst( '_' );
    wxString value;
    if ( ( type == OptionsWrapper::MapOption ) || ( type == OptionsWrapper::ModOption ) || ( type == OptionsWrapper::EngineOption ) )
    {
        OptionType DataType = m_battle.CustomBattleOptions().GetSingleOptionType( key );
        if ( DataType == opt_bool )
        {
            long boolval;
            m_battle.CustomBattleOptions().getSingleValue( key, (OptionsWrapper::GameOption)type ).ToLong( &boolval );
            m_opts_list->SetItem( index, 1, bool2yn( boolval ) );
        }
        else
        {
            m_opts_list->SetItem( index, 1, m_battle.CustomBattleOptions().getSingleValue( key, (OptionsWrapper::GameOption)type ) );
        }
    }
    else if ( type == OptionsWrapper::PrivateOptions )
    {
        if ( key == _T("mapname") ) // the map has been changed
        {
            try   // updates map info summary
            {
                ASSERT_EXCEPTION( m_battle.MapExists(), _("Map does not exist.") );
                UnitSyncMap map = m_battle.LoadMap();
                m_map_lbl->SetLabel( RefineMapname( map.name ) );
                m_opts_list->SetItem( m_opt_list_map[ _("Size") ] , 1, wxString::Format( _T("%.0fx%.0f"), map.info.width/512.0, map.info.height/512.0 ) );
                m_opts_list->SetItem( m_opt_list_map[ _("Windspeed") ], 1, wxString::Format( _T("%d-%d"), map.info.minWind, map.info.maxWind) );
                m_opts_list->SetItem( m_opt_list_map[ _("Tidal strength") ], 1, wxString::Format( _T("%d"), map.info.tidalStrength) );
                //    m_opts_list->SetItem( 0, 1,  );
            }
            catch (...)
            {
                m_map_lbl->SetLabel( RefineMapname( m_battle.GetHostMapName() ) );
                m_opts_list->SetItem( m_opt_list_map[ _("Size") ], 1, _T("?x?") );
                m_opts_list->SetItem( m_opt_list_map[ _("Windspeed") ], 1, _T("?-?") );
                m_opts_list->SetItem( m_opt_list_map[ _("Tidal strength") ], 1, _T("?") );
            }

            //delete any eventual map option from the list and add options of the new map
            for ( long i = m_map_opts_index; i < m_opts_list->GetItemCount(); i++ ) m_opts_list->DeleteItem( i );
            m_battle.CustomBattleOptions().loadOptions( OptionsWrapper::MapOption, m_battle.GetHostModName() );
            AddMMOptionsToList( m_map_opts_index, OptionsWrapper::MapOption );

            m_minimap->UpdateMinimap();

        }
        else if ( key == _T("restrictions") )
        {
            m_opts_list->SetItem( index, 1, bool2yn( m_battle.DisabledUnits().GetCount() > 0 ) );
        }
    }
}

BattleroomListCtrl& BattleRoomTab::GetPlayersListCtrl()
{
    ASSERT_LOGIC( m_players != 0, _T("m_players = 0") );
    return *m_players;
}


void BattleRoomTab::UpdateUser( User& user )
{
    m_players->UpdateUser( user );
    if ( &user != &m_battle.GetMe() ) return;

    UserBattleStatus& bs = user.BattleStatus();
    m_team_sel->SetSelection( bs.team );
    m_ally_sel->SetSelection( bs.ally );
    m_side_sel->SetSelection( bs.side );
    m_spec_chk->SetValue( bs.spectator );

    // Enable or disable widgets' sensitivity as appropriate.
    if ( bs.spectator )
    {
        m_ready_chk->SetValue ( true );
        m_ready_chk->Disable();
        m_side_sel->Disable();
        m_ally_sel->Disable();
        m_team_sel->Disable();
    }
    else
    {
        if ( !IsHosted() )
            m_ready_chk->Enable();

        m_ready_chk->SetValue( bs.ready );
        m_side_sel->Enable();
        m_ally_sel->Enable();
        m_team_sel->Enable();
    }

    icons().SetColourIcon( bs.team, user.BattleStatus().colour );
    m_color_sel->SetColor( user.BattleStatus().colour );

    m_minimap->UpdateMinimap();

    UpdateHighlights();
}


Battle& BattleRoomTab::GetBattle()
{
    return m_battle;
}


ChatPanel& BattleRoomTab::GetChatPanel()
{
    wxLogDebugFunc(_T(""));
    ASSERT_LOGIC( m_chat != 0, _T("m_chat = 0") );
    return *m_chat;
}


void BattleRoomTab::OnStart( wxCommandEvent& event )
{
    if ( m_battle.HaveMultipleBotsInSameTeam() )
    {
        wxMessageDialog dlg( this, _("There are two or more bots on the same team.  Because bots don't know how to share, this won't work."), _("Bot team sharing."), wxOK );
        dlg.ShowModal();
        return;
    }
    m_battle.GetMe().BattleStatus().ready = true;

    if ( !m_battle.IsEveryoneReady() )
    {
        wxMessageDialog dlg1( this, _("Some players are not ready yet.\nRing these players?"), _("Not ready"), wxYES_NO );
        if ( dlg1.ShowModal() == wxID_YES )
        {
            m_battle.RingNotReadyPlayers();
            return;
        }
        wxMessageDialog dlg2( this, _("Force start?"), _("Not ready"), wxYES_NO );
        if ( dlg2.ShowModal() == wxID_NO ) return;
    }
    if ( m_battle.GetNumUsers() > 32 )
    {
      customMessageBoxNoModal( SL_MAIN_ICON, _("You have more than 32 players (including spectators) in your battle\nSpring supports maximum 32"), _("Num players error"), wxICON_EXCLAMATION );
      return;
    }
    m_ui.StartHostedBattle();
}


void BattleRoomTab::OnLeave( wxCommandEvent& event )
{
    m_battle.Leave();
}



void BattleRoomTab::OnBalance( wxCommandEvent& event )
{
    AutoBalanceDialog::BalanceOptions defaultval;
    defaultval.type = (IBattle::BalanceType)sett().GetBalanceMethod();
    defaultval.respectclans = sett().GetBalanceClans();
    defaultval.strongclans = sett().GetBalanceStrongClans();
    defaultval.groupingsize = sett().GetBalanceGrouping();
    AutoBalanceDialog dlg( this, defaultval );
    if ( dlg.ShowModal() == wxID_OK )
    {
        AutoBalanceDialog::BalanceOptions balance = dlg.GetResult();
        sett().SetBalanceMethod( balance.type );
        sett().SetBalanceClans( balance.respectclans );
        sett().SetBalanceStrongClans( balance.strongclans );
        sett().SetBalanceGrouping( balance.groupingsize );
        m_battle.Autobalance( balance.type, balance.respectclans, balance.strongclans, balance.groupingsize );
    }
}


void BattleRoomTab::OnFixTeams( wxCommandEvent& event )
{
    AutoBalanceDialog::BalanceOptions defaultval;
    defaultval.type = (IBattle::BalanceType)sett().GetFixIDMethod();
    defaultval.respectclans = sett().GetFixIDClans();
    defaultval.strongclans = sett().GetFixIDStrongClans();
    defaultval.groupingsize = sett().GetFixIDGrouping();
    AutoBalanceDialog dlg( this, defaultval );
    if ( dlg.ShowModal() == wxID_OK )
    {
        AutoBalanceDialog::BalanceOptions balance = dlg.GetResult();
        sett().SetFixIDMethod( balance.type );
        sett().SetFixIDClans( balance.respectclans );
        sett().SetFixIDStrongClans( balance.strongclans );
        sett().SetFixIDGrouping( balance.groupingsize );
        m_battle.FixTeamIDs( balance.type, balance.respectclans, balance.strongclans, balance.groupingsize );
    }
}


void BattleRoomTab::OnFixColours( wxCommandEvent& event )
{
    wxLogMessage(_T(""));
    if (!IsHosted()) // Works with autohosts, and human hosts knows what it mean.
    {
        m_battle.Say(_T("!fixcolors"));
        return;
    }
    //m_battle.Say(_T("fixing colours"));
    m_battle.FixColours();
}


void BattleRoomTab::OnAddBot( wxCommandEvent& event )
{
    //customMessageBox(SL_MAIN_ICON,_T("Max players reached"),_T("Cannot add bot, maximum number of players already reached.") );
    AddBotDialog dlg( this, m_battle );
    if ( dlg.ShowModal() == wxID_OK )
    {
        UserBattleStatus bs;
        bs.team = m_battle.GetFreeTeamNum( false );
        bs.ally = bs.team;
        bs.sync = SYNC_SYNCED;
        bs.spectator = false;
        bs.side = 0;
        bs.ready = true;
        bs.handicap = 0;
        bs.colour = m_battle.GetFreeColour( NULL );
        m_ui.GetServer().AddBot( m_battle.GetBattleId(), dlg.GetNick(), m_battle.GetMe().GetNick(), bs, dlg.GetAI() );
    }
}


void BattleRoomTab::OnImReady( wxCommandEvent& event )
{
    m_battle.SetImReady( m_ready_chk->GetValue() );
}


void BattleRoomTab::OnLock( wxCommandEvent& event )
{
    m_battle.SetIsLocked( m_lock_chk->GetValue() );
    m_battle.SendHostInfo( IBattle::HI_Locked );
}


void BattleRoomTab::OnAutoHost( wxCommandEvent& event )
{
    m_battle.GetAutoHost().SetEnabled( m_autohost_chk->GetValue() );
}


void BattleRoomTab::OnImSpec( wxCommandEvent& event )
{
    UserBattleStatus& bs = m_battle.GetMe().BattleStatus();
    bs.spectator = m_spec_chk->GetValue();
    //m_battle.GetMe().SetBattleStatus( bs );
    m_battle.SendMyBattleStatus();
}


void BattleRoomTab::OnTeamSel( wxCommandEvent& event )
{
    User& u = m_battle.GetMe();
    UserBattleStatus& bs = u.BattleStatus();
    m_team_sel->GetValue().ToULong( (unsigned long*)&bs.team );
    bs.team--;
    //u.SetBattleStatus( bs );
    m_battle.SendMyBattleStatus();
}


void BattleRoomTab::OnAllySel( wxCommandEvent& event )
{
    User& u = m_battle.GetMe();
    UserBattleStatus& bs = u.BattleStatus();
    m_ally_sel->GetValue().ToULong( (unsigned long*)&bs.ally );
    bs.ally--;
    //u.SetBattleStatus( bs );
    m_battle.SendMyBattleStatus();
}


void BattleRoomTab::OnColourSel( wxCommandEvent& event )
{
    User& u = m_battle.GetMe();
    UserBattleStatus& bs = u.BattleStatus();
    wxColour CurrentColour = bs.colour;
    CurrentColour = GetColourFromUser(this, CurrentColour);
    if ( !CurrentColour.IsColourOk() ) return;
    bs.colour = CurrentColour;
    sett().SetBattleLastColour( CurrentColour );
    //u.SetBattleStatus( bs );
    m_battle.SendMyBattleStatus();
}


void BattleRoomTab::OnSideSel( wxCommandEvent& event )
{
    User& u = m_battle.GetMe();
    UserBattleStatus& bs = u.BattleStatus();
    bs.side = m_side_sel->GetSelection();
    //u.SetBattleStatus( bs );
    m_battle.SendMyBattleStatus();
}


void BattleRoomTab::OnPresetSel( wxCommandEvent& event )
{
    wxString presetname = m_options_preset_sel->GetValue();
    if ( presetname.IsEmpty() ) return;
    m_battle.LoadOptionsPreset( presetname );
    m_battle.SendHostInfo( IBattle::HI_Send_All_opts );
}

void BattleRoomTab::OnAutoLock( wxCommandEvent& event )
{
    m_battle.SetAutoLockOnStart( m_autolock_chk->GetValue() );
    sett().SetLastAutolockStatus( m_autolock_chk->GetValue() );
}


void BattleRoomTab::OnLockBalance( wxCommandEvent& event )
{
  bool locked = m_lock_balance_mnu->IsChecked();
  m_battle.SetLockExternalBalanceChanges( locked );
}

void BattleRoomTab::OnShowManagePlayersMenu( wxCommandEvent& event )
{
  PopupMenu( m_manage_users_mnu );
}

void BattleRoomTab::OnUserJoined( User& user )
{
    m_chat->Joined( user );
    m_players->AddUser( user );
    if ( &user == &m_battle.GetMe() )
    {
        m_players->SetItemState( m_players->GetUserIndex( user ), wxLIST_MASK_STATE, wxLIST_STATE_SELECTED );
    }
}


void BattleRoomTab::OnUserLeft( User& user )
{
    m_chat->Parted( user, wxEmptyString );
    m_players->RemoveUser( user );
}


void BattleRoomTab::OnBotAdded( User& bot )
{
    m_players->AddUser( bot );
}


void BattleRoomTab::OnBotRemoved( User& bot )
{
    m_players->RemoveUser( bot );
}


void BattleRoomTab::OnBotUpdated( User& bot )
{
    m_players->UpdateUser( bot );
}


void BattleRoomTab::OnUnitSyncReloaded()
{
    m_minimap->UpdateMinimap();
    UpdateBattleInfo();
    m_battle.SendMyBattleStatus(); // This should reset sync status.
}

long BattleRoomTab::AddMMOptionsToList( long pos, OptionsWrapper::GameOption optFlag )
{
    OptionsWrapper::wxStringTripleVec optlist = m_battle.CustomBattleOptions().getOptions( optFlag );
    for (OptionsWrapper::wxStringTripleVec::iterator it = optlist.begin(); it != optlist.end(); ++it)
    {
        m_opts_list->InsertItem( pos, it->second.first );
        m_opt_list_map[ wxString::Format(_T("%d_"), optFlag ) + it->first ] = pos;
        OptionType DataType = m_battle.CustomBattleOptions().GetSingleOptionType( it->first );
        wxString value;
        if ( DataType == opt_bool )
        {
            long boolval;
            it->second.second.ToLong( &boolval );
            value = bool2yn( boolval );
        }
        else
            value = it->second.second;
        m_opts_list->SetItem( pos, 1, value );
        pos++;
    }
    return pos;
}

void BattleRoomTab::UpdateHighlights()
{
    m_players->UpdateHighlights();
}


void BattleRoomTab::UpdatePresetList()
{
    m_options_preset_sel->Clear();
    m_options_preset_sel->Append(sett().GetPresetList());
    m_options_preset_sel->SetStringSelection(  m_battle.GetCurrentPreset() );
}


void BattleRoomTab::SortPlayerList()
{
    m_players->SortList();
}
