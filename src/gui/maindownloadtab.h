/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef MAINTORRENTTAB_H
#define MAINTORRENTTAB_H

#include <wx/scrolwin.h>
#include <wx/timer.h>
#include <map>

class wxStaticText;
class wxButton;
class DownloadListCtrl;
class Ui;
struct DownloadInfo;
class wxBoxSizer;
class ColorButton;
class WidgetDownloadDialog;
class ContentDownloadDialog;

typedef std::map<wxString, DownloadInfo> map_infos;
typedef map_infos::iterator map_infos_iter;

class MainDownloadTab : public wxPanel
{
public:
	MainDownloadTab(wxWindow* parent);
	~MainDownloadTab();

	void OnUpdate(wxTimerEvent& /*event*/);

private:
	enum {
		ID_LIST = wxID_HIGHEST,
		ID_BUTTON_CANCEL,
		ID_OUTGOING,
		ID_INCOMING,
		ID_OUTGOING_LBL,
		ID_INCOMING_LBL,
		ID_DOWNLOAD_DIALOG,
		ID_BUTTON_CLEAR,
		ID_BUTTON_WIDGETS,
		ID_TIMER
	};

	wxButton* m_but_cancel;
	wxButton* m_but_clear;
	DownloadListCtrl* m_dl_listctrl;
	WidgetDownloadDialog* m_widgets_dialog;

	void OnCancelButton(wxCommandEvent& event);
	void OnDownloadDialog(wxCommandEvent& event);
	void OnDLWidgets(wxCommandEvent& event);
	void OnClearFinished(wxCommandEvent& event);

	wxBoxSizer* m_mainbox;

private:
	//(*Handlers(MainTorrentTab)
	//*)

	DECLARE_EVENT_TABLE()
	ContentDownloadDialog* m_download_dialog;
	wxButton* m_but_download;
	wxTimer* timer;
};

#endif
