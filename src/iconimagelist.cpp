/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: IconImageList
//

#include <stdexcept>

#include <wx/image.h>

#include "iconimagelist.h"
#include "user.h"
#include "battle.h"
#include "utils.h"
#include "iunitsync.h"

#include "images/bot.xpm"
#include "images/bot_broom.png.h"
#include "images/bot_ingame.png.h"

#include "images/admin.png.h"
#include "images/admin_away.png.h"
#include "images/admin_broom.png.h"
#include "images/admin_ingame.png.h"

#include "images/chanop.xpm"
#include "images/chanop_away.xpm"
#include "images/chanop_broom.xpm"
#include "images/chanop_ingame.xpm"

#include "images/away.png.h"
#include "images/broom.png.h"
#include "images/ingame.png.h"

#include "images/up.xpm"
#include "images/down.xpm"
#include "images/rank0.xpm"
#include "images/rank1.xpm"
#include "images/rank2.xpm"
#include "images/rank3.xpm"
#include "images/rank4.xpm"
#include "images/rank5.xpm"
#include "images/rank6.xpm"

#include "images/open_game.png.h"
#include "images/open_pw_game.png.h"
#include "images/open_full_pw_game.png.h"
#include "images/open_full_game.png.h"
#include "images/closed_game.png.h"
#include "images/closed_pw_game.png.h"
#include "images/closed_full_pw_game.png.h"
#include "images/closed_full_game.png.h"


#include "images/nexists.xpm"
#include "images/exists.xpm"

#include "images/ready_unsync.xpm"
#include "images/ready_q.xpm"
#include "images/nready_unsync.xpm"
#include "images/nready_q.xpm"

#include "images/spectator.xpm"
#include "images/host.xpm"
#include "images/host_spectator.xpm"

#include "images/no1_icon.png.h"
#include "images/no2_icon.png.h"

#include "images/colourbox.xpm"
//#include "images/fixcolours_palette.xpm"

#include "images/unknown_flag.xpm"

#include "flagimages.h"

#include "images/empty.xpm"
#include "uiutils.h"

