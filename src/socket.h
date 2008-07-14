#ifndef SPRINGLOBBY_HEADERGUARD_SOCKET_H
#define SPRINGLOBBY_HEADERGUARD_SOCKET_H

#include <wx/string.h>

#include <wx/event.h>

class iNetClass;
class Socket;
class wxSocketEvent;
class wxSocketClient;
class wxCriticalSection;

class PingThread;

enum SockState
{
  SS_Closed,
  SS_Connecting,
  SS_Open
};

enum SockError
{
  SE_No_Error,
  SE_NotConnected,
  SE_Resolve_Host_Failed,
  SE_Connect_Host_Failed
};

#define SOCKET_ID 100


class SocketEvents: public wxEvtHandler
{
  public:
    SocketEvents( iNetClass& netclass ): m_net_class(netclass) {}
    void OnSocketEvent(wxSocketEvent& event);
  protected:
    iNetClass& m_net_class;
  DECLARE_EVENT_TABLE()
};

typedef void (*socket_callback)(Socket*);


//! @brief Class that implements a TCP client socket.
class Socket
{
  public:

    Socket( iNetClass& netclass, bool blocking = false );
    ~Socket();

    // Socket interface

    void Connect( const wxString& addr, const int port );
    void Disconnect( );

    bool Send( const wxString& data );
    bool Receive( wxString& data );


    void Ping();
    void SetPingInfo( const wxString& msg = wxEmptyString, unsigned int interval = 10000 );
    unsigned int GetPingInterval() { return m_ping_int; }
    bool GetPingEnabled() { return m_ping_msg != wxEmptyString; }

    wxString GetLocalAddress();

    SockState State( );
    SockError Error( );

    void SetSendRateLimit( int Bps = -1 );
    void OnTimer( int mselapsed );

    void OnPingThreadStarted();
    void OnPingThreadStopped();

  protected:

  // Socket variables

    wxSocketClient* m_sock;
    SocketEvents* m_events;

    wxCriticalSection m_lock;
    wxCriticalSection m_ping_thread_wait;

    wxString m_ping_msg;
    unsigned int m_ping_int;

    PingThread* m_ping_t;

    bool m_connecting;
    bool m_block;
    iNetClass& m_net_class;

    unsigned int m_udp_private_port;
    int m_rate;
    int m_sent;
    wxString m_buffer;
    wxString m_rcv_buffer;

    wxSocketClient* _CreateSocket();

    bool _Send( const wxString& data );
    void _EnablePingThread( bool enable = true );
    bool _ShouldEnablePingThread();
};



class PingThread: public wxThread
{
  public:
    PingThread( Socket& sock );
    void Init();
    void* Entry();
    void OnExit();
  private:
    Socket& m_sock;
    int m_next_ping;

};


#endif // SPRINGLOBBY_HEADERGUARD_SOCKET_H
