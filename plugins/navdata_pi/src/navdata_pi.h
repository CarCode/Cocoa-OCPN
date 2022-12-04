/* *************************************************************************
 *
 * Project:  OpenCPN - plugin navdata_pi
 * Purpose:  ROUTE Plugin
 * Author:   Walbert Schulpen (SaltyPaws)
 *
 ***************************************************************************
 *   Copyright (C) 2012-2016 by Walbert Schulpen                           *
 *   $EMAIL$                                                               *
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
 ***************************************************************************/

#ifndef _NAVDATA_PI_H_
#define _NAVDATA_PI_H_

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
  #include <wx/glcanvas.h>
#endif //precompiled headers

#include <wx/fileconf.h>




#ifdef __WXOSX__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Woverloaded-virtual"
#endif
#include "../../../include/ocpn_plugin.h"
#ifdef __WXOSX__
#pragma clang diagnostic pop
#endif

#include "version.h"
#include "routecanv.h"
//---------------------------------------------------------------------------------------------------------
//    The PlugIn Class Definition
//---------------------------------------------------------------------------------------------------------

#ifndef ocpnUSE_SVG
    #define ocpnUSE_SVG 0
#endif

#ifndef PI
#define PI        3.1415926535897931160E0      /* pi */
#endif

#define     MY_API_VERSION_MAJOR    1

#define     MY_API_VERSION_MINOR    16

#define NAVDATA_TOOL_POSITION    -1          // Request default positioning of toolbar tool

#define     TIME_TYPE_UTC 1
#define     TIME_TYPE_LMT 2
#define     TIME_TYPE_COMPUTER 3

#define     TRACKPOINT_FIRST            1
#define     OFFSET_LAT     1e-6

#define     INTERVAL_90MN   5400000  //5400 s 1,5 hour
#define     INTERVAL_HOUR   3600000  //3600 s 1 hour
#define     INTERVAL_20SECOND 20000  //20 s
#define     INTERVAL_10SECOND 10000  //10 s
#define     INTERVAL_4SECOND    4000  //4 s
#define     INTERVAL_3SECOND    3000  //3 s
#define     INTERVAL_2SECOND    2000  //2 s
#define     INTERVAL_1SECOND    1000  //1 s
#define     INTERVAL_10MSECOND   10  //10 ms
#define     IDLE_STATE_NUMBER    99999999

// extern ConsoleCanvas    *m_console;

class navdata_pi : public opencpn_plugin_116, wxTimer
{
public:
      navdata_pi(void *ppimgr);
      ~navdata_pi(void);
//    The required PlugIn Methods
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

private:
      //    The override PlugIn Methods
      bool RenderOverlayMultiCanvas(wxDC &dc, PlugIn_ViewPort *vp, int canvasIndex);
      bool RenderGLOverlayMultiCanvas(wxGLContext *pcontext, PlugIn_ViewPort *vp, int canvasIndex);
      void SetColorScheme(PI_ColorScheme cs);
      bool RenderTargetPoint( wxDC *pdc, PlugIn_ViewPort *vp );
      void SetPluginMessage(wxString &message_id, wxString &message_body);
      int  GetToolbarToolCount(void);
      void OnToolbarToolCallback(int id);
      void SetPositionFix(PlugIn_Position_Fix &pfix);
      bool MouseEventHook( wxMouseEvent &event );
      //
      void LoadocpnConfig();
      float GetSelectRadius(PlugIn_ViewPort *vp);
      void CheckRoutePointSelectable();

      //toolbar variables
      int           m_leftclick_tool_id;
      wxString      m_shareLocn;
      //Route data variables
      bool          m_selectablePoint;
      wxColour      m_defLabelColor;
      int           m_blinkTrigger;
      PlugIn_ViewPort   *m_vp[2];       //allow multi-canvas
      RouteCanvas   *m_console;
      bool          m_isPluginActive;
      //ocpn options variables
      float         m_ocpnSelRadiusMM;
};

#endif //_NAVDATA_PI_H_
