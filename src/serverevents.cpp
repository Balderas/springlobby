/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: ServerEvents
//

#include <wx/intl.h>
#include <stdexcept>

#include "serverevents.h"
#include "mainwindow.h"
#include "ui.h"
#include "channel/channel.h"
#include "user.h"
#include "utils.h"
#include "server.h"
#include "battle.h"
#include "settings.h"
#include "settings++/custom_dialogs.h"
#ifndef NO_TORRENT_SYSTEM
#include "torrentwrapper.h"
#endif
#include "globalsmanager.h"

void ServerEvents::OnConnected( const wxString& server_name, const wxString& server_ver, bool supported, const wxString& server_spring_ver, bool lanmode )
{
    wxLogDebugFunc( server_ver + _T(" ") + server_spring_ver );
    m_serv.SetRequiredSpring( server_spring_ver );
    ui().OnConnected( m_serv, server_name, server_ver, supported );
    m_serv.Login();
}


void ServerEvents::OnDisconnected()
{
    wxLogDebugFunc( _T("") );
    m_serv.SetRequiredSpring (_T(""));
    ui().OnDisconnected( m_serv );
#ifndef NO_TORRENT_SYSTEM
    try /// settings may be already destroyed
    {
        if ( sett().GetTorrentSystemAutoStartMode() == 0 ) torrent().DisconnectFromP2PSystem();
    }
    catch (GlobalDestroyedError e)
    {
    }
#endif
}


void ServerEvents::OnLogin()
{

}


void ServerEvents::OnLoginInfoComplete()
{
    wxLogDebugFunc( _T("") );
    m_serv.JoinChannel( _T("springlobby"), _T("") );
    //m_serv.RequestChannels();
    int num = sett().GetNumChannelsJoin();
    for ( int i= 0; i < num; i++ )
    {
        wxString channel = sett().GetChannelJoinName(i);
        wxString pass = channel.AfterFirst(' ');
        if ( !pass.IsEmpty() ) channel = channel.BeforeFirst(' ');
        m_serv.JoinChannel( channel, pass );
    }
#ifndef NO_TORRENT_SYSTEM
    if ( sett().GetTorrentSystemAutoStartMode() == 0 ) torrent().ConnectToP2PSystem();
#endif
    ui().OnLoggedIn( );
}


void ServerEvents::OnLogout()
{
    //wxLogDebugFunc( _T("") );
}


void ServerEvents::OnUnknownCommand( const wxString& command, const wxString& params )
{
    wxLogDebugFunc( _T("") );
    ui().OnUnknownCommand( m_serv, command, params );
}


void ServerEvents::OnSocketError( const Sockerror& error )
{
    //wxLogDebugFunc( _T("") );
}


void ServerEvents::OnProtocolError( const Protocolerror error )
{
    //wxLogDebugFunc( _T("") );
}


void ServerEvents::OnMotd( const wxString& msg )
{
    wxLogDebugFunc( _T("") );
    ui().OnMotd( m_serv, msg );
}


void ServerEvents::OnPong( int ping_time )
{
    if ( ping_time == -1 )
    {
        wxLogWarning( _T("Ping Timeout!") );
        if ( m_serv.IsConnected() ) m_serv.Disconnect();
    }
}


void ServerEvents::OnNewUser( const wxString& nick, const wxString& country, int cpu )
{
    wxLogDebugFunc( _T("") );
    try
    {
        ASSERT_LOGIC( !m_serv.UserExists( nick ), _T("New user from server, but already exists!") );
    }
    catch (...)
    {
        return;
    }
    User& user = m_serv._AddUser( nick );
    if ( useractions().DoActionOnUser( UserActions::ActNotifLogin, nick ) )
        actNotifBox( SL_MAIN_ICON, nick + _(" is online") );
    user.SetCountry( country );
    user.SetCpu( cpu );
    ui().OnUserOnline( user );
}


