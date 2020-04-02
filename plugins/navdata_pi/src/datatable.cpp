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

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include <wx/progdlg.h>
#include <wx/grid.h>
#include "navdata_pi.h"
#include <wx/tokenzr.h>

extern wxString       g_activeRouteGuid;
extern wxString       g_selectedPointGuid;
extern wxString       g_shareLocn;
extern int            g_selectedPointCol;
extern bool           g_showTripData;
extern bool           g_withSog;
extern wxPoint        g_scrollPos;
//----------------------------------------------------------------------------------------------------------
//          Data Table Implementation
//----------------------------------------------------------------------------------------------------------

DataTable::DataTable(wxWindow *parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ,navdata_pi *ppi )
    : DataTableBase(parent, id, title, pos, size, style )
{
    pPlugin = ppi;
}

DataTable::~DataTable(void)
{ 
	delete m_pDataTable;
}

void DataTable::InitDataTable()
{
    m_pDataTable->m_pParent = this;
    g_scrollPos = wxPoint(0, 0);

    //connect events
    //dialog level
    Bind( wxEVT_SIZE, &DataTable::OnSize, this );
    //grid level
    Bind( wxEVT_LEFT_DOWN, &CustomGrid::OnMouseEvent,m_pDataTable );
    Bind( wxEVT_RIGHT_DOWN, &CustomGrid::OnMouseEvent,m_pDataTable );
    //set trip texts Font and size
    wxFont font = GetOCPNGUIScaledFont_PlugIn(_("Dialog") );
    int w;
    GetTextExtent(wxString(_T("ABCDEFG0000")), &w, NULL, 0, 0, &font);
    wxWindowListNode *node =  this->GetChildren().GetFirst();
    while( node ) {
        wxWindow *win = node->GetData();
        if( win ){
            win->SetFont( font );
            if( win->IsKindOf(CLASSINFO(wxStaticText)) || win->IsKindOf(CLASSINFO(wxTextCtrl)) ){
                win->Bind( wxEVT_LEFT_DOWN, &CustomGrid::OnMouseEvent,m_pDataTable );
                win->Bind( wxEVT_RIGHT_DOWN, &CustomGrid::OnMouseEvent,m_pDataTable );
                win->SetFont( font );
            }
            if( win->IsKindOf(CLASSINFO(wxTextCtrl)) ){
                win->SetMinSize(wxSize(w, -1));
                 win->SetSize(wxSize(w, -1));
            }
        }
        node = node->GetNext();
    }
    //connect timers
	m_SizeTimer.Bind(wxEVT_TIMER, &DataTable::OnSizeTimer, this);

    //get navdata_pi options
    wxFileConfig *pConf = GetOCPNConfigObject();
    if (pConf) {
        pConf->SetPath(_T("/Settings/NAVDATA"));
        pConf->Read(_T("DataTablePosition_x"), &m_dialPosition.x, 0);
        pConf->Read(_T("DataTablePosition_y"), &m_dialPosition.y, 0);
        pConf->Read(_T("NumberColVisibles"), &m_numVisCols, -1);
        pConf->Read(_T("ShowTripData"), &g_showTripData,1);
        pConf->Read(_T("ShowTTGETAatSOG"), &g_withSog,0);
    }
    //size grid data
    int h;
    GetTextExtent(wxString(_T("300°300°(M) ")), &w, &h, 0, 0, &font);
    m_pDataTable->SetDefaultColSize( w, true);
    m_pDataTable->SetDefaultRowSize( h + 4, true);
    //Set scroll step X
    m_pDataTable->SetScrollLineX(w);
    //init cell attr
    m_pDataCol = new wxGridCellAttr();
    m_pDataCol->SetFont( font );
    m_pDataCol->SetReadOnly();
    //create grid and populate labels
    wxString sl;
    sl.Append(_("RNG")).Append(_T("(")).Append(getUsrDistanceUnit_Plugin( pPlugin->GetDistFormat())).Append(_T(")"));
    const wxString s[] = {_("BRG"), sl, _("Total RNG") ,_("TTG @ "), _("ETA @ "), _T("END") };
    wxString v = wxEmptyString;
    for( int i = 0;; i++ ){
        m_pDataTable->AppendRows();
        if( i == 3 || i == 4 ){
            v = g_withSog? _("SOG"): _("VMG");
        }
        m_pDataTable->SetRowLabelValue(i, s[i] + v );
        if( s[i + 1] == _T("END") ) break;
    }
    m_pDataTable->SetLabelFont( font );
    m_pDataTable->SetRowLabelSize(wxGRID_AUTOSIZE);
    //Dim grid & Dialog
    DimGridDialog();
    //set scroll step Y
    m_pDataTable->SetScrollLineY( m_pDataTable->GetRowSize(0) );
}

