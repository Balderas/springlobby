/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */
//
// Class: MainChatTab
//

#include <stdexcept>
#include <wx/intl.h>
#include <wx/imaglist.h>
#include <wx/sizer.h>
#include <wx/notebook.h>
#include <wx/image.h>

#ifndef HAVE_WX26
#include "auimanager.h"
#else
#include <wx/listbook.h>
#endif

#include "mainchattab.h"
#include "utils.h"
#include "mainwindow.h"
#include "channel.h"
#include "user.h"
#include "chatpanel.h"
#include "ui.h"
#include "server.h"
#include "settings.h"

#include "images/close.xpm"
#include "images/server.xpm"
#include "images/channel.xpm"
#include "images/userchat.xpm"

BEGIN_EVENT_TABLE(MainChatTab, wxPanel)

  #ifdef HAVE_WX26
  EVT_NOTEBOOK_PAGE_CHANGED( CHAT_TABS, MainChatTab::OnTabsChanged )
  #else
  EVT_AUINOTEBOOK_PAGE_CHANGED( CHAT_TABS, MainChatTab::OnTabsChanged )
  EVT_AUINOTEBOOK_PAGE_CLOSE ( CHAT_TABS, MainChatTab::OnTabClose )
  #endif
END_EVENT_TABLE()


MainChatTab::MainChatTab( wxWindow* parent, Ui& ui )
: wxScrolledWindow( parent, -1, wxDefaultPosition, wxDefaultSize, 0, wxPanelNameStr ),m_ui(ui)
{

  #ifndef HAVE_WX26
  GetAui().manager->AddPane( this, wxLEFT, _T("mainchattab") );
  #endif

  m_newtab_sel = -1;
  m_server_chat = 0;

  m_main_sizer = new wxBoxSizer( wxVERTICAL );

  #ifdef HAVE_WX26
  m_chat_tabs = new wxNotebook( this, CHAT_TABS, wxDefaultPosition, wxDefaultSize, wxLB_TOP );
  #else
  m_chat_tabs = new wxAuiNotebook( this, CHAT_TABS, wxDefaultPosition, wxDefaultSize, wxAUI_NB_DEFAULT_STYLE | wxAUI_NB_TOP | wxAUI_NB_TAB_EXTERNAL_MOVE );
  #endif

  wxBitmap userchat (userchat_xpm); //*charArr2wxBitmap(userchat_png, sizeof(userchat_png) );
  m_imagelist = new wxImageList( 12, 12 );
  m_imagelist->Add( wxBitmap(close_xpm) );
  m_imagelist->Add( wxBitmap(server_xpm) );
  m_imagelist->Add( wxBitmap(channel_xpm) );
  m_imagelist->Add( wxBitmap(userchat_xpm) );

  m_imagelist->Add( wxBitmap ( ReplaceChannelStatusColour( wxBitmap( channel_xpm ), sett().GetChatColorJoinPart() ) ) );
  m_imagelist->Add( wxBitmap ( ReplaceChannelStatusColour( wxBitmap(userchat_xpm ), sett().GetChatColorJoinPart() ) ) );

  m_imagelist->Add( wxBitmap ( ReplaceChannelStatusColour( wxBitmap( channel_xpm ), sett().GetChatColorMine() ) ) );
  m_imagelist->Add( wxBitmap ( ReplaceChannelStatusColour( wxBitmap( userchat_xpm ), sett().GetChatColorMine() ) ) );

  m_imagelist->Add( wxBitmap ( ReplaceChannelStatusColour( wxBitmap( channel_xpm ), sett().GetChatColorHighlight() ) ) );
  m_imagelist->Add( wxBitmap ( ReplaceChannelStatusColour( wxBitmap( userchat_xpm ), sett().GetChatColorHighlight() ) ) );

  m_imagelist->Add( wxBitmap ( ReplaceChannelStatusColour( wxBitmap( server_xpm ), sett().GetChatColorError() ) ) );

  #ifdef HAVE_WX26
  m_chat_tabs->AssignImageList( m_imagelist );
  #endif

//  m_server_chat = new ChatPanel( m_chat_tabs, serv );
//  m_chat_tabs->AddPage( m_server_chat, _("Server"), true, 1 );

  m_close_window = new wxWindow( m_chat_tabs, -1 );
  #ifdef HAVE_WX26
  m_chat_tabs->AddPage( m_close_window, _T(""), false, 0 );
  #endif

  m_main_sizer->Add( m_chat_tabs, 1, wxEXPAND );

  SetSizer( m_main_sizer );
  m_main_sizer->SetSizeHints( this );
  SetScrollRate( 3, 3 );
  Layout();
}


