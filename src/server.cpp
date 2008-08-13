/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: Server
//

#include <stdexcept>

#include "server.h"
#include "socket.h"
#include "battle.h"
#include "channel.h"
#include "user.h"
#include "utils.h"


Server::~Server()
{
  while ( battles_iter->GetNumBattles() > 0 ) {
    battles_iter->IteratorBegin();
    Battle* b = battles_iter->GetBattle();
    if (b!=0)
    {
        m_battles.RemoveBattle( b->GetBattleId() );
        delete b;
    }
  }
  while ( m_users.GetNumUsers() > 0 ) {
    try{
    User* u = &m_users.GetUser( 0 );
    m_users.RemoveUser( u->GetNick() );
    delete u;
    }catch(std::runtime_error){
    }
  }
  while ( m_channels.GetNumChannels() > 0 ) {
    Channel* c = &m_channels.GetChannel( 0 );
    m_channels.RemoveChannel( c->GetName() );
    delete c;
  }
  delete battles_iter;
}


void Server::SetSocket( Socket* sock )
{
  ASSERT_LOGIC( (!IsConnected()) || (sock == 0), _T("Not connected") );
  m_sock = sock;
}

User& Server::GetUser( const wxString& nickname ) const
{
  return m_users.GetUser( nickname );
}


bool Server::UserExists( const wxString& nickname ) const
{
  return m_users.UserExists( nickname );
}


Channel& Server::GetChannel( const wxString& name )
{
  return m_channels.GetChannel( name );
}


int Server::GetNumChannels()
{
  return m_channels.GetNumChannels();
}


Channel& Server::GetChannel( const int& index )
{
  return m_channels.GetChannel( index );
}


bool Server::ChannelExists( const wxString& name )
{
  return m_channels.ChannelExists( name );
}


Battle& Server::GetBattle( const int& battleid )
{
  return battles_iter->GetBattle( battleid );
}


bool Server::BattleExists( const int& battleid )
{
  return battles_iter->BattleExists( battleid );
}



User& Server::_AddUser( const wxString& user )
{
  if ( m_users.UserExists( user ) ) return m_users.GetUser( user );
  User* u = new User( user, *this );
  m_users.AddUser( *u );
  return *u;
}


void Server::_RemoveUser( const wxString& nickname )
{
  try{
    User* u = &m_users.GetUser( nickname );
    m_users.RemoveUser( nickname );
    delete u;
  }catch(std::runtime_error){
  }
}


Channel& Server::_AddChannel( const wxString& chan )
{
  if ( m_channels.ChannelExists( chan ) ) return m_channels.GetChannel( chan );
  Channel* c = new Channel( *this, m_ui );
  c->SetName( chan );

  m_channels.AddChannel( *c );
  return *c;
}


void Server::_RemoveChannel( const wxString& name )
{
  Channel* c = &m_channels.GetChannel( name );
  m_channels.RemoveChannel( name );
  ASSERT_LOGIC( c != 0, _T("Server::_RemoveChannel(\"") + name + _T("\"): GetChannel returned NULL pointer"));
  delete c;
}

Battle& Server::_AddBattle( const int& id )
{
  if ( battles_iter->BattleExists( id ) ) return battles_iter->GetBattle( id );
  Battle* b = new Battle( *this, m_ui, id );

  m_battles.AddBattle( *b );
  return *b;
}


void Server::_RemoveBattle( const int& id )
{
  Battle* b = &battles_iter->GetBattle( id );
  m_battles.RemoveBattle( id );
  ASSERT_LOGIC( b != 0, _T("Server::_RemoveBattle(): GetBattle returned NULL pointer"));
  delete b;
}