void DataTable::DimGridDialog()
{
    //dim grid
    wxColour bcolour,fcolour;
    GetGlobalColor(_T("DILG0"), &bcolour);       //colour for grid labels & dialogs background
    m_pDataTable->SetCellHighlightColour(bcolour); //do not show cursor position or selection
    m_pDataTable->SetLabelBackgroundColour(bcolour);
    SetBackgroundColour(bcolour);
    //cache whole corner label including settings icon (ovoid flicker in MSW
    m_pDataTable->CacheCornerLabel(bcolour);
    GetGlobalColor(_T("DILG1"), &bcolour);       //colour for grid data & trip data background
    m_pDataCol->SetBackgroundColour(bcolour);
    GetGlobalColor(_T("GREY3"), &fcolour);       //colour for all texts and labels
    m_pDataCol->SetTextColour(fcolour);
    m_pDataTable->SetLabelTextColour(fcolour);
    SetForegroundColour(fcolour);
    wxWindowListNode *node =  this->GetChildren().GetFirst();
    while( node ) {
        wxWindow *win = node->GetData();
        if( win ){
            if( win->IsKindOf(CLASSINFO(wxTextCtrl)) ){
                win->SetBackgroundColour(bcolour);
                win->SetForegroundColour(fcolour);
            }
        }
        node = node->GetNext();
    }
}

