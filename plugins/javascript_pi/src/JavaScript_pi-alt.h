/* *************************************************************************
* Project:  OpenCPN
* Purpose:  JavaScript Plugin
* Author:   Tony Voss 16/05/2020
*
* Copyright Ⓒ 2020 by Tony Voss
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License, under which
* you must including a copy of these terms in all copies
* https://www.gnu.org/licenses/gpl-3.0.en.html
***************************************************************************/

#ifndef _CALCULATOR_PI_H_
#define _CALCULATOR_PI_H_

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
  #include <wx/glcanvas.h>
#endif //precompiled headers

#include <wx/fileconf.h>
#include <wx/arrimpl.cpp>
#include "duktape.h"
#include "ocpn_duk.h"
#include "../../include/ocpn_plugin.h" //Required for OCPN plugin functions
#include "JavaScriptgui_impl.h"
#include "JavaScriptgui.h"

#include "version.h"

#define     MY_API_VERSION_MAJOR    1
#define     MY_API_VERSION_MINOR    16  // was 7



//----------------------------------------------------------------------------------------------------------
//    The PlugIn Class Definition
//----------------------------------------------------------------------------------------------------------

 #define CALCULATOR_TOOL_POSITION    -1          // Request default positioning of toolbar tool

class Console;

class JavaScript_pi : public opencpn_plugin_116  // was 18
{
public:
      JavaScript_pi(void *ppimgr);
	   ~JavaScript_pi(void);

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

//    The required override PlugIn Methods
      int GetToolbarToolCount(void);
      void OnToolbarToolCallback(int id);
     

//    Optional plugin overrides
      void SetColorScheme(PI_ColorScheme cs);


//    The override PlugIn Methods

//    Other public methods
    //void SetCursorLatLon              (double lat, double lon);
      void SetPluginMessage             (wxString &message_id, wxString &message_body);
     void SetNMEASentence               (wxString &sentence);
    void SetPositionFixEx              (PlugIn_Position_Fix_Ex &pfix);
      void SetCalculatorConsoleX         (int x){ m_route_console_x = x;};
      void SetCalculatorConsoleY         (int x){ m_route_console_y = x;};
     void SetCalculatorConsoleWidth     (int x){ m_route_console_width = x;};
      void SetCalculatorConsoleHeight    (int x){ m_route_console_height = x;};
	  void OnJavaScriptConsoleClose();
    
	  wxWindow         *m_parent_window;
   
	  
private:
      
	  Console               *m_pConsole;
	  wxFileConfig      *m_pconfig;
     
      bool              LoadConfig(void);
      bool              SaveConfig(void);
      
      int               m_route_console_x, m_route_console_y,m_route_console_width,m_route_console_height;
      int               m_display_width, m_display_height;      
      int               m_leftclick_tool_id;
	  bool             m_bJavaScriptShowIcon;
	  bool             m_bShowJavaScript;
	  wxBitmap			m_panelBitmap;
};

#endif
