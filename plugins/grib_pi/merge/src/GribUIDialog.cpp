/******************************************************************************
*
* Project: OpenCPN
* Purpose: GRIB Object
* Author: David Register
*
***************************************************************************
* Copyright (C) 2010 by David S. Register *
* *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 2 of the License, or *
* (at your option) any later version. *
* *
* This program is distributed in the hope that it will be useful, *
* but WITHOUT ANY WARRANTY; without even the implied warranty of *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the *
* GNU General Public License for more details. *
* *
* You should have received a copy of the GNU General Public License *
* along with this program; if not, write to the *
* Free Software Foundation, Inc., *
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA. *
***************************************************************************
*
*/

#include "wx/wx.h"
#include "wx/tokenzr.h"
#include "wx/datetime.h"
#include "wx/sound.h"
#include <wx/wfstream.h>
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/debug.h>
#include <wx/graphics.h>

#include <wx/stdpaths.h>

#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "grib_pi.h"
#include "email.h"
#include "folder.xpm"

#include <wx/arrimpl.cpp>

#if defined (_WIN32)
int round (double x) {
int i = (int) x;
if (x >= 0.0) {
return ((x-i) >= 0.5) ? (i + 1) : (i);
} else {
return (-x+i >= 0.5) ? (i - 1) : (i);
}
}
#endif

WX_DEFINE_OBJARRAY( ArrayOfGribRecordSets );

// Sort compare function for File Modification Time
static int CompareFileStringTime( const wxString& first, const wxString& second )
{
    wxFileName f( first );
    wxFileName s( second );
    wxTimeSpan sp = s.GetModificationTime() - f.GetModificationTime();
    return sp.GetMinutes();

// return ::wxFileModificationTime(first) - ::wxFileModificationTime(second);
}

//date/time in the desired time zone format
static wxString TToString( const wxDateTime date_time, const int time_zone )
{
    wxDateTime t( date_time );
    t.MakeFromTimezone( wxDateTime::UTC );
    if( t.IsDST() ) t.Subtract( wxTimeSpan( 1, 0, 0, 0 ) );
    switch( time_zone ) {
        case 0: return t.Format( _T(" %a %d-%b-%Y %H:%M "), wxDateTime::Local ) + _T("LOC");//:%S
        case 1:
        default: return t.Format( _T(" %a %d-%b-%Y %H:%M "), wxDateTime::UTC ) + _T("UTC");
    }
}

wxString toSAILDOC ( int NEflag, int MMflag, double a )
{
    //saildoc needs value in degré (without decimal) so translate doube value in integer and if necessary tacking one degré more
    //to be sure all the screen is covered
    short neg = 0;
    if ( a < 0.0 ) {
        a = -a;
        neg = 1;
    }
    char c;
    switch(NEflag) {
        case 1: {
            if ( !neg ) {
                if( MMflag == 1 ) a += 1.;
                c = 'N';
            } else {
                if( MMflag == 2 ) a += 1.;
                c = 'S';
            }
            break;
        }
        case 2: {
            if ( !neg ) {
                if( MMflag == 1 ) a += 1.;
                c = 'E';
            } else {
                if( MMflag == 2 ) a += 1.;
                c = 'W';
            }
        }
    }
    wxString s;
    s.Printf ( _T ( "%01d%c" ), (int) a, c );
    return s;
}

//---------------------------------------------------------------------------------------
// GRIB Selector/Control Dialog Implementation
//---------------------------------------------------------------------------------------

/* interpolating constructor
as a possible optimization, write this function to also
take latitude longitude boundaries so the resulting record can be
a subset of the input, but also would need to be recomputed when panning the screen */
GribTimelineRecordSet::GribTimelineRecordSet(GribRecordSet &GRS1, GribRecordSet &GRS2, double interp_const)
{
    for(int i=0; i<Idx_COUNT; i++) {
        GribRecord *GR1 = GRS1.m_GribRecordPtrArray[i];
        GribRecord *GR2 = GRS2.m_GribRecordPtrArray[i];
        
        if(GR1 && GR2 && GR1->isOk() && GR2->isOk() &&
           GR1->getDi() == GR2->getDi() && GR1->getDj() == GR2->getDj() &&
           GR1->getLatMin() == GR2->getLatMin() && GR1->getLonMin() == GR2->getLonMin() &&
           GR1->getLatMax() == GR2->getLatMax() && GR1->getLonMax() == GR2->getLonMax())
            m_GribRecordPtrArray[i] = new GribRecord(*GR1, *GR2, interp_const);
        else
            m_GribRecordPtrArray[i] = NULL;
        m_IsobarArray[i] = NULL;
    }

    m_Reference_Time = (1-interp_const)*GRS1.m_Reference_Time
        + interp_const*GRS2.m_Reference_Time;
}

GribTimelineRecordSet::~GribTimelineRecordSet()
{
    for(int i=0; i<Idx_COUNT; i++)
        delete m_GribRecordPtrArray[i]; /* delete these for timeline */
    ClearCachedData();
}

void GribTimelineRecordSet::ClearCachedData()
{
    for(int i=0; i<Idx_COUNT; i++) {
        if(m_IsobarArray[i]) {
            // Clear out the cached isobars
            for( unsigned int j = 0; j < m_IsobarArray[i]->GetCount(); j++ ) {
                IsoLine *piso = (IsoLine *) m_IsobarArray[i]->Item( j );
                delete piso;
            }
            delete m_IsobarArray[i];
            m_IsobarArray[i] = NULL;
        }
    }
}

