#ifndef SPRINGLOBBY_HEADERGUARD_AGREEMENTDIALOG_H
#define SPRINGLOBBY_HEADERGUARD_AGREEMENTDIALOG_H

#include <wx/dialog.h>

class wxTextCtrl;
class wxStaticText;
class wxStaticLine;
class wxButton;

/** \brief dialog showing agreement on first start/ register that user needs to accept
 * \todo DOCMEMORE */
class AgreementDialog : public wxDialog
{
  public:
    AgreementDialog( wxWindow* parent, const wxString& agreement );

    void OnYes( wxCommandEvent& event );
    void OnNo( wxCommandEvent& event );

  protected:
    wxTextCtrl* m_text;
    wxStaticText* m_accept_lbl;
    wxStaticLine* m_button_sep;
    wxButton* m_yes_btn;
    wxButton* m_no_btn;

    enum {
        AGREE_YES = wxID_HIGHEST,
        AGREE_NO,
    };

    DECLARE_EVENT_TABLE()

};

#endif // SPRINGLOBBY_HEADERGUARD_AGREEMENTDIALOG_H
