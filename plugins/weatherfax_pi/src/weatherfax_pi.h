/***************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  weather fax Plugin
 * Author:   Sean D'Epagnier
 *
 ***************************************************************************
 *   Copyright (C) 2013 by Sean D'Epagnier                                 *
 *   sean at depagnier dot com                                             *
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

#ifndef _WEATHERFAXPI_H_
#define _WEATHERFAXPI_H_

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include <wx/fileconf.h>

#define     PLUGIN_VERSION_MAJOR    0
#define     PLUGIN_VERSION_MINOR    9

#define     MY_API_VERSION_MAJOR    1
#define     MY_API_VERSION_MINOR    8

#include "../../../include/ocpn_plugin.h"

#include "defs.h"

//----------------------------------------------------------------------------------------------------------
//    The PlugIn Class Definition
//----------------------------------------------------------------------------------------------------------

#define WEATHERFAX_TOOL_POSITION    -1          // Request default positioning of toolbar tool

class WeatherFaxImage;
class WeatherFax;

class weatherfax_pi : public opencpn_plugin_18
{
public:
      weatherfax_pi(void *ppimgr);


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

//    Optional plugin overrides
      void SetColorScheme(PI_ColorScheme cs);

//    Other public methods
      void SetWeatherFaxX    (int x){ m_weatherfax_dialog_x = x;}
      void SetWeatherFaxY    (int x){ m_weatherfax_dialog_y = x;}

      void OnWeatherFaxClose();

      static wxString StandardPath();

      wxWindow         *m_parent_window;

      wxString m_path;

      /* audio decoding options */
      int m_ImageWidth, m_BitsPerPixel, m_Carrier, m_Deviation, m_Filter;
      bool m_bSkipHeaderDetection, m_bIncludeHeadersInImage;

      wxFileConfig     *m_pconfig;

      PlugIn_Position_Fix_Ex m_lastfix;

      WeatherFax      *m_pWeatherFax;

private:
      void ReadImageToRender(WeatherFaxImage *image);

      bool              LoadConfig(void);
      bool              SaveConfig(void);

      void SetPositionFixEx(PlugIn_Position_Fix_Ex &pfix);
      void ShowPreferencesDialog( wxWindow* parent );

      int               m_weatherfax_dialog_x, m_weatherfax_dialog_y;
      int               m_display_width, m_display_height;

      int               m_leftclick_tool_id;

      void              RearrangeWindow();
      wxString          m_weatherfax_dir;
};

#endif
