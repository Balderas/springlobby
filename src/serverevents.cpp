/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

//
// Class: ServerEvents
//

#ifdef _MSC_VER
#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX
#include <winsock2.h>
#endif // _MSC_VER

#include <wx/intl.h>
#include <wx/log.h>
#include <stdexcept>

#include "serverevents.h"
#include "gui/mainwindow.h"
#include "gui/ui.h"
#include "channel.h"
#include "user.h"
#include "gui/uiutils.h"
#include "iserver.h"
#include "settings.h"
#include "gui/customdialogs.h"
#include "utils/tasutil.h"
#include "utils/uievents.h"
#include "log.h"
#include "utils/conversion.h"
#include "autohostmanager.h"
#include "autohost.h"

#include <lslutils/globalsmanager.h>
#include <exception>

void ServerEvents::OnConnected(const std::string& server_name, const std::string& server_ver, bool supported, const std::string& server_spring_ver, bool /*unused*/)
{
	slLogDebugFunc("%s %s", server_ver.c_str(), server_spring_ver.c_str());
	//Server version will include patchlevel from release 89 onwards
	m_serv.SetRequiredSpring(LSL::Util::BeforeFirst(server_spring_ver, "."));
	ui().OnConnected(m_serv, TowxString(server_name), TowxString(server_spring_ver), supported);
	m_serv.Login();
}


void ServerEvents::OnDisconnected(bool wasonline)
{
	slLogDebugFunc("");
	m_serv.SetRequiredSpring("");
	try {
		ui().OnDisconnected(m_serv, wasonline);
	} catch (LSL::Util::GlobalDestroyedError&) {
		/* At the end of the program, the global reference in ui() might already have been nullified. */
	}
}


void ServerEvents::OnLogin()
{
}


void ServerEvents::OnLoginInfoComplete()
{
	slLogDebugFunc("");
	wxString nick = TowxString(m_serv.GetMe().GetNick());
	wxArrayString highlights = sett().GetHighlightedWords();
	if (highlights.Index(nick) == -1) {
		highlights.Add(nick);
		sett().SetHighlightedWords(highlights);
	}
	//m_serv.RequestChannels();
	GlobalEvent::Send(GlobalEvent::OnLogin);
	std::vector<ChannelJoinInfo> autojoin = sett().GetChannelsJoin();
	for (std::vector<ChannelJoinInfo>::const_iterator itor = autojoin.begin(); itor != autojoin.end(); ++itor) {
		if (itor->name.IsEmpty())
			continue;
		Channel& chan = m_serv._AddChannel(STD_STRING(itor->name));
		chan.SetPassword(STD_STRING(itor->password));
		ui().OnJoinedChannelSuccessful(chan, itor == autojoin.begin());
	}
	for (std::vector<ChannelJoinInfo>::const_iterator itor = autojoin.begin(); itor != autojoin.end(); ++itor)
		m_serv.JoinChannel(STD_STRING(itor->name), STD_STRING(itor->password));

	ui().OnLoggedIn();
}


void ServerEvents::OnUnknownCommand(const std::string& command, const std::string& params)
{
	slLogDebugFunc("");
	ui().OnUnknownCommand(m_serv, TowxString(command), TowxString(params));
}


void ServerEvents::OnSocketError(const Sockerror& /*unused*/)
{
	//wxLogDebugFunc( wxEmptyString );
}


void ServerEvents::OnProtocolError(const Protocolerror /*unused*/)
{
	//wxLogDebugFunc( wxEmptyString );
}


void ServerEvents::OnMotd(const std::string& msg)
{
	slLogDebugFunc("");
	ui().OnMotd(m_serv, TowxString(msg));
}


void ServerEvents::OnPong(wxLongLong ping_time)
{
	//wxLongLong is non-POD and cannot be passed to wxString::Format as such. use c-string rep instead. converting to long might loose precision
	UiEvents::StatusData data(wxString::Format(_("ping: %s ms"), ping_time.ToString().c_str()), 2);
	UiEvents::GetStatusEventSender(UiEvents::addStatusMessage).SendEvent(data);
}


