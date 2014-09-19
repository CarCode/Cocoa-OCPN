/*
    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA  02110-1301, USA.

    ---
    Copyright (C) 2010, Anders Lund <anders@alweb.dk>
 */

#ifndef _RouteManagerDialog_h_
#define _RouteManagerDialog_h_

#include <wx/dialog.h>
#include <wx/timer.h>
#include <wx/notebook.h>
#include <wx/panel.h>
#include <wx/listctrl.h>
#include <wx/notebook.h>

enum {
      SORT_ON_DISTANCE  = 1,
      SORT_ON_NAME
};

enum TrackContextMenu {
      TRACK_MERGE  = 1,
      TRACK_COPY_TEXT,
      TRACK_CLEAN
};

class wxButton;
class Route;
class Track;
class Layer;
class RoutePoint;

class RouteManagerDialog : public wxDialog {
      DECLARE_EVENT_TABLE()

      public:
            RouteManagerDialog(wxWindow *parent);
            ~RouteManagerDialog();
            void SetColorScheme();
            void UpdateRouteListCtrl();     // Rebuild route list
            void UpdateTrkListCtrl();
            void UpdateWptListCtrl(RoutePoint *rp_select = NULL, bool b_retain_sort = false);
            void UpdateLayListCtrl();
            void UpdateWptListCtrlViz();
            
            void OnTabSwitch(wxNotebookEvent& event);
            static void WptShowPropertiesDialog( RoutePoint* wp, wxWindow* parent );
            void TrackToRoute( Track *track );

      private:
            void Create();
            void UpdateRteButtons();           // Correct button state
            void MakeAllRoutesInvisible();  // Mark all routes as invisible. Does not flush settings.
            void ZoomtoRoute(Route *route); // Attempt to zoom route into the view
            void UpdateTrkButtons();           // Correct button state
            void UpdateWptButtons();           // Correct button state
            void UpdateLayButtons();           // Correct button state
            void ToggleLayerContentsOnChart(Layer *layer);
            void ToggleLayerContentsOnListing(Layer *layer);
            void ToggleLayerContentsNames(Layer *layer);

            // event handlers
            void OnRteDeleteClick(wxCommandEvent &event);
            void OnRtePropertiesClick(wxCommandEvent &event);
            void OnRteZoomtoClick(wxCommandEvent &event);
            void OnRteActivateClick(wxCommandEvent &event);
            void OnRteReverseClick(wxCommandEvent &event);
            void OnRteExportClick(wxCommandEvent &event);
            void OnRteToggleVisibility(wxMouseEvent &event);
            void OnRteBtnLeftDown(wxMouseEvent &event); // record control key state for some action buttons
            void OnRteDeleteAllClick(wxCommandEvent &event);
            void OnRteSelected(wxListEvent &event);
            void OnRteSendToGPSClick(wxCommandEvent &event);
            void OnRteDefaultAction(wxListEvent &event);
            void OnRteColumnClicked(wxListEvent &event);
            void OnTrkDefaultAction(wxListEvent &event);
            void OnTrkNewClick(wxCommandEvent &event);
            void OnTrkPropertiesClick(wxCommandEvent &event);
            void OnTrkDeleteClick(wxCommandEvent &event);
            void OnTrkExportClick(wxCommandEvent &event);
            void OnTrkRouteFromTrackClick(wxCommandEvent &event);
            void OnTrkDeleteAllClick(wxCommandEvent &event);
            void OnTrkSelected(wxListEvent &event);
            void OnTrkToggleVisibility(wxMouseEvent &event);
            void OnTrkColumnClicked(wxListEvent &event);
            void OnTrkRightClick(wxListEvent &event);
            void OnTrkMenuSelected(wxCommandEvent &event);
            void OnWptDefaultAction(wxListEvent &event);
            void OnWptNewClick(wxCommandEvent &event);
            void OnWptPropertiesClick(wxCommandEvent &event);
            void OnWptZoomtoClick(wxCommandEvent &event);
            void OnWptDeleteClick(wxCommandEvent &event);
            void OnWptGoToClick(wxCommandEvent &event);
            void OnWptExportClick(wxCommandEvent &event);
            void OnWptSendToGPSClick(wxCommandEvent &event);
            void OnWptDeleteAllClick(wxCommandEvent &event);
            void OnWptSelected(wxListEvent &event);
            void OnWptToggleVisibility(wxMouseEvent &event);
            void OnWptColumnClicked(wxListEvent &event);
            void OnLayDefaultAction(wxListEvent &event);
            void OnLayNewClick(wxCommandEvent &event);
            void OnLayPropertiesClick(wxCommandEvent &event);
            void OnLayToggleChartClick(wxCommandEvent &event);
            void OnLayToggleListingClick(wxCommandEvent &event);
            void OnLayToggleNamesClick(wxCommandEvent &event);
            void OnLayDeleteClick(wxCommandEvent &event);
            void OnLaySelected(wxListEvent &event);
            void OnLayToggleVisibility(wxMouseEvent &event);
            void OnLayColumnClicked(wxListEvent &event);
            void OnImportClick(wxCommandEvent &event);
            void OnExportClick(wxCommandEvent &event);
            void OnExportVizClick(wxCommandEvent &event);
            
            // properties
            wxNotebook *m_pNotebook;
            wxPanel    *m_pPanelRte;
            wxPanel    *m_pPanelTrk;
            wxPanel    *m_pPanelWpt;
            wxPanel     *m_pPanelLay;
            wxListCtrl *m_pRouteListCtrl;
            wxListCtrl *m_pTrkListCtrl;
            wxListCtrl *m_pWptListCtrl;
            wxListCtrl  *m_pLayListCtrl;

            wxButton *btnRteProperties;
            wxButton *btnRteActivate;
            wxButton *btnRteZoomto;
            wxButton *btnRteReverse;
            wxButton *btnRteDelete;
            wxButton *btnRteExport;
            wxButton *btnRteSendToGPS;
            wxButton *btnRteDeleteAll;
            wxButton *btnTrkNew;
            wxButton *btnTrkProperties;
            wxButton *btnTrkDelete;
            wxButton *btnTrkExport;
            wxButton *btnTrkRouteFromTrack;
            wxButton *btnTrkDeleteAll;
            wxButton *btnWptNew;
            wxButton *btnWptProperties;
            wxButton *btnWptZoomto;
            wxButton *btnWptDelete;
            wxButton *btnWptGoTo;
            wxButton *btnWptExport;
            wxButton *btnWptSendToGPS;
            wxButton *btnWptDeleteAll;
            wxButton *btnLayNew;
            //wxButton *btnLayProperties;
            wxButton *btnLayToggleChart;
            wxButton *btnLayToggleListing;
            wxButton *btnLayToggleNames;
            wxButton *btnLayDelete;
            wxButton *btnImport;
            wxButton *btnExport;
            wxButton *btnExportViz;
            
            bool m_bPossibleClick;    // do
            bool m_bCtrlDown;         // record control key state for some action buttons
            bool m_bNeedConfigFlush;  // if true, update config in destructor

            int m_lastWptItem;
            int m_lastTrkItem;
            int m_lastRteItem;
};

#endif // _RouteManagerDialog_h_
// kate: indent-width 6; indent-mode cstyle; space-indent on;
