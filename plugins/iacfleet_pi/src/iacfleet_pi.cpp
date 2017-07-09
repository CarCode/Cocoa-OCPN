/***************************************************************************
 * $Id: iacfleet_pi.cpp, v1.0 2010/08/05 SethDart Exp $
 *
 * Project:  OpenCPN
 * Purpose:  IACFleet Plugin
 * Author:   Carsten Borchardt
 *
 ***************************************************************************
 *   Copyright (C) 2010 by Carsten Borchardt                               *
 *   Carsten@sy-fortytwo.de                                                *
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


#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers

#include <wx/treectrl.h>
#include <wx/fileconf.h>
#include <wx/stdpaths.h>

#include <typeinfo>
#include "iacfleet.h"
#include "iacfleet_pi.h"

// the class factories, used to create and destroy instances of the PlugIn

extern "C" DECL_EXP opencpn_plugin* create_pi( void *ppimgr )
{
    return (opencpn_plugin *)new iacfleet_pi(ppimgr);
}

extern "C" DECL_EXP void destroy_pi( opencpn_plugin* p )
{
    delete p;
}

//---------------------------------------------------------------------------------------------------------
//
//    IACFleet PlugIn Implementation
//
//---------------------------------------------------------------------------------------------------------

#include "icons.h"

//---------------------------------------------------------------------------------------------------------
//
//          PlugIn initialization and de-init
//
//---------------------------------------------------------------------------------------------------------

iacfleet_pi::iacfleet_pi( void *ppimgr ) : opencpn_plugin_113( ppimgr )
{

    // Set some default private member parameters
    m_dialog_x = 0;
    m_dialog_y = 0;
    m_dialog_sx = 200;
    m_dialog_sy = 200;
    m_sort_type = SORT_NAME;
    m_dir = wxEmptyString;
    m_pDialog = NULL;
    m_pdc = NULL;
    m_bShowIcon = false;
    m_leftclick_tool_id = -1;


    // Get a pointer to the opencpn display canvas, to use as a parent for the GRIB dialog
    m_parent_window = GetOCPNCanvasWindow();

    // Create the PlugIn icons
    initialize_images();
//    wxCurlBase::Init();
}

iacfleet_pi::~iacfleet_pi()
{
//    wxCurlBase::Shutdown();
    deinitialize_images();
}


int iacfleet_pi::Init( void )
{
    AddLocaleCatalog( _T("opencpn-iacfleet_pi") );

    //    And load the configuration items
    LoadConfig();

    //    This PlugIn needs a toolbar icon, so request its insertion if enabled locally
#ifdef IACFLEET_USE_SVG
    m_leftclick_tool_id = InsertPlugInToolSVG( _T( "IACFleet" ), _svg_iacfleet, _svg_iacfleet_rollover, _svg_iacfleet_toggled, wxITEM_CHECK, _( "IACFleet" ), _T( "" ), NULL, IACFLEET_TOOL_POSITION, 0, this);
#else
    m_leftclick_tool_id  = InsertPlugInTool(_T(""), _img_iacfleet_pi, _img_iacfleet_pi, wxITEM_NORMAL,
            _("IACFleet"), _T(""), NULL,
            IACFLEET_TOOL_POSITION, 0, this);
#endif

    return (WANTS_OVERLAY_CALLBACK |
            WANTS_OPENGL_OVERLAY_CALLBACK |
            WANTS_CURSOR_LATLON       |
            WANTS_TOOLBAR_CALLBACK    |
            INSTALLS_TOOLBAR_TOOL     |
            WANTS_CONFIG
           );
}

bool iacfleet_pi::DeInit( void )
{
    if( m_pDialog )
    {
        m_pDialog->Close();
    }
    return true;
}

int iacfleet_pi::GetAPIVersionMajor()
{
    return MY_API_VERSION_MAJOR;
}

int iacfleet_pi::GetAPIVersionMinor()
{
    return MY_API_VERSION_MINOR;
}

int iacfleet_pi::GetPlugInVersionMajor()
{
    return PLUGIN_VERSION_MAJOR;
}

int iacfleet_pi::GetPlugInVersionMinor()
{
    return PLUGIN_VERSION_MINOR;
}

wxBitmap *iacfleet_pi::GetPlugInBitmap()
{
    return _img_iacfleet_pi;
}
wxString iacfleet_pi::GetCommonName()
{
    return _("IACFleet");
}

wxString iacfleet_pi::GetShortDescription()
{
    return _("IACFleet PlugIn for OpenCPN");
}


wxString iacfleet_pi::GetLongDescription()
{
    return _("IACFleet PlugIn for OpenCPN\nDisplay IAC Fleet Code information provided by NADI.\nInformation can be received by email by sending an email with the contents\n\"Send fleet.nadi\" to \"query@saildocs.com\"\nSave the EMail you get as response and open it with this\nPlugin to see the decoded text and a graphic overlay\n");
}

int iacfleet_pi::GetToolbarToolCount( void )
{
    return 1;
}

void iacfleet_pi::ShowPreferencesDialog( wxWindow* parent )
{
    wxDialog *dialog = new wxDialog( parent, wxID_ANY, _("IACFleet Preferences"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE );
    dialog->Fit();
}

void iacfleet_pi::SetCursorLatLon( double lat, double lon )
{
    if( NULL != m_pDialog )
    {
        m_pDialog->SetCursorLatLon(lat,lon);
    }
}

void iacfleet_pi::OnToolbarToolCallback( int id )
{
    // show the IACFleet dialog
    if( NULL == m_pDialog )
    {
        m_pDialog = new IACFleetUIDialog();
        long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER;
#ifdef __WXMAC__
        style |= wxSTAY_ON_TOP;
#endif
        wxPoint pos = wxPoint( m_dialog_x, m_dialog_y);
        if( !m_parent_window->GetRect().Contains( pos ) )
            pos = wxDefaultPosition; // If it seems we are off position, move to the default position
        m_pDialog->Create( m_parent_window,
                this,
                -1,
                _("IACFleet Display Control"),
                m_dir,
                m_sort_type,
                pos,
                wxSize( m_dialog_sx, m_dialog_sy),
                style );
    }

    m_pDialog->Show( !m_pDialog->IsShown() ); // Show modeless, so it stays on the screen
}

void iacfleet_pi::OnDialogClose()
{
    m_pDialog = NULL;
    SaveConfig();

}
bool iacfleet_pi::RenderOverlay( wxDC &dc, PlugIn_ViewPort *vp )
{
    m_pdc = &dc;
    bool hasDrawn = m_pDialog && m_pDialog->RenderOverlay(m_pdc,vp);
    return hasDrawn;
}

bool iacfleet_pi::RenderGLOverlay( wxGLContext *pcontext, PlugIn_ViewPort *vp )
{
    m_pdc = NULL;
    bool hasDrawn = m_pDialog && m_pDialog->RenderOverlay(m_pdc,vp);
    return hasDrawn;
}


bool iacfleet_pi::LoadConfig( void )
{
    wxFileConfig *pConf = GetOCPNConfigObject();

    if( pConf )
    {
        pConf->SetPath ( _T( "/Settings" ) );

        m_dialog_sx = pConf->Read ( _T ( "IACFleetDialogSizeX" ), 300L );
        m_dialog_sy = pConf->Read ( _T ( "IACFleetDialogSizeY" ), 540L );
        m_dialog_x =  pConf->Read ( _T ( "IACFleetDialogPosX" ), 20L );
        m_dialog_y =  pConf->Read ( _T ( "IACFleetDialogPosY" ), 170L );

        m_sort_type =  pConf->Read ( _T ( "IACFleetSortType" ), SORT_NAME );

        pConf->SetPath ( _T ( "/Directories" ) );
        pConf->Read ( _T ( "IACFleetDirectory" ), &m_dir, wxStandardPaths::Get().GetDocumentsDir() );

        return true;
    }
    else
        return false;
}

bool iacfleet_pi::SaveConfig( void )
{
    wxFileConfig *pConf = GetOCPNConfigObject();

    if( pConf )
    {
        pConf->SetPath ( _T ( "/Settings" ) );
        pConf->Write ( _T ( "IACFleetDialogSizeX" ),  m_dialog_sx );
        pConf->Write ( _T ( "IACFleetDialogSizeY" ),  m_dialog_sy );
        pConf->Write ( _T ( "IACFleetDialogPosX" ),   m_dialog_x );
        pConf->Write ( _T ( "IACFleetDialogPosY" ),   m_dialog_y );

        pConf->Write ( _T ( "IACFleetSortType" ),   m_sort_type );

        pConf->SetPath ( _T ( "/Directories" ) );
        pConf->Write ( _T ( "IACFleetDirectory" ), m_dir );

        return true;
    }
    else
        return false;
}