void ServerEvents::OnNewUser(const std::string& nick, const std::string& country, int cpu, int id)
{
	slLogDebugFunc("");
	try {
		ASSERT_LOGIC(!m_serv.UserExists(nick), ("New user from server, but already exists!"));
	} catch (...) {
		return;
	}
	User& user = m_serv._AddUser(nick);
	if (useractions().DoActionOnUser(UserActions::ActNotifLogin, TowxString(nick))) {
		actNotifBox(SL_MAIN_ICON, TowxString(nick) + _(" is online"));
	}
	user.SetCountry(country);
	user.SetCpu(cpu);
	user.SetID(id);
	ui().OnUserOnline(user);
}


void ServerEvents::OnUserStatus(const std::string& nick, UserStatus status)
{
	slLogDebugFunc("");
	try {
		User& user = m_serv.GetUser(nick);

		UserStatus oldStatus = user.GetStatus();
		user.SetStatus(status);
		if (useractions().DoActionOnUser(UserActions::ActNotifStatus, TowxString(nick))) {
			wxString diffString = TowxString(status.GetDiffString(oldStatus));
			if (diffString != wxEmptyString)
				actNotifBox(SL_MAIN_ICON, TowxString(nick) + _(" is now ") + diffString);
		}

		ui().OnUserStatusChanged(user);
		if (user.GetBattle() != 0) {
			IBattle& battle = *user.GetBattle();
			try {
				if (battle.GetFounder().GetNick() == user.GetNick()) {
					if (status.in_game != battle.GetInGame()) {
						battle.SetInGame(status.in_game);
						if (status.in_game)
							battle.StartSpring();
						else
							BattleEvents::GetBattleEventSender(BattleEvents::BattleInfoUpdate).SendEvent(std::make_pair(user.GetBattle(), ""));
					}
				}
			} catch (...) {
			}
		}
	} catch (...) {
		wxLogWarning(_("OnUserStatus() failed ! (exception)"));
	}
}


void ServerEvents::OnUserQuit(const std::string& nick)
{
	slLogDebugFunc("");
	try {
		User& user = m_serv.GetUser(nick);
		IBattle* userbattle = user.GetBattle();
		if (userbattle) {
			int battleid = userbattle->GetID();
			try {
				if (&userbattle->GetFounder() == &user) {
					for (int i = 0; i < int(userbattle->GetNumUsers()); i++) {
						User& battleuser = userbattle->GetUser(i);
						OnUserLeftBattle(battleid, battleuser.GetNick());
					}
					OnBattleClosed(battleid);
				} else
					OnUserLeftBattle(battleid, user.GetNick());
			} catch (...) {
			}
		}
		ui().OnUserOffline(user);
		m_serv._RemoveUser(nick);
		if (useractions().DoActionOnUser(UserActions::ActNotifLogin, TowxString(nick)))
			actNotifBox(SL_MAIN_ICON, TowxString(nick) + _(" just went offline"));
	} catch (std::runtime_error& except) {
	}
}


void ServerEvents::OnBattleOpened(int id, BattleType type, NatType nat, const std::string& nick,
				  const std::string& host, int port, int maxplayers,
				  bool haspass, int rank, const std::string& maphash, const std::string& engineName, const std::string& engineVersion, const std::string& map,
				  const std::string& title, const std::string& mod)
{
	slLogDebugFunc("");
	try {
		ASSERT_EXCEPTION(!m_serv.BattleExists(id), _T("New battle from server, but already exists!"));
		IBattle& battle = m_serv._AddBattle(id);

		User& user = m_serv.GetUser(nick);
		battle.OnUserAdded(user);

		battle.SetBattleType(type);
		battle.SetNatType(nat);
		battle.SetFounder(nick);
		battle.SetHostIp(host);
		battle.SetHostPort(port);
		battle.SetMaxPlayers(maxplayers);
		battle.SetIsPassworded(haspass);
		battle.SetRankNeeded(rank);
		battle.SetHostMap(map, maphash);
		battle.SetDescription(title);
		battle.SetHostMod(mod, "");
		battle.SetEngineName(engineName);
		battle.SetEngineVersion(engineVersion);

		if (useractions().DoActionOnUser(UserActions::ActNotifBattle, TowxString(user.GetNick())))
			actNotifBox(SL_MAIN_ICON, TowxString(user.GetNick()) + _(" opened battle ") + TowxString(title));

		ui().OnBattleOpened(battle);
		if (user.Status().in_game) {
			battle.SetInGame(true);
			battle.StartSpring();
		}
	} catch (std::runtime_error& except) {
	}
}


