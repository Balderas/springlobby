/* Author: Tobi Vollebregt */

#ifndef SPRINGLOBBY_HEADERGUARD_MAPGRIDCTRL_H
#define SPRINGLOBBY_HEADERGUARD_MAPGRIDCTRL_H

#include "iunitsync.h"
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/panel.h>

class Ui;


class MapGridCtrl : public wxPanel
{
	public:

		static const wxEventType MapSelectedEvt;

		enum SortKey
		{
			SortKey_Name,
			SortKey_TidalStrength,
			SortKey_Gravity,
			SortKey_MaxMetal,
			SortKey_ExtractorRadius,
			SortKey_MinWind,
			SortKey_MaxWind,
			SortKey_Wind,        // minWind + maxWind
			SortKey_Area,        // width * height
			SortKey_AspectRatio, // max(width/height, height/width)
			SortKey_PosCount,
		};

		MapGridCtrl( wxWindow* parent, Ui& ui, wxSize size = wxDefaultSize, wxWindowID id = -1 );
		~MapGridCtrl();

		void Clear();
		void AddMap( const wxString& mapname );
		void AddMap( const UnitSyncMap& map );

		void CheckInBounds();

		/* ===== sorting ===== */
		void Sort( SortKey vertical, SortKey horizontal, bool vertical_direction = false, bool horizontal_direction = false );

		/* ===== filtering ===== */
		template< class Predicate > int Filter( Predicate pred )
		{
			std::vector< wxString > maps;

			m_maps_filtered.insert( m_maps.begin(), m_maps.end() );
			m_maps_unused.insert( m_maps.begin(), m_maps.end() );
			m_maps.clear();
			m_grid.clear();
			m_mouseover_map = NULL; // can't be sure pointer will stay valid
			m_selected_map = NULL;


			for (MapMap::iterator it = m_maps_filtered.begin(); it != m_maps_filtered.end(); ++it) {
				if ( pred( it->second ) ) maps.push_back( it->first );
			}

			for (std::vector< wxString >::iterator it = maps.begin(); it != maps.end(); ++it) {
				AddMap( *it );
				m_maps_filtered.erase( *it );
			}

			return m_maps.size();
		}

		UnitSyncMap* GetSelectedMap() const { return m_selected_map; }

		void OnPaint( wxPaintEvent& event );
		void OnResize( wxSizeEvent& event );

		void OnMouseMove( wxMouseEvent& event );
		void OnLeftDown( wxMouseEvent& event );
		void OnLeftUp( wxMouseEvent& event );

		void OnGetMapImageAsyncCompleted( wxCommandEvent& event );

	protected:

		enum MapState
		{
			MapState_NoMinimap,
			MapState_GetMinimap,
			MapState_GotMinimap
		};

		struct MapData : UnitSyncMap
		{
			MapData() : state( MapState_NoMinimap ) {}
			void operator=( const UnitSyncMap& other ) { UnitSyncMap::operator=( other ); }

			wxBitmap minimap;
			MapState state;
		};

		typedef std::map< wxString, MapData > MapMap;

		// wrapper around the Compare*() methods below to allow changing sort direction
		template< class Compare > class _Compare2
		{
			public:
				_Compare2( bool direction, Compare cmp )
					: m_cmp( cmp ), m_direction( direction ? -1 : 1 ) {}
				bool operator()( const MapData* a, const MapData* b ) {
					return (m_direction * m_cmp( a, b )) < 0;
				}
			private:
				Compare m_cmp;
				int m_direction;
		};

		// allow a _Compare2 to be constructed with implicit template arguments
		template< class Compare > _Compare2< Compare > _Compare( bool direction, Compare cmp ) {
			return _Compare2< Compare >( direction, cmp );
		}

		// comparison methods returning -1 if a < b, 1 if a > b and 0 if !(a < b) && !(a > b)
		static int CompareName( const MapData* a, const MapData* b );
		static int CompareTidalStrength( const MapData* a, const MapData* b );
		static int CompareGravity( const MapData* a, const MapData* b );
		static int CompareMaxMetal( const MapData* a, const MapData* b );
		static int CompareExtractorRadius( const MapData* a, const MapData* b );
		static int CompareMinWind( const MapData* a, const MapData* b );
		static int CompareMaxWind( const MapData* a, const MapData* b );
		static int CompareWind( const MapData* a, const MapData* b );
		static int CompareArea( const MapData* a, const MapData* b );
		static int CompareAspectRatio( const MapData* a, const MapData* b );
		static int ComparePosCount( const MapData* a, const MapData* b );
		template< class Compare > void _Sort( int dimension, Compare cmp );

		void UpdateGridSize();
		void UpdateToolTip();
		void UpdateAsyncFetches();
		void FetchMinimap( MapData& map );
		void DrawMap( wxDC& dc, MapData& map, int x, int y );
		void DrawBackground( wxDC& dc );
		void SetMinimap( MapMap& maps, const wxString& mapname, const wxBitmap& minimap );

		Ui& m_ui;
		UnitSyncAsyncOps m_async;

		MapMap m_maps;
		MapMap m_maps_unused;
		MapMap m_maps_filtered;
		std::vector< MapData* > m_grid;
		wxSize m_size;

		wxPoint m_pos;
		wxPoint m_first_mouse_pos;
		wxPoint m_last_mouse_pos;
		bool m_in_mouse_drag;

		/// Number of async minimap fetches still running on behalf of this control.
		/// This number is limited so the control can adapt (faster) to changes in
		/// the set of visible maps.  (it fetches only visible maps)
		int m_async_minimap_fetches;

		/// Map which is currently under the mouse pointer.
		MapData* m_mouseover_map;
		/// Map which was last clicked.
		MapData* m_selected_map;

		wxImage m_img_background;
		wxImage m_img_minimap_alpha;
		wxImage m_img_foreground;

		DECLARE_EVENT_TABLE();
};

#endif // SPRINGLOBBY_HEADERGUARD_MAPGRIDCTRL_H
