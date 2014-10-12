/**************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  Vector Chart Object Search Plugin
 * Author:   Pavel Kalian
 *
 ***************************************************************************
 *   Copyright (C) 2014 by Pavel Kalian                                    *
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


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/progdlg.h>
#include "wx/wxsqlite3.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include "csv_parser.h"
#include "objsearch_pi.h"

// Define NAN, which is unavailable on Windows
#ifdef _MSC_VER
#define INFINITY (DBL_MAX+DBL_MAX)
#define NAN (INFINITY-INFINITY)
#endif

//SQLite user functions

void DistanceMercatorFunc::Execute(wxSQLite3FunctionContext& ctx)
{
    if ( ctx.GetArgCount() != 4 )
    {
        ctx.SetResultError(_T("Function takes exactly 4 arguments."));
        return;
    }
    double lat0 = ctx.GetDouble(0);
    double lon0 = ctx.GetDouble(1);
    double lat1 = ctx.GetDouble(2);
    double lon1 = ctx.GetDouble(3);
    if ( lat0 > 90. || lat0 < -90. || lat1 > 90. || lat1 < -90.)
    {
        ctx.SetResultError(_T("Latitude must be between -90 and 90."));
        return;
    }
    
    if ( lon0 > 180. || lon0 < -180. || lon1 > 180. || lon1 < -180.)
    {
        ctx.SetResultError(_T("Longitude must be between -180 and 180."));
        return;
    }
    
    double dist;
    DistanceBearingMercator_Plugin(lat0, lon0, lat1, lon1, NULL, &dist);
    
    ctx.SetResult(dist);
}

// the class factories, used to create and destroy instances of the PlugIn

extern "C" DECL_EXP opencpn_plugin* create_pi ( void *ppimgr )
{
    return new objsearch_pi ( ppimgr );
}

extern "C" DECL_EXP void destroy_pi ( opencpn_plugin* p )
{
    delete p;
}

//---------------------------------------------------------------------------------------------------------
//
//    ObjSearch PlugIn Implementation
//
//---------------------------------------------------------------------------------------------------------

#include "icons.h"

//---------------------------------------------------------------------------------------------------------
//
//          PlugIn initialization and de-init
//
//---------------------------------------------------------------------------------------------------------

wxSQLite3Database* objsearch_pi::initDB(void)
{
    bool have_to_create = false;
#ifdef __WXOSX__
    wxString sDBName = *GetpPrivateApplicationDataLocation() + wxFileName::GetPathSeparator()+ wxT("opencpn/plugins/objsearch/objsearch_pi.db");
#else
    wxString sDBName = *GetpPrivateApplicationDataLocation() + wxFileName::GetPathSeparator()+ wxT("objsearch_pi.db");
#endif
    wxLogMessage( _T("OBJSEARCH_PI: Database file to be used: %s"), sDBName.c_str() );
    if ( !wxFileExists(sDBName) )
    {
        have_to_create = true;
    }
    wxSQLite3Database* db = new wxSQLite3Database();
    try
    {
        db->Open( sDBName );
    }
    catch (wxSQLite3Exception& e)
    {
        wxLogMessage( _T("OBJSEARCH_PI: DB Exception: %i : %s"), e.GetErrorCode(), e.GetMessage().c_str() );
        m_bDBUsable = false;
    }
    catch (...)
    {
        wxLogMessage( _T("OBJSEARCH_PI: Unknown exception") );
        m_bDBUsable = false;
    }

    if ( have_to_create && m_bDBUsable )
    {
        QueryDB( db, wxT("CREATE TABLE chart (id INTEGER PRIMARY KEY AUTOINCREMENT, chartname TEXT, scale REAL, nativescale INTEGER)") );
        QueryDB( db, wxT("CREATE TABLE feature (id INTEGER PRIMARY KEY AUTOINCREMENT, featurename TEXT)") );
        QueryDB( db, wxT("CREATE TABLE object (chart_id INTEGER, feature_id INTEGER, objname TEXT, lat REAL, lon REAL)") );
    }
    
    if ( m_bDBUsable )
	{
        db->CreateFunction(_T("distanceMercator"), 4, distMercFunc, true);
        //sqlite3_create_function(db, "distanceMercator", 4, SQLITE_UTF8, NULL, &distanceMercatorFunc, NULL, NULL));
		QueryDB( db, _T("PRAGMA synchronous=OFF") );
        QueryDB( db, _T("PRAGMA count_changes=OFF") );
        QueryDB( db, _T("PRAGMA journal_mode=MEMORY") );
        QueryDB( db, _T("PRAGMA temp_store=MEMORY") );
        
        //Fix the broken objects created by v 0.1 and 0.2
        QueryDB( db, _T("UPDATE object SET lon = lon - 360 WHERE lon > 180") );
        QueryDB( db, _T("UPDATE object SET lon = lon + 360 WHERE lon < - 180") );
        QueryDB( db, _T("DELETE FROM object WHERE lon < - 180 OR lon > 180 OR lat < -90 OR lat > 90") );
	}
	
    return db;
}

int objsearch_pi::QueryDB(wxSQLite3Database* db, const wxString& sql)
{
    int ret = -1;
    try
    {
        ret = db->ExecuteUpdate(sql);
    }
    catch (wxSQLite3Exception& e)
    {
        wxLogMessage( _T("OBJSEARCH_PI: DB Exception: %i : %s"), e.GetErrorCode(), e.GetMessage().c_str() );
        m_bDBUsable = false;
    }
    catch (...)
    {
        wxLogMessage( _T("OBJSEARCH_PI: Unknown exception during '%s'"), sql.c_str() );
        m_bDBUsable = false;
    }
    
    return ret;
}

wxSQLite3ResultSet objsearch_pi::SelectFromDB(wxSQLite3Database* db, const wxString& sql)
{
    if (!m_bDBUsable)
        return wxSQLite3ResultSet();
    try
    {
        return db->ExecuteQuery(sql);
    }
    catch (wxSQLite3Exception& e)
    {
        wxLogMessage( _T("OBJSEARCH_PI: DB Exception: %i : %s"), e.GetErrorCode(), e.GetMessage().c_str() );
        m_bDBUsable = false;
    }
    catch (...)
    {
        wxLogMessage( _T("OBJSEARCH_PI: Unknown exception during '%s'"), sql.c_str() );
        m_bDBUsable = false;
    }
    return wxSQLite3ResultSet();
}

void objsearch_pi::clearDB(wxSQLite3Database* db)
{
    assert(db != NULL);
    db->Close();
    delete db;
}

objsearch_pi::objsearch_pi ( void *ppimgr )
    : opencpn_plugin_112 ( ppimgr )
{
    // Create the PlugIn icons
    initialize_images();
    
    m_db_thread_running = false;
    
    m_bDBUsable = true;
    
    m_bWaitForDB = true;
    
    m_db = initDB();
    
    wxSQLite3ResultSet set;
    
    if (m_bDBUsable)
    {
        set = SelectFromDB( m_db, wxT("SELECT id, chartname, scale, nativescale FROM chart") );
        if (m_bDBUsable)
        {
            while (set.NextRow())
            {
                Chart ch;
                ch.id = set.GetInt(0);
                ch.name = set.GetAsString(1);
                ch.scale = set.GetDouble(2);
                ch.nativescale = set.GetInt(3);
                m_chartsInDb[ch.name] = ch;
            }
        }
        set.Finalize();
    }
    if (m_bDBUsable)
    {
        set = SelectFromDB( m_db, wxT("SELECT id, featurename FROM feature"));
        if (m_bDBUsable)
        {
            while (set.NextRow())
            {
                m_featuresInDb[set.GetAsString(1)] = set.GetInt(0);
            }
        }
        set.Finalize();
    }
    
    m_bWaitForDB = false;
}

objsearch_pi::~objsearch_pi ( void )
{
    clearDB(m_db);
    delete _img_objsearch_pi;
    delete _img_objsearch;
}

int objsearch_pi::Init ( void )
{
    AddLocaleCatalog ( _T ( "opencpn-objsearch_pi" ) );

    ::wxDisplaySize ( &m_display_width, &m_display_height );

    //    Get a pointer to the opencpn configuration object
    m_pconfig = GetOCPNConfigObject();
    
    //    And load the configuration items
    LoadConfig();

    // Get a pointer to the opencpn display canvas, to use as a parent for the OBJSEARCH dialog
    m_parent_window = GetOCPNCanvasWindow();

    m_leftclick_tool_id = InsertPlugInTool ( _T ( "" ), _img_objsearch, _img_objsearch, wxITEM_CHECK,
                          _ ( "Object Search" ), _T ( "" ), NULL,
                          OBJSEARCH_TOOL_POSITION, 0, this );

    m_pObjSearchDialog = new ObjSearchDialogImpl( this, m_parent_window );
    
    m_chartLoading = wxEmptyString;
    
    m_bWaitForDB = false;
    
    m_boatlat = NAN;
    m_boatlon = NAN;

    m_pThread = new DbThread(this);
    wxThreadError err = m_pThread->Run();

    if ( err != wxTHREAD_NO_ERROR )
    {
        delete m_pThread;
        m_pThread = NULL;
    }

    return ( WANTS_ONPAINT_VIEWPORT    |
             WANTS_TOOLBAR_CALLBACK    |
             INSTALLS_TOOLBAR_TOOL     |
             WANTS_CONFIG              |
             WANTS_NMEA_EVENTS         |
             WANTS_PREFERENCES         |
             WANTS_VECTOR_CHART_OBJECT_INFO
           );
}

bool objsearch_pi::DeInit ( void )
{
    if ( m_pObjSearchDialog )
    {
        m_pObjSearchDialog->Close();
        delete m_pObjSearchDialog;
        m_pObjSearchDialog = NULL;
        SaveConfig();
    }
    
    {
        wxCriticalSectionLocker enter(m_pThreadCS);
        if (m_pThread) // does the thread still exist?
        {
            while (m_pThread->IsWriting())
            {
                wxMilliSleep(10);
            }
            if (m_pThread->Delete() != wxTHREAD_NO_ERROR )
                wxLogError(_T("Can't delete the DB thread!"));
        }
    } // exit from the critical section to give the thread
        // the possibility to enter its destructor
        // (which is guarded with m_pThreadCS critical section!)
    while (1)
    {
        { // was the ~MyThread() function executed?
            wxCriticalSectionLocker enter(m_pThreadCS);
            if (!m_pThread)
                break;
        }
        // wait for thread completion
    }
    
    //Last resort check for thread completion, wait if it looks bad
    #define THREAD_WAIT_SECONDS  5
    //  Try to wait a bit to see if all compression threads exit nicely
    wxDateTime now = wxDateTime::Now();
    time_t stall = now.GetTicks();
    time_t end = stall + THREAD_WAIT_SECONDS;
    
    while(m_db_thread_running && stall < end ){
        wxDateTime later = wxDateTime::Now();
        stall = later.GetTicks();
        
        wxYield();
        wxSleep(1);
        if(!m_db_thread_running)
            break;
    }

    return true;
}

int objsearch_pi::GetAPIVersionMajor()
{
    return MY_API_VERSION_MAJOR;
}

int objsearch_pi::GetAPIVersionMinor()
{
    return MY_API_VERSION_MINOR;
}

int objsearch_pi::GetPlugInVersionMajor()
{
    return PLUGIN_VERSION_MAJOR;
}

int objsearch_pi::GetPlugInVersionMinor()
{
    return PLUGIN_VERSION_MINOR;
}

wxBitmap *objsearch_pi::GetPlugInBitmap()
{
    return _img_objsearch_pi;
}

wxString objsearch_pi::GetCommonName()
{
    return _T ( "ObjSearch" );
}


wxString objsearch_pi::GetShortDescription()
{
    return _ ( "Vector Chart Object Search PlugIn for OpenCPN" );
}


wxString objsearch_pi::GetLongDescription()
{
    return _ ( "Vector Chart Object Search PlugIn for OpenCPN\n\
Provides a search function for the named vector chart objects." );
}

int objsearch_pi::GetToolbarToolCount ( void )
{
    return 1;
}

void objsearch_pi::OnToolbarToolCallback ( int id )
{
    SetToolbarItemState( id, false );
    m_pObjSearchDialog->ClearFeatures();
    for(std::map<wxString, int>::iterator it = m_featuresInDb.begin(); it != m_featuresInDb.end(); ++it)
        m_pObjSearchDialog->AddFeature(it->first);
    
    m_pObjSearchDialog->Show();
}

void objsearch_pi::SetCurrentViewPort(PlugIn_ViewPort &vp)
{
    m_vplat = vp.clat;
    m_vplon = vp.clon;
}

bool objsearch_pi::LoadConfig ( void )
{
    wxFileConfig *pConf = ( wxFileConfig * ) m_pconfig;

    if ( !pConf )
        return false;

    pConf->SetPath ( _T ( "/PlugIns/OBJSEARCH" ) );
    
    pConf->Read ( _T("CloseOnShow" ), &m_bCloseOnShow, 1 );
    pConf->Read ( _T("LimitRange" ), &m_iLimitRange, 0 );
    return true;
}

bool objsearch_pi::SaveConfig ( void )
{
    wxFileConfig *pConf = ( wxFileConfig * ) m_pconfig;

    if ( !pConf )
        return false;

    pConf->SetPath ( _T ( "/PlugIns/OBJSEARCH" ) );
    
    pConf->Write ( _T("CloseOnShow" ), m_bCloseOnShow );
    pConf->Write ( _T("LimitRange" ), m_iLimitRange );

    return true;
}

void objsearch_pi::SetColorScheme ( PI_ColorScheme cs )
{
    DimeWindow ( m_pObjSearchDialog );
}

void objsearch_pi::SetPositionFix( PlugIn_Position_Fix &pfix )
{
    m_boatlat = pfix.Lat;
    m_boatlon = pfix.Lon;
}

void objsearch_pi::SendVectorChartObjectInfo(wxString &chart, wxString &feature, wxString &objname, double lat, double lon, double scale, int nativescale)
{
    if ( !m_bDBUsable || !IsDBThreadRunning() )
        return;
    long chart_id = GetChartId(chart);
    long feature_id = GetFeatureId(feature);
    if ( chart_id == 0 )
    {
        m_bWaitForDB = true;
        Chart ch = StoreNewChart( chart, scale, nativescale );
        m_chartsInDb[ch.name] = ch;
        m_bWaitForDB = false;
    }
    if ( feature_id == 0 )
    {
        m_bWaitForDB = true;
        feature_id = StoreNewFeature( feature ).ToLong();
        m_featuresInDb[feature] = feature_id;
        m_bWaitForDB = false;
    }
        
    if ( chart == m_chartLoading )
        StoreNewObject( chart_id, feature_id, objname, lat, lon );
}

Chart objsearch_pi::StoreNewChart(wxString chart, double scale, int nativescale)
{
    Chart ch;
    if ( !m_bDBUsable )
        return ch;
    wxFileName chartname(chart);
    ch.name = chartname.GetName();
    ch.scale = scale;
    ch.nativescale = nativescale;
    
    m_chartLoading = chart;
    QueryDB( m_db, wxString::Format(_T("INSERT INTO chart(chartname, scale, nativescale) VALUES ('%s', %f, %i)"), ch.name.c_str(), ch.scale, ch.nativescale) );
    ch.id = m_db->GetLastRowId();
    return ch;
}

wxLongLong objsearch_pi::StoreNewFeature(wxString feature)
{
    if ( !m_bDBUsable )
        return -1;
    QueryDB( m_db, wxString::Format(_T("INSERT INTO feature(featurename) VALUES ('%s')"), feature.c_str()) );
    return m_db->GetLastRowId();
}

void objsearch_pi::StoreNewObject(long chart_id, long feature_id, wxString objname, double lat, double lon)
{
    if ( !m_bDBUsable )
        return;
    
    // get object on the world chart...    
    if( lon < -180. )
        lon += 360.;
    else if( lon > 180. )
        lon -= 360.;
    
    // if it is still off the map, forget about it...
    if ( lon > 180. || lon < -180. || lat > 90. || lat < -90.)
        return;
                
    while ( m_bWaitForDB )
        wxMilliSleep(1);
    if ( objname.Len() > 1 )
    {
        wxString safe_value = objname;
        safe_value.Replace(_T("'"), _T("''"));
#ifdef __WXOSX__
        wxString sql = wxString::Format(_T("INSERT INTO object(chart_id, feature_id, objname, lat, lon) VALUES (%ld, %ld, '%s', %f, %f)"), chart_id, feature_id, safe_value.c_str(), lat, lon);
#else
        wxString sql = wxString::Format(_T("INSERT INTO object(chart_id, feature_id, objname, lat, lon) VALUES (%i, %i, '%s', %f, %f)"), chart_id, feature_id, safe_value.c_str(), lat, lon);
#endif
        query_queue.push(sql);
    }
}

long objsearch_pi::GetChartId(wxString chart)
{
    if ( !m_bDBUsable )
        return -1;
    wxFileName chartname(chart);
    wxString chrt = chartname.GetName();
    
    if(m_chartsInDb.find(chrt) == m_chartsInDb.end())
        return 0;
    else
        return m_chartsInDb[chrt].id.ToLong();
}

int objsearch_pi::GetFeatureId(wxString feature)
{
    if ( !m_bDBUsable )
        return -1;
    if(m_featuresInDb.find(feature) == m_featuresInDb.end())
        return 0;
    else
        return m_featuresInDb[feature];
}

void objsearch_pi::FindObjects( const wxString& feature_filter, const wxString& search_string, double lat, double lon, double dist )
{
    if (!m_bDBUsable)
    {
        wxMessageBox(_("There is a problem with your database, check the OpenCPN logfile for more information."));
        return;
    }
    m_pObjSearchDialog->ClearObjects();
    wxString safe_value = search_string;
    safe_value.Replace(_T("'"), _T("''"));
    wxSQLite3ResultSet set;
    if ( dist > 0.1 )
        set = SelectFromDB( m_db, wxString::Format( wxT("SELECT COUNT(*) FROM object o LEFT JOIN feature f ON (o.feature_id = f.id) WHERE instr('%s', featurename) > 0 AND objname LIKE '%%%s%%' AND distanceMercator(lat, lon, %f, %f) <= %f"), feature_filter.c_str(), safe_value.c_str(), lat, lon, dist ) );
    else
        set = SelectFromDB( m_db, wxString::Format( wxT("SELECT COUNT(*) FROM object o LEFT JOIN feature f ON (o.feature_id = f.id) WHERE instr('%s', featurename) > 0 AND objname LIKE '%%%s%%'"), feature_filter.c_str(), safe_value.c_str() ) );
    int objects_found = 0;
    if (m_bDBUsable)
    {
        objects_found = set.GetInt(0);
    }
    set.Finalize();
    int show = wxYES;
    if ( objects_found > 1000 )
    {
        show = wxMessageBox( wxString::Format( _("Your search resulted in %i objects found. This is a lot, do you really want to show all of them?"), objects_found ), _("Too many objects found"), wxYES_NO | wxCENTER );
    }
    if ( show == wxYES )
    {
        if (m_bDBUsable)
        {
            if ( dist > 0.1 )
                set = SelectFromDB( m_db, wxString::Format( wxT("SELECT f.featurename, o.objname, o.lat, o.lon, ch.scale, ch.nativescale, ch.chartname, distanceMercator(lat, lon, %f, %f) FROM object o LEFT JOIN feature f ON (o.feature_id = f.id) LEFT JOIN chart ch ON (o.chart_id = ch.id) WHERE instr('%s', featurename) > 0 AND objname LIKE '%%%s%%' AND distanceMercator(lat, lon, %f, %f) <= %f"),lat, lon, feature_filter.c_str(), safe_value.c_str(), lat, lon, dist ) );
            else
            {
                set = SelectFromDB( m_db, wxString::Format( wxT("SELECT f.featurename, o.objname, o.lat, o.lon, ch.scale, ch.nativescale, ch.chartname, distanceMercator(lat, lon, %f, %f) FROM object o LEFT JOIN feature f ON (o.feature_id = f.id) LEFT JOIN chart ch ON (o.chart_id = ch.id) WHERE instr('%s', featurename) > 0 AND objname LIKE '%%%s%%'"), lat, lon, feature_filter.c_str(), safe_value.c_str() ) );
            }
            if (m_bDBUsable)
            {
                while (set.NextRow())
                {
                    m_pObjSearchDialog->AddObject( set.GetAsString(0),  set.GetAsString(1), set.GetDouble(2), set.GetDouble(3), toUsrDistance_Plugin(set.GetDouble(7)), set.GetDouble(4), set.GetInt(5), set.GetAsString(6) );
                }
                m_pObjSearchDialog->SortResults();
            }
            set.Finalize();
        }
    }
}

ObjSearchDialogImpl::ObjSearchDialogImpl( objsearch_pi* plugin, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style )
: ObjSearchDialog(parent, id, title, pos, size, style )
{
    p_plugin = plugin;
    
    m_btnShowOnChart->Enable(false);
    
	// Set popup interface right away, otherwise some of the calls
	// below may fail
	m_clcPopup = new CheckListComboPopup();
	m_choiceFeature->SetPopupControl(m_clcPopup);
	
	m_cAutoClose->SetValue(p_plugin->GetAutoClose());
	m_scRange->SetValue(p_plugin->GetRangeLimit());
}

ObjSearchDialogImpl::~ObjSearchDialogImpl()
{
    SaveSettings();
}

void ObjSearchDialogImpl::ClearFeatures()
{
    if(m_clcPopup)
    {
        m_clcPopup->Clear();
        m_choiceFeature->SetValue(_("All"));
        ((wxCheckListBox*)m_clcPopup)->Append(_("All"));
        m_clcPopup->Check(0);
    }
}

void ObjSearchDialogImpl::AddFeature(const wxString& feature)
{
    if(m_clcPopup)
    {
        m_clcPopup->Append(HumanizeFeatureName(feature), feature);
        m_clcPopup->Check(m_clcPopup->GetCount() - 1);
    }
}

void ObjSearchDialogImpl::OnSearch( wxCommandEvent& event )
{
    if ( m_textCtrlSearchTerm->GetValue().Len() == 0 )
    {
        wxMessageBox( _("You did not enter any search term, do so.") );
        return;
    }
    p_plugin->SetAutoClose(m_cAutoClose->GetValue());
    p_plugin->SetRangeLimit(m_scRange->GetValue());
    wxString feature_filter = wxEmptyString;
    feature_filter = m_clcPopup->GetStringValue();
    p_plugin->FindObjects( feature_filter, m_textCtrlSearchTerm->GetValue(), p_plugin->GetLat(), p_plugin->GetLon(), p_plugin->GetRangeLimit() );
}

void ObjSearchDialogImpl::ClearObjects()
{
    m_listCtrlResults->ClearAll();
    wxListItem col0;
    col0.SetId(0);
    col0.SetText( _("Type") );
    col0.SetWidth(80);
    m_listCtrlResults->InsertColumn(0, col0);

    wxListItem col1;
    col1.SetId(1);
    col1.SetText( _("Name") );
    col1.SetWidth(300);
    m_listCtrlResults->InsertColumn(1, col1);

    wxListItem col2;
    col2.SetId(2);
    col2.SetText( _("Lat") );
    col2.SetWidth(80);
    m_listCtrlResults->InsertColumn(2, col2);
    
    wxListItem col3;
    col3.SetId(3);
    col3.SetText( _("Lon") );
    col3.SetWidth(80);
    m_listCtrlResults->InsertColumn(3, col3);
    
    wxListItem col4;
    col4.SetId(4);
    col4.SetText( wxString::Format( _("Dist (%s)"), getUsrDistanceUnit_Plugin(-1).c_str() ) );
    col4.SetWidth(80);
    m_listCtrlResults->InsertColumn(4, col4);
    
    wxListItem col5;
    col5.SetId(5);
    col5.SetText( _("Scale") );
    col5.SetWidth(0);
    m_listCtrlResults->InsertColumn(5, col5);
    
    wxListItem col6;
    col6.SetId(6);
    col6.SetText( _("Scale") );
    col6.SetWidth(80);
    m_listCtrlResults->InsertColumn(6, col6);
    
    wxListItem col7;
    col7.SetId(7);
    col7.SetText( _("Chart") );
    col7.SetWidth(80);
    m_listCtrlResults->InsertColumn(7, col7);
    
    m_btnShowOnChart->Enable(false);
}

void ObjSearchDialogImpl::AddObject(const wxString& feature, const wxString& objectname, double lat, double lon, double dist, double scale, int nativescale, const wxString& chart)
{  
    wxListItem item;
    int n = m_listCtrlResults->GetItemCount();
    item.SetId(n);
    item.SetText( objectname );

    m_listCtrlResults->InsertItem( item );
    
    m_listCtrlResults->SetItem(n, 0, HumanizeFeatureName(feature));
    m_listCtrlResults->SetItem(n, 1, objectname);
    m_listCtrlResults->SetItem(n, 2, toSDMM_PlugIn(1, lat));
    m_listCtrlResults->SetItem(n, 3, toSDMM_PlugIn(2, lon));
    m_listCtrlResults->SetItem(n, 4, wxString::Format(_T("%.1f"), toUsrDistance_Plugin(dist, -1)));
    m_listCtrlResults->SetItem(n, 5, wxString::Format(_T("%.4f"), scale));
    m_listCtrlResults->SetItem(n, 6, wxString::Format(_T("%i"), nativescale));
    m_listCtrlResults->SetItem(n, 7, chart);
    m_listCtrlResults->SetItemData(n, (int) (dist * 10) );
}

void ObjSearchDialogImpl::OnItemSelected( wxListEvent& event )
{
    m_btnShowOnChart->Enable();
}

void ObjSearchDialogImpl::SaveSettings()
{
    p_plugin->SetAutoClose(m_cAutoClose->GetValue());
    p_plugin->SetRangeLimit(m_scRange->GetValue());
}

void ObjSearchDialogImpl::OnClose( wxCommandEvent& event )
{
    Hide();
}

void ObjSearchDialogImpl::OnSettings( wxCommandEvent& event )
{
    Hide();
    p_plugin->ShowPreferencesDialog(m_parent);
}


double fromDMM( wxString sdms )
{
    wchar_t buf[64];
    char narrowbuf[64];
    int i, len, top = 0;
    double stk[32], sign = 1;

    //First round of string modifications to accomodate some known strange formats
    wxString replhelper;
    replhelper = wxString::FromUTF8( "´·" ); //UKHO PDFs
    sdms.Replace( replhelper, _T(".") );
    replhelper = wxString::FromUTF8( "\"·" ); //Don't know if used, but to make sure
    sdms.Replace( replhelper, _T(".") );
    replhelper = wxString::FromUTF8( "·" );
    sdms.Replace( replhelper, _T(".") );

    replhelper = wxString::FromUTF8( "s. š." ); //Another example: cs.wikipedia.org (someone was too active translating...)
    sdms.Replace( replhelper, _T("N") );
    replhelper = wxString::FromUTF8( "j. š." );
    sdms.Replace( replhelper, _T("S") );
    sdms.Replace( _T("v. d."), _T("E") );
    sdms.Replace( _T("z. d."), _T("W") );

    //If the string contains hemisphere specified by a letter, then '-' is for sure a separator...
    sdms.UpperCase();
    if( sdms.Contains( _T("N") ) || sdms.Contains( _T("S") ) || sdms.Contains( _T("E") )
            || sdms.Contains( _T("W") ) ) sdms.Replace( _T("-"), _T(" ") );

    wcsncpy( buf, sdms.wc_str( wxConvUTF8 ), 64 );
    len = wcslen( buf );

    for( i = 0; i < len; i++ ) {
        wchar_t c = buf[i];
        if( ( c >= '0' && c <= '9' ) || c == '-' || c == '.' || c == '+' ) {
            narrowbuf[i] = c;
            continue; /* Digit characters are cool as is */
        }
        if( c == ',' ) {
            narrowbuf[i] = '.'; /* convert to decimal dot */
            continue;
        }
        if( ( c | 32 ) == 'w' || ( c | 32 ) == 's' ) sign = -1; /* These mean "negate" (note case insensitivity) */
        narrowbuf[i] = 0; /* Replace everything else with nuls */
    }

    /* Build a stack of doubles */
    stk[0] = stk[1] = stk[2] = 0;
    for( i = 0; i < len; i++ ) {
        while( i < len && narrowbuf[i] == 0 )
            i++;
        if( i != len ) {
            stk[top++] = atof( narrowbuf + i );
            i += strlen( narrowbuf + i );
        }
    }

    return sign * ( stk[0] + ( stk[1] + stk[2] / 60 ) / 60 );
}

