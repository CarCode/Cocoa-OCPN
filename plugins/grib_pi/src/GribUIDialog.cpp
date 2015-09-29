/***************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  GRIB Object
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
#include "GribTable.h"
#include "email.h"
#include "folder.xpm"
#include "GribUIDialog.h"
#include <wx/arrimpl.cpp>

//general variables
double  m_cursor_lat, m_cursor_lon;
int     m_Altitude;
int     m_DialogStyle;
bool    m_OldZoneSelMode;
int     m_ZoneSelMode;

#ifdef __MSVC__
#if _MSC_VER < 1700
int round (double x) {
	int i = (int) x;
	if (x >= 0.0) {
		return ((x-i) >= 0.5) ? (i + 1) : (i);
	} else {
		return (-x+i >= 0.5) ? (i - 1) : (i);
	}
}
#endif
#endif

#if wxCHECK_VERSION(2,9,4) /* to work with wx 2.8 */
#define SetBitmapLabelLabel SetBitmap
#endif


#define DEFAULT_STYLE = wxCAPTION|wxCLOSE_BOX|wxSYSTEM_MENU|wxTAB_TRAVERSAL

WX_DEFINE_OBJARRAY( ArrayOfGribRecordSets );

enum SettingsDisplay {B_ARROWS, ISO_LINE, D_ARROWS, OVERLAY, NUMBERS, PARTICLES};

//    Sort compare function for File Modification Time
static int CompareFileStringTime( const wxString& first, const wxString& second )
{
    wxFileName f( first );
    wxFileName s( second );
    wxTimeSpan sp = s.GetModificationTime() - f.GetModificationTime();
    return sp.GetMinutes();

//      return ::wxFileModificationTime(first) - ::wxFileModificationTime(second);
}

//date/time in the desired time zone format
static wxString TToString( const wxDateTime date_time, const int time_zone )
{
    wxDateTime t( date_time );
    switch( time_zone ) {
        case 0:
            if( (wxDateTime::Now() == (wxDateTime::Now().ToGMT())) && t.IsDST() )  //bug in wxWingets 3.0 for UTC meridien ?
                t.Add( wxTimeSpan( 1, 0, 0, 0 ) );
            return t.Format( _T(" %a %d-%b-%Y  %H:%M "), wxDateTime::Local ) + _T("LOC");
        case 1:
        default: return t.Format( _T(" %a %d-%b-%Y %H:%M  "), wxDateTime::UTC ) + _T("UTC");
    }
}

//---------------------------------------------------------------------------------------
//          GRIB Control Implementation
//---------------------------------------------------------------------------------------
/* interpolating constructor
   as a possible optimization, write this function to also
   take latitude longitude boundaries so the resulting record can be
   a subset of the input, but also would need to be recomputed when panning the screen */
GribTimelineRecordSet::GribTimelineRecordSet(GribRecordSet &GRS1, GribRecordSet &GRS2, double interp_const)
{
    for(int i=0; i<Idx_COUNT; i++) {
        m_GribRecordPtrArray[i] = NULL;
        m_IsobarArray[i] = NULL;
    }
    
    for(int i=0; i<Idx_COUNT; i++) {
        if(m_GribRecordPtrArray[i])
            continue;

        GribRecord *GR1 = GRS1.m_GribRecordPtrArray[i];
        GribRecord *GR2 = GRS2.m_GribRecordPtrArray[i];

        if(!GR1 || !GR2)
            continue;
        
        /* if this is a vector interpolation use the 2d method */
        if(i < Idx_WIND_VY) {
            GribRecord *GR1y = GRS1.m_GribRecordPtrArray[i + Idx_WIND_VY];
            GribRecord *GR2y = GRS2.m_GribRecordPtrArray[i + Idx_WIND_VY];
            if(GR1y && GR2y) {
                m_GribRecordPtrArray[i] = GribRecord::Interpolated2DRecord
                (m_GribRecordPtrArray[i + Idx_WIND_VY], *GR1, *GR1y, *GR2, *GR2y, interp_const);
                continue;
            }
        } else if(i <= Idx_WIND_VY300)
            continue;
        else if(i == Idx_SEACURRENT_VX) {
            GribRecord *GR1y = GRS1.m_GribRecordPtrArray[Idx_SEACURRENT_VY];
            GribRecord *GR2y = GRS2.m_GribRecordPtrArray[Idx_SEACURRENT_VY];
            if(GR1y && GR2y) {
                m_GribRecordPtrArray[i] = GribRecord::Interpolated2DRecord
                (m_GribRecordPtrArray[Idx_SEACURRENT_VY], *GR1, *GR1y, *GR2, *GR2y, interp_const);
                continue;
            }
        } else if(i == Idx_SEACURRENT_VY)
            continue;
        
        m_GribRecordPtrArray[i] = GribRecord::InterpolatedRecord(*GR1, *GR2, interp_const, i == Idx_WVDIR);
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
            //    Clear out the cached isobars
            for( unsigned int j = 0; j < m_IsobarArray[i]->GetCount(); j++ ) {
                IsoLine *piso = (IsoLine *) m_IsobarArray[i]->Item( j );
                delete piso;
            }
            delete m_IsobarArray[i];
            m_IsobarArray[i] = NULL;
        }
    }
}

//---------------------------------------------------------------------------------------
//          GRIB CtrlBar Implementation
//---------------------------------------------------------------------------------------

GRIBUICtrlBar::GRIBUICtrlBar(wxWindow *parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ,grib_pi *ppi )
: GRIBUICtrlBarBase(parent, id, title, pos, size, style )
{
    pParent = parent;
    pPlugIn = ppi;
    m_vp = 0;
    pReq_Dialog = NULL;
    m_bGRIBActiveFile = NULL;
    m_pTimelineSet = NULL;
    m_gCursorData = NULL;
    m_gGRIBUICData = NULL;
    wxFileConfig *pConf = GetOCPNConfigObject();
    
    m_gGrabber = new GribGrabberWin( this );                  //add the grabber to the dialog
    m_fgCtrlGrabberSize->Add( m_gGrabber, 0, wxALL, 0 );
    
    this->SetSizer( m_fgCtrlBarSizer );
    this->Layout();
    m_fgCtrlBarSizer->Fit( this );
    
    
    if(pConf) {
        pConf->SetPath ( _T ( "/Settings/GRIB" ) );
        pConf->Read( _T ( "WindPlot" ), &m_bDataPlot[GribOverlaySettings::WIND], true );
        pConf->Read( _T ( "WindGustPlot" ), &m_bDataPlot[GribOverlaySettings::WIND_GUST], false );
        pConf->Read( _T ( "PressurePlot" ), &m_bDataPlot[GribOverlaySettings::PRESSURE], false );
        pConf->Read( _T ( "WavePlot" ), &m_bDataPlot[GribOverlaySettings::WAVE], false );
        pConf->Read( _T ( "CurrentPlot" ), &m_bDataPlot[GribOverlaySettings::CURRENT], false );
        pConf->Read( _T ( "PrecipitationPlot" ), &m_bDataPlot[GribOverlaySettings::PRECIPITATION], false );
        pConf->Read( _T ( "CloudPlot" ), &m_bDataPlot[GribOverlaySettings::CLOUD], false );
        pConf->Read( _T ( "AirTemperaturePlot" ), &m_bDataPlot[GribOverlaySettings::AIR_TEMPERATURE], false );
        pConf->Read( _T ( "SeaTemperaturePlot" ), &m_bDataPlot[GribOverlaySettings::SEA_TEMPERATURE], false );
        pConf->Read( _T ( "CAPEPlot" ), &m_bDataPlot[GribOverlaySettings::CAPE], false );

        pConf->Read( _T ( "CursorDataShown" ), &m_CDataIsShown, true );
        
        pConf->Read ( _T ( "lastdatatype" ), &m_lastdatatype, 0);
        
        pConf->Read ( _T ( "Filename" ), &m_file_name );
        
        wxStandardPathsBase& spath = wxStandardPaths::Get();
        
        pConf->SetPath ( _T ( "/Directories" ) );
        pConf->Read ( _T ( "GRIBDirectory" ), &m_grib_dir, spath.GetDocumentsDir()  );
        
        pConf->SetPath ( _T( "/PlugIns/GRIB" ) );
        pConf->Read ( _T( "ManualRequestZoneSizing" ), &m_OldZoneSelMode, false );
    }
    //init zone selection parameters
    m_ZoneSelMode = m_OldZoneSelMode ? START_SELECTION : AUTO_SELECTION;                       ////init zone selection parameters
    //set buttons bitmap
    m_bpPrev->SetBitmapLabel(wxBitmap( prev ));
    m_bpNext->SetBitmapLabel(wxBitmap( next ));
    m_bpAltitude->SetBitmapLabel(wxBitmap( altitude ));
    m_bpNow->SetBitmapLabel(wxBitmap( now ));
    m_bpZoomToCenter->SetBitmapLabel(wxBitmap( zoomto ));
    m_bpPlay->SetBitmapLabel(wxBitmap( play ));
    m_bpShowCursorData->SetBitmapLabel(wxBitmap( m_CDataIsShown ? curdata : ncurdata ));
    m_bpOpenFile->SetBitmapLabel(wxBitmap( openfile ));
    m_bpSettings->SetBitmapLabel(wxBitmap( setting ));
    SetRequestBitmap( m_ZoneSelMode );
    
    //connect Timer
    m_tPlayStop.Connect(wxEVT_TIMER, wxTimerEventHandler( GRIBUICtrlBar::OnPlayStopTimer ), NULL, this);
    //connect functions
    Connect( wxEVT_MOVE, wxMoveEventHandler( GRIBUICtrlBar::OnMove ) );
    
    m_OverlaySettings.Read();
#ifndef __WXOSX__
    DimeWindow( this );
#endif
    Fit();
    SetMinSize( GetBestSize() );
    
}

