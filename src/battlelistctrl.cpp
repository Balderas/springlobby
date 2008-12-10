/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

#include <wx/intl.h>
#include <wx/menu.h>

#include "battlelistctrl.h"
#include "utils.h"
#include "user.h"
#include "iconimagelist.h"
#include "battle.h"
#include "uiutils.h"
#include "ui.h"
#include "server.h"
#include "countrycodes.h"
#include "settings.h"
#include "settings++/custom_dialogs.h"
#include "useractions.h"

#ifndef HAVE_WX26
#include "aui/auimanager.h"
#endif

BEGIN_EVENT_TABLE(BattleListCtrl, CustomVirtListCtrl<const Battle *>)

  EVT_LIST_ITEM_RIGHT_CLICK( BLIST_LIST, BattleListCtrl::OnListRightClick )
  EVT_LIST_COL_CLICK       ( BLIST_LIST, BattleListCtrl::OnColClick )
  EVT_MENU                 ( BLIST_DLMAP, BattleListCtrl::OnDLMap )
  EVT_MENU                 ( BLIST_DLMOD, BattleListCtrl::OnDLMod )
#if wxUSE_TIPWINDOW
#if !defined(__WXMSW__) /* && !defined(__WXMAC__) */ //disables tooltips on msw /* and mac */
  EVT_MOTION(BattleListCtrl::OnMouseMotion)
#endif
#endif
END_EVENT_TABLE()

Ui* BattleListCtrl::m_ui_for_sort = 0;

BattleListCtrl::BattleListCtrl( wxWindow* parent, Ui& ui ):
  CustomVirtListCtrl<const Battle *>(parent, BLIST_LIST, wxDefaultPosition, wxDefaultSize,
            wxSUNKEN_BORDER | wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_ALIGN_LEFT, _T("BattleListCtrl"), 10, &CompareOneCrit),
  m_ui(ui)
{

#ifndef HAVE_WX26
    GetAui().manager->AddPane( this, wxLEFT, _T("battlelistctrl") );
#endif

    const int hd = wxLIST_AUTOSIZE_USEHEADER;

#if defined(__WXMAC__)
/// on mac, autosize does not work at all
    const int widths[10] = {20,20,20,170,140,130,110,28,28,28};
#else
    const int widths[10] = {hd,hd,hd,170,140,130,110,hd,hd,hd};
#endif

    AddColumn( 0, widths[0], _T("s"), _T("Status") );
    AddColumn( 1, widths[1], _T("c"), _T("Country") );
    AddColumn( 2, widths[2], _T("r"), _T("Minimum rank to join") );
    AddColumn( 3, widths[3], _("Description"), _T("Game description") );
    AddColumn( 4, widths[4], _("Map"), _T("Mapname") );
    AddColumn( 5, widths[5], _("Mod"), _T("Modname") );
    AddColumn( 6, widths[6], _("Host"), _T("Name of the Host") );
    AddColumn( 7, widths[7], _("a"), _T("Number of Spectators") );
    AddColumn( 8, widths[8], _("p"), _T("Number of Players joined") );
    AddColumn( 9, widths[9], _("m"), _T("Maximum number of Players that can join") );

    m_sortorder[0].col = 0;
    m_sortorder[0].direction = true;
    m_sortorder[1].col = 5;
    m_sortorder[1].direction = true;
    m_sortorder[2].col = 9;
    m_sortorder[2].direction = true;
    m_sortorder[3].col = 4;
    m_sortorder[3].direction = true;

    m_popup = new wxMenu( _T("") );
    // &m enables shortcout "alt + m" and underlines m
    m_popup->Append( BLIST_DLMAP, _("Download &map") );
    m_popup->Append( BLIST_DLMOD, _("Download m&od") );
}


BattleListCtrl::~BattleListCtrl()
{
    delete m_popup;
}

