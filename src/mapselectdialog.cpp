/* Author: Tobi Vollebregt */

#include "mapselectdialog.h"

#include "battle.h"
#include "server.h"
#include "settings.h"
#include "ui.h"
#include "uiutils.h"
#include "utils.h"
#include "settings.h"
#include <wx/settings.h>

//(*InternalHeaders(MapSelectDialog)
#include <wx/listctrl.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/radiobut.h>
#include "mapgridctrl.h"
#include <wx/choice.h>
#include <wx/intl.h>
#include <wx/button.h>
#include <wx/string.h>
//*)

//(*IdInit(MapSelectDialog)
const long MapSelectDialog::ID_STATICTEXT2 = wxNewId();
const long MapSelectDialog::ID_VERTICAL_CHOICE = wxNewId();
const long MapSelectDialog::ID_STATICTEXT1 = wxNewId();
const long MapSelectDialog::ID_HORIZONTAL_CHOICE = wxNewId();
const long MapSelectDialog::ID_FILTER_ALL = wxNewId();
const long MapSelectDialog::ID_FILTER_POPULAR = wxNewId();
const long MapSelectDialog::ID_FILTER_RECENT = wxNewId();
const long MapSelectDialog::ID_FILTER_TEXT = wxNewId();
const long MapSelectDialog::ID_MAP_NAME = wxNewId();
const long MapSelectDialog::ID_MAP_OPTS_LIST = wxNewId();
const long MapSelectDialog::ID_MAPGRID = wxNewId();
//*)
const long MapSelectDialog::ID_VERTICAL_DIRECTION = wxNewId();
const long MapSelectDialog::ID_HORIZONTAL_DIRECTION = wxNewId();

BEGIN_EVENT_TABLE(MapSelectDialog,wxDialog)
	//(*EventTable(MapSelectDialog)
	//*)
END_EVENT_TABLE()

