/* *************************************************************************
 *
 * Project:  OpenCPN - plugin navdata_pi
 * Purpose:  GRIB table
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

#ifndef __DATATABLE_H__
#define __DATATABLE_H__

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers

#include <wx/grid.h>

#include "datadialogbase.h"
#include "vector2D.h"
#include "customgrid.h"
#include <wx/dcgraph.h>

#ifndef PI
#define PI        3.1415926535897931160E0      /* pi */
#endif
#define SECONDS_PER_DAY             86400
#define ACTIVE_POINT_IDX            0
#define SINGLE_BORDER_THICKNESS     3
#define DOUBLE_BORDER_THICKNESS     10
#ifdef __WXMSW__
    #define SCROLL_BAR_THICKNESS    20
#else
    #define SCROLL_BAR_THICKNESS    15
#endif


class navdata_pi;

//----------------------------------------------------------------------------------------------------------
//    Data table dialog Specification
//----------------------------------------------------------------------------------------------------------
class DataTable: public DataTableBase
{
public:

    navdata_pi *pPlugin;

    DataTable(wxWindow *parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ,navdata_pi *ppi );
    
	~DataTable();

    void InitDataTable();
    void UpdateRouteData(wxString pointGuid,
                    double shiplon, double shiplat, double shipcog, double shipsog );
    void UpdateTripData(wxDateTime starttime, double tdist, wxTimeSpan times);
    void UpdateRouteData();
    void UpdateTripData();
    void SetTableSizePosition(bool moveflag, bool calcTextHeight = false);
    void SetTargetFlag(bool flag) {m_targetFlag = flag;}
    void CloseDialog();
    wxPoint2DDouble GetSelPointPos() {return m_SelPointPos;}
    void DimGridDialog();
private:
    void AddDataCol(int num_cols);
    wxString FormatDistance(double val1 , double val2 = 0., bool delta = false);
    void BrgRngMercatorToActiveNormalArrival(double wptlat, double wptlon,
                                  double wptprevlat, double wptprevlon,
                                  double glat, double glon,
                                  double *brg, double *nrng);
    void MakeVisibleCol(int col);
    int  GetDialogHeight(int nVisCols);
    int  GetDataGridWidth(int nVisCols);
    int  GetDataGridHeight(int nVisCols);
    void OnSize(wxSizeEvent &event);
	void OnSizeTimer(wxTimerEvent & event);

    wxGridCellAttr *m_pDataCol;
    bool           m_targetFlag;
    wxPoint2DDouble  m_SelPointPos;
    int            m_numVisCols;
    int            m_numVisRows;
    bool           m_InvalidateSizeEvent;
    wxPoint        m_dialPosition;
	wxTimer		   m_SizeTimer;
};

//----------------------------------------------------------------------------------------------------------
//    Settings dialog Specification
//----------------------------------------------------------------------------------------------------------
class Settings: public SettingsBase
{
private:
    void OnOKButton(wxCommandEvent& event);

public:
    Settings(wxWindow *parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style );

     ~Settings(){}
};

#endif //__DATATABLE_H__
