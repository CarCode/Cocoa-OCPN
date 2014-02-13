/***************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  OpenCPN Main wxWidgets Program
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

#ifndef __CHART1_H__
#define __CHART1_H__

#include "wx/print.h"
#include "wx/datetime.h"
#include <wx/cmdline.h>
#ifndef __WXOSX__
#include <wx/snglinst.h>
#else
#include <wx/menu.h>
#endif

#ifdef __WXMSW__
#include "wx/msw/private.h"
#endif

#include "ocpn_types.h"

#include "nmea0183.h"

WX_DEFINE_ARRAY_INT(int, ArrayOfInts);

#ifdef USE_S57
#include "cpl_error.h"

//    Global Static error reporting function
extern "C" void MyCPLErrorHandler( CPLErr eErrClass, int nError,
                             const char * pszErrorMsg );
#endif

wxArrayString *EnumerateSerialPorts(void);
wxColour GetGlobalColor(wxString colorName);

int GetApplicationMemoryUse(void);

// The point for anchor watch should really be a class...
double AnchorDistFix( double const d, double const AnchorPointMinDist, double const AnchorPointMaxDist);   //  pjotrc 2010.02.22

bool TestGLCanvas(wxString &prog_dir);

class NMEA_Msg_Container;
WX_DECLARE_STRING_HASH_MAP( NMEA_Msg_Container*, MsgPriorityHash );

//    Fwd definitions
class OCPN_NMEAEvent;
class ChartCanvas;
class ocpnFloatingToolbarDialog;
class OCPN_MsgEvent;
class options;
class Track;

//----------------------------------------------------------------------------
//   constants
//----------------------------------------------------------------------------

#define TIMER_GFRAME_1 999

#define ID_QUIT         101
#define ID_CM93ZOOMG    102

//    ToolBar Constants
const int ID_TOOLBAR = 500;

enum
{
      ID_ZOOMIN = 1550,
      ID_ZOOMOUT,
      ID_STKUP,
      ID_STKDN,
      ID_ROUTE,
      ID_FOLLOW,
      ID_SETTINGS,
      ID_AIS,           // pjotrc 2010.02.09
      ID_TEXT,
      ID_CURRENT,
      ID_TIDE,
      ID_HELP,
      ID_TBEXIT,
      ID_TBSTAT,
      ID_PRINT,
      ID_COLSCHEME,
      ID_ROUTEMANAGER,
      ID_TRACK,
      ID_TBSTATBOX,
      ID_MOB,
      ID_PLUGIN_BASE

};
/*
#ifdef __WXOSX__
enum
{
    Menu_mac_Quit = wxID_EXIT,
    Menu_mac_About = wxID_ABOUT
};
#endif
*/
static const long TOOLBAR_STYLE = wxTB_FLAT | wxTB_DOCKABLE | wxTB_TEXT ;

enum
{
    IDM_TOOLBAR_TOGGLETOOLBARSIZE = 200,
    IDM_TOOLBAR_TOGGLETOOLBARORIENT,
    IDM_TOOLBAR_TOGGLETOOLBARROWS,
    IDM_TOOLBAR_ENABLEPRINT,
    IDM_TOOLBAR_DELETEPRINT,
    IDM_TOOLBAR_INSERTPRINT,
    IDM_TOOLBAR_TOGGLEHELP,
    IDM_TOOLBAR_TOGGLE_TOOLBAR,
    IDM_TOOLBAR_TOGGLE_ANOTHER_TOOLBAR,
    IDM_TOOLBAR_CHANGE_TOOLTIP,
    IDM_TOOLBAR_SHOW_TEXT,
    IDM_TOOLBAR_SHOW_ICONS,
    IDM_TOOLBAR_SHOW_BOTH,

    ID_COMBO = 1000
};



#define N_STATUS_BAR_FIELDS_MAX     20

#define STAT_FIELD_TICK             0
#define STAT_FIELD_SOGCOG           1
#define STAT_FIELD_CURSOR_LL        2
#define STAT_FIELD_CURSOR_BRGRNG    3
#define STAT_FIELD_SCALE            4

//      Define a constant GPS signal watchdog timeout value
#define GPS_TIMEOUT_SECONDS  6

//    Define a timer value for Tide/Current updates
//    Note that the underlying data algorithms produce fresh data only every 15 minutes
//    So maybe 5 minute updates should provide sufficient oversampling
#define TIMER_TC_VALUE_SECONDS      300

