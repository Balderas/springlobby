/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef OFFLINEBATTLE_H_INCLUDED
#define OFFLINEBATTLE_H_INCLUDED

#include "ibattle.h"

class OfflineBattle : public IBattle
{
	public:
			OfflineBattle ( const int id );
			OfflineBattle ( );
			OfflineBattle ( const OfflineBattle&  );
			OfflineBattle& operator = ( const OfflineBattle&  );
			~OfflineBattle (){}
			User& GetMe() { return m_me; }
            const User& GetMe() const { return m_me; }
			bool IsFounderMe() const { return true; }
			void StartSpring();

	protected:
			int m_id;
			User m_me;
};

#endif // OFFLINEBATTLE_H_INCLUDED
