/* ************************************************************************
 *
 * Project:  OpenCPN
 * Purpose   Squiddio plugin
 *
 ***************************************************************************
 *   Copyright (C) 2020 by Mauro Calvi                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.         *
 ***************************************************************************/

#ifndef _SQUIDDIOPI_H_
#define _SQUIDDIOPI_H_

#ifdef __WXMSW__
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <sstream>
#  define DEBUGSL(x) do { \
std::ostringstream oss; \
oss << x; \
time_t now = time(0); \
tm* localtm = localtime(&now); \
char *stime = asctime(localtm); \
stime[strlen(stime) - 1 ] = 0; \
std::string s1(oss.str()); \
std::string s = stime; \
s += " :: "; \
s += s1; \
s += "\n"; \
std::wstring stemp = std::wstring(s.begin(), s.end()); \
LPCWSTR sw = stemp.c_str(); \
OutputDebugString(sw); } while (0) 

#  define DEBUGST(x) do { \
std::string s(""); \
std::ostringstream oss; \
oss << x; \
time_t now = time(0); \
tm* localtm = localtime(&now); \
char *stime = asctime(localtm); \
stime[strlen(stime) - 1 ] = 0; \
do { \
std::string s1(oss.str()); \
s += stime; \
s += " :: "; \
s += s1; } while (0);

#  define DEBUGCONT(x) do { \
std::ostringstream oss; \
oss << x; \
std::string s1(oss.str()); \
s += s1 ; } while (0);

#  define DEBUGEND(x) do { \
std::string s1(""); \
std::ostringstream oss; \
oss << x; \
s1 = oss.str(); \
s += s1; } while (0); \
s += "\n" ; \
std::wstring stemp = std::wstring(s.begin(), s.end()); \
LPCWSTR sw = stemp.c_str(); \
OutputDebugString(sw); } while (0) 
#else
#  define DEBUGSL(x) do {} while (0)
#  define DEBUGST(x) do {} while (0)
#  define DEBUGCONT(x) do {} while (0)
#  define DEBUGEND(x) do {} while (0)
#endif
#else
#ifdef DEBUG_BUILD
#  define DEBUGSL(x) do { \
time_t now = time(0); \
tm* localtm = localtime(&now); \
char *stime = asctime(localtm); \
stime[strlen(stime) - 1 ] = 0; \
std::cout << stime << " :: "; \
std::cout << x << std::endl ;} while (0)

#  define DEBUGST(x) do { \
time_t now = time(0); \
tm* localtm = localtime(&now); \
char *stime = asctime(localtm); \
stime[strlen(stime) - 1 ] = 0; \
std::cout << stime << " :: " ; \
std::cout << x; } while (0)

#  define DEBUGCONT(x) do { \
std::cout << x ; } while (0)

#  define DEBUGEND(x) do { \
std::cout << x  << std::endl ; } while (0)
#else
#  define DEBUGSL(x) do {} while (0)
#  define DEBUGST(x) do {} while (0)
#  define DEBUGCONT(x) do {} while (0)
#  define DEBUGEND(x) do {} while (0)
#endif
#endif

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#define     MY_API_VERSION_MAJOR    1
#define     MY_API_VERSION_MINOR    16

#include "version.h"
#include "squiddio_pi_thread.h"
#include <wx/list.h>
#include <wx/sstream.h>
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/fileconf.h>
#include <wx/listimpl.cpp>
#include <wx/aui/aui.h>
#include <wx/aui/framemanager.h>

#include "../../../include/ocpn_plugin.h"
#include "icons.h"
#include "Layer.h"
#include "Hyperlink.h"
#include "PoiMan.h"
#include "Poi.h"
#include "NavObjectCollection.h"
#include "squiddioPrefsDialogBase.h"
#include "../../../src/nmea0183/nmea0183.h"
#include "logs.h"
#include "ODAPI.h"

#define OCPN_WAYPOINTS true
#define OD_TEXTPOINTS false

class logsWindow;
class Layer;
class Poi;
class PoiMan;
class NavObjectCollection1;
class wxFileConfig;
class wxFontDialog;

extern PoiMan *pPoiMan;

int period_secs(int period);

#define SQUIDDIO_TOOL_POSITION    -1  // Request default positioning of toolbar tool

