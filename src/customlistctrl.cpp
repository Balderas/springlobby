#include "customlistctrl.h"
#include "utils.h"
#include "settings.h"
#include <wx/colour.h>
#include <wx/log.h>
#include "iconimagelist.h"
#include "settings++/custom_dialogs.h"
#include "uiutils.h"


BEGIN_EVENT_TABLE(CustomListCtrl, ListBaseType)
#if wxUSE_TIPWINDOW
  EVT_MOTION(CustomListCtrl::OnMouseMotion)
  EVT_TIMER(IDD_TIP_TIMER, CustomListCtrl::OnTimer)
#endif
  EVT_LIST_COL_BEGIN_DRAG(wxID_ANY, CustomListCtrl::OnStartResizeCol)
  EVT_LIST_COL_END_DRAG(wxID_ANY, CustomListCtrl::OnEndResizeCol)
  EVT_LEAVE_WINDOW(CustomListCtrl::noOp)
  EVT_LIST_ITEM_SELECTED   ( wxID_ANY, CustomListCtrl::OnSelected )
  EVT_LIST_ITEM_DESELECTED ( wxID_ANY, CustomListCtrl::OnDeselected )
  EVT_LIST_DELETE_ITEM     ( wxID_ANY, CustomListCtrl::OnDeselected )
END_EVENT_TABLE()


//wxTipWindow* CustomListCtrl::m_tipwindow = 0;
CustomListCtrl::CustomListCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pt, const wxSize& sz,long style,wxString name,
                                bool highlight, UserActions::ActionType hlaction ):
  ListBaseType(parent, id, pt, sz, style),
  m_tiptimer(this, IDD_TIP_TIMER),
  m_tiptext(_T("")),
  m_selected(-1),
  m_selected_index(-1),
  m_prev_selected(-1),
  m_prev_selected_index(-1),
  m_last_mouse_pos( wxPoint(-1,-1) ),
  m_name(name),
  m_highlight(highlight),
  m_highlightAction(hlaction),
  m_bg_color( GetBackgroundColour() ),
  m_dirty_sort(false)
{
#if wxUSE_TIPWINDOW
    m_tipwindow = 0;
    controlPointer = 0;
#endif
    m_tiptext = _T("");


    SetImageList( &icons(), wxIMAGE_LIST_NORMAL );
    SetImageList( &icons(), wxIMAGE_LIST_SMALL );
    SetImageList( &icons(), wxIMAGE_LIST_STATE );
}

void CustomListCtrl::InsertColumn(long i, wxListItem item, wxString tip, bool modifiable)
{
//#ifdef __WXMSW__ //this fixes header text misalignement
//    item.m_mask = wxLIST_MASK_FORMAT | wxLIST_MASK_TEXT;
//    if ( item.m_image != icons().ICON_EMPTY || item.m_image != -1 )
//        item.m_mask = item.m_mask | wxLIST_MASK_IMAGE;
//
//    item.m_format = wxLIST_FORMAT_LEFT;
//#endif
    ListBaseType::InsertColumn(i,item);
    colInfo temp(tip,modifiable);
    m_colinfovec.push_back(temp);
}

void CustomListCtrl::AddColumn(long i, int width, const wxString& label, const wxString& tip, bool modifiable)
{
    ListBaseType::InsertColumn( i, label, wxLIST_FORMAT_LEFT, width);
    colInfo temp(tip,modifiable);
    m_colinfovec.push_back(temp);
}

void CustomListCtrl::SetSelectionRestorePoint()
{
  m_prev_selected = m_selected;
  m_prev_selected_index = m_selected_index;
}

void CustomListCtrl::RestoreSelection()
{
  if ( m_prev_selected_index> -1)
  {
    SetItemState( GetIndexFromData( m_prev_selected ), wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );
  }
}

void CustomListCtrl::ResetSelection()
{
  m_selected = m_prev_selected = m_prev_selected_index = m_selected_index = -1;
}

void CustomListCtrl::OnSelected( wxListEvent& event )
{
  m_selected = GetItemData( event.GetIndex() );
  m_selected_index = event.GetIndex();
  event.Skip();
}


void CustomListCtrl::OnDeselected( wxListEvent& event )
{
  if ( m_selected == (int)GetItemData( event.GetIndex() ) ) m_selected = m_selected_index = -1;
}

long CustomListCtrl::GetIndexFromData( const unsigned long data )
{
  for (int i = 0; i < GetItemCount() ; i++ )
  {
    if ( data == GetItemData( i ) )
      return i;
  }
  return -1;
}

long CustomListCtrl::GetSelectedIndex()
{
  return m_selected_index ;
}

void CustomListCtrl::SelectAll()
{
  for (long i = 0; i < GetItemCount() ; i++ )
  {
    SetItemState( i, wxLIST_STATE_SELECTED, -1  );
  }
}

void CustomListCtrl::SelectNone()
{
  for (long i = 0; i < GetItemCount() ; i++ )
  {
    SetItemState( i, wxLIST_STATE_DONTCARE, -1 );
  }
}

void CustomListCtrl::SelectInverse()
{
  for (long i = 0; i < GetItemCount() ; i++ )
  {
    int state = GetItemState( i, -1 );
    state = ( state == wxLIST_STATE_DONTCARE ? wxLIST_STATE_SELECTED : wxLIST_STATE_DONTCARE );
    SetItemState( i, state, -1 );
  }
}

void CustomListCtrl::SetSelectedIndex(const long newindex)
{
    m_selected_index = newindex;
}