MapSelectDialog::MapSelectDialog(wxWindow* parent,Ui& ui)
	: m_ui(ui)
	, m_horizontal_direction( sett().GetHorizontalSortorder() )
	, m_vertical_direction( sett().GetVerticalSortorder() )
{
	//(*Initialize(MapSelectDialog)
	wxStaticBoxSizer* StaticBoxSizer2;
	wxStaticText* StaticText2;
	wxStaticText* StaticText1;
	wxBoxSizer* BoxSizer2;
	wxBoxSizer* boxSizerHorizontal;
	wxBoxSizer* BoxSizer1;
	wxStaticBoxSizer* StaticBoxSizer1;
	wxBoxSizer* boxSizerVertical;
	wxStdDialogButtonSizer* StdDialogButtonSizer1;

	Create(parent, wxID_ANY, _("Select map"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxMAXIMIZE_BOX, _T("wxID_ANY"));
	BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
	BoxSizer2 = new wxBoxSizer(wxVERTICAL);
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Vertical sort key"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	BoxSizer2->Add(StaticText2, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	boxSizerVertical = new wxBoxSizer(wxHORIZONTAL);
	m_vertical_choice = new wxChoice(this, ID_VERTICAL_CHOICE, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_VERTICAL_CHOICE"));
	boxSizerVertical->Add(m_vertical_choice, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer2->Add(boxSizerVertical, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Horizontal sort key"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	BoxSizer2->Add(StaticText1, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	boxSizerHorizontal = new wxBoxSizer(wxHORIZONTAL);
	m_horizontal_choice = new wxChoice(this, ID_HORIZONTAL_CHOICE, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_HORIZONTAL_CHOICE"));
	boxSizerHorizontal->Add(m_horizontal_choice, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer2->Add(boxSizerHorizontal, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL, this, _("Show"));
	m_filter_all = new wxRadioButton(this, ID_FILTER_ALL, _("All maps"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_FILTER_ALL"));
	m_filter_all->SetToolTip(_("Shows all available maps"));
	StaticBoxSizer1->Add(m_filter_all, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	m_filter_popular = new wxRadioButton(this, ID_FILTER_POPULAR, _("Popular maps"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_FILTER_POPULAR"));
	m_filter_popular->SetToolTip(_("Shows only maps which are currently being player on the server. You must be online to use this."));
	StaticBoxSizer1->Add(m_filter_popular, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	m_filter_recent = new wxRadioButton(this, ID_FILTER_RECENT, _("Recently played maps"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_FILTER_RECENT"));
	m_filter_recent->SetValue(true);
	m_filter_recent->SetToolTip(_("Shows only maps you played recently. (Based on your replays.)"));
	StaticBoxSizer1->Add(m_filter_recent, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	BoxSizer2->Add(StaticBoxSizer1, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer2 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Filter"));
	m_filter_text = new wxTextCtrl(this, ID_FILTER_TEXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_FILTER_TEXT"));
	m_filter_text->SetToolTip(_("Shows only maps which contain this text in their name or description."));
	StaticBoxSizer2->Add(m_filter_text, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	BoxSizer2->Add(StaticBoxSizer2, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_map_details = new wxStaticBoxSizer(wxVERTICAL, this, _("Map details"));
	m_map_name = new wxStaticText(this, ID_MAP_NAME, wxEmptyString, wxDefaultPosition, wxSize(170,90), wxST_NO_AUTORESIZE, _T("ID_MAP_NAME"));
	m_map_name->SetLabel( wxEmptyString );
	m_map_details->Add(m_map_name, 0, wxTOP|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_map_opts_list = new wxListCtrl(this, ID_MAP_OPTS_LIST, wxDefaultPosition, wxSize(170,120), wxLC_REPORT|wxLC_NO_HEADER|wxNO_BORDER, wxDefaultValidator, _T("ID_MAP_OPTS_LIST"));
	m_map_details->Add(m_map_opts_list, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer2->Add(m_map_details, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StdDialogButtonSizer1 = new wxStdDialogButtonSizer();
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_OK, wxEmptyString));
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_CANCEL, wxEmptyString));
	StdDialogButtonSizer1->Realize();
	BoxSizer2->Add(StdDialogButtonSizer1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1->Add(BoxSizer2, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_mapgrid = new MapGridCtrl(this, m_ui, wxSize(600,400), ID_MAPGRID);
	BoxSizer1->Add(m_mapgrid, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	Center();

	Connect(ID_VERTICAL_CHOICE,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&MapSelectDialog::OnSortKeySelect);
	Connect(ID_HORIZONTAL_CHOICE,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&MapSelectDialog::OnSortKeySelect);
	Connect(ID_FILTER_ALL,wxEVT_COMMAND_RADIOBUTTON_SELECTED,(wxObjectEventFunction)&MapSelectDialog::OnFilterAllSelect);
	Connect(ID_FILTER_POPULAR,wxEVT_COMMAND_RADIOBUTTON_SELECTED,(wxObjectEventFunction)&MapSelectDialog::OnFilterPopularSelect);
	Connect(ID_FILTER_RECENT,wxEVT_COMMAND_RADIOBUTTON_SELECTED,(wxObjectEventFunction)&MapSelectDialog::OnFilterRecentSelect);
	Connect(ID_FILTER_TEXT,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&MapSelectDialog::OnFilterTextChanged);
	m_mapgrid->Connect(ID_MAPGRID,wxEVT_LEFT_DCLICK,(wxObjectEventFunction)&MapSelectDialog::OnMapGridLeftDClick,0,this);
	Connect(wxID_ANY,wxEVT_INIT_DIALOG,(wxObjectEventFunction)&MapSelectDialog::OnInit);
	//*)

	Connect(ID_MAPGRID,MapGridCtrl::MapSelectedEvt,(wxObjectEventFunction)&MapSelectDialog::OnMapSelected,0,this);

	// Ugh.. Can not have these created by generated code because wxSmith doesn't accept a symbolic size,
	// (ie. wxSize(CONTROL_HEIGHT,CONTROL_HEIGHT)) and all Set*Size() methods don't seem to have any effect.
	m_vertical_direction_button = new wxButton(this, ID_VERTICAL_DIRECTION, _T("ᴠ"), wxDefaultPosition, wxSize(CONTROL_HEIGHT,CONTROL_HEIGHT), 0, wxDefaultValidator, _T("ID_VERTICAL_DIRECTION"));
	boxSizerVertical->Add(m_vertical_direction_button, 0, wxALL|wxEXPAND|wxSHAPED|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_horizontal_direction_button = new wxButton(this, ID_HORIZONTAL_DIRECTION, _T(">"), wxDefaultPosition, wxSize(CONTROL_HEIGHT,CONTROL_HEIGHT), 0, wxDefaultValidator, _T("ID_HORIZONTAL_DIRECTION"));
	boxSizerHorizontal->Add(m_horizontal_direction_button, 0, wxALL|wxEXPAND|wxSHAPED|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_horizontal_direction_button->SetLabel( m_horizontal_direction ? _T("<") : _T(">") );
    m_vertical_direction_button->SetLabel( m_vertical_direction ? _T("ᴧ") : _T("ᴠ") );
	//<>ᴠᴧ

	Connect(ID_VERTICAL_DIRECTION, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&MapSelectDialog::OnVerticalDirectionClicked);
	Connect(ID_HORIZONTAL_DIRECTION, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&MapSelectDialog::OnHorizontalDirectionClicked);

	// TODO: refactor, this is copied from battlemaptab.cpp
	m_map_opts_list->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	m_map_opts_list->SetFont( wxFont( 8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_LIGHT ) );

	wxListItem col;

	col.SetText( _("Option") );
	m_map_opts_list->InsertColumn( 0, col );
	col.SetText( _("Value") );
	m_map_opts_list->InsertColumn( 1, col );
	m_map_opts_list->SetColumnWidth( 0, 90 );
	m_map_opts_list->SetColumnWidth( 1, 80 );

	m_map_opts_list->InsertItem( 0, _("Size") );
	m_map_opts_list->InsertItem( 1, _("Windspeed") );
	m_map_opts_list->InsertItem( 2, _("Tidal strength") );
	m_map_opts_list->InsertItem( 3, _("Gravity") );
	m_map_opts_list->InsertItem( 4, _("Extractor radius") );
	m_map_opts_list->InsertItem( 5, _("Max metal") );
	m_map_opts_list->InsertItem( 6, _("Start positions") );
}

MapSelectDialog::~MapSelectDialog()
{
	//(*Destroy(MapSelectDialog)
	sett().SetHorizontalSortkeyIndex( m_horizontal_choice->GetSelection() );
	sett().SetVerticalSortkeyIndex( m_vertical_choice->GetSelection() );
	sett().SetHorizontalSortorder( m_horizontal_direction );
	sett().SetVerticalSortorder( m_vertical_direction );
	//*)
}


void MapSelectDialog::OnInit( wxInitDialogEvent& event )
{
	wxLogDebugFunc( _T("") );

	AppendSortKeys( m_horizontal_choice );
	AppendSortKeys( m_vertical_choice );

	m_horizontal_choice->SetSelection( sett().GetHorizontalSortkeyIndex() );
	m_vertical_choice->SetSelection( sett().GetVerticalSortkeyIndex() );

	m_maps = usync().GetMapList();
	usync().GetReplayList( m_replays );

	m_filter_popular->Enable( m_ui.IsConnected() );

	// due to a bug / crappy design in SpringUnitSync / unitsync itself we
	// get a replay list with one empty item when there are no replays..
	if ( m_replays.empty() || ( m_replays.size() == 1 && m_replays[0] == wxEmptyString ) ) {
		m_filter_all->SetValue( true );
		m_filter_recent->Enable( false );
		LoadAll();
	}
	else {
		LoadRecent();
	}

	m_filter_text->SetFocus();
}


void MapSelectDialog::OnSortKeySelect( wxCommandEvent& event )
{
	wxLogDebugFunc( _T("") );
	UpdateSortAndFilter();
}


void MapSelectDialog::AppendSortKeys( wxChoice* choice )
{
	// see MapGridCtrl for available SortKeys
	choice->Append( _("Name"), (void*) MapGridCtrl::SortKey_Name );
	choice->Append( _("Tidal strength"), (void*) MapGridCtrl::SortKey_TidalStrength );
	choice->Append( _("Gravity"), (void*) MapGridCtrl::SortKey_Gravity );
	choice->Append( _("Max metal"), (void*) MapGridCtrl::SortKey_MaxMetal );
	choice->Append( _("Extractor radius"), (void*) MapGridCtrl::SortKey_ExtractorRadius );
	choice->Append( _("Minimum wind"), (void*) MapGridCtrl::SortKey_MinWind );
	choice->Append( _("Maximum wind"), (void*) MapGridCtrl::SortKey_MaxWind );
	choice->Append( _("Average wind"), (void*) MapGridCtrl::SortKey_Wind );
	choice->Append( _("Size (map area)"), (void*) MapGridCtrl::SortKey_Area );
	choice->Append( _("Aspect ratio"), (void*) MapGridCtrl::SortKey_AspectRatio );
	choice->Append( _("Number of start positions"), (void*) MapGridCtrl::SortKey_PosCount );
}

static MapGridCtrl::SortKey GetSelectedSortKey( wxChoice* choice )
{
	return (MapGridCtrl::SortKey) (int) (long) choice->GetClientData( choice->GetSelection() );
}

namespace {
struct FilterPredicate
{
	FilterPredicate( const wxString& searchText ) : searchText(searchText.Lower()) {}
	bool operator () ( const UnitSyncMap& map ) const
	{
		return map.name.Lower().Contains( searchText )
			|| map.info.description.Lower().Contains( searchText )
			|| map.info.author.Lower().Contains( searchText );
	}
	wxString searchText;
};
}

void MapSelectDialog::UpdateSortAndFilter()
{
	FilterPredicate predicate( m_filter_text->GetValue() );
	m_mapgrid->Filter( predicate );
	m_mapgrid->Sort( GetSelectedSortKey( m_vertical_choice ), GetSelectedSortKey( m_horizontal_choice ), m_vertical_direction, m_horizontal_direction );
	m_mapgrid->CheckInBounds();
	m_mapgrid->Refresh();
}

UnitSyncMap* MapSelectDialog::GetSelectedMap() const
{
	wxLogDebugFunc( _T("") );
	return m_mapgrid->GetSelectedMap();
}

void MapSelectDialog::OnMapSelected( wxCommandEvent& event )
{
	const wxString& mapname = event.GetString();

	wxLogDebugFunc( mapname );

	const UnitSyncMap* pMap = m_mapgrid->GetSelectedMap();
	if ( pMap == NULL) return;
	const UnitSyncMap& map = *pMap;

	m_map_name->SetLabel( RefineMapname( map.name ) + _T("\n\n") + map.info.description );

	// TODO: refactor, this is copied from battlemaptab.cpp
	m_map_opts_list->SetItem( 0, 1, wxString::Format( _T("%dx%d"), map.info.width/512, map.info.height/512 ) );
	m_map_opts_list->SetItem( 1, 1, wxString::Format( _T("%d-%d"), map.info.minWind, map.info.maxWind ) );
	m_map_opts_list->SetItem( 2, 1, wxString::Format( _T("%d"), map.info.tidalStrength ) );
	m_map_opts_list->SetItem( 3, 1, wxString::Format( _T("%d"), map.info.gravity ) );
	m_map_opts_list->SetItem( 4, 1, wxString::Format( _T("%d"), map.info.extractorRadius ) );
	m_map_opts_list->SetItem( 5, 1, wxString::Format( _T("%.3f"), map.info.maxMetal ) );
	m_map_opts_list->SetItem( 6, 1, wxString::Format( _T("%d"), map.info.posCount ) );
}

void MapSelectDialog::OnVerticalDirectionClicked( wxCommandEvent& event )
{
	wxLogDebugFunc( _T("") );
	m_vertical_direction = !m_vertical_direction;
	m_vertical_direction_button->SetLabel( m_vertical_direction ? _T("ᴧ") : _T("ᴠ") );
	UpdateSortAndFilter();
}

void MapSelectDialog::OnHorizontalDirectionClicked( wxCommandEvent& event )
{
	wxLogDebugFunc( _T("") );
	m_horizontal_direction = !m_horizontal_direction;
	m_horizontal_direction_button->SetLabel( m_horizontal_direction ? _T("<") : _T(">") );
	UpdateSortAndFilter();
}

void MapSelectDialog::OnMapGridLeftDClick(wxMouseEvent& event)
{
	wxLogDebugFunc( _T("") );

	if ( m_mapgrid->GetSelectedMap() ) {
		EndModal( wxID_OK );
	}
}

void MapSelectDialog::LoadAll()
{
	wxLogDebugFunc( _T("") );
	const int count = m_maps.size();

	m_mapgrid->Clear();

	for ( int i = 0; i < count; ++i ) {
		m_mapgrid->AddMap( m_maps[i] );
	}

	m_mapgrid->Refresh();
}

void MapSelectDialog::LoadPopular()
{
	wxLogDebugFunc( _T("") );

	m_mapgrid->Clear();

	try {
		m_ui.GetServer().battles_iter->IteratorBegin();
		while ( !m_ui.GetServer().battles_iter->EOL() ) {
			Battle* b = m_ui.GetServer().battles_iter->GetBattle();
			if ( b != NULL ) m_mapgrid->AddMap( b->GetHostMapName() );
		}
	}
	catch (...) {} // m_ui.GetServer may throw when disconnected...

	m_mapgrid->Refresh();
}

void MapSelectDialog::LoadRecent()
{
	wxLogDebugFunc( _T("") );
	const int count = m_maps.size();

	m_mapgrid->Clear();

	// just check whether map names are contained in replay names
	// not the most elegant solution but, hey, it works
	for ( int i = 0; i < count; ++i ) {
		// prefix and suffix with underscore to prevent most common partial matches
		const wxString mapname = _T("_") + m_maps[i].BeforeLast( '.' ) + _T("_");
		for (std::vector< wxString >::const_iterator it = m_replays.begin(); it != m_replays.end(); ++it) {
			if ( it->Contains( mapname ) )
				m_mapgrid->AddMap( m_maps[i] );
		}
	}

	m_mapgrid->Refresh();
}

// filter event handlers

void MapSelectDialog::OnFilterAllSelect(wxCommandEvent& event)
{
	wxLogDebugFunc( _T("") );
	LoadAll();
}

void MapSelectDialog::OnFilterPopularSelect(wxCommandEvent& event)
{
	wxLogDebugFunc( _T("") );
	LoadPopular();
}

void MapSelectDialog::OnFilterRecentSelect(wxCommandEvent& event)
{
	wxLogDebugFunc( _T("") );
	LoadRecent();
}

void MapSelectDialog::OnFilterTextChanged(wxCommandEvent& event)
{
	wxLogDebugFunc( _T("") );
	UpdateSortAndFilter();
}
