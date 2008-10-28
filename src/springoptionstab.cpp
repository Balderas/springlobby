
/* Copyright (C) 2007, 2008 The SpringLobby Team. All rights reserved. */
//
// Class: SpringOptionsTab
//

#include <wx/filedlg.h>
#include <wx/dirdlg.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/intl.h>
#include <wx/textctrl.h>
#include <wx/radiobut.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/dir.h>
#include <wx/file.h>
#include <wx/stdpaths.h>
#include <wx/scrolbar.h>        // added for scroll bar powers
#include <wx/log.h>

#ifdef __WXMSW__
#include <wx/msw/registry.h>
#endif

#include "nonportable.h"
#include "springoptionstab.h"
#include "ui.h"
#include "iunitsync.h"
#include "utils.h"
#include "settings.h"
#include "mainwindow.h"

#include "settings++/custom_dialogs.h"

BEGIN_EVENT_TABLE(SpringOptionsTab, wxPanel)

    EVT_BUTTON ( SPRING_EXECBROWSE, SpringOptionsTab::OnBrowseExec )
    EVT_BUTTON ( SPRING_SYNCBROWSE, SpringOptionsTab::OnBrowseSync )
    EVT_BUTTON ( SPRING_AUTOCONF, SpringOptionsTab::OnAutoConf )
    EVT_BUTTON ( SPRING_EXECFIND, SpringOptionsTab::OnFindExec )
    EVT_BUTTON ( SPRING_SYNCFIND, SpringOptionsTab::OnFindSync )
END_EVENT_TABLE()


SpringOptionsTab::SpringOptionsTab( wxWindow* parent, Ui& ui ) : wxScrolledWindow( parent, -1 ),m_ui(ui)
{
  /* ================================
   * Spring executable
   */
  m_exec_box = new wxStaticBox( this, -1, _("Spring executable") );

  m_exec_loc_text = new wxStaticText( this, -1, _("Location") );
  m_exec_edit = new wxTextCtrl( this, -1, sett().GetCurrentUsedSpringBinary() );
  m_exec_browse_btn = new wxButton( this, SPRING_EXECBROWSE, _("Browse") );
  m_exec_find_btn = new wxButton( this, SPRING_EXECFIND, _("Find") );

  /* ================================
   * UnitSync
   */
  m_sync_box = new wxStaticBox( this, -1, _("UnitSync library") );

  m_sync_edit = new wxTextCtrl( this, -1, sett().GetCurrentUsedUnitSync() );
  m_sync_loc_text = new wxStaticText( this, -1, _("Location") );
  m_sync_browse_btn = new wxButton( this, SPRING_SYNCBROWSE, _("Browse") );
  m_sync_find_btn = new wxButton( this, SPRING_SYNCFIND, _("Find") );

  m_auto_btn = new wxButton( this, SPRING_AUTOCONF, _("Auto Configure") );

  m_main_sizer = new wxBoxSizer( wxVERTICAL );
  m_aconf_sizer = new wxBoxSizer( wxVERTICAL );
  m_exec_loc_sizer = new wxBoxSizer( wxHORIZONTAL );
  m_sync_loc_sizer = new wxBoxSizer( wxHORIZONTAL );

  m_exec_loc_sizer->Add( m_exec_loc_text, 0, wxALL, 2 );
  m_exec_loc_sizer->Add( m_exec_edit, 1, wxEXPAND );
  m_exec_loc_sizer->Add( m_exec_browse_btn );
  m_exec_loc_sizer->Add( m_exec_find_btn );

  m_sync_loc_sizer->Add( m_sync_loc_text, 0, wxALL, 2 );
  m_sync_loc_sizer->Add( m_sync_edit, 1, wxEXPAND );
  m_sync_loc_sizer->Add( m_sync_browse_btn );
  m_sync_loc_sizer->Add( m_sync_find_btn );

  m_exec_box_sizer = new wxStaticBoxSizer( m_exec_box, wxVERTICAL );
  m_sync_box_sizer = new wxStaticBoxSizer( m_sync_box, wxVERTICAL );

  m_exec_box_sizer->Add( m_exec_loc_sizer, 0, wxEXPAND | wxALL, 2 );

  m_sync_box_sizer->Add( m_sync_loc_sizer, 0, wxEXPAND | wxALL, 2 );

  m_aconf_sizer->AddStretchSpacer();
  m_aconf_sizer->Add( m_auto_btn );

  m_main_sizer->Add( m_exec_box_sizer, 0, wxEXPAND | wxALL, 2 );
  m_main_sizer->Add( m_sync_box_sizer, 0, wxEXPAND | wxALL, 2 );

  m_main_sizer->Add( m_aconf_sizer, 0, wxEXPAND | wxALL, 2 );
  m_main_sizer->AddStretchSpacer();

  SetSizer( m_main_sizer );

  SetScrollRate( 3, 3 );

  Layout();

  DoRestore();

  if ( sett().IsFirstRun() )
    {
      wxCommandEvent event;
      OnAutoConf( event );
      OnApply( event );
    }

  if ( sett().IsPortableMode() )
  {
    m_exec_box->Disable();
    m_sync_box->Disable();
  }

}