void ServerEvents::OnJoinedBattle(int battleid, const std::string& hash)
{
	slLogDebugFunc("");
	try {
		IBattle& battle = m_serv.GetBattle(battleid);

		battle.SetHostMod(battle.GetHostModName(), hash);
		UserBattleStatus& bs = m_serv.GetMe().BattleStatus();
		bs.spectator = false;

		if (!battle.IsFounderMe() || battle.IsProxy()) {
			battle.CustomBattleOptions().loadOptions(LSL::Enum::MapOption, battle.GetHostMapName());
			battle.CustomBattleOptions().loadOptions(LSL::Enum::ModOption, battle.GetHostModName());
		}

		ui().OnJoinedBattle(battle);
	} catch (std::runtime_error& except) {
	}
}


void ServerEvents::OnHostedBattle(int battleid)
{
	slLogDebugFunc("");
	try {
		IBattle& battle = m_serv.GetBattle(battleid);

		if (battle.GetBattleType() == BT_Played) {
			battle.CustomBattleOptions().loadOptions(LSL::Enum::MapOption, battle.GetHostMapName());
			battle.CustomBattleOptions().loadOptions(LSL::Enum::ModOption, battle.GetHostModName());
		} else {
			battle.GetBattleFromScript(true);
		}


		const std::string presetname = STD_STRING(sett().GetModDefaultPresetName(TowxString(battle.GetHostModName())));
		if (!presetname.empty()) {
			battle.LoadOptionsPreset(presetname);
		}

		battle.LoadMapDefaults(battle.GetHostMapName());

		m_serv.SendHostInfo(IBattle::HI_Send_All_opts);

		ui().OnHostedBattle(battle);
	} catch (assert_exception) {
	}
}


void ServerEvents::OnStartHostedBattle(int battleid)
{
	slLogDebugFunc("");
	IBattle& battle = m_serv.GetBattle(battleid);
	battle.SetInGame(true);
	battle.StartSpring();
}


void ServerEvents::OnClientBattleStatus(int battleid, const std::string& nick, UserBattleStatus status)
{
	try {
		IBattle& battle = m_serv.GetBattle(battleid);
		User& user = battle.GetUser(nick);

		//if ( battle.IsFounderMe() ) AutoCheckCommandSpam( battle, user );

		status.color_index = user.BattleStatus().color_index;
		battle.OnUserBattleStatusUpdated(user, status);
	} catch (std::runtime_error& except) {
	}
}


void ServerEvents::OnUserJoinedBattle(int battleid, const std::string& nick, const std::string& userScriptPassword)
{
	try {
		slLogDebugFunc("");
		User& user = m_serv.GetUser(nick);
		IBattle& battle = m_serv.GetBattle(battleid);

		battle.OnUserAdded(user);
		user.BattleStatus().scriptPassword = userScriptPassword;
		ui().OnUserJoinedBattle(battle, user);
		try {
			if (&user == &battle.GetFounder()) {
				if (user.Status().in_game) {
					battle.SetInGame(true);
					battle.StartSpring();
				}
			}
		} catch (...) {
		}
	} catch (std::runtime_error& except) {
	}
}