#define MAX_COG_AVERAGE_SECONDS        60
#define MAX_COGSOG_FILTER_SECONDS      60
//----------------------------------------------------------------------------
// fwd class declarations
//----------------------------------------------------------------------------
class ChartBase;
class wxSocketEvent;
class ocpnToolBarSimple;
class OCPN_DataStreamEvent;
class DataStream;

//      A class to contain NMEA messages, their receipt time, and their source priority
class NMEA_Msg_Container
{
public:
    wxDateTime  receipt_time;
    int         current_priority;
    wxString    stream_name;
};

//    A small class used in an array to describe chart directories
class ChartDirInfo
{
      public:
      wxString    fullpath;
      wxString    magic_number;
};

WX_DECLARE_OBJARRAY(ChartDirInfo, ArrayOfCDI);
WX_DECLARE_OBJARRAY(wxRect, ArrayOfRect);


class MyApp: public wxApp
{
  public:
    bool OnInit();
    int OnExit();
    void OnInitCmdLine(wxCmdLineParser& parser);
    bool OnCmdLineParsed(wxCmdLineParser& parser);
    void OnActivateApp(wxActivateEvent& event);

    void TrackOff(void);
#ifndef __WXOSX__
    wxSingleInstanceChecker *m_checker;
#endif
    wxDECLARE_EVENT_TABLE();

};

class MyFrame: public wxFrame
{
  public:
    MyFrame(wxFrame *frame, const wxString& title, const wxPoint& pos, const wxSize& size, long style);

    ~MyFrame();

    int GetApplicationMemoryUse(void);

