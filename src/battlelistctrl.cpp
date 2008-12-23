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
#include "Helper/sortutil.h"

#ifndef HAVE_WX26
#include "aui/auimanager.h"
#endif

BEGIN_EVENT_TABLE(BattleListCtrl, CustomVirtListCtrl< Battle *>)

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
  CustomVirtListCtrl< Battle *>(parent, BLIST_LIST, wxDefaultPosition, wxDefaultSize,
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
        case 4: return ( battle.GetHostMapName() );
        case 5: return ( battle.GetHostModName() );
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
//        case 4: return battle.MapExists() ? icons().ICON_EXISTS : icons().ICON_NEXISTS;
//        case 5: return battle.ModExists() ? icons().ICON_EXISTS : icons().ICON_NEXISTS;
    }
}

void BattleListCtrl::AddBattle( Battle& battle )
{
    assert(&battle);

    if ( GetIndexFromData( &battle ) != -1 ) {
        wxLogWarning( _T("Battle already in list.") );
        return;
    }
    m_data.push_back( &battle );
    SetItemCount( m_data.size() );
    RefreshItem( m_data.size() );
    HighlightItem( m_data.size() );
    MarkDirtySort();
}

void BattleListCtrl::RemoveBattle( Battle& battle )
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

void BattleListCtrl::UpdateBattle( Battle& battle )
{
    int index = GetIndexFromData( &battle );

    RefreshVisibleItems( );
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
  m_sortorder[0].direction *= -1;


  GetColumn( m_sortorder[0].col, col );
  //col.SetImage( ( m_sortorder[0].direction )?ICON_UP:ICON_DOWN );
  col.SetImage( ( m_sortorder[0].direction > 0 )?icons().ICON_UP:icons().ICON_DOWN );
  SetColumn( m_sortorder[0].col, col );

  SortList( true );
}


void BattleListCtrl::Sort()
{
    if ( m_data.size() > 0 )
    {
        SaveSelection();
        SLInsertionSort( m_data, m_comparator );
        RestoreSelection();
    }
}

int BattleListCtrl::CompareOneCrit( DataType u1, DataType u2, int col, int dir )
{
    switch ( col ) {
        case 0: return dir * CompareStatus( u1, u2 );
        case 1: return dir * u1->GetFounder().GetCountry().CmpNoCase( u2->GetFounder().GetCountry() );
        case 2: return dir * compareSimple( u1->GetRankNeeded(), u2->GetRankNeeded() );
        case 3: return dir * u1->GetDescription().CmpNoCase( u2->GetDescription() );
        case 4: return dir * RefineMapname( u1->GetHostMapName() ).CmpNoCase( RefineMapname( u2->GetHostMapName() ) );
        case 5: return dir * RefineModname( u1->GetHostModName() ).CmpNoCase( RefineModname( u2->GetHostModName() ) );
        case 6: return dir * u1->GetFounder().GetNick().CmpNoCase( u2->GetFounder().GetNick() );
        case 7: return dir * compareSimple( u1->GetSpectators(), u2->GetSpectators() );
        case 8: return dir * ComparePlayer( u1, u2 );
        case 9: return dir * compareSimple( u1->GetMaxPlayers(), u2->GetMaxPlayers() );
        default: return 0;
    }
}

int BattleListCtrl::CompareStatus( DataType u1, DataType u2 )
{
  const Battle& battle1 = *u1;
  const Battle& battle2 = *u2;

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


int BattleListCtrl::ComparePlayer( DataType u1, DataType u2 )
{
    const Battle& battle1 = *u1;
    const Battle& battle2 = *u2;

    int n1 = battle1.GetNumUsers() - battle1.GetSpectators();
    int n2 = battle2.GetNumUsers() - battle2.GetSpectators();
    return compareSimple( n1, n2 );
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


int BattleListCtrl::GetIndexFromData( const DataType& data ) const
{
    DataCIter it = m_data.begin();
    for ( int i = 0; it != m_data.end(); ++it, ++i ) {
        if ( *it != 0 && data->Equals( *(*it) ) )
            return i;
    }
    wxLogError( _T("didn't find the battle.") );
    return -1;
}