void ServerEvents::OnUserStatus( const wxString& nick, UserStatus status )
{
    wxLogDebugFunc( _T("") );
    try
    {
        wxLogMessage( _T("calling m_serv.GetUser( nick ) ") );
        User& user = m_serv.GetUser( nick );
        wxLogMessage( _T("calling user.SetStatus( status ) ") );

        UserStatus oldStatus = user.GetStatus();
        user.SetStatus( status );
        if ( useractions().DoActionOnUser( UserActions::ActNotifStatus, nick ) )
        {
            wxString diffString = status.GetDiffString( oldStatus ) ;
            if ( diffString != wxEmptyString )
                actNotifBox( SL_MAIN_ICON, nick + _(" is now ") + diffString );
        }

        wxLogMessage( _T("calling ui().OnUserStatusChanged( user ) ") );
        ui().OnUserStatusChanged( user );
        wxLogMessage( _T("updating battles ") );

        if ( user.GetBattle() != 0 )
        {
            Battle& battle = *user.GetBattle();
            if ( battle.GetFounder().GetNick() == user.GetNick() )
            {
                if ( status.in_game != battle.GetInGame() )
                {
                    battle.SetInGame( status.in_game );
                    if ( status.in_game ) ui().OnBattleStarted( battle );
                    else ui().OnBattleInfoUpdated( battle );
                }
            }
        }
    }
    catch (...)
    {
        wxLogWarning( _("OnUserStatus() failed ! (exception)") );
    }
}


void ServerEvents::OnUserQuit( const wxString& nick )
{
    wxLogDebugFunc( _T("") );
    try
    {
        User &user=m_serv.GetUser( nick );
        ui().OnUserOffline( user );
        m_serv._RemoveUser( nick );
        if ( useractions().DoActionOnUser( UserActions::ActNotifLogin, nick ) )
            actNotifBox( SL_MAIN_ICON, nick + _(" just went offline") );
    }
    catch (std::runtime_error &except)
    {
    }
}


void ServerEvents::OnBattleOpened( int id, bool replay, NatType nat, const wxString& nick,
                                   const wxString& host, int port, int maxplayers,
                                   bool haspass, int rank, const wxString& maphash, const wxString& map,
                                   const wxString& title, const wxString& mod )
{
    wxLogDebugFunc( _T("") );
    try
    {
        ASSERT_EXCEPTION( !m_serv.BattleExists( id ), _T("New battle from server, but already exists!") );
        Battle& battle = m_serv._AddBattle( id );

        User& user = m_serv.GetUser( nick );
        battle.OnUserAdded( user );

        battle.SetIsReplay( replay );
        battle.SetNatType( nat );
        battle.SetFounder( nick );
        battle.SetHostIp( host );
        battle.SetHostPort( port );
        battle.SetMaxPlayers( maxplayers );
        battle.SetIsPassworded( haspass );
        battle.SetRankNeeded( ( rank / 100 ) +1 );
        battle.SetHostMap( map, maphash );
        battle.SetDescription( title );
        battle.SetHostMod( mod, wxEmptyString );

        if ( useractions().DoActionOnUser( UserActions::ActNotifBattle, user.GetNick() ) )
            actNotifBox( SL_MAIN_ICON, user.GetNick() + _(" opened battle ") + title );

        ui().OnBattleOpened( battle );
        if ( user.Status().in_game )
        {
            battle.SetInGame( true );
            ui().OnBattleStarted( battle );
        }
    }
    catch (std::runtime_error &except)
    {
    }
}


void ServerEvents::OnJoinedBattle( int battleid, const wxString& hash )
{
    wxLogDebugFunc( _T("") );
    try
    {
        Battle& battle = m_serv.GetBattle( battleid );

        battle.SetHostMod( battle.GetHostModName(), hash );

        UserBattleStatus& bs = m_serv.GetMe().BattleStatus();
        bs.spectator = false;

        if ( !battle.IsFounderMe() || battle.IsProxy() )
        {
            battle.CustomBattleOptions().loadOptions( OptionsWrapper::MapOption, battle.GetHostMapName() );
            battle.CustomBattleOptions().loadOptions( OptionsWrapper::ModOption, battle.GetHostModName() );
        }

        ui().OnJoinedBattle( battle );
    }
    catch (std::runtime_error &except)
    {
    }
}


void ServerEvents::OnHostedBattle( int battleid )
{
    wxLogDebugFunc( _T("") );
    try
    {
        Battle& battle = m_serv.GetBattle( battleid );

        battle.CustomBattleOptions().loadOptions( OptionsWrapper::MapOption, battle.GetHostMapName() );
        battle.CustomBattleOptions().loadOptions( OptionsWrapper::ModOption, battle.GetHostModName() );

        wxString presetname = sett().GetModDefaultPresetName( battle.GetHostModName() );
        if ( !presetname.IsEmpty() )
        {
            battle.LoadOptionsPreset( presetname );
        }

        m_serv.SendHostInfo( IBattle::HI_Send_All_opts );

        ui().OnHostedBattle( battle );
    }
    catch (assert_exception) {}
}