GRIBUICtrlBar::~GRIBUICtrlBar()
{
    wxFileConfig *pConf = GetOCPNConfigObject();;
    
    if(pConf) {
        pConf->SetPath ( _T ( "/Settings/GRIB" ) );
        pConf->Write( _T ( "WindPlot" ), m_bDataPlot[GribOverlaySettings::WIND]);
        pConf->Write( _T ( "WindGustPlot" ), m_bDataPlot[GribOverlaySettings::WIND_GUST]);
        pConf->Write( _T ( "PressurePlot" ), m_bDataPlot[GribOverlaySettings::PRESSURE]);
        pConf->Write( _T ( "WavePlot" ), m_bDataPlot[GribOverlaySettings::WAVE]);
        pConf->Write( _T ( "CurrentPlot" ), m_bDataPlot[GribOverlaySettings::CURRENT]);
        pConf->Write( _T ( "PrecipitationPlot" ), m_bDataPlot[GribOverlaySettings::PRECIPITATION]);
        pConf->Write( _T ( "CloudPlot" ), m_bDataPlot[GribOverlaySettings::CLOUD]);
        pConf->Write( _T ( "AirTemperaturePlot" ), m_bDataPlot[GribOverlaySettings::AIR_TEMPERATURE]);
        pConf->Write( _T ( "SeaTemperaturePlot" ), m_bDataPlot[GribOverlaySettings::SEA_TEMPERATURE]);
        pConf->Write( _T ( "CAPEPlot" ), m_bDataPlot[GribOverlaySettings::CAPE]);

        pConf->Write( _T ( "CursorDataShown" ), m_CDataIsShown );
        
        pConf->Write( _T ( "lastdatatype" ), m_lastdatatype);
        
        pConf->Write ( _T ( "Filename" ), m_file_name );
        
        pConf->SetPath ( _T ( "/Directories" ) );
        pConf->Write ( _T ( "GRIBDirectory" ), m_grib_dir );
    }
    
    delete m_pTimelineSet;
}

void GRIBUICtrlBar::SetRequestBitmap( int type )
{
    switch( type ) {
        case AUTO_SELECTION:
        case START_SELECTION:
            m_bpRequest->SetBitmapLabel(wxBitmap( request));
            m_bpRequest->SetToolTip(_("Start a request"));
            break;
        case DRAW_SELECTION:
            m_bpRequest->SetBitmapLabel(wxBitmap( selzone ));
            m_bpRequest->SetToolTip(_("Stop request"));
            break;
        case COMPLETE_SELECTION:
            m_bpRequest->SetBitmapLabel(wxBitmap(request_end));
            m_bpRequest->SetToolTip(_("Valid request"));
    }
}

void GRIBUICtrlBar::OpenFile(bool newestFile)
{
    m_bpPlay->SetBitmapLabel(wxBitmap( play ));
    m_cRecordForecast->Clear();
    pPlugIn->GetGRIBOverlayFactory()->ClearParticles();
    m_Altitude = 0;
    m_FileIntervalIndex = m_OverlaySettings.m_SlicesPerUpdate;
    delete m_bGRIBActiveFile;
    m_pTimelineSet = NULL;
    m_sTimeline->SetValue(0);
    m_TimeLineHours = 0;
    m_InterpolateMode = false;
    m_pNowMode = false;
    m_SelectionIsSaved = false;
    m_HasAltitude = false;

    //get more recent file in default directory if necessary
    wxFileName f( m_file_name );
    if( newestFile || f.GetFullName().IsEmpty() ) m_file_name = GetNewestFileInDirectory();

    m_bGRIBActiveFile = new GRIBFile( m_file_name,
                                      pPlugIn->GetCopyFirstCumRec(),
                                      pPlugIn->GetCopyMissWaveRec() );

    ArrayOfGribRecordSets *rsa = m_bGRIBActiveFile->GetRecordSetArrayPtr();

    wxFileName fn( m_file_name );
    wxString title( _("File") );
    title.Append( _T(": ") ).Append( fn.GetFullName() );

    if( m_bGRIBActiveFile ) {
        if( m_bGRIBActiveFile->IsOK() ) {
            if( rsa->GetCount() == 0 ) {                        //valid but empty file
                m_bGRIBActiveFile = NULL;
                title.Prepend( _("Error! ") ).Append( _(" contains no valid data!") );
            } else {
                PopulateComboDataList();
                title.append( _T(" (") + TToString( m_bGRIBActiveFile->GetRefDateTime(), pPlugIn->GetTimeZone()) + _T(" )"));

                if( rsa->GetCount() > 1 ) {
                    GribRecordSet &first=rsa->Item(0), &second = rsa->Item(1), &last = rsa->Item(rsa->GetCount()-1);
                    
                    //compute ntotal time span
                    wxTimeSpan span = wxDateTime(last.m_Reference_Time) - wxDateTime(first.m_Reference_Time);
                    m_TimeLineHours = span.GetHours();
                    
                    //get file interval index and update intervale choice if necessary
                    int halfintermin(wxTimeSpan(wxDateTime(second.m_Reference_Time) - wxDateTime(first.m_Reference_Time)).GetMinutes() / 2);
                    for( m_FileIntervalIndex = 0;; m_FileIntervalIndex++){
                        if(m_OverlaySettings.GetMinFromIndex(m_FileIntervalIndex) > halfintermin) break;
                    }
                    m_FileIntervalIndex--;
                    if(m_OverlaySettings.m_SlicesPerUpdate > m_FileIntervalIndex) m_OverlaySettings.m_SlicesPerUpdate = m_FileIntervalIndex;
                }
            }
        } else {
            if( fn.IsDir() ) {
                pPlugIn->GetGRIBOverlayFactory()->SetMessage( _("Warning:  Empty directory!") );
            }
            else
                title.Prepend( _("Error! ") ).Append( m_bGRIBActiveFile->GetLastMessage() );
        }
        pPlugIn->GetGRIBOverlayFactory()->SetMessage( title );
        SetTitle( title );
        SetTimeLineMax(false);
        SetFactoryOptions();
        if( pPlugIn->GetStartOptions() && m_TimeLineHours != 0)                             //fix a crash for one date files
            ComputeBestForecastForNow();
        else
            TimelineChanged();

        //populate  altitude choice and show if necessary
        for( int i = 1; i<5; i++) {
            if( (( m_pTimelineSet && m_bGRIBActiveFile->m_GribIdxArray.Index(Idx_WIND_VX + i) != wxNOT_FOUND
                  && m_bGRIBActiveFile->m_GribIdxArray.Index(Idx_WIND_VY + i) != wxNOT_FOUND )) )
                m_HasAltitude = true;
        }
        m_Altitude = 0;             //set altitude at std
        
        //enable buttons according with file contents to ovoid crashes
        m_bpSettings->Enable(m_pTimelineSet != NULL);
        m_bpZoomToCenter->Enable(m_pTimelineSet != NULL);
        
        m_sTimeline->Enable(m_pTimelineSet != NULL && m_TimeLineHours);
        m_bpPlay->Enable(m_pTimelineSet != NULL && m_TimeLineHours);
        
        m_bpPrev->Enable(m_pTimelineSet != NULL && m_TimeLineHours);
        m_bpNext->Enable(m_pTimelineSet != NULL && m_TimeLineHours);
        m_bpNow->Enable(m_pTimelineSet != NULL && m_TimeLineHours);

    }
    SetCanvasContextMenuItemViz( pPlugIn->m_MenuItem, m_TimeLineHours != 0);
}

