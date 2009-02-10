/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

#include <wx/tokenzr.h>
#include <wx/image.h>

#include "ibattle.h"
#include "utils.h"
#include "uiutils.h"
#include "settings.h"
#include "ui.h"
#include "springunitsynclib.h"
#include "images/fixcolours_palette.xpm"

#include <list>

IBattle::IBattle():
  m_map_loaded(false),
  m_mod_loaded(false),
  m_map_exists(false),
  m_mod_exists(false),
  m_ingame(false),
  m_generating_script(false),
  m_is_self_in(false)

{
}


IBattle::~IBattle()
{
}

bool IBattle::IsSynced()
{
    LoadMod();
    LoadMap();
    bool synced = true;
    if ( !m_host_map.hash.IsEmpty() ) synced = synced && (m_local_map.hash == m_host_map.hash);
    if ( !m_host_map.name.IsEmpty() ) synced = synced && (m_local_map.name == m_host_map.name);
    if ( !m_host_mod.hash.IsEmpty() ) synced = synced && (m_local_mod.hash == m_host_mod.hash);
    if ( !m_host_mod.name.IsEmpty() ) synced = synced && (m_local_mod.name == m_host_mod.name);
    return synced;
}





std::vector<wxColour> &IBattle::GetFixColoursPalette()
{
    static std::vector<wxColour> result;
    if (result.empty())
    {
        wxImage image(fixcolours_palette_xpm);
        unsigned char* data=image.GetData();
        size_t len=image.GetWidth()*image.GetHeight();
        for (size_t i=0;i<len;++i)
        {
            int r=data[i*3];
            int g=data[i*3+1];
            int b=data[i*3+2];
            if (r||g||b)
            {
                result.push_back(wxColour(r,g,b));
            }
        }
    }
    return result;
}

wxColour IBattle::GetFixColour(int i)
{
    std::vector<wxColour> palette = GetFixColoursPalette();
    if (((unsigned int)i)<palette.size())
    {
        return palette[i];
    }
    else
    {
        return wxColour(127,127,127);
    }
}

int IBattle::GetPlayerNum( const User& user )
{
    for (user_map_t::size_type i = 0; i < GetNumUsers(); i++)
    {
        if ( &GetUser(i) == &user ) return i;
    }
    ASSERT_EXCEPTION(false, _T("The player is not in this game.") );
    return -1;
}

wxColour IBattle::GetFreeColour( User &for_whom ) const
{
    int lowest = 0;
    bool changed = true;
    while ( changed )
    {
        changed = false;
        for ( user_map_t::size_type i = 0; i < GetNumUsers(); i++ )
        {
            if ( &GetUser( i ) == &for_whom ) continue;
            UserBattleStatus& bs = GetUser( i ).BattleStatus();
            if ( bs.spectator ) continue;
            if ( AreColoursSimilar( bs.colour, wxColour(colour_values[lowest][0], colour_values[lowest][1], colour_values[lowest][2]) ) )
            {
                lowest++;
                changed = true;
            }
        }
    }
    return wxColour( colour_values[lowest][0], colour_values[lowest][1], colour_values[lowest][2] );
}

wxColour IBattle::GetFreeColour() const
{
		User temp( _T("") ); // fake user that isn't present in battle, therefore will never be found :P
    return GetFreeColour( temp );
}

int IBattle::ColourDifference(const wxColour &a, const wxColour &b) // returns max difference of r,g,b.
{
    return std::max(abs(a.Red()-b.Red()),std::max(abs(a.Green()-b.Green()),abs(a.Blue()-b.Blue())));

}

int IBattle::GetFreeTeamNum( bool excludeme )
{
    int lowest = 0;
    bool changed = true;
    while ( changed )
    {
        changed = false;
        for ( user_map_t::size_type i = 0; i < GetNumUsers(); i++ )
        {
            if ( ( &GetUser( i ) == &GetMe() ) && excludeme ) continue;
            //if ( GetUser( i ).BattleStatus().spectator ) continue;
            if ( GetUser( i ).BattleStatus().team == lowest )
            {
                lowest++;
                changed = true;
            }
        }
    }
    return lowest;
}

