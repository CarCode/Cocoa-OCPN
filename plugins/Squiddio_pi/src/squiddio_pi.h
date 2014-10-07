/***************************************************************************
 *
 * Project:  OpenCPN
 * Purpose   Squiddio plugin
 *
 ***************************************************************************
 *   Copyright (C) 2014 by Mauro Calvi                                     *
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

#include "wx/wxprec.h"
#include "wx/dialog.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include <wx/list.h>

#define     PLUGIN_VERSION_MAJOR    0
#define     PLUGIN_VERSION_MINOR    3

#define     MY_API_VERSION_MAJOR    1
#define     MY_API_VERSION_MINOR    10

#include "../../../include/ocpn_plugin.h"
#include "Layer.h"
#include "Hyperlink.h"

//class demoWindow;
class Layer;
class Poi;
class PoiMan;
class NavObjectCollection1;
class wxFileConfig;

extern PoiMan *pPoiMan;


//----------------------------------------------------------------------------------------------------------
//    The PlugIn Class Definition
//----------------------------------------------------------------------------------------------------------


class squiddio_pi : public opencpn_plugin_110
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

      void RenderLayerContentsOnChart(Layer *layer, bool save_config = true);

      void OnContextMenuItemCallback(int id);
      void UpdateAuiStatus(void);

//    The override PlugIn Methods
      bool RenderOverlay(wxDC &dc, PlugIn_ViewPort *vp);
      void SetCursorLatLon(double lat, double lon);
      bool RenderGLOverlay(wxGLContext *pcontext, PlugIn_ViewPort *vp);
      int GetToolbarToolCount(void);
      void ShowPreferencesDialog( wxWindow* parent );
      void OnToolbarToolCallback(int id);
      void SetPluginMessage(wxString &message_id, wxString &message_body);
      void SetPositionFixEx(PlugIn_Position_Fix_Ex &pfix);
      void appendOSDirSlash( wxString* pString );
      bool IsOnline(void);
      wxString DownloadLayer(void);
      bool SaveLayer(wxString,wxString);
      Layer * GetLocalLayer(void);
      Layer * LoadLayer(wxString, wxString);
      void ReportDestination(double lat, double lon);
      bool LoadLayers(wxString &path);
      bool LoadLayerItems(wxString & path, Layer *l, bool show);

      bool ShowPOI(Poi* wp);
      bool HidePOI(Poi* wp);
      // todo can the follwoign be moved to private?
      double m_cursor_lat, m_cursor_lon;
      Layer *local_sq_layer;



private:
      bool LoadConfig(void);
      bool SaveConfig(void);

      wxWindow         *m_parent_window;
      wxAuiManager     *m_AUImgr;
      int              	m_show_id;
      int              	m_hide_id;
      int 				m_update_id;
      int 				m_report_id;
      bool 				isLayerUpdate;
      wxString 			local_region;
      wxString			layerdir;
      wxFileConfig     *m_pconfig;
      LayerList 	   *pLayerList;
      wxString			g_VisibleLayers;
      wxString			g_InvisibleLayers;

      int 		 		g_LayerIdx;
      bool				g_bShowLayers;
      Plugin_Hyperlink *link;
      Hyperlink 	   *wp_link;
      
      long              last_online_chk;
      bool              last_online;
};

#endif