GRIBUIDialog::GRIBUIDialog(wxWindow *parent, grib_pi *ppi)
: GRIBUIDialogBase(parent)
{
    pParent = parent;
    pPlugIn = ppi;

    m_bGRIBActiveFile = NULL;
    m_pTimelineSet = NULL;

    wxFileConfig *pConf = GetOCPNConfigObject();

    if(pConf) {
        pConf->SetPath ( _T ( "/Settings/GRIB" ) );

        bool value;
        pConf->Read( _T ( "WindPlot" ), &value, true );
        m_cbWind->SetValue(value);
        pConf->Read( _T ( "WindGustPlot" ), &value, true );
        m_cbWindGust->SetValue(value);
        pConf->Read( _T ( "PressurePlot" ), &value, true );
        m_cbPressure->SetValue(value);
        pConf->Read( _T ( "WavePlot" ), &value, false );
        m_cbWave->SetValue(value);
        pConf->Read( _T ( "CurrentPlot" ), &value, false );
        m_cbCurrent->SetValue(value);
        pConf->Read( _T ( "PrecipitationPlot" ), &value, false );
        m_cbPrecipitation->SetValue(value);
        pConf->Read( _T ( "CloudPlot" ), &value, false );
        m_cbCloud->SetValue(value);
        pConf->Read( _T ( "AirTemperaturePlot" ), &value, false );
        m_cbAirTemperature->SetValue(value);
        pConf->Read( _T ( "SeaTemperaturePlot" ), &value, false );
        m_cbSeaTemperature->SetValue(value);

        pConf->Read ( _T ( "lastdatatype" ), &m_lastdatatype, 0);

        pConf->Read ( _T ( "Filename" ), &m_file_name );

        pConf->SetPath ( _T ( "/Directories" ) );
        pConf->Read ( _T ( "GRIBDirectory" ), &m_grib_dir );

    }

#if !wxCHECK_VERSION(2,9,4) /* to work with wx 2.8 */
#define SetBitmap SetLabel
#endif

    m_bpPrev->SetBitmap(wxBitmap( prev ));
    m_bpNext->SetBitmap(wxBitmap( next ));
    m_bpNow->SetBitmap(wxBitmap( now ));
    m_bpOpenFile->SetBitmap(wxBitmap( openfile ));
    m_bpSettings->SetBitmap(wxBitmap( setting ));
    m_bpRequest->SetBitmap(wxBitmap( request ));

    m_tPlayStop.Connect(wxEVT_TIMER, wxTimerEventHandler( GRIBUIDialog::OnPlayStopTimer ), NULL, this);

    m_OverlaySettings.Read();

    DimeWindow( this );

    m_pTimelineSet = NULL;
    PopulateTrackingControls();

    Fit();
    SetMinSize( GetBestSize() );
}


GRIBUIDialog::~GRIBUIDialog()
{
    wxFileConfig *pConf = GetOCPNConfigObject();;

    if(pConf) {
        pConf->SetPath ( _T ( "/Settings/GRIB" ) );

        pConf->Write( _T ( "WindPlot" ), m_cbWind->GetValue());
        pConf->Write( _T ( "WindGustPlot" ), m_cbWindGust->GetValue());
        pConf->Write( _T ( "PressurePlot" ), m_cbPressure->GetValue());
        pConf->Write( _T ( "WavePlot" ), m_cbWave->GetValue());
        pConf->Write( _T ( "CurrentPlot" ), m_cbCurrent->GetValue());
        pConf->Write( _T ( "PrecipitationPlot" ), m_cbPrecipitation->GetValue());
        pConf->Write( _T ( "CloudPlot" ), m_cbCloud->GetValue());
        pConf->Write( _T ( "AirTemperaturePlot" ), m_cbAirTemperature->GetValue());
        pConf->Write( _T ( "SeaTemperaturePlot" ), m_cbSeaTemperature->GetValue());
        pConf->Write( _T ( "lastdatatype" ), m_lastdatatype);

        pConf->Write ( _T ( "Filename" ), m_file_name );

        pConf->SetPath ( _T ( "/Directories" ) );
        pConf->Write ( _T ( "GRIBDirectory" ), m_grib_dir );
    }

    delete m_pTimelineSet;
}

void GRIBUIDialog::SetCursorLatLon( double lat, double lon )
{
    m_cursor_lon = lon;
    m_cursor_lat = lat;

    UpdateTrackingControls();
}

void GRIBUIDialog::AddTrackingControl( wxControl *ctrl1, wxControl *ctrl2, wxControl *ctrl3, bool show )
{
    if(show) {
        m_fgTrackingControls->Add(ctrl1, 0, wxALL, 5);
        ctrl1->Show();
        if(ctrl2) {
            m_fgTrackingControls->Add(ctrl2, 0, wxALL, 5);
            ctrl2->Show();
        } else
              m_fgTrackingControls->Add(0, 0, 1, wxEXPAND, 5); /* spacer */
        
        if(ctrl3) {
            m_fgTrackingControls->Add(ctrl3, 0, wxALL, 5);
            ctrl3->Show();
        } else
            m_fgTrackingControls->Add(0, 0, 1, wxEXPAND, 5); /* spacer */
    } else {
        ctrl1->Hide();
        if(ctrl2)
            ctrl2->Hide();
        if(ctrl3)
            ctrl3->Hide();
    }
}
    
