#ifndef SPRINGLOBBY_HEADERGUARD_USER_H
#define SPRINGLOBBY_HEADERGUARD_USER_H

#include <wx/string.h>
#include <wx/colour.h>

class Server;

const unsigned int SYNC_UNKNOWN = 0;
const unsigned int SYNC_SYNCED = 1;
const unsigned int SYNC_UNSYNCED = 2;

//! @brief Struct used to store a client's status.
struct UserStatus
{
    enum RankContainer
    {
      RANK_1,
      RANK_2,
      RANK_3,
      RANK_4,
      RANK_5,
      RANK_6,
      RANK_7
    };

  bool in_game;
  bool away;
  RankContainer rank;
  bool moderator;
  bool bot;
  UserStatus(): in_game(false), away(false), rank(RANK_1), moderator(false), bot(false) {}
  wxString GetDiffString ( const UserStatus& other );
};

struct UserBattleStatus
{
  // when adding something to this struct, also modify User::UpdateBattleStatus()
  // total 16 members here
  int team;
  int ally;
  wxColour colour;
  int color_index;
  int handicap;
  int side;
  int sync;
  bool spectator;
  bool ready;
	int posx; // for startpos = 4
	int posy; // for startpos = 4
	// bot-only stuff
	wxString owner;
	wxString aishortname;
	wxString aiversion;
  // for nat holepunching
  wxString ip;
  unsigned int udpport;
  bool IsBot() { return !aishortname.IsEmpty(); }
  UserBattleStatus(): team(0),ally(0),colour(wxColour(0,0,0)),color_index(-1),handicap(0),side(0),sync(SYNC_UNKNOWN),spectator(false),ready(false), posx(-1), posy(-1), udpport(0) {}
  bool operator == ( const UserBattleStatus& s )
  {
    return ( ( team == s.team ) && ( colour == s.colour ) && ( handicap == s.handicap ) && ( side == s.side ) && ( sync == s.sync ) && ( spectator == s.spectator ) && ( ready == s.ready ) && ( owner == s.owner ) && ( aishortname == s.aishortname ) );
  }
  bool operator != ( const UserBattleStatus& s )
  {
    return ( ( team != s.team ) || ( colour != s.colour ) || ( handicap != s.handicap ) || ( side != s.side ) || ( sync != s.sync ) || ( spectator != s.spectator ) || ( ready != s.ready ) || ( owner != s.owner ) || ( aishortname != s.aishortname ) );
  }
};

class ChatPanel;
class Battle;

struct UiUserData {
  UiUserData(): panel(0) {}
  ChatPanel* panel;
};

//! parent class leaving out server related functionality
class CommonUser
{
    public:
        CommonUser(const wxString& nick, const wxString& country, const int& cpu)
           : m_nick(nick), m_country(country), m_cpu(cpu)  {};

        wxString GetNick() const { return m_nick; }
        virtual void SetNick( const wxString& nick ) { m_nick = nick; }

        wxString GetCountry() const { return m_country; }
        virtual void SetCountry( const wxString& country ) { m_country = country; }

        int GetCpu() const { return m_cpu; }
        void SetCpu( const int& cpu ) { m_cpu = cpu; }

        UserStatus& Status() { return m_status; }

        UserStatus GetStatus() const { return m_status; }
        virtual void SetStatus( const UserStatus& status );

        UserBattleStatus& BattleStatus() { return m_bstatus; }
		UserBattleStatus GetBattleStatus() const { return m_bstatus; }
        //void SetBattleStatus( const UserBattleStatus& status );/// dont use this to avoid overwriting data like ip and port, use following method.
        void UpdateBattleStatus( const UserBattleStatus& status );

    /*    void SetUserData( void* userdata ) { m_data = userdata; }
        void* GetUserData() { return m_data; }*/

        bool Equals( const CommonUser& other ) const { return ( m_nick == other.GetNick() ); }


    protected:
        wxString m_nick;
        wxString m_country;
        int m_cpu;
        UserStatus m_status;
        UserBattleStatus m_bstatus;

        //void* m_data;

};

//! Class containing all the information about a user
class User : public CommonUser
{
  public:

    mutable UiUserData uidata;

    User( Server& serv );
    User( const wxString& nick, Server& serv );
    User( const wxString& nick, const wxString& country, const int& cpu, Server& serv);
    User( const wxString& nick );
    User( const wxString& nick, const wxString& country, const int& cpu );
    User();

    virtual ~User();

    // User interface

    Server& GetServer() const { return *m_serv; }

    void Said( const wxString& message ) const;
    void Say( const wxString& message ) const;
    void DoAction( const wxString& message ) const;

    Battle* GetBattle() const;
    void SetBattle( Battle* battle );

    void SendMyUserStatus() const;
    void SetStatus( const UserStatus& status );
    void SetCountry( const wxString& country );

    bool ExecuteSayCommand( const wxString& cmd ) const;

    static wxString GetRankName(UserStatus::RankContainer rank);

    float GetBalanceRank();
    UserStatus::RankContainer GetRank();
    wxString GetClan();

    int GetFlagIconIndex() const { return m_flagicon_idx; }
    int GetRankIconIndex() const { return m_rankicon_idx; }
    int GetStatusIconIndex() const { return m_statusicon_idx; }
    //bool operator< ( const User& other ) const { return m_nick < other.GetNick() ; }
    //User& operator= ( const User& other );

  protected:
    // User variables

    Server* m_serv;
    Battle* m_battle;
    int m_flagicon_idx;
    int m_rankicon_idx;
    int m_statusicon_idx;
};

#endif // SPRINGLOBBY_HEADERGUARD_USER_H