wxString BattleListCtrl::OnGetItemText(long item, long column) const
{
    if ( m_data[item] == NULL )
        return wxEmptyString;

    const Battle& battle= *m_data[item];
    const BattleOptions& opts = battle.GetBattleOptions();

    switch ( column ) {
        case 0:
        case 1:
        case 2:
        default: return wxEmptyString;

        case 3: return ( opts.description );
        case 4: return ( opts.mapname );
        case 5: return ( opts.modname );
        case 6: return ( opts.founder );
        case 7: return ( wxString::Format(_T("%d"), int(battle.GetSpectators())) );
        case 8: return ( wxString::Format(_T("%d"), int(battle.GetNumUsers()) - int(battle.GetSpectators()) ) );
        case 9: return ( wxString::Format(_T("%d"), int(battle.GetMaxPlayers())) );
    }
}

int BattleListCtrl::OnGetItemImage(long item) const
{
    if ( m_data[item] == NULL )
        return -1;

    return icons().GetBattleStatusIcon( *m_data[item] );
}

int BattleListCtrl::OnGetItemColumnImage(long item, long column) const
{
    if ( m_data[item] == NULL )
        return -1;

    const Battle& battle= *m_data[item];

    switch ( column ) {
        default: return -1;

        case 0: return icons().GetBattleStatusIcon( battle );
        case 1: return icons().GetFlagIcon( battle.GetFounder().GetCountry() );
        case 2: return icons().GetRankIcon( battle.GetRankNeeded(), false );
        case 4: return battle.MapExists() ? icons().ICON_EXISTS : icons().ICON_NEXISTS;
        case 5: return battle.ModExists() ? icons().ICON_EXISTS : icons().ICON_NEXISTS;
    }
}

void BattleListCtrl::AddBattle( const Battle& battle )
{
    assert(&battle);

    m_data.push_back( &battle );
    SetItemCount( m_data.size() );
    RefreshItem( m_data.size() );
    HighlightItem( m_data.size() );
    MarkDirtySort();
}

void BattleListCtrl::RemoveBattle( const Battle& battle )
{
    int index = GetIndexFromData( &battle );

    if ( index != -1 ) {
        m_data.erase( m_data.begin() + index );
        SetItemCount( m_data.size() );
        RefreshVisibleItems( );
        return;
    }
    wxLogError( _T("Didn't find the battle to remove.") );
}

void BattleListCtrl::UpdateBattle( const Battle& battle )
{
    int index = GetIndexFromData( &battle );

    if ( index != -1 ) {
        m_data.erase( m_data.begin() + index );
        SetItemCount( m_data.size() );

        RefreshVisibleItems( );
        return;
    }
    HighlightItem( index );
    MarkDirtySort();
}

void BattleListCtrl::HighlightItem( long item )
{
    //prioritize highlighting host over joined players
    const Battle& b = ui().GetServer().GetBattle( GetItemData(item) );
    wxString host = b.GetFounder().GetNick();
    HighlightItemUser( item, host );
    if ( useractions().DoActionOnUser( m_highlightAction, host ) )
        return;

    //to avoid color flicker check first if highlighting should be done
    //and return if it should
    for ( unsigned int i = 0; i < b.GetNumUsers(); ++i){
        wxString name = b.GetUser(i).GetNick();
        HighlightItemUser( item, name );
        if ( useractions().DoActionOnUser( m_highlightAction, name ) )
            return;

    }
}

void BattleListCtrl::OnListRightClick( wxListEvent& event )
{
    PopupMenu( m_popup );
}


void BattleListCtrl::OnDLMap( wxCommandEvent& event )
{
//  if ( m_selected != -1 ) {
//    if ( m_ui.GetServer().battles_iter->BattleExists(m_selected) ) {
//      m_ui.DownloadMap( m_ui.GetServer().battles_iter->GetBattle(m_selected).GetHostMapHash(), m_ui.GetServer().battles_iter->GetBattle(m_selected).GetHostMapName() );
//    }
//  }
}