void GRIBUIDialog::PopulateTrackingControls( void )
{
    m_fgTrackingControls->Clear();

    GribRecord **RecordArray;
    if( m_pTimelineSet )
        RecordArray = m_pTimelineSet->m_GribRecordPtrArray;

    AddTrackingControl(m_cbWind, m_tcWindSpeed, m_tcWindDirection,
                       m_pTimelineSet && RecordArray[Idx_WIND_VX] && RecordArray[Idx_WIND_VY]);
    AddTrackingControl(m_cbWindScat, m_tcWindScatSpeed, m_tcWindScatDirection,
                       m_pTimelineSet && RecordArray[Idx_WINDSCAT_VX] && RecordArray[Idx_WINDSCAT_VY]);
    AddTrackingControl(m_cbWindGust, m_tcWindGust, 0, m_pTimelineSet && RecordArray[Idx_WIND_GUST]);
    AddTrackingControl(m_cbPressure, m_tcPressure, 0, m_pTimelineSet && RecordArray[Idx_PRESSURE]);

    /* tracking for wave is funky */
    if(m_pTimelineSet && RecordArray[Idx_HTSIGW]) {
        if(RecordArray[Idx_WVDIR])
            AddTrackingControl(m_cbWave, m_tcWaveHeight, m_tcWaveDirection, true);
        else {
            AddTrackingControl(m_cbWave, m_tcWaveHeight, 0, true);
            m_tcWaveDirection->Hide();
        }
    } else
        if(m_pTimelineSet && RecordArray[Idx_WVDIR]) {
            AddTrackingControl(m_cbWave, 0, m_tcWaveDirection, true);
            m_tcWaveHeight->Hide();
        } else
            AddTrackingControl(m_cbWave, m_tcWaveHeight, m_tcWaveDirection, false);

    AddTrackingControl(m_cbCurrent, m_tcCurrentVelocity, m_tcCurrentDirection,
                       m_pTimelineSet && RecordArray[Idx_SEACURRENT_VX] && RecordArray[Idx_SEACURRENT_VY]);
    AddTrackingControl(m_cbPrecipitation, m_tcPrecipitation, 0,
                       m_pTimelineSet && RecordArray[Idx_PRECIP_TOT]);
    AddTrackingControl(m_cbCloud, m_tcCloud, 0,
                       m_pTimelineSet && RecordArray[Idx_CLOUD_TOT]);
    AddTrackingControl(m_cbAirTemperature, m_tcAirTemperature, 0,
                       m_pTimelineSet && RecordArray[Idx_AIR_TEMP_2M]);
    AddTrackingControl(m_cbSeaTemperature, m_tcSeaTemperature, 0,
                       m_pTimelineSet && RecordArray[Idx_SEA_TEMP]);

    Fit();
    Refresh();
}

