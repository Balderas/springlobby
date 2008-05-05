/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
#include <iterator>
#include <stdexcept>

#include "userlist.h"
#include "user.h"
#include "utils.h"

const user_map_t::size_type SEEKPOS_INVALID = user_map_t::size_type(-1);

UserList::UserList(): m_seek(m_users.end()), m_seekpos(SEEKPOS_INVALID)
{ }

void UserList::AddUser( User& user )
{
  m_users[user.GetNick()] = &user;
  m_seekpos = SEEKPOS_INVALID;
}

void UserList::RemoveUser( const wxString& nick )
{
  m_users.erase(nick);
  m_seekpos = SEEKPOS_INVALID;
}

User& UserList::GetUser( const wxString& nick )
{
  user_iter_t u = m_users.find(nick);
  ASSERT_RUNTIME( u != m_users.end(), _T("UserList::GetUser(\"") + nick + _T("\"): no such user") );
  //ASSERT_LOGIC( u != m_users.end(), _T("UserList::GetUser(\"") + nick + _T("\"): no such user") );
  return *u->second;
}

User& UserList::GetUser( user_map_t::size_type index )
{
  if ((m_seekpos == SEEKPOS_INVALID) || (m_seekpos > index)) {
    m_seek = m_users.begin();
    m_seekpos = 0;
  }
  std::advance( m_seek, index - m_seekpos );
  m_seekpos = index;
  return *m_seek->second;
}

bool UserList::UserExists( wxString const& nick )
{
  return m_users.find(nick) != m_users.end();
}

user_map_t::size_type UserList::GetNumUsers()
{
  return m_users.size();
}

