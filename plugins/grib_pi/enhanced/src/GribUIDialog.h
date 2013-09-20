/******************************************************************************
*
* Project: OpenCPN
* Purpose: GRIB Plugin Friends
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
*/

#ifndef __GRIBUIDIALOG_H__
#define __GRIBUIDIALOG_H__

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers
#include <wx/fileconf.h>
#include <wx/glcanvas.h>

#include "GribUIDialogBase.h"
#include "GribConfigDialog.h"
#include "GribReader.h"
#include "GribRecordSet.h"
#include "IsoLine.h"

#ifndef PI
#define PI 3.1415926535897931160E0 /* pi */
#endif

class GRIBFile;
class GRIBRecord;
class GribRecordTree;
class GRIBOverlayFactory;
class GribRecordSet;

class wxFileConfig;
class grib_pi;
class wxGraphicsContext;

WX_DECLARE_OBJARRAY( GribRecordSet, ArrayOfGribRecordSets );

class GribTimelineRecordSet : public GribRecordSet
{
public:
    GribTimelineRecordSet(GribRecordSet &GRS1, GribRecordSet &GRS2, double interp_const);
    ~GribTimelineRecordSet();

    void ClearCachedData();

    /* cache isobars here to speed up rendering */
    wxArrayPtrVoid *m_IsobarArray[Idx_COUNT];
};

//----------------------------------------------------------------------------------------------------------
// GRIB Selector/Control Dialog Specification
//----------------------------------------------------------------------------------------------------------
class GRIBUIDialog: public GRIBUIDialogBase {
DECLARE_CLASS( GRIBUIDialog )DECLARE_EVENT_TABLE()
public:

    GRIBUIDialog(wxWindow *parent, grib_pi *ppi);

    ~GRIBUIDialog();
    void Init();

    void SetFilename( wxString file_name ) { m_file_name = file_name; }
    void SelectTreeControlGRS( GRIBFile *pgribfile );
    void PopulateTreeControlGRS( GRIBFile *pgribfile, int file_index );
    void SelectGribRecordSet( GribRecordSet *pGribRecordSet );
    void SetGribTimelineRecordSet(GribTimelineRecordSet *pTimelineSet);
    void SetCursorLatLon( double lat, double lon );
    void SetFactoryOptions();

    GribOverlayConfig m_OverlayConfig;

    wxTimer m_tPlayStop;

    wxDateTime TimelineTime();
    wxDateTime MinTime();
    wxDateTime MaxTime();
    GribTimelineRecordSet* GetTimeLineRecordSet(wxDateTime time);
    void TimelineChanged();

private:
    void OnClose( wxCloseEvent& event );
    void OnMove( wxMoveEvent& event );
    void OnSize( wxSizeEvent& event );
    void OnConfig( wxCommandEvent& event );
    void OnPlayStop( wxCommandEvent& event );
    void OnPlayStopTimer( wxTimerEvent & );
    void OnFileDirChange( wxFileDirPickerEvent &event );
    void UpdateTrackingControls( void );
    void PopulateTreeControl( void );

    void OnTimeline( wxCommandEvent& event );
    void OnCBAny( wxCommandEvent& event );

    // Data
    wxWindow *pParent;
    grib_pi *pPlugIn;

    int m_lastdatatype;

    wxTreeItemId m_RecordTree_root_id;

    int m_n_files;

    int m_sequence_active;

    double m_cursor_lat, m_cursor_lon;

    ArrayOfGribRecordSets *m_pTimelineBase;
    GribTimelineRecordSet *m_pTimelineSet;
    int m_TimeLineHours;

    wxString m_file_name; /* selected file */
    wxString m_grib_dir;
};

//----------------------------------------------------------------------------------------------------------
// GRIBFile Object Specification
//----------------------------------------------------------------------------------------------------------
class GRIBFile {
public:

    GRIBFile( const wxString file_name );
    ~GRIBFile();

    bool IsOK( void )
    {
        return m_bOK;
    }
    wxString GetLastErrorMessage( void )
    {
        return m_last_error_message;
    }
    ArrayOfGribRecordSets *GetRecordSetArrayPtr( void )
    {
        return &m_GribRecordSetArray;
    }

private:

    bool m_bOK;
    wxString m_last_error_message;
    GribReader *m_pGribReader;

    // An array of GribRecordSets found in this GRIB file
    ArrayOfGribRecordSets m_GribRecordSetArray;

    int m_nGribRecords;
};

#endif