void DataTable::UpdateRouteData( wxString pointGuid,
                double shiplat, double shiplon, double shipcog, double shipsog )
{
    //Do not update data if displaying the long way point name
    if(m_pDataTable->m_colLongname != wxNOT_FOUND) return;

    //if no active route set a one empty column grid
    if (g_activeRouteGuid == wxEmptyString) {
        UpdateRouteData();
        return;
    }
    //find active route and wpts
    g_selectedPointCol = wxNOT_FOUND;
    std::unique_ptr<PlugIn_Route> r;
    r = GetRoute_Plugin( g_activeRouteGuid );
    //set label route name in title
    wxString title = r.get()->m_NameString;
    if( title.IsEmpty() )
        title = _("Unnamed route");
    SetTitle(title);
    //populate cols
    m_pDataTable->BeginBatch();
    double brg, rng, speed, nrng;
    speed = 0.; nrng = 0.;
#ifdef __WXOSX__
    double latprev = 0.0, lonprev = 0.0, trng = 0.;
#else
    double latprev, lonprev, trng = 0.;
#endif
    float tttg_sec =0.;
    int ncols = wxNOT_FOUND;
    bool first = true;
    wxPlugin_WaypointListNode *node = r.get()->pWaypointList->GetFirst();
    while( node ){
        PlugIn_Waypoint *wpt = node->GetData();
        if(wpt) {
            wxString srng, sbrg;
            if( first ){
                latprev = wpt->m_lat;
                lonprev = wpt->m_lon;
                first = false;
            }
            if( wpt->m_GUID == pointGuid ) //find active wpt
                ncols = ACTIVE_POINT_IDX;
            if( ncols > wxNOT_FOUND ){
                AddDataCol( ncols );
                if( wpt->m_GUID == g_selectedPointGuid ){
                    if( ncols > ACTIVE_POINT_IDX ){
                        g_selectedPointCol = ncols;
                        m_SelPointPos.m_x = wpt->m_lat;
                        m_SelPointPos.m_y = wpt->m_lon;
                    }
                }
                m_pDataTable->SetColLabelValue( ncols, wpt->m_MarkName );
                if( ncols == ACTIVE_POINT_IDX ){ //active leg from ownship to active point
					//rng, brg, nrng
                    rng = DistGreatCircle_Plugin( shiplat, shiplon, wpt->m_lat, wpt->m_lon );
                    BrgRngMercatorToActiveNormalArrival( wpt->m_lat, wpt->m_lon, latprev, lonprev,
                                            shiplat, shiplon, &brg, &nrng);
					//vmg
					if (!std::isnan(shipcog) && !std::isnan(shipsog)) {
						double vmg = shipsog * cos((brg - shipcog) * PI / 180.);
						if (g_withSog)
							speed = shipsog;
						else
							speed = vmg;
					}
					//sbrg
					if (brg > 359.5) brg = 0;
					if (pPlugin->GetShowMag() == 0 || pPlugin->GetShowMag() == 2)
						sbrg << wxString::Format(wxString("%3.0f°", wxConvUTF8), brg);
					if (pPlugin->GetShowMag() == 1 || pPlugin->GetShowMag() == 2)
						sbrg << wxString::Format(wxString("%3.0f°(M)", wxConvUTF8), pPlugin->GetMag(brg));
					//srng with eventually nrng
                    double deltarng = fabs( rng - nrng );
                    bool delta = false;
                    if( ( deltarng > .01 ) && ( ( deltarng / rng ) > .10 ) && ( rng < 10.0 ) )
                        delta = true;
                    srng = FormatDistance( rng, nrng, delta );
					//total rng
                    trng = rng;
                } else {// following legs
                    //brg, rng
                    DistanceBearingMercator_Plugin(wpt->m_lat, wpt->m_lon, latprev, lonprev,
                                &brg, &rng);
                    srng = FormatDistance( rng );
                    sbrg << wxString::Format( wxString("%3d°", wxConvUTF8 ), (int)brg );
					//total rng
                    trng += rng;
                }
                // print brg, ttg, eta
                wxString tttg_s, teta_s;
                if( speed > 0. ) {
                    float ttg_sec;
                    /*TTG and ETA based on VMG for active leg then on SOG for
                     * the following legs*/
                    if( ncols == ACTIVE_POINT_IDX )
                        ttg_sec = ( rng / speed ) * 3600.;
                    else
                        ttg_sec = ( rng / shipsog ) * 3600.;
                    tttg_sec += ttg_sec;
                    wxTimeSpan tttg_span = wxTimeSpan::Seconds( (long) tttg_sec );
                    //Show also #days if TTG > 24 h
                    tttg_s = tttg_sec > SECONDS_PER_DAY ?
                            tttg_span.Format(_T("%Dd %H:%M")) : tttg_span.Format("%H:%M:%S");
                    wxDateTime dtnow, eta;
                    dtnow.SetToCurrent();
                    eta = dtnow.Add( tttg_span );
                    teta_s = tttg_sec > SECONDS_PER_DAY ?
                            eta.Format(_T("(%d) %H:%M")) : eta.Format(_T("%H:%M"));
                }
                else
                {
                    tttg_s = _T("---");
                    teta_s = _T("---");
                }
                //update cells
                for (int j = 0; j < m_pDataTable->GetNumberRows(); j++ ){
                m_pDataCol->IncRef();
                    switch(j){
                    case 0:
                        m_pDataTable->SetCellValue( j, ncols, sbrg );
                        break;
                    case 1:
                        m_pDataTable->SetCellValue( j, ncols, srng );
                        break;
                    case 2:
                        if( ncols == 0 )
                            m_pDataTable->SetCellValue( j, ncols, _T("---") );
                        else
                            m_pDataTable->SetCellValue( j, ncols, FormatDistance( trng ) );
                        break;
                    case 3:
                        m_pDataTable->SetCellValue( j, ncols, tttg_s );
                        break;
                    case 4:
                        m_pDataTable->SetCellValue( j, ncols, teta_s );
                    }
                }
                ncols++;
            }
        }
        latprev = wpt->m_lat;
        lonprev = wpt->m_lon;
        node = node->GetNext();
    }
    //Clear all useless lines if exist
    int ex = m_pDataTable->GetNumberCols() - ncols;
    if( ex > 0 )
        m_pDataTable->DeleteCols( ncols, ex );
    //set visible point
    if( g_selectedPointCol == wxNOT_FOUND ){
        if ( !g_selectedPointGuid.IsEmpty() )
            MakeVisibleCol( 0 );
        g_selectedPointGuid = wxEmptyString;
	}
	else {
		if (m_targetFlag)
			MakeVisibleCol(g_selectedPointCol);
	}
    //close counters
    m_pDataTable->EndBatch();
    m_pDataCol->DecRef();              // Give up pointer control to Grid
    m_pDataTable->GetFirstVisibleCell(g_scrollPos.y, g_scrollPos.x);
}

