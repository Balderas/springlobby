/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

#include <wx/socket.h>
#include <wx/thread.h>
#include <wx/string.h>
#include <wx/log.h>
#include <stdexcept>
#include <algorithm>

#include "socket.h"
#include "server.h"
#include "utils.h"

#ifdef __WXMSW__
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")
#else
#include <sys/ioctl.h>
#include <net/if.h>
#endif

#define LOCK_SOCKET wxCriticalSectionLocker criticalsection_lock(m_lock)


BEGIN_EVENT_TABLE(SocketEvents, wxEvtHandler)

EVT_SOCKET(SOCKET_ID, SocketEvents::OnSocketEvent)

END_EVENT_TABLE()


void SocketEvents::OnSocketEvent(wxSocketEvent& event)
{
  Socket* sock = (Socket*)event.GetClientData();
  try
  {
  ASSERT_LOGIC( sock != 0, _T("sock = 0") );
  } catch (...) { return; }

  if ( event.GetSocketEvent() == wxSOCKET_INPUT ) {
    m_net_class.OnDataReceived( sock );
  } else if ( event.GetSocketEvent() == wxSOCKET_LOST ) {
    m_net_class.OnDisconnected( sock );
  } else if ( event.GetSocketEvent() == wxSOCKET_CONNECTION ) {
    m_net_class.OnConnected( sock );
  } else {
    try
    {
    ASSERT_LOGIC( false, _T("Unknown socket event."));
    } catch (...) { return; };
  }
}


//! @brief Constructor
Socket::Socket( iNetClass& netclass, bool blocking ):
  m_block(blocking),
  m_net_class(netclass),
  m_rate(-1),
  m_sent(0)
{
  m_connecting = false;

  m_sock = 0;
  m_events = 0;

  //resetting the ping state vars.
  m_ping_msg = wxEmptyString;
  m_ping_int = 0;
  m_ping_t = 0;

}


//! @brief Destructor
Socket::~Socket()
{
  Disconnect();
  LOCK_SOCKET;
  if ( m_sock != 0 ) m_sock->Destroy();
  delete m_events;
}


//! @brief Creates an TCP socket and sets it up.
wxSocketClient* Socket::_CreateSocket()
{
  wxSocketClient* sock = new wxSocketClient();

  sock->SetClientData( (void*)this );
  if ( !m_block ) {
    if ( m_events == 0 ) m_events = new SocketEvents( m_net_class );
    sock->SetFlags( wxSOCKET_NOWAIT );

    sock->SetEventHandler(*m_events, SOCKET_ID);
    sock->SetNotify( wxSOCKET_CONNECTION_FLAG | wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG );
    sock->Notify(true);
  } else {
    if ( m_events != 0 ) {
      delete m_events;
      m_events = 0;
    }
  }
  return sock;
}

//! @brief Connect to remote host.
//! @note This turns off the ping thread.
void Socket::Connect( const wxString& addr, const int port )
{
  LOCK_SOCKET;
  _EnablePingThread( false ); // Turn off ping thread.

  wxIPV4address wxaddr;
  m_connecting = true;

  wxaddr.Hostname( addr );
  wxaddr.Service( port );

  if ( m_sock != 0 ) m_sock->Destroy();
  m_sock = _CreateSocket();
  m_sock->Connect( wxaddr, m_block );
  m_sock->SetTimeout( 40 );
}


//! @brief Disconnect from remote host if connected.
//! @note This turns off the ping thread.
void Socket::Disconnect( )
{
  if ( m_sock ) m_sock->SetTimeout( 0 );
  m_net_class.OnDisconnected( this );
  _EnablePingThread( false );

  if ( m_sock ) {
    m_sock->Destroy();
    m_sock = 0;
  }
}


//! @brief Send data over connection.
bool Socket::Send( const wxString& data )
{
  LOCK_SOCKET;
  return _Send( data );
}


//! @brief Internal send function.
//! @note Does not lock the criticalsection.
bool Socket::_Send( const wxString& data )
{
  if ( !m_sock ) {
    wxLogError( _T("Socket NULL") );
    return false;
  }

  if ( m_rate > 0 ) {
    m_buffer += data;
    int max = m_rate - m_sent;
    if ( max > 0 ) {
      wxString send = m_buffer.substr( 0, max );
      m_buffer.erase( 0, max );
      //wxLogMessage( _T("send: %d  sent: %d  max: %d   :  buff: %d"), send.length() , m_sent, max, m_buffer.length() );
      std::string s = (const char*)send.mb_str(wxConvUTF8);
      m_sock->Write( s.c_str(), s.length() );
      m_sent += s.length();
    }
  } else {
    if ( data.length() <= 0) return true;
    std::string s = (const char*)data.mb_str(wxConvUTF8);
    m_sock->Write( s.c_str(), s.length() );
  }
  return !m_sock->Error();
}


//! @brief Receive data from connection
bool Socket::Receive( wxString& data )
{
  if ( m_sock == 0 ) {
    wxLogError( _T("Socket NULL") );
    return false;
  }

  LOCK_SOCKET;

  const int buff_size = 1337;

  char buff[buff_size+2] = { 0 };
  int readnum;

  buff[buff_size+1] = '\0';

  do {
    m_sock->Read( &buff[0], buff_size );
    readnum = m_sock->LastCount();
    buff[readnum] = '\0';

    if ( readnum > 0 ) {
      wxString d = wxString( &buff[0], wxConvUTF8 );
      if ( d.IsEmpty() )
      {
        d = wxString( &buff[0], wxConvLocal );
        #ifndef HAVE_WX26
        if ( d.IsEmpty() ) d = wxString( &buff[0], wxCSConv(_T("latin-1")) );
        #endif
      }
      m_rcv_buffer << d;
    }
  } while ( readnum >= buff_size );

  if ( m_rcv_buffer.Contains(_T("\n")) ) {
    data = m_rcv_buffer.BeforeFirst('\n');
    m_rcv_buffer = m_rcv_buffer.AfterFirst('\n');
    return true;
  } else {
    return false;
  }
}