int IBattle::GetClosestFixColour(const wxColour &col, const std::vector<int> &excludes, int &difference)
{
    std::vector<wxColour> palette = GetFixColoursPalette();
    int mindiff=1024;
    int result=0;
    int t1=palette.size();
    int t2=excludes.size();
    wxLogMessage(_T("GetClosestFixColour %d %d"),t1,t2);
    for (size_t i=0;i<palette.size();++i)
    {
        if ((i>=excludes.size()) || (!excludes[i]))
        {
            int diff=ColourDifference(palette[i],col);
            if (diff<mindiff)
            {
                mindiff=diff;
                result=i;
            }
        }
    }
    difference=mindiff;
    wxLogMessage(_T("GetClosestFixColour result=%d diff=%d"),result,difference);
    return result;
}

bool IBattle::HaveMultipleBotsInSameTeam() const
{
    wxLogDebugFunc(_T(""));

    std::vector<int> teams ( GetMaxPlayers(), -1 );
		for ( user_map_t::size_type i = 0; i < GetNumUsers(); i++ )
    {
				User& usr = GetUser( i );
        if ( !usr.BattleStatus().IsBot() ) continue;
        if ( teams[ usr.BattleStatus().team ] == 1 )return true;
        teams[ usr.BattleStatus().team ] = 1;
    }
    return false;
}

void IBattle::SendHostInfo( HostInfo update )
{
}

void IBattle::SendHostInfo( const wxString& Tag )
{
}

void IBattle::Update ( const wxString& Tag )
{
}

User& IBattle::OnUserAdded( User& user )
{
    UserList::AddUser( user );

    user.BattleStatus().spectator = false;
    user.BattleStatus().ready = false;
    user.BattleStatus().sync = SYNC_UNKNOWN;
    if ( !user.BattleStatus().IsBot() )
    {
			user.BattleStatus().team = GetFreeTeamNum( &user == &GetMe() );
			user.BattleStatus().ally = GetFreeAlly( &user == &GetMe() );
			user.BattleStatus().colour = GetFreeColour();
    }
    if ( ( user.BattleStatus().pos.x < 0 ) || ( user.BattleStatus().pos.y < 0 ) ) GetFreePosition( user.BattleStatus().pos.x, user.BattleStatus().pos.y );
    return user;
}

User& IBattle::OnBotAdded( const wxString& nick, const UserBattleStatus& bs )
{
		m_internal_bot_list[nick] = User( nick );
		User& user = m_internal_bot_list[nick];
		user.UpdateBattleStatus( bs );
		return OnUserAdded( user );
}

unsigned int IBattle::GetNumBots() const
{
		return m_internal_bot_list.size();
}

unsigned int IBattle::GetNumPlayers() const
{
		return GetNumUsers() - GetNumBots();
}

void IBattle::OnUserBattleStatusUpdated( User &user, UserBattleStatus status )
{

    bool previousspectatorstatus = user.BattleStatus().spectator;
    int previousteam = user.BattleStatus().team;
    int previousally = user.BattleStatus().ally;

    user.UpdateBattleStatus( status );

    if ( IsFounderMe() )
    {
			if ( status.spectator != previousspectatorstatus )
			{
					if ( status.spectator )
					{
							m_opts.spectators++;
					}
					else
					{
							m_opts.spectators--;
					}
					SendHostInfo( HI_Spectators );
			}
			if ( m_opts.lockexternalbalancechanges )
			{
				if ( previousteam != user.BattleStatus().team ) ForceTeam( user, previousteam );
				if ( previousally != user.BattleStatus().ally ) ForceAlly( user, previousally );
			}
    }
}