void GRIBUIDialog::UpdateTrackingControls( void )
{
    if( !m_pTimelineSet )
        return;

    wxDateTime t = m_pTimelineSet->m_Reference_Time;
    t.MakeFromTimezone( wxDateTime::UTC );
    if( t.IsDST() ) t.Subtract( wxTimeSpan( 1, 0, 0, 0 ) );
    m_cRecordForecast->SetLabel(t.Format(_T("%Y-%m-%d %H:%M:%S "), wxDateTime::Local) + _T("Local - ") +
                                t.Format(_T("%H:%M:%S "), wxDateTime::UTC) + _T("GMT"));
    
    GribRecord **RecordArray = m_pTimelineSet->m_GribRecordPtrArray;
    // Update the wind control
    if( RecordArray[Idx_WIND_VX] && RecordArray[Idx_WIND_VY] ) {
        double vx = RecordArray[Idx_WIND_VX]->
            getInterpolatedValue(m_cursor_lon, m_cursor_lat, true );
        double vy = RecordArray[Idx_WIND_VY]->
            getInterpolatedValue(m_cursor_lon, m_cursor_lat, true );
        
        if( ( vx != GRIB_NOTDEF ) && ( vy != GRIB_NOTDEF ) ) {
            vx = m_OverlaySettings.CalibrateValue(GribOverlaySettings::WIND, vx);
            vy = m_OverlaySettings.CalibrateValue(GribOverlaySettings::WIND, vy);
            
            double vkn = sqrt( vx * vx + vy * vy );
            m_tcWindSpeed->SetValue( wxString::Format( _T("%2d"), (int) vkn ));
            
            double ang = 90. + ( atan2( vy, -vx ) * 180. / PI );
            if( ang > 360. ) ang -= 360.;
            if( ang < 0. ) ang += 360.;
            m_tcWindDirection->SetValue( wxString::Format( _T("%03d"), (int) ( ang ) ));
        }
    }
    
    // Update the QuickScat (aka Wind) control
    if( RecordArray[Idx_WINDSCAT_VX] && RecordArray[Idx_WINDSCAT_VY] ) {
        double vx = RecordArray[Idx_WINDSCAT_VX]->
            getInterpolatedValue(m_cursor_lon, m_cursor_lat, true );
        double vy = RecordArray[Idx_WINDSCAT_VY]->
            getInterpolatedValue(m_cursor_lon, m_cursor_lat, true );
        
        if( ( vx != GRIB_NOTDEF ) && ( vy != GRIB_NOTDEF ) ) {
            vx = m_OverlaySettings.CalibrateValue(GribOverlaySettings::WIND, vx);
            vy = m_OverlaySettings.CalibrateValue(GribOverlaySettings::WIND, vy);
            
            double vkn = sqrt( vx * vx + vy * vy );
            m_tcWindScatSpeed->SetValue( wxString::Format( _T("%2d"), (int) vkn ) );
            
            double ang = 90. + ( atan2( vy, -vx ) * 180. / PI );
            if( ang > 360. ) ang -= 360.;
            if( ang < 0. ) ang += 360.;
            m_tcWindScatDirection->SetValue( wxString::Format( _T("%03d"), (int) ( ang ) ) );
        }
    }
    
    // Update the Wind gusts control
    if( RecordArray[Idx_WIND_GUST] ) {
        double vkn = RecordArray[Idx_WIND_GUST]->
            getInterpolatedValue(m_cursor_lon, m_cursor_lat, true );
        
        if( vkn != GRIB_NOTDEF ) {
            vkn = m_OverlaySettings.CalibrateValue(GribOverlaySettings::WIND_GUST, vkn);
            m_tcWindGust->SetValue( wxString::Format(_T("%2d"), (int) ( vkn )) );
        }
    }
    
    // Update the Pressure control
    if( RecordArray[Idx_PRESSURE] ) {
        double press = RecordArray[Idx_PRESSURE]->
            getInterpolatedValue(m_cursor_lon, m_cursor_lat, true );
        
        if( press != GRIB_NOTDEF ) {
            press = m_OverlaySettings.CalibrateValue(GribOverlaySettings::PRESSURE, press);
            m_tcPressure->SetValue( wxString::Format(_T("%2d"), (int) ( press )) );
        }
    }

    // Update the Sig Wave Height
    if( RecordArray[Idx_HTSIGW] ) {
        double height = RecordArray[Idx_HTSIGW]->
            getInterpolatedValue(m_cursor_lon, m_cursor_lat, true );
        
        if( height != GRIB_NOTDEF ) {
            height = m_OverlaySettings.CalibrateValue(GribOverlaySettings::WAVE, height);
            m_tcWaveHeight->SetValue( wxString::Format( _T("%4.1f"), height ));
        }
    }
    
    // Update the Wave direction
    if( RecordArray[Idx_WVDIR] ) {
        double direction = RecordArray[Idx_WVDIR]->
            getInterpolatedValue(m_cursor_lon, m_cursor_lat, true );
        if( direction != GRIB_NOTDEF )
            m_tcWaveDirection->SetValue( wxString::Format( _T("%03d"), (int)direction ));
    }
    
    
    // Update the Current control
    if( RecordArray[Idx_SEACURRENT_VX] && RecordArray[Idx_SEACURRENT_VY] ) {
        double vx = RecordArray[Idx_SEACURRENT_VX]->
            getInterpolatedValue(m_cursor_lon, m_cursor_lat, true );
        double vy = RecordArray[Idx_SEACURRENT_VY]->
            getInterpolatedValue(m_cursor_lon, m_cursor_lat, true );
        
        if( ( vx != GRIB_NOTDEF ) && ( vy != GRIB_NOTDEF ) ) {
            vx = m_OverlaySettings.CalibrateValue(GribOverlaySettings::CURRENT, vx);
            vy = m_OverlaySettings.CalibrateValue(GribOverlaySettings::CURRENT, vy);
            
            double vkn = sqrt( vx * vx + vy * vy );
            m_tcCurrentVelocity->SetValue( wxString::Format( _T("%5.2f"), vkn ) );
            
            double ang = 90. + ( atan2( -vy, vx ) * 180. / PI );
            if( ang > 360. ) ang -= 360.;
            if( ang < 0. ) ang += 360.;
            m_tcCurrentDirection->SetValue( wxString::Format( _T("%03d"), (int) ( ang ) ) );
        }
    }
    
    // Update total rainfall control
    if( RecordArray[Idx_PRECIP_TOT] ) {
        double precip = RecordArray[Idx_PRECIP_TOT]->
            getInterpolatedValue( m_cursor_lon, m_cursor_lat, true );
        
        if( precip != GRIB_NOTDEF ) {
            precip = m_OverlaySettings.CalibrateValue(GribOverlaySettings::PRECIPITATION, precip);
            m_tcPrecipitation->SetValue( wxString::Format( _T("%6.2f"), precip ) );
        }
    }
    
    // Update total cloud control
    if( RecordArray[Idx_CLOUD_TOT] ) {
        double cloud = RecordArray[Idx_CLOUD_TOT]->
            getInterpolatedValue( m_cursor_lon, m_cursor_lat, true );
        
        if( cloud != GRIB_NOTDEF ) {
            cloud = m_OverlaySettings.CalibrateValue(GribOverlaySettings::CLOUD, cloud);
            m_tcCloud->SetValue( wxString::Format( _T("%6.2f"), cloud ) );
        }
    }
    
    // Update the Air Temperature
    if( RecordArray[Idx_AIR_TEMP_2M] ) {
        double temp = RecordArray[Idx_AIR_TEMP_2M]->
            getInterpolatedValue( m_cursor_lon, m_cursor_lat, true );
        
        if( temp != GRIB_NOTDEF ) {
            temp = m_OverlaySettings.CalibrateValue(GribOverlaySettings::AIR_TEMPERATURE, temp);
            m_tcAirTemperature->SetValue( wxString::Format( _T("%6.2f"), temp ) );
        }
    }
    
    // Update the Sea Surface Temperature
    if( RecordArray[Idx_SEA_TEMP] ) {
        double temp = RecordArray[Idx_SEA_TEMP]->
            getInterpolatedValue( m_cursor_lon, m_cursor_lat, true );
        
        if( temp != GRIB_NOTDEF ) {
            temp = m_OverlaySettings.CalibrateValue(GribOverlaySettings::SEA_TEMPERATURE, temp);
            m_tcSeaTemperature->SetValue( wxString::Format( _T("%6.2f"), temp ) );
        }
    }

    Fit();
}

void GRIBUIDialog::OnClose( wxCloseEvent& event )
{
    pPlugIn->OnGribDialogClose();
}

void GRIBUIDialog::OnMove( wxMoveEvent& event )
{
    // Record the dialog position
    wxPoint p = GetPosition();
    pPlugIn->SetGribDialogX( p.x );
    pPlugIn->SetGribDialogY( p.y );

    event.Skip();
}

void GRIBUIDialog::OnSize( wxSizeEvent& event )
{
    // Record the dialog size
    wxSize p = event.GetSize();
    pPlugIn->SetGribDialogSizeX( p.x );
    pPlugIn->SetGribDialogSizeY( p.y );

    event.Skip();
}


