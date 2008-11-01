#ifndef SPRINGLOBBY_HEADERGUARD_FLAGIMAGES_H
#define SPRINGLOBBY_HEADERGUARD_FLAGIMAGES_H

class wxImageList;
class wxString;

int GetFlagIndex( const wxString& flag );
int AddFlagImages( wxImageList& imgs );

enum {
  FLAG_NONE = -1
};

#endif // SPRINGLOBBY_HEADERGUARD_FLAGIMAGES_H