void BattleListCtrl::OnDLMod( wxCommandEvent& event )
{
//  if ( m_selected != -1 ) {
//    if ( m_ui.GetServer().battles_iter->BattleExists(m_selected) ) {
//      m_ui.DownloadMod( m_ui.GetServer().battles_iter->GetBattle(m_selected).GetHostModHash(), m_ui.GetServer().battles_iter->GetBattle(m_selected).GetHostModName() );
//    }
//  }
}


void BattleListCtrl::OnColClick( wxListEvent& event )
{
  if ( event.GetColumn() == -1 ) return;
  wxListItem col;
  GetColumn( m_sortorder[0].col, col );
  col.SetImage( icons().ICON_NONE );
  SetColumn( m_sortorder[0].col, col );

  int i;
  for ( i = 0; m_sortorder[i].col != event.GetColumn() && i < 4; ++i ) {}
  if ( i > 3 ) { i = 3; }
  for ( ; i > 0; i--) { m_sortorder[i] = m_sortorder[i-1]; }
  m_sortorder[0].col = event.GetColumn();
  m_sortorder[0].direction = !m_sortorder[0].direction;


  GetColumn( m_sortorder[0].col, col );
  //col.SetImage( ( m_sortorder[0].direction )?ICON_UP:ICON_DOWN );
  col.SetImage( ( m_sortorder[0].direction )?icons().ICON_UP:icons().ICON_DOWN );
  SetColumn( m_sortorder[0].col, col );

  SortList( true );
}


void BattleListCtrl::Sort()
{
//  bool changed = false;
//  BattleListCtrl::m_ui_for_sort = &m_ui;
//  if (!m_ui_for_sort || !m_ui_for_sort->GetServerStatus()  ) return;
//  for (int i = 3; i >= 0; i--) {
//    switch ( m_sortorder[ i ].col ) {
//      case 0 : changed = SortItems( ( m_sortorder[ i ].direction )?&CompareStatusUP:&CompareStatusDOWN , 0 ); break;
//      case 1 : changed = SortItems( ( m_sortorder[ i ].direction )?&CompareCountryUP:&CompareCountryDOWN , 0 ); break;
//      case 2 : changed = SortItems( ( m_sortorder[ i ].direction )?&CompareRankUP:&CompareRankDOWN , 0 ); break;
//      case 3 : changed = SortItems( ( m_sortorder[ i ].direction )?&CompareDescriptionUP:&CompareDescriptionDOWN , 0 ); break;
//      case 4 : changed = SortItems( ( m_sortorder[ i ].direction )?&CompareMapUP:&CompareMapDOWN , 0 ); break;
//      case 5 : changed = SortItems( ( m_sortorder[ i ].direction )?&CompareModUP:&CompareModDOWN , 0 ); break;
//      case 6 : changed = SortItems( ( m_sortorder[ i ].direction )?&CompareHostUP:&CompareHostDOWN , 0 ); break;
//      case 7 : changed = SortItems( ( m_sortorder[ i ].direction )?&CompareSpectatorsUP:&CompareSpectatorsDOWN , 0 ); break;
//      case 8 : changed = SortItems( ( m_sortorder[ i ].direction )?&ComparePlayerUP:&ComparePlayerDOWN , 0 ); break;
//      case 9 : changed = SortItems( ( m_sortorder[ i ].direction )?&CompareMaxPlayerUP:&CompareMaxPlayerDOWN , 0 ); break;
//    }
//  }
}


int wxCALLBACK BattleListCtrl::CompareStatusUP(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item1);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  int b1 = 0, b2 = 0;

  if ( battle1.GetInGame() )
    b1 += 1000;
  if ( battle2.GetInGame() )
    b2 += 1000;
  if ( battle1.IsLocked() )
    b1 += 100;
  if ( battle2.IsLocked() )
    b2 += 100;
  if ( battle1.IsPassworded() )
    b1 += 50;
  if ( battle2.IsPassworded() )
    b2 += 50;
  if ( battle1.IsFull() )
    b1 += 25;
  if ( battle2.IsFull() )
    b2 += 25;

  // inverse the order
  if ( b1 < b2 )
      return -1;
  if ( b1 > b2 )
      return 1;

  return 0;
}