void DataTable::UpdateRouteData()
{
    //Do not update data if displaying the long way point name
    if(m_pDataTable->m_colLongname != wxNOT_FOUND) return;

    //in this case there is no active route so create a one empty column grid
    SetTitle(_("No Active Route!"));
    //populate cols
    m_pDataTable->BeginBatch();
    AddDataCol( 0 );
    m_pDataTable->SetColLabelValue( 0, _T("----") );
    //update cells
    for (int j = 0; j < m_pDataTable->GetNumberRows(); j++ ){
        m_pDataCol->IncRef();
        m_pDataTable->SetCellValue( j, 0, _T("---") );
    }
    //Clear all useless lines if exist
    int ex = m_pDataTable->GetNumberCols() - 1;
    if( ex > 0 )
        m_pDataTable->DeleteCols( 1, ex );
    //close counters
    m_pDataTable->EndBatch();
    m_pDataCol->DecRef();              // Give up pointer control to Grid
    m_pDataTable->GetFirstVisibleCell(g_scrollPos.y, g_scrollPos.x);
}

void DataTable::UpdateTripData( wxDateTime starttime, double tdist, wxTimeSpan times )
{
    //Do not update data if displaying the long way point name
    if(m_pDataTable->m_colLongname != wxNOT_FOUND) return;

    if( !g_showTripData ) return;
        m_pStartDate->SetValue( starttime.Format(_T("%b %d %Y")) );
        m_pStartTime->SetValue( starttime.Format(_T("%H:%M:%S")) );
        tdist = toUsrDistance_Plugin( tdist, pPlugin->GetDistFormat());
        wxString sd = tdist > 999.99 ? wxString(_T("%1.0f"),wxConvUTF8 ):
                                       tdist > 99.99 ? wxString(_T("%1.1f"),wxConvUTF8 ):
                                       wxString(_T("%1.2f"),wxConvUTF8 );
        m_pDistValue->SetValue( wxString::Format( sd, tdist )
                                + getUsrDistanceUnit_Plugin( pPlugin->GetDistFormat() ) );
        m_pTimeValue->SetValue( times.Format(_T("%Hh %Mm")) );
        double th = times.GetSeconds().ToDouble() / 3600;
        double sp = tdist / ( th );
        if( std::isnan(sp) )
            m_pSpeedValue->SetValue( _T("----") );
        else
            m_pSpeedValue->SetValue( wxString::Format( wxString("%2.2f", wxConvUTF8 ),
                        toUsrSpeed_Plugin( sp, pPlugin->GetSpeedFormat() ) )
                               + getUsrSpeedUnit_Plugin( pPlugin->GetSpeedFormat() ) );
}

