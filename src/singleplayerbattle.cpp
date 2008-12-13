/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

#include <stdexcept>
#include <wx/log.h>

#include "singleplayerbattle.h"
#include "mainsingleplayertab.h"
#include "server.h"
#include "utils.h"
#include "uiutils.h"
#include "ui.h"
#include "settings.h"


SinglePlayerBattle::SinglePlayerBattle(Ui& ui, MainSinglePlayerTab& msptab):
  m_ui(ui),
  m_sptab(msptab)
{
  CustomBattleOptions().setSingleOption( _T("startpostype"), wxString::Format(_T("%d"), 3), OptionsWrapper::EngineOption );
}


SinglePlayerBattle::~SinglePlayerBattle()
{

}









void SinglePlayerBattle::SendHostInfo( HostInfo update )
{
  if ( (update & HI_StartType) != 0 ) m_sptab.UpdateMinimap();
  if ( (update & HI_Restrictions) != 0 ) m_sptab.ReloadRestrictions();
  if ( (update & HI_Map_Changed) != 0 )
  {
    SetLocalMap( usync().GetMapEx( usync().GetMapIndex( m_host_map.name ) ) );
    CustomBattleOptions().loadOptions( OptionsWrapper::MapOption, GetHostMapName() );
    m_sptab.ReloadMapOptContrls();
  }
  if ( (update & HI_Mod_Changed) != 0 )
  {
    for ( unsigned int num = 0; num < GetNumBots(); num++ ) RemoveBot( num ); /// remove all bots
    CustomBattleOptions().loadOptions( OptionsWrapper::ModOption, GetHostModName() );
    wxString presetname = sett().GetModDefaultPresetName( GetHostModName() );
    if ( !presetname.IsEmpty() )
    {
      LoadOptionsPreset( presetname );
      SendHostInfo( HI_Send_All_opts );
    }
    m_sptab.ReloadModOptContrls();
    Update(  wxString::Format(_T("%d_%s"), OptionsWrapper::PrivateOptions , _T("mapname") ) );
  }
  if ( (update & HI_Send_All_opts) != 0 )
  {
    for ( int i = 0; i < (int)OptionsWrapper::LastOption; i++)
    {
      std::map<wxString,wxString> options = CustomBattleOptions().getOptionsMap( (OptionsWrapper::GameOption)i );
      for ( std::map<wxString,wxString>::iterator itor = options.begin(); itor != options.end(); itor++ )
      {
        Update(  wxString::Format(_T("%d_%s"), i , itor->first.c_str() ) );
      }
    }
  }
}


void SinglePlayerBattle::Update( const wxString& Tag )
{
  m_sptab.Update( Tag );
}