int wxCALLBACK BattleListCtrl::CompareStatusDOWN(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item1);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  int b1 = 0, b2 = 0;

  if ( battle1.GetInGame() )
    b1 += 1000;
  if ( battle2.GetInGame() )
    b2 += 1000;
  if ( battle1.IsLocked() )
    b1 += 100;
  if ( battle2.IsLocked() )
    b2 += 100;
  if ( battle1.IsPassworded() )
    b1 += 50;
  if ( battle2.IsPassworded() )
    b2 += 50;
  if ( battle1.IsFull() )
    b1 += 25;
  if ( battle2.IsFull() )
    b2 += 25;

  // inverse the order
  if ( b1 < b2 )
      return 1;
  if ( b1 > b2 )
      return -1;

  return 0;
}


int wxCALLBACK BattleListCtrl::CompareRankUP(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item1);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  if ( battle1.GetRankNeeded() < battle2.GetRankNeeded() )
      return -1;
  if ( battle1.GetRankNeeded() > battle2.GetRankNeeded() )
      return 1;

  return 0;
}


int wxCALLBACK BattleListCtrl::CompareRankDOWN(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item1);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  if ( battle1.GetRankNeeded() < battle2.GetRankNeeded() )
      return 1;
  if ( battle1.GetRankNeeded() > battle2.GetRankNeeded() )
      return -1;

  return 0;
}


int wxCALLBACK BattleListCtrl::CompareCountryUP(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item1);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  return battle1.GetFounder().GetCountry().CmpNoCase(battle2.GetFounder().GetCountry());
}



int wxCALLBACK BattleListCtrl::CompareCountryDOWN(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item1);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  return battle2.GetFounder().GetCountry().CmpNoCase(battle1.GetFounder().GetCountry());
}


int wxCALLBACK BattleListCtrl::CompareDescriptionUP(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item1);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  return battle1.GetDescription().CmpNoCase(battle2.GetDescription());
}


int wxCALLBACK BattleListCtrl::CompareDescriptionDOWN(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item1);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  return battle2.GetDescription().CmpNoCase(battle1.GetDescription());
}


int wxCALLBACK BattleListCtrl::CompareMapUP(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item1);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  return RefineMapname(battle1.GetHostMapName()).CmpNoCase(RefineMapname(battle2.GetHostMapName()));
}


int wxCALLBACK BattleListCtrl::CompareMapDOWN(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item1);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  return RefineMapname(battle2.GetHostMapName()).CmpNoCase(RefineMapname(battle1.GetHostMapName()));
}


int wxCALLBACK BattleListCtrl::CompareModUP(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item1);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  return RefineModname(battle1.GetHostModName()).CmpNoCase(RefineModname(battle2.GetHostModName()));
}


int wxCALLBACK BattleListCtrl::CompareModDOWN(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item1);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  return RefineModname(battle2.GetHostModName()).CmpNoCase(RefineModname(battle1.GetHostModName()));
}


int wxCALLBACK BattleListCtrl::CompareHostUP(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item1);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  return battle1.GetFounder().GetNick().CmpNoCase(battle2.GetFounder().GetNick());
}


int wxCALLBACK BattleListCtrl::CompareHostDOWN(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item1);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  return battle2.GetFounder().GetNick().CmpNoCase(battle1.GetFounder().GetNick());
}


int wxCALLBACK BattleListCtrl::CompareSpectatorsUP(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item1);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  if ( battle1.GetSpectators() < battle2.GetSpectators() )
      return -1;
  if ( battle1.GetSpectators() > battle2.GetSpectators() )
      return 1;

  return 0;
}