void ServerEvents::OnUserLeftBattle(int battleid, const std::string& nick)
{
	slLogDebugFunc("");
	try {
		IBattle& battle = m_serv.GetBattle(battleid);
		User& user = battle.GetUser(nick);
		// this is necessary since the user will be deleted when the gui function is called
		bool isbot = user.BattleStatus().IsBot();
		user.BattleStatus().scriptPassword.clear();
		battle.OnUserRemoved(user);
		ui().OnUserLeftBattle(battle, user, isbot);
	} catch (std::runtime_error& except) {
	}
}


void ServerEvents::OnBattleInfoUpdated(int battleid, int spectators, bool locked, const std::string& maphash, const std::string& map)
{
	slLogDebugFunc("");
	try {
		IBattle& battle = m_serv.GetBattle(battleid);

		battle.SetSpectators(spectators);
		battle.SetIsLocked(locked);

		const std::string oldmap = battle.GetHostMapName();

		battle.SetHostMap(map, maphash);

		if ((oldmap != map) && (battle.UserExists(m_serv.GetMe().GetNick()))) {
			battle.SendMyBattleStatus();
			battle.CustomBattleOptions().loadOptions(LSL::Enum::MapOption, map);
			battle.Update(stdprintf("%d_mapname", LSL::Enum::PrivateOptions));
		}

		BattleEvents::GetBattleEventSender(BattleEvents::BattleInfoUpdate).SendEvent(std::make_pair(&battle, ""));
	} catch (assert_exception) {
	}
}

void ServerEvents::OnSetBattleInfo(int battleid, const std::string& param, const std::string& value)
{
	slLogDebugFunc("%s, %s", param.c_str(), value.c_str());
	try {
		IBattle& battle = m_serv.GetBattle(battleid);
		battle.m_script_tags[param] = value;
		wxString key = TowxString(param);
		if (key.Left(5) == _T("game/")) {
			key = key.AfterFirst('/');
			if (key.Left(11) == _T( "mapoptions/" )) {
				key = key.AfterFirst('/');
				battle.CustomBattleOptions().setSingleOption(STD_STRING(key), value, LSL::Enum::MapOption);
				battle.Update(stdprintf("%d_%s", LSL::Enum::MapOption, STD_STRING(key).c_str()));
			} else if (key.Left(11) == _T( "modoptions/" )) {
				key = key.AfterFirst('/');
				battle.CustomBattleOptions().setSingleOption(STD_STRING(key), value, LSL::Enum::ModOption);
				battle.Update(stdprintf("%d_%s", LSL::Enum::ModOption, STD_STRING(key).c_str()));
			} else if (key.Left(8) == _T( "restrict" )) {
				OnBattleDisableUnit(battleid, STD_STRING(key.AfterFirst(_T('/'))), LSL::Util::FromString<int>(value));
			} else if (key.Left(4) == _T( "team" ) && key.Find(_T("startpos")) != wxNOT_FOUND) {
				int team = FromwxString(key.BeforeFirst(_T('/')).Mid(4));
				if (key.Find(_T("startposx")) != wxNOT_FOUND) {
					int numusers = battle.GetNumUsers();
					for (int i = 0; i < numusers; i++) {
						User& usr = battle.GetUser(i);
						UserBattleStatus& status = usr.BattleStatus();
						if (status.team == team) {
							status.pos.x = LSL::Util::FromString<int>(value);
							battle.OnUserBattleStatusUpdated(usr, status);
						}
					}
				} else if (key.Find(_T("startposy")) != wxNOT_FOUND) {
					int numusers = battle.GetNumUsers();
					for (int i = 0; i < numusers; i++) {
						User& usr = battle.GetUser(i);
						UserBattleStatus& status = usr.BattleStatus();
						if (status.team == team) {
							status.pos.y = LSL::Util::FromString<int>(value);
							battle.OnUserBattleStatusUpdated(usr, status);
						}
					}
				}
			} else if (key.Left(8) == _T("hosttype")) {
				battle.m_autohost_manager->RecognizeAutohost(value);
			} else {
				battle.CustomBattleOptions().setSingleOption(STD_STRING(key), value, LSL::Enum::EngineOption);
				battle.Update(stdprintf("%d_%s", LSL::Enum::EngineOption, STD_STRING(key).c_str()));
			}
		}
	} catch (assert_exception) {
	}
}