//----------------------------------------------------------------------------------------------------------
//    The PlugIn Class Definition
//----------------------------------------------------------------------------------------------------------

typedef void (wxEvtHandler::*myEventFunction)(SquiddioEvent&);

class squiddio_pi : public opencpn_plugin_116, public wxEvtHandler
{
public:
      squiddio_pi(void *ppimgr);
      ~squiddio_pi(void);

//    The required PlugIn Methods
      int Init(void);
      bool DeInit(void);

      int GetAPIVersionMajor();
      int GetAPIVersionMinor();
      int GetPlugInVersionMajor();
      int GetPlugInVersionMinor();
      wxBitmap * GetPlugInBitmap();

      wxString GetCommonName();
      wxString GetShortDescription();
      wxString GetLongDescription();

//    The optional method overrides

      void RenderLayerContentsOnChart(Layer *layer, bool save_config = false, bool hidePOI = false);

      void OnContextMenuItemCallback(int id);
      void UpdateAuiStatus(void);

//    The override PlugIn Methods
      bool RenderOverlay(wxDC &dc, PlugIn_ViewPort *vp);
      bool RenderGLOverlay(wxGLContext *pcontext, PlugIn_ViewPort *vp);
      bool RenderOverlayMultiCanvas(wxDC &dc, PlugIn_ViewPort *vp, int canvasIndex);
      bool RenderGLOverlayMultiCanvas(wxGLContext *pcontext, PlugIn_ViewPort *vp, int canvasIndex);

      void SetCursorLatLon(double lat, double lon);
      int GetToolbarToolCount(void);
      bool LoadConfig(void);

      void ShowPreferencesDialog( wxWindow* parent );
      void OnToolbarToolCallback(int id);
      void SetPluginMessage(wxString &message_id, wxString &message_body);
      void SetPositionFixEx(PlugIn_Position_Fix_Ex &pfix);
      void appendOSDirSlash( wxString* pString );
      wxString DownloadLayer(wxString url_path);
      bool SaveLayer(wxString,wxString);
      Layer * GetLocalLayer(void);
      Layer * LoadLayer(wxString, wxString);
      bool LoadLayers(wxString &path);
      bool LoadLayerItems(wxString & path, Layer *l, bool show);
      bool ShowPOI(Poi* wp);
      bool HidePOI(Poi* wp);

      void RenderLayers(bool hidePOI = false);

      void SetNMEASentence(wxString &sentence);
      void SetLogsWindow();
      void LateInit(void);
      
      void ResetODAPI(void);

      PlugIn_ViewPort  *m_vp;

      wxString   layerdir;
      LayerList  *pLayerList;
      wxString   g_Email;
      wxString   g_ApiKey;
      wxString   g_DomainName;
      wxString   g_UrlVersion;
      int        g_LastLogSent;
      int        g_LastLogsRcvd;
      bool       g_SendXml;
      bool       g_OCPN;

      bool  m_bODAPIOK;
      int   m_iODAPIVersionMajor;
      int   m_iODAPIVersionMinor;

      double    m_cursor_lat, m_cursor_lon;
      Layer     *local_sq_layer;
      int       g_PostPeriod;
      int       g_RetrievePeriod;

      wxFont      g_fontODDisplayTextFont;

      void OnThreadActionFinished(SquiddioEvent& event);
      void SetThreadRunning( bool state ) { m_bThreadRuning = state; }
      bool IsThreadRunning() { return m_bThreadRuning; }
      bool CheckIsOnline();
      void RefreshLayer();

private:
      SquiddioThread *m_pThread;
      wxCriticalSection m_pThreadCS; // protects the m_pThread pointer
      friend class SquiddioThread; // allow it to access our m_pThread

      bool SaveConfig(void);
      bool ShowType(Poi * wp);
      void PreferencesDialog(wxWindow* parent);
      void GetODAPI(void);
      void AddODIcons(void);
      void SwitchPointType(bool bPointType, bool Changed);
      void DownloadSatImages();
      bool ProcessZipFile(wxString chart_dir, wxString tmp_file);
      bool UnzipFile(const wxString& aZipFile, const wxString& aTargetDir);
      void MoveDataDir(wxString old_dir, wxString new_dir);

//       bool UnzipFile(const wxString& aZipFile, const wxString& aTargetDir);
      bool IsPOIinLayer(int layer_id);

