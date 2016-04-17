/***************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  sweepplot Plugin
 * Author:   Sean D'Epagnier
 *
 ***************************************************************************
 *   Copyright (C) 2015 by Sean D'Epagnier                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
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

#ifndef _SWEEPPLOTPI_H_
#define _SWEEPPLOTPI_H_

#include <list>

#include <wx/wx.h>
#include <wx/fileconf.h>

//#include "version.h"

#define     MY_API_VERSION_MAJOR    1
#define     MY_API_VERSION_MINOR    10

#define PLUGIN_VERSION_MAJOR 1
#define PLUGIN_VERSION_MINOR 0
#define PLUGIN_VERSION_DATE "15.02.2016"

#define ABOUT_AUTHOR_URL "http://seandepagnier.users.sourceforge.net"

#include "../../../include/ocpn_plugin.h"

#ifdef __MSVC__
#include <float.h>
#include <iostream>
#include <limits>

# if !defined(M_PI)
# define M_PI		3.14159265358979323846	/* pi */
# endif

# if !defined(NAN)
# define NAN std::numeric_limits<double>::quiet_NaN ()
# endif

# if !defined(INFINITY)
# define INFINITY std::numeric_limits<double>::infinity ()
# endif

#define isnan _isnan
#define isinf(x) (!_finite(x) && !_isnan(x))

inline double trunc(double d){ return (d>0) ? floor(d) : ceil(d) ; }
inline double round(double n) { return n < 0.0 ? ceil(n - 0.5) : floor(n + 0.5); }

# if !defined(snprintf)
# define snprintf _snprintf
# endif
#define vsnprintf _vsnprintf
#define strcasecmp _stricmp
#define strncasecmp _strnicmp

#define strtok_r strtok_s

#endif

#include "../../../src/nmea0183/nmea0183.h"

#include "History.h"

double heading_resolve(double degrees, double ref=0);

//----------------------------------------------------------------------------------------------------------
//    The PlugIn Class Definition
//----------------------------------------------------------------------------------------------------------

#define SWEEPPLOT_TOOL_POSITION    -1          // Request default positioning of toolbar tool

class SweepPlotDialog;
class PreferencesDialog;

class sweepplot_pi : public wxEvtHandler, public opencpn_plugin_113
{
public:
      sweepplot_pi(void *ppimgr);

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

      int GetToolbarToolCount(void);

      void OnToolbarToolCallback(int id);

      bool RenderOverlay(wxDC &dc, PlugIn_ViewPort *vp);
      bool RenderGLOverlay(wxGLContext *pcontext, PlugIn_ViewPort *vp);
      void Render(wxDC *dc, PlugIn_ViewPort &vp);

//    Optional plugin overrides
      void SetColorScheme(PI_ColorScheme cs);

      wxWindow         *m_parent_window;

      SweepPlotDialog   *m_SweepPlotDialog;
      PreferencesDialog *m_Preferences;
      
private:

      bool LoadConfig(void);
      bool SaveConfig(void);
      void WriteHistory();

      wxString StandardPath();

      void SetNMEASentence( wxString &sentence );
      void SetPositionFixEx(PlugIn_Position_Fix_Ex &pfix);

      void UpdatePositionDetermined(enum HistoryEnum speed, enum HistoryEnum course, int tick_diff);

      void AddData(enum HistoryEnum e, double value, time_t ticks=0);
      void OnHistoryWriteTimer( wxTimerEvent & );

      void RearrangeWindow();

      int m_leftclick_tool_id;

      NMEA0183 m_NMEA0183;

      wxTimer m_HistoryWriteTimer;
};

#endif