SpringOptionsTab::~SpringOptionsTab()
{

}


void SpringOptionsTab::DoRestore()
{
  m_sync_edit->SetValue( sett().GetCurrentUsedUnitSync() );
  m_exec_edit->SetValue( sett().GetCurrentUsedSpringBinary() );
}


bool SpringOptionsTab::IsSpringExe( const wxString& exe )
{
  if ( !wxFile::Exists( exe ) ) return false;
#ifdef HAVE_WX28
  if ( !wxFileName::IsFileExecutable( exe ) ) return false;
#endif
  return true;
}


bool SpringOptionsTab::IsUnitSyncLib( const wxString& lib )
{
  if ( !wxFile::Exists( lib ) ) return false;
  return true;
}


wxString SpringOptionsTab::AutoFindSpringExe( const wxString& def )
{
  wxPathList pl;
  wxStandardPathsBase& sp = wxStandardPathsBase::Get();

#ifdef __WXMSW__
  wxRegKey programreg( _T("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion") );
  wxString tmp;
  if ( programreg.QueryValue( _T("ProgramFilesDir"), tmp ) ) pl.Add( tmp );

  pl.Add( _T("C:\\Program") );
  pl.Add( _T("C:\\Program Files") );
#else
  pl.Add( _T("/usr/local/games") );
  pl.Add( _T("/usr/local/games/bin") );
  pl.Add( _T("/usr/local/bin") );
  pl.Add( _T("/usr/games") );
  pl.Add( _T("/usr/games/bin") );
  pl.Add( _T("/usr/bin") );
#endif

  pl.Add( wxFileName::GetCwd() );

#ifdef HAVE_WX28
  pl.Add( sp.GetExecutablePath() );
#endif

  pl.Add( wxFileName::GetHomeDir() );
  pl.Add( sp.GetUserDataDir().BeforeLast( wxFileName::GetPathSeparator() ) );
  pl.Add( sp.GetDataDir().BeforeLast( wxFileName::GetPathSeparator() ) );

#ifdef HAVE_WX28
  pl.Add( sp.GetResourcesDir().BeforeLast( wxFileName::GetPathSeparator() ) );
#endif

  for ( size_t i = 0; i < pl.GetCount(); i++ ) {
    wxString path = pl[i];
    if ( path.Last() != wxFileName::GetPathSeparator() ) path += wxFileName::GetPathSeparator();
    if ( IsSpringExe( path + SPRING_BIN ) ) return path + SPRING_BIN;
    if ( IsSpringExe( path + _T("Spring") + wxFileName::GetPathSeparator() + SPRING_BIN ) ) return path + _T("Spring") + wxFileName::GetPathSeparator() + SPRING_BIN;
    if ( IsSpringExe( path + _T("spring") + wxFileName::GetPathSeparator() + SPRING_BIN ) ) return path + _T("spring") + wxFileName::GetPathSeparator() + SPRING_BIN;
  }

  return def;
}