IconImageList::IconImageList() : wxImageList(16,16)
{
    ICON_ADMIN = Add( *charArr2wxBitmap( admin_png, sizeof(admin_png) ) );
    ICON_ADMIN_AWAY = Add( *charArr2wxBitmap( admin_away_png, sizeof(admin_away_png) ) );
    ICON_ADMIN_BROOM = Add( *charArr2wxBitmap( admin_broom_png, sizeof(admin_broom_png) ) );
    ICON_ADMIN_INGAME = Add( *charArr2wxBitmap( admin_ingame_png, sizeof(admin_ingame_png) ) );

    ICON_BOT = Add( wxBitmap(bot_xpm) );
    ICON_BOT_BROOM = Add( *charArr2wxBitmap( bot_broom_png, sizeof( bot_broom_png ) )  );
    ICON_BOT_INGAME = Add( *charArr2wxBitmap( bot_ingame_png, sizeof( bot_ingame_png ) ) );

    ICON_AWAY = Add( *charArr2wxBitmap( away_png, sizeof( away_png ) ) );
    ICON_BROOM = Add( *charArr2wxBitmap(broom_png, sizeof(broom_png) ) );
    ICON_INGAME = Add( *charArr2wxBitmap(ingame_png, sizeof(ingame_png) ) );

    ICON_OP = Add( wxBitmap(chanop_xpm) );
    ICON_OP_AWAY = Add( wxBitmap(chanop_away_xpm) );
    ICON_OP_BROOM = Add( wxBitmap(chanop_broom_xpm) );
    ICON_OP_INGAME = Add( wxBitmap(chanop_ingame_xpm) );

    ICON_UP = Add( wxBitmap(up_xpm) );
    ICON_DOWN = Add( wxBitmap(down_xpm) );

    ICON_RANK0 = Add( wxBitmap(rank0_xpm) );
    ICON_RANK1 = Add( wxBitmap(rank1_xpm) );
    ICON_RANK2 = Add( wxBitmap(rank2_xpm) );
    ICON_RANK3 = Add( wxBitmap(rank3_xpm) );
    ICON_RANK4 = Add( wxBitmap(rank4_xpm) );
    ICON_RANK5 = Add( wxBitmap(rank5_xpm) );
    ICON_RANK6 = Add( wxBitmap(rank6_xpm) );

    ICON_READY = ICON_OPEN_GAME = Add( *charArr2wxBitmap(open_game_png, sizeof(open_game_png) ) );
    ICON_OPEN_PW_GAME = Add( *charArr2wxBitmap(open_pw_game_png, sizeof(open_pw_game_png) ) );
    ICON_OPEN_FULL_PW_GAME = Add( *charArr2wxBitmap(open_full_pw_game_png, sizeof(open_full_pw_game_png) ) );
    ICON_OPEN_FULL_GAME = Add( *charArr2wxBitmap(open_full_game_png, sizeof(open_full_game_png) ) );
    ICON_NREADY = ICON_CLOSED_GAME = Add( *charArr2wxBitmap(closed_game_png, sizeof(closed_game_png) ) );
    ICON_CLOSED_PW_GAME = Add( *charArr2wxBitmap(closed_pw_game_png, sizeof(closed_pw_game_png) ) );
    ICON_CLOSED_FULL_PW_GAME = Add( *charArr2wxBitmap(closed_full_pw_game_png, sizeof(closed_full_pw_game_png) ) );
    ICON_CLOSED_FULL_GAME = Add( *charArr2wxBitmap(closed_full_game_png, sizeof(closed_full_game_png) ) );
    ICON_STARTED_GAME = Add(  *charArr2wxBitmap(ingame_png, sizeof(ingame_png) ) );


    ICON_READY_UNSYNC = Add( wxBitmap(ready_unsync_xpm) );
    ICON_NREADY_UNSYNC = Add( wxBitmap(nready_unsync_xpm) );
    ICON_READY_QSYNC = Add( wxBitmap(ready_q_xpm) );
    ICON_NREADY_QSYNC = Add( wxBitmap(nready_q_xpm) );

    ICON_NEXISTS = Add( wxBitmap(nexists_xpm) );
    ICON_EXISTS = Add( wxBitmap(exists_xpm) );

    ICON_SPECTATOR = Add( wxBitmap(spectator_xpm) );
    ICON_HOST = Add( wxBitmap(host_xpm) );
    ICON_HOST_SPECTATOR = Add( wxBitmap(host_spectator_xpm) );

    ICON_SIDEPIC_0 = Add( *charArr2wxBitmap(no1_icon_png, sizeof(no1_icon_png) ) );
    ICON_SIDEPIC_1 = Add( *charArr2wxBitmap(no2_icon_png, sizeof(no2_icon_png) ) );

    SetColourIcon( 0, wxColour( 255,   0,   0 ) );
    SetColourIcon( 1, wxColour(   0, 255,   0 ) );
    SetColourIcon( 2, wxColour(   0,   0, 255 ) );


  //ICON_FIXCOLOURS_PALETTE = Add( wxBitmap(fixcolours_palette_xpm) );

    ICON_UNK_FLAG = Add( wxBitmap(unknown_flag_xpm) );

    ICON_FLAGS_BASE = AddFlagImages( *this );

    ICON_EMPTY = Add( wxBitmap(empty_xpm) );

#ifdef __WXMSW__
    ICON_NONE = ICON_NOSTATE = ICON_RANK_NONE = ICON_GAME_UNKNOWN = ICON_EMPTY;
#else
    ICON_NONE = ICON_NOSTATE = ICON_RANK_NONE = ICON_GAME_UNKNOWN = -1;
#endif

}


IconImageList& icons()
{
    static IconImageList m_icons;
    return m_icons;
}


int IconImageList::GetUserListStateIcon( const UserStatus& us, bool chanop, bool inbroom )
{
    if ( us.bot )
    {
        if ( us.in_game ) return ICON_BOT_INGAME;
        if ( inbroom ) return ICON_BOT_BROOM;
        return ICON_BOT;
    }
    if (us.moderator )
    {
        if ( us.in_game ) return ICON_ADMIN_INGAME;
        if ( us.away ) return ICON_ADMIN_AWAY;
        if ( inbroom ) return ICON_ADMIN_BROOM;
        return ICON_ADMIN;
    }
    if ( chanop )
    {
        if ( us.in_game ) return ICON_OP_INGAME;
        if ( us.away ) return ICON_OP_AWAY;
        if ( inbroom ) return ICON_OP_BROOM;
        return ICON_OP;
    }

    if ( us.in_game ) return ICON_INGAME;
    if ( us.away ) return ICON_AWAY;
    if ( inbroom ) return ICON_BROOM;

    return ICON_NOSTATE;
}


int IconImageList::GetUserBattleStateIcon( const UserStatus& us )
{
    if ( us.bot ) return ICON_BOT;
    if (us.moderator )
    {
        if ( us.in_game ) return ICON_ADMIN_INGAME;
        if ( us.away ) return ICON_ADMIN_AWAY;
        return ICON_ADMIN;
    }

    if ( us.in_game ) return ICON_INGAME;
    if ( us.away ) return ICON_AWAY;

    return ICON_NOSTATE;
}