void ServerEvents::OnUnsetBattleInfo(int /*battleid*/, const std::string& /*param*/)
{
	//FIXME: implement this
}


void ServerEvents::OnBattleInfoUpdated(int battleid)
{
	slLogDebugFunc("");
	try {
		IBattle& battle = m_serv.GetBattle(battleid);
		BattleEvents::GetBattleEventSender(BattleEvents::BattleInfoUpdate).SendEvent(std::make_pair(&battle, ""));
	} catch (assert_exception) {
	}
}


void ServerEvents::OnBattleClosed(int battleid)
{
	slLogDebugFunc("");
	try {
		IBattle& battle = m_serv.GetBattle(battleid);

		ui().OnBattleClosed(battle);

		m_serv._RemoveBattle(battleid);
	} catch (assert_exception) {
	}
}


void ServerEvents::OnBattleDisableUnit(int battleid, const std::string& unitname, int count)
{
	slLogDebugFunc("");
	try {
		IBattle& battle = m_serv.GetBattle(battleid);
		battle.RestrictUnit(unitname, count);
		battle.Update(stdprintf("%d_restrictions", LSL::Enum::PrivateOptions));
	} catch (assert_exception) {
	}
}


void ServerEvents::OnBattleEnableUnit(int battleid, const std::string& unitname)
{
	slLogDebugFunc("");
	try {
		IBattle& battle = m_serv.GetBattle(battleid);
		battle.UnrestrictUnit(unitname);
		battle.Update(stdprintf("%d_restrictions", LSL::Enum::PrivateOptions));
	} catch (assert_exception) {
	}
}


void ServerEvents::OnBattleEnableAllUnits(int battleid)
{
	slLogDebugFunc("");
	try {
		IBattle& battle = m_serv.GetBattle(battleid);
		battle.UnrestrictAllUnits();
		battle.Update(stdprintf("%d_restrictions", LSL::Enum::PrivateOptions));
	} catch (assert_exception) {
	}
}


void ServerEvents::OnJoinChannelResult(bool success, const std::string& channel, const std::string& reason)
{
	slLogDebugFunc("");
	if (success) {
		Channel& chan = m_serv._AddChannel(channel);
		chan.SetPassword(m_serv.m_channel_pw[channel]);
		ui().OnJoinedChannelSuccessful(chan);

	} else {
		ui().ShowMessage(_("Join channel failed"), _("Could not join channel ") + TowxString(channel) + _(" because: ") + TowxString(reason));
	}
}


void ServerEvents::OnChannelSaid(const std::string& channel, const std::string& who, const std::string& message)
{
	slLogDebugFunc("");
	try {
		if ((m_serv.GetMe().GetNick() == who) || !useractions().DoActionOnUser(UserActions::ActIgnoreChat, TowxString(who))) {
			if (m_serv.UserExists(who)) {
				m_serv.GetChannel(channel).Said(m_serv.GetUser(who), message);
			} else {
				User u(who);
				m_serv.GetChannel(channel).Said(u, message); // offline message
			}
		}
	} catch (std::runtime_error& except) {
	}
}


void ServerEvents::OnChannelJoin(const std::string& channel, const std::string& who)
{
	slLogDebugFunc("");
	try {
		m_serv.GetChannel(channel).OnChannelJoin(m_serv.GetUser(who));
	} catch (std::runtime_error& except) {
	}
}


void ServerEvents::OnChannelPart(const std::string& channel, const std::string& who, const std::string& message)
{
	slLogDebugFunc("");
	try {
		m_serv.GetChannel(channel).Left(m_serv.GetUser(who), message);
	} catch (std::runtime_error& except) {
	}
}


void ServerEvents::OnChannelTopic(const std::string& channel, const std::string& who, const std::string& message, int /*unused*/)
{
	slLogDebugFunc("");
	try {
		m_serv.GetChannel(channel).SetTopic(message, who);
	} catch (std::runtime_error& except) {
	}
}