long CustomListCtrl::GetSelectedData()
{
  return m_selected ;
}

void CustomListCtrl::OnTimer(wxTimerEvent& event)
{
#if wxUSE_TIPWINDOW

  if (!m_tiptext.empty())
  {
      m_tipwindow = new SLTipWindow(this, m_tiptext);
      controlPointer = &m_tipwindow;
      m_tipwindow->SetTipWindowPtr((wxTipWindow**)controlPointer);
#ifndef __WXMSW__
      m_tipwindow->SetBoundingRect(wxRect(1,1,50,50));
#endif
      m_tiptext = wxEmptyString;
      m_tiptimer.Start(m_tooltip_duration, wxTIMER_ONE_SHOT);
  }
  else
  {
      m_tiptext = wxEmptyString;
      m_tiptimer.Stop();
      if (controlPointer!= 0 && *controlPointer!= 0)
      {
          m_tipwindow->Close();
          m_tipwindow = 0;
      }
  }

#endif
}

void CustomListCtrl::OnMouseMotion(wxMouseEvent& event)
{
#if wxUSE_TIPWINDOW
  //we don't want to display the tooltip again until mouse has moved
  if ( m_last_mouse_pos == event.GetPosition() )
    return;

  m_last_mouse_pos = event.GetPosition();

  if (event.Leaving())
  {
    m_tiptext = _T("");
    if (m_tipwindow)
    {
      m_tipwindow->Close();
      m_tipwindow = 0;
    }
    m_tiptimer.Stop();
  }
  else
  {
    if ( m_tiptimer.IsRunning() )
    {
      m_tiptimer.Stop();
    }

    wxPoint position = event.GetPosition();

    int flag = wxLIST_HITTEST_ONITEM;

#ifdef HAVE_WX28
    long subItem;
    long item_hit = HitTest(position, flag, &subItem);
#else
    long item_hit = HitTest(position, flag);
#endif
    if (item_hit != wxNOT_FOUND && item_hit>=0 && item_hit<GetItemCount())
    {
        // we don't really need to recover from this if it fails
      try
      {
        SetTipWindowText(item_hit,m_last_mouse_pos);
        m_tiptimer.Start(m_tooltip_delay, wxTIMER_ONE_SHOT);
      }
      catch ( ... ) {
        wxLogWarning( _T("Exception setting tooltip") );
      }
    }
  }
#endif
}

void CustomListCtrl::SetTipWindowText( const long item_hit, const wxPoint position)
{
  int coloumn = getColoumnFromPosition(position);
  if (coloumn >= int(m_colinfovec.size()) || coloumn < 0)
  {
    m_tiptext = _T("");
  }
  else
  {
    m_tiptimer.Start(m_tooltip_delay, wxTIMER_ONE_SHOT);
    m_tiptext = TE(m_colinfovec[coloumn].first);
  }
}

int CustomListCtrl::getColoumnFromPosition(wxPoint pos)
{
    int x_pos = 0;
    for (int i = 0; i < int(m_colinfovec.size());++i)
    {
        x_pos += GetColumnWidth(i);
        if (pos.x < x_pos)
            return i;
    }
    return -1;
}

void CustomListCtrl::OnStartResizeCol(wxListEvent& event)
{
    if (!m_colinfovec[event.GetColumn()].second)
        event.Veto();
}

void CustomListCtrl::OnEndResizeCol(wxListEvent& event)
{
    int column = event.GetColumn();
    int new_size = GetColumnWidth( column );
    sett().SetColumnWidth( m_name, column, new_size );

    //let the event go further
    event.Skip();
}

bool CustomListCtrl::SetColumnWidth(int col, int width)
{
    if ( sett().GetColumnWidth( m_name, col) != Settings::columnWidthUnset)
    {
        return ListBaseType::SetColumnWidth( col, sett().GetColumnWidth( m_name, col) );
    }
    else
    {
        sett().SetColumnWidth( m_name, col, width );
        return ListBaseType::SetColumnWidth( col, width );
    }
}

void CustomListCtrl::noOp(wxMouseEvent& event)
{
    m_tiptext = wxEmptyString;
//            m_tiptimer.Stop();
//            if (controlPointer!= 0 && *controlPointer!= 0)
//            {
//                m_tipwindow->Close();
//                m_tipwindow = 0;
//            }
    event.Skip();
}

void CustomListCtrl::UpdateHighlights()
{
  Freeze();
  try {
      long item = -1;
      while ( true ) {
        item = GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_DONTCARE);
        if ( item == -1 )
          break;
        HighlightItem( item );
      }
  } catch(...) {}
  Thaw();
}

void CustomListCtrl::HighlightItemUser( long item, const wxString& name )
{
   if ( m_highlight && useractions().DoActionOnUser( m_highlightAction, name ) ) {
        wxColor c = sett().GetGroupHLColor( useractions().GetGroupOfUser( name ) );
        SetItemBackgroundColour( item, c );
  }
  else
    SetItemBackgroundColour( item, m_bg_color );
}

void CustomListCtrl::SetHighLightAction( UserActions::ActionType action )
{
    m_highlightAction = action;
}

void CustomListCtrl::MarkDirtySort()
{
  m_dirty_sort = true;
}

void CustomListCtrl::CancelTooltipTimer()
{
    m_tiptimer.Stop();
}

bool CustomListCtrl::PopupMenu(wxMenu* menu, const wxPoint& pos )
{
    CancelTooltipTimer();
    return ListBaseType::PopupMenu( menu, pos );
}
