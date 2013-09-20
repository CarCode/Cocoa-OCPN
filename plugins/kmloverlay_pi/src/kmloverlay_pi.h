/***************************************************************************
 * $Id: kmloverlay_pi.h, v0.1 2012-01-20 SethDart Exp $
 *
 * Project:  OpenCPN
 * Purpose:  KML overlay plugin
 * Author:   Jean-Eudes Onfray
 *
 ***************************************************************************
 *   Copyright (C) 2012 by Jean-Eudes Onfray                               *
 *   je@onfray.fr                                                          *
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************
 */

#ifndef _KMLOverlayPI_H_
#define _KMLOverlayPI_H_

#include <wx/wxprec.h>

#ifndef  WX_PRECOMP
  #include <wx/wx.h>
#endif //precompiled headers

#define     PLUGIN_VERSION_MAJOR    0
#define     PLUGIN_VERSION_MINOR    2

#define     MY_API_VERSION_MAJOR    1
#define     MY_API_VERSION_MINOR    7

#include <wx/aui/aui.h>
#include <wx/fileconf.h>
#include "../../../include/ocpn_plugin.h"
#include "ui.h"

#define     KMLOVERLAY_TOOL_POSITION -1          // Request default positioning of toolbar tool

//----------------------------------------------------------------------------------------------------------
//    The plugin class definition
//----------------------------------------------------------------------------------------------------------

class kmloverlay_pi : public opencpn_plugin_17
{
public:
      kmloverlay_pi(void *ppimgr);

      int Init(void);
      bool DeInit(void);

      int GetAPIVersionMajor();
      int GetAPIVersionMinor();
      int GetPlugInVersionMajor();
      int GetPlugInVersionMinor();
      wxBitmap *GetPlugInBitmap();
      wxString GetCommonName();
      wxString GetShortDescription();
      wxString GetLongDescription();
      int GetToolbarToolCount(void);
      void OnToolbarToolCallback(int id);

      void SetColorScheme( PI_ColorScheme cs );
      bool RenderOverlay( wxDC &dc, PlugIn_ViewPort *vp );
      bool RenderGLOverlay( wxGLContext *pcontext, PlugIn_ViewPort *vp );

      void ShowPreferencesDialog( wxWindow* parent );

private:
      bool LoadConfig(void);
      bool SaveConfig(void);
      void ApplyConfig(void);

      wxFileConfig    *m_pconfig;
      wxAuiManager    *m_pauimgr;
      int              m_toolbar_item_id;
      KMLOverlayUI    *m_puserinput;
      int              m_interval;

};

#endif