bool GRIBUICtrlBar::GetGribZoneLimits(GribTimelineRecordSet *timelineSet, double *latmin, double *latmax, double *lonmin, double *lonmax)
{
    //calculate the largest overlay size
    GribRecord **pGR = timelineSet->m_GribRecordPtrArray;
    double ltmi = -GRIB_NOTDEF, ltma = GRIB_NOTDEF, lnmi = -GRIB_NOTDEF, lnma = GRIB_NOTDEF;
    for( int i = 0; i<36; i++){
        GribRecord *pGRA = pGR[i];
        if(!pGRA) continue;
            if(pGRA->getLatMin() < ltmi) ltmi = pGRA->getLatMin();
                if(pGRA->getLatMax() > ltma) ltma = pGRA->getLatMax();
                    if(pGRA->getLonMin() < lnmi) lnmi = pGRA->getLonMin();
                        if(pGRA->getLonMax() > lnma) lnma = pGRA->getLonMax();
    }
    if( ltmi == -GRIB_NOTDEF || lnmi == -GRIB_NOTDEF ||
        ltma ==  GRIB_NOTDEF || lnma ==  GRIB_NOTDEF)
        return false;

    if(latmin) *latmin = ltmi;
        if(latmax) *latmax = ltma;
            if(lonmin) *lonmin = lnmi;
                if(lonmax) *lonmax = lnma;
                    return true;
}

wxString GRIBUICtrlBar::GetNewestFileInDirectory()
{
    if( !wxDir::Exists( m_grib_dir ) ) {
        wxStandardPathsBase& path = wxStandardPaths::Get();
        m_grib_dir = path.GetDocumentsDir();
    }
    //    Get an array of GRIB file names in the target directory, not descending into subdirs
    wxArrayString file_array;
    int m_n_files = 0;
    m_n_files = wxDir::GetAllFiles( m_grib_dir, &file_array, _T ( "*.grb" ), wxDIR_FILES );
    m_n_files += wxDir::GetAllFiles( m_grib_dir, &file_array, _T ( "*.bz2" ),
            wxDIR_FILES );
    if( m_n_files ) {
        file_array.Sort( CompareFileStringTime );              //sort the files by File Modification Date

        return file_array[0];                                  //return the first file (the more recent one)
    } else {
        wxFileName d(m_grib_dir);
        return wxString( m_grib_dir.Append(d.GetPathSeparator()) );   //this directory is empty
    }
}

void GRIBUICtrlBar::SetCursorLatLon( double lat, double lon )
{
    m_cursor_lon = lon;
    m_cursor_lat = lat;
    
    UpdateTrackingControl();
}

void GRIBUICtrlBar::UpdateTrackingControl()
{
    if( !m_CDataIsShown ) return;

    if( m_DialogStyle >> 1== SEPARATED ) {
        if( m_gGRIBUICData ) {
            if( !m_gGRIBUICData->m_gCursorData->m_tCursorTrackTimer.IsRunning() )
                m_gGRIBUICData->m_gCursorData->m_tCursorTrackTimer.Start(50, wxTIMER_ONE_SHOT );

        }
    } else {
        if( m_gCursorData ) {
            if(!m_gCursorData->m_tCursorTrackTimer.IsRunning())
                m_gCursorData->m_tCursorTrackTimer.Start(50, wxTIMER_ONE_SHOT );
        }
    }
}

void GRIBUICtrlBar::OnShowCursorData( wxCommandEvent& event )
{
    m_CDataIsShown = !m_CDataIsShown;

    m_bpShowCursorData->SetBitmapLabel(wxBitmap( m_CDataIsShown ? curdata : ncurdata ));
    
    SetDialogsStyleSizePosition( true );
}

void GRIBUICtrlBar::SetDialogsStyleSizePosition( bool force_recompute )
{
    /*Not all plateforms accept the dynamic window style changes.
     So these changes are applied only after exit from the plugin and re-opening it*/

    if( !force_recompute && (m_old_DialogStyle == m_DialogStyle                            //recompute only if necessary
                             || (m_old_DialogStyle >> 1 == ATTACHED && m_DialogStyle >> 1 == ATTACHED)) )
        return;


    bool m_HasCaption = GetWindowStyleFlag() == (wxCAPTION|wxCLOSE_BOX|wxSYSTEM_MENU|wxTAB_TRAVERSAL);

    /* first hide grabber, detach cursordata and set ctrl/buttons visibility to have CtrlBar in his "alone" version
     altitude button visibility is a special case ( i == 0 ) */
    int state = (m_DialogStyle >> 1 == ATTACHED && m_CDataIsShown) ? 0 : 1;
    for( unsigned i = 0; i < m_OverlaySettings.m_iCtrlBarCtrlVisible[state].Len(); i++ ) {
        bool vis = i > 0 ? true : m_HasAltitude ? true : false;
        FindWindow( i + ID_CTRLALTITUDE )->Show( m_OverlaySettings.m_iCtrlBarCtrlVisible[state].GetChar(i) == _T('X') && vis );
    }
    //initiate tooltips
    m_bpShowCursorData->SetToolTip( m_CDataIsShown ? _("Hide Data at cursor" ) : _("Show Data at cursor" ) );
    m_bpPlay->SetToolTip(_("Start play back"));

    m_gGrabber->Hide();
    //then hide and detach cursor data window
    if( m_gCursorData ) {
        m_gCursorData->Hide();
        m_fgCDataSizer->Detach(m_gCursorData);
    }

    SetMinSize( wxSize(0, 0));

    //then hide eventually Cursor data dialog
    if( m_gGRIBUICData )
        m_gGRIBUICData->Hide();
    
    if( (m_DialogStyle >> 1 == SEPARATED || !m_CDataIsShown) && !m_HasCaption ) {                   // Size and show grabber if necessary
        Fit();                                                                                      // each time CtrlData dialog will be alone
        m_gGrabber->Size( GetSize().y );                                                            // or separated
        m_gGrabber->Show();
    }
    
    if( m_CDataIsShown ) {
        
        if( m_DialogStyle >> 1 == ATTACHED ) {  //dialogs attached
            //generate CursorData
            if( !m_gCursorData )
                m_gCursorData = new CursorData( this, *this );
            pPlugIn->SetDialogFont( m_gCursorData );
            m_gCursorData->PopulateTrackingControls( false );
            //attach CursorData to CtrlBar if necessary
            if( m_fgCDataSizer->GetItem( m_gCursorData ) == NULL )
                m_fgCDataSizer->Add(m_gCursorData,0);
            m_gCursorData->Show();
    
        } else if( m_DialogStyle >> 1 == SEPARATED ) { //dialogs isolated
        //buile cursor data dialog
            if( !m_gGRIBUICData )
                m_gGRIBUICData = new GRIBUICData( *this );
            m_gGRIBUICData->m_gCursorData->PopulateTrackingControls( m_DialogStyle == SEPARATED_VERTICAL );
            pPlugIn->SetDialogFont( m_gGRIBUICData->m_gCursorData );
            m_gGRIBUICData->Fit();
            m_gGRIBUICData->Update();
            m_gGRIBUICData->Show();
            
            pPlugIn->MoveDialog( m_gGRIBUICData, pPlugIn->GetCursorDataXY(), wxPoint(20, 170) );
        }
        
    }
    Layout();
    Fit();
    SetMinSize( GetBestSize() );
    SetSize( GetBestSize() );
    Update();
    pPlugIn->MoveDialog( this, pPlugIn->GetCtrlBarXY(), wxPoint(20, 60) );

    m_old_DialogStyle = m_DialogStyle;
}