void ServerEvents::OnChannelAction(const std::string& channel, const std::string& who, const std::string& action)
{
	slLogDebugFunc("");
	try {
		if ((m_serv.GetMe().GetNick() == who) || !useractions().DoActionOnUser(UserActions::ActIgnoreChat, TowxString(who)))
			m_serv.GetChannel(channel).DidAction(m_serv.GetUser(who), action);
	} catch (std::runtime_error& except) {
	}
}


void ServerEvents::OnPrivateMessage(const std::string& user, const std::string& message, bool fromme)
{
	slLogDebugFunc("");
	try {
		User& who = m_serv.GetUser(user);
		if (!useractions().DoActionOnUser(UserActions::ActIgnorePM, TowxString(who.GetNick())))
			ui().OnUserSaid(who, TowxString(message), fromme);
	} catch (std::runtime_error& except) {
	}
}

void ServerEvents::OnPrivateMessageEx(const std::string& user, const std::string& action, bool fromme)
{
	slLogDebugFunc("");
	try {
		User& who = m_serv.GetUser(user);
		if (!useractions().DoActionOnUser(UserActions::ActIgnorePM, TowxString(who.GetNick())))
			ui().OnUserSaidEx(who, TowxString(action), fromme);
	} catch (std::runtime_error& except) {
	}
}

void ServerEvents::OnChannelList(const std::string& channel, const int& numusers, const std::string& topic)
{
	ui().mw().OnChannelList(TowxString(channel), numusers, TowxString(topic));
}


void ServerEvents::OnUserJoinChannel(const std::string& channel, const std::string& who)
{
	slLogDebugFunc("");
	try {
		m_serv.GetChannel(channel).Joined(m_serv.GetUser(who));
	} catch (std::runtime_error& except) {
	}
}


void ServerEvents::OnRequestBattleStatus(int battleid)
{
	try {
		IBattle& battle = m_serv.GetBattle(battleid);
		ui().OnRequestBattleStatus(battle);
	} catch (assert_exception) {
	}
}


void ServerEvents::OnSaidBattle(int battleid, const std::string& nick, const std::string& msg)
{
	try {
		IBattle& battle = m_serv.GetBattle(battleid);
		if ((m_serv.GetMe().GetNick() == nick) || !useractions().DoActionOnUser(UserActions::ActIgnoreChat, TowxString(nick))) {
			ui().OnSaidBattle(battle, TowxString(nick), TowxString(msg));
		}
		AutoHost* ah = battle.GetAutoHost();
		if (ah != NULL) {
			ah->OnSaidBattle(TowxString(nick), TowxString(msg));
		}
	} catch (assert_exception) {
	}
}

void ServerEvents::OnBattleAction(int /*battleid*/, const std::string& nick, const std::string& msg)
{
	try {
		UiEvents::GetUiEventSender(UiEvents::OnBattleActionEvent).SendEvent(
		    UiEvents::OnBattleActionData(TowxString(nick), TowxString(msg)));
	} catch (assert_exception) {
	}
}


void ServerEvents::OnBattleStartRectAdd(int battleid, int allyno, int left, int top, int right, int bottom)
{
	try {
		IBattle& battle = m_serv.GetBattle(battleid);
		battle.AddStartRect(allyno, left, top, right, bottom);
		battle.StartRectAdded(allyno);
		battle.Update(stdprintf("%d_mapname", LSL::Enum::PrivateOptions));
	} catch (assert_exception) {
	}
}


void ServerEvents::OnBattleStartRectRemove(int battleid, int allyno)
{
	try {
		IBattle& battle = m_serv.GetBattle(battleid);
		battle.RemoveStartRect(allyno);
		battle.StartRectRemoved(allyno);
		battle.Update(stdprintf("%d_mapname", LSL::Enum::PrivateOptions));
	} catch (assert_exception) {
	}
}