void ServerEvents::OnStartHostedBattle( int battleid )
{
    wxLogDebugFunc( _T("") );
    Battle& battle = m_serv.GetBattle( battleid );
    battle.SetInGame( true );
    ui().OnBattleStarted( battle );
}


void ServerEvents::OnClientBattleStatus( int battleid, const wxString& nick, UserBattleStatus status )
{
    try
    {
        Battle& battle = m_serv.GetBattle( battleid );
        User& user = battle.GetUser( nick );

        if ( battle.IsFounderMe() ) AutoCheckCommandSpam( battle, user );

        status.color_index = user.BattleStatus().color_index;
        battle.OnUserBattleStatusUpdated( user, status );
    }
    catch (std::runtime_error &except)
    {
    }
}


void ServerEvents::OnUserJoinedBattle( int battleid, const wxString& nick )
{
    try
    {
        wxLogDebugFunc( _T("") );
        User& user = m_serv.GetUser( nick );
        Battle& battle = m_serv.GetBattle( battleid );

        battle.OnUserAdded( user );
        ui().OnUserJoinedBattle( battle, user );

        if ( &user == &battle.GetFounder() )
        {
            if ( user.Status().in_game )
            {
                battle.SetInGame( true );
                ui().OnBattleStarted( battle );
            }
        }
        if ( battle.IsFounderMe() &&  ( battle.GetNumUsers() > 31 ) )
        {
          battle.DoAction( _T("is locking the battle because Spring does not support more than 32 players (including spectators)" ) );
          battle.SetIsLocked( true );
          battle.SendHostInfo( IBattle::HI_Locked );
        }
    }
    catch (std::runtime_error &except)
    {
    }
}


void ServerEvents::OnUserLeftBattle( int battleid, const wxString& nick )
{
    wxLogDebugFunc( _T("") );
    try
    {
        Battle& battle = m_serv.GetBattle( battleid );
				User& user = battle.GetUser( nick );

        battle.OnUserRemoved( user );

        ui().OnUserLeftBattle( battle, user );
    }
    catch (std::runtime_error &except)
    {
    }

}


void ServerEvents::OnBattleInfoUpdated( int battleid, int spectators, bool locked, const wxString& maphash, const wxString& map )
{
    wxLogDebugFunc( _T("") );
    try
    {
        Battle& battle = m_serv.GetBattle( battleid );

        battle.SetSpectators( spectators );
        battle.SetIsLocked( locked );

        wxString oldmap = battle.GetHostMapName();

        battle.SetHostMap( map, maphash );

        if ( (oldmap != map) && (battle.UserExists( m_serv.GetMe().GetNick())) )
        {
            battle.SendMyBattleStatus();
            battle.CustomBattleOptions().loadOptions( OptionsWrapper::MapOption, map );
            battle.Update( wxString::Format( _T("%d_mapname"), OptionsWrapper::PrivateOptions ) );
        }

        ui().OnBattleInfoUpdated( battle );
    }
    catch (assert_exception) {}
}

void ServerEvents::OnSetBattleInfo( int battleid, const wxString& param, const wxString& value )
{
    wxLogDebugFunc( param + _T(", ") + value );
    try
    {
        Battle& battle = m_serv.GetBattle( battleid );

        wxString key = param;
        if ( key.Left( 5 ) == _T("game/") )
        {
            key = key.AfterFirst( '/' );
            if ( key.Left( 11 ) == _T( "mapoptions/" ) )
            {
                key = key.AfterFirst( '/' );
                battle.CustomBattleOptions().setSingleOption( key,  value, OptionsWrapper::MapOption );
								battle.Update( wxString::Format(_T("%d_%s"), OptionsWrapper::MapOption, key.c_str() ) );
            }
            else if ( key.Left( 11 ) == _T( "modoptions/" ) )
            {
                key = key.AfterFirst( '/' );
								battle.CustomBattleOptions().setSingleOption( key, value, OptionsWrapper::ModOption );
                battle.Update(  wxString::Format(_T("%d_%s"), OptionsWrapper::ModOption,  key.c_str() ) );
            }
            else
            {
							battle.CustomBattleOptions().setSingleOption( key,  value, OptionsWrapper::EngineOption );
							battle.Update( wxString::Format(_T("%d_%s"), OptionsWrapper::EngineOption, key.c_str() ) );
            }
        }
    }
    catch (assert_exception) {}
}