void IBattle::OnUserRemoved( User& user )
{
    if ( IsFounderMe() && user.BattleStatus().spectator )
    {
      m_opts.spectators--;
      SendHostInfo( HI_Spectators );
    }
    UserList::RemoveUser( user.GetNick() );
    if ( !user.BattleStatus().IsBot() ) user.SetBattle( 0 );
    else
    {
    	UserVecIter itor = m_internal_bot_list.find( user.GetNick() );
			if ( itor != m_internal_bot_list.end() )
			{
    			m_internal_bot_list.erase( itor );
			}
    }
}

bool IBattle::IsEveryoneReady()
{
    for (user_map_t::size_type i = 0; i < GetNumUsers(); i++)
    {
				User& usr = GetUser(i);
				if ( usr.BattleStatus().IsBot() ) continue;
        UserBattleStatus& bs = usr.BattleStatus();
        if ( !bs.ready && !bs.spectator ) return false;
    }
    return true;
}



void IBattle::AddStartRect( unsigned int allyno, unsigned int left, unsigned int top, unsigned int right, unsigned int bottom )
{
    BattleStartRect sr;

    sr.ally = allyno;
    sr.left = left;
    sr.top = top;
    sr.right = right;
    sr.bottom = bottom;
    sr.toadd = true;
    sr.todelete = false;
    sr.toresize = false;
    sr.exist = true;

    m_rects[allyno] = sr;
}



void IBattle::RemoveStartRect( unsigned int allyno )
{
    if ( allyno >= m_rects.size() ) return;
    BattleStartRect sr = m_rects[allyno];
    sr.todelete = true;
    m_rects[allyno] = sr;
}


void IBattle::ResizeStartRect( unsigned int allyno )
{
    if ( allyno >= m_rects.size() ) return;
    BattleStartRect sr = m_rects[allyno];
    sr.toresize = true;
    m_rects[allyno] = sr;
}


void IBattle::StartRectRemoved( unsigned int allyno )
{
    if ( allyno >= m_rects.size() ) return;
    if ( m_rects[allyno].todelete ) m_rects.erase(allyno);
}


void IBattle::StartRectResized( unsigned int allyno )
{
    if ( allyno >= m_rects.size() ) return;
    BattleStartRect sr = m_rects[allyno];
    sr.toresize = false;
    m_rects[allyno] = sr;
}


void IBattle::StartRectAdded( unsigned int allyno )
{
    if ( allyno >= m_rects.size() ) return;
    BattleStartRect sr = m_rects[allyno];
    sr.toadd = false;
    m_rects[allyno] = sr;
}


BattleStartRect IBattle::GetStartRect( unsigned int allyno )
{
    return m_rects[allyno];
}

unsigned int IBattle::GetNumRects()
{
    return m_rects.size();
}

void IBattle::ClearStartRects()
{
    m_rects.clear();
}

void IBattle::ForceSide( User& user, int side )
{
	if ( IsFounderMe() || user.BattleStatus().IsBot() )
	{
		 user.BattleStatus().side = side;
	}
}

void IBattle::ForceTeam( User& user, int team )
{
  if ( IsFounderMe() || user.BattleStatus().IsBot() )
  {
    user.BattleStatus().team = team;
  }
}


void IBattle::ForceAlly( User& user, int ally )
{

  if ( IsFounderMe() || user.BattleStatus().IsBot() )
  {
    user.BattleStatus().ally = ally;
  }

}


void IBattle::ForceColour( User& user, const wxColour& col )
{
  if ( IsFounderMe() || user.BattleStatus().IsBot() )
		{
			 user.BattleStatus().colour = col;
		}

}


void IBattle::ForceSpectator( User& user, bool spectator )
{
		if ( IsFounderMe() || user.BattleStatus().IsBot() )
		{
			 user.BattleStatus().spectator = spectator;
		}
}

void IBattle::SetHandicap( User& user, int handicap)
{
		if ( IsFounderMe() || user.BattleStatus().IsBot() )
		{
			 user.BattleStatus().handicap = handicap;
		}
}