void ServerEvents::OnBattleAddBot(int battleid, const std::string& nick, UserBattleStatus status)
{
	slLogDebugFunc("");
	try {
		IBattle& battle = m_serv.GetBattle(battleid);
		battle.OnBotAdded(nick, status);
		User& bot = battle.GetUser(nick);
		ASSERT_LOGIC(&bot != 0, "Bot null after add.");
		ui().OnUserJoinedBattle(battle, bot);
	} catch (assert_exception) {
	}
}

void ServerEvents::OnBattleUpdateBot(int battleid, const std::string& nick, UserBattleStatus status)
{
	OnClientBattleStatus(battleid, nick, status);
}


void ServerEvents::OnBattleRemoveBot(int battleid, const std::string& nick)
{
	slLogDebugFunc("");
	try {
		IBattle& battle = m_serv.GetBattle(battleid);
		User& user = battle.GetUser(nick);
		bool isbot = user.BattleStatus().IsBot();
		ui().OnUserLeftBattle(battle, user, isbot);
		battle.OnUserRemoved(user);
	} catch (std::runtime_error& except) {
	}
}


void ServerEvents::OnAcceptAgreement(const std::string& agreement)
{
	ui().OnAcceptAgreement(TowxString(agreement));
}


void ServerEvents::OnRing(const std::string& from)
{
	ui().OnRing(TowxString(from));
}

void ServerEvents::OnServerBroadcast(const std::string& message)
{
	ui().OnServerBroadcast(m_serv, TowxString(message));
}

void ServerEvents::OnServerMessage(const std::string& message)
{
	ui().OnServerMessage(m_serv, TowxString(message));
}


void ServerEvents::OnServerMessageBox(const std::string& message)
{
	ui().ShowMessage(_("Server Message"), TowxString(message));
}


void ServerEvents::OnChannelMessage(const std::string& channel, const std::string& msg)
{
	ui().OnChannelMessage(m_serv.GetChannel(channel), msg);
}


void ServerEvents::OnHostExternalUdpPort(const unsigned int udpport)
{
	if (!m_serv.GetCurrentBattle())
		return;
	if (m_serv.GetCurrentBattle()->GetNatType() == NAT_Hole_punching || m_serv.GetCurrentBattle()->GetNatType() == NAT_Fixed_source_ports)
		m_serv.GetCurrentBattle()->SetHostPort(udpport);
}


void ServerEvents::OnMyInternalUdpSourcePort(const unsigned int udpport)
{
	if (!m_serv.GetCurrentBattle())
		return;
	m_serv.GetCurrentBattle()->SetMyInternalUdpSourcePort(udpport);
}


void ServerEvents::OnMyExternalUdpSourcePort(const unsigned int udpport)
{
	if (!m_serv.GetCurrentBattle())
		return;
	m_serv.GetCurrentBattle()->SetMyExternalUdpSourcePort(udpport);
}

void ServerEvents::OnClientIPPort(const std::string& username, const std::string& ip, unsigned int udpport)
{
	wxLogMessage(_T("OnClientIPPort(%s,%s,%d)"), username.c_str(), ip.c_str(), udpport);
	if (!m_serv.GetCurrentBattle()) {
		wxLogMessage(_T("GetCurrentBattle() returned null"));
		return;
	}
	try {
		User& user = m_serv.GetCurrentBattle()->GetUser(username);

		user.BattleStatus().ip = ip;
		user.BattleStatus().udpport = udpport;
		wxLogMessage(_T("set to %s %d "), user.BattleStatus().ip.c_str(), user.BattleStatus().udpport);

		if (sett().GetShowIPAddresses()) {
			UiEvents::GetUiEventSender(UiEvents::OnBattleActionEvent).SendEvent(
			    UiEvents::OnBattleActionData(TowxString(username), wxString::Format(_(" has ip=%s"), ip.c_str())));
		}

		if (m_serv.GetCurrentBattle()->GetNatType() != NAT_None && (udpport == 0)) {
			// todo: better warning message
			//something.OutputLine( _T(" ** ") + who.GetNick() + _(" does not support nat traversal! ") + GetChatTypeStr() + _T("."), sett().GetChatColorJoinPart(), sett().GetChatFont() );
			UiEvents::GetUiEventSender(UiEvents::OnBattleActionEvent).SendEvent(
			    UiEvents::OnBattleActionData(TowxString(username), _(" does not really support nat traversal")));
		}
		m_serv.GetCurrentBattle()->CheckBan(user);
	} catch (std::runtime_error) {
		wxLogMessage(_T("runtime_error inside OnClientIPPort()"));
	}
}