void GRIBUICtrlBar::OnAltitude( wxCommandEvent& event )
{
    if( !m_HasAltitude ) return;
    
    wxMenu* amenu = new wxMenu();
    amenu->Connect( wxEVT_COMMAND_MENU_SELECTED, wxMenuEventHandler(GRIBUICtrlBar::OnMenuEvent), NULL, this );
    
    const wxString l[] = { _T(" "), wxString::Format( _T("\u2022") ) };
    for( int i = 0; i<5; i++) {
        if( (( m_pTimelineSet && m_bGRIBActiveFile->m_GribIdxArray.Index(Idx_WIND_VX + i) != wxNOT_FOUND
              && m_bGRIBActiveFile->m_GribIdxArray.Index(Idx_WIND_VY + i) != wxNOT_FOUND )) || i == 0 ) {
            MenuAppend( amenu, ID_CTRLALTITUDE + 1000 + i ,
#ifdef __WXMSW__
                       (i == m_Altitude ? l[1] : l[0]) +
#endif
                       m_OverlaySettings.GetAltitudeFromIndex( i , m_OverlaySettings.Settings[GribOverlaySettings::PRESSURE].m_Units),
                       wxITEM_RADIO );
        }
    }
    
    amenu->Check( ID_CTRLALTITUDE + 1000 + m_Altitude, true );
    
    PopupMenu( amenu );
    
    delete amenu;
}

void GRIBUICtrlBar::OnMove( wxMoveEvent& event )
{
    wxPoint p = event.GetPosition();
    pPlugIn->SetCtrlBarXY ( p );
    
    //event.Skip();
}

void GRIBUICtrlBar::OnMenuEvent( wxMenuEvent& event )
{
    int id = event.GetId();
    wxCommandEvent evt;
    evt.SetId( id );
    int alt = m_Altitude;
    switch( id ) {
            //sub menu altitude data
        case ID_CTRLALTITUDE + 1000:
            m_Altitude = 0;
            break;
        case ID_CTRLALTITUDE + 1001:
            m_Altitude = 1;
            break;
        case ID_CTRLALTITUDE + 1002:
            m_Altitude = 2;
            break;
        case ID_CTRLALTITUDE + 1003:
            m_Altitude = 3;
            break;
        case ID_CTRLALTITUDE + 1004:
            m_Altitude = 4;
            break;
            //   end sub menu
        case ID_BTNNOW:
            OnNow( evt );
            break;
        case ID_BTNZOOMTC:
            OnZoomToCenterClick( evt );
            break;
        case ID_BTNSHOWCDATA:
            OnShowCursorData( evt );
            break;
        case ID_BTNPLAY:
            OnPlayStop( evt );
            break;
        case ID_BTNOPENFILE:
            OnOpenFile( evt );
            break;
        case ID_BTNSETTING:
            OnSettings( evt );
            break;
        case ID_BTNREQUEST:
            OnRequest( evt );
    }
    if( alt != m_Altitude ) {
        SetDialogsStyleSizePosition( true );
        SetFactoryOptions();                     // Reload the visibility options
    }
}

void GRIBUICtrlBar::MenuAppend( wxMenu *menu, int id, wxString label, wxItemKind kind, wxBitmap bitmap , wxMenu *submenu )
{
    wxMenuItem *item = new wxMenuItem(menu, id, label, _T(""), kind, submenu );
    
#ifdef __WXMSW__
    wxFont *qFont = OCPNGetFont( _("Menu"), 10 );
    item->SetFont(*qFont);
#endif
    
#if defined(__WXMSW__) || defined( __WXGTK__)
    if( !bitmap.IsSameAs( wxNullBitmap ) )
        item->SetBitmap( bitmap );
#endif
    
    menu->Append( item );
    
}

void GRIBUICtrlBar::OnMouseEvent( wxMouseEvent& event )
{
    if( event.RightDown() ) {
        //populate menu
        wxMenu* xmenu = new wxMenu();
        xmenu->Connect( wxEVT_COMMAND_MENU_SELECTED, wxMenuEventHandler(GRIBUICtrlBar::OnMenuEvent), NULL, this );
        
        if( m_HasAltitude ) {    //eventually populate altitude choice
            wxMenu* smenu = new wxMenu();
            smenu->Connect( wxEVT_COMMAND_MENU_SELECTED, wxMenuEventHandler(GRIBUICtrlBar::OnMenuEvent), NULL, this );
            
            const wxString l[] = { _T(" "), wxString::Format( _T("\u2022") ) };
            for( int i = 0; i<5; i++) {
                if( (( m_pTimelineSet && m_bGRIBActiveFile->m_GribIdxArray.Index(Idx_WIND_VX + i) != wxNOT_FOUND
                      && m_bGRIBActiveFile->m_GribIdxArray.Index(Idx_WIND_VY + i) != wxNOT_FOUND )) || i == 0 ) {
                    MenuAppend( smenu, ID_CTRLALTITUDE + 1000 + i ,
#ifdef __WXMSW__
                               (i == m_Altitude ? l[1] : l[0]) +
#endif
                               m_OverlaySettings.GetAltitudeFromIndex( i , m_OverlaySettings.Settings[GribOverlaySettings::PRESSURE].m_Units),
                               wxITEM_RADIO );
                }
            }
            smenu->Check( ID_CTRLALTITUDE + 1000 + m_Altitude, true );
            MenuAppend( xmenu, wxID_ANY, _("Select geopotential altitude"), wxITEM_NORMAL, wxBitmap( altitude ), smenu);
        }
        MenuAppend( xmenu, ID_BTNNOW, _("Now"), wxITEM_NORMAL, wxBitmap( now ) );
        MenuAppend( xmenu, ID_BTNZOOMTC, _("Zoom To Center"), wxITEM_NORMAL, wxBitmap( zoomto ) );
        MenuAppend( xmenu, ID_BTNSHOWCDATA, m_CDataIsShown ? _("Hide data at cursor") : _("Show data at cursor"),
                   wxITEM_NORMAL, wxBitmap( m_CDataIsShown ? curdata : ncurdata ) );
        MenuAppend( xmenu, ID_BTNPLAY, m_tPlayStop.IsRunning() ? _("Stop play back") : _("Start play back"),
                   wxITEM_NORMAL, wxBitmap( m_tPlayStop.IsRunning() ? stop : play ) );
        MenuAppend( xmenu, ID_BTNOPENFILE, _("Open a new file"), wxITEM_NORMAL, wxBitmap( openfile ) );
        MenuAppend( xmenu, ID_BTNSETTING, _("Settings"), wxITEM_NORMAL, wxBitmap( setting ) );
        MenuAppend( xmenu, ID_BTNREQUEST, (m_ZoneSelMode == AUTO_SELECTION || m_ZoneSelMode == START_SELECTION) ?
                   _("Start a request") : m_ZoneSelMode == DRAW_SELECTION ? _( "Stop request" ) : _("Valid request"),
                   wxITEM_NORMAL, wxBitmap( (m_ZoneSelMode == AUTO_SELECTION ||
                                             m_ZoneSelMode == START_SELECTION) ? request : m_ZoneSelMode == DRAW_SELECTION ? selzone : request_end ) );
        
        PopupMenu( xmenu );
        
        delete xmenu;
        
        return;
    }

    if( m_DialogStyle >> 1 == SEPARATED ) return;
    wxMouseEvent evt(event);
    evt.SetId( 1000 );
    if( m_gCursorData && m_CDataIsShown ) m_gCursorData->OnMouseEvent (evt );
}