void GRIBUIDialog::OnRequest( wxCommandEvent& event )
{
    double lonmax=m_vp->lon_max;
    double lonmin=m_vp->lon_min;
    if( ( fabs( m_vp->lat_max ) < 90. ) && ( fabs( lonmax ) < 360. ) ) {
           if( lonmax < -180. ) lonmax += 360.;
           if( lonmax > 180. ) lonmax -= 360.;
    }
    if( ( fabs( m_vp->lat_min ) < 90. ) && ( fabs( lonmin ) < 360. ) ) {
            if( lonmin < -180. ) lonmin += 360.;
            if( lonmin > 180. ) lonmin -= 360.;
    }
    wxString zone(
        toSAILDOC( 1, 1, m_vp->lat_max ) + wxT(",") +
        toSAILDOC( 1, 2, m_vp->lat_min ) + wxT(",") +
        toSAILDOC( 2, 2, lonmin ) + wxT(",") +
        toSAILDOC( 2, 1, lonmax ) );
    ShowSendRequest( zone );
}

void GRIBUIDialog::ShowSendRequest( wxString r_zone )
{
    wxString config = pPlugIn->GetMailRequestConfig();
    wxString r_action = wxT("send");
    wxString r_model,r_resolution,r_interval,r_period,r_parameters;
    if( config.GetChar( 1 ) == '0' ) { //GFS
        r_model = wxT("GFS:");
        if( config.GetChar( 2 ) == '0' )
            r_resolution = wxT("0.5,0.5");
        else if( config.GetChar( 2 ) == '1' )
            r_resolution = wxT("1.0,1.0");
        else if( config.GetChar( 2 ) == '2' )
            r_resolution = wxT("1.5,1.5");
        else if( config.GetChar( 2 ) == '3' )
            r_resolution = wxT("2.0,2.0");
        r_parameters = wxT("WIND,PRESS");
        if( config.GetChar( 7 ) == 'X' )
            r_parameters.Append( wxT(",WAVES") );
        if( config.GetChar( 8 ) == 'X' )
            r_parameters.Append( wxT(",APCP") );
        if( config.GetChar( 9 ) == 'X' )
            r_parameters.Append( wxT(",TCDC") );
        if( config.GetChar( 10 ) == 'X' )
            r_parameters.Append( wxT(",AIRTMP") );
        if( config.GetChar( 11 ) == 'X' )
            r_parameters.Append( wxT(",SEATMP") );
    } else if( config.GetChar( 1 ) == '1' ) { //COAMPS
        r_model = wxT("COAMPS:");
        if( config.GetChar( 2 ) == '0' )
            r_resolution = wxT("0.2,0.2");
        else if( config.GetChar( 2 ) == '1' )
            r_resolution = wxT("0.6,0.6");
        else if( config.GetChar( 2 ) == '2' )
            r_resolution = wxT("1.2,1.2");
        else if( config.GetChar( 2 ) == '3' )
            r_resolution = wxT("2.0,2.0");
        r_parameters = wxT("WIND,PRMSL");
    } else if( config.GetChar( 1 ) == '2' ) { //RTOFS
        r_model = wxT("RTOFS:");
        r_resolution = wxT("0.5,0.5");
        r_parameters = wxT("CUR,WTMP");
    }
    if( config.GetChar( 3 ) == '0' ) //ALL
        r_interval = wxT("0,3,6");
    else if( config.GetChar( 3 ) == '1' )
        r_interval = wxT("0,6,12");
    else if( config.GetChar( 3 ) == '2' )
        r_interval = wxT("0,12,24");
    if( config.GetChar( 4 ) == '0' )
        r_period = wxT("..96");
    else if( config.GetChar( 4 ) == '1' )
        r_period = wxT("..144");
    else if( config.GetChar( 4 ) == '2' )
        r_period = wxT("..192");

    wxString r_separator = wxT("|");
    wxString r_return = wxT("|=\n");
    wxString r_colon = wxT(": ");

    //display request profile
    wxString r_info,i;
    double v;
    r_info.Append( _("eMail To") + r_colon );
    r_info.Append( pPlugIn->GetSaildocAdresse() );
    r_info.Append( wxT("\n") );
    r_info.Append( _("Action") + r_colon );
    r_info.Append( _("Send") );
    r_info.Append( wxT("\n") );
    r_info.Append( _("Model") + r_colon );
    r_info.Append( r_model );
    r_info.Append( wxT("\n") );
    r_info.Append( _("Zone") + r_colon );
    r_info.Append( r_zone );
    r_info.Append( wxT("\n") );
    r_info.Append( _("Resolution") + r_colon );
    r_info.Append( r_resolution.BeforeFirst( ',' ) + _(" Deg") );
    r_info.Append( wxT("\n") );
    r_info.Append( _("Interval") + r_colon );
    i = r_interval.AfterFirst( ',' );
    r_info.Append( i.BeforeLast( ',' ) + _(" h") );
    r_info.Append( wxT("\n") );
    r_info.Append( _("Period") + r_colon );
    r_period.AfterLast( '.' ).ToDouble( &v );
    i.Printf( _T("%.0f " ), v/24. );
    r_info.Append( i );
    r_info.Append( _("Days") );
    r_info.Append( wxT("\n") );
    r_info.Append( _("Parameters") + r_colon );
    r_info.Append( r_parameters );
    
     GribPofileDisplay *r_dialog = new GribPofileDisplay(this, r_info );
     r_dialog->Fit();
     int choice = r_dialog->ShowModal();
        if( choice == wxID_OK ) { //print and send request mail

            wxMailMessage *message = new wxMailMessage(
            wxT("Grib-Request"),
            pPlugIn->GetSaildocAdresse(), //to ( saildoc request adresse )
            wxT("send ") + r_model + r_zone + r_separator + r_resolution
            + r_separator + r_interval + r_period + r_return + r_parameters, //message
            wxT("")
            );
            wxEmail mail ;
            if(mail.Send( *message ) ) {
                wxMessageDialog *dialog = new wxMessageDialog(this,
#ifdef __WXMSW__
                    _("Your request is ready. An eMail is prepared in your eMail environment.\nYou have just to click 'send' to send it.\nOK to continue ...")
#else
                    _("Your request was sent (if your system has an MTA configured and is able to send mail).\nOK to continue ...")
#endif
                    ,_("eMail"),wxOK );
                dialog->ShowModal();
            }
        }

        r_dialog->Destroy();
}