void DataTable::UpdateTripData()
{
    //Do not update data if displaying the long way point name
    if(m_pDataTable->m_colLongname != wxNOT_FOUND) return;

    if( !g_showTripData ) return;
    m_pStartDate->SetValue( _T("----") );
    m_pStartTime->SetValue( _T("----") );
    m_pDistValue->SetValue( _T("----") );
    m_pTimeValue->SetValue( _T("----") );
    m_pSpeedValue->SetValue( _T("----") );
}

void DataTable::MakeVisibleCol( int col )
{
    int row, fcol;
    m_pDataTable->GetFirstVisibleCell( row, fcol );
    if( m_pDataTable->IsVisible(row, col, true) ) return;
    m_pDataTable->MakeCellVisible( row, m_pDataTable->GetNumberCols() - 1 );
    m_pDataTable->MakeCellVisible( row, col );
}

wxWindow *GetNAVCanvas();
void DataTable::SetTableSizePosition(bool moveflag, bool calcTextHeight)
{
    m_InvalidateSizeEvent = true;

    //find the baest visible column number for the route
    int numVisCols = (m_numVisCols > m_pDataTable->GetNumberCols() || m_numVisCols == - 1 )?
                    m_pDataTable->GetNumberCols(): m_numVisCols;

    m_pTripSizer00->Show(g_showTripData);
    /*If trip data visible but not before, we have to set again the size of at least one TextCtrl
     * to be used to calculate the dialog best height*/
    if(calcTextHeight){
        wxFont font = GetOCPNGUIScaledFont_PlugIn(_("Dialog") );
        int w;
        GetTextExtent(wxString(_T("ABCDEFG0000")), &w, NULL, 0, 0, &font);
        m_pStartDate->SetMinSize(wxSize( w, - 1));
    }
    //)adjust visibles columns number
    int scw = GetNAVCanvas()->GetSize().GetWidth();
	int w = GetDataGridWidth(numVisCols);
    if(m_dialPosition.x + w > scw - 1 || m_dialPosition.x < 1 ){
        m_dialPosition.x = scw * 0.1;
		scw *= 0.8;
        if(w > scw ){
			for( int i = numVisCols; i > 0; i-- ){
				if(GetDataGridWidth(i) <= scw ) {
					numVisCols = i;
                    break;
                }
            }
            w = GetDataGridWidth(numVisCols);
        }
    }
    int h = GetDialogHeight(numVisCols);
    this->SetMinClientSize(wxSize(GetDataGridWidth(1), h));
    this->SetClientSize(wxSize(w, h));

    this->Layout();
    RequestRefresh(this);

    if( moveflag )
		this->Move(m_dialPosition);

	m_targetFlag = true;

	m_InvalidateSizeEvent = false;
}

void DataTable::OnSize( wxSizeEvent& event )
{
    if(m_InvalidateSizeEvent)
        return;

    //do not recompute the  visible column number if there is no active route
    if( !g_activeRouteGuid.IsEmpty() ){
        int tempDialWidth = event.GetSize().GetWidth();
        m_numVisCols = (tempDialWidth - (DOUBLE_BORDER_THICKNESS + m_pDataTable->GetRowLabelSize())) / m_pDataTable->GetDefaultColSize();
    }
    m_SizeTimer.Start(TIMER_INTERVAL_10MSECOND, wxTIMER_ONE_SHOT);

    event.Skip();
}

void DataTable::OnSizeTimer(wxTimerEvent & event)
{
	m_InvalidateSizeEvent = true;

    //find the baest visible column number for the route
    int numVisCols = (m_numVisCols > m_pDataTable->GetNumberCols() || m_numVisCols == - 1 )?
                    m_pDataTable->GetNumberCols(): m_numVisCols;
    int w = GetDataGridWidth(numVisCols);
    int h = GetDialogHeight(numVisCols);
    this->SetMinClientSize(wxSize(GetDataGridWidth(1), h));
    this->SetClientSize(wxSize(w, h));

	this->Layout();
    RequestRefresh(this);

	m_targetFlag = true;

	m_InvalidateSizeEvent = false;
}