wxString Socket::GetHandle()
{
	wxString handle;
	#ifdef __WXMSW__

    IP_ADAPTER_INFO AdapterInfo[16];       // Allocate information for 16 cards
    DWORD dwBufLen = sizeof(AdapterInfo);  // Save memory size of buffer

    DWORD dwStatus = GetAdaptersInfo ( AdapterInfo, &dwBufLen); // Get info
		if (dwStatus != NO_ERROR) return _T(""); // Check status
    for (unsigned int i=0; i<std::min( (unsigned int)6, (unsigned int)AdapterInfo[0].AddressLength); i++)
    {
        handle += TowxString(((unsigned int)AdapterInfo[0].Address[i])&255);
        if (i != 5) handle += _T(':');
    }
	#elif defined(__WXGTK__)
	int sock = socket (AF_INET, SOCK_DGRAM, 0);
	if (sock < 0)
	{
		return _T(""); //not a valid socket
	}
	struct ifreq dev; //container for the hw data
	struct if_nameindex *NameList = if_nameindex(); //container for the interfaces list
	if (NameList == NULL)
	{
		close(sock);
		return _T(""); //cannot list the interfaces
	}

	int pos = 0;
	std::string InterfaceName;
	do
	{
		if (NameList[pos].if_index == 0)
		{
			close(sock);
			if_freenameindex(NameList);
			return _T(""); // no valid interfaces found
		}
		InterfaceName = NameList[pos].if_name;
		pos++;
	} while (InterfaceName.substr(0,2) == "lo" || InterfaceName.substr(0,3) == "sit");

	if_freenameindex(NameList); //free the memory

	strcpy (dev.ifr_name, InterfaceName.c_str()); //select from the name
	if (ioctl(sock, SIOCGIFHWADDR, &dev) < 0) //get the interface data
	{
		close(sock);
		return _T(""); //cannot list the interfaces
	}

    for (int i=0; i<6; i++)
    {
        handle += TowxString(((unsigned int)dev.ifr_hwaddr.sa_data[i])&255);
        if (i != 5) handle += _T(':');
    }
	close(sock);
	#endif
	return handle;
}

//! @brief Get curent socket state
SockState Socket::State( )
{
  if ( m_sock == 0 ) return SS_Closed;

  LOCK_SOCKET;
  if ( m_sock->IsConnected() ) {
    m_connecting = false;
    return SS_Open;
  } else {
    if ( m_connecting ) {
      return SS_Connecting;
    } else {
      return SS_Closed;
    }
  }
}


//! @brief Get socket error code
//! @todo Implement
SockError Socket::Error( )
{
  return (SockError)-1;
}


//! @brief used to retrieve local ip address behind NAT to communicate to the server on login
wxString Socket::GetLocalAddress()
{
  if ( m_sock || !m_sock->IsConnected() ) return wxEmptyString;
  wxIPV4address localaddr;
  m_sock->GetLocal( localaddr );
  return localaddr.IPAddress();
}


//! @brief Set ping info to be used by the ping thread.
//! @note Set msg to an empty string to turn off the ping thread.
//! @note This has to be set every time the socket connects.
void Socket::SetPingInfo( const wxString& msg, unsigned int interval )
{
  LOCK_SOCKET;
  m_ping_msg = msg;
  m_ping_int = interval;
  _EnablePingThread( _ShouldEnablePingThread() );
}


void Socket::_EnablePingThread( bool enable )
{

  if ( !enable ) {
    if ( m_ping_t ) {

      // Reset values to be sure.
      m_ping_int = 0;
      m_ping_msg = wxEmptyString;

      m_ping_t->Wait();
      delete m_ping_t;

      m_ping_t = 0;
    }
  } else {
    if ( !m_ping_t ) {
      m_ping_t = new PingThread( *this );
      m_ping_t->Init();
    }
  }
}


//! @brief Check if we should enable or dsable the ping htread.
//! @see Socket::_EnablePingThread
bool Socket::_ShouldEnablePingThread()
{
  return ( (m_ping_msg != wxEmptyString) );
}


//! @brief Set the maximum upload ratio.
void Socket::SetSendRateLimit( int Bps )
{
  m_rate = Bps;
}


//! @brief Ping remote host with custom protocol message.
//! @note Called from separate thread
void Socket::Ping()
{
  // Dont log here, else it may crash.
  // wxLogMessage( _T("Sent ping.") );
  if ( m_ping_msg != wxEmptyString ) Send( m_ping_msg );
}



void Socket::OnTimer( int mselapsed )
{
  LOCK_SOCKET;

  if ( m_rate > 0 ) {
    m_sent -= int( ( mselapsed / 1000.0 ) * m_rate );
    if ( m_sent < 0 ) m_sent = 0;
    if ( m_buffer.length() > 0 ) _Send(_T(""));
  } else {
    m_sent = 0;
  }
}

PingThread::PingThread( Socket& sock ):
  m_sock(sock)
{
}


void PingThread::Init()
{
  Create();
  SetPriority( WXTHREAD_MAX_PRIORITY );
  Run();
}


void* PingThread::Entry()
{
  int milliseconds = m_sock.GetPingInterval();

  while ( !TestDestroy() )
  {
    if ( !m_sock.GetPingEnabled() ) break;
    m_sock.Ping();
    // break if woken
    if(!Sleep(milliseconds))break;
  }
  return 0;
}

void PingThread::OnExit()
{
}
