#ifndef AUTOJOINCHANNELDIALOG_H
#define AUTOJOINCHANNELDIALOG_H

#include <wx/dialog.h>
#include <wx/intl.h>
class wxCommandEvent;
class wxTextCtrl;
class wxWindow;

/** \brief A dialog that lets the user manullay edit his list of channels to join on startup (possibly with passwords)
 * Passwords for existing channels aren't displayed. The dialog expects a [chanName|password] pair per line.
 * \todo See that parsing input doesn't crap out when user enters rubbish
 */
class AutojoinChannelDialog : public wxDialog
{
	public:
		/** Default constructor */
		AutojoinChannelDialog( wxWindow* parent );
		/** Default destructor */
		virtual ~AutojoinChannelDialog();
	protected:

		wxTextCtrl* m_channel_list;

		void OnOk( wxCommandEvent& event );
		void OnCancel( wxCommandEvent& event );
        wxString m_old_channels;
		DECLARE_EVENT_TABLE()
};

#endif // AUTOJOINCHANNELDIALOG_H