int wxCALLBACK BattleListCtrl::CompareSpectatorsDOWN(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item1);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  if ( battle1.GetSpectators() < battle2.GetSpectators() )
      return 1;
  if ( battle1.GetSpectators() > battle2.GetSpectators() )
      return -1;

  return 0;
}


int wxCALLBACK BattleListCtrl::ComparePlayerUP(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item1);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  if ( battle1.GetNumUsers() - battle1.GetSpectators() < battle2.GetNumUsers() - battle2.GetSpectators() )
      return -1;
  if ( battle1.GetNumUsers() - battle1.GetSpectators() > battle2.GetNumUsers() - battle2.GetSpectators() )
      return 1;

  return 0;
}



int wxCALLBACK BattleListCtrl::ComparePlayerDOWN(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item1);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  if ( battle1.GetNumUsers() - battle1.GetSpectators() < battle2.GetNumUsers() - battle2.GetSpectators() )
      return 1;
  if ( battle1.GetNumUsers() - battle1.GetSpectators() > battle2.GetNumUsers() - battle2.GetSpectators() )
      return -1;

  return 0;
}


int wxCALLBACK BattleListCtrl::CompareMaxPlayerUP(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item1);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  if ( battle1.GetMaxPlayers() < battle2.GetMaxPlayers() )
      return -1;
  if ( battle1.GetMaxPlayers() > battle2.GetMaxPlayers() )
      return 1;

  return 0;
}


int wxCALLBACK BattleListCtrl::CompareMaxPlayerDOWN(long item1, long item2, long sortData)
{
  Ui* ui = m_ui_for_sort;
  Battle& battle1 = ui->GetServer().battles_iter->GetBattle(item1);
  Battle& battle2 = ui->GetServer().battles_iter->GetBattle(item2);

  if ( battle1.GetMaxPlayers() < battle2.GetMaxPlayers() )
      return 1;
  if ( battle1.GetMaxPlayers() > battle2.GetMaxPlayers() )
      return -1;

  return 0;
}

void BattleListCtrl::SetTipWindowText( const long item_hit, const wxPoint position)
{
    long item = GetItemData(item_hit);
    if ( m_data[item] == NULL ) {
        m_tiptext = _T("");
        return;
    }

    const Battle& battle= *m_data[item];

    int coloumn = getColoumnFromPosition(position);
    switch (coloumn)
    {
        case 0: // status
        m_tiptext = icons().GetBattleStatus(battle);
            break;
        case 1: // country
            m_tiptext = GetFlagNameFromCountryCode(battle.GetFounder().GetCountry());
            break;
        case 2: // rank_min
            m_tiptext = m_colinfovec[coloumn].first;
            break;
        case 3: // descrp
            m_tiptext = battle.GetDescription();
            break;
        case 4: //map
            m_tiptext = RefineMapname(battle.GetHostMapName());
            break;
        case 5: //mod
            m_tiptext = RefineModname(battle.GetHostModName());
            break;
        case 6: // host
            m_tiptext = battle.GetFounder().GetNick();
            break;
        case 7: // specs
            m_tiptext = _("Spectators:");
            for (unsigned int i = 0; i < battle.GetNumUsers(); ++i )
            {
                if ( battle.GetUser(i).BattleStatus().spectator ) m_tiptext << _T("\n") << battle.GetUser(i).GetNick();
            }
            break;
        case 8: // player
            m_tiptext = _("Active Players:");
            for ( unsigned int i = 0; i < battle.GetNumUsers(); ++i )
            {
                if ( !battle.GetUser(i).BattleStatus().spectator ) m_tiptext << _T("\n") << battle.GetUser(i).GetNick();
            }
            break;
        case 9: //may player
            m_tiptext = (m_colinfovec[coloumn].first);
            break;

        default: m_tiptext = _T("");
            break;
    }
}

int BattleListCtrl::CompareOneCrit( DataType u1, DataType u2, int col, int dir )
{
    return 0;
}

int BattleListCtrl::GetIndexFromData( const DataType& data ) const
{
    return -1;
}