void IBattle::KickPlayer( User& user )
{
		if ( IsFounderMe() || user.BattleStatus().IsBot() )
		{
			 OnUserRemoved( user );
		}
}

int IBattle::GetFreeAlly( bool excludeme )
{
  int lowest = 0;
  bool changed = true;
  while ( changed )
   {
    changed = false;
    for ( unsigned int i = 0; i < GetNumUsers(); i++ )
    {
      User& user = GetUser( i );
      if ( ( &GetUser( i ) == &GetMe() ) && excludeme ) continue;
      if ( user.BattleStatus().ally == lowest )
      {
        lowest++;
        changed = true;
      }
    }
  }
  return lowest;
}

void IBattle::GetFreePosition( int& x, int& y )
{
  UnitSyncMap map = LoadMap();
  for ( int i = 0; i < map.info.posCount; i++ )
	{
    bool taken = false;
    for ( unsigned int bi = 0; bi < GetNumUsers(); bi++ )
    {
      User& user = GetUser( bi );
      if ( ( map.info.positions[i].x == user.BattleStatus().pos.x ) && ( map.info.positions[i].y == user.BattleStatus().pos.y ) )
      {
        taken = true;
        break;
      }
    }
    if ( !taken )
    {
      x = CLAMP(map.info.positions[i].x, 0, map.info.width);
      y = CLAMP(map.info.positions[i].y, 0, map.info.height);
      return;
    }
  }
  x = map.info.width / 2;
  y = map.info.height / 2;
}


void IBattle::SetHostMap(const wxString& mapname, const wxString& hash)
{
  if ( mapname != m_host_map.name || hash != m_host_map.hash )
  {
    m_map_loaded = false;
    m_host_map.name = mapname;
    m_host_map.hash = hash;
    if ( !m_host_map.hash.IsEmpty() ) m_map_exists = usync().MapExists( m_host_map.name, m_host_map.hash );
    else m_map_exists = usync().MapExists( m_host_map.name );
    if ( m_map_exists && !ui().IsSpringRunning() ) usync().PrefetchMap( m_host_map.name );
  }
}


void IBattle::SetLocalMap(const UnitSyncMap& map)
{
  if ( map.name != m_local_map.name || map.hash != m_local_map.hash ) {
    m_local_map = map;
    m_map_loaded = true;
    if ( !m_host_map.hash.IsEmpty() ) m_map_exists = usync().MapExists( m_host_map.name, m_host_map.hash );
    else m_map_exists = usync().MapExists( m_host_map.name );
    if ( m_map_exists && !ui().IsSpringRunning() ) usync().PrefetchMap( m_host_map.name );
  }
}


const UnitSyncMap& IBattle::LoadMap()
{

  if ( !m_map_loaded ) {
    try {
      ASSERT_EXCEPTION( m_map_exists, _T("Map does not exist.") );
      m_local_map = usync().GetMapEx( m_host_map.name );
      m_map_loaded = true;

    } catch (...) {}
  }
  return m_local_map;
}


wxString IBattle::GetHostMapName() const
{
  return m_host_map.name;
}


wxString IBattle::GetHostMapHash() const
{
  return m_host_map.hash;
}


void IBattle::SetHostMod( const wxString& modname, const wxString& hash )
{
  if ( m_host_mod.name != modname || m_host_mod.hash != hash )
  {
    m_mod_loaded = false;
    m_host_mod.name = modname;
    m_host_mod.hash = hash;
    if ( !m_host_mod.hash.IsEmpty() ) m_mod_exists = usync().ModExists( m_host_mod.name, m_host_mod.hash );
    else m_mod_exists = usync().ModExists( m_host_mod.name );
  }
}


void IBattle::SetLocalMod( const UnitSyncMod& mod )
{
  if ( mod.name != m_local_mod.name || mod.hash != m_local_mod.hash )
  {
    m_local_mod = mod;
    m_mod_loaded = true;
    if ( !m_host_mod.hash.IsEmpty() ) m_mod_exists = usync().ModExists( m_host_mod.name, m_host_mod.hash );
    else m_mod_exists = usync().ModExists( m_host_mod.name );
  }
}


