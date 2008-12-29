#include "lobbyoptionstab.h"
#include "nonportable.h"
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/intl.h>
#include <wx/stattext.h>
#include <wx/checkbox.h>
#include <wx/tooltip.h>
#include <wx/radiobut.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/filedlg.h>

#include "settings.h"
#include "springlobbyapp.h"
#include "settings++/custom_dialogs.h"
#include "utils.h"


#ifndef HAVE_WX26
#include "aui/auimanager.h"
#endif

BEGIN_EVENT_TABLE(LobbyOptionsTab, wxPanel)
    EVT_BUTTON ( SPRING_WEBBROWSE, LobbyOptionsTab::OnBrowseWeb )
    EVT_BUTTON ( BUTTON_SELECT_LOCALE, LobbyOptionsTab::OnSelectLocale )
    EVT_RADIOBUTTON( SPRING_DEFWEB, LobbyOptionsTab::OnDefaultWeb )
END_EVENT_TABLE()

LobbyOptionsTab::LobbyOptionsTab(wxWindow* parent)
    : wxScrolledWindow( parent, -1 )
{
    #ifndef HAVE_WX26
    GetAui().manager->AddPane( this, wxLEFT, _T("lobbyoptionstab") );
    #endif
    m_main_sizer = new wxBoxSizer ( wxVERTICAL );

    //! TODO make this not feel totally misplaced
    wxStaticBoxSizer* m_select_locale_sizer = new wxStaticBoxSizer ( wxVERTICAL, this, _("") );
    m_select_locale = new wxButton( this, -1, _("Choose lanuage"), wxDefaultPosition, wxDefaultSize, 0 );
    m_select_locale_sizer->Add( m_select_locale, 0, wxEXPAND|wxALL, 5);

    m_main_sizer->Add( m_select_locale_sizer, 0, wxALL, 15 );

/* ================================
   * Web browser
   */

    wxStaticBox* m_web_box = new wxStaticBox( this , -1, _("Web Browser") );
    m_web_loc_text = new wxStaticText( this, -1, _("Web Browser") );

    m_web_def_radio = new wxRadioButton( this, SPRING_DEFWEB, _("Default Browser."),
                 wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
    m_web_def_radio->SetToolTip(TE(_("Use your system-wide browser preference")));

    m_web_spec_radio = new wxRadioButton( this, SPRING_DEFWEB, _("Specify:") );
    m_web_spec_radio->SetToolTip(TE(_("Specify the web browser you want to use")));

    m_web_edit = new wxTextCtrl( this, -1, sett().GetWebBrowserPath() );

    m_web_browse_btn = new wxButton( this, SPRING_WEBBROWSE, _("Browse") );
    m_web_browse_btn->SetToolTip(TE(_("Use a file dialog to find the web browser")));

    if ( sett().GetWebBrowserUseDefault() ) m_web_def_radio->SetValue( true );
    else m_web_spec_radio->SetValue( true );

    m_web_loc_sizer = new wxBoxSizer( wxHORIZONTAL );
    m_web_loc_sizer->Add( m_web_loc_text, 0, wxALL, 2 );
    m_web_loc_sizer->Add( m_web_edit, 1, wxEXPAND );
    m_web_loc_sizer->Add( m_web_browse_btn );

    wxStaticBoxSizer* m_web_box_sizer = new wxStaticBoxSizer( m_web_box, wxVERTICAL );

    m_web_box_sizer->Add( m_web_def_radio, 0, wxALL, 2 );
    m_web_box_sizer->Add( m_web_spec_radio, 0, wxALL, 2 );
    m_web_box_sizer->Add( m_web_loc_sizer, 0, wxEXPAND | wxALL, 2 );

    wxStaticBoxSizer* m_autojoin_sizer= new wxStaticBoxSizer ( wxVERTICAL, this, _("Autoconnect") );
    m_autoconnect_label = new wxStaticText ( this, -1, _("If checked, SpringLobby will automatically log on to the last used server") );
    m_autojoin = new wxCheckBox( this, -1, _("Autoconnect on lobby start"), wxDefaultPosition, wxDefaultSize, 0 );
    m_autojoin->SetValue( sett().GetAutoConnect() );
    m_autojoin_sizer->Add( m_autoconnect_label, 1, wxEXPAND | wxALL, 5 );
    m_autojoin_sizer->Add( m_autojoin, 0, wxEXPAND | wxALL, 5 );

    wxStaticBoxSizer* m_reportstats_sizer = new wxStaticBoxSizer ( wxVERTICAL, this, _("Report statistics") );
    m_reportstats_label = new wxStaticText ( this, -1, _("By default SpringLobby will send some statistics (OS,SpringLobby version) to server,\nto both make helping you in case of problems easier and inform of critical updates.\nUncheck to disable.") );
    m_reportstats = new wxCheckBox( this, -1, _("report statistics"), wxDefaultPosition, wxDefaultSize, 0 );
    m_reportstats->SetValue( sett().GetReportStats() );
    m_reportstats_sizer->Add( m_reportstats_label, 1, wxEXPAND|wxALL, 5);
    m_reportstats_sizer->Add( m_reportstats, 0, wxEXPAND|wxALL, 5);

    m_main_sizer->Add( m_web_box_sizer, 0, wxEXPAND | wxALL, 15 );
    m_main_sizer->Add( m_autojoin_sizer, 0, wxALL, 15 );
    m_main_sizer->Add( m_reportstats_sizer, 0, wxALL, 15 );

#ifdef __WXMSW__
    wxStaticBoxSizer* m_updater_sizer = new wxStaticBoxSizer ( wxVERTICAL, this, _("Automatic updates") );
    m_updater_label = new wxStaticText ( this, -1, _("SpringLobby can check at startup if a newer version is available and automatically download it for you.") );
    m_updater = new wxCheckBox( this, -1, _("automatically check for updates"), wxDefaultPosition, wxDefaultSize, 0 );
    m_updater->SetValue( sett().GetAutoUpdate() );
    m_updater_sizer->Add( m_updater_label, 1, wxEXPAND|wxALL, 5);
    m_updater_sizer->Add( m_updater, 0, wxEXPAND|wxALL, 5);

    m_main_sizer->Add( m_updater_sizer, 0, wxALL, 15 );
#endif

    wxStaticBoxSizer* m_show_tooltips_sizer = new wxStaticBoxSizer ( wxVERTICAL, this, _("Tooltips") );
    m_show_tooltips_label = new wxStaticText ( this, -1, _("Requires SpringLobby restart to take effect.") );
    m_show_tooltips = new wxCheckBox( this, -1, _("Show Tooltips?"), wxDefaultPosition, wxDefaultSize, 0 );
    m_show_tooltips->SetValue( sett().GetShowTooltips() );
#ifndef __WXMSW__ // on windows this change is immediate
    m_show_tooltips_sizer->Add( m_show_tooltips_label, 1, wxEXPAND|wxALL, 5);
#endif
    m_show_tooltips_sizer->Add( m_show_tooltips, 0, wxEXPAND|wxALL, 5);

    m_main_sizer->Add( m_show_tooltips_sizer, 0, wxALL, 15 );

    wxStaticBoxSizer* m_complete_method_sizer = new wxStaticBoxSizer ( wxVERTICAL, this, _("Tab completion method") );
    m_complete_method_label = new wxStaticText ( this, -1, _("\"Match exact\" will complete a word if there is one and only one match.\n"
        "\"Match nearest\" will select the (first) match that has closest Levenshtein distance") );
    m_complete_method_old = new wxRadioButton( this, -1, _("Match exact"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
    m_complete_method_new = new wxRadioButton( this, -1, _("Match nearest"), wxDefaultPosition, wxDefaultSize );
    m_complete_method_old->SetValue( sett().GetCompletionMethod() == Settings::MatchExact );
    m_complete_method_new->SetValue( sett().GetCompletionMethod() == Settings::MatchNearest );
    m_complete_method_sizer->Add( m_complete_method_label, 1, wxEXPAND|wxALL, 5);
    m_complete_method_sizer->Add( m_complete_method_old, 0, wxEXPAND|wxALL, 5);
    m_complete_method_sizer->Add( m_complete_method_new, 0, wxEXPAND|wxALL, 5);

    m_main_sizer->Add( m_complete_method_sizer, 0, wxALL, 15 );

    SetScrollRate( 10, 10 );
    SetSizer( m_main_sizer );
}

LobbyOptionsTab::~LobbyOptionsTab()
{
    //dtor
}

void LobbyOptionsTab::OnApply(wxCommandEvent& event)
{
    if ( !m_web_def_radio->GetValue() ) sett().SetWebBrowserPath( m_web_edit->GetValue() );
    sett().SetWebBrowserUseDefault( m_web_def_radio->GetValue() );

    sett().SetAutoConnect( m_autojoin->IsChecked() );
    sett().SetReportStats( m_reportstats->GetValue() );
#ifdef __WXMSW__
    sett().SetAutoUpdate( m_updater->IsChecked() );
#endif
    bool show = m_show_tooltips->IsChecked();
    wxToolTip::Enable(show);
    sett().SetShowTooltips(show);

    sett().SetCompletionMethod( m_complete_method_new->GetValue() ? Settings::MatchNearest: Settings::MatchExact );
}


void LobbyOptionsTab::OnRestore(wxCommandEvent& event)
{
    m_autojoin->SetValue( sett().GetAutoConnect() );
    m_reportstats->SetValue( sett().GetReportStats() );
#ifdef __WXMSW__
    m_updater->SetValue( sett().GetAutoUpdate() );
#endif
    bool show = sett().GetShowTooltips();
    m_show_tooltips->SetValue(show);
    wxToolTip::Enable(show);

    m_complete_method_old->SetValue( sett().GetCompletionMethod() == Settings::MatchExact );
    m_complete_method_new->SetValue( sett().GetCompletionMethod() == Settings::MatchNearest );

    HandleWebloc( sett().GetWebBrowserUseDefault() );
}

void LobbyOptionsTab::HandleWebloc( bool defloc )
{
  m_web_def_radio->SetValue( defloc );
  m_web_spec_radio->SetValue( !defloc );
  if ( defloc ) {
    m_web_edit->Enable( false );
    m_web_browse_btn->Enable( false );
    //m_sync_find_btn->Enable( false );
    m_web_edit->SetValue( _T("") );
  } else {
    m_web_edit->Enable( true );
    m_web_browse_btn->Enable( true );
    //m_sync_find_btn->Enable( true );
    m_web_edit->SetValue( sett().GetWebBrowserPath() );
  }
}

void LobbyOptionsTab::OnBrowseWeb( wxCommandEvent& event )
{
  wxFileDialog pick( this, _("Choose a web browser executable"), _T(""), _T("*"), CHOOSE_EXE );
  if ( pick.ShowModal() == wxID_OK ) m_web_edit->SetValue( pick.GetPath() );
}

void LobbyOptionsTab::OnDefaultWeb( wxCommandEvent& event )
{
  HandleWebloc( m_web_def_radio->GetValue() );
}

void LobbyOptionsTab::OnSelectLocale( wxCommandEvent& event )
{
    if ( wxGetApp().SelectLanguage() ) {
        customMessageBoxNoModal( SL_MAIN_ICON, _("You need to restart SpringLobby for the language change to take effect."),
                                    _("Restart required"), wxICON_EXCLAMATION | wxOK );
    }
}