MainChatTab::~MainChatTab()
{

}

ChatPanel& MainChatTab::ServerChat()
{
  ASSERT_LOGIC( m_server_chat != 0, _T("m_server_chat = 0") );
  return *m_server_chat;
}


ChatPanel* MainChatTab::GetActiveChatPanel()
{
  #ifdef HAVE_WX26
  return (ChatPanel*)m_chat_tabs->GetCurrentPage();
  #else
  return (ChatPanel*)m_chat_tabs->GetPage(m_chat_tabs->GetSelection());
  #endif
}


ChatPanel* MainChatTab::GetChannelChatPanel( const wxString& channel )
{
  for ( unsigned int i = 0; i < m_chat_tabs->GetPageCount(); i++ ) {
    ChatPanel* tmp = (ChatPanel*)m_chat_tabs->GetPage(i);
    if ( tmp->GetPanelType() == CPT_Channel ) {
      wxString name = m_chat_tabs->GetPageText(i);
      if ( name.Lower() == channel.Lower() ) return (ChatPanel*)m_chat_tabs->GetPage(i);
    }
  }
  return 0;
}

void MainChatTab::UpdateNicklistHighlights()
{
    for ( unsigned int i = 0; i < m_chat_tabs->GetPageCount(); i++ ) {
    ChatPanel* tmp = (ChatPanel*)m_chat_tabs->GetPage(i);
    if ( tmp->GetPanelType() == CPT_Channel ) {
      tmp->UpdateNicklistHighlights();
    }
  }
}

ChatPanel* MainChatTab::GetUserChatPanel( const wxString& user )
{
  for ( unsigned int i = 0; i < m_chat_tabs->GetPageCount(); i++ ) {
    ChatPanel* tmp = (ChatPanel*)m_chat_tabs->GetPage(i);
    if ( tmp->GetPanelType() == CPT_User ) {
      wxString name = m_chat_tabs->GetPageText(i);
      if ( name.Lower() == user.Lower() ) return (ChatPanel*)m_chat_tabs->GetPage(i);
    }
  }
  return 0;
}


void MainChatTab::OnUserConnected( User& user )
{
  ChatPanel* panel = GetUserChatPanel( user.GetNick() );
  if ( panel != 0 ) {
    panel->SetUser( &user );
    panel->OnUserConnected();
    //TODO enable send button (koshi)
  }
}


void MainChatTab::OnUserDisconnected( User& user )
{
  ChatPanel* panel = GetUserChatPanel( user.GetNick() );
  if ( panel != 0 ) {
    panel->OnUserDisconnected();
    panel->SetUser( 0 );
    //TODO disable send button (koshi)
  }
}


void MainChatTab::RejoinChannels()
{
  for ( unsigned int i = 0; i < m_chat_tabs->GetPageCount(); i++ ) {
    ChatPanel* tmp = (ChatPanel*)m_chat_tabs->GetPage(i);
    if ( tmp->GetPanelType() == CPT_Channel ) {

      // TODO: This will not rejoin passworded channels.
      wxString name = m_chat_tabs->GetPageText(i);
      // #springlobby is joined automatically
      if ( name != _T("springlobby") ) m_ui.GetServer().JoinChannel( name, _T("") );

    } else if (tmp->GetPanelType() == CPT_User ) {

      wxString name = m_chat_tabs->GetPageText(i);
      if ( m_ui.GetServer().UserExists( name ) ) tmp->SetUser( &m_ui.GetServer().GetUser( name ) );

    }
  }
}