const UnitSyncMod& IBattle::LoadMod()
{
  if ( !m_mod_loaded )
   {
    try {
      ASSERT_EXCEPTION( m_mod_exists, _T("Mod does not exist.") );
      m_local_mod = usync().GetMod( m_host_mod.name );
      m_mod_loaded = true;
    } catch (...) {}
  }
  return m_local_mod;
}


wxString IBattle::GetHostModName() const
{
  return m_host_mod.name;
}


wxString IBattle::GetHostModHash() const
{
  return m_host_mod.hash;
}


bool IBattle::MapExists()
{
  return m_map_exists;
  //return usync().MapExists( m_map_name, m_map.hash );
}


bool IBattle::ModExists()
{
  return m_mod_exists;
  //return usync().ModExists( m_mod_name );
}



void IBattle::DisableUnit( const wxString& unitname )
{
  if ( m_units.Index( unitname ) == wxNOT_FOUND ) m_units.Add( unitname );
}


void IBattle::EnableUnit( const wxString& unitname )
{
  int pos = m_units.Index( unitname );
  if ( pos == wxNOT_FOUND ) return;
  m_units.RemoveAt( pos );
}


void IBattle::EnableAllUnits()
{
  m_units.Empty();
}


wxArrayString IBattle::DisabledUnits()
{
  return m_units;
}

void IBattle::OnSelfLeftBattle()
{
    susynclib().UnSetCurrentMod(); //left battle
    m_is_self_in = false;
    ClearStartRects();
}

void IBattle::OnUnitSyncReloaded()
{
  if ( !m_host_mod.hash.IsEmpty() ) m_mod_exists = usync().ModExists( m_host_mod.name, m_host_mod.hash);
  else m_mod_exists = usync().ModExists( m_host_mod.name );
  if ( !m_host_map.hash.IsEmpty() )  m_map_exists = usync().MapExists( m_host_map.name, m_host_map.hash );
  else  m_map_exists = usync().MapExists( m_host_map.name );
}



static wxString FixPresetName( const wxString& name )
{
  // look name up case-insensitively
  const wxArrayString& presetList = sett().GetPresetList();
  int index = presetList.Index( name, false /*case insensitive*/ );
  if ( index == -1 ) return _T("");

  // set preset to the actual name, with correct case
  return presetList[index];
}


bool IBattle::LoadOptionsPreset( const wxString& name )
{
  wxString preset = FixPresetName(name);
  if (preset == _T("")) return false; ///preset not found
  m_preset = preset;

  for ( unsigned int i = 0; i < OptionsWrapper::LastOption; i++)
  {
    std::map<wxString,wxString> options = sett().GetHostingPreset( m_preset, i );
    if ( (OptionsWrapper::GameOption)i != OptionsWrapper::PrivateOptions )
    {
      for ( std::map<wxString,wxString>::iterator itor = options.begin(); itor != options.end(); itor++ )
      {
        CustomBattleOptions().setSingleOption( itor->first, itor->second, (OptionsWrapper::GameOption)i );
      }
    }
    else
    {
      if ( !options[_T("mapname")].IsEmpty() )
      {
        if ( usync().MapExists( options[_T("mapname")] ) ) {
            UnitSyncMap map = usync().GetMapEx( options[_T("mapname")] );
            SetLocalMap( map );
            SendHostInfo( HI_Map );
        }
        else if ( !ui().OnPresetRequiringMap( options[_T("mapname")] ) ) {
            //user didn't want to download the missing map, so set to empty to not have it tried to be loaded again
            options[_T("mapname")] = _T("");
            sett().SetHostingPreset( m_preset, i, options );
        }
      }
      unsigned int localrectcount = GetNumRects();
      for( unsigned int localrect = 0 ; localrect < localrectcount; ++localrect) if ( GetStartRect( localrect ).exist ) RemoveStartRect( localrect );
      SendHostInfo( HI_StartRects );

      unsigned int rectcount = s2l( options[_T("numrects")] );
      for ( unsigned int loadrect = 0; loadrect < rectcount; loadrect++)
      {
        int ally = s2l(options[_T("rect_") + TowxString(loadrect) + _T("_ally")]);
        if ( ally == 0 ) continue;
        AddStartRect( ally - 1, s2l(options[_T("rect_") + TowxString(loadrect) + _T("_left")]), s2l(options[_T("rect_") + TowxString(loadrect) + _T("_top")]), s2l(options[_T("rect_") + TowxString(loadrect) + _T("_right")]), s2l(options[_T("rect_") + TowxString(loadrect) + _T("_bottom")]) );
      }
      SendHostInfo( HI_StartRects );

      m_units = wxStringTokenize( options[_T("restrictions")], _T('\t') );
      SendHostInfo( HI_Restrictions );
      Update( wxString::Format( _T("%d_restrictions"), OptionsWrapper::PrivateOptions ) );

    }
  }
  SendHostInfo( HI_Send_All_opts );
  ui().ReloadPresetList();
  return true;
}


