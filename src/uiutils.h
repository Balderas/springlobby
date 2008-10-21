/// \file uiutils.h

#ifndef SPRINGLOBBY_HEADERGUARD_UIUTILS_H
#define SPRINGLOBBY_HEADERGUARD_UIUTILS_H

#include <wx/intl.h>

class wxColour;
class wxImage;
class wxBitmap;
class wxWindow;

#define bool2yn(b) ((b)?_("Yes"):_("No"))

#define wxDefaultBitmap wxBitmap()

#ifdef __WXMSW__
    const wxString DEFAULT_COLORDLG_TITLE = _("Choose color");
#else
    const wxString DEFAULT_COLORDLG_TITLE = _("Choose color (only first 16 will be saved)");
#endif


const int colour_values[][3] = { {240,210,0}, {128, 128, 128}, {0, 0, 128}, {0, 0, 255},
      {0, 128, 0}, {0, 255, 0}, {0, 128, 128}, {0, 255, 255}, {128, 0, 0}, {255, 0, 0},
      {128, 0, 128}, {255, 0, 255}, {128, 128, 0}, {255, 255, 0}, {192, 192, 192}, {0, 220, 250}
};

wxString RefineMapname( const wxString& mapname );
wxString RefineModname( const wxString& modname );
wxString RTFtoText( const wxString& rtfinput );
bool AreColoursSimilar( const wxColour& col1, const wxColour& col2, int mindiff = 10 );

void ColourDelta( int& r, int& g, int& b, const int& delta );
wxColour ColourDelta( const wxColour& colour, const int& delta );

wxString GetColorString( const wxColour& color );
wxColour GetColorFromStrng( const wxString color );

//! apply standard alpha blending to images
wxImage BlendImage( const wxImage& foreground, const wxImage& background );
wxBitmap* BlendBitmaps( const wxBitmap& background, const wxBitmap& overlay, const int dim = 16 );
//! used to load png data into a wxBitmap
wxBitmap* charArr2wxBitmap(const unsigned char * arg, int size);
//wxBitmap charArr2wxBitmap(const unsigned char * arg, int size);
wxBitmap* charArr2wxBitmapWithBlending(const unsigned char * arg, int size,
            const unsigned char * text, int text_size);


//! when querying for a color, always use this (it'll autosave/retrieve custom defined colors)
wxColour GetColourFromUser(wxWindow *parent, const wxColour& colInit,
        const wxString& caption = DEFAULT_COLORDLG_TITLE, const wxString& palette = _T("Default") );

wxImage ReplaceChannelStatusColour( wxBitmap img, const wxColour& colour );


#if wxUSE_TIPWINDOW
#include <wx/tipwin.h>

class SLTipWindow : public wxTipWindow{
    public:
        SLTipWindow(wxWindow *parent, const wxString &text)
            :wxTipWindow(parent,text){};
        void Cancel(wxMouseEvent& event);

        DECLARE_EVENT_TABLE()
};
#endif

#endif
// SPRINGLOBBY_HEADERGUARD_UIUTILS_H