void ObjSearchDialogImpl::OnShowOnChart( wxCommandEvent& event )
{
    long itemIndex = -1;
    itemIndex = m_listCtrlResults->GetNextItem(itemIndex, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (itemIndex == -1) return;
    
    wxListItem     row_info;  
    wxString       cell_contents_string;
 
    row_info.m_itemId = itemIndex;
    row_info.m_col = 2;
    row_info.m_mask = wxLIST_MASK_TEXT;
    m_listCtrlResults->GetItem( row_info );
    double lat = fromDMM(row_info.m_text);
    
    row_info.m_col = 3;
    m_listCtrlResults->GetItem( row_info );
    double lon = fromDMM(row_info.m_text);
    
    row_info.m_col = 5;
    m_listCtrlResults->GetItem( row_info );
    double scale;
    row_info.m_text.ToDouble(&scale);
    if (scale < 0.001)
        scale = 0.001;
        
    event.Skip();
    JumpToPosition(lat, lon, scale);
    if (m_cAutoClose->GetValue())
        Hide();
}

wxString ObjSearchDialogImpl::HumanizeFeatureName(const wxString& feature_name_chart)
{
    wxString feature_name = feature_name_chart.Upper();
    wxString inland = wxEmptyString;

    if (feature_name != feature_name_chart)
        inland = _("Inland - ");

    //ENC objects (could have inland version)
    if ( feature_name == _T("ADMARE") )
        return inland + _("Administration area (Named)");
    if ( feature_name == _T("AIRARE") )
        return inland + _("Airport / airfield");
    if ( feature_name == _T("ACHBRT") )
        return inland + _("Anchor berth");
    if ( feature_name == _T("ACHARE") )
        return inland + _("Anchorage area");
    if ( feature_name == _T("BCNCAR") )
        return inland + _("Beacon, cardinal");
    if ( feature_name == _T("BCNISD") )
        return inland + _("Beacon, isolated danger");
    if ( feature_name == _T("BCNLAT") )
        return inland + _("Beacon, lateral");
    if ( feature_name == _T("BCNSAW") )
        return inland + _("Beacon, safe water");
    if ( feature_name == _T("BCNSPP") )
        return inland + _("Beacon, special purpose/general");
    if ( feature_name == _T("BERTHS") )
        return inland + _("Berth");
    if ( feature_name == _T("BRIDGE") )
        return inland + _("Bridge");
    if ( feature_name == _T("BUISGL") )
        return inland + _("Building, single");
    if ( feature_name == _T("BUAARE") )
        return inland + _("Built-up area");
    if ( feature_name == _T("BOYCAR") )
        return inland + _("Buoy, cardinal");
    if ( feature_name == _T("BOYINB") )
        return inland + _("Buoy, installation");
    if ( feature_name == _T("BOYISD") )
        return inland + _("Buoy, isolated danger");
    if ( feature_name == _T("BOYLAT") )
        return inland + _("Buoy, lateral");
    if ( feature_name == _T("BOYSAW") )
        return inland + _("Buoy, safe water");
    if ( feature_name == _T("BOYSPP") )
        return inland + _("Buoy, special purpose/general");
    if ( feature_name == _T("CBLARE") )
        return inland + _("Cable area");
    if ( feature_name == _T("CBLOHD") )
        return inland + _("Cable, overhead");
    if ( feature_name == _T("CBLSUB") )
        return inland + _("Cable, submarine");
    if ( feature_name == _T("CANALS") )
        return inland + _("Canal");
    if ( feature_name == _T("CANBNK") )
        return inland + _("Canal bank");
    if ( feature_name == _T("CTSARE") )
        return inland + _("Cargo transshipment area");
    if ( feature_name == _T("CAUSWY") )
        return inland + _("Causeway");
#ifdef __WXOSX__
    if ( feature_name == _T("CHIMNY") )
        return inland + _("Chimney");
#endif
    if ( feature_name == _T("CHKPNT") )
        return inland + _("Checkpoint");
    if ( feature_name == _T("CGUSTA") )
        return inland + _("Coastguard station");
    if ( feature_name == _T("COALNE") )
        return inland + _("Coastline");
    if ( feature_name == _T("COSARE") )
        return inland + _("Continental shelf area");
    if ( feature_name == _T("CTRPNT") )
        return inland + _("Control point");
    if ( feature_name == _T("CONVYR") )
        return inland + _("Conveyor");
    if ( feature_name == _T("CRANES") )
        return inland + _("Crane");
    if ( feature_name == _T("CURENT") )
        return inland + _("Current - non - gravitational");
    if ( feature_name == _T("DAMCON") )
        return inland + _("Dam");
    if ( feature_name == _T("DAYMAR") )
        return inland + _("Daymark");
    if ( feature_name == _T("DWRTCL") )
        return inland + _("Deep water route centerline");
    if ( feature_name == _T("DWRTPT") )
        return inland + _("Deep water route part");
    if ( feature_name == _T("DISMAR") )
        return inland + _("Distance mark");
    if ( feature_name == _T("DOCARE") )
        return inland + _("Dock area");
    if ( feature_name == _T("DRGARE") )
        return inland + _("Dredged area");
    if ( feature_name == _T("DRYDOC") )
        return inland + _("Dry dock");
    if ( feature_name == _T("DMPGRD") )
        return inland + _("Dumping ground");
    if ( feature_name == _T("FAIRWY") )
        return inland + _("Fairway");
    if ( feature_name == _T("FNCLNE") )
        return inland + _("Fence/wall");
    if ( feature_name == _T("FERYRT") )
        return inland + _("Ferry route");
    if ( feature_name == _T("FSHZNE") )
        return inland + _("Fishery zone");
    if ( feature_name == _T("FSHFAC") )
        return inland + _("Fishing facility");
    if ( feature_name == _T("FSHGRD") )
        return inland + _("Fishing ground");
    if ( feature_name == _T("FLODOC") )
        return inland + _("Floating dock");
    if ( feature_name == _T("FOGSIG") )
        return inland + _("Fog signal");
    if ( feature_name == _T("FORSTC") )
        return inland + _("Fortified structure");
    if ( feature_name == _T("FRPARE") )
        return inland + _("Free port area");
    if ( feature_name == _T("GATCON") )
        return inland + _("Gate");
    if ( feature_name == _T("GRIDRN") )
        return inland + _("Gridiron");
    if ( feature_name == _T("HRBARE") )
        return inland + _("Harbour area (administrative)");
    if ( feature_name == _T("HRBFAC") )
        return inland + _("Harbour facility");
    if ( feature_name == _T("HULKES") )
        return inland + _("Hulk");
    if ( feature_name == _T("ICEARE") )
        return inland + _("Ice area");
    if ( feature_name == _T("ICNARE") )
        return inland + _("Incineration area");
    if ( feature_name == _T("LAKARE") )
        return inland + _("Lake");
    if ( feature_name == _T("LAKSHR") )
        return inland + _("Lake shore");
    if ( feature_name == _T("LNDARE") )
        return inland + _("Land area");
    if ( feature_name == _T("LNDELV") )
        return inland + _("Land elevation");
    if ( feature_name == _T("LNDRGN") )
        return inland + _("Land region");
    if ( feature_name == _T("LNDMRK") )
        return inland + _("Landmark");
    if ( feature_name == _T("LIGHTS") )
        return inland + _("Light");
    if ( feature_name == _T("LITFLT") )
        return inland + _("Light float");
    if ( feature_name == _T("LITVES") )
        return inland + _("Light vessel");
    if ( feature_name == _T("LOCMAG") )
        return inland + _("Local magnetic anomaly");
    if ( feature_name == _T("LOKBSN") )
        return inland + _("Lock basin");
    if ( feature_name == _T("LOGPON") )
        return inland + _("Log pond");
    if ( feature_name == _T("MARCUL") )
        return inland + _("Marine farm/culture");
    if ( feature_name == _T("MIPARE") )
        return inland + _("Military practice area");
    if ( feature_name == _T("MORFAC") )
        return inland + _("Mooring/warping facility");
    if ( feature_name == _T("OBSTRN") )
        return inland + _("Obstruction");
    if ( feature_name == _T("OFSPLF") )
        return inland + _("Offshore platform");
    if ( feature_name == _T("OSPARE") )
        return inland + _("Offshore production area");
    if ( feature_name == _T("OILBAR") )
        return inland + _("Oil barrier");
    if ( feature_name == _T("PILPNT") )
        return inland + _("Pile");
    if ( feature_name == _T("PILBOP") )
        return inland + _("Pilot boarding place");
    if ( feature_name == _T("PIPARE") )
        return inland + _("Pipeline area");
    if ( feature_name == _T("PIPOHD") )
        return inland + _("Pipeline, overhead");
    if ( feature_name == _T("PIPSOL") )
        return inland + _("Pipeline, submarine/on land");
    if ( feature_name == _T("PONTON") )
        return inland + _("Pontoon");
    if ( feature_name == _T("PRDARE") )
        return inland + _("Production / storage area");
    if ( feature_name == _T("PYLONS") )
        return inland + _("Pylon/bridge support");
    if ( feature_name == _T("RADLNE") )
        return inland + _("Radar line");
    if ( feature_name == _T("RADRNG") )
        return inland + _("Radar range");
    if ( feature_name == _T("RADSTA") )
        return inland + _("Radar station");
    if ( feature_name == _T("RTPBCN") )
        return inland + _("Radar transponder beacon");
    if ( feature_name == _T("RDOCAL") )
        return inland + _("Radio calling-in point");
    if ( feature_name == _T("RDOSTA") )
        return inland + _("Radio station");
    if ( feature_name == _T("RAILWY") )
        return inland + _("Railway");
    if ( feature_name == _T("RAPIDS") )
        return inland + _("Rapids");
    if ( feature_name == _T("RCRTCL") )
        return inland + _("Recommended route centerline");
    if ( feature_name == _T("RECTRC") )
        return inland + _("Recommended track");
    if ( feature_name == _T("RSCSTA") )
        return inland + _("Rescue station");
    if ( feature_name == _T("RESARE") )
        return inland + _("Restricted area");
    if ( feature_name == _T("RIVERS") )
        return inland + _("River");
    if ( feature_name == _T("RIVBNK") )
        return inland + _("River bank");
    if ( feature_name == _T("ROADWY") )
        return inland + _("Road");
    if ( feature_name == _T("RUNWAY") )
        return inland + _("Runway");
    if ( feature_name == _T("SEAARE") )
        return inland + _("Sea area / named water area");
    if ( feature_name == _T("SPLARE") )
        return inland + _("Sea-plane landing area");
    if ( feature_name == _T("SBDARE") )
        return inland + _("Seabed area");
    if ( feature_name == _T("SLCONS") )
        return inland + _("Shoreline Construction");
    if ( feature_name == _T("SISTAT") )
        return inland + _("Signal station, traffic");
    if ( feature_name == _T("SISTAW") )
        return inland + _("Signal station, warning");
    if ( feature_name == _T("SILTNK") )
        return inland + _("Silo / tank");
    if ( feature_name == _T("SLOTOP") )
        return inland + _("Slope topline");
    if ( feature_name == _T("SLOGRD") )
        return inland + _("Sloping ground");
    if ( feature_name == _T("SMCFAC") )
        return inland + _("Small craft facility");
    if ( feature_name == _T("SOUNDG") )
        return inland + _("Sounding");
    if ( feature_name == _T("SPRING") )
        return inland + _("Spring");
    if ( feature_name == _T("SQUARE") )
        return inland + _("Square");
    if ( feature_name == _T("SUBTLN") )
        return inland + _("Submarine transit lane");
    if ( feature_name == _T("TS_PRH") )
        return inland + _("Tidal stream - harmonic prediction");
    if ( feature_name == _T("TS_PNH") )
        return inland + _("Tidal stream - non-harmonic prediction");
    if ( feature_name == _T("TS_PAD") )
        return inland + _("Tidal stream panel data");
    if ( feature_name == _T("TS_TIS") )
        return inland + _("Tidal stream - time series");
    if ( feature_name == _T("T_HMON") )
        return inland + _("Tide - harmonic prediction");
    if ( feature_name == _T("T_NHMN") )
        return inland + _("Tide - non-harmonic prediction");
    if ( feature_name == _T("T_TIMS") )
        return inland + _("Tidal stream - time series");
    if ( feature_name == _T("TIDEWY") )
        return inland + _("Tideway");
#ifdef __WXOSX__
    if ( feature_name == _T("TREPNT") )
        return inland + _("Tree");
#endif
    if ( feature_name == _T("TUNNEL") )
        return inland + _("Tunnel");
    if ( feature_name == _T("UWTROC") )
        return inland + _("Underwater rock / awash rock");
    if ( feature_name == _T("VEGATN") )
        return inland + _("Vegetation");
    if ( feature_name == _T("WATTUR") )
        return inland + _("Water turbulence");
    if ( feature_name == _T("WATFAL") )
        return inland + _("Waterfall");
    if ( feature_name == _T("WEDKLP") )
        return inland + _("Weed/Kelp");
    if ( feature_name == _T("WRECKS") )
        return inland + _("Wreck");
    if ( feature_name == _T("TS_FEB") )
        return inland + _("Tidal stream - flood/ebb");
    if ( feature_name == _T("C_AGGR") )
        return inland + _("Aggregation");
    if ( feature_name == _T("C_ASSO") )
        return inland + _("Association");
    if ( feature_name == _T("BRTFAC") )
        return inland + _("Berthing facility");
    if ( feature_name == _T("BUIREL") )
        return inland + _("Building, religious");
    if ( feature_name == _T("CTNARE") )
        return inland + _("Caution area");
    if ( feature_name == _T("MONUMT") )
        return inland + _("Monument");
    if ( feature_name == _T("PRDINS") )
        return inland + _("Production installation");
    if ( feature_name == _T("SLTPAN") )
        return inland + _("Salt pan");
    if ( feature_name == _T("TNKCON") )
        return inland + _("Tank");
    if ( feature_name == _T("TOWERS") )
        return inland + _("Tower structure");
    if ( feature_name == _T("WNDMIL") )
        return inland + _("Windmill");

    //IENC-only objects
    if ( feature_name_chart == _T("canbnk") )
        return _("Canal bank");
    if ( feature_name_chart == _T("rivbnk") )
        return _("River bank");
    if ( feature_name_chart == _T("wtwaxs") )
        return _("waterway axis");
    if ( feature_name_chart == _T("brgare") )
        return _("Bridge area");
    if ( feature_name_chart == _T("lokare") )
        return _("Lock area");
    if ( feature_name_chart == _T("bcnwtw") )
        return _("Beacon water-way");
    if ( feature_name_chart == _T("boywtw") )
        return _("Buoy water-way");
    if ( feature_name_chart == _T("rtplpt") )
        return _("Route planning point");
    if ( feature_name_chart == _T("comare") )
        return _("Communication area");
    if ( feature_name_chart == _T("wtwgag") )
        return _("Waterway gauge");
    if ( feature_name_chart == _T("termnl") )
        return _("Terminal");
    if ( feature_name_chart == _T("notmrk") )
        return _("Notice mark");
    if ( feature_name_chart == _T("bunsta") )
        return _("Bunker station");
    if ( feature_name_chart == _T("hrbbsn") )
        return _("Harbour basin");
    if ( feature_name_chart == _T("refdmp") )
        return _("Refuse dump");
    if ( feature_name_chart == _T("trnbsn") )
        return _("Turning basin");

    //CM93, "special case"
    if ( feature_name_chart == _T("_texto") )
        return _("Text label");
#ifdef __WXOSX__
    if ( feature_name_chart == _T("_extgn") )
        return _("Extended navigational add");
#endif

    //We don't know that object, just return the mnemonic
    return feature_name_chart;
}

int wxCALLBACK ObjectDistanceCompareFunction(wxIntPtr item1, wxIntPtr item2, wxIntPtr WXUNUSED(sortData))
{
    if (item1 < item2)
        return -1;
  	if (item1 > item2)
        return 1;

 	return 0;
}

void ObjSearchDialogImpl::SortResults()
{
    m_listCtrlResults->SortItems(ObjectDistanceCompareFunction, 0);
}

void *DbThread::Entry()
{
    m_pHandler->SetDBThreadRunning(true);
    while (!TestDestroy())
    {
        m_pHandler->QueryDB(_T("BEGIN TRANSACTION"));
        m_bIsWriting = true;
        while (m_pHandler->HasQueries())
        {
            m_pHandler->QueryDB(m_pHandler->GetQuery());
        }
        m_pHandler->QueryDB(_T("COMMIT TRANSACTION"));
        m_bIsWriting = false;
        Sleep(500);
        //wxQueueEvent(m_pHandler, new wxThreadEvent(wxEVT_COMMAND_DBTHREAD_UPDATE));
    }
    // signal the event handler that this thread is going to be destroyed
    // NOTE: here we assume that using the m_pHandler pointer is safe,
    // (in this case this is assured by the MyFrame destructor)
    //    wxQueueEvent(m_pHandler, new wxThreadEvent(wxEVT_COMMAND_DBTHREAD_COMPLETED));
    //return (wxThread::ExitCode)0; // success

    return 0;
}


DbThread::~DbThread()
{
    wxCriticalSectionLocker enter(m_pHandler->m_pThreadCS);
    m_pHandler->m_pThread = NULL;
    m_pHandler->SetDBThreadRunning(false);
}

wxString objsearch_pi::GetQuery()
{
    wxString query = query_queue.front();
    query_queue.pop();
    return query;
}

bool objsearch_pi::HasQueries()
{
    return !query_queue.empty();
}

void objsearch_pi::ShowPreferencesDialog(wxWindow * parent)
{
    SettingsDialogImpl* settingsdlg = new SettingsDialogImpl(this, parent);
    settingsdlg->ShowModal();
}

SettingsDialogImpl::SettingsDialogImpl( objsearch_pi* plugin, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style )
: SettingsDialog(parent, id, title, pos, size, style )
{
    p_plugin = plugin;
    m_prgdlg = NULL;
    m_iProcessed = 0;
}

SettingsDialogImpl::~SettingsDialogImpl()
{
}


void SettingsDialogImpl::OnBrowse(wxCommandEvent& event)
{
    wxFileDialog dlg(this, _T("Import data"), wxEmptyString, wxEmptyString, _("CSV files (*.csv)|*.csv|All files (*.*)|*.*"));
    if( dlg.ShowModal() == wxID_OK )
    {
        m_tPath->SetValue(dlg.GetPath());
    }
}

void SettingsDialogImpl::CreateObject( double lat, double lon, wxString& name, wxString& feature, wxString& source, long scale, double truescale )
{
    m_iProcessed++;
    p_plugin->SendVectorChartObjectInfo( source, feature, name, lat, lon, truescale, (int)scale );
    if( m_iProcessed % 10 == 0 )
        m_prgdlg->Update(m_iProcessed);
}

int SettingsDialogImpl::ProcessCsvLine(void * frm, int cnt, const char ** cv)
{
    SettingsDialogImpl* p_frm=(SettingsDialogImpl*)frm;
    
    if( cnt < 5 )
        return 0; //At least Lat, Lon, Object name, Feature name and "Source" name are needed
    double lat = 0.0;
    if( cnt >= 1)
        lat = strtod(cv[0], NULL);
    double lon = 0.0;
    if( cnt >= 2)
        lon = strtod(cv[1], NULL);
    wxString name = wxEmptyString;
    if( cnt >= 3)
        name = wxString::FromUTF8(cv[2]);
    wxString feature = wxEmptyString;
    if( cnt >= 4)
        feature = wxString::FromUTF8(cv[3]);
    wxString source = wxEmptyString;
    if( cnt >= 5)
        source = wxString::FromUTF8(cv[4]);
    long scale = -1;
    if( cnt >= 6)
        scale = strtol(cv[5], NULL, 10);
    double truescale = 0.0;
    if( cnt >= 7)
        truescale = strtod(cv[6], NULL);
    if( lat >= -90.0 && lat <= 90.0 && lon >= -180.0 && lon <= 180.0 && name != wxEmptyString && feature != wxEmptyString && source != wxEmptyString )
        p_frm->CreateObject( lat, lon, name, feature, source, scale, truescale );
    return 0;
}

void SettingsDialogImpl::OnOk(wxCommandEvent& event)
{
    if( m_tPath->GetValue() == wxEmptyString )
    {
        //TODO: perform scan
    }
    else
    {
        if( wxFileExists(m_tPath->GetValue()) )
        {
            std::ifstream inFile( m_tPath->GetValue().mb_str() );
            int linecount = std::count(std::istreambuf_iterator<char>(inFile), std::istreambuf_iterator<char>(), '\n');
            m_prgdlg = new wxProgressDialog(_("Import progress..."),
                                            wxString::Format( _("Importing data from %s."), m_tPath->GetValue().c_str() ),
                                            linecount, this	);
            m_prgdlg->Show();
            
            FILE *fp;
            if ( NULL==(fp=fopen (m_tPath->GetValue().mb_str(),"r") ) )
            {
                fprintf (stderr, "Cannot open input file sales.csv\n");
                return;
            }
            switch( csv_parse (fp, ProcessCsvLine, this) )
            {
                case E_LINE_TOO_WIDE:
                    //fprintf(stderr,"Error parsing csv: line too wide.\n");
                    break;
                case E_QUOTED_STRING:
                    //fprintf(stderr,"Error parsing csv: ill-formatted quoted string.\n");
                    break;
            }
            
            fclose (fp);
            
            m_prgdlg->Close();
            delete m_prgdlg;
            m_iProcessed = 0;
            m_prgdlg = NULL;
        }
        else
        {
            wxMessageBox( wxString::Format( _("The files %s does not exist, nothing to import."), m_tPath->GetValue().c_str() ) );
        }
    }
    this->Close();
}

void SettingsDialogImpl::OnCancel(wxCommandEvent& event)
{
    this->Close();
}