void GRIBUICtrlBar::ContextMenuItemCallback(int id)
{
    wxFileConfig *pConf = GetOCPNConfigObject();
#ifdef __WXOSX__
    int x = 0,y = 0,w = 0,h = 0;
#else
    int x,y,w,h;
#endif
    if(pConf) {
        pConf->SetPath ( _T ( "/Settings/GRIB" ) );

        pConf->Read( _T ( "GribDataTablePosition_x" ), &x, -1 );
        pConf->Read( _T ( "GribDataTablePosition_y" ), &y, -1 );
        pConf->Read( _T ( "GribDataTableWidth" ), &w, 900 );
        pConf->Read( _T ( "GribDataTableHeight" ), &h, 350 );
    }
    //init centered position and default size if not set yet
    if(x==-1 && y == -1) { x = (m_vp->pix_width - w) / 2; y = (m_vp->pix_height - h) /2; }

    ArrayOfGribRecordSets *rsa = m_bGRIBActiveFile->GetRecordSetArrayPtr();
    GRIBTable *table = new GRIBTable(*this);

    table->InitGribTable( pPlugIn->GetTimeZone(), rsa );
    table->m_pButtonTableOK->SetLabel(_("Close"));

    //set dialog size and position
    table->SetSize(w, h);
    table->SetPosition(wxPoint(x, y));
    table->ShowModal();

}

void GRIBUICtrlBar::SetViewPort( PlugIn_ViewPort *vp )
{
    if(m_vp == vp)  return;

    m_vp = new PlugIn_ViewPort(*vp);

    if(pReq_Dialog)
        if(pReq_Dialog->IsShown()) pReq_Dialog->OnVpChange(vp);
}

void GRIBUICtrlBar::OnClose( wxCloseEvent& event )
{
    StopPlayBack();
    if( m_gGRIBUICData ) m_gGRIBUICData->Hide();
    if(pReq_Dialog)
        if( m_ZoneSelMode > START_SELECTION ) {
            pReq_Dialog->StopGraphicalZoneSelection();
            m_ZoneSelMode = START_SELECTION;
            //SetRequestBitmap( m_ZoneSelMode );
        }
    
    pPlugIn->OnGribCtrlBarClose();
}

void GRIBUICtrlBar::OnSize( wxSizeEvent& event )
{
    //    Record the dialog size
    wxSize p = event.GetSize();
    pPlugIn->SetCtrlBarSizeXY( p );

    event.Skip();
}

void GRIBUICtrlBar::OnRequest(  wxCommandEvent& event )
{
    if( m_tPlayStop.IsRunning() ) return;                            // do nothing when play back is running !

    /*if there is one instance of the dialog already visible, do nothing*/
    if(pReq_Dialog){
        if( pReq_Dialog->IsShown() ) return;
    }
    /*a second click without selection cancel the process*/
    if( m_ZoneSelMode == DRAW_SELECTION ) {
        m_ZoneSelMode = START_SELECTION;
#ifdef __WXOSX__
        if(pReq_Dialog) pReq_Dialog->StopGraphicalZoneSelection();
#else
        pReq_Dialog->StopGraphicalZoneSelection();
#endif
        SetRequestBitmap( m_ZoneSelMode );
        return;
    }

    /*create new request dialog*/
    if( m_ZoneSelMode == AUTO_SELECTION || m_ZoneSelMode == START_SELECTION ) {
        
        delete pReq_Dialog;     //delete to be re-created
        
        pReq_Dialog = new GribRequestSetting( *this );
        pPlugIn->SetDialogFont( pReq_Dialog );
        pPlugIn->SetDialogFont( pReq_Dialog->m_sScrolledDialog );
        pReq_Dialog->OnVpChange(m_vp);
        pReq_Dialog->SetRequestDialogSize();
        //need to set a position at start
        int w;
        ::wxDisplaySize( &w, NULL);
        pReq_Dialog->Move( (w - pReq_Dialog->GetSize().GetX() ) / 3, 30 );
    } //end create new request dialog

#ifdef __WXOSX__
    if(pReq_Dialog)
    pReq_Dialog->Show( m_ZoneSelMode == AUTO_SELECTION || m_ZoneSelMode == COMPLETE_SELECTION );
#else
    pReq_Dialog->Show( m_ZoneSelMode == AUTO_SELECTION || m_ZoneSelMode == COMPLETE_SELECTION );
#endif
    m_ZoneSelMode = m_ZoneSelMode == START_SELECTION ? DRAW_SELECTION : m_ZoneSelMode == COMPLETE_SELECTION ? START_SELECTION : m_ZoneSelMode;
#ifdef __WXOSX__
    if( m_ZoneSelMode == START_SELECTION )
        if(pReq_Dialog) pReq_Dialog->StopGraphicalZoneSelection();
#else
    if( m_ZoneSelMode == START_SELECTION ) pReq_Dialog->StopGraphicalZoneSelection();
#endif
    SetRequestBitmap( m_ZoneSelMode );                   //set appopriate bitmap
}

void GRIBUICtrlBar::OnSettings( wxCommandEvent& event )
{
    if( m_tPlayStop.IsRunning() ) return;      // do nothing when play back is running !

    GribOverlaySettings initSettings = m_OverlaySettings;
    GribSettingsDialog *dialog = new GribSettingsDialog( *this, m_OverlaySettings,  m_lastdatatype, m_FileIntervalIndex);
    //set font
    pPlugIn->SetDialogFont( dialog );
    for( size_t i = 0; i < dialog->m_nSettingsBook->GetPageCount(); i++ ) {
        wxScrolledWindow *sc = ((wxScrolledWindow*) dialog->m_nSettingsBook->GetPage( i ));
        pPlugIn->SetDialogFont( sc );
    }		//end set font
    
    dialog->m_nSettingsBook->ChangeSelection( dialog->GetPageIndex() );
    dialog->SetSettingsDialogSize();
    //need to set a position at start
    int w;
    ::wxDisplaySize( &w, NULL);
    dialog->Move( (w - dialog->GetSize().GetX() ) / 2, 30 );
    // end set position
    if(dialog->ShowModal() == wxID_OK)
    {
        dialog->WriteSettings();
        m_OverlaySettings.Write();
        if( m_OverlaySettings.Settings[GribOverlaySettings::WIND].m_Units != initSettings.Settings[GribOverlaySettings::WIND].m_Units
           && (m_OverlaySettings.Settings[GribOverlaySettings::WIND].m_Units == GribOverlaySettings::BFS
               || initSettings.Settings[GribOverlaySettings::WIND].m_Units == GribOverlaySettings::BFS) )
            m_old_DialogStyle = STARTING_STATE_STYLE;                   //must recompute dialogs size if wind unit have been changed
    } else {
        m_OverlaySettings = initSettings;
        m_DialogStyle = initSettings.m_iCtrlandDataStyle;
    }
    
    dialog->SaveLastPage();
    if( !m_OverlaySettings.m_bInterpolate ) m_InterpolateMode = false;        //Interpolate could have been unchecked
    SetTimeLineMax(true);
    SetFactoryOptions();

    SetDialogsStyleSizePosition(true);
    
    event.Skip();
}

void GRIBUICtrlBar::OnPlayStop( wxCommandEvent& event )
{
    if( m_tPlayStop.IsRunning() ) {
        StopPlayBack();
    } else {
        m_bpPlay->SetBitmapLabel(wxBitmap( stop ));
        m_bpPlay->SetToolTip( _("Stop play back") );
        m_tPlayStop.Start( 1000/m_OverlaySettings.m_UpdatesPerSecond, wxTIMER_CONTINUOUS );
        m_InterpolateMode = m_OverlaySettings.m_bInterpolate;
    }
}