ChatPanel* MainChatTab::AddChatPannel( Channel& channel )
{

  for ( unsigned int i = 0; i < m_chat_tabs->GetPageCount(); i++ ) {
    if ( m_chat_tabs->GetPageText(i) == channel.GetName() ) {
      ChatPanel* tmp = (ChatPanel*)m_chat_tabs->GetPage(i);
      if ( tmp->GetPanelType() == CPT_Channel ) {
        m_chat_tabs->SetSelection( i );
        tmp->SetChannel( &channel );
        return tmp;
      }
    }
  }

  ChatPanel* chat = new ChatPanel( m_chat_tabs, m_ui, channel, m_imagelist );
  #ifdef HAVE_WX26
  m_chat_tabs->InsertPage( m_chat_tabs->GetPageCount() - 1, chat, channel.GetName(), true, 2 );
  #else
  m_chat_tabs->InsertPage( m_chat_tabs->GetPageCount() - 1, chat, channel.GetName(), true, wxBitmap(channel_xpm) );
  #endif
  chat->FocusInputBox();
  return chat;
}

ChatPanel* MainChatTab::AddChatPannel( Server& server, const wxString& name )
{

  for ( unsigned int i = 0; i < m_chat_tabs->GetPageCount(); i++ ) {
    if ( m_chat_tabs->GetPageText(i) == name ) {
      ChatPanel* tmp = (ChatPanel*)m_chat_tabs->GetPage(i);
      if ( tmp->GetPanelType() == CPT_Server ) {
        m_chat_tabs->SetSelection( i );
        tmp->SetServer( &server );
        return tmp;
      }
    }
  }

  ChatPanel* chat = new ChatPanel( m_chat_tabs, m_ui, server, m_imagelist );
  #ifdef HAVE_WX26
  m_chat_tabs->InsertPage( m_chat_tabs->GetPageCount() - 1, chat, name, true, 1 );
  #else
  m_chat_tabs->InsertPage( m_chat_tabs->GetPageCount() - 1, chat, name, true, wxBitmap(server_xpm) );
  #endif
  return chat;
}

ChatPanel* MainChatTab::AddChatPannel( User& user )
{
  for ( unsigned int i = 0; i < m_chat_tabs->GetPageCount(); i++ ) {
    if ( m_chat_tabs->GetPageText(i) == user.GetNick() ) {
      ChatPanel* tmp = (ChatPanel*)m_chat_tabs->GetPage(i);
      if ( tmp->GetPanelType() == CPT_User ) {
        m_chat_tabs->SetSelection( i );
        tmp->SetUser( &user );
        return tmp;
      }
    }
  }

  ChatPanel* chat = new ChatPanel( m_chat_tabs, m_ui, user, m_imagelist );
  #ifdef HAVE_WX26
  m_chat_tabs->InsertPage( m_chat_tabs->GetPageCount() - 1, chat, user.GetNick(), true, 3 );
  #else
  m_chat_tabs->InsertPage( m_chat_tabs->GetPageCount() - 1, chat, user.GetNick(), true, wxBitmap(userchat_xpm) );
  #endif
  chat->FocusInputBox();
  return chat;
}

#ifndef HAVE_WX26
void MainChatTab::OnTabClose( wxAuiNotebookEvent& event )
{
    int oldsel = event.GetOldSelection();
    ChatPanel* oldpanel = (ChatPanel*)m_chat_tabs->GetPage( oldsel );
    if ( oldpanel )
    {
        oldpanel->Part();
    }
}
#endif