int DataTable::GetDialogHeight( int nVisCols )
{
    //compute best trip data height
    int h = 0;
	if (g_showTripData) {
        /*Compute Trip Data (get number of sizer column then compute height)
         *if the sizer has 6 columns, there is 2 data lines + 1 box sizer line
         *if the sizer has 4 columns, there is 3 data lines + 1 box sizer line
         *if the sizer has 2 colums, there is 5 data lines + 1 box sizer line*/
		int col;
        int lines;
        switch (nVisCols) {
		case 1:
        case 2:
            col = 2;
            lines = 6;
            break;
        case 3:
        case 4:
            col = 4;
            lines = 4;
            ; break;
		default :
            col = 6;
            lines = 3;
            break;
        }
        m_pTripSizer01->SetCols(col);
        h = (m_pStartDate->GetSize().GetHeight() + SINGLE_BORDER_THICKNESS) * lines;
    }
    //then compute and set best grid height
	m_numVisRows = 5;
	//get display zone heigh
    int sch = GetNAVCanvas()->GetSize().GetHeight() - 1;
    int ht = GetDataGridHeight(m_numVisRows);
    if (m_dialPosition.y + ht +h > sch || m_dialPosition.y < 1) {
        m_dialPosition.y = sch * 0.1;
		sch -= m_dialPosition.y;
        if (ht + h > sch) {
			for (int j = m_numVisRows; j > 0; j--) {
				if (h + GetDataGridHeight(j) <= sch) {
					m_numVisRows = j;
					break;
				}
			}
			//show at least one row
            if (m_numVisRows < 1) m_numVisRows = 1;
            ht = GetDataGridHeight(m_numVisRows);
		}
        //add a margin
	}//
    //add space for an horizontal scroll bar and margin if necessary
    ht += (nVisCols < m_pDataTable->GetNumberCols())? SCROLL_BAR_THICKNESS: 0;
    if( g_showTripData )
        h += SINGLE_BORDER_THICKNESS;
    else
        ht += DOUBLE_BORDER_THICKNESS;

    //set route data grid minsize
    this->m_pDataTable->SetMinSize( wxSize(GetDataGridWidth(nVisCols), ht));

    //return whole dialog best heigh
#ifdef __WXOSX__
    return ht + h + 20;
#else
    return ht + h;
#endif
}

int DataTable::GetDataGridWidth(int nVisCols)
{
    int scbw = (m_numVisRows < m_pDataTable->GetNumberRows()) ? SCROLL_BAR_THICKNESS : 0;
    return  DOUBLE_BORDER_THICKNESS + m_pDataTable->GetRowLabelSize() + (m_pDataTable->GetDefaultColSize() * nVisCols) + scbw;
}

int DataTable::GetDataGridHeight(int nVisCols)
{
    return m_pDataTable->GetColLabelSize() + (m_pDataTable->GetDefaultRowSize() * nVisCols);
}

void DataTable::CloseDialog()
{
    //disconnect timer
    m_SizeTimer.Unbind(wxEVT_TIMER, &DataTable::OnSizeTimer, this);

    m_dialPosition = this->GetPosition();

    this->Show(false);

    wxFileConfig *pConf = GetOCPNConfigObject();
    if(pConf) {
        pConf->SetPath ( _T ( "/Settings/NAVDATA" ) );
        pConf->Write(_T("DataTablePosition_x"), m_dialPosition.x);
        pConf->Write(_T("DataTablePosition_y"), m_dialPosition.y);
        pConf->Write(_T("NumberColVisibles"), m_numVisCols);
        pConf->Write(_T("ShowTripData"), g_showTripData);
        pConf->Write(_T("ShowTTGETAatSOG"), g_withSog);
    }
}

