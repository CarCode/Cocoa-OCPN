/* ***********************************************************************
 * $Id: from_ownship.h
 *
 * Project:  OpenCPN
 * Purpose:  Dashboard Plugin
 * Author:   Pavel Kalian
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

#ifndef _FROM_OWNSHIP_H_
#define _FROM_OWNSHIP_H_

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include "instrument.h"

class DashboardInstrument_FromOwnship : public DashboardInstrument
{
public:
    DashboardInstrument_FromOwnship(wxWindow *pparent, wxWindowID id, wxString title, DASH_CAP cap_flag1=OCPN_DBP_STC_PLA, DASH_CAP cap_flag2=OCPN_DBP_STC_PLO,DASH_CAP cap_flag3=OCPN_DBP_STC_LAT,DASH_CAP cap_flag4=OCPN_DBP_STC_LON);
    ~DashboardInstrument_FromOwnship(){}

    void SetData(DASH_CAP st, double data, wxString unit);
    wxSize GetSize( int orient, wxSize hint );

protected:
    wxString          m_data1;
    wxString          m_data2;
    double            c_lat;
    double            c_lon;
    double            s_lat;
    double            s_lon;
    DASH_CAP          m_cap_flag1;
    DASH_CAP          m_cap_flag2;
    DASH_CAP          m_cap_flag3;
    DASH_CAP          m_cap_flag4;
    int               m_DataHeight;

    void Draw(wxGCDC* dc);
};

#endif