void GRIBUICtrlBar::OnPlayStopTimer( wxTimerEvent & event )
{
    if(m_sTimeline->GetValue() >= m_sTimeline->GetMax()) {
        if(m_OverlaySettings.m_bLoopMode) {
            if(m_OverlaySettings.m_LoopStartPoint) {
                ComputeBestForecastForNow();
                if(m_sTimeline->GetValue() >= m_sTimeline->GetMax()) StopPlayBack();        //will stop playback
                return;
            } else
                m_sTimeline->SetValue(0);
        } else {
            StopPlayBack();                                           //will stop playback
            return;
        }
    } else {
        int value = m_pNowMode ? m_OverlaySettings.m_bInterpolate ?
        GetNearestValue(GetNow(), 1) : GetNearestIndex(GetNow(), 2) : m_sTimeline->GetValue();
        m_sTimeline->SetValue(value + 1);
    }

    m_pNowMode = false;
    if(!m_InterpolateMode) m_cRecordForecast->SetSelection( m_sTimeline->GetValue() );
    TimelineChanged();
}

void GRIBUICtrlBar::StopPlayBack()
{
    if( m_tPlayStop.IsRunning() ) {
        m_tPlayStop.Stop();
        m_bpPlay->SetBitmapLabel(wxBitmap( play ));
        m_bpPlay->SetToolTip( _("Start play back") );
    }
}

void GRIBUICtrlBar::TimelineChanged()
{
    if( !m_bGRIBActiveFile || (m_bGRIBActiveFile && !m_bGRIBActiveFile->IsOK()) ) {
        pPlugIn->GetGRIBOverlayFactory()->SetGribTimelineRecordSet(NULL);
        return;
    }

    RestaureSelectionString();                      //eventually restaure the previousely saved time label

    wxDateTime time = TimelineTime();
    SetGribTimelineRecordSet(GetTimeLineRecordSet(time));

    if( !m_InterpolateMode ){
    /* get closest value to update timeline */
        ArrayOfGribRecordSets *rsa = m_bGRIBActiveFile->GetRecordSetArrayPtr();
        GribRecordSet &sel=rsa->Item(m_cRecordForecast->GetCurrentSelection());
        wxDateTime t = sel.m_Reference_Time;
        m_sTimeline->SetValue(
                    m_OverlaySettings.m_bInterpolate ?
                    wxTimeSpan(t - MinTime()).GetMinutes() / m_OverlaySettings.GetMinFromIndex(m_OverlaySettings.m_SlicesPerUpdate)
                    : m_cRecordForecast->GetCurrentSelection()
        );
    } else {
        m_cRecordForecast->SetSelection(GetNearestIndex(time, 2));
        SaveSelectionString();                                                                     //memorize index and label
        m_cRecordForecast->SetString( m_Selection_index, TToString(time, pPlugIn->GetTimeZone()) );//replace it by the interpolated time label
        m_cRecordForecast->SetStringSelection( TToString(time, pPlugIn->GetTimeZone()) );          //ensure it's visible in the box
    }

    UpdateTrackingControl();

    pPlugIn->SendTimelineMessage(time);
    RequestRefresh( pParent );
}

void GRIBUICtrlBar::RestaureSelectionString()
{
    if( !m_SelectionIsSaved ) return;
    
    int sel = m_cRecordForecast->GetSelection();
    m_cRecordForecast->SetString( m_Selection_index, m_Selection_label );
    m_cRecordForecast->SetSelection( sel );
    m_SelectionIsSaved = false;
}

int GRIBUICtrlBar::GetNearestIndex(wxDateTime time, int model)
{
    /* get closest index to update combo box */
    size_t i;
    ArrayOfGribRecordSets *rsa = m_bGRIBActiveFile->GetRecordSetArrayPtr();

    wxDateTime itime, ip1time;
    for(i=0; i<rsa->GetCount()-1; i++) {
        itime = rsa->Item(i).m_Reference_Time;
        ip1time = rsa->Item(i+1).m_Reference_Time;
        if(ip1time >= time)
            break;
    }
    if(!model) return (time - itime > (ip1time - time)*3) ? i+1 : i;

    return model == 1 ? time == ip1time ? i : i+1 : time == ip1time ? i+1 : i;
}

int GRIBUICtrlBar::GetNearestValue(wxDateTime time, int model)
{
    /* get closest value to update Time line */
    if(m_TimeLineHours == 0) return 0;
    wxDateTime itime, ip1time;
    int stepmin = m_OverlaySettings.GetMinFromIndex(m_OverlaySettings.m_SlicesPerUpdate);
    wxTimeSpan span = time - MinTime();
    int t = span.GetMinutes()/stepmin;
    itime = MinTime() + wxTimeSpan( t * stepmin / 60, (t * stepmin) % 60 );     //time at t
    ip1time = itime + wxTimeSpan( stepmin / 60, stepmin % 60 );                 //time at t+1

    if(model == 1) return time == ip1time ? t+1 : t;

    return (time - itime > (ip1time - time)*3) ? t+1 : t;
}

wxDateTime GRIBUICtrlBar::GetNow()
{
    wxDateTime now = wxDateTime::Now();
    now.GetSecond(0);

    ArrayOfGribRecordSets *rsa = m_bGRIBActiveFile->GetRecordSetArrayPtr();

    //verifie if we are outside of the file time range
    now = (now > rsa->Item(rsa->GetCount()-1).m_Reference_Time) ? rsa->Item(rsa->GetCount()-1).m_Reference_Time :
    (now < rsa->Item(0).m_Reference_Time) ? rsa->Item(0).m_Reference_Time : now;
    return now;
}

wxDateTime GRIBUICtrlBar::TimelineTime()
{
    if(m_InterpolateMode) {
        int tl = (m_TimeLineHours == 0) ? 0 : m_sTimeline->GetValue();
        int stepmin = m_OverlaySettings.GetMinFromIndex(m_OverlaySettings.m_SlicesPerUpdate);
        return MinTime() + wxTimeSpan( tl * stepmin / 60, (tl * stepmin) % 60 );
    } else {
        ArrayOfGribRecordSets *rsa = m_bGRIBActiveFile->GetRecordSetArrayPtr();
        int index = m_cRecordForecast->GetCurrentSelection() < 1 ? 0 : m_cRecordForecast->GetCurrentSelection();
        return rsa->Item(index).m_Reference_Time;
    }
}

wxDateTime GRIBUICtrlBar::MinTime()
{
    ArrayOfGribRecordSets *rsa = m_bGRIBActiveFile->GetRecordSetArrayPtr();
    if(rsa && rsa->GetCount()) {
        GribRecordSet &first = rsa->Item(0);
        return first.m_Reference_Time;
    }
    return wxDateTime(0.0);
}

GribTimelineRecordSet* GRIBUICtrlBar::GetTimeLineRecordSet(wxDateTime time)
{
    unsigned int i, im1;
    ArrayOfGribRecordSets *rsa = m_bGRIBActiveFile->GetRecordSetArrayPtr();

    if(rsa->GetCount() == 0)
        return NULL;

    wxDateTime curtime;
    for(i=0; i<rsa->GetCount(); i++) {
        GribRecordSet &cur=rsa->Item(i);
        curtime = cur.m_Reference_Time;
        if(curtime >= time)
            break;
    }
    if(i == 0)
        im1 = 0;
    else
        im1 = i-1;

    if(curtime == time) im1 = i;                            //no interpolation for record boundary

    wxDateTime mintime = MinTime();
    double minute2 = (curtime - mintime).GetMinutes();
    curtime = rsa->Item(im1).m_Reference_Time;
    double minute1 = (curtime - mintime).GetMinutes();
    double nminute = (time - mintime).GetMinutes();

    if(minute2<minute1 || nminute < minute1 || nminute > minute2)
        return NULL;

    double interp_const;
    if(minute1 == minute2)
        interp_const = 0;
    else
        interp_const = (nminute-minute1) / (minute2-minute1);

    GribRecordSet &GRS1 = rsa->Item(im1), &GRS2 = rsa->Item(i);
    return new GribTimelineRecordSet(GRS1, GRS2, interp_const);
}

void GRIBUICtrlBar::OnTimeline( wxScrollEvent& event )
{
    StopPlayBack();
    m_InterpolateMode = m_OverlaySettings.m_bInterpolate;
    if(!m_InterpolateMode) m_cRecordForecast->SetSelection(m_sTimeline->GetValue());
    m_pNowMode = false;
    TimelineChanged();
}