void ServerEvents::OnBattleInfoUpdated( int battleid )
{
    wxLogDebugFunc( _T("") );
    try
    {
        Battle& battle = m_serv.GetBattle( battleid );
        ui().OnBattleInfoUpdated( battle );
    }
    catch ( assert_exception ) {}
}


void ServerEvents::OnBattleClosed( int battleid )
{
    wxLogDebugFunc( _T("") );
    try
    {
        Battle& battle = m_serv.GetBattle( battleid );

        ui().OnBattleClosed( battle );

        m_serv._RemoveBattle( battleid );
    }
    catch ( assert_exception ) {}
}


void ServerEvents::OnBattleDisableUnit( int battleid, const wxString& unitname )
{
    wxLogDebugFunc( _T("") );
    try
    {
        Battle& battle = m_serv.GetBattle( battleid );
        battle.DisableUnit( unitname );
        battle.Update( wxString::Format( _T("%d_restrictions"), OptionsWrapper::PrivateOptions ) );
    }
    catch ( assert_exception ) {}
}


void ServerEvents::OnBattleEnableUnit( int battleid, const wxString& unitname )
{
    wxLogDebugFunc( _T("") );
    try
    {
        Battle& battle = m_serv.GetBattle( battleid );
        battle.EnableUnit( unitname );
        battle.Update( wxString::Format( _T("%d_restrictions"), OptionsWrapper::PrivateOptions ) );
    }
    catch ( assert_exception ) {}
}


void ServerEvents::OnBattleEnableAllUnits( int battleid )
{
    wxLogDebugFunc( _T("") );
    try
    {
        Battle& battle = m_serv.GetBattle( battleid );
        battle.EnableAllUnits();
        battle.Update( wxString::Format( _T("%d_restrictions"), OptionsWrapper::PrivateOptions ) );
    }
    catch ( assert_exception ) {}
}


void ServerEvents::OnJoinChannelResult( bool success, const wxString& channel, const wxString& reason )
{
    wxLogDebugFunc( _T("") );
    if ( success )
    {

        Channel& chan = m_serv._AddChannel( channel );
        chan.SetPassword( m_serv.m_channel_pw[channel] );
        ui().OnJoinedChannelSuccessful( chan );
        if ( channel == _T("springlobby") && sett().GetReportStats() )
        {
            m_serv.DoActionChannel( _T("springlobby"), _T("is using SpringLobby v") + GetSpringLobbyVersion() );
        }

    }
    else
    {
        ui().ShowMessage( _("Join channel failed"), _("Could not join channel ") + channel + _(" because: ") + reason );
    }
}


void ServerEvents::OnChannelSaid( const wxString& channel, const wxString& who, const wxString& message )
{
    wxLogDebugFunc( _T("") );
    try
    {
        if ( ( m_serv.GetMe().GetNick() ==  who ) || !useractions().DoActionOnUser( UserActions::ActIgnoreChat, who ) )
            m_serv.GetChannel( channel ).Said( m_serv.GetUser( who ), message );
    }
    catch (std::runtime_error &except)
    {
    }
}


void ServerEvents::OnChannelJoin( const wxString& channel, const wxString& who )
{
    wxLogDebugFunc( _T("") );
    try
    {
        m_serv.GetChannel( channel ).OnChannelJoin( m_serv.GetUser( who ) );
    }
    catch (std::runtime_error &except)
    {
    }
}


void ServerEvents::OnChannelPart( const wxString& channel, const wxString& who, const wxString& message )
{
    wxLogDebugFunc( _T("") );
    try
    {
        m_serv.GetChannel( channel ).Left( m_serv.GetUser( who ), message );
    }
    catch (std::runtime_error &except)
    {
    }
}


void ServerEvents::OnChannelTopic( const wxString& channel, const wxString& who, const wxString& message, int when )
{
    wxLogDebugFunc( _T("") );
    try
    {
        m_serv.GetChannel( channel ).SetTopic( message, who );
    }
    catch (std::runtime_error &except)
    {
    }
}