void ServerEvents::OnKickedFromBattle()
{
	customMessageBoxNoModal(SL_MAIN_ICON, _("You were kicked from the battle!"), _("Kicked by Host"));
}


void ServerEvents::OnRedirect(const std::string& address, unsigned int port, const std::string& CurrentNick, const std::string& CurrentPassword)
{
	const wxString name = TowxString(address) + _T(":") + TowxString(port);
	sett().SetServer(name, TowxString(address), port);
	ui().DoConnect(name, TowxString(CurrentNick), TowxString(CurrentPassword));
}


void ServerEvents::AutoCheckCommandSpam(IBattle& battle, User& user)
{
	const std::string nick = user.GetNick();
	MessageSpamCheck info = m_spam_check[nick];
	time_t now = time(0);
	if (info.lastmessage == now)
		info.count++;
	else
		info.count = 0;
	info.lastmessage = now;
	m_spam_check[nick] = info;
	if (info.count == 7) {
		battle.DoAction("is autokicking " + nick + " due to command spam.");
		battle.KickPlayer(user);
	}
}

void ServerEvents::OnMutelistBegin(const std::string& channel)
{
	mutelistWindow(_("Begin mutelist for ") + TowxString(channel), wxString::Format(_("%s mutelist"), channel.c_str()));
}

void ServerEvents::OnMutelistItem(const std::string& /*unused*/, const std::string& mutee, const std::string& description)
{
	wxString message = TowxString(mutee);
	wxString desc = TowxString(description);
	wxString mutetime = TowxString(GetWordParam(desc));
	long time;
	if (mutetime == _T("indefinite"))
		message << _(" indefinite time remaining");
	else if (mutetime.ToLong(&time))
		message << wxString::Format(_(" %d minutes remaining"), time / 60 + 1);
	else
		message << mutetime;
	if (!desc.IsEmpty())
		message << _T(", ") << desc;
	mutelistWindow(message);
}

void ServerEvents::OnMutelistEnd(const std::string& channel)
{
	mutelistWindow(_("End mutelist for ") + TowxString(channel));
}

void ServerEvents::OnScriptStart(int battleid)
{
	if (!m_serv.BattleExists(battleid)) {
		return;
	}
	m_serv.GetBattle(battleid).ClearScript();
}

void ServerEvents::OnScriptLine(int battleid, const std::string& line)
{
	if (!m_serv.BattleExists(battleid)) {
		return;
	}
	m_serv.GetBattle(battleid).AppendScriptLine(line);
}

void ServerEvents::OnScriptEnd(int battleid)
{
	if (!m_serv.BattleExists(battleid)) {
		return;
	}
	m_serv.GetBattle(battleid).GetBattleFromScript(true);
}

void ServerEvents::OnForceJoinBattle(int battleid, const std::string& scriptPW)
{
	IBattle* battle = m_serv.GetCurrentBattle();
	if (battle != NULL) {
		m_serv.LeaveBattle(battle->GetID());
	}
	m_serv.JoinBattle(battleid, scriptPW);
	UiEvents::GetStatusEventSender(UiEvents::addStatusMessage).SendEvent(
	    UiEvents::StatusData(_("Automatically moved to new battle"), 1));
}

void ServerEvents::RegistrationAccepted(const std::string& user, const std::string& pass)
{
	ui().OnRegistrationAccepted(TowxString(user), TowxString(pass));
}

void ServerEvents::RegistrationDenied(const std::string& reason)
{
	ui().OnRegistrationDenied(TowxString(reason));
}

void ServerEvents::OnLoginDenied(const std::string& reason)
{
	ui().OnLoginDenied(reason);
}