void IBattle::SaveOptionsPreset( const wxString& name )
{
  m_preset = FixPresetName(name);
  if (m_preset == _T("")) m_preset = name; ///new preset

  for ( int i = 0; i < (int)OptionsWrapper::LastOption; i++)
  {
    if ( (OptionsWrapper::GameOption)i != OptionsWrapper::PrivateOptions )
    {
      sett().SetHostingPreset( m_preset, (OptionsWrapper::GameOption)i, CustomBattleOptions().getOptionsMap( (OptionsWrapper::GameOption)i ) );
    }
    else
    {
      std::map<wxString,wxString> opts;
      opts[_T("mapname")] = GetHostMapName();
      unsigned int validrectcount = 0;
      if ( s2l (CustomBattleOptions().getSingleValue( _T("startpostype"), OptionsWrapper::EngineOption ) ) == ST_Choose )
      {
        unsigned int boxcount = GetNumRects();
        for ( unsigned int boxnum = 0; boxnum < boxcount; boxnum++ )
        {
          BattleStartRect rect = GetStartRect( boxnum );
          if ( rect.IsOk() )
          {
            opts[_T("rect_") + TowxString(validrectcount) + _T("_ally")] = TowxString( rect.ally + 1 );
            opts[_T("rect_") + TowxString(validrectcount) + _T("_left")] = TowxString( rect.left );
            opts[_T("rect_") + TowxString(validrectcount) + _T("_top")] = TowxString( rect.top );
            opts[_T("rect_") + TowxString(validrectcount) + _T("_bottom")] = TowxString( rect.bottom );
            opts[_T("rect_") + TowxString(validrectcount) + _T("_right")] = TowxString( rect.right );
            validrectcount++;
          }
        }
      }
      opts[_T("numrects")] = TowxString( validrectcount );

      unsigned int restrcount = m_units.GetCount();
      wxString restrictionsstring;
      for ( unsigned int restrnum = 0; restrnum < restrcount; restrnum++ )
      {
        restrictionsstring << m_units[restrnum] << _T('\t');
      }
      opts[_T("restrictions")] = restrictionsstring;

      sett().SetHostingPreset( m_preset, (OptionsWrapper::GameOption)i, opts );
    }
  }
  sett().SaveSettings();
  ui().ReloadPresetList();
}


wxString IBattle::GetCurrentPreset()
{
  return m_preset;
}


void IBattle::DeletePreset( const wxString& name )
{
  wxString preset = FixPresetName(name);
  if ( m_preset == preset ) m_preset = _T("");
  sett().DeletePreset( preset );
  ui().ReloadPresetList();
}


wxArrayString IBattle::GetPresetList()
{
  return sett().GetPresetList();
}

void IBattle::UserPositionChanged( const User& user )
{
}
