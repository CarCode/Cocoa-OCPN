/* *************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  Console Canvas
 * Author:   David Register
 *
 ***************************************************************************
 *   Copyright (C) 2010 by David S. Register                               *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.         *
 ***************************************************************************/

#ifndef __concanv_H__
#define __concanv_H__

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers

//----------------------------------------------------------------------------
//   constants
//----------------------------------------------------------------------------

#define SPEED_VMG 0
#define SPEED_SOG 1

#define SECONDS_PER_DAY     86400

#define ACTIVE_POINT_IDX    0

// Class declarations
class navdata_pi;
//----------------------------------------------------------------------------
// AnnunText
//----------------------------------------------------------------------------
class AnnunText : public wxWindow
{
public:
      AnnunText(wxWindow *parent, wxWindowID id);

      ~AnnunText();

      void SetALabel(const wxString &l);
      void SetAValue(const wxString &v);
      void OnPaint(wxPaintEvent& event);
      void SetColorScheme();
      void CalculateMinSize(void);
      
private:
      void MouseEvent( wxMouseEvent& event );

      wxBrush     m_backBrush;

      wxString    m_label;
      wxString    m_value;

DECLARE_EVENT_TABLE()

};




//----------------------------------------------------------------------------
// RouteCanvas
//----------------------------------------------------------------------------
class RouteCanvas: public wxFrame
{
public:
      RouteCanvas(wxWindow *parent, navdata_pi *ppi);
      ~RouteCanvas();
      void UpdateRouteData();
      void ShowWithFreshFonts(void);
      void UpdateFonts(void);
      void SetColorScheme();
      void LegRoute();
      void OnContextMenu( wxContextMenuEvent& event );
      void OnContextMenuSelection( wxCommandEvent& event );
      void RefreshConsoleData(void);
      void ToggleVmgSogDisplay();

      navdata_pi *pPlugin;

      wxBoxSizer        *m_pitemBoxSizerLeg;
      wxString          m_pointName;
      wxBrush           m_backBrush;

      AnnunText         *pRNG;
      AnnunText         *pTTG;
      AnnunText         *pETA;

      bool              m_bNeedClear;

private:
      void OnPaintEvent( wxPaintEvent &event );
      void OnMouseEvent( wxMouseEvent &event );
      void BrgRngMercatorToActiveNormalArrival(double wptlat, double wptlon,
                                    double wptprevlat, double wptprevlon,
                                    double glat, double glon,
                                    double *brg, double *nrng);
      wxFont     *m_pThisPointFont;
      char       m_speedUsed;
      wxPoint    m_consPosition;


DECLARE_EVENT_TABLE()
};

#endif
