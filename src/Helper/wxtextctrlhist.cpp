// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wxtextctrlhist.h"
#include "TextCompletionDatabase.hpp"
#include <wx/regex.h>
#include "../utils.h"

BEGIN_EVENT_TABLE(wxTextCtrlHist, wxTextCtrl)
    EVT_TEXT_ENTER(wxID_ANY, wxTextCtrlHist::OnSendMessage)
    EVT_KEY_DOWN(wxTextCtrlHist::OnChar)
END_EVENT_TABLE()


wxTextCtrlHist::wxTextCtrlHist(TextCompletionDatabase& textDb, wxWindow* parent, wxWindowID id, const wxString& value, const wxPoint& pos, const wxSize& size, long style )
    : wxTextCtrl(parent, id, value, pos, size, wxTE_PROCESS_ENTER | wxTE_PROCESS_TAB ),
    textcompletiondatabase(textDb), current_pos(0), history_max(32)
{
}

void wxTextCtrlHist::OnSendMessage( wxCommandEvent &event )
{

  Historical.Add(GetLineText(0));
        current_pos = Historical.GetCount();
        if(current_pos > history_max)
        {
                Historical.RemoveAt(0);
                --current_pos;
        }
  event.Skip();
}

void wxTextCtrlHist::OnChar(wxKeyEvent & event)
{
        int keyCode = event.GetKeyCode();

        if(keyCode == WXK_UP)
        {
            if(current_pos > 0)
            {
                    --current_pos;
                    SetValue(Historical[current_pos]);
                    SetInsertionPointEnd();
            }
            event.Skip();
        }
        else
        if(keyCode == WXK_DOWN)
        {
            if(current_pos < static_cast<int>(Historical.GetCount())-1)
            {
                ++current_pos;
                SetValue(Historical[current_pos]);
            }
            else
            {
                current_pos = Historical.GetCount();
                SetValue(wxT(""));
                SetInsertionPointEnd();
            }
            event.Skip();
        }
        else
        if(keyCode == WXK_TAB)
        {
         #ifndef HAVE_WX26
            wxString text = this->GetValue();
            long pos_Cursor = this->GetInsertionPoint();
            wxString selection_Begin_InsertPos = this->GetRange( 0, pos_Cursor );
            wxString selection_InsertPos_End = this->GetRange( pos_Cursor, this->GetLastPosition() );

            // Search for the shortest Match, starting from the Insertionpoint to the left, until we find a "\ "
            // Special Characters according to regular Expression Syntax needs to be escaped: [,]
            wxRegEx regex_currentWord;
            #ifdef wxHAS_REGEX_ADVANCED
            regex_currentWord.Compile( wxT("(_|\\[|\\]|\\w)+$"), wxRE_ADVANCED );
            #else
            regex_currentWord.Compile( wxT("(_|\\[|\\]|\\w)+$"), wxRE_EXTENDED );
            #endif

            if ( regex_currentWord.Matches( selection_Begin_InsertPos ) ) {
                wxString currentWord = regex_currentWord.GetMatch( selection_Begin_InsertPos );
                // std::cout << "#########: Current Word: (" << currentWord.char_str() << ")" << std::endl;

                wxString selection_Begin_BeforeCurrentWord = this->GetRange( 0, pos_Cursor - currentWord.length() );
                // std::cout << "#########: selection_Begin_BeforeCurrentWord: (" << selection_Begin_BeforeCurrentWord.char_str() << ")" << std::endl;

                HashMap_String_String hm = textcompletiondatabase.GetMapping( currentWord );

                // std::cout << "#########: Mapping-Size: (" << hm.size() << ")" << std::endl;

                wxString completed_Text;
                int new_Cursor_Pos = 0;
                if( hm.size() == 1 ) {
                    completed_Text.append( selection_Begin_BeforeCurrentWord );
                    completed_Text.append( hm.begin()->second );
                    completed_Text.append( selection_InsertPos_End );
                    new_Cursor_Pos = selection_Begin_BeforeCurrentWord.length() + hm.begin()->second.length();
                } else {
                    completed_Text.append( selection_Begin_BeforeCurrentWord );
                    completed_Text.append( currentWord );
                    completed_Text.append( selection_InsertPos_End );
                    new_Cursor_Pos = selection_Begin_BeforeCurrentWord.length() + currentWord.length();
                    // We ring the System Bell, to signalise the User, that no Completion was applied.
                    wxBell();
                }
                // Replace the old Text with our completed Text
                // or
                // if nothing was found remove the typed TAB, so that the User stays comfortable not to remove the TAB by himself.
                this->ChangeValue( completed_Text );
                this->SetInsertionPoint( new_Cursor_Pos );
            } else {
                wxString old_Text;
                old_Text.append( selection_Begin_InsertPos );
                old_Text.append( selection_InsertPos_End );
                this->ChangeValue( old_Text );
                this->SetInsertionPoint( selection_Begin_InsertPos.length() );
                wxBell();
            }
            #endif
        }
        else
            event.Skip();

}