void GRIBUIDialog::OnSettings( wxCommandEvent& event )
{
    GribOverlaySettings initSettings = m_OverlaySettings;
    GribSettingsDialog *dialog = new GribSettingsDialog( *this, m_OverlaySettings, m_lastdatatype);
    if(dialog->ShowModal() == wxID_OK)
    {
        dialog->WriteSettings();
        m_OverlaySettings.Write();
    } else
        m_OverlaySettings = initSettings;

    SetFactoryOptions();
}

void GRIBUIDialog::OnPlayStop( wxCommandEvent& event )
{
    if(m_tbPlayStop->GetValue()) {
        m_tbPlayStop->SetLabel(_("Stop"));
        m_tPlayStop.Start( 1000/m_OverlaySettings.m_UpdatesPerSecond, wxTIMER_CONTINUOUS );
    } else
        m_tbPlayStop->SetLabel(_("Play"));
}

void GRIBUIDialog::OnPlayStopTimer( wxTimerEvent & )
{
    if(!m_tbPlayStop->GetValue())
        m_tPlayStop.Stop();
    else if(m_sTimeline->GetValue() >= m_sTimeline->GetMax()) {
        if(m_OverlaySettings.m_bLoopMode) {
            m_sTimeline->SetValue(0);
            TimelineChanged();
        } else {
            m_tbPlayStop->SetValue(0);
            m_tbPlayStop->SetLabel(_("Play"));
            m_tPlayStop.Stop();
        }
    } else {
        m_sTimeline->SetValue(m_sTimeline->GetValue() + m_OverlaySettings.m_SlicesPerUpdate);
        TimelineChanged();
    }
}

#if 0
void GRIBUIDialog::OnFileDirChange( wxFileDirPickerEvent &event )
{
m_pRecordTree->DeleteAllItems();
delete m_pRecordTree->m_file_id_array;

m_RecordTree_root_id = m_pRecordTree->AddRoot( _T ( "GRIBs" ) );
PopulateTreeControl();
m_pRecordTree->Expand( m_RecordTree_root_id );

pPlugIn->GetGRIBOverlayFactory()->Reset();

Refresh();

m_grib_dir = event.GetPath();
}
#endif

void GRIBUIDialog::TimelineChanged()
{
    if(!m_bGRIBActiveFile)
        return;

    wxDateTime time = TimelineTime();
    SetGribTimelineRecordSet(GetTimeLineRecordSet(time));

    /* get closest index to update combo box */
    unsigned int i;
    ArrayOfGribRecordSets *rsa = m_bGRIBActiveFile->GetRecordSetArrayPtr();
    wxDateTime itime, ip1time;
    for(i=0; i<rsa->GetCount()-1; i++) {
        itime = rsa->Item(i).m_Reference_Time;
        ip1time = rsa->Item(i+1).m_Reference_Time;
        if(ip1time >= time)
            break;
    }

    if(time - itime < ip1time - time)
        m_cRecordForecast->SetSelection(i);
    else
        m_cRecordForecast->SetSelection(i+1);

    m_cRecordForecast->ToggleWindowStyle(wxCB_READONLY);
    m_cRecordForecast->SetValue( TToString( time, pPlugIn->GetTimeZone() ) );
    m_cRecordForecast->ToggleWindowStyle(wxCB_READONLY);
    
    pPlugIn->SendTimelineMessage();
}

wxDateTime GRIBUIDialog::TimelineTime()
{
    return MinTime() + wxTimeSpan(m_sTimeline->GetValue()/m_OverlaySettings.m_HourDivider,
                                  (m_sTimeline->GetValue()%m_OverlaySettings.m_HourDivider)
                                  *60/m_OverlaySettings.m_HourDivider);
}

wxDateTime GRIBUIDialog::MinTime()
{
    ArrayOfGribRecordSets *rsa = m_bGRIBActiveFile->GetRecordSetArrayPtr();
    if(rsa && rsa->GetCount()) {
        GribRecordSet &first = rsa->Item(0);
        return first.m_Reference_Time;
    }
    return wxDateTime(0.0);
}

wxDateTime GRIBUIDialog::MaxTime()
{
    return MinTime() + wxTimeSpan(m_sTimeline->GetMax()/m_OverlaySettings.m_HourDivider);
}

GribTimelineRecordSet* GRIBUIDialog::GetTimeLineRecordSet(wxDateTime time)
{
    unsigned int i, ip1=0;
    ArrayOfGribRecordSets *rsa = m_bGRIBActiveFile->GetRecordSetArrayPtr();
    wxDateTime curtime;
    for(i=0; i<rsa->GetCount()-1; i++) {
        ip1 = i+1;
        GribRecordSet &cur=rsa->Item(ip1);
        curtime = cur.m_Reference_Time;
        if(curtime >= time)
            break;
    }
    
    wxDateTime mintime = MinTime();
    double hour2 = (curtime - mintime).GetHours();
    curtime = rsa->Item(i).m_Reference_Time;
    double hour1 = (curtime - mintime).GetHours();
    double nhour = (time - mintime).GetHours();
    
    if(hour2<hour1 || nhour < hour1 || nhour > hour2)
        return NULL;

    double interp_const;
    if(hour1 == hour2)
        interp_const = 0;
    else
        interp_const = (nhour-hour1) / (hour2-hour1);
    
    if(!m_OverlaySettings.m_bInterpolate)
        interp_const = round(interp_const);

    GribRecordSet &GRS1 = rsa->Item(i), &GRS2 = rsa->Item(ip1);
    return new GribTimelineRecordSet(GRS1, GRS2, interp_const);
}