void ServerEvents::OnChannelAction( const wxString& channel, const wxString& who, const wxString& action )
{
    wxLogDebugFunc( _T("") );
    try
    {
        m_serv.GetChannel( channel ).DidAction( m_serv.GetUser( who ), action );
    }
    catch (std::runtime_error &except)
    {
    }
}


void ServerEvents::OnPrivateMessage( const wxString& user, const wxString& message, bool fromme )
{
    wxLogDebugFunc( _T("") );
    try
    {
        User& who = m_serv.GetUser( user );
        if (!useractions().DoActionOnUser( UserActions::ActIgnorePM, who.GetNick() ) )
            ui().OnUserSaid( who, message, fromme );
    }
    catch (std::runtime_error &except)
    {
    }
}

void ServerEvents::OnChannelList( const wxString& channel, const int& numusers, const wxString& topic )
{
    ui().mw().OnChannelList( channel, numusers, topic );
}


void ServerEvents::OnUserJoinChannel( const wxString& channel, const wxString& who )
{
    wxLogDebugFunc( _T("") );
    try
    {
        m_serv.GetChannel( channel ).Joined( m_serv.GetUser( who ) );
    }
    catch (std::runtime_error &except)
    {
    }
}


void ServerEvents::OnRequestBattleStatus( int battleid )
{
    try
    {
        Battle& battle = m_serv.GetBattle( battleid );
        ui().OnRequestBattleStatus( battle );
    }
    catch (assert_exception) {}
}


void ServerEvents::OnSaidBattle( int battleid, const wxString& nick, const wxString& msg )
{
    try
    {
        Battle& battle = m_serv.GetBattle( battleid );
        ui().OnSaidBattle( battle, nick, msg );
        battle.GetAutoHost().OnSaidBattle( nick, msg );
    }
    catch (assert_exception) {}
}

void ServerEvents::OnBattleAction( int battleid, const wxString& nick, const wxString& msg )
{
    try
    {
        Battle& battle = m_serv.GetBattle( battleid );
        ui().OnBattleAction( battle, nick, msg );
    }
    catch (assert_exception) {}
}


void ServerEvents::OnBattleStartRectAdd( int battleid, int allyno, int left, int top, int right, int bottom )
{
    try
    {
        Battle& battle = m_serv.GetBattle( battleid );
        battle.AddStartRect( allyno, left, top, right, bottom );
        battle.StartRectAdded( allyno );
        battle.Update( wxString::Format( _T("%d_mapname"), OptionsWrapper::PrivateOptions ) );
    }
    catch (assert_exception) {}
}


void ServerEvents::OnBattleStartRectRemove( int battleid, int allyno )
{
    try
    {
        Battle& battle = m_serv.GetBattle( battleid );
        battle.RemoveStartRect( allyno );
        battle.StartRectRemoved( allyno );
        battle.Update( wxString::Format( _T("%d_mapname"), OptionsWrapper::PrivateOptions ) );
    }
    catch (assert_exception) {}
}


void ServerEvents::OnBattleAddBot( int battleid, const wxString& nick, UserBattleStatus status )
{
    wxLogDebugFunc( _T("") );
    try
    {
        Battle& battle = m_serv.GetBattle( battleid );
        battle.OnBotAdded( nick, status );
        User& bot = battle.GetUser( nick );
        ASSERT_LOGIC( &bot != 0, _T("Bot null after add.") );
        ui().OnUserJoinedBattle( battle, bot );
    }
    catch (assert_exception) {}
}

void ServerEvents::OnBattleUpdateBot( int battleid, const wxString& nick, UserBattleStatus status )
{
    OnClientBattleStatus( battleid, nick, status );
}


void ServerEvents::OnBattleRemoveBot( int battleid, const wxString& nick )
{
    OnUserLeftBattle( battleid, nick );
}


void ServerEvents::OnAcceptAgreement( const wxString& agreement )
{
    ui().OnAcceptAgreement( agreement );
}


void ServerEvents::OnRing( const wxString& from )
{
    ui().OnRing( from );
}


void ServerEvents::OnServerMessage( const wxString& message )
{
    ui().OnServerMessage( m_serv, message );
}


void ServerEvents::OnServerMessageBox( const wxString& message )
{
    ui().ShowMessage( _("Server Message"), message );
}


