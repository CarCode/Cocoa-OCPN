/***************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  messagescope Plugin
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

#include <wx/process.h>

#ifdef __unix__
#include <sys/stat.h>
#endif

#ifdef BUILTIN_RTLAIS
#include "rtl-sdr-misc/rtl_ais.h"
#endif

#include "messagescope_pi.h"
#include "MessageScopeDialog.h"

#include "icons.h"

// the class factories, used to create and destroy instances of the PlugIn

extern "C" DECL_EXP opencpn_plugin* create_pi(void *ppimgr)
{
    return new messagescope_pi(ppimgr);
}

extern "C" DECL_EXP void destroy_pi(opencpn_plugin* p)
{
    delete p;
}

//---------------------------------------------------------------------------------------------------------
//
//          PlugIn initialization and de-init
//
//---------------------------------------------------------------------------------------------------------

messagescope_pi::messagescope_pi(void *ppimgr)
      :opencpn_plugin_18(ppimgr)
{
      // Create the PlugIn icons
      initialize_images();
}

messagescope_pi::~messagescope_pi()
{
      delete _img_messagescope;
}


int messagescope_pi::Init(void)
{
      AddLocaleCatalog( _T("opencpn-messages_pi") );

      //    Get a pointer to the opencpn display canvas, to use as a parent for the POI Manager dialog
      m_parent_window = GetOCPNCanvasWindow();

      //    And load the configuration items
      LoadConfig();

      m_leftclick_tool_id  = InsertPlugInTool(_T(""), _img_messagescope, _img_messagescope, wxITEM_NORMAL,
            _("Messages"), _T(""), NULL,
             MESSAGESCOPE_TOOL_POSITION, 0, this);

      m_pDialog = NULL;

      return (WANTS_TOOLBAR_CALLBACK    |
              INSTALLS_TOOLBAR_TOOL     |
              WANTS_PLUGIN_MESSAGING    |
              WANTS_CONFIG
           );
}

bool messagescope_pi::DeInit(void)
{
    //    Record the dialog position
    if (NULL != m_pDialog)
    {
        wxPoint p = m_pDialog->GetPosition();
        SetDialogX(p.x);
        SetDialogY(p.y);
        
        m_pDialog->Close();
        delete m_pDialog;
        m_pDialog = NULL;
    }
    SaveConfig();
    
    RemovePlugInTool(m_leftclick_tool_id);
    
    return true;
}

int messagescope_pi::GetAPIVersionMajor()
{
      return MY_API_VERSION_MAJOR;
}

int messagescope_pi::GetAPIVersionMinor()
{
      return MY_API_VERSION_MINOR;
}

int messagescope_pi::GetPlugInVersionMajor()
{
      return PLUGIN_VERSION_MAJOR;
}

int messagescope_pi::GetPlugInVersionMinor()
{
      return PLUGIN_VERSION_MINOR;
}

wxBitmap *messagescope_pi::GetPlugInBitmap()
{
    return new wxBitmap(_img_messagescope->ConvertToImage().Copy());
}

wxString messagescope_pi::GetCommonName()
{
      return _T("Messages");
}


wxString messagescope_pi::GetShortDescription()
{
      return _("Messages Check PlugIn for OpenCPN");
}

wxString messagescope_pi::GetLongDescription()
{
      return _("Messages Check PlugIn for OpenCPN\n\
This plugin identifys and displays internal messages\n\
");
}

int messagescope_pi::GetToolbarToolCount(void)
{
      return 1;
}

void messagescope_pi::SetColorScheme(PI_ColorScheme cs)
{
      if (NULL == m_pDialog)
            return;
      DimeWindow(m_pDialog);
}

void messagescope_pi::RearrangeWindow()
{
      if (NULL == m_pDialog)
            return;

      SetColorScheme(PI_ColorScheme());

      m_pDialog->Fit();
}

void messagescope_pi::OnToolbarToolCallback(int id)
{
    if(!m_pDialog)
    {
        m_pDialog = new MessageScopeDialog(*this, m_parent_window);
        m_pDialog->Move(wxPoint(m_dialog_x, m_dialog_y));
    }

    RearrangeWindow();
    m_pDialog->Show(!m_pDialog->IsShown());

    wxPoint p = m_pDialog->GetPosition();
    m_pDialog->Move(0,0);        // workaround for gtk autocentre dialog behavior
    m_pDialog->Move(p);
}

bool messagescope_pi::LoadConfig(void)
{
    wxFileConfig *pConf = GetOCPNConfigObject();

    if(!pConf)
        return false;

    pConf->SetPath ( _T( "/Settings/messages" ) );

    m_dialog_x =  pConf->Read ( _T ( "DialogPosX" ), 20L );
    m_dialog_y =  pConf->Read ( _T ( "DialogPosY" ), 20L );

    return true;
}

bool messagescope_pi::SaveConfig(void)
{
    wxFileConfig *pConf = GetOCPNConfigObject();

    if(!pConf)
        return false;

    pConf->SetPath ( _T ( "/Settings/messages" ) );

    pConf->Write ( _T ( "DialogPosX" ),   m_dialog_x );
    pConf->Write ( _T ( "DialogPosY" ),   m_dialog_y );

    return true;
}


void messagescope_pi::SetPluginMessage(wxString &message_id, wxString &message_body)
{
    if(m_pDialog)
        m_pDialog->SetPluginMessage(message_id, message_body);
}