void GRIBUIDialog::OnTimeline( wxScrollEvent& event )
{
    TimelineChanged();
}

void GRIBUIDialog::OnCBAny( wxCommandEvent& event )
{
    SetFactoryOptions(); // Reload the visibility options
}

void GRIBUIDialog::OnOpenFile( wxCommandEvent& event )
{
    // m_pGribForecastTimer.Stop();
       
    wxFileDialog *dialog = new wxFileDialog(this, _("Select a GRIB file"), pPlugIn->GetGribDirectory(),
        _T(""), wxT ( "Grib files (*.grb;*.grb.bz2|*.grb;*.grb.bz2"), wxFD_OPEN, wxDefaultPosition,
        wxDefaultSize, _T("File Dialog") );

    if( dialog->ShowModal() == wxID_OK ) {
        m_cRecordForecast->Clear();
        m_bGRIBActiveFile = NULL;
        pPlugIn->SetGribDirectory( dialog->GetDirectory() );

        m_bGRIBActiveFile = new GRIBFile( dialog->GetPath(),
            pPlugIn->GetCopyFirstCumRec(), pPlugIn->GetCopyMissWaveRec() );

        ArrayOfGribRecordSets *rsa = m_bGRIBActiveFile->GetRecordSetArrayPtr();
        if(rsa->GetCount() < 2)
            m_TimeLineHours = 0;
        else {
            GribRecordSet &first=rsa->Item(0), &last = rsa->Item(rsa->GetCount()-1);

            wxTimeSpan span = wxDateTime(last.m_Reference_Time) - wxDateTime(first.m_Reference_Time);
            m_TimeLineHours = span.GetHours();
        }

        wxFileName fn( dialog->GetPath() );
        SetLabel( fn.GetFullName() );

        if( m_bGRIBActiveFile && m_bGRIBActiveFile->IsOK() ) {
            PopulateComboDataList( 0 );
            ComputeBestForecastForNow();
        } else
            pPlugIn->GetGRIBOverlayFactory()->SetMessage( m_bGRIBActiveFile->GetLastMessage() );

        DisplayDataGRS();
        PopulateTrackingControls();
    }
}

void GRIBUIDialog::CreateActiveFileFromName( wxString filename )
{
    if( !filename.IsEmpty() ) {
        m_bGRIBActiveFile = NULL;
        m_bGRIBActiveFile = new GRIBFile( filename , pPlugIn->GetCopyFirstCumRec(),
                                          pPlugIn->GetCopyMissWaveRec() );
    }
}

void GRIBUIDialog::GetFirstrFileInDirectory()
{
    //reinitialise data containers
     m_cRecordForecast->Clear();
     m_bGRIBActiveFile = NULL;
     if( !wxDir::Exists( pPlugIn->GetGribDirectory() ) ) {
         wxStandardPaths path;
         pPlugIn->SetGribDirectory( path.GetDocumentsDir() );
    }
    // Get an array of GRIB file names in the target directory, not descending into subdirs
    wxArrayString file_array;
    int m_n_files = 0;
    m_n_files = wxDir::GetAllFiles( pPlugIn->GetGribDirectory(), &file_array, _T ( "*.grb" ), wxDIR_FILES );
    m_n_files += wxDir::GetAllFiles( pPlugIn->GetGribDirectory(), &file_array, _T ( "*.grb.bz2" ),
        wxDIR_FILES );
    if( m_n_files ) {
        file_array.Sort( CompareFileStringTime ); //sort the files by File Modification Date

        m_bGRIBActiveFile = new GRIBFile( file_array[0] , //take the younger
            pPlugIn->GetCopyFirstCumRec(), pPlugIn->GetCopyMissWaveRec() );

        wxFileName fn( file_array[0] );
        SetLabel( fn.GetFullName() );

        if( m_bGRIBActiveFile && m_bGRIBActiveFile->IsOK() ) {
            PopulateComboDataList(0);
            ComputeBestForecastForNow();
        } else
            pPlugIn->GetGRIBOverlayFactory()->SetMessage( m_bGRIBActiveFile->GetLastMessage() );
    } else {
         pPlugIn->GetGRIBOverlayFactory()->SetMessage( _("Warning : This directory is Empty!") );
         SetLabel( pPlugIn->GetGribDirectory() );
    }
}

void GRIBUIDialog::PopulateComboDataList( int index )
{
    m_cRecordForecast->Clear();

    ArrayOfGribRecordSets *rsa = m_bGRIBActiveFile->GetRecordSetArrayPtr();
    for( size_t i = 0; i < rsa->GetCount(); i++ ) {
        wxDateTime t( rsa->Item( i ).m_Reference_Time );

        m_cRecordForecast->Append( TToString( t, pPlugIn->GetTimeZone() ) );
    }
    m_cRecordForecast->SetSelection( index );
}

void GRIBUIDialog::OnPrev( wxCommandEvent& event )
{
// m_pGribForecastTimer.Stop();
    int selection = m_cRecordForecast->GetCurrentSelection() - 1;
    if(selection < 0)
        selection = 0;
    m_cRecordForecast->SetSelection( selection );
    DisplayDataGRS( );
}

void GRIBUIDialog::OnNext( wxCommandEvent& event )
{
    if( m_cRecordForecast->GetCurrentSelection() == (int)m_cRecordForecast->GetCount() - 1 ) return; //end of list

// m_pGribForecastTimer.Stop();
    m_cRecordForecast->SetSelection( m_cRecordForecast->GetCurrentSelection() + 1 );
    DisplayDataGRS( );
}

void GRIBUIDialog::ComputeBestForecastForNow()
{
    wxTimeSpan span = wxDateTime::Now() - MinTime();
    m_sTimeline->SetValue(span.GetHours()*m_OverlaySettings.m_HourDivider);

    TimelineChanged();
}