    void OnEraseBackground(wxEraseEvent& event);
    void OnActivate(wxActivateEvent& event);
    void OnMaximize(wxMaximizeEvent& event);
    void OnCloseWindow(wxCloseEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnMove(wxMoveEvent& event);
    void OnFrameTimer1(wxTimerEvent& event);
    bool DoChartUpdate(void);
    void OnEvtTHREADMSG(wxCommandEvent& event);
    void OnEvtOCPN_NMEA(OCPN_DataStreamEvent & event);
    void OnEvtPlugInMessage( OCPN_MsgEvent & event );
    void OnMemFootTimer(wxTimerEvent& event);

    void UpdateAllFonts(void);
    void PositionConsole(void);
    void OnToolLeftClick(wxCommandEvent& event);
    void ClearRouteTool();
    void DoStackUp(void);
    void DoStackDown(void);

    void MouseEvent(wxMouseEvent& event);
    void SelectChartFromStack(int index,  bool bDir = false,  ChartTypeEnum New_Type = CHART_TYPE_DONTCARE, ChartFamilyEnum New_Family = CHART_FAMILY_DONTCARE);
    void SelectdbChart(int dbindex);
    void SelectQuiltRefChart(int selected_index);
    void SelectQuiltRefdbChart(int db_index);

    void JumpToPosition(double lat, double lon, double scale);

    void ProcessCanvasResize(void);

    void ApplyGlobalSettings(bool bFlyingUpdate, bool bnewtoolbar);
    void SetChartThumbnail(int index);
    int  DoOptionsDialog();
    int  ProcessOptionsDialog(int resultFlags , options* dialog );
    void DoPrint(void);
    void StopSockets(void);
    void ResumeSockets(void);
    void TogglebFollow(void);
    void ToggleFullScreen();
    void SetbFollow(void);
    void ClearbFollow(void);
    void ToggleChartOutlines(void);
    void ToggleENCText(void);
    void ToggleSoundings(void);
    void ToggleRocks(void);
    bool ToggleLights( bool doToggle = true, bool temporary = false );
    void ToggleAnchor(void);
    void TrackOn(void);
    Track *TrackOff(bool do_add_point = false);
    void TrackMidnightRestart(void);
    void ToggleColorScheme();
    int GetnChartStack(void);
    void SetToolbarItemState ( int tool_id, bool state );
    void SetToolbarItemBitmaps ( int tool_id, wxBitmap *bitmap, wxBitmap *bmpDisabled );
    void ToggleQuiltMode(void);
    void ToggleCourseUp(void);
    void SetQuiltMode(bool bquilt);
    bool GetQuiltMode(void);
    void UpdateControlBar(void);
    void RemoveChartFromQuilt(int dbIndex);

    void SubmergeToolbar(void);
    void SubmergeToolbarIfOverlap(int x, int y, int margin = 0);
    void SurfaceToolbar(void);

    void HandlePianoClick(int selected_index, int selected_dbIndex);
    void HandlePianoRClick(int x, int y,int selected_index, int selected_dbIndex);
    void HandlePianoRollover(int selected_index, int selected_dbIndex);
    void HandlePianoRolloverIcon(int selected_index, int selected_dbIndex);

    void PianoPopupMenu ( int x, int y, int selected_index, int selected_dbIndex );
    void OnPianoMenuDisableChart(wxCommandEvent& event);
    void OnPianoMenuEnableChart(wxCommandEvent& event);
#ifdef __WXOSX__
//    void MacSetCommonMenuBar(wxMenuBar* menubar);
    wxMenu *mac_menu;
    void onAbout(wxCommandEvent& event);
    void onEinst(wxCommandEvent& event);
    void onHelp(wxCommandEvent& event);
    void startHelp(void);
    void onZoomin(wxCommandEvent& event);
    void onZoomout(wxCommandEvent& event);
    void onStackup(wxCommandEvent& event);
    void onStackdown(wxCommandEvent& event);
    void onFollow(wxCommandEvent& event);
    void onText(wxCommandEvent& event);
    void onAis(wxCommandEvent& event);
    void onCurrent(wxCommandEvent& event);
    void onTide(wxCommandEvent& event);
#endif
    void SetGroupIndex(int index);
    double GetTrueOrMag(double a);

    double GetBestVPScale(ChartBase *pchart);

    ChartCanvas *GetCanvasWindow(){ return m_pchart_canvas; }
    void SetCanvasWindow(ChartCanvas *pcanv){ m_pchart_canvas = pcanv; }

    ColorScheme GetColorScheme();
    void SetAndApplyColorScheme(ColorScheme cs);

    void OnFrameTCTimer(wxTimerEvent& event);
    void OnFrameCOGTimer(wxTimerEvent& event);
    void SetupQuiltMode(void);

    void ChartsRefresh(int dbi_hint, ViewPort &vp, bool b_purge = true);

    bool CheckGroup(int igroup);

    void TouchAISActive(void);
    void UpdateAISTool(void);

    wxStatusBar         *m_pStatusBar;
    int                 nRoute_State;
    int                 nBlinkerTick;
    bool                m_bTimeIsSet;

    wxTimer             FrameTCTimer;
    wxTimer             FrameTimer1;
    wxTimer             FrameCOGTimer;
    wxTimer             MemFootTimer;

    //      PlugIn support
    int GetNextToolbarToolId(){return m_next_available_plugin_tool_id;}
    void RequestNewToolbarArgEvent( wxCommandEvent & event ){ return RequestNewToolbar(); }
    void RequestNewToolbar();

    void ActivateMOB(void);
    void UpdateGPSCompassStatusBox(bool b_force_new = false);
    bool UpdateChartDatabaseInplace(ArrayOfCDI &DirArray,
                                    bool b_force, bool b_prog,
                                    const wxString &ChartListFileName);

    bool                m_bdefer_resize;
    wxSize              m_defer_size;

  private:
#ifdef __WXOSX__
    void ocpnDoSetSize(void);
#else
    void DoSetSize(void);
#endif
    void DoCOGSet(void);

        //      Toolbar support
    ocpnToolBarSimple *CreateAToolbar();
    void DestroyMyToolbar();
    void UpdateToolbar(ColorScheme cs);

    void EnableToolbar(bool newstate);

    bool CheckAndAddPlugInTool(ocpnToolBarSimple *tb);
    bool AddDefaultPositionPlugInTools(ocpnToolBarSimple *tb);
    void FilterCogSog(void);
    void SetChartUpdatePeriod(ViewPort &vp);

    void ApplyGlobalColorSchemetoStatusBar(void);
    void PostProcessNNEA(bool pos_valid, const wxString &sfixtime);

    bool ScrubGroupArray();
    wxString GetGroupName(int igroup);
    void LoadHarmonics();

    bool EvalPriority(const wxString & message, DataStream *pDS );

    int                 m_StatusBarFieldCount;

    ChartCanvas         *m_pchart_canvas;

    NMEA0183        m_NMEA0183;                 // Used to parse messages from NMEA threads

    wxDateTime       m_MMEAeventTime;
    unsigned long    m_ulLastNEMATicktime;

    wxMutex          m_mutexNMEAEvent;         // Mutex to handle static data from NMEA threads

    wxString         m_last_reported_chart_name;
    wxString         m_last_reported_chart_pubdate;

    double           COGTable[MAX_COG_AVERAGE_SECONDS];

    wxString         m_lastAISiconName;

    bool             m_toolbar_scale_tools_shown;

    //      Plugin Support
    int                 m_next_available_plugin_tool_id;

    double              m_COGFilterLast;
    double              COGFilterTable[MAX_COGSOG_FILTER_SECONDS];
    double              SOGFilterTable[MAX_COGSOG_FILTER_SECONDS];

    bool                m_bpersistent_quilt;
    int                 m_ChartUpdatePeriod;
    bool                m_last_bGPSValid;

    wxString            prev_locale;
    bool                bPrevQuilt;
    bool                bPrevFullScreenQuilt;
    bool                bPrevOGL;

    MsgPriorityHash     NMEA_Msg_Hash;
    wxString            m_VDO_accumulator;

    time_t              m_fixtime;

    wxDECLARE_EVENT_TABLE();
};

//--------------------------------------------------------------------
//          Printing Support
//--------------------------------------------------------------------

class MyPrintout: public wxPrintout
{
 public:
  MyPrintout(const wxChar *title = _T("My printout")):wxPrintout(title){}
  virtual
  bool OnPrintPage(int page);
  virtual
  bool HasPage(int page);
  virtual
  bool OnBeginDocument(int startPage, int endPage);
  virtual
  void GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo);