void GRIBUICtrlBar::OnOpenFile( wxCommandEvent& event )
{
    if( m_tPlayStop.IsRunning() ) return;      // do nothing when play back is running !

    if( !wxDir::Exists( m_grib_dir ) ) {
        wxStandardPathsBase& path = wxStandardPaths::Get();
        m_grib_dir = path.GetDocumentsDir();
    }
    wxFileDialog *dialog = new wxFileDialog(NULL, _("Select a GRIB file"), m_grib_dir,
                                            _T(""), wxT ( "Grib files (*.grb;*.bz2)|*.grb;*.bz2|All files (*)|*.*"), wxFD_OPEN, wxDefaultPosition,
                                            wxDefaultSize, _T("File Dialog") );

    if( dialog->ShowModal() == wxID_OK ) {
        m_grib_dir = dialog->GetDirectory();
        m_file_name = dialog->GetPath();
        OpenFile();
        SetDialogsStyleSizePosition( true );
    }

}

void GRIBUICtrlBar::CreateActiveFileFromName( wxString filename )
{
    if( !filename.IsEmpty() ) {
        m_bGRIBActiveFile = NULL;
        m_bGRIBActiveFile = new GRIBFile( filename , pPlugIn->GetCopyFirstCumRec(),
                                          pPlugIn->GetCopyMissWaveRec() );
    }
}

void GRIBUICtrlBar::PopulateComboDataList()
{
    int index = 0;
    if( m_cRecordForecast->GetCount() ){
        index = m_cRecordForecast->GetCurrentSelection();
        m_cRecordForecast->Clear();
    }

    ArrayOfGribRecordSets *rsa = m_bGRIBActiveFile->GetRecordSetArrayPtr();
    for( size_t i = 0; i < rsa->GetCount(); i++ ) {
        wxDateTime t( rsa->Item( i ).m_Reference_Time );

        m_cRecordForecast->Append( TToString( t, pPlugIn->GetTimeZone() ) );
    }
    m_cRecordForecast->SetSelection( index );
}

void GRIBUICtrlBar::OnZoomToCenterClick( wxCommandEvent& event )
{
    if(!m_pTimelineSet) return;

    double latmin,latmax,lonmin,lonmax;
    if(!GetGribZoneLimits(m_pTimelineSet, &latmin, &latmax, &lonmin, &lonmax ))
        return;

    //calculate overlay size
    double width = lonmax - lonmin;
    double height = latmax - latmin;

    // Calculate overlay center
    double clat = latmin + height / 2;
    double clon = lonmin + width / 2;

    //try to limit the ppm at a reasonable value
    if(width  > 120.){
        lonmin = clon - 60.;
        lonmax = clon + 60.;
    }
    if(height > 120.){
        latmin = clat - 60.;
        latmax = clat + 60.;
    }


    //Calculate overlay width & height in nm (around the center)
    double ow, oh;
    DistanceBearingMercator_Plugin(clat, lonmin, clat, lonmax, NULL, &ow );
    DistanceBearingMercator_Plugin( latmin, clon, latmax, clon, NULL, &oh );

    //calculate screen size
    int w = pPlugIn->GetGRIBOverlayFactory()->m_ParentSize.GetWidth();
    int h = pPlugIn->GetGRIBOverlayFactory()->m_ParentSize.GetHeight();

    //calculate final ppm scale to use
    double ppm;
    ppm = wxMin(w/(ow*1852), h/(oh*1852)) * ( 100 - fabs( clat ) ) / 90;

    ppm = wxMin(ppm, 1.0);

    JumpToPosition(clat, clon, ppm);

    RequestRefresh( pParent );

}

void GRIBUICtrlBar::OnPrev( wxCommandEvent& event )
{
    if( m_tPlayStop.IsRunning() ) return;      // do nothing when play back is running !

    RestaureSelectionString();

    int selection;
    if(m_pNowMode)
        selection = GetNearestIndex(GetNow(), 1);
    else if(m_InterpolateMode)
        selection = GetNearestIndex(TimelineTime(), 1);  /* set to interpolated entry */
    else
        selection = m_cRecordForecast->GetCurrentSelection();
    
    m_pNowMode = false;
    m_InterpolateMode = false;

    m_cRecordForecast->SetSelection( selection < 1 ? 0: selection - 1 );

    TimelineChanged();

}

void GRIBUICtrlBar::OnNext( wxCommandEvent& event )
{
    if( m_tPlayStop.IsRunning() ) return;      // do nothing when play back is running !

    RestaureSelectionString();

    int selection;
    if(m_pNowMode)
        selection = GetNearestIndex(GetNow(), 2);
    else if(m_InterpolateMode)
        selection = GetNearestIndex(TimelineTime(), 2);  /* set to interpolated entry */
    else
        selection = m_cRecordForecast->GetCurrentSelection();
    
    m_cRecordForecast->SetSelection( selection );

    m_pNowMode = false;
    m_InterpolateMode = false;

    if( selection == (int)m_cRecordForecast->GetCount() - 1 ) return; //end of list

    m_cRecordForecast->SetSelection( selection  + 1 );

    TimelineChanged();
}

void GRIBUICtrlBar::ComputeBestForecastForNow()
{
    if( !m_bGRIBActiveFile || (m_bGRIBActiveFile && !m_bGRIBActiveFile->IsOK()) ) {
        pPlugIn->GetGRIBOverlayFactory()->SetGribTimelineRecordSet(NULL);
        return;
    }

    wxDateTime now = GetNow();

    if( m_OverlaySettings.m_bInterpolate )
        m_sTimeline->SetValue(GetNearestValue(now, 0));
    else{
        m_cRecordForecast->SetSelection(GetNearestIndex(now, 0));
        m_sTimeline->SetValue(m_cRecordForecast->GetCurrentSelection());
    }

    if( pPlugIn->GetStartOptions() != 2 ) {         //no interpolation at start : take the nearest forecast
        m_InterpolateMode = m_OverlaySettings.m_bInterpolate;
        TimelineChanged();
        return;
    }
    //interpolation on 'now' at start
    m_InterpolateMode = true;
    m_pNowMode = true;
    SetGribTimelineRecordSet(GetTimeLineRecordSet(now));             //take current time & interpolate forecast

    RestaureSelectionString();                                       //eventually restaure the previousely saved wxChoice date time label
    m_cRecordForecast->SetSelection(GetNearestIndex(now, 2));
    SaveSelectionString();                                           //memorize the new selected wxChoice date time label
    m_cRecordForecast->SetString( m_Selection_index, TToString(now, pPlugIn->GetTimeZone()) );        //write the now date time label in the right place in wxChoice
    m_cRecordForecast->SetStringSelection( TToString(now, pPlugIn->GetTimeZone()) );                  //put it in the box

    UpdateTrackingControl();

    pPlugIn->SendTimelineMessage(now);
    RequestRefresh( pParent );

}

void GRIBUICtrlBar::SetGribTimelineRecordSet(GribTimelineRecordSet *pTimelineSet)
{
    delete m_pTimelineSet;
    m_pTimelineSet = pTimelineSet;

    if(!pPlugIn->GetGRIBOverlayFactory())
        return;

    pPlugIn->GetGRIBOverlayFactory()->SetGribTimelineRecordSet(m_pTimelineSet);
}

void GRIBUICtrlBar::SetTimeLineMax( bool SetValue )
{
    int oldmax = wxMax(m_sTimeline->GetMax(), 1), oldval = m_sTimeline->GetValue();             //memorize the old range and value

    if(m_OverlaySettings.m_bInterpolate){
        int stepmin = m_OverlaySettings.GetMinFromIndex(m_OverlaySettings.m_SlicesPerUpdate);
        m_sTimeline->SetMax(m_TimeLineHours * 60 / stepmin );
    } else {
        if(m_bGRIBActiveFile && m_bGRIBActiveFile->IsOK()) {
            ArrayOfGribRecordSets *rsa = m_bGRIBActiveFile->GetRecordSetArrayPtr();
            m_sTimeline->SetMax(rsa->GetCount()-1);
        }
    }
    //try to retrieve a coherent timeline value with the new timeline range if it has changed
    if( SetValue && m_sTimeline->GetMax() != 0 ) {
        if( m_pNowMode )
            ComputeBestForecastForNow();
        else
            m_sTimeline->SetValue( m_sTimeline->GetMax() * oldval / oldmax );
    }
}

