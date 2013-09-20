/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  Polar Plugin
 * Author:   Konni
 *
 ***************************************************************************
 *   Copyright (C) 2013 by Konni   *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.             *
 ***************************************************************************
 */

#ifndef _POLARPI_H_
#define _POLARPI_H_

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#define     PLUGIN_VERSION_MAJOR    0
#define     PLUGIN_VERSION_MINOR    6

#define     MY_API_VERSION_MAJOR    1
#define     MY_API_VERSION_MINOR    7

#include "../../../include/ocpn_plugin.h"

#include "PolarDialog.h"
#include "Options.h"

//----------------------------------------------------------------------------------------------------------
//    The PlugIn Class Definition
//----------------------------------------------------------------------------------------------------------

#define POLAR_TOOL_POSITION    -1          // Request default positioning of toolbar tool

class polar_pi : public opencpn_plugin_17
{
public:
      polar_pi(void *ppimgr);
      ~polar_pi(void);

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

      void SetDefaults(void);

      int GetToolbarToolCount(void);
      void ShowPreferencesDialog( wxWindow* parent );
      void OnToolbarToolCallback(int id);

      void SetNMEASentence(wxString &sentence);
	  void SetPositionFix(PlugIn_Position_Fix &pfix);

	  void SetPluginMessage(wxString &message_id, wxString &message_body);
//	  void SendPluginMessage(wxString &message_id, wxString &message_body);

// Other public methods

      void SetPolarDir(wxString Polar_dir){ m_Polar_dir = Polar_dir;};
      void SetPolarDialogX    (int x){ m_Polar_dialog_x = x;};
      void SetPolarDialogY    (int x){ m_Polar_dialog_y = x;}
      void SetPolarDialogSizeX(int x){ m_Polar_dialog_sx = x;}
      void SetPolarDialogSizeY(int x){ m_Polar_dialog_sy = x;}
      void SetColorScheme(PI_ColorScheme cs);

      bool GetUseMS(void){ return m_bPolarUseMS; }
      void OnPolarDialogClose();

	  Options			*opt;

private:
      bool LoadConfig(void);
      bool SaveConfig(void);

      wxFileConfig		*m_pconfig;
      wxWindow			*m_parent_window;
	  PolarDialog		*m_pPolarDialog;

      int              m_display_width, m_display_height;
      int              m_leftclick_tool_id;

      bool             m_bShowPolar;

      int              m_Polar_dialog_x, m_Polar_dialog_y;
      int              m_Polar_dialog_sx, m_Polar_dialog_sy;
      wxString         m_Polar_dir;

      bool              m_bPolarShowIcon;
      bool              m_bPolarUseMS;

      //    Controls added to Preferences panel
      wxCheckBox              *m_pPolarShowIcon;
      wxCheckBox              *m_pPolarUseMS;

};

#endif