  void DrawPageOne(wxDC *dc);




};


//      A global definition for window, timer and other ID's as needed.
#ifdef __WXOSX__
enum {
    ID_NMEA_WINDOW      = wxID_HIGHEST,
    ID_AIS_WINDOW,
    FRAME_TIMER_1,
    FRAME_TIMER_2,
    TIMER_AIS1,
    TIMER_AISAUDIO,
    AIS_SOCKET_ID,
    FRAME_TIMER_DOG,
    FRAME_TC_TIMER,
    FRAME_COG_TIMER,
    MEMORY_FOOTPRINT_TIMER,
    ID_NMEA_THREADMSG,
    TIMER_WIFI1,
    WIFI_SOCKET_ID
};

#else
enum {
    ID_NMEA_WINDOW      = wxID_HIGHEST,
    ID_AIS_WINDOW,
    FRAME_TIMER_1,
    FRAME_TIMER_2,
    TIMER_AIS1,
    TIMER_AISAUDIO,
    AIS_SOCKET_ID,
    FRAME_TIMER_DOG,
    FRAME_TC_TIMER,
    FRAME_COG_TIMER,
    MEMORY_FOOTPRINT_TIMER,
    ID_NMEA_THREADMSG

};
#endif
//-----------------------------------------------------------------------
//          Dummy Text Control for global key events
//-----------------------------------------------------------------------
class DummyTextCtrl: public wxTextCtrl
{
public:
      DummyTextCtrl(wxWindow *parent, wxWindowID id);
      void OnChar(wxKeyEvent &event);
      void OnMouseEvent(wxMouseEvent& event);

      wxTimer     m_MouseWheelTimer;
      int         m_mouse_wheel_oneshot;
      int         m_last_wheel_dir;

      wxDECLARE_EVENT_TABLE();
};


extern int OCPNMessageBox(wxWindow *parent,
                          const wxString& message,
                          const wxString& caption = _T("Message"),
                          int style = wxOK,  int timout_sec = -1, int x = -1, int y = -1);


//----------------------------------------------------------------------------
// Generic Auto Timed Window
// Belongs to the creator, not deleted automatically on applicaiton close
//----------------------------------------------------------------------------

class TimedPopupWin: public wxWindow
{
public:
    TimedPopupWin( wxWindow *parent, int timeout = -1 );
    ~TimedPopupWin();

    void OnPaint( wxPaintEvent& event );

    void SetBitmap( wxBitmap &bmp );
    wxBitmap* GetBitmap() { return m_pbm; }
    void OnTimer( wxTimerEvent& event );
    bool IsActive() { return isActive; }
    void IsActive( bool state ) { isActive = state; }

private:
    wxBitmap *m_pbm;
    wxTimer m_timer_timeout;
    int m_timeout_sec;
    bool isActive;

    wxDECLARE_EVENT_TABLE();
};

#endif
