#ifndef WIDGETDOWNLOADLISTCTRL_H
#define WIDGETDOWNLOADLISTCTRL_H

#include "../customvirtlistctrl.h"
#include "widget.h"


class WidgetDownloadListctrl : public CustomVirtListCtrl<Widget>
{
    public:
        WidgetDownloadListctrl(wxWindow* parent, wxWindowID id, const wxString& name = _T("WidgetDownloadListCtrl"),
            long style = wxSUNKEN_BORDER | wxLC_REPORT | wxLC_ALIGN_LEFT, const wxPoint& pt = wxDefaultPosition,
                    const wxSize& sz = wxDefaultSize);
        virtual ~WidgetDownloadListctrl();

        void AddWidget( const Widget widget );
        Widget& GetSelectedWidget();

        //these are overloaded to use list in virtual style
        virtual wxString OnGetItemText(long item, long column) const;
        virtual int OnGetItemImage(long item) const;
        virtual int OnGetItemColumnImage(long item, long column) const;

        void Sort();

    protected:
        static int CompareOneCrit( DataType u1, DataType u2, int col, int dir ) ;

        void HighlightItem( long item );
        void OnColClick( wxListEvent& event );
        void OnActivateItem( wxListEvent& event );

        int GetIndexFromData( const DataType& data ) const;

    public:
        enum {
            WIDGETLISTCTRL_ID
        };

    protected:
        DECLARE_EVENT_TABLE()
};

#endif // WIDGETDOWNLOADLISTCTRL_H
