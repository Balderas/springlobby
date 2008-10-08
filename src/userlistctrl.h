#ifndef SPRINGLOBBY_HEADERGUARD_USERLISTCTRL_H
#define SPRINGLOBBY_HEADERGUARD_USERLISTCTRL_H

#include "customlistctrl.h"
#include "tasserver.h"


//! a rather sneaky approach to resue NickList for both off/online users
/**
    Don't ever use this for anything that needs constant updating, context menus sorting and the like.\n
    Basically don't use it outside the groupuserdialog without extreme caution.
**/
class UserListctrl : public CustomListCtrl
{
    public:
        //! nickname - country
        typedef std::pair<wxString, wxString> UserData;
        typedef std::map<unsigned int, UserData> UserDataMap;
        typedef UserDataMap::const_iterator UserDataMapConstIter;
        typedef UserDataMap::iterator UserDataMapIter;

    public:
        UserListctrl( wxWindow* parent, const wxString& name = _T("usergrouplist"), bool highlight = false,wxWindowID id=USERLIST  );
        virtual ~UserListctrl();

        void AddUser( const UserData userdata );
        void AddUser( const UserDataMap& userdata );
        const UserDataMap& GetUserData() const;
        void RemoveUsers( const UserDataMap& userdata );
        UserDataMap GetSelectedUserData() const;
        wxArrayString GetUserNicks( ) const;
        void SetColumnWidths();
        void OnColClick( wxListEvent& event );
        //! delete both all items and associated data, handle with care!
        void Clear();

    protected:
        UserDataMap m_userdata;

        //! no-op atm, so i don't get segfault because of missing data
        virtual void Sort();
        virtual void SetTipWindowText( const long item_hit, const wxPoint position);

        void HighlightItem( long item );

        bool IsInList( const UserData userdata );
        UserDataMapIter FindData( const UserData userdata );

        static int wxCALLBACK ComparePlayernameUP(long item1, long item2, long sortData);
        static int wxCALLBACK ComparePlayernameDOWN(long item1, long item2, long sortData);
        static int wxCALLBACK ComparePlayercountryUP(long item1, long item2, long sortData);
        static int wxCALLBACK ComparePlayercountryDOWN(long item1, long item2, long sortData);

        enum {
            USERLIST = 2312
        };

        struct {
          int col;
          bool direction;
        } m_sortorder[2];

        DECLARE_EVENT_TABLE()

};

#endif // SPRINGLOBBY_HEADERGUARD_USERLISTCTRL_H