void DataTable::AddDataCol( int num_cols )
{
    if(m_pDataTable->GetNumberCols() == num_cols) {
        m_pDataTable->AppendCols(1);
        m_pDataCol->IncRef();
        m_pDataTable->SetColAttr(num_cols, m_pDataCol);
    }
}

wxString DataTable::FormatDistance( double val1, double val2, bool delta ){
    wxString srng;
    if( delta ){
        if( val2 < 10.0 )
            srng.Printf( _T("%5.2f/%5.2f"), toUsrDistance_Plugin( val1, pPlugin->GetDistFormat() ), toUsrDistance_Plugin( val2, pPlugin->GetDistFormat() ) );
        else
            srng.Printf( _T("%5.1f/%5.1f"), toUsrDistance_Plugin( val1, pPlugin->GetDistFormat() ), toUsrDistance_Plugin( val2, pPlugin->GetDistFormat() ) );
    } else {
        if( val1 < 10.0 )
            srng.Printf( _T("%6.2f"), toUsrDistance_Plugin( val1, pPlugin->GetDistFormat() ) );
        else
            srng.Printf( _T("%6.1f"), toUsrDistance_Plugin( val1, pPlugin->GetDistFormat() ) );
    }
    return srng;
}

void DataTable::BrgRngMercatorToActiveNormalArrival(double wptlat, double wptlon,
                                          double wptprevlat, double wptprevlon,
                                          double glat, double glon,
                                          double *brg, double *nrng)
{
            vector2D va, vb, vn;

            double brg1, dist1, brg2, dist2;
            DistanceBearingMercator_Plugin( wptlat, wptlon,
                    wptprevlat, wptprevlon, &brg1,
                    &dist1 );
            vb.x = dist1 * sin( brg1 * PI / 180. );
            vb.y = dist1 * cos( brg1 * PI / 180. );

            DistanceBearingMercator_Plugin( wptlat, wptlon, glat, glon, &brg2,
                    &dist2 );
            va.x = dist2 * sin( brg2 * PI / 180. );
            va.y = dist2 * cos( brg2 * PI / 180. );

            *brg = brg2;

            vGetLengthOfNormal( &va, &vb, &vn );             // NM
    //    Calculate the distance to the arrival line, which is perpendicular to the current route segment
    //    Taking advantage of the calculated normal from current position to route segment vn
            vector2D vToArriveNormal;
            vSubtractVectors( &va, &vn, &vToArriveNormal );

            *nrng = vVectorMagnitude( &vToArriveNormal );
}


//----------------------------------------------------------------------------------------------------------
//          Settings Dialog Implementation
//----------------------------------------------------------------------------------------------------------

Settings::Settings(wxWindow *parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style )
    : SettingsBase(parent, id, title, pos, size, style )
{
    m_pShowTripData->SetValue( g_showTripData );
    m_pShowspeed->SetSelection(g_withSog ? 1 : 0);

    //dim the dialog
    wxColour colour1, colour2;
    GetGlobalColor(_T("DILG0"), &colour1);
    GetGlobalColor(_T("GREY3"), &colour2);
    SetBackgroundColour( colour1 );
    SetForegroundColour( colour2 );
    wxWindowListNode *node =  this->GetChildren().GetFirst();
    while( node ) {
        wxWindow *win = node->GetData();
        if( win ){
            win->SetBackgroundColour( colour1 );
            win->SetForegroundColour( colour2 );
        }
        node = node->GetNext();
    }

    m_pSettingsOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Settings::OnOKButton ), NULL, this );
}

void Settings::OnOKButton( wxCommandEvent& event )
{
        g_showTripData = m_pShowTripData->GetValue();
        g_withSog = (m_pShowspeed->GetSelection() == 1);
        this->EndModal(wxID_OK);
}