void GRIBUICtrlBar::SetFactoryOptions()
{
    if(m_pTimelineSet)
        m_pTimelineSet->ClearCachedData();

    pPlugIn->GetGRIBOverlayFactory()->ClearCachedData();

    UpdateTrackingControl();
    RequestRefresh( pParent );
}

//----------------------------------------------------------------------------------------------------------
//          GRIBFile Object Implementation
//----------------------------------------------------------------------------------------------------------

GRIBFile::GRIBFile( const wxString file_name, bool CumRec, bool WaveRec )
{
    m_bOK = true;           // Assume ok until proven otherwise
    m_pGribReader = NULL;
    m_last_message = wxEmptyString;

    if( !::wxFileExists( file_name ) ) {
        m_last_message = _( " does not exist!" );
        m_bOK = false;
        return;
    }

    //    Use the zyGrib support classes, as (slightly) modified locally....

    m_pGribReader = new GribReader();

    //    Read and ingest the entire GRIB file.......
    m_pGribReader->openFile( file_name );

    if( !m_pGribReader->isOk() ) {
        m_last_message = _( " can't be read!" );
        m_bOK = false;
        return;
    }

    m_FileName = file_name;

    if( CumRec ) m_pGribReader->copyFirstCumulativeRecord();            //add missing records if option selected
    if( WaveRec ) m_pGribReader->copyMissingWaveRecords ();             //  ""                   ""

    m_nGribRecords = m_pGribReader->getTotalNumberOfGribRecords();

    //    Walk the GribReader date list to populate our array of GribRecordSets

    std::set<time_t>::iterator iter;
    std::set<time_t> date_list = m_pGribReader->getListDates();
    for( iter = date_list.begin(); iter != date_list.end(); iter++ ) {
        GribRecordSet *t = new GribRecordSet();
        time_t reftime = *iter;
        t->m_Reference_Time = reftime;
        m_GribRecordSetArray.Add( t );
    }

    //    Convert from zyGrib organization by data type/level to our organization by time.

    GribRecord *pRec;
    bool isOK(false);
    //    Get the map of GribRecord vectors
    std::map<std::string, std::vector<GribRecord *>*> *p_map = m_pGribReader->getGribMap();

    //    Iterate over the map to get vectors of related GribRecords
    std::map<std::string, std::vector<GribRecord *>*>::iterator it;
    for( it = p_map->begin(); it != p_map->end(); it++ ) {
        std::vector<GribRecord *> *ls = ( *it ).second;
        for( zuint i = 0; i < ls->size(); i++ ) {
            pRec = ls->at( i );
            isOK = true;
            time_t thistime = pRec->getRecordCurrentDate();

            //   Search the GribRecordSet array for a GribRecordSet with matching time
            for( unsigned int j = 0; j < m_GribRecordSetArray.GetCount(); j++ ) {
                if( m_GribRecordSetArray.Item( j ).m_Reference_Time == thistime ) {
                    int idx = -1, mdx = -1;
                    switch(pRec->getDataType()) {
                    case GRB_WIND_VX:
                        if(pRec->getLevelType() == LV_ISOBARIC){
                            switch(pRec->getLevelValue()){
                            case 300: idx = Idx_WIND_VX300;break;
                            case 500: idx = Idx_WIND_VX500;break;
                            case 700: idx = Idx_WIND_VX700;break;
                            case 850: idx = Idx_WIND_VX850;break;
                            }
                        } else
                            idx = Idx_WIND_VX;
                        break;
                    case GRB_WIND_VY:
                        if(pRec->getLevelType() == LV_ISOBARIC){
                            switch(pRec->getLevelValue()){
                            case 300: idx = Idx_WIND_VY300;break;
                            case 500: idx = Idx_WIND_VY500;break;
                            case 700: idx = Idx_WIND_VY700;break;
                            case 850: idx = Idx_WIND_VY850;break;
                            }
                        } else
                            idx = Idx_WIND_VY;
                        break;
                    case GRB_WIND_GUST: idx = Idx_WIND_GUST; break;
                    case GRB_PRESSURE: idx = Idx_PRESSURE;   break;
                    case GRB_HTSGW:    idx = Idx_HTSIGW;  break;
                    case GRB_WVPER:    idx = Idx_WVPER;  break;
                    case GRB_WVDIR:    idx = Idx_WVDIR;   break;
                    case GRB_UOGRD:    idx = Idx_SEACURRENT_VX; break;
                    case GRB_VOGRD:    idx = Idx_SEACURRENT_VY; break;
                    case GRB_PRECIP_TOT: idx = Idx_PRECIP_TOT; break;
                    case GRB_CLOUD_TOT:  idx = Idx_CLOUD_TOT; break;
                    case GRB_TEMP:
                        if(pRec->getLevelType() == LV_ISOBARIC){
                            switch(pRec->getLevelValue()){
                            case 300: idx = Idx_AIR_TEMP300;break;
                            case 500: idx = Idx_AIR_TEMP500;break;
                            case 700: idx = Idx_AIR_TEMP700;break;
                            case 850: idx = Idx_AIR_TEMP850;break;
                            }
                        } else
                            idx = Idx_AIR_TEMP;
                        if(pRec->getDataCenterModel() == NORWAY_METNO ) mdx = 1000 + NORWAY_METNO;
                        break;
                    case GRB_WTMP:
                        idx = Idx_SEA_TEMP;
                        if(pRec->getDataCenterModel() == NOAA_GFS ) mdx = 1000 + NOAA_GFS;
                        break;
                    case GRB_CAPE:      idx = Idx_CAPE;break;
                    case GRB_HUMID_REL:
                        if(pRec->getLevelType() == LV_ISOBARIC){
                            switch(pRec->getLevelValue()){
                            case 300: idx = Idx_HUMID_RE300;break;
                            case 500: idx = Idx_HUMID_RE500;break;
                            case 700: idx = Idx_HUMID_RE700;break;
                            case 850: idx = Idx_HUMID_RE850;break;
                            }
                        }
                        break;
                    case GRB_GEOPOT_HGT:
                        if(pRec->getLevelType() == LV_ISOBARIC){
                            switch(pRec->getLevelValue()){
                            case 300: idx = Idx_GEOP_HGT300;break;
                            case 500: idx = Idx_GEOP_HGT500;break;
                            case 700: idx = Idx_GEOP_HGT700;break;
                            case 850: idx = Idx_GEOP_HGT850;break;
                            }
                        }
                        break;
                    }

                    if(idx != -1) {
                        m_GribRecordSetArray.Item( j ).m_GribRecordPtrArray[idx]= pRec;
                        if(m_GribIdxArray.Index(idx) == wxNOT_FOUND ) m_GribIdxArray.Add(idx, 1);
                        if(mdx != -1 && m_GribIdxArray.Index(mdx) == wxNOT_FOUND ) m_GribIdxArray.Add(mdx, 1);
                    }
                    break;
                }
            }
        }
    }
    if(isOK) m_pRefDateTime = pRec->getRecordRefDate();     //to ovoid crash with some bad files
}

GRIBFile::~GRIBFile()
{
    delete m_pGribReader;
}

//---------------------------------------------------------------------------------------
//               GRIB Cursor Data Ctrl & Display implementation
//---------------------------------------------------------------------------------------
GRIBUICData::GRIBUICData( GRIBUICtrlBar &parent )
: GRIBUICDataBase( &parent), m_gpparent(parent)
{
    // m_gGrabber = new GribGrabberWin( this );
    //  fgSizer58->Add( m_gGrabber, 0, wxALL, 0 );
    
    m_gCursorData = new CursorData( this, m_gpparent );
    m_fgCdataSizer->Add( m_gCursorData, 0, wxALL, 0 );
    
    Connect( wxEVT_MOVE, wxMoveEventHandler( GRIBUICData::OnMove ) );
    
}

void GRIBUICData::OnMove( wxMoveEvent& event )
{
    wxPoint p = event.GetPosition();
    m_gpparent.pPlugIn->SetCursorDataXY ( p );
}
