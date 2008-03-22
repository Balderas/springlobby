#ifndef SPRINGLOBBY_HEADERGUARD_TASSERVER_H
#define SPRINGLOBBY_HEADERGUARD_TASSERVER_H

#include <wx/string.h>
#include <list>

#include "server.h"

#define FIRST_UDP_SOURCEPORT 8300

//! @brief Struct used internally by the TASServer class to calculate ping roundtimes.
struct TASPingListItem {
  int id;
  time_t t;
};

class Ui;
class Socket;
class User;
struct UserBattleStatus;
class ServerEvents;
class wxString;

//! @brief TASServer protocol implementation.
class TASServer : public Server
{
  public:
    TASServer( Ui& ui );
    ~TASServer();

    // Overloaded functions from Server
    bool ExecuteSayCommand( const wxString& cmd );

    void SetSocket( Socket* sock );

    bool Register( const wxString& addr, const int port, const wxString& nick, const wxString& password,wxString& reason );
    void AcceptAgreement();

    void Connect( const wxString& addr, const int port );
    void Disconnect();
    bool IsConnected();

    void Login();
    void Logout();
    bool IsOnline();

    void Update( int mselapsed );

    void Ping();


    /// generic udp "ping" function
    /// return value: actual source port which was used. May differ from src_port
    /// 0 if udp ping failed
    unsigned int UdpPing(unsigned int src_port, const wxString &target, unsigned int target_port, const wxString &message);
    /// specialized udp ping functions
    void UdpPingTheServer(const wxString &message=_T("ipv4 sux!"));/// used for nat travelsal. pings the server.
    void UdpPingAllClients();/// used when hosting with nat holepunching

    User& GetMe();

    void JoinChannel( const wxString& channel, const wxString& key );
    void PartChannel( const wxString& channel );

    void DoActionChannel( const wxString& channel, const wxString& msg );
    void SayChannel( const wxString& channel, const wxString& msg );

    void DoActionPrivate( const wxString& nick, const wxString& msg );
    void SayPrivate( const wxString& nick, const wxString& msg );

    void SayBattle( int battleid, const wxString& msg );
    void DoActionBattle( int battleid, const wxString& msg );

    void Ring( const wxString& nick );

    void ModeratorSetChannelTopic( const wxString& channel, const wxString& topic );
    void ModeratorSetChannelKey( const wxString& channel, const wxString& key );
    void ModeratorMute( const wxString& channel, const wxString& nick, int duration, bool byip );
    void ModeratorUnmute( const wxString& channel, const wxString& nick );
    void ModeratorKick( const wxString& channel, const wxString& reason );
    void ModeratorBan( const wxString& nick, bool byip );
    void ModeratorUnban( const wxString& nick );
    void ModeratorGetIP( const wxString& nick );
    void ModeratorGetLastLogin( const wxString& nick );
    void ModeratorGetLastIP( const wxString& nick );
    void ModeratorFindByIP( const wxString& ipadress );

    void AdminGetAccountAccess( const wxString& nick );
    void AdminChangeAccountAccess( const wxString& nick, const wxString& accesscode );
    void AdminSetBotMode( const wxString& nick, bool isbot );

    void HostBattle( BattleOptions bo, const wxString& password = _T("") );
    void JoinBattle( const int& battleid, const wxString& password = _T("") );
    void LeaveBattle( const int& battleid );
    void SendMyBattleStatus( UserBattleStatus& bs );
    void SendMyUserStatus();

    void ForceSide( int battleid, const wxString& nick, int side );
    void ForceTeam( int battleid, const wxString& nick, int team );
    void ForceAlly( int battleid, const wxString& nick, int ally );
    void ForceColour( int battleid, const wxString& nick, const wxColour& col );
    void ForceSpectator( int battleid, const wxString& nick, bool spectator );
    void BattleKickPlayer( int battleid, const wxString& nick );
    void SetHandicap( int battleid, const wxString& nick, int handicap);

    void AddBot( int battleid, const wxString& nick, const wxString& owner, UserBattleStatus status, const wxString& aidll );
    void RemoveBot( int battleid, const wxString& nick );
    void UpdateBot( int battleid, const wxString& nick, UserBattleStatus status );

    void StartHostedBattle();
    void SendHostInfo( HostInfo update );
    void SendHostInfo( const wxString& Tag );

    void SendRaw( const wxString& raw );

    void RequestInGameTime( const wxString& nick );

    void SendUdpSourcePort( int udpport );
    void SendNATHelperInfos( const wxString& username, const wxString& ip, int port );

    Battle* GetCurrentBattle();

    void RequestChannels();
    // TASServer specific functions
    void ExecuteCommand( const wxString& in );
    void ExecuteCommand( const wxString& cmd, const wxString& inparams, int replyid = -1 );

    void HandlePong( int replyid );
    void HandlePinglist();

    void OnConnected( Socket* sock );
    void OnDisconnected( Socket* sock );
    void OnDataReceived( Socket* sock );

    bool IsPasswordHash( const wxString& pass );
    wxString GetPasswordHash( const wxString& pass );

    bool TestOpenPort( unsigned int port );

  protected:
    Ui& m_ui;
    ServerEvents* m_se;
    double m_ser_ver;

    bool m_connected;
    bool m_online;
    wxString m_buffer;
    time_t m_last_udp_ping;
    int m_ping_id;
    std::list<TASPingListItem> m_pinglist;

    unsigned long m_udp_private_port;
    unsigned long m_nat_helper_port;

    int m_battle_id;

    wxString m_agreement;

    wxString m_addr;

    int m_finalize_join_battle_id;
    wxString m_finalize_join_battle_pw;
    bool m_do_finalize_join_battle;
    void FinalizeJoinBattle();

    void ReceiveAndExecute();
    void SendCmd( const wxString& command, const wxString& param = _T("") );
};

#endif // SPRINGLOBBY_HEADERGUARD_TASSERVER_H