void GRIBUIDialog::DisplayDataGRS()
{
    if( m_bGRIBActiveFile && m_bGRIBActiveFile->IsOK() ) {
        ArrayOfGribRecordSets *rsa = m_bGRIBActiveFile->GetRecordSetArrayPtr();
        int selection = m_cRecordForecast->GetCurrentSelection();
        if(selection >=0) {
            GribRecordSet *record = &rsa->Item( selection );
            SelectGribRecordSet( record );
        }
    } else
        SelectGribRecordSet( NULL );
}

void GRIBUIDialog::SelectGribRecordSet( GribRecordSet *pGribRecordSet )
{
    if(!pGribRecordSet) {
        pPlugIn->GetGRIBOverlayFactory()->SetGribTimelineRecordSet(NULL);
        return;
    }

    wxDateTime mintime = MinTime(), curtime = pGribRecordSet->m_Reference_Time;
    double hour = (curtime - mintime).GetHours();

    m_sTimeline->SetValue(hour*m_OverlaySettings.m_HourDivider);
    TimelineChanged();
}

void GRIBUIDialog::SetGribTimelineRecordSet(GribTimelineRecordSet *pTimelineSet)
{
    delete m_pTimelineSet;
    m_pTimelineSet = pTimelineSet;

    if(!pPlugIn->GetGRIBOverlayFactory())
        return;
    
    pPlugIn->GetGRIBOverlayFactory()->SetGribTimelineRecordSet(m_pTimelineSet);
    SetFactoryOptions(); /* not sure if needed here */
}

void GRIBUIDialog::SetFactoryOptions()
{
    m_sTimeline->SetMax(m_TimeLineHours*m_OverlaySettings.m_HourDivider);
    if(m_pTimelineSet)
        m_pTimelineSet->ClearCachedData();

    pPlugIn->GetGRIBOverlayFactory()->ClearCachedData();

    UpdateTrackingControls();
    RequestRefresh( pParent );
}

//----------------------------------------------------------------------------------------------------------
// GRIBFile Object Implementation
//----------------------------------------------------------------------------------------------------------

GRIBFile::GRIBFile( const wxString file_name, bool CumRec, bool WaveRec )
{
    m_bOK = true; // Assume ok until proven otherwise

    if( !::wxFileExists( file_name ) ) {
        m_last_message = _( "Error: File does not exist!" );
        m_bOK = false;
        return;
    }

    // Use the zyGrib support classes, as (slightly) modified locally....

    m_pGribReader = new GribReader();

    // Read and ingest the entire GRIB file.......
    m_pGribReader->openFile( file_name );

    if( !m_pGribReader->isOk() ) {
        m_last_message = _( "Error: Can't read this File!" );
        m_bOK = false;
        return;
    }

    m_FileName = file_name;
    
    if( CumRec ) m_pGribReader->copyFirstCumulativeRecord(); //add missing records if option selected
    if( WaveRec ) m_pGribReader->copyMissingWaveRecords (); // "" ""

    m_nGribRecords = m_pGribReader->getTotalNumberOfGribRecords();

    // Walk the GribReader date list to populate our array of GribRecordSets

    std::set<time_t>::iterator iter;
    std::set<time_t> date_list = m_pGribReader->getListDates();
    for( iter = date_list.begin(); iter != date_list.end(); iter++ ) {
        GribRecordSet *t = new GribRecordSet();
        time_t reftime = *iter;
        t->m_Reference_Time = reftime;
        m_GribRecordSetArray.Add( t );
    }

    // Convert from zyGrib organization by data type/level to our organization by time.

    GribRecord *pRec;

    // Get the map of GribRecord vectors
    std::map<std::string, std::vector<GribRecord *>*> *p_map = m_pGribReader->getGribMap();

    // Iterate over the map to get vectors of related GribRecords
    std::map<std::string, std::vector<GribRecord *>*>::iterator it;
    for( it = p_map->begin(); it != p_map->end(); it++ ) {
        std::vector<GribRecord *> *ls = ( *it ).second;
        for( zuint i = 0; i < ls->size(); i++ ) {
            pRec = ls->at( i );

            time_t thistime = pRec->getRecordCurrentDate();

            // Search the GribRecordSet array for a GribRecordSet with matching time
            for( unsigned int j = 0; j < m_GribRecordSetArray.GetCount(); j++ ) {
                if( m_GribRecordSetArray.Item( j ).m_Reference_Time == thistime ) {
                    int idx = -1;
                    switch(pRec->getDataType()) {
                    case GRB_WIND_VX: idx = Idx_WIND_VX; break;
                    case GRB_WIND_VY: idx = Idx_WIND_VY; break;
                    case GRB_WIND_GUST: idx = Idx_WIND_GUST; break;
                    case GRB_PRESSURE: idx = Idx_PRESSURE; break;
                    case GRB_HTSGW: idx = Idx_HTSIGW; break;
                    case GRB_WVDIR: idx = Idx_WVDIR; break;
                    case GRB_USCT: idx = Idx_WINDSCAT_VX; break;
                    case GRB_VSCT: idx = Idx_WINDSCAT_VY; break;
                    case GRB_UOGRD: idx = Idx_SEACURRENT_VX; break;
                    case GRB_VOGRD: idx = Idx_SEACURRENT_VY; break;
                    case GRB_PRECIP_TOT: idx = Idx_PRECIP_TOT; break;
                    case GRB_CLOUD_TOT: idx = Idx_CLOUD_TOT; break;
                    case GRB_TEMP: idx = Idx_AIR_TEMP_2M; break;
                    case GRB_WTMP: idx = Idx_SEA_TEMP; break;
                    }

                    if(idx != -1)
                        m_GribRecordSetArray.Item( j ).m_GribRecordPtrArray[idx]= pRec;
                    break;
                }
            }
        }
    }
}

GRIBFile::~GRIBFile()
{
    delete m_pGribReader;
}