#ifdef HAVE_WX26
void MainChatTab::OnTabsChanged( wxListbookEvent& event )
#else
void MainChatTab::OnTabsChanged( wxAuiNotebookEvent& event )
#endif
{
  wxLogDebugFunc( _T("") );

  int oldsel = event.GetOldSelection();
  if ( oldsel < 0 ) return;
  int newsel = event.GetSelection();
  if ( newsel < 0 ) return;

  // change icon to default the icon to show that no new events happened
  size_t ImageIndex = ((ChatPanel*)m_chat_tabs->GetPage(newsel))->GetIconIndex();
  if ( ImageIndex == 4 || ImageIndex == 6 || ImageIndex == 8 )
  {
     GetActiveChatPanel()->SetIconIndex(2);
     #ifdef HAVE_WX26
     m_chat_tabs->SetPageImage( newsel, 2);
     #else
     m_chat_tabs->SetPageBitmap( newsel, wxBitmap(channel_xpm));
     #endif
  }
  else if ( ImageIndex == 5 || ImageIndex == 7 || ImageIndex == 9 )
  {
     GetActiveChatPanel()->SetIconIndex(3);
     #ifdef HAVE_WX26
     m_chat_tabs->SetPageImage( newsel, 3);
     #else
     m_chat_tabs->SetPageBitmap( newsel, wxBitmap(userchat_xpm) );
     #endif
  }
  else if ( ImageIndex == 10 )
  {
     GetActiveChatPanel()->SetIconIndex(1);
     #ifdef HAVE_WX26
     m_chat_tabs->SetPageImage( newsel, 1);
     #else
     m_chat_tabs->SetPageBitmap( newsel, wxBitmap(server_xpm));
     #endif
  }

  wxWindow* newpage = m_chat_tabs->GetPage( newsel );
  if ( newpage == 0 ) { // Not sure what to do here
    wxLogError( _T("Newpage NULL.") );
    return;
  }

}


wxImage MainChatTab::ReplaceChannelStatusColour( wxBitmap img, const wxColour& colour )
{
  wxImage ret = img.ConvertToImage();
  wxImage::HSVValue origcolour = wxImage::RGBtoHSV( wxImage::RGBValue::RGBValue( colour.Red(), colour.Green(), colour.Blue() ) );

  double bright = origcolour.value - 0.1*origcolour.value;
  CLAMP(bright,0,1);
  wxImage::HSVValue hsvdarker1( origcolour.hue, origcolour.saturation, bright );
  bright = origcolour.value - 0.5*origcolour.value;
  CLAMP(bright,0,1);
  wxImage::HSVValue hsvdarker2( origcolour.hue, origcolour.saturation, bright );

  wxImage::RGBValue rgbdarker1 = wxImage::HSVtoRGB( hsvdarker1 );
  wxImage::RGBValue rgbdarker2 = wxImage::HSVtoRGB( hsvdarker2 );


  ret.Replace( 164, 147, 0, rgbdarker2.red, rgbdarker2.green, rgbdarker2.blue );

  ret.Replace( 255, 228, 0, rgbdarker1.red, rgbdarker1.green, rgbdarker1.blue );

  ret.Replace( 255, 253, 234, colour.Red(), colour.Green(), colour.Blue() );

  ret.Replace( 255, 228, 0, rgbdarker1.red, rgbdarker1.green, rgbdarker1.blue );


    return ret;
}

bool MainChatTab::RemoveChatPanel( ChatPanel* panel )
{
    for ( unsigned int i = 0; i < m_chat_tabs->GetPageCount(); i++ ) {
        ChatPanel* tmp = (ChatPanel*)m_chat_tabs->GetPage(i);
        if ( tmp == panel && panel != 0 )
        {
            m_chat_tabs->DeletePage(i);
            return true;
        }
    }
    return false;
}


void MainChatTab::Update()
{
    for ( unsigned int i = 0; i < m_chat_tabs->GetPageCount(); i++ )
    {
        ChatPanel* tmp = (ChatPanel*)m_chat_tabs->GetPage(i);
        if ( m_close_window == m_chat_tabs->GetPage( i ) ) continue; ///skip the close button
        if ( ( tmp != 0 ) && ( tmp->GetPanelType() == CPT_Channel ) )
        {
            tmp->SortNickList();
        }
    }
}