int IconImageList::GetRankIcon( const int& rank, const bool& showlowest )
{
    if ( rank <= RANK_0 )
    {
        if ( showlowest ) return ICON_RANK0;
        else return ICON_RANK_NONE;
    }
    if ( rank <= RANK_1 ) return ICON_RANK1;
    if ( rank <= RANK_2 ) return ICON_RANK2;
    if ( rank <= RANK_3 ) return ICON_RANK3;
    if ( rank <= RANK_4 ) return ICON_RANK4;
    if ( rank <= RANK_5 ) return ICON_RANK5;
    return ICON_RANK6;
}


int IconImageList::GetFlagIcon( const wxString& flagname )
{
    return ICON_FLAGS_BASE + GetFlagIndex( flagname );
}


int IconImageList::GetBattleStatusIcon( Battle& battle )
{
    if ( battle.GetInGame() ) return ICON_STARTED_GAME;
    if ( !battle.IsLocked() )
    {
        if ( battle.IsFull() )
        {
            if ( battle.IsPassworded() )
                return ICON_OPEN_FULL_PW_GAME;
            else
                return ICON_OPEN_FULL_GAME;
        }
        if ( !battle.IsPassworded() ) return ICON_OPEN_GAME;
        else return ICON_OPEN_PW_GAME;
    }
    else
    {
        if ( battle.IsFull() )
        {
            if ( battle.IsPassworded() )
                return ICON_CLOSED_FULL_PW_GAME;
            else
                return ICON_CLOSED_FULL_GAME;
        }
        if ( !battle.IsPassworded() ) return ICON_CLOSED_GAME;
        else return ICON_CLOSED_PW_GAME;
    }


    return ICON_GAME_UNKNOWN;
}

wxString IconImageList::GetBattleStatus( Battle& battle )
{
    if ( battle.GetInGame() ) return _T("Game has already started");
    if ( !battle.IsLocked() )
    {
        if ( !battle.IsPassworded() ) return _T("Game is open for players");
        else return _T("You need a password to join");
    }
    else
    {
        if ( !battle.IsPassworded() ) return _T("Game is closed");
        else return _T("Game is closed (and passworded)");
    }

    return _T("Game has unknown status");
}


int IconImageList::GetColourIcon( const int& num )
{
    if ( m_player_colour_icons[num] != 0 ) return m_player_colour_icons[num];
    else return -1;
}


int IconImageList::GetHostIcon( const bool& spectator )
{
    return spectator?ICON_HOST_SPECTATOR:ICON_HOST;
}


void IconImageList::SetColourIcon( const int& num, const wxColour& colour )
{
    wxImage img( colourbox_xpm );

    img.Replace( 1, 1, 1, colour.Red(), colour.Green(), colour.Blue() );

    int r,g,b;
    r = colour.Red()+80;
    g = colour.Green()+80;
    b = colour.Blue()+80;
    img.Replace( 2, 2, 2, r>255?255:r, g>255?255:g, b>255?255:b );

    /*r = colour.Red()-60; g = colour.Green()-60; b = colour.Blue()-60;
    img.Replace( 200, 200, 200, r<0?0:r, g<0?0:g, b<0?0:b );*/
    if ( m_player_colour_icons[num] != 0 ) Replace( m_player_colour_icons[num], wxBitmap( img ) );
    else m_player_colour_icons[num] = Add( wxBitmap( img ) );
}


int IconImageList::GetSideIcon( const wxString& modname, int side )
{
  wxString sidename = usync()->GetSideName( modname, side );

  if (m_cached_side_icons[sidename] == 0){
    try
    {
      int IconPosition = Add(wxBitmap( usync()->GetSidePicture( modname , sidename ) ), wxNullBitmap);
      m_cached_side_icons[sidename] = IconPosition;
      return IconPosition;
    } catch (...)
    {
      if ( side == 0 ) m_cached_side_icons[sidename] = ICON_SIDEPIC_0;
      else if ( side == 1 ) m_cached_side_icons[sidename] = ICON_SIDEPIC_1;
    }
  } else return m_cached_side_icons[sidename];
  return -1;
}

int IconImageList::GetReadyIcon( const bool& ready, const int& sync )
{
    if ( ready )
    {
        if ( sync == SYNC_SYNCED ) return ICON_READY;
        else if ( sync == SYNC_UNSYNCED ) return ICON_READY_UNSYNC;
        else return ICON_READY_QSYNC;
    }
    else
    {
        if ( sync == SYNC_SYNCED ) return ICON_NREADY;
        else if ( sync == SYNC_UNSYNCED ) return ICON_NREADY_UNSYNC;
        else return ICON_NREADY_QSYNC;
    }
}
