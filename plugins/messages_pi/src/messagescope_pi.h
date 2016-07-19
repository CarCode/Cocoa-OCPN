/***************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  Message Scope Plugin
 * Author:   Sean D'Epagnier
 *
 ***************************************************************************
 *   Copyright (C) 2016 by Sean D'Epagnier                                 *
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

#ifndef _MESSAGESCOPEPI_H_
#define _MESSAGESCOPEPI_H_

#include <wx/wx.h>
#include <wx/fileconf.h>

//#include "version.h"

#define     PLUGIN_VERSION_MAJOR    0
#define     PLUGIN_VERSION_MINOR    1

#define     MY_API_VERSION_MAJOR    1
#define     MY_API_VERSION_MINOR    8

#define ABOUT_AUTHOR_URL "http://seandepagnier.users.sourceforge.net"

#include "../../../include/ocpn_plugin.h"

//----------------------------------------------------------------------------------------------------------
//    The PlugIn Class Definition
//----------------------------------------------------------------------------------------------------------

#define MESSAGESCOPE_TOOL_POSITION    -1          // Request default positioning of toolbar tool

class MessageScopeDialog;
class messagescope_pi : public opencpn_plugin_18, public wxEvtHandler
{
public:
      messagescope_pi(void *ppimgr);
      ~messagescope_pi();

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

//    Optional plugin overrides
      void SetColorScheme(PI_ColorScheme cs);

      void SetPluginMessage(wxString &message_id, wxString &message_body);

//    Other public methods
      void SetDialogX    (int x) { m_dialog_x = x;};
      void SetDialogY    (int x) { m_dialog_y = x;}

      void OnDialogClose();

      wxWindow         *m_parent_window;

private:
      bool              LoadConfig(void);
      bool              SaveConfig(void);

      void              RearrangeWindow();

      int               m_dialog_x, m_dialog_y;
      int               m_leftclick_tool_id;

      MessageScopeDialog *m_pDialog;
};

#endif