void ServerEvents::OnChannelMessage( const wxString& channel, const wxString& msg )
{
    ui().OnChannelMessage( channel, msg );
}


void ServerEvents::OnHostExternalUdpPort( const unsigned int udpport )
{
    if ( !m_serv.GetCurrentBattle() ) return;
    if ( m_serv.GetCurrentBattle()->GetNatType() == NAT_Hole_punching || m_serv.GetCurrentBattle()->GetNatType() == NAT_Fixed_source_ports ) m_serv.GetCurrentBattle()->SetHostPort( udpport );
}


void ServerEvents::OnMyInternalUdpSourcePort( const unsigned int udpport )
{
    if ( !m_serv.GetCurrentBattle() ) return;
    m_serv.GetCurrentBattle()->SetMyInternalUdpSourcePort(udpport);
}


void ServerEvents::OnMyExternalUdpSourcePort( const unsigned int udpport )
{
    if ( !m_serv.GetCurrentBattle() ) return;
    m_serv.GetCurrentBattle()->SetMyExternalUdpSourcePort(udpport);
}

void ServerEvents::OnClientIPPort( const wxString &username, const wxString &ip, unsigned int udpport )
{
    wxLogMessage(_T("OnClientIPPort(%s,%s,%d)"),username.c_str(),ip.c_str(),udpport);
    if ( !m_serv.GetCurrentBattle() )
    {
        wxLogMessage(_T("GetCurrentBattle() returned null"));
        return;
    }
    try
    {
        User &user=m_serv.GetCurrentBattle()->GetUser( username );

        user.BattleStatus().ip=ip;
        user.BattleStatus().udpport=udpport;
        wxLogMessage(_T("set to %s %d "),user.BattleStatus().ip.c_str(),user.BattleStatus().udpport);

        if (sett().GetShowIPAddresses())ui().OnBattleAction(*m_serv.GetCurrentBattle(),username,wxString::Format(_(" has ip=%s"),ip.c_str()));

        if (m_serv.GetCurrentBattle()->GetNatType() != NAT_None && (udpport==0))
        {
            /// todo: better warning message
            ///something.OutputLine( _T(" ** ") + who.GetNick() + _(" does not support nat traversal! ") + GetChatTypeStr() + _T("."), sett().GetChatColorJoinPart(), sett().GetChatFont() );
            ui().OnBattleAction(*m_serv.GetCurrentBattle(),username,_(" does not really support nat traversal"));
        }
        m_serv.GetCurrentBattle()->CheckBan(user);
    }
    catch (std::runtime_error)
    {
        wxLogMessage(_T("runtime_error inside OnClientIPPort()"));
    }
}


void ServerEvents::OnKickedFromBattle()
{
    customMessageBoxNoModal(SL_MAIN_ICON,_("You were kicked from the battle!"),_("Kicked by Host"));
}


void ServerEvents::OnRedirect( const wxString& address,  unsigned int port, const wxString& CurrentNick, const wxString& CurrentPassword )
{
    sett().AddServer( address );
    sett().SetServerHost( address, address );
    sett().SetServerPort( address, (int)port );
    ui().DoConnect( address, CurrentNick, CurrentPassword );
}


void ServerEvents::AutoCheckCommandSpam( Battle& battle, User& user )
{
    wxString nick = user.GetNick();
    MessageSpamCheck info = m_spam_check[nick];
    time_t now = time( 0 );
    if ( info.lastmessage == now ) info.count++;
    else info.count = 0;
    info.lastmessage = now;
    m_spam_check[nick] = info;
    if ( info.count == 7 )
    {
			battle.DoAction( _T("is autokicking ") + nick + _T(" due to command spam.") );
			battle.KickPlayer( user );
    }
}

void ServerEvents::OnMutelistBegin( const wxString& channel )
{
    mutelistWindow( _("Begin mutelist for ") + channel, channel + _(" mutelist") );
}

void ServerEvents::OnMutelistItem( const wxString& channel, const wxString& mutee, const wxString& description )
{
    wxString message = mutee;
    if ( description == _T("indefinite") )
        message << _(" indefinite time remaining");
    else
        message << wxString::Format( _(" %d minutes remaining") , s2l(description)/60 + 1 ) ;
    mutelistWindow( message );
}

void ServerEvents::OnMutelistEnd( const wxString& channel )
{
    mutelistWindow( _("End mutelist for ") + channel );
}
