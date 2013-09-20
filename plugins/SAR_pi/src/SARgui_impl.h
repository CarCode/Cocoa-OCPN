/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  SAR Plugin
 * Author:   SaltyPaws
 *
 ***************************************************************************
 *   Copyright (C) 2012 by Brazil BrokeTail                                *
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
 ***************************************************************************
 */

#ifndef _CALCULATORGUI_IMPL_H_
#define _CALCULATORGUI_IMPL_H_

//#include <cstdlib>
//#include <cstdio>
#ifdef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "SARgui.h"
#include "bitmaps.h"
#include "SAR_pi.h"
#include "NavFunc.h"
#include "tinyxml.h"
using namespace std;

class SAR_pi;

class CfgDlg : public CfgDlgDef
{
public:
      CfgDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("SAR preferences"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE );
};

class Dlg : public DlgDef
{
public:
        Dlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("SAR Plugin by SaltyPaws"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxCAPTION|wxDEFAULT_DIALOG_STYLE|wxMAXIMIZE_BOX|wxRESIZE_BORDER );
        //void OnToggle( wxCommandEvent& event );
        void OnConverttoDegree( wxCommandEvent& event );
        void OnNoteBookFit( wxCommandEvent& event );
        void OnFit( wxCommandEvent& event );

        void OnPSCalc( wxCommandEvent& event );
        void OnPSGPX( wxCommandEvent& event );

        void OnESCalc( wxCommandEvent& event );
        void OnESGPX( wxCommandEvent& event );

        void OnSSCalc( wxCommandEvent& event );
        void OnSSGPX( wxCommandEvent& event );

        void OnORCalc( wxCommandEvent& event );
        void OnORGPX( wxCommandEvent& event );

        void OnShip( wxCommandEvent& event );
        void OnCursor( wxCommandEvent& event );
        void OnShip( void );
        void OnCursor( void );
        void key_shortcut(wxKeyEvent& event);
        void OnCursorSelect( wxCommandEvent& event );
        //void mouse_shortcut(wxMouseEvent& event);

        void Calculate( wxCommandEvent& event, bool Export, int Pattern );

        void Addpoint(TiXmlElement* Route, wxString ptlat, wxString ptlon, wxString ptname, wxString ptsym, wxString pttype);

        void Expanding_Square (double lat, double lon);
        //friend class function;
        SAR_pi *plugin;
        double m_ship_lon,m_ship_lat,m_cursor_lon,m_cursor_lat;


private:
        wxPoint xy;
        wxSize  wh;
        double lat1, lon1, lat2, lon2, targetAz;
        //double F(double x);
        bool error_found;
        bool dbg;
};

#endif