wxString SpringOptionsTab::AutoFindUnitSyncLib( const wxString& def )
{
  wxPathList pl;
  wxStandardPathsBase& sp = wxStandardPathsBase::Get();

#ifdef __WXMSW__
  wxRegKey programreg( _T("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion") );
  wxString tmp;
  if ( programreg.QueryValue( _T("ProgramFilesDir"), tmp ) ) pl.Add( tmp );

  pl.Add( wxGetOSDirectory() );
  pl.Add( _T("C:\\Program") );
  pl.Add( _T("C:\\Program Files") );
#else
  pl.Add( _T("/usr/local/lib64") );
  pl.Add( _T("/usr/local/games") );
  pl.Add( _T("/usr/local/games/lib") );
  pl.Add( _T("/usr/local/lib") );
  pl.Add( _T("/usr/lib64") );
  pl.Add( _T("/usr/lib") );
  pl.Add( _T("/usr/games") );
  pl.Add( _T("/usr/games/lib64") );
  pl.Add( _T("/usr/games/lib") );
#endif

  pl.Add( wxFileName::GetCwd() );

#ifdef HAVE_WX28
  pl.Add( sp.GetExecutablePath() );
#endif

  pl.Add( wxFileName::GetCwd() );

#ifdef HAVE_WX28
  pl.Add( sp.GetExecutablePath() );
#endif

  pl.Add( wxFileName::GetHomeDir() );
  pl.Add( sp.GetUserDataDir().BeforeLast( wxFileName::GetPathSeparator() ) );
  pl.Add( sp.GetDataDir().BeforeLast( wxFileName::GetPathSeparator() ) );

#ifdef HAVE_WX28
  pl.Add( sp.GetResourcesDir().BeforeLast( wxFileName::GetPathSeparator() ) );
#endif

  for ( size_t i = 0; i < pl.GetCount(); i++ ) {
    wxString path = pl[i];
    if ( path.Last() != wxFileName::GetPathSeparator() ) path += wxFileName::GetPathSeparator();
    if ( IsUnitSyncLib( path + _T("unitsync") + GetLibExtension() ) ) return path + _T("unitsync") + GetLibExtension();
    if ( IsUnitSyncLib( path + _T("Spring") + wxFileName::GetPathSeparator() + _T("unitsync") + GetLibExtension() ) ) return path + _T("Spring") + wxFileName::GetPathSeparator() + _T("unitsync") + GetLibExtension();
    if ( IsUnitSyncLib( path + _T("spring") + wxFileName::GetPathSeparator() + _T("unitsync") + GetLibExtension() ) ) return path + _T("spring") + wxFileName::GetPathSeparator() + _T("unitsync") + GetLibExtension();
    if ( IsUnitSyncLib( path + _T("libunitsync") + GetLibExtension() ) ) return path + _T("libunitsync") + GetLibExtension();
    if ( IsUnitSyncLib( path + _T("Spring") + wxFileName::GetPathSeparator() + _T("libunitsync") + GetLibExtension() ) ) return path + _T("Spring") + wxFileName::GetPathSeparator() + _T("libunitsync") + GetLibExtension();
    if ( IsUnitSyncLib( path + _T("spring") + wxFileName::GetPathSeparator() + _T("libunitsync") + GetLibExtension() ) ) return path + _T("spring") + wxFileName::GetPathSeparator() + _T("libunitsync") + GetLibExtension();
  }

  return def;
}


void SpringOptionsTab::OnAutoConf( wxCommandEvent& event )
{
  OnFindExec( event );
  OnFindSync( event );
}


void SpringOptionsTab::OnFindExec( wxCommandEvent& event )
{
  wxString found = AutoFindSpringExe( m_exec_edit->GetValue() );
  m_exec_edit->SetValue( found );
}


void SpringOptionsTab::OnFindSync( wxCommandEvent& event )
{
  wxString found = AutoFindUnitSyncLib( m_sync_edit->GetValue() );
  m_sync_edit->SetValue( found );
}

void SpringOptionsTab::OnBrowseExec( wxCommandEvent& event )
{
  wxFileDialog pick( this, _("Choose a Spring executable"),
		    sett().GetCurrentUsedSpringBinary(),
		    wxString(SPRING_BIN), CHOOSE_EXE );
  if ( pick.ShowModal() == wxID_OK ) m_exec_edit->SetValue( pick.GetPath() );
}

void SpringOptionsTab::OnBrowseSync( wxCommandEvent& event )
{
  wxFileDialog pick( this, _("Choose UnitSync library"),
		    sett().GetCurrentUsedSpringBinary(),
		    _T("unitsync") + GetLibExtension(),
		    wxString(_("Library")) + _T("(*") + GetLibExtension() + _T(")|*") + GetLibExtension() + _T("|") + wxString(_("Any File")) + _T(" (*.*)|*.*")  );
  if ( pick.ShowModal() == wxID_OK ) m_sync_edit->SetValue( pick.GetPath() );
}


void SpringOptionsTab::OnApply( wxCommandEvent& event )
{
  if ( sett().IsFirstRun() ) return;

  usync().FreeUnitSyncLib();
  if ( !usync().LoadUnitSyncLib( sett().GetCurrentUsedUnitSync() ) )
  {
    wxLogWarning( _T("Cannot load UnitSync") );
    customMessageBox( SL_MAIN_ICON,
    _("SpringLobby is unable to load your UnitSync library.\n\nYou might want to take another look at your unitsync setting."),
    _("Spring error"), wxOK );
  }

  m_ui.mw().OnUnitSyncReloaded();
}


void SpringOptionsTab::OnRestore( wxCommandEvent& event )
{
  DoRestore();
}

