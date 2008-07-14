#ifndef SPRINGLOBBY_HEADERGUARD_SERVEREVENTS_H
#define SPRINGLOBBY_HEADERGUARD_SERVEREVENTS_H

#include "battle.h"

class Ui;
struct UserStatus;
struct UserBattleStatus;
class Server;

// FIXME this is defined elsewhere, should use a different kind of type so we could use forward decl
typedef int Sockerror;

typedef int Protocolerror;

//! @brief Class that implements server event behaviour.
class ServerEvents
{
  public:
    ServerEvents( Server& serv, Ui& ui ) : m_serv(serv),m_ui(ui) {}
    ~ServerEvents() {}

  // Uicontrol interface

    void OnConnected( const wxString& server_name, const wxString& server_ver, bool supported, const wxString& server_spring_ver, bool lanmode );
    void OnDisconnected();

    void OnLogin();
    void OnLoginInfoComplete();
    void OnLogout();

    void OnUnknownCommand( const wxString& command, const wxString& params );
    void OnSocketError( const Sockerror& error );
    void OnProtocolError( const Protocolerror error );
    void OnMotd( const wxString& msg );
    void OnPong( int ping_time );

    void OnNewUser( const wxString& nick, const wxString& conutry, int cpu );
    void OnUserStatus( const wxString& nick, UserStatus status );
    void OnUserQuit( const wxString& nick );

    void OnBattleOpened( int id, bool replay, NatType nat, const wxString& nick,
                         const wxString& host, int port, int maxplayers,
                         bool haspass, int rank, const wxString& maphash, const wxString& map,
                         const wxString& title, const wxString& mod );

    void OnUserJoinedBattle( int battleid, const wxString& nick );
    void OnUserLeftBattle( int battleid, const wxString& nick );
    void OnBattleInfoUpdated( int battleid, int spectators, bool locked, const wxString& maphash, const wxString& map );
    void OnSetBattleInfo( int battleid, const wxString& param, const wxString& value );
    void OnBattleInfoUpdated( int battleid );
    void OnBattleClosed( int battleid );

    void OnJoinedBattle( int battleid );
    void OnHostedBattle( int battleid );

    void OnStartHostedBattle( int battleid );
    void OnClientBattleStatus( int battleid, const wxString& nick, UserBattleStatus status );

    void OnBattleStartRectAdd( int battleid, int allyno, int left, int top, int right, int bottom );
    void OnBattleStartRectRemove( int battleid, int allyno );

    void OnBattleAddBot( int battleid, const wxString& nick, const wxString& owner, UserBattleStatus status, const wxString& aidll );
    void OnBattleUpdateBot( int battleid, const wxString& nick, UserBattleStatus status );
    void OnBattleRemoveBot( int battleid, const wxString& nick );

    void OnBattleDisableUnit( int battleid, const wxString& unitname );
    void OnBattleEnableUnit( int battleid, const wxString& unitname );
    void OnBattleEnableAllUnits( int battleid );

    void OnJoinChannelResult( bool success, const wxString& channel, const wxString& reason );

    void OnChannelSaid( const wxString& channel, const wxString& who, const wxString& message );
    void OnChannelJoin( const wxString& channel, const wxString& who );
    void OnChannelPart( const wxString& channel, const wxString& who, const wxString& message );
    void OnChannelTopic( const wxString& channel, const wxString& who, const wxString& message, int when );
    void OnChannelAction( const wxString& channel, const wxString& who, const wxString& action );
    void OnChannelList( const wxString& channel, const int& numusers );
    void OnUserJoinChannel( const wxString& channel, const wxString& who );

    void OnPrivateMessage( const wxString& user, const wxString& message, bool fromme = false );

    void OnRequestBattleStatus( int battleid );
    void OnSaidBattle( int battleid, const wxString& nick, const wxString& msg );
    void OnBattleAction( int battleid, const wxString& nick, const wxString& msg );

    void OnAcceptAgreement( const wxString& agreement );

    void OnRing( const wxString& from );

    void OnServerMessage( const wxString& message );
    void OnServerMessageBox( const wxString& message );
    void OnChannelMessage( const wxString& channel, const wxString& msg );

    void OnHostExternalUdpPort( const unsigned int udpport );

    void OnMyExternalUdpSourcePort( const unsigned int udpport );
    void OnMyInternalUdpSourcePort( const unsigned int udpport );

    void OnClientIPPort( const wxString &username, const wxString &ip, unsigned int udpport );

    void OnKickedFromBattle();

    void OnRedirect( const wxString& address,  unsigned int port, const wxString& CurrentNick, const wxString& CurrentPassword );

  protected:
    Server& m_serv;
    Ui& m_ui;
};

#endif // SPRINGLOBBY_HEADERGUARD_SERVEREVENTS_H