      wxWindow      *m_parent_window;
      int           m_show_id;
      int           m_hide_id;
      int           m_retrieve_id;
      int           m_update_id;
      int           m_report_id;
      int           m_download_id;
      bool          isLayerUpdate;
      wxString      local_region;
      wxString      m_rgn_to_dld;
      wxString      g_InvisibleLayers;
      wxString      g_BaseChartDir, g_InitChartDir,g_ZoomLevels;
      int           g_LayerIdx;
      bool          g_bShowLayers, g_DownloadVPMap, g_DelGpxs;

      wxAuiManager     *m_AUImgr;
      wxFileConfig     *m_pconfig;
      Plugin_Hyperlink *link;
      Hyperlink        *wp_link;

      bool        g_ViewMarinas;
      bool        g_ViewAnchorages;
      bool        g_ViewDocks;
      bool        g_ViewYachtClubs;
      bool        g_ViewMoorings;
      bool        g_ViewFuelStations;
      bool        g_ViewBoatYards;
      bool        g_ViewRamps;
      bool        g_ViewAIS;
      bool        g_ViewNDBC;
      bool        g_ViewShipRep;
      bool        g_ViewOthers;

      bool        g_bODTextPointShowName;
      int         g_iODTextPointTextPosition;
      wxColour    g_colourODDefaultTextColour;
      wxColour    g_colourODDefaultTextBackgroundColour;
      int         g_iODTextBackgroundTransparency;
      int         g_iTextPointDisplayTextWhen;

      logsWindow  *m_plogs_window;
      int         m_squiddio_dialog_x, m_squiddio_dialog_y;
      int         m_demoshow_id;
      int         m_demohide_id;
      int         m_leftclick_tool_id;

      bool  m_bThreadRuning;

      int   m_iODVersionMajor;
      int   m_iODVersionMinor;
      int   m_iODVersionPatch;
      int   m_display_width, m_display_height;

      bool  m_bDoneODVersionCall;
      bool  m_bDoneODAPIVersionCall;
      bool  m_bODAPIMessageShown;
      bool  m_bODFindPointInAnyBoundary;
      bool  m_bODFindClosestBoundaryLineCrossing;
      bool  m_bODFindFirstBoundaryLineCrossing;
      bool  m_bODCreateBoundary;
      bool  m_bODCreateBoundaryPoint;
      bool  m_bODCreateTextPoint;
      bool  m_bODDeleteTextPoint;
      bool  m_bODAddPointIcon;
      bool  m_bODDeletePointIcon;
      OD_FindPointInAnyBoundary             m_pOD_FindPointInAnyBoundary;
      OD_FindClosestBoundaryLineCrossing    m_pODFindClosestBoundaryLineCrossing;
      OD_FindFirstBoundaryLineCrossing      m_pODFindFirstBoundaryLineCrossing;
      OD_CreateBoundary                     m_pODCreateBoundary;
      OD_CreateBoundaryPoint                m_pODCreateBoundaryPoint;
      OD_CreateTextPoint                    m_pODCreateTextPoint;
      OD_DeleteTextPoint                    m_pODDeleteTextPoint;
      OD_AddPointIcon                       m_pODAddPointIcon;
      OD_DeletePointIcon                    m_pODDeletePointIcon;

      DECLARE_EVENT_TABLE()
};

class SquiddioPrefsDialog : public SquiddioPrefsDialogBase
{
public:
    SquiddioPrefsDialog( squiddio_pi &_sq_pi, wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Squiddio Preferences"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 250,495 ), long style = wxCAPTION|wxDEFAULT_DIALOG_STYLE|wxTAB_TRAVERSAL )
        : SquiddioPrefsDialogBase( parent, id, title, pos, size, style ), m_sq_pi(_sq_pi) {}

    void OnCheckBoxAll( wxCommandEvent& event );
    void LaunchHelpPage( wxCommandEvent& event );
    void OnSendXml( wxCommandEvent& event );
    void OnButtonClickFonts( wxCommandEvent& event );
    void m_sdbButtonSizerOnCancelButtonClick( wxCommandEvent& event ) { EndModal(wxCANCEL); }
    void m_sdbButtonSizerOnOKButtonClick( wxCommandEvent& event ) { EndModal(wxOK); }

    wxFontDialog  *m_pfdDialog;

protected:
    squiddio_pi &m_sq_pi;
};

#endif
