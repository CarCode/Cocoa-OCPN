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

#ifndef _OBJSEARCHPI_H_
#define _OBJSEARCHPI_H_

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers

#include <map>
#include <queue>
#include <wx/fileconf.h>
//#include <wx/thread.h>
//#include <wx/event.h>

#define     PLUGIN_VERSION_MAJOR    0
#define     PLUGIN_VERSION_MINOR    4

#define     MY_API_VERSION_MAJOR    1
#define     MY_API_VERSION_MINOR    12

#include "../../../include/ocpn_plugin.h"

#include "ObjSearchDialog.h"


//----------------------------------------------------------------------------------------------------------
//    The PlugIn Class Definition
//----------------------------------------------------------------------------------------------------------

#define OBJSEARCH_TOOL_POSITION    -1          // Request default positioning of toolbar tool

class objsearch_pi;

struct Chart
{
    wxLongLong id;
    wxString name;
    double scale;
    int nativescale;
    Chart():id(-1),name(wxEmptyString),scale(0.0),nativescale(-1) { }
};

class DistanceMercatorFunc : public wxSQLite3ScalarFunction
{
public:
    DistanceMercatorFunc() {};
    void Execute(wxSQLite3FunctionContext& ctx);
};

class DbThread : public wxThread
{
public:
    DbThread(objsearch_pi * handler) : wxThread() { Create(); m_pHandler = handler; m_bIsWriting = false; }
    ~DbThread();
    void *Entry();
    bool IsWriting() { return m_bIsWriting; }
protected:
    objsearch_pi *m_pHandler;
private:
    bool m_bIsWriting;
};

class SettingsDialogImpl : public SettingsDialog
{
public:
    SettingsDialogImpl( objsearch_pi* plugin, wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Object Search Settings"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 600,440 ), long style = wxDEFAULT_DIALOG_STYLE );
    ~SettingsDialogImpl();
    void OnBrowse( wxCommandEvent& event );
    void OnOk( wxCommandEvent& event );
    void OnCancel( wxCommandEvent& event );
    void CreateObject( double lat, double lon, wxString& name, wxString& feature, wxString& source, long scale, double truescale );
    
private:
    static int ProcessCsvLine(void * frm, int cnt, const char ** cv);
    objsearch_pi *p_plugin;
    wxProgressDialog* m_prgdlg;
    int m_iProcessed;
};

class ObjSearchDialogImpl : public ObjSearchDialog
{
public:
    ObjSearchDialogImpl( objsearch_pi* plugin, wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Chart Object Search"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 660,480 ), long style = wxDEFAULT_DIALOG_STYLE );
    ~ObjSearchDialogImpl();

    void ClearFeatures();
    void AddFeature(const wxString& feature);
    
    void ClearObjects();
    void AddObject(const wxString& feature, const wxString& objectname, double lat, double lon, double dist, double scale, int nativescale, const wxString& chart);
    
    void SortResults();
    
protected:
    CheckListComboPopup* m_clcPopup;
    void OnSearch( wxCommandEvent& event );
    void OnItemSelected( wxListEvent& event );
    void OnClose( wxCommandEvent& event );
    void OnShowOnChart( wxCommandEvent& event );
    void OnSettings( wxCommandEvent& event );

private:
    wxString HumanizeFeatureName(const wxString& feature_name);
    void SaveSettings();
    objsearch_pi *p_plugin;
};

class objsearch_pi : public opencpn_plugin_112
{
public:
    objsearch_pi ( void *ppimgr );
    ~objsearch_pi ( void );

//    The required PlugIn Methods
    int Init ( void );
    bool DeInit ( void );

    int GetAPIVersionMajor();
    int GetAPIVersionMinor();
    int GetPlugInVersionMajor();
    int GetPlugInVersionMinor();
    wxBitmap *GetPlugInBitmap();
    wxString GetCommonName();
    wxString GetShortDescription();
    wxString GetLongDescription();

//    The override PlugIn Methods
    void SetCurrentViewPort(PlugIn_ViewPort &vp);
    int GetToolbarToolCount ( void );
    void OnToolbarToolCallback ( int id );
    void SetPositionFix( PlugIn_Position_Fix &pfix );
    void SendVectorChartObjectInfo( wxString &chart, wxString &feature, wxString &objname, double lat, double lon, double scale, int nativescale );
    void ShowPreferencesDialog(wxWindow * parent);

// Other public methods
    void SetColorScheme ( PI_ColorScheme cs );
    
    void FindObjects( const wxString& feature_filter, const wxString& search_string, double lat, double lon, double dist );
    
    bool GetAutoClose() { return m_bCloseOnShow; }
	int GetRangeLimit() { return m_iLimitRange; }
	void SetAutoClose(bool val) { m_bCloseOnShow = val; }
    void SetRangeLimit(int val) { m_iLimitRange = round(fromUsrDistance_Plugin(val)); }
	
    double GetLat() { if ( m_boatlat == NAN || m_boatlon == NAN) return m_vplat; else return m_boatlat; }
    double GetLon() { if ( m_boatlat == NAN || m_boatlon == NAN) return m_vplon; else return m_boatlon; }

    void SetDBThreadRunning(bool state) { m_db_thread_running = state; }
	bool IsDBThreadRunning() { return m_db_thread_running; }

protected:
    int QueryDB(const wxString& sql) { return QueryDB(m_db, sql); }
    wxString GetQuery();
    bool HasQueries();

private:
    bool LoadConfig ( void );
    bool SaveConfig ( void );
    bool m_db_thread_running;
    
    bool m_bCloseOnShow;
    int m_iLimitRange;

    wxFileConfig *m_pconfig;
    wxWindow *m_parent_window;

    int m_display_width, m_display_height;
    int m_leftclick_tool_id;
    
    ObjSearchDialogImpl *m_pObjSearchDialog;
    
    std::map<wxString, Chart> m_chartsInDb;
    std::map<wxString, int> m_featuresInDb;
    wxSQLite3Database *m_db;
    
    long GetChartId(wxString chart);
    int GetFeatureId(wxString feature);
    
    Chart StoreNewChart(wxString chart, double scale, int nativescale);
    wxLongLong StoreNewFeature(wxString feature);
    void StoreNewObject(long chart_id, long feature_id, wxString objname, double lat, double lon);
    
    wxString m_chartLoading;
    bool m_bDBUsable;
    bool m_bWaitForDB;
    
    wxSQLite3Database* initDB(void);
    void clearDB(wxSQLite3Database* db);
    
    int QueryDB(wxSQLite3Database* db, const wxString& sql);
    wxSQLite3ResultSet SelectFromDB(wxSQLite3Database* db, const wxString& sql);
    
    double m_boatlat;
    double m_boatlon;
    
    double m_vplat;
    double m_vplon;

    DbThread *m_pThread;
    wxCriticalSection m_pThreadCS; // protects the m_pThread pointer
    friend class DbThread; // allow it to access our m_pThread

    std::queue<wxString> query_queue;

    DistanceMercatorFunc distMercFunc;
};

#endif
