/* **************************************************************************
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
 **************************************************************************/

#include <memory>
#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers
#ifdef __WXMSW__
//#include "c:\\Program Files\\visual leak detector\\include\\vld.h"
#endif


#include <wx/settings.h>
#include <wx/print.h>
#include <wx/printdlg.h>
#include <wx/artprov.h>
#include <wx/stdpaths.h>
#include <wx/intl.h>
#include <wx/listctrl.h>
#include <wx/aui/aui.h>
#include <wx/dialog.h>
#include <wx/progdlg.h>
#include <wx/clrpicker.h>
#include <wx/tokenzr.h>
#include <wx/dir.h>
#include <wx/dialog.h>

#include "dychart.h"

#include <limits.h>
#include <algorithm>

#ifdef __WXMSW__
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <psapi.h>
#endif

#ifndef __WXMSW__
#include <signal.h>
#include <setjmp.h>
#endif

#ifdef OCPN_HAVE_X11
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#endif

#include "config.h"
#include "chart1.h"
#include "chcanv.h"
#include "chartdb.h"
#include "logger.h"
#include "navutil.h"
#include "styles.h"
#include "routeman.h"
#include "piano.h"
#include "concanv.h"
#include "options.h"
#include "AboutFrameImpl.h"
#include "thumbwin.h"
#include "tcmgr.h"
#include "ais.h"
#include "chartimg.h"               // for ChartBaseBSB
#include "MarkInfo.h"
#include "RoutePropDlgImpl.h"
#include "toolbar.h"
#include "compass.h"
#include "datastream.h"
#include "OCPN_DataStreamEvent.h"
#include "multiplexer.h"
#include "routeprintout.h"
#include "Select.h"
#include "FontMgr.h"
#include "NMEALogWindow.h"
#include "Layer.h"
#include "NavObjectCollection.h"
#include "AISTargetListDialog.h"
#include "AISTargetAlertDialog.h"
#include "AIS_Decoder.h"
#include "OCP_DataStreamInput_Thread.h"
#include "TrackPropDlg.h"
#include "gshhs.h"
#include "cutil.h"
#include "routemanagerdialog.h"
#include "pluginmanager.h"
#include "AIS_Target_Data.h"
#include "OCPNPlatform.h"
#include "AISTargetQueryDialog.h"
#include "S57QueryDialog.h"
#include "glTexCache.h"
#include "Track.h"
#include "iENCToolbar.h"
#include "Quilt.h"
#include "Route.h"
#include "OCPN_AUIManager.h"
#include "CanvasConfig.h"
#include "ConfigMgr.h"
#include "MUIBar.h"
#include "OCPN_Sound.h"
#include "SoundFactory.h"

#ifdef ocpnUSE_GL
#include "glChartCanvas.h"
#endif

#include <wx/image.h>
#include "wx/apptrait.h"

#ifdef __WXOSX__
#include "macutils.h"
#include "osx_helpers.h"  // für Sparkle
#include <CoreFoundation/CFBundle.h>  // für Help
#include <Carbon/Carbon.h>  // für ???
#include <wx/menu.h>  // für ???
#include <wx/power.h>  // für ???
#endif

#ifdef USE_S57
#include "cm93.h"
#include "s52plib.h"
#include "s57chart.h"
#include "mygdal/cpl_csv.h"
#include "s52utils.h"
#endif

#ifdef __WXMSW__
//#define __MSVC__LEAK
#ifdef __MSVC__LEAK
#include "Stackwalker.h"
#endif
#endif

#include <wx/jsonreader.h>

#ifdef __OCPN__ANDROID__
#include "androidUTIL.h"
#endif

#ifdef LINUX_CRASHRPT
#include "crashprint.h"
#endif

#ifdef __WXOSX__
#include "DarkMode.h"
#endif

#ifdef ocpnUSE_NEWSERIAL
#include "serial/serial.h"
#endif

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( ArrayOfCDI );

#ifdef __WXMSW__
void RedirectIOToConsole();
#endif

//------------------------------------------------------------------------------
//      Fwd Declarations
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//      Static variable definition
//------------------------------------------------------------------------------

OCPNPlatform              *g_Platform;

bool                      g_bFirstRun;
bool                      g_bUpgradeInProcess;

bool                      g_bPauseTest;
int                       g_unit_test_1;
int                       g_unit_test_2;
bool                      g_start_fullscreen;
bool                      g_rebuild_gl_cache;
bool                      g_parse_all_enc;

MyFrame                   *gFrame;

ConsoleCanvas             *console;

MyConfig                  *pConfig;

ChartBase                 *Current_Vector_Ch;
ChartDB                   *ChartData;
wxString                  *pdir_list[20];
int                       g_restore_stackindex;
int                       g_restore_dbindex;
double                    g_ChartNotRenderScaleFactor;
int                       g_nDepthUnitDisplay;

RouteList                 *pRouteList;
TrackList                 *pTrackList;
LayerList                 *pLayerList;
bool                      g_bIsNewLayer;
int                       g_LayerIdx;
bool                      g_bLayerViz;

Select                    *pSelect;
Select                    *pSelectTC;
Select                    *pSelectAIS;

Routeman                  *g_pRouteMan;
WayPointman               *pWayPointMan;
MarkInfoDlg               *g_pMarkInfoDialog;
RoutePropDlgImpl          *pRoutePropDialog;
TrackPropDlg              *pTrackPropDialog;
RouteManagerDialog        *pRouteManagerDialog;
GoToPositionDialog        *pGoToPositionDialog;

double                    gLat, gLon, gCog, gSog, gHdt, gHdm, gVar;
wxString                  gRmcDate, gRmcTime;
double                    vLat, vLon;
double                    initial_scale_ppm, initial_rotation;

int                       g_nbrightness = 100;

bool                      bDBUpdateInProgress;

ThumbWin                  *pthumbwin;
TCMgr                     *ptcmgr;

bool                      g_bshowToolbar = true;
bool                      g_bBasicMenus = false;;

bool                      bDrawCurrentValues;

wxString                  ChartListFileName;
wxString                  AISTargetNameFileName;
wxString                  gWorldMapLocation, gDefaultWorldMapLocation;
wxString                  *pInit_Chart_Dir;
wxString                  g_csv_locn;
wxString                  g_SENCPrefix;
wxString                  g_UserPresLibData;
wxString                  g_VisibleLayers;
wxString                  g_InvisibleLayers;
wxString                  g_VisiNameinLayers;
wxString                  g_InVisiNameinLayers;

bool                      g_bcompression_wait;
bool                      g_FlushNavobjChanges;
int                       g_FlushNavobjChangesTimeout;

wxString                  g_uploadConnection;

int                       user_user_id;
int                       file_user_id;

int                       quitflag;
int                       g_tick = 0;
int                       g_mem_total, g_mem_used, g_mem_initial;

bool                      s_bSetSystemTime;

wxString                  *phost_name;

static unsigned int       malloc_max;

wxArrayOfConnPrm          *g_pConnectionParams;

wxDateTime                g_start_time;
wxDateTime                g_loglast_time;
static OcpnSound* _bells_sounds[]  = {SoundFactory(), SoundFactory()};
std::vector<OcpnSound*>   bells_sound(_bells_sounds, _bells_sounds + 2  );

OcpnSound*                g_anchorwatch_sound = SoundFactory();
wxString                  g_anchorwatch_sound_file;
wxString                  g_DSC_sound_file;
wxString                  g_SART_sound_file;
wxString                  g_AIS_sound_file;

RoutePoint                *pAnchorWatchPoint1;
RoutePoint                *pAnchorWatchPoint2;
double                    AnchorPointMinDist;
bool                      AnchorAlertOn1, AnchorAlertOn2;
bool                      g_bCruising;

ChartDummy                *pDummyChart;

ocpnStyle::StyleManager*  g_StyleManager;


// Global print data, to remember settings during the session
wxPrintData               *g_printData = (wxPrintData*) NULL ;

// Global page setup data
wxPageSetupData*          g_pageSetupData = (wxPageSetupData*) NULL;

bool                      g_bShowOutlines;
bool                      g_bShowDepthUnits;
bool                      g_bDisplayGrid;  // Flag indicating weather the lat/lon grid should be displayed
bool                      g_bShowChartBar;
bool                      g_bShowActiveRouteHighway;
int                       g_nNMEADebug;
int                       g_nAWDefault;
int                       g_nAWMax;
bool                      g_bPlayShipsBells;
bool                      g_bFullscreenToolbar;
bool                      g_bShowLayers;
bool                      g_bTransparentToolbar;
bool                      g_bTransparentToolbarInOpenGLOK;
int                       g_nAutoHideToolbar;
bool                      g_bAutoHideToolbar;

bool                      g_bPermanentMOBIcon;
bool                      g_bTempShowMenuBar;

int                       g_iSDMMFormat;
int                       g_iDistanceFormat;
int                       g_iSpeedFormat;
int                       g_iTempFormat;

int                       g_iNavAidRadarRingsNumberVisible;
float                     g_fNavAidRadarRingsStep;
int                       g_pNavAidRadarRingsStepUnits;
int                       g_iWaypointRangeRingsNumber;
float                     g_fWaypointRangeRingsStep;
int                       g_iWaypointRangeRingsStepUnits;
wxColour                  g_colourWaypointRangeRingsColour;
bool                      g_bWayPointPreventDragging;
bool                      g_bConfirmObjectDelete;
wxColour                  g_colourOwnshipRangeRingsColour;
int                       g_iWpt_ScaMin;
bool                      g_bUseWptScaMin;
bool                      g_bShowWptName;
int                       g_maxzoomin;

// Set default color scheme
ColorScheme               global_color_scheme = GLOBAL_COLOR_SCHEME_DAY;

static int                Usercolortable_index;

static wxArrayPtrVoid     *UserColorTableArray;
static wxArrayPtrVoid     *UserColourHashTableArray;

static wxColorHashMap     *pcurrent_user_color_hash;

int                       gps_watchdog_timeout_ticks;
int                       sat_watchdog_timeout_ticks;

int                       gGPS_Watchdog;
bool                      bGPSValid;

int                       gHDx_Watchdog;
int                       gHDT_Watchdog;
int                       gVAR_Watchdog;
bool                      g_bHDT_Rx;
bool                      g_bVAR_Rx;

int                       gSAT_Watchdog;
int                       g_priSats;
int                       g_SatsInView;
bool                      g_bSatValid;

bool                      g_bDebugCM93;
bool                      g_bDebugS57;

bool                      g_bfilter_cogsog;
int                       g_COGFilterSec = 1;
int                       g_SOGFilterSec;

int                       g_ChartUpdatePeriod;
int                       g_SkewCompUpdatePeriod;

int                       g_lastClientRectx;
int                       g_lastClientRecty;
int                       g_lastClientRectw;
int                       g_lastClientRecth;
double                    g_display_size_mm;
double                    g_config_display_size_mm;
bool                      g_config_display_size_manual;
float                     g_selection_radius_mm = 2.0;
float                     g_selection_radius_touch_mm = 10.0;

int                       g_GUIScaleFactor;
int                       g_ChartScaleFactor;
float                     g_ChartScaleFactorExp;
int                       g_last_ChartScaleFactor;
int                       g_ShipScaleFactor;
float                     g_ShipScaleFactorExp;
int                       g_ENCSoundingScaleFactor;

bool                      g_bShowTide;
bool                      g_bShowCurrent;

#ifdef USE_S57
s52plib                   *ps52plib;
S57ClassRegistrar         *g_poRegistrar;
s57RegistrarMgr           *m_pRegistrarMan;

CM93OffsetDialog          *g_pCM93OffsetDialog;
#endif

#ifdef __WXOSX__
#include "macutils.h"
#endif

// begin rms
#if defined( USE_S57) || defined ( __WXOSX__ )
#ifdef __WXMSW__
#ifdef USE_GLU_TESS
#ifdef USE_GLU_DLL
// end rms
extern bool               s_glu_dll_ready;
extern HINSTANCE          s_hGLU_DLL; // Handle to DLL
#endif
#endif
#endif
#endif

double                    g_ownship_predictor_minutes;
double                    g_ownship_HDTpredictor_miles;

bool                      g_own_ship_sog_cog_calc;
int                       g_own_ship_sog_cog_calc_damp_sec;
wxDateTime                last_own_ship_sog_cog_calc_ts;
double                    last_own_ship_sog_cog_calc_lat, last_own_ship_sog_cog_calc_lon;

Multiplexer               *g_pMUX;

AIS_Decoder               *g_pAIS;
bool                      g_bAIS_CPA_Alert;
bool                      g_bAIS_CPA_Alert_Audio;
AISTargetAlertDialog      *g_pais_alert_dialog_active;
AISTargetQueryDialog      *g_pais_query_dialog_active;
int                       g_iSoundDeviceIndex;

int                       g_ais_alert_dialog_x, g_ais_alert_dialog_y;
int                       g_ais_alert_dialog_sx, g_ais_alert_dialog_sy;
int                       g_ais_query_dialog_x, g_ais_query_dialog_y;

int                       g_S57_dialog_sx, g_S57_dialog_sy;

int                       g_nframewin_x;
int                       g_nframewin_y;
int                       g_nframewin_posx;
int                       g_nframewin_posy;
bool                      g_bframemax;

bool                      g_bAutoAnchorMark;

wxRect                    g_blink_rect;
double                    g_PlanSpeed;
wxDateTime                g_StartTime;
int                       g_StartTimeTZ;
IDX_entry                 *gpIDX;
int                       gpIDXn;
long                      gStart_LMT_Offset;

wxArrayString             *pMessageOnceArray;

FILE                      *s_fpdebug;
bool                      bAutoOpen;

bool                      g_bUseGLL = true;

int                       g_nCacheLimit;
int                       g_memCacheLimit;
bool                      g_bGDAL_Debug;

double                    g_VPRotate; // Viewport rotation angle, used on "Course Up" mode
bool                      g_bCourseUp;
int                       g_COGAvgSec = 15; // COG average period (sec.) for Course Up Mode
double                    g_COGAvg;
bool                      g_bLookAhead;
bool                      g_bskew_comp;
bool                      g_bopengl;
bool                      g_bSoftwareGL;
bool                      g_bShowFPS;
bool                      g_bsmoothpanzoom;
bool                      g_fog_overzoom;
double                    g_overzoom_emphasis_base;
bool                      g_oz_vector_scale;
double                    g_plus_minus_zoom_factor;

int                       g_nCOMPortCheck = 32;

bool                      g_b_legacy_input_filter_behaviour;  // Support original input filter process or new process

bool                      g_bbigred;

PlugInManager             *g_pi_manager;

bool                      g_bAISRolloverShowClass;
bool                      g_bAISRolloverShowCOG;
bool                      g_bAISRolloverShowCPA;

bool                      g_bDebugGPSD;

bool                      g_bFullScreenQuilt = true;
bool                      g_bQuiltEnable;
bool                      g_bQuiltStart;

bool                      g_bportable;

bool                      g_bdisable_opengl;

ChartGroupArray           *g_pGroupArray;

wxString                  g_GPS_Ident;

S57QueryDialog            *g_pObjectQueryDialog;

wxArrayString             TideCurrentDataSet;
wxString                  g_TCData_Dir;


bool                      g_boptionsactive;
options                   *g_options;
bool                      g_bDeferredInitDone;
int                       options_lastPage = 0;
int                       options_subpage = 0;

wxPoint                   options_lastWindowPos( 0,0 );
wxSize                    options_lastWindowSize( 0,0 );

bool                      g_bSleep;
bool                      g_bsimplifiedScalebar;

int                       g_grad_default;
wxColour                  g_border_color_default;
int                       g_border_size_default;
int                       g_sash_size_default;
wxColour                  g_caption_color_default;
wxColour                  g_sash_color_default;
wxColour                  g_background_color_default;

int                       osMajor, osMinor;

bool GetMemoryStatus(int *mem_total, int *mem_used);
bool                      g_bHasHwClock;

#ifdef __WXMSW__
// System color control support

typedef DWORD (WINAPI *SetSysColors_t)(DWORD, DWORD *, DWORD *);
typedef DWORD (WINAPI *GetSysColor_t)(DWORD);

SetSysColors_t            pSetSysColors;
GetSysColor_t             pGetSysColor;

void SaveSystemColors(void);
void RestoreSystemColors(void);

DWORD                     color_3dface;
DWORD                     color_3dhilite;
DWORD                     color_3dshadow;
DWORD                     color_3ddkshadow;
DWORD                     color_3dlight;
DWORD                     color_activecaption;
DWORD                     color_gradientactivecaption;
DWORD                     color_captiontext;
DWORD                     color_windowframe;
DWORD                     color_inactiveborder;

#endif

// AIS Global configuration
bool                      g_bShowAIS;
bool                      g_bCPAMax;
double                    g_CPAMax_NM;
bool                      g_bCPAWarn;
double                    g_CPAWarn_NM;
bool                      g_bTCPA_Max;
double                    g_TCPA_Max;
bool                      g_bMarkLost;
double                    g_MarkLost_Mins;
bool                      g_bRemoveLost;
double                    g_RemoveLost_Mins;
bool                      g_bShowCOG;
double                    g_ShowCOG_Mins;
bool                      g_bAISShowTracks;
double                    g_AISShowTracks_Mins;
double                    g_AISShowTracks_Limit;
bool                      g_bHideMoored;
bool                      g_bAllowShowScaled;
double                    g_ShowMoored_Kts;
wxString                  g_sAIS_Alert_Sound_File;
bool                      g_bAIS_CPA_Alert_Suppress_Moored;
bool                      g_bAIS_ACK_Timeout;
double                    g_AckTimeout_Mins;
bool                      g_bShowScaled;
bool                      g_bShowAreaNotices;
bool                      g_bDrawAISSize;
bool                      g_bDrawAISRealtime;
double                    g_AIS_RealtPred_Kts;
bool                      g_bShowAISName;
int                       g_Show_Target_Name_Scale;
bool                      g_bWplUsePosition;
int                       g_WplAction;

int                       g_nAIS_activity_timer;

DummyTextCtrl             *g_pDummyTextCtrl;
bool                      g_bEnableZoomToCursor;

bool                      g_bTrackActive;
bool                      g_bTrackCarryOver;
bool                      g_bDeferredStartTrack;
bool                      g_bTrackDaily;
int                       g_track_rotate_time;
int                       g_track_rotate_time_type;
bool                      g_bHighliteTracks;
int                       g_route_line_width;
int                       g_track_line_width;
wxColour                  g_colourTrackLineColour;
wxString                  g_default_wp_icon;
wxString                  g_default_routepoint_icon;

ActiveTrack              *g_pActiveTrack;
double                    g_TrackIntervalSeconds;
double                    g_TrackDeltaDistance;
int                       g_nTrackPrecision;
#ifdef __WXOSX__
double                    sumlogsum;
#endif
int                       g_total_NMEAerror_messages;

int                       g_cm93_zoom_factor;
PopUpDSlide                *pPopupDetailSlider;
bool                      g_bShowDetailSlider;
int                       g_detailslider_dialog_x, g_detailslider_dialog_y;

bool                      g_bUseGreenShip;

wxString                  g_AW1GUID;
wxString                  g_AW2GUID;

bool                      g_b_overzoom_x = true; // Allow high overzoom

int                       g_OwnShipIconType;
double                    g_n_ownship_length_meters;
double                    g_n_ownship_beam_meters;
double                    g_n_gps_antenna_offset_y;
double                    g_n_gps_antenna_offset_x;
int                       g_n_ownship_min_mm;

double                    g_n_arrival_circle_radius;

bool                      g_bNeedDBUpdate;
bool                      g_bPreserveScaleOnX;

AboutFrameImpl            *g_pAboutDlg;

#if wxUSE_XLOCALE || !wxCHECK_VERSION(3,0,0)
wxLocale                  *plocale_def_lang;
#endif

wxString                  g_locale;
wxString                  g_localeOverride;
bool                      g_b_assume_azerty;

bool                      g_bUseRaster;
bool                      g_bUseVector;
bool                      g_bUseCM93;

int                       g_click_stop;

int                       g_MemFootSec;
int                       g_MemFootMB;

wxStaticBitmap            *g_pStatBoxTool;
bool                      g_bShowStatusBar;

bool                      g_bquiting;
int                       g_BSBImgDebug;

AISTargetListDialog       *g_pAISTargetList;
wxString                  g_AisTargetList_perspective;
int                       g_AisTargetList_range;
int                       g_AisTargetList_sortColumn;
bool                      g_bAisTargetList_sortReverse;
wxString                  g_AisTargetList_column_spec;
wxString                  g_AisTargetList_column_order;
int                       g_AisTargetList_count;
bool                      g_bAisTargetList_autosort;

bool                      g_bGarminHostUpload;
bool                      g_bFullscreen;

OCPN_AUIManager           *g_pauimgr;
wxAuiDefaultDockArt       *g_pauidockart;

wxString                  g_toolbarConfig = _T("XXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
wxString                  g_toolbarConfigSecondary = _T("....XX..X........XXXXXXXXXXXX");

ocpnFloatingToolbarDialog *g_MainToolbar;
int                       g_maintoolbar_x;
int                       g_maintoolbar_y;
long                      g_maintoolbar_orient;
float                     g_toolbar_scalefactor;

float                     g_compass_scalefactor;
bool                      g_bShowMenuBar;
bool                      g_bShowCompassWin;

bool                      g_benable_rotate;

bool                      g_bShowTrue = true;
bool                      g_bShowMag;

double                    g_UserVar;
bool                      g_bMagneticAPB;

bool                      g_bInlandEcdis;

//                        OpenGL Globals
int                       g_GPU_MemSize;

bool                      g_bserial_access_checked;
wxString                  g_uiStyle;

//      Values returned from WMM_PI for variation computation request
//      Initialize to invalid value so we don't use if if WMM hasn't updated yet
double                    gQueryVar = 361.0;


char bells_sound_file_name[2][12] = { "1bells.wav", "2bells.wav" };

int                       portaudio_initialized;

bool                      g_bAIS_GCPA_Alert_Audio;
bool                      g_bAIS_SART_Alert_Audio;
bool                      g_bAIS_DSC_Alert_Audio;
bool                      g_bAnchor_Alert_Audio;

static char nmea_tick_chars[] = { '|', '/', '-', '\\', '|', '/', '-', '\\' };
static int tick_idx;

int               g_sticky_chart;
int               g_sticky_projection;

bool              g_benableUDPNullHeader;

extern options          *g_pOptions;

int n_NavMessageShown;
wxString g_config_version_string;

wxString g_CmdSoundString;

bool             g_btouch;
bool             g_bresponsive;

bool             b_inCompressAllCharts;
bool             g_bGLexpert;
bool             g_bUIexpert;

int              g_chart_zoom_modifier;
int              g_chart_zoom_modifier_vector;

int              g_NMEAAPBPrecision;

wxString         g_TalkerIdText;
int              g_maxWPNameLength;

bool             g_bAdvanceRouteWaypointOnArrivalOnly;

bool             g_bSpaceDropMark;

wxArrayString    g_locale_catalog_array;
bool             b_reloadForPlugins;

unsigned int     g_canvasConfig;
bool             g_useMUI;
bool             g_bmasterToolbarFull = true;

int              g_memUsed;
SENCThreadManager *g_SencThreadManager;

WX_DEFINE_ARRAY_PTR(ChartCanvas*, arrayofCanvasPtr);

arrayofCanvasPtr   g_canvasArray;
arrayofCanvasConfigPtr g_canvasConfigArray;
wxString         g_lastAppliedTemplateGUID;

ChartCanvas      *g_focusCanvas;
ChartCanvas      *g_overlayCanvas;

#ifdef LINUX_CRASHRPT
wxCrashPrint g_crashprint;
#endif

#ifndef __WXMSW__
sigjmp_buf env;                    // the context saved by sigsetjmp();
#endif

// {2C9C45C2-8E7D-4C08-A12D-816BBAE722C0}
#ifdef  __WXMSW__
DEFINE_GUID( GARMIN_DETECT_GUID, 0x2c9c45c2L, 0x8e7d, 0x4c08, 0xa1, 0x2d, 0x81, 0x6b, 0xba, 0xe7,
        0x22, 0xc0 );
#endif

#ifdef __MSVC__
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__ )
#define new DEBUG_NEW
#endif

#if !defined(NAN)
static const long long lNaN = 0xfff8000000000000;
#define NAN (*(double*)&lNaN)
#endif

//    Some static helpers
void appendOSDirSlash( wxString* pString );

static void InitializeUserColors( void );
static void DeInitializeUserColors( void );
static void SetSystemColors( ColorScheme cs );

extern "C" bool CheckSerialAccess( void );
#if 0
// Refresh the Piano Bar
static void refresh_Piano()
{
//     int idx = pCurrentStack->GetCurrentEntrydbIndex();
//     if (idx < 0)
//         return;
// 
//     std::vector<int> piano_active_chart_index_array;
//     piano_active_chart_index_array.push_back( pCurrentStack->GetCurrentEntrydbIndex() );
//     g_Piano->SetActiveKeyArray( piano_active_chart_index_array );
}
#endif

//------------------------------------------------------------------------------
//    PNG Icon resources
//------------------------------------------------------------------------------

#if defined(__WXGTK__) || defined(__WXQT__)
#include "bitmaps/opencpn.xpm"
#endif

//------------------------------------------------------------------------------
//              Local constants
//------------------------------------------------------------------------------
// enum {
//     ID_PIANO_DISABLE_QUILT_CHART = 32000, ID_PIANO_ENABLE_QUILT_CHART
// };

//------------------------------------------------------------------------------
//              Fwd Refs
//------------------------------------------------------------------------------

iENCToolbar *g_iENCToolbar;
int g_iENCToolbarPosX;
int g_iENCToolbarPosY;

void BuildiENCToolbar( bool bnew )
{
    if(g_bInlandEcdis){
        if(bnew){
            if(g_iENCToolbar){
                wxPoint locn = g_iENCToolbar->GetPosition();
                wxPoint tbp_incanvas = gFrame->GetPrimaryCanvas()->ScreenToClient( locn );

                g_iENCToolbarPosY = tbp_incanvas.y;
                g_iENCToolbarPosX = tbp_incanvas.x;

                delete g_iENCToolbar;
                g_iENCToolbar = 0;
            }
        }

        if( !g_iENCToolbar ) {

            wxPoint posn(g_iENCToolbarPosX, g_iENCToolbarPosY);

            // Overlapping main toolbar?
            if(g_MainToolbar){
                if((g_iENCToolbarPosY > g_maintoolbar_y) && (g_iENCToolbarPosY < g_maintoolbar_y + g_MainToolbar->GetSize().y) )
                    g_iENCToolbarPosY = -1;         // force a reposition
            }

            if((g_iENCToolbarPosX < 0) || (g_iENCToolbarPosY < 0)){
                posn.x = 0;
                posn.y = 100;

                if(g_MainToolbar)
                    posn = wxPoint(g_maintoolbar_x + g_MainToolbar->GetSize().x + 4, g_maintoolbar_y );
            }

            double tool_scale_factor = g_Platform->GetToolbarScaleFactor( g_GUIScaleFactor );

            g_iENCToolbar = new iENCToolbar( gFrame,  posn, wxTB_HORIZONTAL, tool_scale_factor );
            g_iENCToolbar->SetColorScheme(global_color_scheme);
        }
    }
    else{
        delete g_iENCToolbar;
        g_iENCToolbar = NULL;
    }

}



int ShowNavWarning()
{
    wxString msg0(
            _("\n\
OpenCPN is distributed in the hope that it will be useful, \
but WITHOUT ANY WARRANTY; without even the implied \
warranty of MERCHANTABILITY or FITNESS FOR A \
PARTICULAR PURPOSE.\n\n\
See the GNU General Public License for more details.\n\n\
OpenCPN must only be used in conjunction with approved \
paper charts and traditional methods of navigation.\n\n\
DO NOT rely upon OpenCPN for safety of life or property.\n\n\
Please click \"OK\" to agree and proceed, \"Cancel\" to quit.\n") );

    wxString vs =
        wxString::Format(wxT(" .. Version %s"),
            VERSION_FULL);

//    wxMessageDialog odlg( gFrame, msg0, _("Welcome to OpenCPN") + vs, wxCANCEL | wxOK );

//    return ( odlg.ShowModal() );
        wxColor fg = wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOWTEXT );
        wxString msg1;
        msg1.Printf(_T("<html><body><font color=#%02x%02x%02x><hr />"), fg.Red(), fg.Green(), fg.Blue());

        for(unsigned int i=0 ; i < msg0.Length() ; i++){
            if(msg0[i] == '\n')
                msg1 +=  _T("<br>");
            else
                msg1 += msg0[i];
        }

        msg1 <<  _T("<hr /></font></body></html>");

        OCPN_TimedHTMLMessageDialog infoDlg( gFrame, msg1, _("Welcome to OpenCPN") + vs, -1, wxCANCEL | wxOK);

        infoDlg.ShowModal();

        return (infoDlg.GetReturnCode() );

}


wxString newPrivateFileName(wxString home_locn, const char *name, const char *windowsName)
{
    wxString fname = wxString::FromUTF8(name);
    wxString fwname = wxString::FromUTF8(windowsName);
    wxString filePathAndName;

    filePathAndName = g_Platform->GetPrivateDataDir();
    if (filePathAndName.Last() != wxFileName::GetPathSeparator())
       filePathAndName.Append(wxFileName::GetPathSeparator());

#ifdef __WXMSW__
     filePathAndName.Append( fwname );
#else
     filePathAndName.Append( fname );
#endif

     return filePathAndName;
}

bool isSingleChart(ChartBase *chart)
{
   if (chart == nullptr)
       return false;

   // ..For each canvas...
   for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
      ChartCanvas *cc = g_canvasArray.Item(i);
      if(cc && cc->m_singleChart == chart){
         return true;
      }
   }
   return false;
}


// `Main program' equivalent, creating windows and returning main app frame
//------------------------------------------------------------------------------
// MyApp
//------------------------------------------------------------------------------
//#ifndef OCPN_USE_WRAPPER
IMPLEMENT_APP( MyApp )
//#endif

BEGIN_EVENT_TABLE(MyApp, wxApp)
EVT_ACTIVATE_APP(MyApp::OnActivateApp)
END_EVENT_TABLE()

#include "wx/dynlib.h"

#if wxUSE_CMDLINE_PARSER
void MyApp::OnInitCmdLine( wxCmdLineParser& parser )
{
    //    Add some OpenCPN specific command line options
    parser.AddSwitch( _T("h"), _T("help"), _("Show usage syntax."), wxCMD_LINE_OPTION_HELP );
    parser.AddSwitch( _T("p"), wxEmptyString, _("Run in portable mode.") );
    parser.AddSwitch( _T("fullscreen"), wxEmptyString, _("Switch to full screen mode on start.") );
    parser.AddSwitch( _T("no_opengl"), wxEmptyString, _("Disable OpenGL video acceleration. This setting will be remembered.") );
    parser.AddSwitch( _T("rebuild_gl_raster_cache"), wxEmptyString, _T("Rebuild OpenGL raster cache on start.") );
    parser.AddSwitch( _T("parse_all_enc"), wxEmptyString, _T("Convert all S-57 charts to OpenCPN's internal format on start.") );
    parser.AddOption( _T("l"), _T("loglevel"), _("Amount of logging: error, warning, message, info, debug or trace"));
    parser.AddOption( _T("unit_test_1"), wxEmptyString, _("Display a slideshow of <num> charts and then exit. Zero or negative <num> specifies no limit."), wxCMD_LINE_VAL_NUMBER );

    parser.AddSwitch( _T("unit_test_2") );
}

/* Parse --loglevel and set up logging, falling back to defaults. */
static void ParseLoglevel(wxCmdLineParser& parser)
{
    const char* strLevel = std::getenv("OPENCPN_LOGLEVEL");
    strLevel = strLevel ? strLevel : "info";
    wxString wxLevel;
    if (parser.Found("l", &wxLevel)) {
        strLevel = wxLevel.c_str();
    }
    wxLogLevel level = OcpnLog::str2level(strLevel);
    if (level == OcpnLog::LOG_BADLEVEL) {
        fprintf(stderr, "Bad loglevel %s, using \"info\"", strLevel);
        strLevel = "info";
        level = wxLOG_Info;
    }
    wxLog::SetLogLevel(level);
}

bool MyApp::OnCmdLineParsed( wxCmdLineParser& parser )
{
    long number;

    g_unit_test_2 = parser.Found( _T("unit_test_2") );
    g_bportable = parser.Found( _T("p") );
    g_start_fullscreen = parser.Found( _T("fullscreen") );
    g_bdisable_opengl = parser.Found( _T("no_opengl") );
    g_rebuild_gl_cache = parser.Found( _T("rebuild_gl_raster_cache") );
    g_parse_all_enc = parser.Found( _T("parse_all_enc") );
    if( parser.Found( _T("unit_test_1"), &number ) )
    {
        g_unit_test_1 = static_cast<int>( number );
        if( g_unit_test_1 == 0 )
            g_unit_test_1 = -1;
    }
    ParseLoglevel(parser);

    return true;
}
#endif

#ifdef __WXMSW__
    //  Handle any exception not handled by CrashRpt
    //  Most probable:  Malloc/new failure

bool MyApp::OnExceptionInMainLoop()
{
    wxLogWarning(_T("Caught MainLoopException, continuing..."));
    return true;
}
#endif

void MyApp::OnActivateApp( wxActivateEvent& event )
{
    return;
//    Code carefully in this method.
//    It is called in some unexpected places,
//    such as on closure of dialogs, etc.

    if( !event.GetActive() ) {

        //  Remove a temporary Menubar when the application goes inactive
        //  This is one way to handle properly ALT-TAB navigation on the Windows desktop
        //  without accidentally leaving an unwanted Menubar shown.
#ifdef __WXMSW__
        if( g_bTempShowMenuBar ) {
            g_bTempShowMenuBar = false;
            if(gFrame)
                gFrame->ApplyGlobalSettings(false);
        }
#endif


    }    
    event.Skip();
}

void LoadS57()
{
#ifndef USE_S57
    return;
#else
    if(ps52plib) // already loaded?
        return;

    //  Start a SENC Thread manager
    g_SencThreadManager = new SENCThreadManager();

//      Set up a useable CPL library error handler for S57 stuff
    CPLSetErrorHandler( MyCPLErrorHandler );

//      Init the s57 chart object, specifying the location of the required csv files
    g_csv_locn = g_Platform->GetSharedDataDir();
    g_csv_locn.Append( _T("s57data") );

    if( g_bportable ) {
        g_csv_locn = _T(".");
        appendOSDirSlash( &g_csv_locn );
        g_csv_locn.Append( _T("s57data") );
    }

//      If the config file contains an entry for SENC file prefix, use it.
//      Otherwise, default to PrivateDataDir
    if( g_SENCPrefix.IsEmpty() ) {
        g_SENCPrefix = g_Platform->GetPrivateDataDir();
        appendOSDirSlash(&g_SENCPrefix);
        g_SENCPrefix.Append( _T("SENC") );
    }

    if( g_bportable ) {
        wxFileName f( g_SENCPrefix );
        if( f.MakeRelativeTo( g_Platform->GetPrivateDataDir() ) )
            g_SENCPrefix = f.GetFullPath();
        else
            g_SENCPrefix = _T("SENC");
    }

//      If the config file contains an entry for PresentationLibraryData, use it.
//      Otherwise, default to conditionally set spot under g_pcsv_locn
    wxString plib_data;
    bool b_force_legacy = false;

    if( g_UserPresLibData.IsEmpty() ) {
        plib_data = g_csv_locn;
        appendOSDirSlash( &plib_data );
        plib_data.Append( _T("S52RAZDS.RLE") );
    } else {
        plib_data = g_UserPresLibData;
        b_force_legacy = true;
    }

    ps52plib = new s52plib( plib_data, b_force_legacy );

    //  If the library load failed, try looking for the s57 data elsewhere

    //  First, look in UserDataDir
    /*    From wxWidgets documentation

     wxStandardPaths::GetUserDataDir
     wxString GetUserDataDir() const
     Return the directory for the user-dependent application data files:
     * Unix: ~/.appname
     * Windows: C:\Documents and Settings\username\Application Data\appname
     * Mac: ~/Library/Application Support/appname
     */

    if( !ps52plib->m_bOK ) {
        delete ps52plib;

        wxStandardPaths& std_path = g_Platform->GetStdPaths();

        wxString look_data_dir;
        look_data_dir.Append( std_path.GetUserDataDir() );
        appendOSDirSlash( &look_data_dir );
        wxString tentative_SData_Locn = look_data_dir;
        look_data_dir.Append( _T("s57data") );

        plib_data = look_data_dir;
        appendOSDirSlash( &plib_data );
        plib_data.Append( _T("S52RAZDS.RLE") );

        wxLogMessage( _T("Looking for s57data in ") + look_data_dir );
        ps52plib = new s52plib( plib_data );

        if( ps52plib->m_bOK ) {
            g_csv_locn = look_data_dir;
///???            g_SData_Locn = tentative_SData_Locn;
        }
    }

    //  And if that doesn't work, look again in the original SData Location
    //  This will cover the case in which the .ini file entry is corrupted or moved

    if( !ps52plib->m_bOK ) {
        delete ps52plib;

        wxString look_data_dir;
        look_data_dir = g_Platform->GetSharedDataDir();
        look_data_dir.Append( _T("s57data") );

        plib_data = look_data_dir;
        appendOSDirSlash( &plib_data );
        plib_data.Append( _T("S52RAZDS.RLE") );

        wxLogMessage( _T("Looking for s57data in ") + look_data_dir );
        ps52plib = new s52plib( plib_data );

        if( ps52plib->m_bOK ) g_csv_locn = look_data_dir;
    }

    if( ps52plib->m_bOK ) {
        wxLogMessage( _T("Using s57data in ") + g_csv_locn );
        m_pRegistrarMan = new s57RegistrarMgr( g_csv_locn, g_Platform->GetLogFilePtr() );


            //    Preset some object class visibilites for "User Standard" disply category
            //  They may be overridden in LoadS57Config
        for( unsigned int iPtr = 0; iPtr < ps52plib->pOBJLArray->GetCount(); iPtr++ ) {
            OBJLElement *pOLE = (OBJLElement *) ( ps52plib->pOBJLArray->Item( iPtr ) );
            if( !strncmp( pOLE->OBJLName, "DEPARE", 6 ) ) pOLE->nViz = 1;
            if( !strncmp( pOLE->OBJLName, "LNDARE", 6 ) ) pOLE->nViz = 1;
            if( !strncmp( pOLE->OBJLName, "COALNE", 6 ) ) pOLE->nViz = 1;
        }

        pConfig->LoadS57Config();
        ps52plib->SetPLIBColorScheme( global_color_scheme );

        if(gFrame->GetPrimaryCanvas() )
            ps52plib->SetPPMM( gFrame->GetPrimaryCanvas()->GetPixPerMM() );
 
#ifdef ocpnUSE_GL

        // Setup PLIB OpenGL options, if enabled
        extern bool g_b_EnableVBO;    
        extern GLenum  g_texture_rectangle_format;
        if(g_bopengl )   
            ps52plib->SetGLOptions(glChartCanvas::s_b_useStencil,
                                   glChartCanvas::s_b_useStencilAP,
                                   glChartCanvas::s_b_useScissorTest,
                                   glChartCanvas::s_b_useFBO,
                                   g_b_EnableVBO,
                                   g_texture_rectangle_format);
#endif


    } else {
        wxLogMessage( _T("   S52PLIB Initialization failed, disabling Vector charts.") );
        delete ps52plib;
        ps52plib = NULL;
    }
#endif
}

#if defined(__WXGTK__) && defined(OCPN_HAVE_X11)

// Note: use XFree to free this pointer. Use unique_ptr in the future.
static char *get_X11_property (Display *disp, Window win,
                            Atom xa_prop_type, const char *prop_name) {
    Atom xa_prop_name;
    Atom xa_ret_type;
    int ret_format;
    unsigned long ret_nitems;
    unsigned long ret_bytes_after;
    unsigned char *ret_prop;

    xa_prop_name = XInternAtom(disp, prop_name, False);

    // For XGetWindowProperty source see
    // https://github.com/mirror/libX11/blob/master/src/GetProp.c#L107
    // it is quite tricky. Some notes.
    // + Results are already NULL terminated.
    // + 32 as a ret_format means sizeof(long) in the API...
    // + but as xlib does the null termination we can just ignore the sizes.
    if (XGetWindowProperty(disp, win, xa_prop_name, 0, 1024, False,
                           xa_prop_type, &xa_ret_type, &ret_format,
                           &ret_nitems, &ret_bytes_after, &ret_prop) != Success)
        return NULL;

    if (xa_ret_type != xa_prop_type) {
       XFree(ret_prop);
       return NULL;
    }
    return (char*)ret_prop;
}
#endif

// Determine if a transparent toolbar is possible under linux with opengl
static bool isTransparentToolbarInOpenGLOK(void) {
#ifdef __WXOSX__
    return true;
#else
    bool status = false;
#ifndef __WXQT__
#ifdef OCPN_HAVE_X11
    if(!g_bdisable_opengl) {
        Display *disp = XOpenDisplay(NULL);
        Window *sup_window;
        if ((sup_window = (Window *)get_X11_property(disp, DefaultRootWindow(disp),
                                                 XA_WINDOW, "_NET_SUPPORTING_WM_CHECK")) ||
            (sup_window = (Window *)get_X11_property(disp, DefaultRootWindow(disp),
                                                 XA_CARDINAL, "_WIN_SUPPORTING_WM_CHECK"))) {
            /* WM_NAME */
            char *wm_name;
            if ((wm_name = get_X11_property(disp, *sup_window,
                                        XInternAtom(disp, "UTF8_STRING", False), "_NET_WM_NAME")) ||
                (wm_name = get_X11_property(disp, *sup_window,
                                        XA_STRING, "_NET_WM_NAME"))) {
                // we know it works in xfce4, add other checks as we can validate them
                if(strstr(wm_name, "Xfwm4") || strstr(wm_name, "Compiz"))
                    status = true;

                XFree(wm_name);
            }
            XFree(sup_window);
        }
        XCloseDisplay(disp);
    }
#endif
#endif
    return status;
#endif
}

static wxStopWatch init_sw;
class ParseENCWorkerThread : public wxThread
{
public:
    ParseENCWorkerThread(wxString filename, Extent &ext, int scale)
        : wxThread(wxTHREAD_JOINABLE)
        {
            m_filename = filename;
            m_ext = ext;
            m_scale = scale;
            Create();
        }

    void *Entry() {
//         ChartBase *pchart = ChartData->OpenChartFromDB(m_filename, FULL_INIT);
//         ChartData->DeleteCacheChart(pchart);
        s57chart *newChart = new s57chart;

        newChart->SetNativeScale(m_scale);
        newChart->SetFullExtent(m_ext);

        newChart->FindOrCreateSenc(m_filename);
        delete newChart;
        return 0;
    }

    wxString m_filename;
    Extent m_ext;
    int m_scale;
};

// begin duplicated code
static double chart_dist(int index)
{
    double d;
    float  clon;
    float  clat;
    const ChartTableEntry &cte = ChartData->GetChartTableEntry(index);
    // if the chart contains ownship position set the distance to 0
    if (cte.GetBBox().Contains(gLat, gLon))
        d = 0.;
    else {
        // find the nearest edge 
        double t;
        clon = (cte.GetLonMax() + cte.GetLonMin())/2;
        d = DistGreatCircle(cte.GetLatMax(), clon, gLat, gLon);
        t = DistGreatCircle(cte.GetLatMin(), clon, gLat, gLon);
        if (t < d)
            d = t;

        clat = (cte.GetLatMax() + cte.GetLatMin())/2;
        t = DistGreatCircle(clat, cte.GetLonMin(), gLat, gLon);
        if (t < d)
            d = t;
        t = DistGreatCircle(clat, cte.GetLonMax(), gLat, gLon);
        if (t < d)
            d = t;
    }
    return d;
}

WX_DEFINE_SORTED_ARRAY_INT(int, MySortedArrayInt);
static int CompareInts(int n1, int n2)
{
    double d1 = chart_dist(n1);
    double d2 = chart_dist(n2);
    return (int)(d1 - d2);
}

class compress_target
{
public:
    wxString chart_path;
    double distance;
};

WX_DECLARE_OBJARRAY(compress_target, ArrayOfCompressTargets);
WX_DEFINE_OBJARRAY(ArrayOfCompressTargets);

#include <wx/arrimpl.cpp> 
// end duplicated code

void ParseAllENC(wxWindow* parent)
{
    MySortedArrayInt idx_sorted_by_distance(CompareInts);

    // Building the cache may take a long time....
    // Be a little smarter.
    // Build a sorted array of chart database indices, sorted on distance from the ownship currently.
    // This way, a user may build a few chart SENCs for immediate use, then "skip" or "cancel"out on the rest until later.
    int count = 0;
    for(int i = 0; i<ChartData->GetChartTableEntries(); i++) {
        /* skip if not ENC */
        const ChartTableEntry &cte = ChartData->GetChartTableEntry(i);
        if(CHART_TYPE_S57 != cte.GetChartType())
            continue;

        idx_sorted_by_distance.Add(i);
        count++;
    }


    if(count == 0)
        return;

    wxLogMessage(wxString::Format(_T("ParseAllENC() count = %d"), count ));

    //  Build another array of sorted compression targets.
    //  We need to do this, as the chart table will not be invariant
    //  after the compression threads start, so our index array will be invalid.

    ArrayOfCompressTargets ct_array;
    for(unsigned int j = 0; j<idx_sorted_by_distance.GetCount(); j++) {

        int i = idx_sorted_by_distance[j];

        const ChartTableEntry &cte = ChartData->GetChartTableEntry(i);
        double distance = chart_dist(i);

        wxString filename = cte.GetFullSystemPath();

        compress_target *pct = new compress_target;
        pct->distance = distance;
        pct->chart_path = filename;

        ct_array.push_back(pct);
    }

    int thread_count = 0;
    ParseENCWorkerThread **workers = NULL;

    extern int              g_nCPUCount;
    if(g_nCPUCount > 0)
        thread_count = g_nCPUCount;
    else
        thread_count = wxThread::GetCPUCount();

    if (thread_count < 1) {
        // obviously there's a least one CPU!
        thread_count = 1;
    }

    //thread_count = 1; // for now because there is a problem with more than 1

#if 0
    workers = new ParseENCWorkerThread*[thread_count];
    for(int t = 0; t < thread_count; t++)
        workers[t] = NULL;
#endif

    wxGenericProgressDialog *prog = nullptr;
    wxSize csz = GetOCPNCanvasWindow()->GetClientSize();

    if(1){    
        long style =  wxPD_SMOOTH | wxPD_ELAPSED_TIME | wxPD_ESTIMATED_TIME | wxPD_REMAINING_TIME | wxPD_CAN_SKIP ;

        prog = new wxGenericProgressDialog();
        wxFont *qFont = GetOCPNScaledFont(_("Dialog"));
        prog->SetFont( *qFont );

        prog->Create(_("OpenCPN ENC Prepare"), _T("Longgggggggggggggggggggggggggggg"), count+1, parent, style );

        // make wider to show long filenames
        //wxSize sz = prog->GetSize();
        //sz.x = csz.x * 8 / 10;
        //prog->SetSize( sz );

        DimeControl( prog );
        prog->Show();
    }

        // parse targets
        bool skip = false;
        count = 0;
        for(unsigned int j = 0; j<ct_array.size(); j++) {
            wxString filename = ct_array[j].chart_path;
            double distance = ct_array[j].distance;
            int index = ChartData->FinddbIndex(filename);
            if (index < 0) 
                continue;
            const ChartTableEntry &cte = ChartData->GetChartTableEntry(index);
            Extent ext;
            ext.NLAT = cte.GetLatMax();
            ext.SLAT = cte.GetLatMin();
            ext.WLON = cte.GetLonMin();
            ext.ELON = cte.GetLonMax();

            int scale = cte.GetScale();

            wxString msg;
            msg.Printf( _("Distance from Ownship:  %4.0f NMi"), distance);

            count++;
            if(wxThread::IsMain()){
                if(prog){
                    wxSize sz = prog->GetSize();
                    if(sz.x > 600){
                        msg += _T("   Chart:");
                        msg += filename;
                    }
                    prog->Update(count, msg, &skip );
#ifndef __WXMSW__
                    prog->Raise();
#endif                    
                }
                if(skip)
                    break;
            }

#if 1
            if(ps52plib){
                s57chart *newChart = new s57chart;

                newChart->SetNativeScale(scale);
                newChart->SetFullExtent(ext);
                newChart->DisableBackgroundSENC();

                newChart->FindOrCreateSenc(filename, false);    // no progress dialog required
                delete newChart;

                if(wxThread::IsMain()){
                    msg.Printf( _("ENC Completed.") );
                    if(prog){
                        prog->Update(count, msg, &skip );
#ifndef __WXMSW__
                        prog->Raise();
#endif                        
                    }
                    if(skip)
                        break;
                }


            }


#else
            for(int t = 0;; t=(t+1)%thread_count) {
                if(!workers[t]) {
                    workers[t] = new ParseENCWorkerThread(filename);
                    workers[t]->Run();
                    break;
                }

                if(!workers[t]->IsAlive()) {
                    workers[t]->Wait();
                    delete workers[t];
                    workers[t] = NULL;
                }
                if(t == 0) {
                    //                ::wxYield();                // allow ChartCanvas main message loop to run 
                    wxThread::Sleep(1); /* wait for a worker to finish */
                }
            }
#endif

#if defined(__WXMSW__) || defined (__WXOSX__)
            ::wxSafeYield();
#endif            
        }

#if 0
        /* wait for workers to finish, and clean up after then */
        for(int t = 0; t<thread_count; t++) {
                        if(workers[t]) {
                            workers[t]->Wait();
                            delete workers[t];
                        }
        }
        delete [] workers;
#endif
        
        delete prog;
}


bool MyApp::OnInit()
{
    if( !wxApp::OnInit() ) return false;
#ifdef __OCPN__ANDROID__
    androidEnableBackButton( false );
    androidEnableOptionItems( false );
#endif

    GpxDocument::SeedRandom();

    last_own_ship_sog_cog_calc_ts = wxInvalidDateTime;

#if defined(__WXGTK__)  && defined(ocpnUSE_GLES) && defined(__ARM_ARCH)
    // There is a race condition between cairo which is used for text rendering
    // by gtk and EGL which without the below code causes a bus error and the
    // program aborts before startup
    // this hack forces cairo to load right now by rendering some text

    wxBitmap bmp( 10, 10, -1 );
    wxMemoryDC dc;
    dc.SelectObject( bmp );
    dc.DrawText( _T("X"), 0, 0 );
#endif

    //  On Windows
    //  We allow only one instance unless the portable option is used
#ifdef __WXMSW__
    m_checker = new wxSingleInstanceChecker(_T("_OpenCPN_SILock"), g_Platform->GetPrivateDataDir());
    if(!g_bportable) {
        if ( m_checker->IsAnotherRunning() )
            return false;               // exit quietly
    }
#endif

    // Instantiate the global OCPNPlatform class
    g_Platform = new OCPNPlatform;

    //  Perform first stage initialization
    OCPNPlatform::Initialize_1( );

#if wxCHECK_VERSION(3,0,0)
    // Set the name of the app as displayed to the user.
    // This is necessary at least on OS X, for the capitalisation to be correct in the system menus.
    MyApp::SetAppDisplayName("OpenCPN");
#endif




    //  Seed the random number generator
    wxDateTime x = wxDateTime::UNow();
    long seed = x.GetMillisecond();
    seed *= x.GetTicks();
    srand(seed);


//Fulup: force floating point to use dot as separation.
// This needs to be set early to catch numerics in config file.
    setlocale( LC_NUMERIC, "C" );



    g_start_time = wxDateTime::Now();

    g_loglast_time = g_start_time;
    g_loglast_time.MakeGMT();
    g_loglast_time.Subtract( wxTimeSpan( 0, 29, 0, 0 ) ); // give 1 minute for GPS to get a fix

    AnchorPointMinDist = 5.0;


//      Init the private memory manager
    malloc_max = 0;

    //      Record initial memory status
    GetMemoryStatus( &g_mem_total, &g_mem_initial );


// Set up default FONT encoding, which should have been done by wxWidgets some time before this......
    wxFont temp_font( 10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, FALSE, wxString( _T("") ),
            wxFONTENCODING_SYSTEM );
    temp_font.SetDefaultEncoding( wxFONTENCODING_SYSTEM );


    //      Establish Log File location
    if(!g_Platform->InitializeLogFile())
        return false;


#ifdef __WXMSW__

    //  Un-comment the following to establish a separate console window as a target for printf() in Windows
    //     RedirectIOToConsole();

#endif

//      Send init message
    wxLogMessage( _T("\n\n________\n") );

    wxString g_vs = wxString(VERSION_FULL).Trim(true).Trim(false);
    wxDateTime now = wxDateTime::Now();
    LOG_INFO("------- OpenCPN version %s restarted at %s -------\n",
             VERSION_FULL, now.FormatISODate().mb_str().data());
    wxLogLevel level = wxLog::GetLogLevel();
    LOG_INFO("Using loglevel %s", OcpnLog::level2str(level).c_str());

    wxString wxver(wxVERSION_STRING);
    wxver.Prepend( _T("wxWidgets version: ") );

    wxPlatformInfo platforminfo = wxPlatformInfo::Get();

    wxString os_name;
#ifndef __OCPN__ANDROID__
    os_name = platforminfo.GetOperatingSystemIdName();
#else
    os_name = platforminfo.GetOperatingSystemFamilyName();
#endif
#ifdef __WXOSX__
    wxString osName;
    osName = wxGetOsDescription();
    wxString platform = os_name + _T(" ") +
    platforminfo.GetArchName() + _T(" ") +
    platforminfo.GetPortIdName() + _T(" ") + osName;
#else
    wxString platform = os_name + _T(" ") +
    platforminfo.GetArchName()+ _T(" ") +
    platforminfo.GetPortIdName();
#endif
    //  result: Mac OS X 64 bit wxMac
    //  result new: wxWidgets version: wxWidgets 3.1.3 Apple Mac OS X 64 bit wxMacOS X El Capitan Version 10.11.5 (Build 15F34)
    //  result aktuell: wxWidgets version: wxWidgets 3.1.3 Apple Mac OS X 64 bit wxMacmacOS Sierra Version 10.12 (Build 16A323)
    // result now: wxWidgets version: wxWidgets 3.1.3 Apple Mac OS X 64 bit wxMac macOS Mojave Version 10.14.6 (Build 18G2022)

    wxLogMessage( wxver + _T(" ") + platform );

    ::wxGetOsVersion(&osMajor, &osMinor);
    wxString osVersionMsg;
    osVersionMsg.Printf(_T("OS Version reports as:  %d.%d"), osMajor, osMinor);
    wxLogMessage(osVersionMsg);

    wxLogMessage( _T("MemoryStatus:  mem_total: %d mb,  mem_initial: %d mb"), g_mem_total / 1024,
            g_mem_initial / 1024 );

    //    Initialize embedded PNG icon graphics
    ::wxInitAllImageHandlers();


    wxString imsg = _T("SData_Locn is ");
    imsg += g_Platform->GetSharedDataDir();
    wxLogMessage( imsg );

#ifdef __OCPN__ANDROID__
    //  Now we can load a Qt StyleSheet, if present
    wxString style_file = g_Platform->GetSharedDataDir();
    style_file += _T("styles");
    appendOSDirSlash( &style_file );
    style_file += _T("qtstylesheet.qss");
    if(LoadQtStyleSheet(style_file)){
        wxString smsg = _T("Loaded Qt Stylesheet: ") + style_file;
        wxLogMessage( smsg );
    }
    else{
        wxString smsg = _T("Qt Stylesheet not found: ") + style_file;
        wxLogMessage( smsg );
    }
#endif

    //      Create some static strings
    pInit_Chart_Dir = new wxString();

    //  Establish an empty ChartCroupArray
    g_pGroupArray = new ChartGroupArray;


    imsg = _T("PrivateDataDir is ");
    imsg += g_Platform->GetPrivateDataDir();
    wxLogMessage( imsg );


//      Create an array string to hold repeating messages, so they don't
//      overwhelm the log
    pMessageOnceArray = new wxArrayString;

//      Init the Route Manager
    g_pRouteMan = new Routeman( this );

    //      Init the Selectable Route Items List
    pSelect = new Select();
    pSelect->SetSelectPixelRadius( 12 );

    //      Init the Selectable Tide/Current Items List
    pSelectTC = new Select();
    //  Increase the select radius for tide/current stations
    pSelectTC->SetSelectPixelRadius( 25 );

    //      Init the Selectable AIS Target List
    pSelectAIS = new Select();
    pSelectAIS->SetSelectPixelRadius( 12 );

//      Initially AIS display is always on
    g_bShowAIS = true;
    g_pais_query_dialog_active = NULL;

//      Who am I?
    phost_name = new wxString( ::wxGetHostName() );

//      Initialize connection parameters array
    g_pConnectionParams = new wxArrayOfConnPrm();

//      Initialize some lists    
    //    Layers
    pLayerList = new LayerList;
    //  Routes
    pRouteList = new RouteList;
    // Tracks
    pTrackList = new TrackList;


//      (Optionally) Capture the user and file(effective) ids
//  Some build environments may need root privileges for hardware
//  port I/O, as in the NMEA data input class.  Set that up here.

#ifndef __WXMSW__
#ifdef PROBE_PORTS__WITH_HELPER
    user_user_id = getuid ();
    file_user_id = geteuid ();
#endif
#endif


    bool b_initial_load = false;

    wxFileName config_test_file_name( g_Platform->GetConfigFileName() );
    if( config_test_file_name.FileExists() ) wxLogMessage(
        _T("Using existing Config_File: ") + g_Platform->GetConfigFileName() );
    else {
        {
            wxLogMessage( _T("Creating new Config_File: ") + g_Platform->GetConfigFileName() );

            b_initial_load = true;

            if( true != config_test_file_name.DirExists( config_test_file_name.GetPath() ) )
                if( !config_test_file_name.Mkdir(config_test_file_name.GetPath() ) )
                    wxLogMessage( _T("Cannot create config file directory for ") + g_Platform->GetConfigFileName() );
        }
    }

    //      Open/Create the Config Object
    pConfig = g_Platform->GetConfigObject();
    pConfig->LoadMyConfig();

    //  Override for some safe and nice default values if the config file was created from scratch
    if(b_initial_load)
        g_Platform->SetDefaultOptions();

    g_Platform->applyExpertMode(g_bUIexpert);

    // Now initialize UI Style.
    g_StyleManager = new ocpnStyle::StyleManager();

//     if(g_useMUI)
//         g_uiStyle = _T("MUI_flat");

    g_StyleManager->SetStyle( _T("MUI_flat") );
    if( !g_StyleManager->IsOK() ) {
        wxString msg = _("Failed to initialize the user interface. ");
        msg << _("OpenCPN cannot start. ");
        msg << _("The necessary configuration files were not found. ");
        msg << _("See the log file at ") << g_Platform->GetLogFileName() << _(" for details.") << _T("\n\n");
        msg << g_Platform->GetSharedDataDir();

        wxMessageDialog w( NULL, msg, _("Failed to initialize the user interface. "),
                           wxCANCEL | wxICON_ERROR );
        w.ShowModal();
        exit( EXIT_FAILURE );
    }

    if(g_useMUI){
        ocpnStyle::Style* style = g_StyleManager->GetCurrentStyle();
        style->chartStatusWindowTransparent = true;
    }


    //      Init the WayPoint Manager
    pWayPointMan = NULL;

    g_display_size_mm = wxMax(100, g_Platform->GetDisplaySizeMM());
    wxString msg;
    msg.Printf(_T("Detected display size (horizontal): %d mm"), (int) g_display_size_mm);
    wxLogMessage(msg);

    // User override....
    if((g_config_display_size_mm > 0) &&(g_config_display_size_manual)){
        g_display_size_mm = g_config_display_size_mm;
        wxString msg;
        msg.Printf(_T("Display size (horizontal) config override: %d mm"), (int) g_display_size_mm);
        wxLogMessage(msg);
        g_Platform->SetDisplaySizeMM(g_display_size_mm);
    }

    if(g_btouch){
        int SelectPixelRadius = 50;

        pSelect->SetSelectPixelRadius(SelectPixelRadius);
        pSelectTC->SetSelectPixelRadius( wxMax(25, SelectPixelRadius) );
        pSelectAIS->SetSelectPixelRadius(SelectPixelRadius);
    }


    //        Is this the first run after a clean install?
    if( !n_NavMessageShown ) g_bFirstRun = true;

    //  Now we can set the locale
    //    using wxWidgets/gettext methodology....


#if wxUSE_XLOCALE || !wxCHECK_VERSION(3,0,0)

    //  Where are the opencpn.mo files?
    g_Platform->SetLocaleSearchPrefixes();

    wxString def_lang_canonical = g_Platform->GetDefaultSystemLocale();

    imsg = _T("System default Language:  ") + def_lang_canonical;
    wxLogMessage( imsg );

    wxString cflmsg = _T("Config file language:  ") + g_locale;
    wxLogMessage( cflmsg );

    //  Make any adjustments necessary
    g_locale = g_Platform->GetAdjustedAppLocale();
    cflmsg = _T("Adjusted App language:  ") + g_locale;
    wxLogMessage( cflmsg );


    // Set the desired locale
    g_Platform->ChangeLocale(g_locale, plocale_def_lang, &plocale_def_lang);

    imsg = _T("Opencpn language set to:  ");
    imsg += g_locale;
    wxLogMessage( imsg );

#ifdef __WXOSX__
    if( plocale_def_lang ) plocale_def_lang->AddCatalog("wxstd");
#endif

    //  French language locale is assumed to include the AZERTY keyboard
    //  This applies to either the system language, or to OpenCPN language selection
    if( g_locale == _T("fr_FR") ) g_b_assume_azerty = true;
#else
    wxLogMessage( _T("wxLocale support not available") );
#endif

    // Instantiate and initialize the Config Manager
    ConfigMgr::Get();

    // Is this an upgrade?
    wxString vs =
        wxString("Version ") +  VERSION_FULL + " Build " + VERSION_DATE;
    g_bUpgradeInProcess = (vs != g_config_version_string);

    g_Platform->SetUpgradeOptions(vs, g_config_version_string);

    //  log deferred log restart message, if it exists.
#ifdef __WXOSX__
    if( !g_Platform->GetLargeLogMessage().IsEmpty() )
    {
        wxLogMessage( g_Platform->GetLargeLogMessage() );
        wxString msg1 = g_Platform->GetLargeLogMessage();
        OCPNMessageBox(gFrame, msg1, wxString( _("OpenCPN Info") ), wxICON_INFORMATION | wxOK );
    }
#else
    if( !g_Platform->GetLargeLogMessage().IsEmpty() )
        wxLogMessage( g_Platform->GetLargeLogMessage() );
#endif
    //  Validate OpenGL functionality, if selected
#ifdef ocpnUSE_GL

#ifdef __WXMSW__
#if !wxCHECK_VERSION(2, 9, 0)           // The OpenGL test app only runs on wx 2.8, unavailable on wx3.x
        
    if( /*g_bopengl &&*/ !g_bdisable_opengl ) {
        wxFileName fn(g_Platform->GetExePath());
        bool b_test_result = TestGLCanvas(fn.GetPathWithSep() );

        if( !b_test_result )
            wxLogMessage( _T("OpenGL disabled due to test app failure.") );

        g_bdisable_opengl = !b_test_result;
    }
#endif
#endif

#else
    g_bdisable_opengl = true;;
#endif

    if(g_bdisable_opengl)
        g_bopengl = false;

#if defined(__UNIX__) && !defined(__OCPN__ANDROID__) && !defined(__WXOSX__)
    if(g_bSoftwareGL)
        setenv("LIBGL_ALWAYS_SOFTWARE", "1", 1);
#endif

    g_bTransparentToolbarInOpenGLOK = isTransparentToolbarInOpenGLOK();

    // On Windows platforms, establish a default cache managment policy
    // as allowing OpenCPN a percentage of available physical memory,
    // not to exceed 1 GB
    // Note that this logic implies that Windows platforms always use
    // the memCacheLimit policy, and never use the fallback nCacheLimit policy
#ifdef __WXMSW__
    if( 0 == g_memCacheLimit )
        g_memCacheLimit = (int) ( g_mem_total * 0.5 );
    g_memCacheLimit = wxMin(g_memCacheLimit, 1024 * 1024); // math in kBytes, Max is 1 GB
#else
    // All other platforms will use the nCacheLimit policy
    // sinc on linux it is impossible to accurately measure the application memory footprint without
    // expensive methods such as malloc/free tracking, and such

    g_memCacheLimit = 0;
    if( 0 ==  g_nCacheLimit)                            // allow config file override
        g_nCacheLimit = CACHE_N_LIMIT_DEFAULT;
#endif

//      Establish location and name of chart database
    ChartListFileName = newPrivateFileName(g_Platform->GetPrivateDataDir(), "chartlist.dat", "CHRTLIST.DAT");

//      Establish location and name of AIS MMSI -> Target Name mapping
    AISTargetNameFileName = newPrivateFileName(g_Platform->GetPrivateDataDir(), "mmsitoname.csv", "MMSINAME.CSV");

//      Establish guessed location of chart tree
    if( pInit_Chart_Dir->IsEmpty() ) {
        wxStandardPaths& std_path = g_Platform->GetStdPaths();

        if( !g_bportable )
#ifndef __OCPN__ANDROID__
        pInit_Chart_Dir->Append( std_path.GetDocumentsDir() );
#else
        pInit_Chart_Dir->Append( androidGetExtStorageDir() );
#endif
    }

//      Establish the GSHHS Dataset location
    gDefaultWorldMapLocation = "gshhs";
    gDefaultWorldMapLocation.Prepend( g_Platform->GetSharedDataDir() );
    gDefaultWorldMapLocation.Append( wxFileName::GetPathSeparator() );
    if( gWorldMapLocation == wxEmptyString ) {
        gWorldMapLocation = gDefaultWorldMapLocation;
    }

    //  Check the global Tide/Current data source array
    //  If empty, preset default (US + ROW) data sources
    wxString default_tcdata0 =
        ( g_Platform->GetSharedDataDir() + _T("tcdata") +
         wxFileName::GetPathSeparator() + _T("harmonics-dwf-20210110-free.tcd"));
    wxString default_tcdata1 =
        (g_Platform->GetSharedDataDir() + _T("tcdata") +
         wxFileName::GetPathSeparator() + _T("HARMONICS_NO_US.IDX"));
    
    if(!TideCurrentDataSet.GetCount()) {
        TideCurrentDataSet.Add(g_Platform->NormalizePath(default_tcdata0));
        TideCurrentDataSet.Add(g_Platform->NormalizePath(default_tcdata1));
    }
    else {
        wxString first_tide = TideCurrentDataSet[0];
        wxFileName ft(first_tide);
        if(!ft.FileExists()){
            TideCurrentDataSet.RemoveAt(0);
            TideCurrentDataSet.Insert(g_Platform->NormalizePath(default_tcdata0), 0);
            TideCurrentDataSet.Add(g_Platform->NormalizePath(default_tcdata1));
        }
    }


    //  Check the global AIS alarm sound file
    //  If empty, preset default
    if(g_sAIS_Alert_Sound_File.IsEmpty()) {
        wxString default_sound =  ( g_Platform->GetSharedDataDir() + _T("sounds") +
        wxFileName::GetPathSeparator() +
        _T("2bells.wav"));
        g_sAIS_Alert_Sound_File = g_Platform->NormalizePath(default_sound);
    }

    gpIDX = NULL;
    gpIDXn = 0;

    g_Platform->Initialize_2();

//  Set up the frame initial visual parameters
//      Default size, resized later
    wxSize new_frame_size( -1, -1 );
    int cx, cy, cw, ch;
    ::wxClientDisplayRect( &cx, &cy, &cw, &ch );

    InitializeUserColors();

    if( ( g_nframewin_x > 100 ) && ( g_nframewin_y > 100 ) && ( g_nframewin_x <= cw )
            && ( g_nframewin_y <= ch ) )
#ifdef __WXOSX__
        new_frame_size.Set( 1625, 937 );
#else
        new_frame_size.Set( g_nframewin_x, g_nframewin_y );
#endif
    else
#ifdef __WXOSX__
        new_frame_size.Set( 1625, 937 );
#else
        new_frame_size.Set( cw * 7 / 10, ch * 7 / 10 );
#endif

    //  Try to detect any change in physical screen configuration
    //  This can happen when drivers are changed, for instance....
    //  and can confuse the WUI layout perspective stored in the config file.
    //  If detected, force a nominal window size and position....
    if( ( g_lastClientRectx != cx ) || ( g_lastClientRecty != cy ) || ( g_lastClientRectw != cw )
            || ( g_lastClientRecth != ch ) ) {
#ifdef __WXOSX__
        new_frame_size.Set( 1625, 937 );
#else
        new_frame_size.Set( cw * 7 / 10, ch * 7 / 10 );
#endif
        g_bframemax = false;
    }

    g_lastClientRectx = cx;
    g_lastClientRecty = cy;
    g_lastClientRectw = cw;
    g_lastClientRecth = ch;

    //  Validate config file position
    wxPoint position( 0, 0 );
    wxSize dsize = wxGetDisplaySize();

#ifdef __WXMAC__
    g_nframewin_posy = wxMax(g_nframewin_posy, 22);
#endif

    if( ( g_nframewin_posx < dsize.x ) && ( g_nframewin_posy < dsize.y ) ) position = wxPoint(
            g_nframewin_posx, g_nframewin_posy );

#ifdef __WXMSW__
    //  Support MultiMonitor setups which an allow negative window positions.
    RECT frame_rect;
    frame_rect.left = position.x;
    frame_rect.top = position.y;
    frame_rect.right = position.x + new_frame_size.x;
    frame_rect.bottom = position.y + new_frame_size.y;

    //  If the requested frame window does not intersect any installed monitor,
    //  then default to simple primary monitor positioning.
    if( NULL == MonitorFromRect( &frame_rect, MONITOR_DEFAULTTONULL ) ) position = wxPoint( 10,
            10 );
#endif

#ifdef __OCPN__ANDROID__
    wxSize asz = getAndroidDisplayDimensions();
    ch = asz.y;
    cw = asz.x;
//    qDebug() << cw << ch;

    if((cw > 200) && (ch > 200) )
        new_frame_size.Set( cw, ch );
    else
        new_frame_size.Set( 800, 400 );
#endif

    //  For Windows and GTK, provide the expected application Minimize/Close bar
    long app_style = wxDEFAULT_FRAME_STYLE;
    app_style |= wxWANTS_CHARS;

// Create the main frame window
    wxString myframe_window_title = wxString::Format(wxT("OpenCPN %s"),
            VERSION_FULL); //Gunther

    if( g_bportable ) {
        myframe_window_title += _(" -- [Portable(-p) executing from ");
        myframe_window_title += g_Platform->GetHomeDir();
        myframe_window_title += _T("]");
    }
#ifdef __WXOSX__
    int ret = GetMacMonitorSize();
    wxString msga;
    msga.Printf(_T("Detected display size (horizontal): %d mm"), (int) ret);
    wxLogMessage(msga);
#endif
    wxString fmsg;
    fmsg.Printf(_T("Creating MyFrame...size(%d, %d)  position(%d, %d)"), new_frame_size.x, new_frame_size.y, position.x, position.y);
    wxLogMessage(fmsg);

    gFrame = new MyFrame( NULL, myframe_window_title, position, new_frame_size, app_style ); //Gunther

    //  Do those platform specific initialization things that need gFrame
    g_Platform->Initialize_3();


//  Initialize the Plugin Manager
    g_pi_manager = new PlugInManager( gFrame );

    //g_pauimgr = new wxAuiManager;
    g_pauimgr = new OCPN_AUIManager;
    g_pauidockart= new wxAuiDefaultDockArt;
    g_pauimgr->SetArtProvider(g_pauidockart);
    g_pauimgr->SetDockSizeConstraint(.9, .9);

    //g_pauimgr->SetFlags(g_pauimgr->GetFlags() | wxAUI_MGR_LIVE_RESIZE);

    g_grad_default = g_pauidockart->GetMetric(wxAUI_DOCKART_GRADIENT_TYPE);
    g_border_color_default = g_pauidockart->GetColour(wxAUI_DOCKART_BORDER_COLOUR );
    g_border_size_default = g_pauidockart->GetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE );
    g_sash_size_default = g_pauidockart->GetMetric(wxAUI_DOCKART_SASH_SIZE);
    g_caption_color_default = g_pauidockart->GetColour(wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR);
    g_sash_color_default = g_pauidockart->GetColour(wxAUI_DOCKART_SASH_COLOUR );
    g_background_color_default = g_pauidockart->GetColour(wxAUI_DOCKART_BACKGROUND_COLOUR );



// tell wxAuiManager to manage the frame
    g_pauimgr->SetManagedWindow( gFrame );

    gFrame->CreateCanvasLayout();

    //gFrame->RequestNewMasterToolbar( true );

    gFrame->SetChartUpdatePeriod();             // Reasonable default

    gFrame->Enable();

    gFrame->GetPrimaryCanvas()->SetFocus();

    pthumbwin = new ThumbWin( gFrame->GetPrimaryCanvas() );

    gFrame->ApplyGlobalSettings( false );               // done once on init with resize


    gFrame->SetAllToolbarScale();

     
// Show the frame

    gFrame->Show( TRUE );

#ifdef __OCPN__ANDROID__
    androidShowBusyIcon();
#endif

    gFrame->SetAndApplyColorScheme( global_color_scheme );

    if( g_bframemax ) gFrame->Maximize( true );

    if( g_bresponsive  && ( gFrame->GetPrimaryCanvas()->GetPixPerMM() > 4.0))
        gFrame->Maximize( true );

    //  Yield to pick up the OnSize() calls that result from Maximize()
    Yield();

    //   Build the initial chart dir array
    ArrayOfCDI ChartDirArray;
    pConfig->LoadChartDirArray( ChartDirArray );

    //  Windows installer may have left hints regarding the initial chart dir selection
#ifdef __WXMSW__
    if( g_bFirstRun && (ChartDirArray.GetCount() == 0) ) {
        int ndirs = 0;

        wxRegKey RegKey( wxString( _T("HKEY_LOCAL_MACHINE\\SOFTWARE\\OpenCPN") ) );
        if( RegKey.Exists() ) {
            wxLogMessage( _("Retrieving initial Chart Directory set from Windows Registry") );
            wxString dirs;
            RegKey.QueryValue( wxString( _T("ChartDirs") ), dirs );

            wxStringTokenizer tkz( dirs, _T(";") );
            while( tkz.HasMoreTokens() ) {
                wxString token = tkz.GetNextToken();

                ChartDirInfo cdi;
                cdi.fullpath = token.Trim();
                cdi.magic_number = _T("");

                ChartDirArray.Add( cdi );
                ndirs++;
            }

        }

		if (g_bportable)
		{
			ChartDirInfo cdi;
			cdi.fullpath =_T("charts");
			cdi.fullpath.Prepend(g_Platform->GetSharedDataDir());
			cdi.magic_number = _T("");
			ChartDirArray.Add(cdi);
			ndirs++;
		}

        if( ndirs ) pConfig->UpdateChartDirs( ChartDirArray );

    }
#endif

//    If the ChartDirArray is empty at this point, any existing chart database file must be declared invalid,
//    So it is best to simply delete it if present.
//    TODO  There is a possibility of recreating the dir list from the database itself......

    if( !ChartDirArray.GetCount() )
        if(::wxFileExists( ChartListFileName ))
            ::wxRemoveFile( ChartListFileName );

//      Try to load the current chart list Data file
    ChartData = new ChartDB( );
    if (!ChartData->LoadBinary(ChartListFileName, ChartDirArray)) {
        g_bNeedDBUpdate = true;
    }


    //  Verify any saved chart database startup index
    if(g_restore_dbindex >= 0){
        if(ChartData->GetChartTableEntries() == 0)
            g_restore_dbindex = -1;

        else if(g_restore_dbindex > (ChartData->GetChartTableEntries()-1))
            g_restore_dbindex = 0;
    }

    //  Apply the inital Group Array structure to the chart data base
    ChartData->ApplyGroupArray( g_pGroupArray );

//      All set to go.....

    // Process command line option to rebuild cache
#ifdef ocpnUSE_GL
extern ocpnGLOptions g_GLOptions;

    if(g_rebuild_gl_cache && g_bopengl &&
        g_GLOptions.m_bTextureCompression && g_GLOptions.m_bTextureCompressionCaching ) {

        gFrame->ReloadAllVP();                  //  Get a nice chart background loaded

        //      Turn off the toolbar as a clear signal that the system is busy right now.
        // Note: I commented this out because the toolbar never comes back for me
        // and is unusable until I restart opencpn without generating the cache
//        if( g_MainToolbar )
//            g_MainToolbar->Hide();

        if(g_glTextureManager)
            g_glTextureManager->BuildCompressedCache();

    }
#endif

    if(g_parse_all_enc )
        ParseAllENC(gFrame);

//      establish GPS timeout value as multiple of frame timer
//      This will override any nonsense or unset value from the config file
    if( ( gps_watchdog_timeout_ticks > 60 ) || ( gps_watchdog_timeout_ticks <= 0 ) ) gps_watchdog_timeout_ticks =
            ( GPS_TIMEOUT_SECONDS * 1000 ) / TIMER_GFRAME_1;

    wxString dogmsg;
    dogmsg.Printf( _T("GPS Watchdog Timeout is: %d sec."), gps_watchdog_timeout_ticks );
    wxLogMessage( dogmsg );

    sat_watchdog_timeout_ticks = 12;

    gGPS_Watchdog = 2;
    gHDx_Watchdog = 2;
    gHDT_Watchdog = 2;
    gSAT_Watchdog = 2;
    gVAR_Watchdog = 2;

    g_priSats = 99;

    //  Most likely installations have no ownship heading information
    g_bHDT_Rx = false;
    g_bVAR_Rx = false;

//  Start up a new track if enabled in config file
    if( g_bTrackCarryOver )
        g_bDeferredStartTrack = true;

    pAnchorWatchPoint1 = NULL;
    pAnchorWatchPoint2 = NULL;

    Yield();

    gFrame->DoChartUpdate();

    FontMgr::Get().ScrubList(); // Clean the font list, removing nonsensical entries


    gFrame->ReloadAllVP();                  // once more, and good to go


    gFrame->Refresh( false );
    gFrame->Raise();

    gFrame->GetPrimaryCanvas()->Enable();
    gFrame->GetPrimaryCanvas()->SetFocus();

    //  This little hack fixes a problem seen with some UniChrome OpenGL drivers
    //  We need a deferred resize to get glDrawPixels() to work right.
    //  So we set a trigger to generate a resize after 5 seconds....
    //  See the "UniChrome" hack elsewhere
#ifdef ocpnUSE_GL
    if ( !g_bdisable_opengl )
    {
        glChartCanvas *pgl = (glChartCanvas *) gFrame->GetPrimaryCanvas()->GetglCanvas();
        if( pgl && ( pgl->GetRendererString().Find( _T("UniChrome") ) != wxNOT_FOUND ) )
        {
            gFrame->m_defer_size = gFrame->GetSize();
            gFrame->SetSize( gFrame->m_defer_size.x - 10, gFrame->m_defer_size.y );
            g_pauimgr->Update();
            gFrame->m_bdefer_resize = true;
        }
    }
#endif

    if ( g_start_fullscreen )
        gFrame->ToggleFullScreen();

#ifdef __OCPN__ANDROID__
    //  We need a resize to pick up height adjustment after building android ActionBar
    if(pConfig->m_bShowMenuBar)
        gFrame->SetSize(getAndroidDisplayDimensions());
    androidSetFollowTool(gFrame->GetPrimaryCanvas()->m_bFollow);
#endif

    gFrame->Raise();
    gFrame->GetPrimaryCanvas()->Enable();
    gFrame->GetPrimaryCanvas()->SetFocus();


    // Setup Tides/Currents to settings present at last shutdown
// TODO
//     gFrame->ShowTides( g_bShowTide );
//     gFrame->ShowCurrents( g_bShowCurrent );
 
//      Start up the ticker....
    gFrame->FrameTimer1.Start( TIMER_GFRAME_1, wxTIMER_CONTINUOUS );

//      Start up the ViewPort Rotation angle Averaging Timer....
    gFrame->FrameCOGTimer.Start( 10, wxTIMER_CONTINUOUS );

    // Start delayed initialization chain after 100 milliseconds
    gFrame->InitTimer.Start( 100, wxTIMER_CONTINUOUS );
#ifndef __WXOSX__
    wxLogMessage( wxString::Format(_("OpenCPN Initialized in %ld ms."), init_sw.Time() ) );
#endif
    OCPNPlatform::Initialize_4( );

#ifdef __OCPN__ANDROID__
    androidHideBusyIcon();
#endif
    wxLogMessage( wxString::Format(_("OpenCPN Initialized in %ld ms."), init_sw.Time() ) );

    //  Send the Welcome/warning message if it has never been sent before,
    //  or if the version string has changed at all
    //  We defer until here to allow for localization of the message
        if( !n_NavMessageShown || ( vs != g_config_version_string ) ) {
            if( wxID_CANCEL == ShowNavWarning() )
                return false;
            n_NavMessageShown = 1;
        }

// As an a.e. Raspberry does not have a hardwareclock we will have some problems with date/time setting
    g_bHasHwClock = true; //by default most computers do have a hwClock
#if defined(__UNIX__) && !defined(__OCPN__ANDROID__)
    struct stat buffer;
    g_bHasHwClock = ( (stat ("/dev/rtc", &buffer) == 0) || (stat ("/dev/rtc0", &buffer) == 0) || (stat ("/dev/misc/rtc", &buffer) == 0) );
#endif

    g_config_version_string = vs;

    //The user accepted the "not for navigation" nag, so persist it here...
    pConfig->UpdateSettings();

    // Start delayed initialization chain after some milliseconds
    gFrame->InitTimer.Start( 5, wxTIMER_CONTINUOUS );

    g_pauimgr->Update();

    return TRUE;
}

int MyApp::OnExit()
{
    wxLogMessage( _T("opencpn::MyApp starting exit.") );

    //  Send current nav status data to log file   // pjotrc 2010.02.09

    wxDateTime lognow = wxDateTime::Now();
    lognow.MakeGMT();
    wxString day = lognow.FormatISODate();
    wxString utc = lognow.FormatISOTime();
    wxString navmsg = _T("LOGBOOK:  ");
    navmsg += day;
    navmsg += _T(" ");
    navmsg += utc;
    navmsg += _T(" UTC ");

    if( bGPSValid ) {
        wxString data;
        data.Printf( _T("OFF: Lat %10.5f Lon %10.5f "), gLat, gLon );
        navmsg += data;

        wxString cog;
        if( std::isnan(gCog) ) cog.Printf( _T("COG ----- ") );
        else
            cog.Printf( _T("COG %10.5f "), gCog );

        wxString sog;
        if( std::isnan(gSog) ) sog.Printf( _T("SOG -----  ") );
        else
            sog.Printf( _T("SOG %6.2f ") + getUsrSpeedUnit(), toUsrSpeed( gSog ) );

        navmsg += cog;
        navmsg += sog;

    } else {
        wxString data;
        data.Printf( _T("OFF: Lat %10.5f Lon %10.5f"), gLat, gLon );
        navmsg += data;
    }
    wxLogMessage( navmsg );
    g_loglast_time = lognow;

    if( ptcmgr ) delete ptcmgr;

    delete pConfig;
    delete pSelect;
    delete pSelectTC;
    delete pSelectAIS;

#ifdef USE_S57
    delete ps52plib;
#endif

    if(g_pGroupArray){
        for(unsigned int igroup = 0; igroup < g_pGroupArray->GetCount(); igroup++){
            delete g_pGroupArray->Item(igroup);
        }

        g_pGroupArray->Clear();
        delete g_pGroupArray;
    }

    delete pDummyChart;

    wxLogMessage( _T("opencpn::MyApp exiting cleanly...\n") );
    wxLog::FlushActive();

    g_Platform->CloseLogFile();

    delete phost_name;
    delete pInit_Chart_Dir;

    if (pTrackList)
    {
        pTrackList->DeleteContents(true);
        pTrackList->Clear();
        delete pTrackList;
        pTrackList = NULL;
    }

    delete g_pRouteMan;
    delete pWayPointMan;

    delete pMessageOnceArray;

    DeInitializeUserColors();

    delete pLayerList;

#ifdef USE_S57
    delete m_pRegistrarMan;
    CSVDeaccess( NULL );
#endif

    delete g_StyleManager;

#ifdef USE_S57
#ifdef __WXMSW__
#ifdef USE_GLU_TESS
#ifdef USE_GLU_DLL
    if(s_glu_dll_ready)
    FreeLibrary(s_hGLU_DLL);           // free the glu32.dll
#endif
#endif
#endif
#endif

    //      Restore any changed system colors
#ifdef __WXMSW__
    RestoreSystemColors();
#endif

#ifdef __MSVC__LEAK
    DeInitAllocCheck();
#endif

#if wxUSE_XLOCALE || !wxCHECK_VERSION(3,0,0)
    delete plocale_def_lang;
#endif

    FontMgr::Shutdown();

#ifdef __WXMSW__
    delete m_checker;
#endif


    g_Platform->OnExit_2();

    return TRUE;
}

#ifdef LINUX_CRASHRPT
void MyApp::OnFatalException () {
    g_crashprint.Report();
}
#endif

void MyApp::TrackOff( void )
{
    if( gFrame ) gFrame->TrackOff();
}




#include <wx/power.h>

//------------------------------------------------------------------------------
// MyFrame
//------------------------------------------------------------------------------

//      Frame implementation
wxDEFINE_EVENT(BELLS_PLAYED_EVTYPE, wxCommandEvent);

BEGIN_EVENT_TABLE(MyFrame, wxFrame) EVT_CLOSE(MyFrame::OnCloseWindow)
EVT_MENU(wxID_EXIT, MyFrame::OnExit)
EVT_SIZE(MyFrame::OnSize)
EVT_MOVE(MyFrame::OnMove)
EVT_ICONIZE(MyFrame::OnIconize)
EVT_MENU(-1, MyFrame::OnToolLeftClick)
EVT_TIMER(INIT_TIMER, MyFrame::OnInitTimer)
EVT_TIMER(FRAME_TIMER_1, MyFrame::OnFrameTimer1)
EVT_TIMER(FRAME_TC_TIMER, MyFrame::OnFrameTCTimer)
EVT_TIMER(FRAME_COG_TIMER, MyFrame::OnFrameCOGTimer)
EVT_TIMER(MEMORY_FOOTPRINT_TIMER, MyFrame::OnMemFootTimer)
EVT_MAXIMIZE(MyFrame::OnMaximize)
EVT_COMMAND(wxID_ANY, wxEVT_COMMAND_TOOL_RCLICKED, MyFrame::RequestNewToolbarArgEvent)
EVT_ERASE_BACKGROUND(MyFrame::OnEraseBackground)
EVT_TIMER(RECAPTURE_TIMER, MyFrame::OnRecaptureTimer)
EVT_TIMER(TOOLBAR_ANIMATE_TIMER, MyFrame::OnToolbarAnimateTimer)
EVT_COMMAND(wxID_ANY, BELLS_PLAYED_EVTYPE, MyFrame::OnBellsFinished)
#ifdef wxHAS_POWER_EVENTS
EVT_POWER_SUSPENDING(MyFrame::OnSuspending)
EVT_POWER_SUSPENDED(MyFrame::OnSuspended)
EVT_POWER_SUSPEND_CANCEL(MyFrame::OnSuspendCancel)
EVT_POWER_RESUME(MyFrame::OnResume)
#endif // wxHAS_POWER_EVENTS

END_EVENT_TABLE()


/*
 * Direct callback from completed sound, possibly in an interrupt
 * context. Just post an event to be processed in main thread.
 */
static void onBellsFinishedCB(void* ptr)
{
   auto framePtr  = static_cast<MyFrame*>(ptr);
    if( framePtr){
     wxCommandEvent ev(BELLS_PLAYED_EVTYPE);
     wxPostEvent(framePtr, ev);
    }
}


// My frame constructor
MyFrame::MyFrame( wxFrame *frame, const wxString& title, const wxPoint& pos, const wxSize& size,
        long style ) :
        wxFrame( frame, -1, title, pos, size, style ) //wxSIMPLE_BORDER | wxCLIP_CHILDREN | wxRESIZE_BORDER)
//wxCAPTION | wxSYSTEM_MENU | wxRESIZE_BORDER
{
    m_last_track_rotation_ts = 0;
    m_ulLastNEMATicktime = 0;

    m_pStatusBar = NULL;
    m_StatusBarFieldCount = g_Platform->GetStatusBarFieldCount();

    m_pMenuBar = NULL;
    g_options = NULL;
    piano_ctx_menu = NULL;

    //      Redirect the initialization timer to this frame
    InitTimer.SetOwner( this, INIT_TIMER );
    m_iInitCount = 0;
    m_initializing = false;

    //      Redirect the global heartbeat timer to this frame
    FrameTimer1.SetOwner( this, FRAME_TIMER_1 );

    //      Redirect the Tide/Current update timer to this frame
    FrameTCTimer.SetOwner( this, FRAME_TC_TIMER );

    //      Redirect the COG Averager timer to this frame
    FrameCOGTimer.SetOwner( this, FRAME_COG_TIMER );

    //      Redirect the Memory Footprint Management timer to this frame
    MemFootTimer.SetOwner( this, MEMORY_FOOTPRINT_TIMER );

    //      Direct the Toolbar Animation timer to this frame
    ToolbarAnimateTimer.SetOwner( this, TOOLBAR_ANIMATE_TIMER );
    
#ifdef __OCPN__ANDROID__
//    m_PrefTimer.SetOwner( this, ANDROID_PREF_TIMER );
//    Connect( m_PrefTimer.GetId(), wxEVT_TIMER, wxTimerEventHandler( MyFrame::OnPreferencesResultTimer ), NULL, this );
#endif

    //      Set up some assorted member variables
    m_bTimeIsSet = false;
    m_bDateIsSet = false;
    nBlinkerTick = 0;

    m_bdefer_resize = false;

    //    Clear the NMEA Filter tables
    for( int i = 0; i < MAX_COGSOG_FILTER_SECONDS; i++ ) {
        COGFilterTable[i] = NAN;
        SOGFilterTable[i] = NAN;
    }
    m_last_bGPSValid = false;

    gHdt = NAN;
    gHdm = NAN;
    gVar = NAN;
    gSog = NAN;
    gCog = NAN;

    for (int i = 0; i < MAX_COG_AVERAGE_SECONDS; i++ )
        COGTable[i] = NAN;

    m_fixtime = 0;

    m_bpersistent_quilt = false;

    m_ChartUpdatePeriod = 1;                  // set the default (1 sec.) period

//    Establish my children
    g_pMUX = new Multiplexer();

    g_pAIS = new AIS_Decoder( this );

    g_pMUX->SetAISHandler(g_pAIS);
    g_pMUX->SetGPSHandler(this);
    //  Create/connect a dynamic event handler slot
    Connect( wxEVT_OCPN_DATASTREAM, (wxObjectEventFunction) (wxEventFunction) &MyFrame::OnEvtOCPN_NMEA );

    b_autofind = false;
    
    //  Create/connect a dynamic event handler slot for OCPN_MsgEvent(s) coming from PlugIn system
    Connect( wxEVT_OCPN_MSG, (wxObjectEventFunction) (wxEventFunction) &MyFrame::OnEvtPlugInMessage );

    Connect( wxEVT_OCPN_THREADMSG, (wxObjectEventFunction) (wxEventFunction) &MyFrame::OnEvtTHREADMSG );

    //  And from the thread SENC creator
    Connect( wxEVT_OCPN_BUILDSENCTHREAD, (wxObjectEventFunction) (wxEventFunction) &MyFrame::OnSENCEvtThread );
    //        Establish the system icons for the frame.

#ifdef __WXMSW__
    SetIcon( wxICON(0) );           // this grabs the first icon in the integrated MSW resource file
#endif

#if defined(__WXGTK__) || defined(__WXQT__)
    wxIcon app_icon(opencpn);          // This comes from opencpn.xpm inclusion above
    SetIcon(app_icon);
#endif

#ifdef __WXMSW__

//    Establish the entry points in USER32.DLL for system color control

    wxDynamicLibrary dllUser32( _T("user32.dll") );

    pSetSysColors = (SetSysColors_t) dllUser32.GetSymbol( wxT("SetSysColors") );
    pGetSysColor = (GetSysColor_t) dllUser32.GetSymbol( wxT("GetSysColor") );

    SaveSystemColors();
#endif

    m_next_available_plugin_tool_id = ID_PLUGIN_BASE;

    g_sticky_chart = -1;
    g_sticky_projection = -1;
    m_BellsToPlay = 0;

    m_resizeTimer.SetOwner(this, RESIZE_TIMER);
    m_recaptureTimer.SetOwner(this, RECAPTURE_TIMER);

#ifdef __WXOSX__
    // Enable native fullscreen on macOS
    EnableFullScreenView();
#endif

}

MyFrame::~MyFrame()
{
    FrameTimer1.Stop();
    delete ChartData;
    //delete pCurrentStack;

//      Free the Route List
    wxRouteListNode *node = pRouteList->GetFirst();

    while( node ) {
        Route *pRouteDelete = node->GetData();
        delete pRouteDelete;

        node = node->GetNext();
    }
    delete pRouteList;
}

void MyFrame::OnSENCEvtThread( OCPN_BUILDSENC_ThreadEvent & event)
{
    s57chart *chart;
    switch(event.type){
       case SENC_BUILD_STARTED:
            //printf("Myframe SENC build started\n");
            break;
        case SENC_BUILD_DONE_NOERROR:
            //printf("Myframe SENC build done no error\n");
            chart = event.m_ticket->m_chart;
            if(chart){
                chart->PostInit(FULL_INIT, global_color_scheme);
               // ..For each canvas, force an S52PLIB reconfig...
                for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
                    ChartCanvas *cc = g_canvasArray.Item(i);
                        if(cc)
                            cc->ClearS52PLIBStateHash();         // Force a S52 PLIB re-configure
                }
            }

            ReloadAllVP();
            break;
        case SENC_BUILD_DONE_ERROR:
            //printf("Myframe SENC build done ERROR\n");
            break;
        default:
            break;
    }
}

void MyFrame::RebuildChartDatabase()
{
    bool b_SetInitialPoint = false;

    //   Build the initial chart dir array
    ArrayOfCDI ChartDirArray;
    pConfig->LoadChartDirArray( ChartDirArray );



        if( ChartDirArray.GetCount() ) {
//              Create and Save a new Chart Database based on the hints given in the config file

            wxString msg1(_("OpenCPN needs to update the chart database from config file entries...."));

            OCPNMessageDialog mdlg(gFrame, msg1, wxString(_("OpenCPN Info")),wxICON_INFORMATION | wxOK );
            int dlg_ret;
            dlg_ret = mdlg.ShowModal();

            delete ChartData;
            ChartData = new ChartDB( );

            wxString line( _("Rebuilding chart database from configuration file entries...") );
            /* The following 3 strings are embeded in wxProgressDialog but must be included by xgettext
             * to be localized properly. See {wxWidgets}src/generic/progdlgg.cpp:190 */
            wxString dummy1 = _("Elapsed time : ");
            wxString dummy2 = _("Estimated time : ");
            wxString dummy3 = _("Remaining time : ");
            wxGenericProgressDialog *pprog = new wxGenericProgressDialog( _("OpenCPN Chart Update"), line, 100,
                     NULL, wxPD_SMOOTH | wxPD_ELAPSED_TIME | wxPD_ESTIMATED_TIME | wxPD_REMAINING_TIME );

            ChartData->Create( ChartDirArray, pprog );
            ChartData->SaveBinary(ChartListFileName);

            delete pprog;

            //  Apply the inital Group Array structure to the chart data base
            ChartData->ApplyGroupArray( g_pGroupArray );
        }
}


// play an arbitrary number of bells by using 1 and 2 bell sounds
void MyFrame::OnBellsFinished( wxCommandEvent& event )
{
    int bells = wxMin(m_BellsToPlay, 2);
    if(bells <= 0)
        return;

    wxString soundfile = _T("sounds");
    appendOSDirSlash( &soundfile );
    soundfile += wxString( bells_sound_file_name[bells - 1], wxConvUTF8 );
    soundfile.Prepend( g_Platform->GetSharedDataDir() );
    wxLogMessage( _T("Using bells sound file: ") + soundfile );

    OcpnSound* sound = bells_sound[bells - 1];
    sound->SetFinishedCallback(onBellsFinishedCB, this);
    sound->SetCmd( g_CmdSoundString.mb_str( wxConvUTF8 ) );
    sound->Load( soundfile );
    if( !sound->IsOk() ) {
        wxLogMessage( _T("Failed to load bells sound file: ") + soundfile );
        return;
    }
    sound->Play();
    m_BellsToPlay -= bells;
}

#ifdef __WXOSX__
void MyFrame::startHelp(void)
{
    //    AHGotoPage(NULL, NULL);
    CFBundleRef const bundle = CFBundleGetMainBundle ( );
    if ( !bundle )
        return;

    CFStringRef const key = CFSTR( "CFBundleHelpBookName" );
    CFTypeRef const book =
    CFBundleGetValueForInfoDictionaryKey ( bundle, key );
    if ( !book )
        return;

    CFStringRef const help = (CFStringRef)book;

    //    _Error =
    AHGotoPage ( help, NULL, NULL );
}
#endif

void MyFrame::OnEraseBackground( wxEraseEvent& event )
{
}

void MyFrame::OnMaximize( wxMaximizeEvent& event )
{
    g_click_stop = 0;
#ifdef __WXOSX__
    event.Skip();
#endif
}

ColorScheme GetColorScheme()
{
    return global_color_scheme;
}

ColorScheme MyFrame::GetColorScheme()
{
    return global_color_scheme;
}

void MyFrame::ReloadAllVP()
{
    for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
        ChartCanvas *cc = g_canvasArray.Item(i);
        if(cc)
            cc->ReloadVP();
    }
}

void MyFrame::SetAndApplyColorScheme( ColorScheme cs )
{
    global_color_scheme = cs;

    wxString SchemeName;
    switch( cs ){
        case GLOBAL_COLOR_SCHEME_DAY:
            SchemeName = _T("DAY");
            break;
        case GLOBAL_COLOR_SCHEME_DUSK:
            SchemeName = _T("DUSK");
            break;
        case GLOBAL_COLOR_SCHEME_NIGHT:
            SchemeName = _T("NIGHT");
            break;
        default:
            SchemeName = _T("DAY");
            break;
    }

#if defined(__WXOSX__) && defined(OCPN_USE_DARKMODE)
    bool darkMode = (cs == GLOBAL_COLOR_SCHEME_DUSK || cs == GLOBAL_COLOR_SCHEME_NIGHT);

    if (wxPlatformInfo::Get().CheckOSVersion(10, 14)) {
        setAppLevelDarkMode(darkMode);
    }
    else if (wxPlatformInfo::Get().CheckOSVersion(10, 12)) {
        setWindowLevelDarkMode(MacGetTopLevelWindowRef(), darkMode);
    }
#endif

    g_pauidockart->SetMetric(wxAUI_DOCKART_GRADIENT_TYPE, wxAUI_GRADIENT_NONE);

    g_pauidockart->SetColour(wxAUI_DOCKART_BORDER_COLOUR, wxColour(0,0,0));
    g_pauidockart->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE, 1);
    g_pauidockart->SetColour(wxAUI_DOCKART_SASH_COLOUR, wxColour(0,0,0));
    g_pauidockart->SetMetric(wxAUI_DOCKART_SASH_SIZE, 0);
    g_pauidockart->SetColour(wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR, wxColour(0,0,0));
    g_pauidockart->SetColour(wxAUI_DOCKART_BACKGROUND_COLOUR, wxColour(0,0,0));

//    if( cs == GLOBAL_COLOR_SCHEME_DUSK || cs == GLOBAL_COLOR_SCHEME_NIGHT )
//    {
//        g_pauidockart->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE, 0);
//        g_pauidockart->SetColour(wxAUI_DOCKART_BACKGROUND_COLOUR, wxColour(0,0,0));
//        g_pauidockart->SetColour(wxAUI_DOCKART_BORDER_COLOUR, wxColour(0,0,0));
//    }

//      else{
//          g_pauidockart->SetMetric(wxAUI_DOCKART_GRADIENT_TYPE, g_grad_default);
//          g_pauidockart->SetColour(wxAUI_DOCKART_BORDER_COLOUR, g_border_color_default);
//          g_pauidockart->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE, g_border_size_default);
//          g_pauidockart->SetColour(wxAUI_DOCKART_SASH_COLOUR, g_sash_color_default);
//          g_pauidockart->SetMetric(wxAUI_DOCKART_SASH_SIZE, g_sash_size_default);
//          g_pauidockart->SetColour(wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR, g_caption_color_default);
//          g_pauidockart->SetColour(wxAUI_DOCKART_BACKGROUND_COLOUR, g_background_color_default);
//
//      }

    g_pauidockart->SetColour(wxAUI_DOCKART_SASH_COLOUR, wxColour(0,0,0));
    g_pauidockart->SetMetric(wxAUI_DOCKART_SASH_SIZE, 6);

    g_pauimgr->Update();

    g_StyleManager->GetCurrentStyle()->SetColorScheme( cs );

    //Search the user color table array to find the proper hash table
    Usercolortable_index = 0;
    for( unsigned int i = 0; i < UserColorTableArray->GetCount(); i++ ) {
        colTable *ct = (colTable *) UserColorTableArray->Item( i );
        if( SchemeName.IsSameAs( *ct->tableName ) ) {
            Usercolortable_index = i;
            break;
        }
    }

#ifdef USE_S57
    if( ps52plib ) ps52plib->SetPLIBColorScheme( SchemeName );
#endif

    //    Set up a pointer to the proper hash table
    pcurrent_user_color_hash = (wxColorHashMap *) UserColourHashTableArray->Item(
            Usercolortable_index );

    SetSystemColors( cs );

    // ..For each canvas...
    for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
        ChartCanvas *cc = g_canvasArray.Item(i);
        if(cc){
            cc->SetColorScheme( cs );
            cc->GetWorldBackgroundChart()->SetColorScheme( cs );
            cc->HideChartInfoWindow();
            cc->SetQuiltChartHiLiteIndex( -1 );
        }

    }


    if( pWayPointMan )
        pWayPointMan->SetColorScheme( cs );

    if( ChartData )
        ChartData->ApplyColorSchemeToCachedCharts( cs );

    SetChartThumbnail( -1 );


    if( g_options ) {
        g_options->SetColorScheme( cs );
    }

    if( console ) {
        console->SetColorScheme( cs );
    }

    if( g_pRouteMan ) {
        g_pRouteMan->SetColorScheme( cs );
    }

    if( g_pMarkInfoDialog ) {
        g_pMarkInfoDialog->SetColorScheme( cs );
    }
    
    if( pRoutePropDialog ) {
        pRoutePropDialog->SetColorScheme( cs );
    }

    //    For the AIS target query dialog, we must rebuild it to incorporate the style desired for the colorscheme selected
    if( g_pais_query_dialog_active ) {
        bool b_isshown = g_pais_query_dialog_active->IsShown();
        int n_mmsi = g_pais_query_dialog_active->GetMMSI();
        if( b_isshown ) g_pais_query_dialog_active->Show( false );              // dismiss it

        g_pais_query_dialog_active->Close();

        g_pais_query_dialog_active = new AISTargetQueryDialog();
        g_pais_query_dialog_active->Create( this, -1, _( "AIS Target Query" ),
                wxPoint( g_ais_query_dialog_x, g_ais_query_dialog_y ) );
        g_pais_query_dialog_active->SetMMSI( n_mmsi );
        g_pais_query_dialog_active->UpdateText();
        if( b_isshown ) g_pais_query_dialog_active->Show();
    }

    if( pRouteManagerDialog ) pRouteManagerDialog->SetColorScheme();

    if( g_pAISTargetList ) g_pAISTargetList->SetColorScheme();

    if( g_pObjectQueryDialog ) g_pObjectQueryDialog->SetColorScheme();

    ApplyGlobalColorSchemetoStatusBar();

    UpdateAllToolbars( cs );

    if(g_MainToolbar){
        if(g_MainToolbar->GetColorScheme() != cs){

            // capture the current toolbar collapse state
            bool btoolbarFull = g_bmasterToolbarFull;

            g_MainToolbar->SetColorScheme( cs );
            //g_MainToolbar->DestroyToolBar();
            //CreateMasterToolbar();

            if(!btoolbarFull){
                g_MainToolbar->Hide();
                RequestNewMasterToolbar();
                g_MainToolbar->SetColorScheme( cs );
                CollapseGlobalToolbar();
                g_MainToolbar->Show();
            }
            else{
                RequestNewMasterToolbar();
                g_MainToolbar->SetColorScheme( cs );
            }
        }
    }

    if( g_pi_manager ) g_pi_manager->SetColorSchemeForAllPlugIns( cs );
}

void MyFrame::ApplyGlobalColorSchemetoStatusBar( void )
{
    if( m_pStatusBar != NULL ) {
        m_pStatusBar->SetBackgroundColour(GetGlobalColor(_T("UIBDR")));    //UINFF
        m_pStatusBar->ClearBackground();

    }
}

ChartCanvas *MyFrame::GetPrimaryCanvas()
{
    if(g_canvasArray.GetCount() > 0)
        return g_canvasArray.Item(0);
    else
        return NULL;
}
void MyFrame::CancelAllMouseRoute()
{
// ..For each canvas...
    for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
        ChartCanvas *cc = g_canvasArray.Item(i);
        if(cc)
            cc->CancelMouseRoute();
    }
}

void MyFrame::NotifyChildrenResize()
{
}
    
void MyFrame::CreateCanvasLayout( bool b_useStoredSize )
{
    //  Clear the cache, and thus close all charts to avoid memory leaks
    if(ChartData)
        ChartData->PurgeCache();

    // Detach all canvases from AUI manager
    for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
        ChartCanvas *cc = g_canvasArray[i];
        if(cc){
            g_pauimgr->DetachPane(cc);
        }
    }

    //Destroy any existing canvases, except for Primary canvas
    for(unsigned int i=1 ; i < g_canvasArray.GetCount() ; i++){
        ChartCanvas *cc = g_canvasArray.Item(i);
        if(cc){
            pthumbwin = NULL;           // TODO
            cc->DestroyToolbar();
            cc->Destroy();
        }
    }

    // Canvas pointers in config array are now invalid
    for(unsigned int i=1 ; i < g_canvasConfigArray.GetCount() ; i++){
        g_canvasConfigArray.Item(i)->canvas = NULL;
    }

//    g_canvasArray.Clear();

    // Clear the canvas Array, except for Primary canvas
    for(unsigned int i=1 ; i < g_canvasArray.GetCount() ; i++){
        g_canvasArray.RemoveAt(i);
    }

    ChartCanvas *cc = NULL;
    switch(g_canvasConfig){
        default:
        case 0:                                                 // a single canvas
            if(!g_canvasArray.GetCount() || !g_canvasConfigArray.Item(0)){
                cc = new ChartCanvas( this, 0 );                         // the chart display canvas
                g_canvasArray.Add(cc);
            }
            else{
                cc = g_canvasArray[0];
            }

            // Verify that glCanvas is ready, if necessary
            if(g_bopengl){
                if(!cc->GetglCanvas())
                    cc->SetupGlCanvas();
            }

            g_canvasConfigArray.Item(0)->canvas = cc;

            cc->SetDisplaySizeMM(g_display_size_mm);

            cc->ApplyCanvasConfig(g_canvasConfigArray.Item(0));

//            cc->SetToolbarPosition(wxPoint( g_maintoolbar_x, g_maintoolbar_y ));
            cc->ConfigureChartBar();
            cc->SetColorScheme( global_color_scheme );
            cc->GetCompass()->SetScaleFactor(g_compass_scalefactor);
            cc->SetShowGPS( true );

            g_pauimgr->AddPane( cc );
            g_pauimgr->GetPane( cc ).Name( _T("ChartCanvas") );
            g_pauimgr->GetPane( cc ).Fixed();
            g_pauimgr->GetPane( cc ).CaptionVisible( false );
            g_pauimgr->GetPane( cc ).CenterPane();

            break;

        case 1:{                                                 // two canvas, horizontal
            if(!g_canvasArray.GetCount() || !g_canvasArray[0]){
                cc = new ChartCanvas( this, 0 );                         // the chart display canvas
                g_canvasArray.Add(cc);
           }
            else{
                cc = g_canvasArray[0];
           }

           // Verify that glCanvas is ready, if not already built
           if(g_bopengl){
                if(!cc->GetglCanvas())
                    cc->SetupGlCanvas();
           }

           g_canvasConfigArray.Item(0)->canvas = cc;

           cc->ApplyCanvasConfig(g_canvasConfigArray.Item(0));

           cc->SetDisplaySizeMM(g_display_size_mm);
           cc->ConfigureChartBar();
           cc->SetColorScheme( global_color_scheme );
           cc->GetCompass()->SetScaleFactor(g_compass_scalefactor);
           cc->SetShowGPS( false );

           g_pauimgr->AddPane( cc );
           g_pauimgr->GetPane( cc ).Name( _T("ChartCanvas") );
           g_pauimgr->GetPane( cc ).CaptionVisible( false ).PaneBorder(false).CloseButton(false);

           g_pauimgr->GetPane( cc ).CenterPane();

           cc = new ChartCanvas( this, 1 );                         // the chart display canvas
           g_canvasArray.Add(cc);

           //  There is not yet a config descriptor for canvas 2, so create one by copy ctor from canvas {0}.
           if(g_canvasConfigArray.GetCount() < 2){
               canvasConfig *pcc = new canvasConfig(*g_canvasConfigArray.Item(0));
               pcc->configIndex = 1;

               // Arbitrarily establish the initial size of the new canvas to be
               // half the screen width.
               pcc->canvasSize = wxSize(GetClientSize().x / 2, GetClientSize().y);
               g_canvasConfigArray.Add(pcc);
           }

           g_canvasConfigArray.Item(1)->canvas = cc;

           cc->ApplyCanvasConfig(g_canvasConfigArray.Item(1));

           cc->SetDisplaySizeMM(g_display_size_mm);
           cc->SetToolbarOrientation( g_maintoolbar_orient);
           cc->ConfigureChartBar();
           cc->SetColorScheme( global_color_scheme );
           cc->SetShowGPS( true );

           g_pauimgr->AddPane( cc );
           g_pauimgr->GetPane( cc ).Name( _T("ChartCanvas2") );
           g_pauimgr->GetPane( cc ).CaptionVisible( false ).PaneBorder(false).CloseButton(false);
           g_pauimgr->GetPane( cc ).RightDockable(true);
           g_pauimgr->GetPane( cc ).Right();


           // If switching from single canvas to 2-canvas mode dynamically,
           //  try to use the latest persisted size for the new second canvas.
           if(b_useStoredSize){
                int ccw = g_canvasConfigArray.Item(1)->canvasSize.x;
                int cch = g_canvasConfigArray.Item(1)->canvasSize.y;

                // Check for undefined size, and set a nice default size if necessary.
                if( ccw < GetClientSize().x / 10){
                    ccw = GetClientSize().x / 2;
                    cch = GetClientSize().y;
                }

                g_pauimgr->GetPane( cc ).BestSize( ccw, cch );
                cc->SetSize(ccw, cch);
            }

           break;
        }

        case 2:                                                 // two canvas, vertical

            break;
    }

    g_focusCanvas = GetPrimaryCanvas();

}






static bool b_inCloseWindow;

void MyFrame::RequestNewToolbars(bool bforcenew)
{
    if( b_inCloseWindow ) {
        return;
    }

    BuildiENCToolbar(bforcenew);
    PositionIENCToolbar();

#ifdef __OCPN__ANDROID__
    DoChartUpdate();
#endif


}

//      Update inplace the various controls with bitmaps corresponding to the current color scheme
void MyFrame::UpdateAllToolbars( ColorScheme cs )
{

    if(g_iENCToolbar)
        g_iENCToolbar->SetColorScheme( cs );


    return;
}


void MyFrame::SetAllToolbarScale()
{
    double scale_factor = g_Platform->GetToolbarScaleFactor( g_GUIScaleFactor );
    g_toolbar_scalefactor = g_Platform->GetToolbarScaleFactor( g_GUIScaleFactor );

    //  Round to the nearest "quarter", to avoid rendering artifacts
    scale_factor = wxRound( scale_factor * 4.0 )/ 4.0;

    // ..For each canvas...
    for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
        ChartCanvas *cc = g_canvasArray.Item(i);
        if(cc)
            cc->SetToolbarScaleFactor(scale_factor);
    }

}

void MyFrame::SetGPSCompassScale()
{
    g_compass_scalefactor = g_Platform->GetCompassScaleFactor( g_GUIScaleFactor );

}

ChartCanvas *MyFrame::GetCanvasUnderMouse()
{
    wxPoint screenPoint = ::wxGetMousePosition();
    canvasConfig *cc;

    switch(g_canvasConfig){
        case 1:
            cc = g_canvasConfigArray.Item(0);
            if(cc ){
                ChartCanvas *canvas = cc->canvas;
                if(canvas->GetScreenRect().Contains(/*canvas->ScreenToClient*/(screenPoint)))
                    return canvas;
            }
            cc = g_canvasConfigArray.Item(1);
            if(cc ){
                ChartCanvas *canvas = cc->canvas;
                if(canvas->GetScreenRect().Contains(/*canvas->ScreenToClient*/(screenPoint)))
                    return canvas;
            }
            break;

        default:
            cc = g_canvasConfigArray.Item(0);
            if(cc ){
                ChartCanvas *canvas = cc->canvas;
                if(canvas->GetScreenRect().Contains(canvas->ScreenToClient(screenPoint)))
                    return canvas;
            }
    }

    return NULL;    
}

int MyFrame::GetCanvasIndexUnderMouse()
{
    wxPoint screenPoint = ::wxGetMousePosition();
    canvasConfig *cc;

    switch(g_canvasConfig){
        case 1:
            cc = g_canvasConfigArray.Item(0);
            if(cc ){
                ChartCanvas *canvas = cc->canvas;
                if(canvas->GetScreenRect().Contains(/*canvas->ScreenToClient*/(screenPoint)))
                    return 0;
            }
            cc = g_canvasConfigArray.Item(1);
            if(cc ){
                ChartCanvas *canvas = cc->canvas;
                if(canvas->GetScreenRect().Contains(/*canvas->ScreenToClient*/(screenPoint)))
                    return 1;
            }
            break;

        default:
            cc = g_canvasConfigArray.Item(0);
            if(cc ){
                ChartCanvas *canvas = cc->canvas;
                if(canvas->GetScreenRect().Contains(canvas->ScreenToClient(screenPoint)))
                    return 0;
            }
    }

    return -1;    
}


bool MyFrame::DropMarker( bool atOwnShip )
{
    double lat, lon;
    if(atOwnShip){
        lat = gLat;
        lon = gLon;
    }
    else{
        ChartCanvas *canvas = GetCanvasUnderMouse();
        if(!canvas)
            return false;

        lat = canvas->m_cursor_lat;
        lon = canvas->m_cursor_lon;
    }

    RoutePoint *pWP = new RoutePoint( lat, lon, g_default_wp_icon, wxEmptyString, wxEmptyString );
    pWP->m_bIsolatedMark = true;                      // This is an isolated mark
    pSelect->AddSelectableRoutePoint( lat, lon, pWP );
    pConfig->AddNewWayPoint( pWP, -1 );    // use auto next num
    if( !pWP->IsVisibleSelectable( GetCanvasUnderMouse() ) )
        pWP->ShowScaleWarningMessage(GetCanvasUnderMouse());
    if( pRouteManagerDialog && pRouteManagerDialog->IsShown() )
        pRouteManagerDialog->UpdateWptListCtrl();
//     undo->BeforeUndoableAction( Undo_CreateWaypoint, pWP, Undo_HasParent, NULL );
//     undo->AfterUndoableAction( NULL );

    InvalidateAllGL();
    RefreshAllCanvas( false );

    return true;
}

void MyFrame::SwitchKBFocus( ChartCanvas *pCanvas )
{
    if(g_canvasConfig != 0){             // multi-canvas?
        canvasConfig *cc;
        int nTarget = -1;
        int nTargetGTK = -1;
        ChartCanvas *target;
        wxWindow *source = FindFocus();
        ChartCanvas *test = wxDynamicCast(source, ChartCanvas);
        if(!test)
            return;

        // On linux(GTK), the TAB key causes a loss of focus immediately
        //  So the logic needs a switch    
        switch(g_canvasConfig){
            case 1:
                cc = g_canvasConfigArray.Item(0);
                if(cc ){
                    ChartCanvas *canvas = cc->canvas;
                    if(canvas && (canvas == test)){
                        nTarget = 1;
                        nTargetGTK = 0;
                    }
                }
                cc = g_canvasConfigArray.Item(1);
                if(cc ){
                    ChartCanvas *canvas = cc->canvas;
                    if(canvas && (canvas == test)){
                        nTarget = 0;
                        nTargetGTK = 1;
                    }
                }

                if(nTarget >= 0){
                    //printf("sw %d\n", nTarget);
                    int nfinalTarget = nTarget;
#ifdef __WXGTK__
                    nfinalTarget = nTargetGTK;
#endif                    
                    target = g_canvasConfigArray.Item(nfinalTarget)->canvas;
                    if(target){
                        wxWindow *win = wxDynamicCast(target, wxWindow);
                        win->SetFocus();
                        target->Refresh(true);
                    }
                }
                break;

            default:
                break;
                
        }
    }
}


void MyFrame::FastClose(){

    FrameTimer1.Stop();
    quitflag++;                             // signal to the timer loop
    FrameTimer1.Start(1);                    // real quick now...
}

// Intercept menu commands
void MyFrame::OnExit( wxCommandEvent& event )
{
    quitflag++;                             // signal to the timer loop

}

void MyFrame::OnCloseWindow( wxCloseEvent& event )
{
    //    It is possible that double clicks on application exit box could cause re-entrance here
    //    Not good, and don't need it anyway, so simply return.
    if( b_inCloseWindow ) {
//            wxLogMessage(_T("opencpn::MyFrame re-entering OnCloseWindow"));
        return;
    }

    // The Options dialog, and other deferred init items, are not fully initialized.
    // Best to just cancel the close request. 
    // This is probably only reachable on slow hardware...
    if(!g_bDeferredInitDone)
        return;


    if(g_options){
        delete g_options;
        g_options = NULL;
        g_pOptions = NULL;
    }

    //  If the multithread chart compressor engine is running, cancel the close command
    if( b_inCompressAllCharts ) {
        return;
    }

    if( bDBUpdateInProgress )
        return;

    b_inCloseWindow = true;

    ::wxSetCursor( wxCURSOR_WAIT );

    // If we happen to have the measure tool open on Ctrl-Q quit
    // ..For each canvas...
    for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
        ChartCanvas *cc = g_canvasArray.Item(i);
        if(cc && cc->IsMeasureActive()){
            cc->CancelMeasureRoute();
        }
    }


    // We save perspective before closing to restore position next time
    // Pane is not closed so the child is not notified (OnPaneClose)
    if( g_pAISTargetList ) {
        wxAuiPaneInfo &pane = g_pauimgr->GetPane( g_pAISTargetList );
        g_AisTargetList_perspective = g_pauimgr->SavePaneInfo( pane );
        g_pauimgr->DetachPane( g_pAISTargetList );
    }


    // Make sure the saved perspective minimum canvas sizes are essentially undefined
//     for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
//         ChartCanvas *cc = g_canvasArray.Item(i);
//         if(cc)
//             g_pauimgr->GetPane( cc ).MinSize(10,10);
//     }

    pConfig->SetPath( _T ( "/AUI" ) );
    pConfig->Write( _T ( "AUIPerspective" ), g_pauimgr->SavePerspective() );

    g_bquiting = true;

#ifdef ocpnUSE_GL
    // cancel compression jobs
    if(g_bopengl){
        if(g_glTextureManager){
            g_glTextureManager->PurgeJobList();

            if(g_glTextureManager->GetRunningJobCount())
                g_bcompression_wait = true;
        }
    }
#endif

    SetCursor( wxCURSOR_WAIT );

    RefreshAllCanvas( true );

    //  This yield is not necessary, since the Update() proceeds syncronously...
    //wxYield();

    //   Save the saved Screen Brightness
    RestoreScreenBrightness();

 // Persist the toolbar locations
 if( g_MainToolbar ) {
     g_MainToolbar->GetFrameRelativePosition(&g_maintoolbar_x, &g_maintoolbar_y);
    }

   if(g_iENCToolbar){
       wxPoint locn = g_iENCToolbar->GetPosition();
       wxPoint tbp_incanvas = GetPrimaryCanvas()->ScreenToClient( locn );
       g_iENCToolbarPosY = tbp_incanvas.y;
       g_iENCToolbarPosX = tbp_incanvas.x;
   }
   
   g_bframemax = IsMaximized();

    FrameTimer1.Stop();
    FrameCOGTimer.Stop();
    TrackOff();

    /*
     Automatically drop an anchorage waypoint, if enabled
     On following conditions:
     1.  In "Cruising" mode, meaning that speed has at some point exceeded 3.0 kts.
     2.  Current speed is less than 0.5 kts.
     3.  Opencpn has been up at least 30 minutes
     4.  And, of course, opencpn is going down now.
     5.  And if there is no anchor watch set on "anchor..." icon mark           // pjotrc 2010.02.15
     */
    if( g_bAutoAnchorMark ) {
        bool watching_anchor = false;                                           // pjotrc 2010.02.15
        if( pAnchorWatchPoint1 )                                               // pjotrc 2010.02.15
        watching_anchor = ( pAnchorWatchPoint1->GetIconName().StartsWith( _T("anchor") ) ); // pjotrc 2010.02.15
        if( pAnchorWatchPoint2 )                                               // pjotrc 2010.02.15
        watching_anchor |= ( pAnchorWatchPoint2->GetIconName().StartsWith( _T("anchor") ) ); // pjotrc 2010.02.15

        wxDateTime now = wxDateTime::Now();
        wxTimeSpan uptime = now.Subtract( g_start_time );

        if( !watching_anchor && ( g_bCruising ) && ( gSog < 0.5 )
                && ( uptime.IsLongerThan( wxTimeSpan( 0, 30, 0, 0 ) ) ) )     // pjotrc 2010.02.15
                {
            //    First, delete any single anchorage waypoint closer than 0.25 NM from this point
            //    This will prevent clutter and database congestion....

            wxRoutePointListNode *node = pWayPointMan->GetWaypointList()->GetFirst();
            while( node ) {
                RoutePoint *pr = node->GetData();
                if( pr->GetName().StartsWith( _T("Anchorage") ) ) {
                    double a = gLat - pr->m_lat;
                    double b = gLon - pr->m_lon;
                    double l = sqrt( ( a * a ) + ( b * b ) );

                    // caveat: this is accurate only on the Equator
                    if( ( l * 60. * 1852. ) < ( .25 * 1852. ) ) {
                        pConfig->DeleteWayPoint( pr );
                        pSelect->DeleteSelectablePoint( pr, SELTYPE_ROUTEPOINT );
                        delete pr;
                        break;
                    }
                }

                node = node->GetNext();
            }

            wxString name = now.Format();
            name.Prepend( _("Anchorage created ") );
            RoutePoint *pWP = new RoutePoint( gLat, gLon, _T("anchorage"), name, wxEmptyString );
            pWP->m_bShowName = false;
            pWP->m_bIsolatedMark = true;

            pConfig->AddNewWayPoint( pWP, -1 );       // use auto next num
        }
    }

    // Provisionally save all settings before deactivating plugins
    pConfig->UpdateSettings();

    //    Deactivate the PlugIns
    if( g_pi_manager ) {
        g_pi_manager->DeactivateAllPlugIns();
    }

    wxLogMessage( _T("opencpn::MyFrame exiting cleanly.") );

    quitflag++;

    pConfig->UpdateNavObj();

//    pConfig->m_pNavObjectChangesSet->Clear();
    delete pConfig->m_pNavObjectChangesSet;

    //Remove any leftover Routes and Waypoints from config file as they were saved to navobj before
    pConfig->DeleteGroup( _T ( "/Routes" ) );
    pConfig->DeleteGroup( _T ( "/Marks" ) );
    pConfig->Flush();


    delete g_printData;
    delete g_pageSetupData;

    if( g_pAboutDlg ) g_pAboutDlg->Destroy();

//      Explicitely Close some children, especially the ones with event handlers
//      or that call GUI methods

#ifdef USE_S57
    if( g_pCM93OffsetDialog ) {
        g_pCM93OffsetDialog->Destroy();
        g_pCM93OffsetDialog = NULL;
    }
#endif

    // .. for each canvas...
    // ..For each canvas...
    for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
        ChartCanvas *cc = g_canvasArray.Item(i);
        if(cc)
            cc->DestroyToolbar();
    }

    if(g_MainToolbar)
        g_MainToolbar->Destroy();
    g_MainToolbar = NULL;


    if(g_iENCToolbar){
        wxPoint locn = g_iENCToolbar->GetPosition();
        g_iENCToolbarPosY = locn.y;
        g_iENCToolbarPosX = locn.x;
        g_iENCToolbar->Destroy();
    }
    
    if( g_pAISTargetList ) {
        g_pAISTargetList->Disconnect_decoder();
        g_pAISTargetList->Destroy();
    }


#ifndef __WXQT__
    SetStatusBar( NULL );
#endif

    if(RouteManagerDialog::getInstanceFlag()){
        if( pRouteManagerDialog ) {
            pRouteManagerDialog->Destroy();
            pRouteManagerDialog = NULL;
        }
    }

    //  Clear the cache, and thus close all charts to avoid memory leaks
    if(ChartData)
        ChartData->PurgeCache();

    // pthumbwin is a canvas child 
    pthumbwin = NULL;

    // Finally ready to destroy the canvases
    g_focusCanvas = NULL;

    // ..For each canvas...
    for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
        ChartCanvas *cc = g_canvasArray.Item(i);
        if(cc)
            cc->Destroy();
    }

    g_canvasArray.Clear();


    g_pauimgr->UnInit();
    delete g_pauimgr;
    g_pauimgr = NULL;

    //    Unload the PlugIns
    //      Note that we are waiting until after the canvas is destroyed,
    //      since some PlugIns may have created children of canvas.
    //      Such a PlugIn must stay intact for the canvas dtor to call DestoryChildren()

    if(ChartData)
        ChartData->PurgeCachePlugins();

    if( g_pi_manager ) {
        g_pi_manager->UnLoadAllPlugIns();
        delete g_pi_manager;
        g_pi_manager = NULL;
    }

    delete pConfig;             // All done
    pConfig = NULL;

    if( g_pAIS ) {
        if(g_pMUX)
            g_pMUX->SetAISHandler(NULL);
        delete g_pAIS;
        g_pAIS = NULL;
    }

    delete g_pMUX;
    g_pMUX = NULL;


    //  Clear some global arrays, lists, and hash maps...
    for ( size_t i = 0; i < g_pConnectionParams->Count(); i++ )
    {
        ConnectionParams *cp = g_pConnectionParams->Item(i);
        delete cp;
    }
    delete g_pConnectionParams;

    if(pLayerList){
        LayerList::iterator it;
        while(pLayerList->GetCount()){
            Layer *lay = pLayerList->GetFirst()->GetData();
            delete lay;                 // automatically removes the layer from list, see Layer dtor
        }
    }

    MsgPriorityHash::iterator it;
    for( it = NMEA_Msg_Hash.begin(); it != NMEA_Msg_Hash.end(); ++it ){
        NMEA_Msg_Container *pc = it->second;
        delete pc;
    }
    NMEA_Msg_Hash.clear();


    NMEALogWindow::Shutdown();

    g_MainToolbar = NULL;
    g_bTempShowMenuBar = false;


    #define THREAD_WAIT_SECONDS  5
#ifdef ocpnUSE_GL
    // The last thing we do is finish the compression threads.
    // This way the main window is already invisible and to the user
    // it appears to have finished rather than hanging for several seconds
    // while the compression threads exit
    if(g_bopengl && g_glTextureManager && g_glTextureManager->GetRunningJobCount()){
        g_glTextureManager->ClearAllRasterTextures();

        wxLogMessage(_T("Starting compressor pool drain"));
        wxDateTime now = wxDateTime::Now();
        time_t stall = now.GetTicks();
        time_t end = stall + THREAD_WAIT_SECONDS;

        int n_comploop = 0;
        while(stall < end ) {
            wxDateTime later = wxDateTime::Now();
            stall = later.GetTicks();

            wxString msg;
            msg.Printf(_T("Time: %d  Job Count: %d"), n_comploop, g_glTextureManager->GetRunningJobCount());
            wxLogMessage(msg);
            if(!g_glTextureManager->GetRunningJobCount())
                break;
            wxYield();
            wxSleep(1);
        }

        wxString fmsg;
        fmsg.Printf(_T("Finished compressor pool drain..Time: %d  Job Count: %d"),
                    n_comploop, g_glTextureManager->GetRunningJobCount());
        wxLogMessage(fmsg);
    }
    delete g_glTextureManager;
#endif

    this->Destroy();
    gFrame = NULL;

    wxLogMessage(_T("gFrame destroyed."));

#ifdef __OCPN__ANDROID__
    qDebug() << "Calling OnExit()";
    wxTheApp->OnExit();
#endif

}

void MyFrame::OnMove( wxMoveEvent& event )
{
    // ..For each canvas...
    for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
        ChartCanvas *cc = g_canvasArray.Item(i);
        if(cc)
            cc->SetMUIBarPosition();
    }

    UpdateGPSCompassStatusBoxes( );

    if( console && console->IsShown() )
        PositionConsole();

    //  If global toolbar is shown, reposition it...
    if( g_MainToolbar){
        g_MainToolbar->RestoreRelativePosition(  g_maintoolbar_x, g_maintoolbar_y );
        g_MainToolbar->Realize();
    }

    PositionIENCToolbar();

//    Somehow, this method does not work right on Windows....
//      g_nframewin_posx = event.GetPosition().x;
//      g_nframewin_posy = event.GetPosition().y;

    g_nframewin_posx = GetPosition().x;
    g_nframewin_posy = GetPosition().y;
}

void MyFrame::ProcessCanvasResize( void )
{
    UpdateGPSCompassStatusBoxes( true );

    if( console && console->IsShown() )
        PositionConsole();

    PositionIENCToolbar();

    TriggerRecaptureTimer();
}

void MyFrame::TriggerRecaptureTimer()
{
    m_recaptureTimer.Start(1000, wxTIMER_ONE_SHOT);     //One second seems enough, on average
}

void MyFrame::OnRecaptureTimer(wxTimerEvent &event)
{
    Raise();
}



void MyFrame::SetCanvasSizes( wxSize frameSize )
{
    if(!g_canvasArray.GetCount())
        return;
#if 0
    int cccw = frameSize.x;
    int ccch = frameSize.y;
#endif
    // .. for each canvas...
    switch( g_canvasConfig){
        default:
        case 0:
#if 0
            cc = g_canvasArray.Item(0);
            if( cc ) {
                cc->GetSize( &cur_width, &cur_height );
                if( ( cur_width != cccw ) || ( cur_height != ccch ) ) {
                    if( g_pauimgr->GetPane( cc ).IsOk() )
                        g_pauimgr->GetPane( cc ).BestSize( cccw, ccch );
                    else
                        cc->SetSize( 0, 0, cccw, ccch );
                }
            }
#endif
            break;


        case 1:
#if 0
            cc = g_canvasArray.Item(1);
            if( cc ) {
               int ccw = g_canvasConfigArray.Item(1)->canvasSize.x;
               int cch = g_canvasConfigArray.Item(1)->canvasSize.y;

               ccw = wxMin(ccw, cccw * 8 / 10);
               ccw = wxMax(ccw, cccw * 2 / 10);
               if(cccw < 100)
                   ccw = 20;

               g_canvasConfigArray.Item(1)->canvasSize = wxSize(ccw, cch);
//               g_pauimgr->GetPane(cc).MinSize(cccw * 2 / 10, ccch);


#if 1 //ndef __WXMSW__
               //wxAUI hack: This is needed to explicietly set a docked pane size
               //Set MinSize to desired value, then call wxAuiPaneInfo::Fixed() to apply it
                g_pauimgr->GetPane(cc).MinSize(ccw, cch);
                g_pauimgr->GetPane(cc).Fixed();
                g_pauimgr->Update();

                //now make resizable again
                g_pauimgr->GetPane(cc).Resizable();
                ///g_pauimgr->GetPane(cc).MinSize(cccw * 2 / 10, ccch);
                //g_pauimgr->Update();  //Deferred
                //g_pauimgr->GetPane( cc ).BestSize( ccw, cch );
#endif
            }
#endif

            break;

    }

}

void MyFrame::OnIconize( wxIconizeEvent& event )
{
#ifdef __WXOSX__
    if(g_MainToolbar) {
        g_MainToolbar->Show(!event.IsIconized());
    }
    if(g_iENCToolbar) {
        g_iENCToolbar->Show(!event.IsIconized());
    }
    for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++) {
        ChartCanvas *cc = g_canvasArray.Item(i);
        if(cc && cc->GetMUIBar()) {
            cc->GetMUIBar()->Show(!event.IsIconized());
        }
    }
#endif
}

void MyFrame::OnSize( wxSizeEvent& event )
{
    ODoSetSize();
}

void MyFrame::ODoSetSize( void )
{
    int x, y;
    GetClientSize( &x, &y );
//      Resize the children

        if( m_pStatusBar != NULL ) {
            m_StatusBarFieldCount = g_Platform->GetStatusBarFieldCount();
            int currentCount = m_pStatusBar->GetFieldsCount();
            if(currentCount != m_StatusBarFieldCount){
                if( (currentCount > 0) && (currentCount < 7)){

                    // reset the widths very small to avoid auto-resizing of the frame
                    // The sizes will be reset later in this method
                    int widths[] = { 2,2,2,2,2,2 };
                    m_pStatusBar->SetStatusWidths( currentCount, widths );
                }

                m_pStatusBar->SetFieldsCount(m_StatusBarFieldCount);
            }

            if(m_StatusBarFieldCount){

                //  If the status bar layout is "complex", meaning more than two columns,
                //  then use custom crafted relative widths for the fields.
                //  Otherwise, just split the frame client width into equal spaces

                if(m_StatusBarFieldCount > 2){
                    int widths[] = { -6, -5, -5, -6, -4 };
                    m_pStatusBar->SetStatusWidths( m_StatusBarFieldCount, widths );
                }
                else if(m_StatusBarFieldCount == 2){
                    int cwidth = x * 90 / 100;
                    int widths[] = { 100, 100 };
                    widths[0] = cwidth * 6.4 / 10.0;
                    widths[1] = cwidth * 3.6 /  10.0;
                    m_pStatusBar->SetStatusWidths( m_StatusBarFieldCount, widths );
                }
                else{
                    int widths[] = { 100, 100 };
                    widths[0] = x * 90 / 100 ;
                    m_pStatusBar->SetStatusWidths( m_StatusBarFieldCount, widths );
                }

                int styles[] = { wxSB_FLAT, wxSB_FLAT, wxSB_FLAT, wxSB_FLAT, wxSB_FLAT, wxSB_FLAT };
                m_pStatusBar->SetStatusStyles( m_StatusBarFieldCount, styles );

                wxString sogcog( _T("SOG --- ") + getUsrSpeedUnit() + + _T("     ") + _T(" COG ---\u00B0") );
                m_pStatusBar->SetStatusText( sogcog, STAT_FIELD_SOGCOG );
                                    
            }
        }



    if( m_pStatusBar ) {
        //  Maybe resize the font so the text fits in the boxes

        wxRect stat_box;
        m_pStatusBar->GetFieldRect( 0, stat_box );
        // maximum size is 1/28 of the box width, or the box height - whicever is less
        int max_font_size = wxMin( (stat_box.width / 28), (stat_box.height) );

        wxFont sys_font = *wxNORMAL_FONT;
        int try_font_size = sys_font.GetPointSize();

#ifdef __WXOSX__
        int min_font_size = 10; // much less than 10pt is unreadably small on OS X
        try_font_size += 1;     // default to 1pt larger than system UI font
#else
        int min_font_size = 7;  // on Win/Linux the text does not shrink quite so fast
        try_font_size += 2;     // default to 2pt larger than system UI font
#endif

        // get the user's preferred font, or if none set then the system default with the size overridden
        wxFont* statusBarFont = FontMgr::Get().GetFont( _("StatusBar"), try_font_size );
        int font_size = statusBarFont->GetPointSize();

        font_size = wxMin( font_size, max_font_size );  // maximum to fit in the statusbar boxes
        font_size = wxMax( font_size, min_font_size );  // minimum to stop it being unreadable

#ifdef __OCPN__ANDROID__
        font_size = statusBarFont->GetPointSize();
#endif


        wxFont *pstat_font = FontMgr::Get().FindOrCreateFont(font_size, statusBarFont->GetFamily(),
            statusBarFont->GetStyle(), statusBarFont->GetWeight(), false, statusBarFont->GetFaceName());

        int min_height = stat_box.height;

        m_pStatusBar->SetFont( *pstat_font );
        m_pStatusBar->SetForegroundColour(FontMgr::Get().GetFontColor(_("StatusBar")));
#ifdef __OCPN__ANDROID__
        min_height = ( pstat_font->GetPointSize() * getAndroidDisplayDensity() ) + 10;
        m_pStatusBar->SetMinHeight( min_height );
//        qDebug() <<"StatusBar min height:" << min_height << "StatusBar font points:" << pstat_font->GetPointSize();
#endif
//        wxString msg;
//        msg.Printf(_T("StatusBar min height: %d    StatusBar font points: %d"), min_height, pstat_font->GetPointSize());
//        wxLogMessage(msg);

    }

    SetCanvasSizes( GetClientSize() );

    UpdateGPSCompassStatusBoxes( true );

    if( console )
        PositionConsole();

    // .. for each canvas...
    for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
        ChartCanvas *cc = g_canvasArray.Item(i);
        if(cc)
            cc->FormatPianoKeys();
    }

    //  If global toolbar is shown, reposition it...
    if( g_MainToolbar){
        bool bShow = g_MainToolbar->IsShown();
        wxSize szBefore = g_MainToolbar->GetSize();
#ifdef __WXGTK__
        // For large vertical size changes on some platforms, it is necessary to hide the toolbar
        // in order to correctly set its rounded-rectangle shape
        // It will be shown again before exit of this method.
        double deltay = g_nframewin_y - GetSize().y;
        if((fabs(deltay) > (g_Platform->getDisplaySize().y / 5)))
            g_MainToolbar->Hide();
#endif
        g_MainToolbar->RestoreRelativePosition(  g_maintoolbar_x, g_maintoolbar_y );
        //g_MainToolbar->SetGeometry(false, wxRect());
        g_MainToolbar->Realize();
        if(szBefore != g_MainToolbar->GetSize())
            g_MainToolbar->Refresh(true);
        g_MainToolbar->Show( bShow);
    }

//  Update the stored window size
    GetSize( &x, &y );
    g_nframewin_x = x;
    g_nframewin_y = y;

//  Inform the PlugIns
    if( g_pi_manager ) g_pi_manager->SendResizeEventToAllPlugIns( x, y );

//  Force redraw if in lookahead mode
    // TODO is this all right?
//     if( g_bLookAhead ) {
//         DoCOGSet();
//         DoChartUpdate();
//     }

    if( pthumbwin )
        pthumbwin->SetMaxSize( GetClientSize() );

    //  Reset the options dialog size logic
    options_lastWindowSize = wxSize(0,0);
    options_lastWindowPos = wxPoint(0,0);

    if(g_pauimgr)
        g_pauimgr->Update();

}

void MyFrame::PositionConsole( void )
{
    if( NULL == GetPrimaryCanvas() ) return;
    //    Reposition console based on its size and chartcanvas size
    int ccx, ccy, ccsx, ccsy, consx, consy;
    ChartCanvas *consoleHost = GetPrimaryCanvas();
    if(g_canvasConfig > 0)
        consoleHost = g_canvasArray[1];

    if(consoleHost){
        consoleHost->GetSize( &ccsx, &ccsy );
        consoleHost->GetPosition( &ccx, &ccy );
    }
    else{
        GetPrimaryCanvas()->GetSize( &ccsx, &ccsy );
        GetPrimaryCanvas()->GetPosition( &ccx, &ccy );
    }

    console->GetSize( &consx, &consy );

    int yOffset = 60;
//  TODO    if(g_Compass){
//         if(g_Compass->GetRect().y < 100)        // Compass is is normal upper right position.                
//             yOffset = g_Compass->GetRect().y + g_Compass->GetRect().height + 45;
//     }

    wxPoint screen_pos = ClientToScreen( wxPoint( ccx + ccsx - consx - 2, ccy + yOffset ) );
    console->Move( screen_pos );
}

void MyFrame::UpdateAllFonts()
{
    if( console ) {
        console->UpdateFonts();
        //    Reposition console
        PositionConsole();
    }

    //  Close and destroy any persistent dialogs, so that new fonts will be utilized
    DestroyPersistentDialogs();

    if( pWayPointMan ) pWayPointMan->ClearRoutePointFonts();

    RefreshAllCanvas();
}

void MyFrame::DestroyPersistentDialogs()
{
    if( g_pais_query_dialog_active ) {
        g_pais_query_dialog_active->Hide();
        g_pais_query_dialog_active->Destroy();
        g_pais_query_dialog_active = NULL;
    }

    if( pRoutePropDialog ) {  // NEU???:     if( RoutePropDlgImpl::getInstanceFlag() && pRoutePropDialog ) {
        pRoutePropDialog->Hide();
        pRoutePropDialog->Destroy();
        pRoutePropDialog = NULL;
    }

    if( pTrackPropDialog ) {
        pTrackPropDialog->Hide();
        pTrackPropDialog->Destroy();
        pTrackPropDialog = NULL;
    }

    if( g_pMarkInfoDialog ) {
        g_pMarkInfoDialog->Hide();
        g_pMarkInfoDialog->Destroy();
        g_pMarkInfoDialog = NULL;
    }

    if( g_pObjectQueryDialog ) {
        g_pObjectQueryDialog->Hide();
        g_pObjectQueryDialog->Destroy();
        g_pObjectQueryDialog = NULL;
    }

}


void MyFrame::RefreshGroupIndices( void )
{
    // ..For each canvas...
    for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
        ChartCanvas *cc = g_canvasArray.Item(i);
        if(cc)
            cc->canvasRefreshGroupIndex();
    }
}

void MyFrame::OnToolLeftClick( wxCommandEvent& event )
{
    if(g_MainToolbar)
        g_MainToolbar->HideTooltip();

    switch( event.GetId() ){
        case ID_MENU_SCALE_OUT:
            DoStackDelta( GetPrimaryCanvas(), 1 );
            DoChartUpdate();
            break;

        case ID_MENU_SCALE_IN:
            DoStackDelta( GetPrimaryCanvas(), -1 );
            DoChartUpdate();
            break;

        case ID_MENU_ZOOM_IN:{
            if(GetFocusCanvas()){
                GetFocusCanvas()->ZoomCanvas( g_plus_minus_zoom_factor, false );
            }
            break;
        }

        case ID_MENU_ZOOM_OUT:{
            if(GetFocusCanvas()){
                GetFocusCanvas()->ZoomCanvas( 1.0 / g_plus_minus_zoom_factor, false );
            }
            break;
        }

        case ID_MENU_ROUTE_NEW: {
            if(GetFocusCanvas()){
                if( 0 == GetFocusCanvas()->m_routeState ){
                    GetFocusCanvas()->StartRoute();
                }
                else {
                    GetFocusCanvas()->FinishRoute();
                }
            }
            break;
        }

        case ID_MENU_TOOL_MEASURE: {
            GetPrimaryCanvas()->StartMeasureRoute();
            break;
        }

        case ID_MENU_MARK_BOAT: {
            DropMarker(true);
            break;
        }

        case ID_MENU_MARK_CURSOR: {
            DropMarker(false);
            break;
        }

        case ID_MENU_NAV_FOLLOW:{
            if(gFrame->GetPrimaryCanvas())
                gFrame->GetPrimaryCanvas()->TogglebFollow();
            break;
        }

        case ID_MENU_CHART_OUTLINES: {
            ToggleChartOutlines(GetFocusCanvas());
            break;
        }

        case ID_MENU_CHART_QUILTING: {
            ToggleQuiltMode(GetFocusCanvas());
            break;
        }

        case ID_MENU_UI_CHARTBAR: {
            ToggleChartBar(GetFocusCanvas());
            break;
        }

#ifdef USE_S57
        case ID_MENU_ENC_TEXT:
        case ID_ENC_TEXT: {
            ToggleENCText(GetFocusCanvas());
            break;
        }
        case ID_MENU_ENC_LIGHTS: {
            ToggleLights(GetFocusCanvas());
            break;
        }
        case ID_MENU_ENC_SOUNDINGS: {
            ToggleSoundings(GetFocusCanvas());
            break;
        }
        case ID_MENU_ENC_ANCHOR: {
            ToggleAnchor( GetFocusCanvas() );
            break;
        }
        case ID_MENU_ENC_DATA_QUALITY: {
            ToggleDataQuality( GetFocusCanvas() );
            break;
        }
#endif

        case ID_MENU_SHOW_NAVOBJECTS : {
            ToggleNavobjects( GetFocusCanvas() );
            break;
        }

        case ID_MENU_AIS_TARGETS: {
            ToggleAISDisplay( GetFocusCanvas() );
            break;
        }
         case ID_MENU_AIS_MOORED_TARGETS: {
            g_bHideMoored = !g_bHideMoored;
            break;
        }
         case ID_MENU_AIS_SCALED_TARGETS: {
            ToggleAISMinimizeTargets( GetFocusCanvas() );
            break;
        }

        case ID_MENU_AIS_TARGETLIST: {
            if ( GetPrimaryCanvas() ) GetPrimaryCanvas()->ShowAISTargetList();
            break;
        }

        case ID_MENU_AIS_TRACKS: {
            g_bAISShowTracks = !g_bAISShowTracks;
            SetMenubarItemState(ID_MENU_AIS_TRACKS, g_bAISShowTracks);
            break;
        }

        case ID_MENU_AIS_CPADIALOG: {
            g_bAIS_CPA_Alert = !g_bAIS_CPA_Alert;
            SetMenubarItemState(ID_MENU_AIS_CPADIALOG, g_bAIS_CPA_Alert);
            break;
        }

        case ID_MENU_AIS_CPASOUND: {
            g_bAIS_CPA_Alert_Audio = !g_bAIS_CPA_Alert_Audio;
            SetMenubarItemState(ID_MENU_AIS_CPASOUND, g_bAIS_CPA_Alert_Audio);
            break;
        }

        case wxID_PREFERENCES:
        case ID_SETTINGS: {
            DoSettings();
            break;
        }


        case ID_MENU_SETTINGS_BASIC:
        {
 #ifdef __OCPN__ANDROID__
            // LoadS57();
            DoAndroidPreferences();
 #else
            DoSettings();
 #endif
            break;
        }

        case ID_MENU_UI_FULLSCREEN: {
            ToggleFullScreen();
            break;
        }

        case ID_MENU_SHOW_CURRENTS:
        {
            GetFocusCanvas()->ShowCurrents( !GetFocusCanvas()->GetbShowCurrent() );
            GetFocusCanvas()->ReloadVP();
            GetFocusCanvas()->Refresh( false );
            break;

        }

        case ID_MENU_SHOW_TIDES:
        {
            GetFocusCanvas()->ShowTides( !GetFocusCanvas()->GetbShowTide() );
            GetFocusCanvas()->ReloadVP();
            GetFocusCanvas()->Refresh( false );
            break;

        }

        case wxID_ABOUT:
        case ID_ABOUT: {
            if( !g_pAboutDlg ) {
                g_pAboutDlg = new AboutFrameImpl( this );
                //g_pAboutDlg->();
            } else {
                g_pAboutDlg->SetFocus();
            }
            g_pAboutDlg->Show();

            break;
        }

        case wxID_HELP: {

 #ifdef __WXOSX__
             startHelp();
 #else
             g_Platform->LaunchLocalHelp();
 #endif
            break;
        }

        case ID_PRINT: {
            DoPrint();
            break;
        }

        case ID_MENU_UI_COLSCHEME:
        case ID_COLSCHEME: {
            ToggleColorScheme();
            break;
        }

        case ID_TBEXIT: {
            Close();
            break;
        }

        case ID_MENU_OQUIT: {
            Close();
            break;
        }

        case ID_MENU_ROUTE_MANAGER:
        case ID_ROUTEMANAGER: {

            pRouteManagerDialog = RouteManagerDialog::getInstance( this ); // There is one global instance of the Dialog

            if( pRouteManagerDialog->IsShown() )
                pRouteManagerDialog->Hide();
            else {
                pRouteManagerDialog->UpdateRouteListCtrl();
                pRouteManagerDialog->UpdateTrkListCtrl();
                pRouteManagerDialog->UpdateWptListCtrl();
                pRouteManagerDialog->UpdateLayListCtrl();

                pRouteManagerDialog->Show();

            //    Required if RMDialog is not STAY_ON_TOP
#ifdef __WXOSX__
                pRouteManagerDialog->Centre();
                pRouteManagerDialog->Raise();
#endif
            }
            break;
        }

        case ID_MENU_NAV_TRACK:
        case ID_TRACK: {
            if( !g_bTrackActive ) {
                TrackOn();
                g_bTrackCarryOver = true;
            } else {
                TrackOff( true );
                g_bTrackCarryOver = false;
                RefreshAllCanvas( true );
            }
            break;
        }

        case ID_MENU_CHART_NORTHUP:{
            SetUpMode(GetPrimaryCanvas(), NORTH_UP_MODE);
            break;
        }
        case ID_MENU_CHART_COGUP:{
                SetUpMode(GetPrimaryCanvas(), COURSE_UP_MODE);
                break;
            }
        case ID_MENU_CHART_HEADUP:{
                SetUpMode(GetPrimaryCanvas(), HEAD_UP_MODE);
            break;
        }

        case ID_MENU_MARK_MOB:
        case ID_MOB: {
            ActivateMOB();
            break;
        }


        case ID_MASTERTOGGLE:{
            if(g_MainToolbar){
                wxString tip = _("Show Toolbar");
                if(!g_bmasterToolbarFull)
                    tip = _("Hide Toolbar");
                if( g_MainToolbar->GetToolbar() )
                    g_MainToolbar->GetToolbar()->SetToolShortHelp( ID_MASTERTOGGLE, tip );

                g_bmasterToolbarFull = !g_bmasterToolbarFull;

#ifdef __WXOSX__
                if(g_bmasterToolbarFull)
                    m_nMasterToolCountShown = g_MainToolbar->GetToolCount() - 1;        //TODO disable animation on OSX. Maybe use fade effect?
                else
                    m_nMasterToolCountShown = 2;
#else
                m_nMasterToolCountShown = g_MainToolbar->GetToolShowCount();        // Current state
#endif
                    ToolbarAnimateTimer.Start( 10, wxTIMER_ONE_SHOT );
                }
                break;
            }

        //  Various command events coming from (usually) other threads,
        //  used to control OCPN modes in a thread-safe way.

        case ID_CMD_SELECT_CHART_TYPE:{
            selectChartDisplay( event.GetExtraLong(), -1);
            break;
        }

        case ID_CMD_SELECT_CHART_FAMILY:{
            selectChartDisplay( -1, event.GetExtraLong());
            break;
        }

        case ID_CMD_APPLY_SETTINGS:{
            applySettingsString(event.GetString());
            break;
        }

        case ID_CMD_NULL_REFRESH:{
            Refresh(true);
            break;
        }

        case ID_CMD_SETVP:{
            setStringVP(event.GetString());
            break;
        }

        case ID_CMD_INVALIDATE:{
            InvalidateAllGL();
            Refresh(true);
            break;
        }

        case ID_CMD_POST_JSON_TO_PLUGINS:{

            // Extract the Message ID which is embedded in the JSON string passed in the event
            wxJSONValue  root;
            wxJSONReader reader;

            int numErrors = reader.Parse( event.GetString(), &root );
            if ( numErrors == 0 )  {
                if(root[_T("MessageID")].IsString()){
                    wxString MsgID = root[_T("MessageID")].AsString();
                    SendPluginMessage( MsgID, event.GetString() );  // Send to all PlugIns
                }
            }

            break;
        }

        default: {
            //        Look for PlugIn tools
            //        If found, make the callback.
            //        TODO Modify this to allow multiple tools per plugin
            if( g_pi_manager ) {
                ArrayOfPlugInToolbarTools tool_array = g_pi_manager->GetPluginToolbarToolArray();
                for( unsigned int i = 0; i < tool_array.size(); i++ ) {
                    PlugInToolbarToolContainer *pttc = tool_array[i];
                    if( event.GetId() == pttc->id ) {
                        if( pttc->m_pplugin ) pttc->m_pplugin->OnToolbarToolCallback( pttc->id );
                        return; // required to prevent event.Skip() being called
                    }
                }
            }

            // If we didn't handle the event, allow it to bubble up to other handlers.
            // This is required for the system menu items (Hide, etc) on OS X to work.
            // This must only be called if we did NOT handle the event, otherwise it
            // stops the menu items from working on Windows.
            event.Skip();

            break;
        }

    }         // switch

}

bool MyFrame::SetGlobalToolbarViz( bool viz )
{
    bool viz_now = g_bmasterToolbarFull;
    wxString tip = _("Show Toolbar");
    if(viz){
        tip = _("Hide Toolbar");
        if( g_MainToolbar->GetToolbar() )
            g_MainToolbar->GetToolbar()->SetToolShortHelp( ID_MASTERTOGGLE, tip );
    }

    bool toggle = false;
    if(viz && !g_bmasterToolbarFull)
        toggle = true;

    else if(!viz && g_bmasterToolbarFull)
        toggle = true;

    if(toggle){
            g_bmasterToolbarFull = !g_bmasterToolbarFull;

#ifdef __WXOSX__
            if(g_bmasterToolbarFull)
                m_nMasterToolCountShown = g_MainToolbar->GetToolCount() - 1;        //TODO disable animation on OSX. Maybe use fade effect?
            else
                m_nMasterToolCountShown = 2;
#else
            m_nMasterToolCountShown = g_MainToolbar->GetToolShowCount();        // Current state
#endif
            ToolbarAnimateTimer.Start( 10, wxTIMER_ONE_SHOT );
    }

    return viz_now;
}


void MyFrame::ScheduleSettingsDialog()
{
    wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED);
    evt.SetId( ID_SETTINGS/*ID_MENU_SETTINGS_BASIC*/ );
    GetEventHandler()->AddPendingEvent(evt);
}

ChartCanvas *MyFrame::GetFocusCanvas()
{
    if( (g_canvasConfig != 0) && g_focusCanvas )             // multi-canvas?
        return g_focusCanvas;
    else
        return GetPrimaryCanvas();
}

void MyFrame::OnToolbarAnimateTimer( wxTimerEvent& event )
{
    if(g_bmasterToolbarFull){
        if(m_nMasterToolCountShown < (int)g_MainToolbar->GetToolCount()){
            m_nMasterToolCountShown++;
            g_MainToolbar->SetToolShowCount(m_nMasterToolCountShown);
            g_MainToolbar->Realize();

            ToolbarAnimateTimer.Start( 20, wxTIMER_ONE_SHOT );
        }
        else{
            //  One last "Realize()" to establish the final toolbar shape
            g_MainToolbar->GetToolbar()->InvalidateBitmaps();
            g_MainToolbar->Realize();
            g_MainToolbar->Show();
        }            
    }
    else{
        if(m_nMasterToolCountShown > 1){
            m_nMasterToolCountShown--;
            g_MainToolbar->SetToolShowCount(m_nMasterToolCountShown);
            g_MainToolbar->Realize();
            ToolbarAnimateTimer.Start( 10, wxTIMER_ONE_SHOT );
        }
        else{
            g_MainToolbar->GetToolbar()->InvalidateBitmaps();
            g_MainToolbar->Realize();
            g_MainToolbar->Show();
        }
    }
}


void MyFrame::InvalidateAllGL()
{
#ifdef ocpnUSE_GL
    // For each canvas
    for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
        ChartCanvas *cc = g_canvasArray.Item(i);
        if(cc){
            cc->InvalidateGL();
            cc->Refresh();
        }
    }
#endif    
}

void MyFrame::RefreshAllCanvas( bool bErase)
{
    // For each canvas
    for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
        ChartCanvas *cc = g_canvasArray.Item(i);
        if(cc){
            cc->Refresh( bErase );
        }
    }
}


void MyFrame::SetAISDisplayStyle(ChartCanvas *cc, int StyleIndx)
{
    cc->SetAISCanvasDisplayStyle(StyleIndx);

    UpdateGlobalMenuItems();
    ReloadAllVP();    
}

void MyFrame::setStringVP(wxString VPS)
{
    ChartCanvas *cc = GetPrimaryCanvas();
    
    if(!cc)
        return;
    
    wxStringTokenizer tkz(VPS, _T(";"));
    
    wxString token = tkz.GetNextToken();
    double lat = gLat;
    token.ToDouble(&lat);
    
    token = tkz.GetNextToken();
    double lon = gLon;
    token.ToDouble(&lon);
    
    token = tkz.GetNextToken();
    double scale_ppm = cc->GetVP().view_scale_ppm;
    token.ToDouble(&scale_ppm);
    
    cc->SetViewPoint( lat, lon, scale_ppm, 0, cc->GetVPRotation() );
    
}




void MyFrame::DoSettings()
{
    if (g_boptionsactive)
        return;

    bool bnewtoolbar = !( DoOptionsDialog() == 0 );

    //              Apply various system settings
    ApplyGlobalSettings( bnewtoolbar );                 

    if( g_MainToolbar )
        g_MainToolbar->RefreshFadeTimer();

        // ..For each canvas...
    bool b_loadHarmonics = false;    
    for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
        ChartCanvas *cc = g_canvasArray.Item(i);
        if(cc){
            if( cc->GetbShowCurrent() || cc->GetbShowTide() )
                b_loadHarmonics = true;
        }
    }
    if( b_loadHarmonics )
        LoadHarmonics();

    //  The chart display options may have changed, especially on S57 ENC,
    //  So, flush the cache and redraw
    ReloadAllVP();

}

void MyFrame::ToggleChartBar( ChartCanvas *cc)
{
    g_bShowChartBar = !g_bShowChartBar;

    if(g_bShowChartBar)
        cc->m_brepaint_piano = true;

    cc->ReloadVP(); // needed to set VP.pix_height
    Refresh();

    if(g_bShowChartBar) {
        DoChartUpdate();
        UpdateControlBar(cc);
    }

    SetMenubarItemState( ID_MENU_UI_CHARTBAR, g_bShowChartBar );
}

void MyFrame::ToggleColorScheme()
{
    static bool lastIsNight;
    ColorScheme s = GetColorScheme();
    int is = (int) s;
    is++;
    if (lastIsNight && is == 3)         // Back from step 3
        { is = 1; lastIsNight = false; }//      Goto to Day
    if (lastIsNight) is = 2;            // Back to Dusk on step 3
    if ( is == 3 ) lastIsNight = true;  // Step 2 Night
    s = (ColorScheme) is;
    if (s == N_COLOR_SCHEMES) s = GLOBAL_COLOR_SCHEME_RGB;

    SetAndApplyColorScheme( s );
}

void MyFrame::ToggleFullScreen()
{
    bool to = !IsFullScreen();

 //    long style = wxFULLSCREEN_NOBORDER | wxFULLSCREEN_NOCAPTION;; // | wxFULLSCREEN_NOMENUBAR;
#ifdef __WXOSX__
     ShowFullScreen( to );
#else
     long style = wxFULLSCREEN_NOBORDER | wxFULLSCREEN_NOCAPTION;; // | wxFULLSCREEN_NOMENUBAR;
     ShowFullScreen( to, style );
#endif
    UpdateAllToolbars( global_color_scheme );
    SurfaceAllCanvasToolbars();
    UpdateControlBar( GetPrimaryCanvas());
    Layout();
    TriggerRecaptureTimer();
}

void MyFrame::ActivateMOB( void )
{
    //    The MOB point
    wxDateTime mob_time = wxDateTime::Now();
    wxString mob_label( _( "MAN OVERBOARD" ) );
    mob_label += _(" at ");
    mob_label += mob_time.FormatTime();
    mob_label += _(" on ");
    mob_label += mob_time.FormatISODate();

    RoutePoint *pWP_MOB = new RoutePoint( gLat, gLon, _T ( "mob" ), mob_label, wxEmptyString );
    pWP_MOB->SetShared( true );
    pWP_MOB->m_bIsolatedMark = true;
    pWP_MOB->SetWaypointArrivalRadius( -1.0 ); // Negative distance is code to signal "Never Arrive"
    pWP_MOB->SetUseSca(false); //Do not use scaled hiding for MOB 
    pSelect->AddSelectableRoutePoint( gLat, gLon, pWP_MOB );
    pConfig->AddNewWayPoint( pWP_MOB, -1 );       // use auto next num


    if( bGPSValid && !std::isnan(gCog) && !std::isnan(gSog) ) {
        //    Create a point that is one mile along the present course
        double zlat, zlon;
        ll_gc_ll( gLat, gLon, gCog, 1.0, &zlat, &zlon );

        RoutePoint *pWP_src = new RoutePoint( zlat, zlon, g_default_wp_icon,
                wxString( _( "1.0 NM along COG" ) ), wxEmptyString );
        pSelect->AddSelectableRoutePoint( zlat, zlon, pWP_src );

        Route *temp_route = new Route();
        pRouteList->Append( temp_route );

        temp_route->AddPoint( pWP_src );
        temp_route->AddPoint( pWP_MOB );

        pSelect->AddSelectableRouteSegment( gLat, gLon, zlat, zlon, pWP_src, pWP_MOB, temp_route );

        temp_route->m_RouteNameString = _("Temporary MOB Route");
        temp_route->m_RouteStartString = _("Assumed 1 Mile Point");
        ;
        temp_route->m_RouteEndString = mob_label;

        temp_route->m_bDeleteOnArrival = false;

        temp_route->SetRouteArrivalRadius( -1.0 );                    // never arrives

        if( g_pRouteMan->GetpActiveRoute() ) g_pRouteMan->DeactivateRoute();
        g_pRouteMan->ActivateRoute( temp_route, pWP_MOB );

        wxJSONValue v;
        v[_T("GUID")] = temp_route->m_GUID;
        wxString msg_id( _T("OCPN_MAN_OVERBOARD") );
        g_pi_manager->SendJSONMessageToAllPlugins( msg_id, v );
    }

    if( pRouteManagerDialog && pRouteManagerDialog->IsShown() ) {
        pRouteManagerDialog->UpdateRouteListCtrl();
        pRouteManagerDialog->UpdateWptListCtrl();
    }

    InvalidateAllGL();
    RefreshAllCanvas( false );

    wxString mob_message( _( "MAN OVERBOARD" ) );
    mob_message += _(" Time: ");
    mob_message += mob_time.Format();
    mob_message += _("  Position: ");
    mob_message += toSDMM( 1, gLat );
    mob_message += _T("   ");
    mob_message += toSDMM( 2, gLon );
    wxLogMessage( mob_message );

}
void MyFrame::TrackOn( void )
{
    g_bTrackActive = true;
    g_pActiveTrack = new ActiveTrack();

    pTrackList->Append( g_pActiveTrack );
    if(pConfig)
        pConfig->AddNewTrack( g_pActiveTrack );

    g_pActiveTrack->Start();

    SetMasterToolbarItemState( ID_TRACK, g_bTrackActive );
    if( g_MainToolbar )
        g_MainToolbar->SetToolShortHelp( ID_TRACK, _("Disable Tracking") );

    SetMenubarItemState( ID_MENU_NAV_TRACK, g_bTrackActive );

#ifdef __OCPN__ANDROID__
    androidSetTrackTool(true);
#endif


    if( pRouteManagerDialog && pRouteManagerDialog->IsShown() )
    {
        pRouteManagerDialog->UpdateTrkListCtrl();
        pRouteManagerDialog->UpdateRouteListCtrl();
    }

    wxJSONValue v;
    wxDateTime now;
    now = now.Now().ToUTC();
    wxString name = g_pActiveTrack->GetName();
    if(name.IsEmpty())
    {
        TrackPoint *tp = g_pActiveTrack->GetPoint( 0 );
        if( tp->GetCreateTime().IsValid() )
            name = tp->GetCreateTime().FormatISODate() + _T(" ") + tp->GetCreateTime().FormatISOTime();
        else
            name = _("(Unnamed Track)");
    }
    v[_T("Name")] = name;
    v[_T("GUID")] = g_pActiveTrack->m_GUID;
    wxString msg_id( _T("OCPN_TRK_ACTIVATED") );
    g_pi_manager->SendJSONMessageToAllPlugins( msg_id, v );
    g_FlushNavobjChangesTimeout = 30;           //Every thirty seconds, consider flushing navob changes
}

Track *MyFrame::TrackOff( bool do_add_point )
{
    Track *return_val = g_pActiveTrack;

    if( g_pActiveTrack )
    {
        wxJSONValue v;
        wxString msg_id( _T("OCPN_TRK_DEACTIVATED") );
        v[_T("GUID")] = g_pActiveTrack->m_GUID;
        g_pi_manager->SendJSONMessageToAllPlugins( msg_id, v );

        g_pActiveTrack->Stop( do_add_point );

        if( g_pActiveTrack->GetnPoints() < 2 ) {
            g_pRouteMan->DeleteTrack( g_pActiveTrack );
            return_val = NULL;
        }
        else {
            if( g_bTrackDaily ) {
                Track *pExtendTrack = g_pActiveTrack->DoExtendDaily();
                if(pExtendTrack) {
                    g_pRouteMan->DeleteTrack( g_pActiveTrack );
                    return_val = pExtendTrack;
                }
            }
        }
    }

    g_pActiveTrack = NULL;

    g_bTrackActive = false;

    if(RouteManagerDialog::getInstanceFlag()){
        if( pRouteManagerDialog && pRouteManagerDialog->IsShown() ){
            pRouteManagerDialog->UpdateTrkListCtrl();
            pRouteManagerDialog->UpdateRouteListCtrl();
        }
    }

    SetMasterToolbarItemState( ID_TRACK, g_bTrackActive );
    if( g_MainToolbar )
        g_MainToolbar->SetToolShortHelp( ID_TRACK, _("Enable Tracking") );
    SetMenubarItemState( ID_MENU_NAV_TRACK, g_bTrackActive );

#ifdef __OCPN__ANDROID__
    androidSetTrackTool(false);
#endif

    g_FlushNavobjChangesTimeout = 600;           //Revert to checking/flushing navob changes every 5 minutes

    return return_val;
}

bool MyFrame::ShouldRestartTrack( void )
{
    if( !g_pActiveTrack || !g_bTrackDaily)
        return false;
    time_t now = wxDateTime::Now().GetTicks();
    time_t today = wxDateTime::Today().GetTicks();
    int rotate_at = 0;
    switch( g_track_rotate_time_type )
    {
        case TIME_TYPE_LMT:
            rotate_at = g_track_rotate_time + wxRound(gLon * 3600. / 15.);
            break;
        case TIME_TYPE_COMPUTER:
            rotate_at = g_track_rotate_time;
            break;
        case TIME_TYPE_UTC:
            int utc_offset = wxDateTime::Now().GetTicks() - wxDateTime::Now().ToUTC().GetTicks();
            rotate_at = g_track_rotate_time + utc_offset;
            break;
    }
    if( rotate_at > 86400 )
        rotate_at -= 86400;
    else if (rotate_at < 0 )
        rotate_at += 86400;
    if( now >= m_last_track_rotation_ts + 86400 - 3600 &&
        now - today >= rotate_at )
    {
        if( m_last_track_rotation_ts == 0 )
        {
            if( now - today > rotate_at)
                m_last_track_rotation_ts = today + rotate_at;
            else
                m_last_track_rotation_ts = today + rotate_at - 86400;
            return false;
        }
        m_last_track_rotation_ts = now;
        return true;
    }
    return false;
}

void MyFrame::TrackDailyRestart( void )
{
    if( !g_pActiveTrack )
        return;

    Track *pPreviousTrack = TrackOff( true );
    TrackOn();

    //  Set the restarted track's current state such that the current track point's attributes match the
    //  attributes of the last point of the track that was just stopped at midnight.

    if( pPreviousTrack ) {
        TrackPoint *pMidnightPoint = pPreviousTrack->GetLastPoint();
        g_pActiveTrack->AdjustCurrentTrackPoint(pMidnightPoint);
    }

    if( pRouteManagerDialog && pRouteManagerDialog->IsShown() ) {
        pRouteManagerDialog->UpdateTrkListCtrl();
        pRouteManagerDialog->UpdateRouteListCtrl();
    }
}

void MyFrame::SetUpMode( ChartCanvas *cc, int mode )
{
    if(cc){
        cc->SetUpMode( mode );

        SetMenubarItemState( ID_MENU_CHART_COGUP, mode == COURSE_UP_MODE );
        SetMenubarItemState( ID_MENU_CHART_NORTHUP, mode == NORTH_UP_MODE );
        SetMenubarItemState( ID_MENU_CHART_HEADUP, mode == HEAD_UP_MODE );

        if(m_pMenuBar)
            m_pMenuBar->SetLabel( ID_MENU_CHART_NORTHUP, _("North Up Mode") );
    }
}

void MyFrame::ToggleENCText( ChartCanvas *cc )
{
    cc->SetShowENCText( !cc->GetShowENCText());

    SetMenubarItemState( ID_MENU_ENC_TEXT, cc->GetShowENCText() );

//     if(g_pi_manager)
//         g_pi_manager->SendConfigToAllPlugIns();

    ReloadAllVP();
}

void MyFrame::SetENCDisplayCategory( ChartCanvas *cc, enum _DisCat nset )
{
#ifdef USE_S57
    if( ps52plib ) {

       if(cc){
            cc->SetENCDisplayCategory(nset);

            UpdateGlobalMenuItems();

/*            if(g_pi_manager)
                g_pi_manager->SendConfigToAllPlugIns();
 */
       ReloadAllVP();
       }
    }

#endif
}

void MyFrame::ToggleSoundings( ChartCanvas *cc )
{
    cc->SetShowENCDepth( !cc->GetShowENCDepth());

    SetMenubarItemState( ID_MENU_ENC_SOUNDINGS, cc->GetShowENCDepth() );

//     if(g_pi_manager)
//         g_pi_manager->SendConfigToAllPlugIns();

    ReloadAllVP();
}

bool MyFrame::ToggleLights( ChartCanvas *cc )
{
    cc->SetShowENCLights( !cc->GetShowENCLights());

    SetMenubarItemState( ID_MENU_ENC_LIGHTS, cc->GetShowENCLights() );

    if(g_pi_manager)
        g_pi_manager->SendS52ConfigToAllPlugIns( true );

    ReloadAllVP();

    return true;
}

#if 0
void MyFrame::ToggleRocks( void )
{
#ifdef USE_S57
    if( ps52plib ) {
        int vis =  0;
        // Need to loop once for UWTROC, which is our "master", then for
        // other categories, since order is unknown?
        for( unsigned int iPtr = 0; iPtr < ps52plib->pOBJLArray->GetCount(); iPtr++ ) {
            OBJLElement *pOLE = (OBJLElement *) ( ps52plib->pOBJLArray->Item( iPtr ) );
            if( !strncmp( pOLE->OBJLName, "UWTROC", 6 ) ) {
                pOLE->nViz = !pOLE->nViz;
                vis = pOLE->nViz;
            }
        }
        for( unsigned int iPtr = 0; iPtr < ps52plib->pOBJLArray->GetCount(); iPtr++ ) {
            OBJLElement *pOLE = (OBJLElement *) ( ps52plib->pOBJLArray->Item( iPtr ) );
            if( !strncmp( pOLE->OBJLName, "OBSTRN", 6 ) ) {
                pOLE->nViz = vis;
            }
            if( !strncmp( pOLE->OBJLName, "WRECKS", 6 ) ) {
                pOLE->nViz = vis;
            }
        }
        ps52plib->GenerateStateHash();
        ReloadAllVP();
    }
#endif
}
#endif

void MyFrame::ToggleAnchor( ChartCanvas *cc )
{
    cc->SetShowENCAnchor( !cc->GetShowENCAnchor());

    SetMenubarItemState( ID_MENU_ENC_ANCHOR, cc->GetShowENCAnchor() );

    if(g_pi_manager)
        g_pi_manager->SendS52ConfigToAllPlugIns();

    ReloadAllVP();
}

void MyFrame::ToggleDataQuality( ChartCanvas *cc )
{
    cc->SetShowENCDataQual( !cc->GetShowENCDataQual());

    SetMenubarItemState( ID_MENU_ENC_DATA_QUALITY, cc->GetShowENCDataQual() );

    if(g_pi_manager)
        g_pi_manager->SendS52ConfigToAllPlugIns();

    ReloadAllVP();
}

void MyFrame::TogglebFollow( ChartCanvas *cc )
{
    if( !cc->m_bFollow ) SetbFollow( cc );
    else
        ClearbFollow( cc );

}

void MyFrame::ToggleNavobjects( ChartCanvas *cc )
{
    cc->m_bShowNavobjects = !cc->m_bShowNavobjects;
    SetMenubarItemState( ID_MENU_SHOW_NAVOBJECTS, cc->m_bShowNavobjects );
    cc->Refresh();
}

void MyFrame::ToggleAISDisplay( ChartCanvas *cc )
{
    cc->SetShowAIS(!cc->GetShowAIS());
    SetMenubarItemState( ID_MENU_AIS_TARGETS, cc->GetShowAIS() );
    cc->Refresh();

}

void MyFrame::ToggleAISMinimizeTargets( ChartCanvas *cc )
{
    cc->SetAttenAIS(!cc->GetAttenAIS());
    SetMenubarItemState( ID_MENU_AIS_SCALED_TARGETS, cc->GetAttenAIS() );
    cc->Refresh();
}


void MyFrame::SetbFollow( ChartCanvas *cc )
{
    JumpToPosition(cc, gLat, gLon, cc->GetVPScale());
    cc->m_bFollow = true;

    cc->SetCanvasToolbarItemState( ID_FOLLOW, true );
    SetMenubarItemState( ID_MENU_NAV_FOLLOW, true );

#ifdef __OCPN__ANDROID__
    androidSetFollowTool(true);
#endif

    DoChartUpdate();
    cc->ReloadVP();
    SetChartUpdatePeriod( );
}

void MyFrame::ClearbFollow( ChartCanvas *cc )
{
    //    Center the screen on the GPS position, for lack of a better place
    vLat = gLat;
    vLon = gLon;

#ifdef __OCPN__ANDROID__
    androidSetFollowTool(false);
#endif

    cc->m_bFollow = false;
    cc->SetCanvasToolbarItemState(ID_FOLLOW, false );
    SetMenubarItemState( ID_MENU_NAV_FOLLOW, false );

    DoChartUpdate();
    cc->ReloadVP();
    SetChartUpdatePeriod();
}

void MyFrame::ToggleChartOutlines( ChartCanvas *cc )
{
    cc->SetShowOutlines( !cc->GetShowOutlines() );

    RefreshAllCanvas( false );

#ifdef ocpnUSE_GL         // opengl renders chart outlines as part of the chart this needs a full refresh
    if( g_bopengl )
        InvalidateAllGL();
#endif

    SetMenubarItemState( ID_MENU_CHART_OUTLINES, cc->GetShowOutlines() );
}

void MyFrame::ToggleTestPause( void )
{
    g_bPauseTest = !g_bPauseTest;
}

void MyFrame::SetMenubarItemState( int item_id, bool state )
{
    if( m_pMenuBar ) {
        bool enabled = m_pMenuBar->IsEnabled( item_id );
        m_pMenuBar->Enable( item_id, false );
        m_pMenuBar->Check( item_id, state );
        m_pMenuBar->Enable( item_id, enabled );
     }
}

void MyFrame::SetMasterToolbarItemState( int tool_id, bool state )
{
    if( g_MainToolbar && g_MainToolbar->GetToolbar() )
        g_MainToolbar->GetToolbar()->ToggleTool( tool_id, state );
}


void MyFrame::SetToolbarItemBitmaps( int tool_id, wxBitmap *bmp, wxBitmap *bmpRollover )
{
    if( g_MainToolbar && g_MainToolbar->GetToolbar() ) {
        g_MainToolbar->GetToolbar()->SetToolBitmaps( tool_id, bmp, bmpRollover );
        wxRect rect = g_MainToolbar->GetToolbar()->GetToolRect( tool_id );
        g_MainToolbar->GetToolbar()->RefreshRect( rect );
    }
}

void MyFrame::SetToolbarItemSVG( int tool_id, wxString normalSVGfile, wxString rolloverSVGfile, wxString toggledSVGfile )
{
    if( g_MainToolbar && g_MainToolbar->GetToolbar() ) {
        g_MainToolbar->GetToolbar()->SetToolBitmapsSVG( tool_id, normalSVGfile, rolloverSVGfile, toggledSVGfile );
        wxRect rect = g_MainToolbar->GetToolbar()->GetToolRect( tool_id );
        g_MainToolbar->GetToolbar()->RefreshRect( rect );
    }
}


void MyFrame::ApplyGlobalSettings( bool bnewtoolbar )
{
    //             ShowDebugWindow as a wxStatusBar
    m_StatusBarFieldCount = g_Platform->GetStatusBarFieldCount();

#ifdef __WXMSW__
    UseNativeStatusBar( false );              // better for MSW, undocumented in frame.cpp
#endif

    if( g_bShowStatusBar ) {
        if( !m_pStatusBar ) {
            m_pStatusBar = CreateStatusBar( m_StatusBarFieldCount, 0 );   // No wxST_SIZEGRIP needed
            ApplyGlobalColorSchemetoStatusBar();
        }

    } else {
        if( m_pStatusBar ) {
            m_pStatusBar->Destroy();
            m_pStatusBar = NULL;
            SetStatusBar( NULL );
        }
    }

    wxSize lastOptSize = options_lastWindowSize;
    SendSizeEvent();

    BuildMenuBar();

    SendSizeEvent();
    options_lastWindowSize = lastOptSize;

    if( bnewtoolbar )
        UpdateAllToolbars( global_color_scheme );

}


wxString _menuText( wxString name, wxString shortcut ) {
    wxString menutext;
    menutext << name;
#ifndef __OCPN__ANDROID__
    menutext << _T("\t") << shortcut;
#endif
    return menutext;
}

void MyFrame::BuildMenuBar( void )
{
    /*
     * Menu Bar - add or remove it if necessary, and update the state of the menu items
     */
#ifdef __WXOSX__
    bool showMenuBar = true;    // the menu bar is always visible in OS X
#else
    bool showMenuBar = g_bShowMenuBar; // get visibility from options

    if (!showMenuBar && g_bTempShowMenuBar)     // allows pressing alt to temporarily show
        showMenuBar = true;
#endif

    if ( showMenuBar ) {
        //  Menu bar has some dependencies on S52 PLIB, so be sure it is loaded.
        LoadS57();

        if ( !m_pMenuBar ) {    // add the menu bar if it is enabled
            m_pMenuBar = new wxMenuBar();
            RegisterGlobalMenuItems();
            SetMenuBar(m_pMenuBar); // must be after RegisterGlobalMenuItems for wx to populate the OS X App Menu correctly
        }

        UpdateGlobalMenuItems(); // update the state of the menu items (checkmarks etc)
    } else {
        if ( m_pMenuBar ) {     // remove the menu bar if it is disabled
            SetMenuBar( NULL );
            m_pMenuBar->Destroy();
            m_pMenuBar = NULL;
        }
    }
}

void MyFrame::RegisterGlobalMenuItems()
{
    if ( !m_pMenuBar ) return;  // if there isn't a menu bar


    wxMenu *nav_menu = new wxMenu();
    nav_menu->AppendCheckItem( ID_MENU_NAV_FOLLOW, _menuText(_("Auto Follow"), _T("Ctrl-A")) );
    nav_menu->AppendCheckItem( ID_MENU_NAV_TRACK, _("Enable Tracking") );
    nav_menu->AppendSeparator();
    nav_menu->AppendRadioItem( ID_MENU_CHART_NORTHUP, _("North Up Mode") );
    nav_menu->AppendRadioItem( ID_MENU_CHART_COGUP, _("Course Up Mode") );
    nav_menu->AppendRadioItem( ID_MENU_CHART_HEADUP, _("Head Up Mode") );
    nav_menu->AppendSeparator();
#ifndef __WXOSX__
    nav_menu->Append( ID_MENU_ZOOM_IN, _menuText(_("Zoom In"), _T("+")) );
    nav_menu->Append( ID_MENU_ZOOM_OUT, _menuText(_("Zoom Out"), _T("-")) );
#else
    nav_menu->Append( ID_MENU_ZOOM_IN, _menuText(_("Zoom In"), _T("Ctrl-+")) );
    nav_menu->Append( ID_MENU_ZOOM_OUT, _menuText(_("Zoom Out"), _T("Ctrl--")) );
#endif
    nav_menu->AppendSeparator();
    nav_menu->Append( ID_MENU_SCALE_IN, _menuText(_("Larger Scale Chart"), _T("Ctrl-Left")) );
    nav_menu->Append( ID_MENU_SCALE_OUT, _menuText(_("Smaller Scale Chart"), _T("Ctrl-Right")) );
#ifndef __WXOSX__
    nav_menu->AppendSeparator();
    nav_menu->Append( ID_MENU_OQUIT, _menuText(_("Exit OpenCPN"), _T("Ctrl-Q")) );
#endif
#ifdef __WXOSX__
    m_pMenuBar->Append( nav_menu, _("Navigate") );
#else
    m_pMenuBar->Append( nav_menu, _("&Navigate") );
#endif

    wxMenu* view_menu = new wxMenu();
#ifndef __WXOSX__
    view_menu->AppendCheckItem( ID_MENU_CHART_QUILTING, _menuText(_("Enable Chart Quilting"), _T("Q")) );
    view_menu->AppendCheckItem( ID_MENU_CHART_OUTLINES, _menuText(_("Show Chart Outlines"), _T("O")) );
#else
    view_menu->AppendCheckItem( ID_MENU_CHART_QUILTING, _menuText(_("Enable Chart Quilting"), _T("Alt-Q")) );
    view_menu->AppendCheckItem( ID_MENU_CHART_OUTLINES, _menuText(_("Show Chart Outlines"), _T("Alt-O")) );
#endif
    view_menu->AppendCheckItem( ID_MENU_UI_CHARTBAR, _menuText(_("Show Chart Bar"), _T("Ctrl-B")) );

#ifdef USE_S57
    view_menu->AppendSeparator();
#ifndef __WXOSX__
    view_menu->AppendCheckItem( ID_MENU_ENC_TEXT, _menuText(_("Show ENC text"), _T("T")) );
    view_menu->AppendCheckItem( ID_MENU_ENC_LIGHTS, _menuText(_("Show ENC Lights"), _T("L")) );
    view_menu->AppendCheckItem( ID_MENU_ENC_SOUNDINGS, _menuText(_("Show ENC Soundings"), _T("S")) );
    view_menu->AppendCheckItem( ID_MENU_ENC_ANCHOR, _menuText(_("Show ENC Anchoring Info"), _T("A")) );
    view_menu->AppendCheckItem( ID_MENU_ENC_DATA_QUALITY, _menuText(_("Show ENC Data Quality"), _T("U")) );
    view_menu->AppendCheckItem( ID_MENU_SHOW_NAVOBJECTS, _menuText(_("Show Navobjects"), _T("V")) );
#else
    view_menu->AppendCheckItem( ID_MENU_ENC_TEXT, _menuText(_("Show ENC text"), _T("Alt-T")) );
    view_menu->AppendCheckItem( ID_MENU_ENC_LIGHTS, _menuText(_("Show ENC Lights"), _T("Alt-L")) );
    view_menu->AppendCheckItem( ID_MENU_ENC_SOUNDINGS, _menuText(_("Show ENC Soundings"), _T("Alt-S")) );
    view_menu->AppendCheckItem( ID_MENU_ENC_ANCHOR, _menuText(_("Show ENC Anchoring Info"), _T("Alt-A")) );
    view_menu->AppendCheckItem( ID_MENU_ENC_DATA_QUALITY, _menuText(_("Show ENC Data Quality"), _T("Alt-U")) );
    view_menu->AppendCheckItem( ID_MENU_SHOW_NAVOBJECTS, _menuText(_("Show Navobjects"), _T("Alt-V")) );
#endif
#endif
    view_menu->AppendSeparator();
    view_menu->AppendCheckItem( ID_MENU_SHOW_TIDES, _("Show Tides") );
    view_menu->AppendCheckItem( ID_MENU_SHOW_CURRENTS, _("Show Currents") );
    view_menu->AppendSeparator();
#ifndef __WXOSX__
    view_menu->Append( ID_MENU_UI_COLSCHEME, _menuText(_("Change Color Scheme"), _T("C")) );
#else
    view_menu->Append( ID_MENU_UI_COLSCHEME, _menuText(_("Change Color Scheme"), _T("Alt-C")) );
#endif

    view_menu->AppendSeparator();
#ifdef __WXOSX__
    view_menu->Append(ID_MENU_UI_FULLSCREEN, _menuText(_("Enter Full Screen"), _T("RawCtrl-Ctrl-F")) );
#else
    view_menu->Append(ID_MENU_UI_FULLSCREEN, _menuText(_("Enter Full Screen"), _T("F11")) );
#endif
    m_pMenuBar->Append( view_menu, _("&View") );


    wxMenu* ais_menu = new wxMenu();
    ais_menu->AppendCheckItem( ID_MENU_AIS_TARGETS, _("Show AIS Targets") );
    ais_menu->AppendCheckItem( ID_MENU_AIS_SCALED_TARGETS, _("Attenuate less critical AIS targets") );
    ais_menu->AppendSeparator();
    ais_menu->AppendCheckItem( ID_MENU_AIS_MOORED_TARGETS, _("Hide Moored AIS Targets") );
    ais_menu->AppendCheckItem( ID_MENU_AIS_TRACKS, _("Show AIS Target Tracks") );
    ais_menu->AppendCheckItem( ID_MENU_AIS_CPADIALOG, _("Show CPA Alert Dialogs") );
    ais_menu->AppendCheckItem( ID_MENU_AIS_CPASOUND, _("Sound CPA Alarms") );
    ais_menu->AppendSeparator();
    ais_menu->Append( ID_MENU_AIS_TARGETLIST, _("AIS target list") + _T("...") );
#ifdef __WXOSX__
    m_pMenuBar->Append( ais_menu, _("AIS") );
#else
    m_pMenuBar->Append( ais_menu, _("&AIS") );
#endif
    wxMenu* tools_menu = new wxMenu();
#ifndef __WXOSX__
    tools_menu->Append( ID_MENU_TOOL_MEASURE, _menuText(_("Measure Distance"), _T("M")) );
#else
    tools_menu->Append( ID_MENU_TOOL_MEASURE, _menuText(_("Measure Distance"), _T("Alt-M")) );
#endif

    tools_menu->AppendSeparator();
    tools_menu->Append( ID_MENU_ROUTE_MANAGER, _("Route && Mark Manager...") );
    tools_menu->Append( ID_MENU_ROUTE_NEW, _menuText(_("Create Route"), _T("Ctrl-R")) );
    tools_menu->AppendSeparator();
    tools_menu->Append( ID_MENU_MARK_BOAT, _menuText(_("Drop Mark at Boat"), _T("Ctrl-O")) );
    tools_menu->Append( ID_MENU_MARK_CURSOR, _menuText(_("Drop Mark at Cursor"), _T("Ctrl-M")) );
    tools_menu->AppendSeparator();
#ifdef __WXOSX__
    tools_menu->Append( ID_MENU_MARK_MOB, _menuText(_("Drop MOB Marker"), _T("Alt-Space")) ); // NOTE Cmd+Space is reserved for Spotlight
    tools_menu->AppendSeparator();
    tools_menu->Append( wxID_PREFERENCES, _menuText(_("Preferences") + _T("..."), _T("Ctrl-,")) );
#else
    tools_menu->Append( ID_MENU_MARK_MOB, _menuText(_("Drop MOB Marker"), _T("Ctrl-Space")) );
    tools_menu->AppendSeparator();
    tools_menu->Append( wxID_PREFERENCES, _menuText(_("Options") + _T("..."), _T("Ctrl-,")) );
#endif
#ifdef __WXOSX__
    m_pMenuBar->Append( tools_menu, _("Tools") );
#else
    m_pMenuBar->Append( tools_menu, _("&Tools") );
#endif
#ifdef __WXOSX__
    wxMenu* window_menu = new wxMenu();
    m_pMenuBar->Append( window_menu, _("&Window") );
#endif
    wxMenu* help_menu = new wxMenu();
    help_menu->Append( wxID_ABOUT, _("About OpenCPN") );
#ifdef __WXOSX__
    help_menu->Append( wxID_HELP, _menuText(_("OpenCPN Hilfe"), _T("Ctrl-J")) );
    m_pMenuBar->Append( help_menu, _("Help"));
        
#if USE_SPARKLE
    wxMenu *apple = m_pMenuBar->OSXGetAppleMenu();
    if (!apple)
        return; // huh
    Sparkle_AddMenuItem(apple->GetHMenu(), _("Check for Updates...").utf8_str());
#endif
#else
    help_menu->Append( wxID_HELP, _("OpenCPN Help") );
    m_pMenuBar->Append( help_menu, _("&Help") );
#endif

    // Set initial values for menu check items and radio items
    UpdateGlobalMenuItems();
}

void MyFrame::UpdateGlobalMenuItems()
{
    if ( !m_pMenuBar ) return;  // if there isn't a menu bar

    m_pMenuBar->FindItem( ID_MENU_NAV_FOLLOW )->Check( GetPrimaryCanvas()->m_bFollow );
    m_pMenuBar->FindItem( ID_MENU_CHART_NORTHUP )->Check( GetPrimaryCanvas()->GetUpMode() == NORTH_UP_MODE );
    m_pMenuBar->FindItem( ID_MENU_CHART_COGUP )->Check( GetPrimaryCanvas()->GetUpMode() == COURSE_UP_MODE );
    m_pMenuBar->FindItem( ID_MENU_CHART_HEADUP )->Check( GetPrimaryCanvas()->GetUpMode() == HEAD_UP_MODE );
    m_pMenuBar->FindItem( ID_MENU_NAV_TRACK )->Check( g_bTrackActive );
    m_pMenuBar->FindItem( ID_MENU_CHART_OUTLINES )->Check( g_bShowOutlines );
    m_pMenuBar->FindItem( ID_MENU_CHART_QUILTING )->Check( g_bQuiltEnable );
    m_pMenuBar->FindItem( ID_MENU_UI_CHARTBAR )->Check( g_bShowChartBar );
    m_pMenuBar->FindItem( ID_MENU_AIS_TARGETS )->Check( g_bShowAIS );
    m_pMenuBar->FindItem( ID_MENU_AIS_MOORED_TARGETS )->Check( g_bHideMoored );
    m_pMenuBar->FindItem( ID_MENU_AIS_SCALED_TARGETS )->Check( g_bShowScaled );
    m_pMenuBar->FindItem( ID_MENU_AIS_SCALED_TARGETS )->Enable(g_bAllowShowScaled);
    m_pMenuBar->FindItem( ID_MENU_AIS_TRACKS )->Check( g_bAISShowTracks );
    m_pMenuBar->FindItem( ID_MENU_AIS_CPADIALOG )->Check( g_bAIS_CPA_Alert );
    m_pMenuBar->FindItem( ID_MENU_AIS_CPASOUND )->Check( g_bAIS_CPA_Alert_Audio );
    m_pMenuBar->FindItem( ID_MENU_SHOW_NAVOBJECTS )->Check( GetPrimaryCanvas()->m_bShowNavobjects );

#ifdef USE_S57
    if( ps52plib ) {
        m_pMenuBar->FindItem( ID_MENU_ENC_TEXT )->Check( ps52plib->GetShowS57Text() );
        m_pMenuBar->FindItem( ID_MENU_ENC_SOUNDINGS )->Check( ps52plib->GetShowSoundings() );

        bool light_state = false;
        if( ps52plib ) {
            for( unsigned int iPtr = 0; iPtr < ps52plib->pOBJLArray->GetCount(); iPtr++ ) {
                OBJLElement *pOLE = (OBJLElement *) ( ps52plib->pOBJLArray->Item( iPtr ) );
                if( !strncmp( pOLE->OBJLName, "LIGHTS", 6 ) ) {
                    light_state = (pOLE->nViz == 1);
                    break;
                }
            }
        }
        m_pMenuBar->FindItem( ID_MENU_ENC_LIGHTS )->Check( (!ps52plib->IsObjNoshow("LIGHTS")) && light_state );

        // Menu "Anchor Info" entry is only accessible in "All" or "User Standard" categories
        DisCat nset = ps52plib->GetDisplayCategory();
        if((nset == MARINERS_STANDARD) || (nset == OTHER) ){
            m_pMenuBar->FindItem( ID_MENU_ENC_ANCHOR )->Check( !ps52plib->IsObjNoshow("SBDARE") );
            m_pMenuBar->Enable( ID_MENU_ENC_ANCHOR, true);
            m_pMenuBar->FindItem( ID_MENU_ENC_DATA_QUALITY )->Check( !ps52plib->IsObjNoshow("M_QUAL")  );
            m_pMenuBar->Enable( ID_MENU_ENC_DATA_QUALITY, true);
        }
        else{
            m_pMenuBar->FindItem( ID_MENU_ENC_ANCHOR )->Check( false );
            m_pMenuBar->Enable( ID_MENU_ENC_ANCHOR, false);
            m_pMenuBar->Enable( ID_MENU_ENC_DATA_QUALITY, false);
        }

    }
#endif
}

void MyFrame::UpdateGlobalMenuItems( ChartCanvas *cc)
{
    if ( !m_pMenuBar ) return;  // if there isn't a menu bar

    m_pMenuBar->FindItem( ID_MENU_NAV_FOLLOW )->Check( cc->m_bFollow );

    if(cc->GetUpMode() == NORTH_UP_MODE)
        m_pMenuBar->FindItem( ID_MENU_CHART_NORTHUP )->Check( true  );
    else if (cc->GetUpMode() == COURSE_UP_MODE)
        m_pMenuBar->FindItem( ID_MENU_CHART_COGUP )->Check( true );
    else
        m_pMenuBar->FindItem( ID_MENU_CHART_HEADUP )->Check( true );

    m_pMenuBar->FindItem( ID_MENU_NAV_TRACK )->Check( g_bTrackActive );
    m_pMenuBar->FindItem( ID_MENU_CHART_OUTLINES )->Check( cc->GetShowOutlines() );
    m_pMenuBar->FindItem( ID_MENU_CHART_QUILTING )->Check( cc->GetQuiltMode() );
    m_pMenuBar->FindItem( ID_MENU_UI_CHARTBAR )->Check( cc->GetShowChartbar() );
    m_pMenuBar->FindItem( ID_MENU_AIS_TARGETS )->Check( cc->GetShowAIS() );
    m_pMenuBar->FindItem( ID_MENU_AIS_MOORED_TARGETS )->Check( g_bHideMoored );
    m_pMenuBar->FindItem( ID_MENU_AIS_SCALED_TARGETS )->Check( cc->GetAttenAIS() );
    m_pMenuBar->FindItem( ID_MENU_AIS_SCALED_TARGETS )->Enable(g_bAllowShowScaled);
    m_pMenuBar->FindItem( ID_MENU_AIS_TRACKS )->Check( g_bAISShowTracks );
    m_pMenuBar->FindItem( ID_MENU_AIS_CPADIALOG )->Check( g_bAIS_CPA_Alert );
    m_pMenuBar->FindItem( ID_MENU_AIS_CPASOUND )->Check( g_bAIS_CPA_Alert_Audio );
    m_pMenuBar->FindItem( ID_MENU_SHOW_NAVOBJECTS )->Check( cc->m_bShowNavobjects );
    m_pMenuBar->FindItem( ID_MENU_SHOW_TIDES )->Check( cc->GetbShowTide() );
    m_pMenuBar->FindItem( ID_MENU_SHOW_CURRENTS )->Check( cc->GetbShowCurrent() );

#ifdef USE_S57
    if( ps52plib ) {
        m_pMenuBar->FindItem( ID_MENU_ENC_TEXT )->Check( cc->GetShowENCText() );
        m_pMenuBar->FindItem( ID_MENU_ENC_SOUNDINGS )->Check( cc->GetShowENCDepth() );

        if( ps52plib ) {
            for( unsigned int iPtr = 0; iPtr < ps52plib->pOBJLArray->GetCount(); iPtr++ ) {
                OBJLElement *pOLE = (OBJLElement *) ( ps52plib->pOBJLArray->Item( iPtr ) );
                if( !strncmp( pOLE->OBJLName, "LIGHTS", 6 ) ) {
                    break;
                }
            }
        }
        m_pMenuBar->FindItem( ID_MENU_ENC_LIGHTS )->Check( cc->GetShowENCLights() );

        // Menu "Anchor Info" entry is only accessible in "All" or "UserStandard" categories
        DisCat nset = (DisCat)cc->GetENCDisplayCategory();
        if((nset == MARINERS_STANDARD) || (nset == OTHER) ){
            m_pMenuBar->FindItem( ID_MENU_ENC_ANCHOR )->Check( cc->GetShowENCAnchor());
            m_pMenuBar->Enable( ID_MENU_ENC_ANCHOR, true);
            m_pMenuBar->FindItem( ID_MENU_ENC_DATA_QUALITY )->Check( cc->GetShowENCDataQual()  );
            m_pMenuBar->Enable( ID_MENU_ENC_DATA_QUALITY, true);
        }
        else{
            m_pMenuBar->FindItem( ID_MENU_ENC_ANCHOR )->Check( false );
            m_pMenuBar->Enable( ID_MENU_ENC_ANCHOR, false);
            m_pMenuBar->Enable( ID_MENU_ENC_DATA_QUALITY, false);
        }

    }
#endif
}

void MyFrame::InvalidateAllCanvasUndo()
{
    // .. for each canvas...
    for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
        ChartCanvas *cc = g_canvasArray.Item(i);
        if(cc)
            cc->undo->InvalidateUndo( );
    }
}



void MyFrame::SubmergeAllCanvasToolbars( void )
{
}

void MyFrame::SurfaceAllCanvasToolbars( void )
{

#ifndef __WXQT__
       //  removed to show MUIBars on MSVC
       // Raise();
#endif
}

void MyFrame::ToggleAllToolbars( bool b_smooth )
{
    // .. for each canvas...
    for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
        ChartCanvas *cc = g_canvasArray.Item(i);
        if(cc)
            cc->ToggleToolbar( b_smooth );
    }
}


void MyFrame::JumpToPosition( ChartCanvas *cc, double lat, double lon, double scale )
{
    if (lon > 180.0)
        lon -= 360.0;
    // XXX is vLat/vLon always equal to cc m_vLat, m_vLon after SetViewPoint? Does it matter?
    vLat = lat;
    vLon = lon;
    cc->JumpToPosition(lat, lon, scale);

    if( g_pi_manager ) {
        g_pi_manager->SendViewPortToRequestingPlugIns( cc->GetVP() );
    }
}

void MyFrame::UpdateCanvasConfigDescriptors()
{
    // ..For each canvas...
    for(unsigned int i=0 ; i < g_canvasConfigArray.GetCount() ; i++){
        canvasConfig *cc = g_canvasConfigArray.Item(i);
        if(cc ){
            ChartCanvas *chart = cc->canvas;
            if(chart){
                cc->iLat = chart->GetVP().clat;
                cc->iLon = chart->GetVP().clon;
                cc->iRotation = chart->GetVP().rotation;
                cc->iScale = chart->GetVP().view_scale_ppm;
                cc->DBindex = chart->GetQuiltReferenceChartIndex();
                cc->GroupID = chart->m_groupIndex;
                cc->canvasSize = chart->GetSize();
            }

        }
    }
}





int MyFrame::DoOptionsDialog()
{
    if (g_boptionsactive)
        return 0;

    g_boptionsactive = true;
    g_last_ChartScaleFactor = g_ChartScaleFactor;


    if(NULL == g_options) {
        g_Platform->ShowBusySpinner();
        g_options = new options( this, -1, _("Options") );

        g_Platform->HideBusySpinner();
    }

//    Set initial Chart Dir
    g_options->SetInitChartDir( *pInit_Chart_Dir );

//      Pass two working pointers for Chart Dir Dialog
    g_options->SetCurrentDirList( ChartData->GetChartDirArray() );
    ArrayOfCDI *pWorkDirArray = new ArrayOfCDI;
    g_options->SetWorkDirListPtr( pWorkDirArray );

//      Pass a ptr to MyConfig, for updates
    g_options->SetConfigPtr( pConfig );

    g_options->SetInitialSettings();

    bPrevQuilt = g_bQuiltEnable;
    bPrevFullScreenQuilt = g_bFullScreenQuilt;
    bPrevOGL = g_bopengl;

    prev_locale = g_locale;

#if defined(__WXOSX__) || defined(__WXQT__)
    bool b_restoreAIS = false;
    if( g_pAISTargetList  && g_pAISTargetList->IsShown() ){
        b_restoreAIS = true;
        g_pAISTargetList->Shutdown();
        g_pAISTargetList = NULL;
    }
#endif
/*
#ifdef __WXOSX__
       // ..For each canvas...
    for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
        ChartCanvas *cc = g_canvasArray.Item(i);
        if(cc && cc->GetMUIBar())
            cc->GetMUIBar()->Hide();  
    }

    SubmergeAllCanvasToolbars();
    g_MainToolbar->Submerge();
#endif        
*/
    g_options->SetInitialPage(options_lastPage, options_subpage );

    if(!g_bresponsive){
        g_options->lastWindowPos = options_lastWindowPos;
        if( options_lastWindowPos != wxPoint(0,0) ) {
            g_options->Move( options_lastWindowPos );
            g_options->SetSize( options_lastWindowSize );
        } else {
            g_options->Center();
        }
        if( options_lastWindowSize != wxSize(0,0) ) {
            g_options->SetSize( options_lastWindowSize );
        }
    }

          // Correct some fault in Options dialog layout logic on GTK3 by forcing a re-layout to new slightly reduced size.
#ifdef __WXGTK3__
    if( options_lastWindowSize != wxSize(0,0) )
        g_options->SetSize( options_lastWindowSize.x - 1, options_lastWindowSize.y );
#endif

    if( g_MainToolbar)
        g_MainToolbar->DisableTooltips();

    // Record current canvas config
    unsigned int last_canvasConfig = g_canvasConfig;
    wxSize cc1SizeBefore;
    if( g_canvasConfig > 0 ){
        canvasConfig *cc = g_canvasConfigArray.Item(0);
        if(cc )
            cc1SizeBefore = g_canvasArray.Item(0)->GetSize();
    }

    //  Capture the full path names and VPScale of charts currently shown in all canvases
    wxArrayString pathArray;
    double restoreScale[4];

    // ..For each canvas...
    for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
        ChartCanvas *cc = g_canvasArray.Item(i);
        if(cc){
            wxString chart_file_name;
            if( cc->GetQuiltMode() ) {
                int dbi = cc->GetQuiltRefChartdbIndex();
                chart_file_name = ChartData->GetDBChartFileName( dbi );
            } else{
                if( cc->m_singleChart )
                    chart_file_name =  cc->m_singleChart->GetFullPath();
            }

            pathArray.Add(chart_file_name);
            restoreScale[i] = cc->GetVPScale();
        }                
    }

    int rr = g_options->ShowModal();

    if( g_MainToolbar)
        g_MainToolbar->EnableTooltips();

    options_lastPage = g_options->lastPage;
    options_subpage = g_options->lastSubPage;

    options_lastWindowPos = g_options->lastWindowPos;
    options_lastWindowSize = g_options->lastWindowSize;

    GetPrimaryCanvas()->SetFocus();

#ifdef __WXGTK__
    Raise();                      // I dunno why...
#endif


    bool ret_val = false;
    rr = g_options->GetReturnCode();

    if(g_last_ChartScaleFactor != g_ChartScaleFactor)
        rr |= S52_CHANGED;

    bool b_refresh = true;
#if 0
    bool ccRightSizeChanged = false;
    if( g_canvasConfig > 0 ){
        canvasConfig *cc = g_canvasConfigArray.Item(0);
        if(cc ){
            wxSize cc1Size = cc->canvasSize;
            if(cc1Size.x != cc1SizeBefore.x)
                ccRightSizeChanged = true;
        }
    }
#endif
    if( (g_canvasConfig != last_canvasConfig) || ( rr & GL_CHANGED) ){
        UpdateCanvasConfigDescriptors();

        if( (g_canvasConfig > 0) && (last_canvasConfig == 0) )
            CreateCanvasLayout(true);    
        else
            CreateCanvasLayout();

        SendSizeEvent();

        g_pauimgr->Update();

        // We need a yield() here to pick up the size event
        // so that the toolbars will be sized correctly
        wxYield();

        // ..For each canvas...
        for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
            ChartCanvas *cc = g_canvasArray.Item(i);
            if(cc)
                cc->CreateMUIBar();  
        }

        rr |= GENERIC_CHANGED;

        if(g_bopengl)           // Force mark/waypoint icon reload
            rr |= S52_CHANGED;

        b_refresh = true;
    }

    // Here check for the case wherein the relative sizes of a multicanvas layout have been changed.
    // We do not need to reqbuild the canvases, we just need to resize whichever one is docked.

//     if( (g_canvasConfig > 0)  && ccRightSizeChanged ){
//         canvasConfig *cc = g_canvasConfigArray.Item(1);
//         if(cc ){
//             wxAuiPaneInfo& p = g_pauimgr->GetPane(g_canvasArray.Item(1));
//             wxAuiDockInfo *dockRight = g_pauimgr->FindDock(p);
//             if(dockRight)
//                 g_pauimgr->SetDockSize(dockRight, cc->canvasSize.x);
//         }
//     }

    if( rr & CONFIG_CHANGED){
        // Apply the changed canvas configs to each canvas
        // ..For each canvas...
        for(unsigned int i=0 ; i < g_canvasConfigArray.GetCount() ; i++){
            canvasConfig *cc = g_canvasConfigArray.Item(i);
            if(cc ){
                ChartCanvas *chartCanvas = cc->canvas;
                if(chartCanvas){
                    chartCanvas->ApplyCanvasConfig(cc);
                }
            }
        }
    }


    if( rr ) {
        bDBUpdateInProgress = true;
        b_refresh |= ProcessOptionsDialog( rr,  g_options->GetWorkDirListPtr() );
        ChartData->GetChartDirArray() = *(g_options->GetWorkDirListPtr()); // Perform a deep copy back to main database.
        bDBUpdateInProgress = false;
        ret_val = true;
    }

    delete pWorkDirArray;


    gFrame->Raise();
    DoChartUpdate();

    //  We set the compass size first, since that establishes the available space for the toolbar.
    SetGPSCompassScale();
    // ..For each canvas...
    for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
        ChartCanvas *cc = g_canvasArray.Item(i);
        if(cc){
            cc->GetCompass()->SetScaleFactor(g_compass_scalefactor);
            cc->UpdateCanvasControlBar();
        }
    }
    UpdateGPSCompassStatusBoxes();

    SetAllToolbarScale();
    RequestNewToolbars();

    // Change of master toolbar scale?
    bool b_masterScaleChange = false;
    if(fabs(g_MainToolbar->GetScaleFactor() - g_toolbar_scalefactor) > 0.01f)
        b_masterScaleChange = true;


    if((rr & TOOLBAR_CHANGED) || b_masterScaleChange )
        RequestNewMasterToolbar( true );

    // Inform the canvases
    if( b_masterScaleChange ){
            // ..For each canvas...
        for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
            ChartCanvas *cc = g_canvasArray.Item(i);
            if(cc ){
                cc->ProcessNewGUIScale();
            }
        }
    }

#if defined(__WXOSX__) || defined(__WXQT__)
    if( b_restoreAIS ){
        g_pAISTargetList = new AISTargetListDialog( this, g_pauimgr, g_pAIS );
        g_pAISTargetList->UpdateAISTargetList();
    }
#endif

    if(console && console->IsShown())
        console->Raise();


    if (NMEALogWindow::Get().Active())
        NMEALogWindow::Get().GetTTYWindow()->Raise();

    //  Force reload of options dialog to pick up font changes
    if(rr & FONT_CHANGED){
        delete g_options;
        g_options = NULL;
        g_pOptions = NULL;
    }

    //  Pick up chart object icon size changes (g_ChartScaleFactorExp)
    if( g_pMarkInfoDialog ) {
        g_pMarkInfoDialog->Hide();
        g_pMarkInfoDialog->Destroy();
        g_pMarkInfoDialog = NULL;
    }

#if wxUSE_XLOCALE    
    if(rr & LOCALE_CHANGED){

        g_Platform->ChangeLocale(g_locale, plocale_def_lang, &plocale_def_lang);
        ApplyLocale();
    }
#endif

    // If needed, refresh each canvas,
    // trying to reload the previously displayed chart by name as saved in pathArray
    // Also, restoring the previous chart VPScale, if possible
    if(b_refresh){
    // ..For each canvas...
        for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
            ChartCanvas *cc = g_canvasArray.Item(i);
            if(cc ){
                int index_hint = -1;
                if( i < pathArray.GetCount())
                    index_hint = ChartData->FinddbIndex( pathArray.Item(i));
                cc->canvasChartsRefresh( index_hint );
                if(index_hint != -1)
                    cc->SetVPScale( restoreScale[i] );
            }
        }
    }


    g_boptionsactive = false;

    //  If we had a config chage, then schedule a re-entry to the settings dialog
    if(rr & CONFIG_CHANGED){
        options_subpage = 3;            // Back to the "templates" page
        ScheduleSettingsDialog();
    }
    else
        options_subpage = 0;

    return ret_val;
}

bool MyFrame::ProcessOptionsDialog( int rr, ArrayOfCDI *pNewDirArray )
{
    bool b_need_refresh = false;                // Do we need a full reload?

    if( ( rr & VISIT_CHARTS )
            && ( ( rr & CHANGE_CHARTS ) || ( rr & FORCE_UPDATE ) || ( rr & SCAN_UPDATE ) ) ) {
        if(pNewDirArray){
            UpdateChartDatabaseInplace( *pNewDirArray, ( ( rr & FORCE_UPDATE ) == FORCE_UPDATE ),
                true, ChartListFileName );

            b_need_refresh = true;
        }
    }

    if(  rr & STYLE_CHANGED  ) {
        OCPNMessageBox(NULL, _("Please restart OpenCPN to activate language or style changes."),
                _("OpenCPN Info"), wxOK | wxICON_INFORMATION );
    }

    bool b_groupchange = false;
    if( ( ( rr & VISIT_CHARTS )
            && ( ( rr & CHANGE_CHARTS ) || ( rr & FORCE_UPDATE ) || ( rr & SCAN_UPDATE ) ) )
            || ( rr & GROUPS_CHANGED ) ) {
        b_groupchange = ScrubGroupArray();
        ChartData->ApplyGroupArray( g_pGroupArray );
        RefreshGroupIndices( );
    }

    if( rr & GROUPS_CHANGED || b_groupchange) {
        pConfig->DestroyConfigGroups();
        pConfig->CreateConfigGroups( g_pGroupArray );
    }

    if( rr & TIDES_CHANGED ) {
        LoadHarmonics();
    }

    //  S52_CHANGED is a byproduct of a change in the chart object render scale
    //  So, applies to RoutePoint icons also
    if( rr & S52_CHANGED){
        //  Reload Icons
        pWayPointMan->ReloadAllIcons( );
    }
    
    pConfig->UpdateSettings();

    if( g_pActiveTrack ) {
        g_pActiveTrack->SetPrecision( g_nTrackPrecision );
    }

//     if( ( bPrevQuilt != g_bQuiltEnable ) || ( bPrevFullScreenQuilt != g_bFullScreenQuilt ) ) {
//         GetPrimaryCanvas()->SetQuiltMode( g_bQuiltEnable );
//         GetPrimaryCanvas()->SetupCanvasQuiltMode();
//     }

#if 0
//TODO Not need with per-canvas CourseUp
    if( g_bCourseUp ) {
        //    Stuff the COGAvg table in case COGUp is selected
        double stuff = NAN;
        if( !std::isnan(gCog) ) stuff = gCog;
        if( g_COGAvgSec > 0 ) {
            for( int i = 0; i < g_COGAvgSec; i++ )
                COGTable[i] = stuff;
        }

        g_COGAvg = stuff;

        DoCOGSet();
    }
#endif

    g_pRouteMan->SetColorScheme(global_color_scheme);           // reloads pens and brushes

    //    Stuff the Filter tables
    double stuffcog = NAN;
    double stuffsog = NAN;
    if( !std::isnan(gCog) ) stuffcog = gCog;
    if( !std::isnan(gSog) ) stuffsog = gSog;

    for( int i = 0; i < MAX_COGSOG_FILTER_SECONDS; i++ ) {
        COGFilterTable[i] = stuffcog;
        SOGFilterTable[i] = stuffsog;
    }

    SetChartUpdatePeriod( );              // Pick up changes to skew compensator

    if(rr & GL_CHANGED){
        //    Refresh the chart display, after flushing cache.
        //      This will allow all charts to recognise new OpenGL configuration, if any
        b_need_refresh = true;
    }

    if(rr & S52_CHANGED){
        b_need_refresh = true;
    }

#ifdef ocpnUSE_GL
    if(rr & REBUILD_RASTER_CACHE){
        if(g_glTextureManager) {
            GetPrimaryCanvas()->Disable();
            g_glTextureManager->BuildCompressedCache();
            GetPrimaryCanvas()->Enable();
        }
    }
#endif

    if(g_config_display_size_mm > 0){
        g_display_size_mm = g_config_display_size_mm;
    }
    else{
        g_display_size_mm = wxMax(100, g_Platform->GetDisplaySizeMM());
    }

    for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
        ChartCanvas *cc = g_canvasArray.Item(i);
        if(cc)
            cc->SetDisplaySizeMM( g_display_size_mm );
    }

    if(g_pi_manager){
        g_pi_manager->SendBaseConfigToAllPlugIns();
        int rrt = rr & S52_CHANGED;
        g_pi_manager->SendS52ConfigToAllPlugIns( (rrt == S52_CHANGED) || (g_last_ChartScaleFactor != g_ChartScaleFactor));
    }


    if(g_MainToolbar){
        g_MainToolbar->SetAutoHide(g_bAutoHideToolbar);
        g_MainToolbar->SetAutoHideTimer(g_nAutoHideToolbar);
    }

    // Apply any needed updates to each canvas
    for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
        ChartCanvas *cc = g_canvasArray.Item(i);
        if(cc)
            cc->ApplyGlobalSettings();
    }


    //    Do a full Refresh, trying to open the last open chart
//TODO  This got move up a level.  FIX ANDROID codepath    
#if 0    
    if(b_need_refresh){
        int index_hint = ChartData->FinddbIndex( chart_file_name );
        if( -1 == index_hint )
            b_autofind = true;
        ChartsRefresh( );
    }
#endif

    //  The zoom-scale factor may have changed
    //  so, trigger a recalculation of the reference chart

    bool ztc = g_bEnableZoomToCursor;     // record the present state 
    g_bEnableZoomToCursor = false;        // since we don't want to pan to an unknown cursor position

    //  This is needed to recognise changes in zoom-scale factors
    GetPrimaryCanvas()->DoZoomCanvas(1.0001);

    g_bEnableZoomToCursor = ztc;

    g_last_ChartScaleFactor = g_ChartScaleFactor;

    return b_need_refresh;
}


wxString MyFrame::GetGroupName( int igroup )
{
    ChartGroup *pGroup = g_pGroupArray->Item( igroup - 1 );
    return pGroup->m_group_name;
}

bool MyFrame::CheckGroup( int igroup )
{
    if( igroup == 0 ) return true;              // "all charts" is always OK

    ChartGroup *pGroup = g_pGroupArray->Item( igroup - 1 );

    if( !pGroup->m_element_array.size() )   //  truly empty group is OK
        return true;

    bool b_chart_in_group = false;

    for( unsigned int j = 0; j < pGroup->m_element_array.size(); j++ ) {
        wxString element_root = pGroup->m_element_array[j]->m_element_name;

        for( unsigned int ic = 0; ic < (unsigned int) ChartData->GetChartTableEntries(); ic++ ) {
            ChartTableEntry *pcte = ChartData->GetpChartTableEntry( ic );
            wxString chart_full_path = pcte->GetFullSystemPath();

            if( chart_full_path.StartsWith( element_root ) ) {
                b_chart_in_group = true;
                break;
            }
        }

        if( b_chart_in_group ) break;
    }

    return b_chart_in_group;                           // this group is empty

}

bool MyFrame::ScrubGroupArray()
{
    //    For each group,
    //    make sure that each group element (dir or chart) references at least oneitem in the database.
    //    If not, remove the element.

    bool b_change = false;
    unsigned int igroup = 0;
    while( igroup < g_pGroupArray->GetCount() ) {
        bool b_chart_in_element = false;
        ChartGroup *pGroup = g_pGroupArray->Item( igroup );

        for( unsigned int j = 0; j < pGroup->m_element_array.size(); j++ ) {
            wxString element_root = pGroup->m_element_array[j]->m_element_name;

            for( unsigned int ic = 0; ic < (unsigned int) ChartData->GetChartTableEntries();
                    ic++ ) {
                ChartTableEntry *pcte = ChartData->GetpChartTableEntry( ic );
                wxString chart_full_path = pcte->GetFullSystemPath();

                if( chart_full_path.StartsWith( element_root ) ) {
                    b_chart_in_element = true;
                    break;
                }
            }
            
            // Explicit check to avoid removing a group containing only GSHHS
            if(!b_chart_in_element){
                wxString test_string = _T("GSHH");
                if(element_root.Upper().Contains(test_string))
                    b_chart_in_element = true;
            }

            if( !b_chart_in_element )             // delete the element
            {
                ChartGroupElement *pelement = pGroup->m_element_array[j];
                pGroup->m_element_array.RemoveAt( j );
                j--;
                delete pelement;
                b_change = true;
            }
        }

        igroup++;                                 // next group
    }

    return b_change;
}

void MyFrame::RefreshCanvasOther( ChartCanvas *ccThis )
{
    // ..For each canvas...
    for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
        ChartCanvas *cc = g_canvasArray.Item(i);
        if(cc && (cc != ccThis))
            cc->Refresh();
    }
}



// Flav: This method reloads all charts for convenience
void MyFrame::ChartsRefresh( )
{
    if( !ChartData ) return;

    OCPNPlatform::ShowBusySpinner();

    bool b_run = FrameTimer1.IsRunning();

    FrameTimer1.Stop();                  // stop other asynchronous activity
    bool b_runCOGTimer = FrameCOGTimer.IsRunning();
    FrameCOGTimer.Stop();

    // ..For each canvas...
    for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
        ChartCanvas *cc = g_canvasArray.Item(i);
        if(cc ){
            int currentIndex = cc->GetpCurrentStack()->GetCurrentEntrydbIndex();
            if(cc->GetQuiltMode()){
                currentIndex = cc->GetQuiltReferenceChartIndex();
                }
            cc->canvasChartsRefresh( currentIndex );
        }
    }


    if( b_run ) FrameTimer1.Start( TIMER_GFRAME_1, wxTIMER_CONTINUOUS );

    OCPNPlatform::HideBusySpinner();

}

void MyFrame::InvalidateAllQuilts()
{
     for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
        ChartCanvas *cc = g_canvasArray.Item(i);
        if( cc ) {
            cc->InvalidateQuilt();
            cc->SetQuiltRefChart( -1 );
            cc->m_singleChart = NULL;
        }
    }
}

bool MyFrame::UpdateChartDatabaseInplace( ArrayOfCDI &DirArray, bool b_force, bool b_prog,
        const wxString &ChartListFileName )
{
    bool b_run = FrameTimer1.IsRunning();
    FrameTimer1.Stop();                  // stop other asynchronous activity
    bool b_runCOGTimer = FrameCOGTimer.IsRunning();
        FrameCOGTimer.Stop();
    // ..For each canvas...
    for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
        ChartCanvas *cc = g_canvasArray.Item(i);
        if( cc ) {
            cc->InvalidateQuilt();
            cc->SetQuiltRefChart( -1 );
            cc->m_singleChart = NULL;
        }
    }

    ChartData->PurgeCache();

//TODO    
//     delete pCurrentStack;
//     pCurrentStack = NULL;

    OCPNPlatform::ShowBusySpinner();

    wxGenericProgressDialog *pprog = nullptr;
    if( b_prog ) {
        wxString longmsg = _("OpenCPN Chart Update");
        longmsg += _T("..........................................................................");

        pprog = new wxGenericProgressDialog();

        wxFont *qFont = GetOCPNScaledFont(_("Dialog"));
        pprog->SetFont( *qFont );

        pprog->Create( _("OpenCPN Chart Update"), longmsg, 100,
                                          gFrame, wxPD_SMOOTH | wxPD_ELAPSED_TIME | wxPD_ESTIMATED_TIME | wxPD_REMAINING_TIME );


        DimeControl( pprog );
        pprog->Show();
    }

    wxLogMessage( _T("   ") );
    wxLogMessage( _T("Starting chart database Update...") );
    wxString gshhg_chart_loc = gWorldMapLocation;
    gWorldMapLocation = wxEmptyString;
    ChartData->Update( DirArray, b_force, pprog );
    ChartData->SaveBinary(ChartListFileName);
    wxLogMessage( _T("Finished chart database Update") );
    wxLogMessage( _T("   ") );
    if( gWorldMapLocation.empty() ) { //Last resort. User might have deleted all GSHHG data, but we still might have the default dataset distributed with OpenCPN or from the package repository...
       gWorldMapLocation = gDefaultWorldMapLocation;
       gshhg_chart_loc = wxEmptyString;
    }

    if( gWorldMapLocation != gshhg_chart_loc ){
    // ..For each canvas...
        for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
            ChartCanvas *cc = g_canvasArray.Item(i);
            if( cc ) 
                cc->ResetWorldBackgroundChart();
        }
    }


    delete pprog;

    OCPNPlatform::HideBusySpinner();

    pConfig->UpdateChartDirs( DirArray );

    // Restart timers, if necessary
    if( b_run )
        FrameTimer1.Start( TIMER_GFRAME_1, wxTIMER_CONTINUOUS );
    if( b_runCOGTimer ){
           //    Restart the COG rotation timer, max frequency is 10 hz.
        int period_ms = 100;
        if( g_COGAvgSec > 0 )
            period_ms = g_COGAvgSec * 1000;
        FrameCOGTimer.Start( period_ms, wxTIMER_CONTINUOUS );
    }

    return true;
}

void MyFrame::ToggleQuiltMode( ChartCanvas *cc )
{
    if( cc ) {
        cc->ToggleCanvasQuiltMode();
#if 0
        bool cur_mode = cc->GetQuiltMode();

        if( !cc->GetQuiltMode() )
            cc->SetQuiltMode( true );
        else
            if( cc->GetQuiltMode() ) {
                cc->SetQuiltMode( false );
                g_sticky_chart = cc->GetQuiltReferenceChartIndex();
            }


        if( cur_mode != cc->GetQuiltMode() ){
            //TODO >>SetupQuiltMode();
            DoChartUpdate();
            cc->InvalidateGL();
            Refresh();
        }
        g_bQuiltEnable = cc->GetQuiltMode();
        
#ifdef USE_S57
        // Recycle the S52 PLIB so that vector charts will flush caches and re-render
        if(ps52plib)
            ps52plib->GenerateStateHash();
#endif
#endif        
    }
}


void MyFrame::ClearRouteTool()
{
    if( g_MainToolbar->GetToolbar() )
        g_MainToolbar->GetToolbar()->ToggleTool( ID_ROUTE, false );

#ifdef __OCPN__ANDROID__
        androidSetRouteAnnunciator(false);
#endif
}

void MyFrame::DoStackDown( ChartCanvas *cc )
{
    DoStackDelta( cc, -1 );
}

void MyFrame::DoStackUp( ChartCanvas *cc )
{
    DoStackDelta( cc, 1 );
}

void MyFrame::DoStackDelta( ChartCanvas *cc, int direction )
{
    if(cc){
        cc->DoCanvasStackDelta( direction );
    }
}

void MyFrame::PositionIENCToolbar()
{
    if(g_iENCToolbar){
        wxPoint posn;
        posn.x = (GetPrimaryCanvas()->GetSize().x - g_iENCToolbar->GetSize().x ) / 2;
        posn.y = 4;
        g_iENCToolbar->Move(GetPrimaryCanvas()->ClientToScreen(posn));
    }
}

// Defered initialization for anything that is not required to render the initial frame
// and takes a while to initialize.  This gets opencpn up and running much faster.
void MyFrame::OnInitTimer(wxTimerEvent& event)
{
    InitTimer.Stop();
    wxString msg;
    msg.Printf(_T("OnInitTimer...%d"), m_iInitCount);
    wxLogMessage(msg);

    switch(m_iInitCount++) {
        case 0:
        {
            if( g_MainToolbar )
                g_MainToolbar->EnableTool( ID_SETTINGS, false );

            if(g_bInlandEcdis){
                double range = GetPrimaryCanvas()->GetCanvasRangeMeters();
                double range_set = 500.;

                range = wxRound(range * 10) / 10.;

                if(range > 4000.)
                    range_set = 4000.;
                else if(range > 2000.)
                    range_set = 2000.;
                else if(range > 1600.)
                    range_set = 1600.;
                else if(range > 1200.)
                    range_set = 1200.;
                else if(range > 800.)
                    range_set = 800.;
                else
                    range_set = 500.;

                GetPrimaryCanvas()->SetCanvasRangeMeters(range_set);

            }

            // Set persistent Fullscreen mode
            g_Platform->SetFullscreen(g_bFullscreen);

            // Rebuild chart database, if necessary
            if(g_bNeedDBUpdate){
                RebuildChartDatabase();
                for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
                    ChartCanvas *cc = g_canvasArray.Item(i);
                    if(cc){
                        cc->SetGroupIndex( 0, false );  //all charts
                    }
                }

                //    As a favor to new users, poll the database and
                //    move the initial viewport so that a chart will come up.

                double clat, clon;
                if( ChartData->GetCentroidOfLargestScaleChart( &clat, &clon, CHART_FAMILY_RASTER ) ) {
                    gLat = clat;
                    gLon = clon;
                    gFrame->ClearbFollow(gFrame->GetPrimaryCanvas());
                } else {
                    if( ChartData->GetCentroidOfLargestScaleChart( &clat, &clon, CHART_FAMILY_VECTOR ) ) {
                        gLat = clat;
                        gLon = clon;
                        gFrame->ClearbFollow(gFrame->GetPrimaryCanvas());
                    }
                }

                g_bNeedDBUpdate = false;
            }

            // Load the waypoints.. both of these routines are very slow to execute which is why
            // they have been to defered until here
            pWayPointMan = new WayPointman();
            pWayPointMan->SetColorScheme( global_color_scheme );

            // Reload the ownship icon from UserIcons, if present
            for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
                ChartCanvas *cc = g_canvasArray.Item(i);
                if(cc){
                    if(cc->SetUserOwnship())
                        cc->SetColorScheme(global_color_scheme);
                }
            }

            pConfig->LoadNavObjects();
            //    Re-enable anchor watches if set in config file
            if( !g_AW1GUID.IsEmpty() ) {
                pAnchorWatchPoint1 = pWayPointMan->FindRoutePointByGUID( g_AW1GUID );
            }
            if( !g_AW2GUID.IsEmpty() ) {
                pAnchorWatchPoint2 = pWayPointMan->FindRoutePointByGUID( g_AW2GUID );
            }

            // Import Layer-wise any .gpx files from /layers directory
            wxString layerdir = g_Platform->GetPrivateDataDir();
            appendOSDirSlash( &layerdir );
            layerdir.Append( _T("layers") );

            if( wxDir::Exists( layerdir ) ) {
                wxString laymsg;
                laymsg.Printf( wxT("Getting .gpx layer files from: %s"), layerdir.c_str() );
                wxLogMessage( laymsg );
                pConfig->LoadLayers(layerdir);
            }

            break;
        }
        case 1:
            // Connect Datastreams


            for ( size_t i = 0; i < g_pConnectionParams->Count(); i++ )
            {
                ConnectionParams *cp = g_pConnectionParams->Item(i);
                if( cp->bEnabled ) {

#ifdef __unix__
                    if( cp->GetDSPort().Contains(_T("Serial"))) {
                        if( ! g_bserial_access_checked ){
                            if( !CheckSerialAccess() ){
                            }
                            g_bserial_access_checked = true;
                        }
                    }
#endif

                    dsPortType port_type = cp->IOSelect;
                    DataStream *dstr = new DataStream( g_pMUX,
                                                    cp->Type,
                                                    cp->GetDSPort(),
                                                    wxString::Format(wxT("%i"),cp->Baudrate),
                                                    port_type,
                                                    cp->Priority,
                                                    cp->Garmin
                        );
                    dstr->SetInputFilter(cp->InputSentenceList);
                    dstr->SetInputFilterType(cp->InputSentenceListType);
                    dstr->SetOutputFilter(cp->OutputSentenceList);
                    dstr->SetOutputFilterType(cp->OutputSentenceListType);
                    dstr->SetChecksumCheck(cp->ChecksumCheck);

                    cp->b_IsSetup = true;

                    g_pMUX->AddStream(dstr);
                }
            }

            console = new ConsoleCanvas( gFrame );                    // the console
            console->SetColorScheme( global_color_scheme );
            break;

        case 2:
        {
            if (m_initializing)
                break;
            m_initializing = true;
            g_pi_manager->LoadAllPlugIns( g_Platform->GetPluginDir(), true, false );

//            RequestNewToolbars();
            RequestNewMasterToolbar();
            // A Plugin (e.g. Squiddio) may have redefined some routepoint icons...
            // Reload all icons, to be sure.
            if(pWayPointMan)
                pWayPointMan->ReloadRoutepointIcons();

            if( g_MainToolbar )
                g_MainToolbar->EnableTool( ID_SETTINGS, false );

            wxString perspective;
            pConfig->SetPath( _T ( "/AUI" ) );
            pConfig->Read( _T ( "AUIPerspective" ), &perspective );

            // Make sure the perspective saved in the config file is "reasonable"
            // In particular, the perspective should have an entry for every
            // windows added to the AUI manager so far.
            // If any are not found, then use the default layout

            bool bno_load = false;

            wxArrayString name_array;
            wxStringTokenizer st(perspective, _T("|;"));
            while( st.HasMoreTokens() )
            {
                wxString s1 = st.GetNextToken();
                if(s1.StartsWith(_T("name="))){
                    wxString sc = s1.AfterFirst('=');
                    name_array.Add(sc);
                }
            }

            wxAuiPaneInfoArray pane_array_val = g_pauimgr->GetAllPanes();
            for( unsigned int i = 0; i < pane_array_val.GetCount(); i++ ) {
                wxAuiPaneInfo pane = pane_array_val.Item( i );

                // If we find a pane that is not in the perspective,
                //  then we should not load the perspective at all
                if(name_array.Index(pane.name) == wxNOT_FOUND) {
                    bno_load = true;
                    break;
                }
            }

            if( !bno_load )
                g_pauimgr->LoadPerspective( perspective, false );

#if 0
            // Undefine the canvas sizes as expressed by the loaded perspective
            for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
                ChartCanvas *cc = g_canvasArray.Item(i);
                if(cc)
                    g_pauimgr->GetPane(cc).MinSize(10,10);
            }
 
 #endif
 
            // Touch up the AUI manager
            //  Make sure that any pane width is reasonable default value
            for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
                ChartCanvas *cc = g_canvasArray.Item(i);
                if(cc){
                    wxSize frameSize = GetClientSize();
                    wxSize minSize = g_pauimgr->GetPane(cc).min_size;
                    int width = wxMax(minSize.x, frameSize.x / 10);
                    g_pauimgr->GetPane(cc).MinSize(frameSize.x * 1 / 5, frameSize.y);
                }
            }
            g_pauimgr->Update();

            //   Notify all the AUI PlugIns so that they may syncronize with the Perspective
            g_pi_manager->NotifyAuiPlugIns();
            g_pi_manager->ShowDeferredBlacklistMessages(); //  Give the use dialog on any blacklisted PlugIns
            g_pi_manager->CallLateInit();

            //  If any PlugIn implements PlugIn Charts, we need to re-run the initial chart load logic
            //  to select the correct chart as saved from the last run of the app.
            //  This will be triggered at the next DoChartUpdate()
            if( g_pi_manager->IsAnyPlugInChartEnabled() ){

                for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
                    ChartCanvas *cc = g_canvasArray.Item(i);
                    if(cc)
                        cc->SetFirstAuto(true);
                }

                b_reloadForPlugins = true;
            }

            break;
        }

        case 3:
        {
            if(g_MainToolbar){
                g_MainToolbar->SetAutoHide(g_bAutoHideToolbar);
                g_MainToolbar->SetAutoHideTimer(g_nAutoHideToolbar);
            }

#if 0   // per-canvas toolbars deprecated in MUI

            // .. for each canvas...
            for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
                ChartCanvas *cc = g_canvasArray.Item(i);
                cc->RequestNewCanvasToolbar( true );

                if(cc && cc->GetToolbarEnable()){
                    cc->GetToolbar()->SetAutoHide(g_bAutoHideToolbar);
                    cc->GetToolbar()->SetAutoHideTimer(g_nAutoHideToolbar);
                }
            }
#endif

            break;
        }

        case 4:
        {
            g_options = new options( this, -1, _("Options") );

            if( g_MainToolbar )
                g_MainToolbar->EnableTool( ID_SETTINGS, true );

            // needed to ensure that the chart window starts with keyboard focus
            SurfaceAllCanvasToolbars();

            BuildiENCToolbar( true );

            break;
        }

        default:
        {
            // Last call....
            wxLogMessage(_T("OnInitTimer...Last Call"));

            PositionIENCToolbar();

            g_bDeferredInitDone = true;

            GetPrimaryCanvas()->SetFocus();
            g_focusCanvas = GetPrimaryCanvas();

            gFrame->Raise();

            if(b_reloadForPlugins){
                DoChartUpdate();
                ChartsRefresh();
            }

            wxLogMessage(_T("OnInitTimer...Finalize Canvases"));

            for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
                ChartCanvas *cc = g_canvasArray.Item(i);
                if(cc){
                    cc->CreateMUIBar();
                    cc->CheckGroupValid();
                }
            }
// Not moved to here from line 6952 above:
//            if( g_MainToolbar )
//                g_MainToolbar->EnableTool( ID_SETTINGS, true );

            break;
        }
    }   // switch

    if(!g_bDeferredInitDone)
        InitTimer.Start( 100, wxTIMER_ONE_SHOT );

    RefreshAllCanvas( true );
}

//    Manage the application memory footprint on a periodic schedule
void MyFrame::OnMemFootTimer( wxTimerEvent& event )
{
    MemFootTimer.Stop();

    int memsize = GetApplicationMemoryUse();

    g_MemFootMB = 100;
    printf( "Memsize: %d  \n", memsize );
    // The application memory usage has exceeded the target, so try to manage it down....
    if( memsize > ( g_MemFootMB * 1000 ) ) {
        ChartCanvas *cc = GetPrimaryCanvas();
        if( ChartData && cc ) {
            //    Get a local copy of the cache info
            wxArrayPtrVoid *pCache = ChartData->GetChartCache();
            unsigned int nCache = pCache->GetCount();
            CacheEntry *pcea = new CacheEntry[nCache];

            for( unsigned int i = 0; i < nCache; i++ ) {
                CacheEntry *pce = (CacheEntry *) ( pCache->Item( i ) );
                pcea[i] = *pce;                  //ChartBase *Ch = (ChartBase *)pce->pChart;
            }

            if( nCache > 1 ) {
                //    Bubble Sort the local cache entry array
                bool b_cont = true;
                while( b_cont ) {
                    b_cont = false;
                    for( unsigned int i = 0; i < nCache - 1; i++ ) {
                        if( pcea[i].RecentTime > pcea[i + 1].RecentTime ) {
                            CacheEntry tmp = pcea[i];
                            pcea[i] = pcea[i + 1];
                            pcea[i + 1] = tmp;
                            b_cont = true;
                            break;
                        }
                    }
                }

                //    Free up some chart cache entries until the memory footprint target is realized

                unsigned int idelete = 0;                 // starting at top. which is oldest
                unsigned int idelete_max = pCache->GetCount();

                //    How many can be deleted?
                unsigned int minimum_cache = 1;
                if( cc->GetQuiltMode() ) minimum_cache = cc->GetQuiltChartCount();

                while( ( memsize > ( g_MemFootMB * 1000 ) )
                        && ( pCache->GetCount() > minimum_cache ) && ( idelete < idelete_max ) ) {
                    int memsizeb = memsize;

                    ChartData->DeleteCacheChart( (ChartBase *) pcea[idelete].pChart );
                    idelete++;
                    memsize = GetApplicationMemoryUse();
                    printf( "delete, before: %d  after: %d\n", memsizeb, memsize );
                }

            }

            delete[] pcea;
        }

    }

    MemFootTimer.Start( 9000, wxTIMER_CONTINUOUS );
}

int ut_index;

void MyFrame::CheckToolbarPosition()
{
#ifdef __WXMAC__    
    // Manage Full Screen mode on Mac Mojave 10.14
    static bool bMaximized;

    if(IsMaximized() && !bMaximized){
        bMaximized = true;
        if(g_MainToolbar){
            g_MainToolbar->SetYAuxOffset(g_MainToolbar->GetToolSize().y * 15 / 10 );
            g_MainToolbar->RestoreRelativePosition( g_maintoolbar_x, g_maintoolbar_y );
            g_MainToolbar->Realize();
        }
        PositionIENCToolbar();
    }
    else if(!IsMaximized() && bMaximized){
         bMaximized = false;
         if(g_MainToolbar){
            g_MainToolbar->SetYAuxOffset(0);
            g_MainToolbar->SetDockY( -1 );
            g_MainToolbar->RestoreRelativePosition( g_maintoolbar_x, g_maintoolbar_y );
            g_MainToolbar->Realize();
        }
        PositionIENCToolbar();
    }
#endif    
}

void MyFrame::OnFrameTimer1( wxTimerEvent& event )
{
    CheckToolbarPosition();

    if( ! g_bPauseTest && (g_unit_test_1 || g_unit_test_2) ) {
//            if((0 == ut_index) && GetQuiltMode())
//                  ToggleQuiltMode();

        // We use only one canvas for the unit tests, so far...
        ChartCanvas *cc = GetPrimaryCanvas();

        cc->m_bFollow = false;
        if( g_MainToolbar && g_MainToolbar->GetToolbar() )
            g_MainToolbar->GetToolbar()->ToggleTool( ID_FOLLOW, cc->m_bFollow );
        int ut_index_max = ( ( g_unit_test_1 > 0 ) ? ( g_unit_test_1 - 1 ) : INT_MAX );

        if( ChartData ) {
            if( cc->m_groupIndex > 0 ) {
                while (ut_index < ChartData->GetChartTableEntries() && !ChartData->IsChartInGroup( ut_index, cc->m_groupIndex ) ) {
                    ut_index++;
                }
            }
            if( ut_index < ChartData->GetChartTableEntries() ) {
                // printf("%d / %d\n", ut_index, ChartData->GetChartTableEntries());
                const ChartTableEntry *cte = &ChartData->GetChartTableEntry( ut_index );

                double clat = ( cte->GetLatMax() + cte->GetLatMin() ) / 2;
                double clon = ( cte->GetLonMax() + cte->GetLonMin() ) / 2;

                vLat = clat;
                vLon = clon;

                cc->SetViewPoint( clat, clon );

                if( cc->GetQuiltMode() ) {
                    if( cc->IsChartQuiltableRef( ut_index ) )
                        cc->SelectQuiltRefdbChart( ut_index );
                } else
                    cc->SelectdbChart( ut_index );

                double ppm; // final ppm scale to use
                if (g_unit_test_1) {  // Nur bei Command Line Start von OCPN
                    ppm = cc->GetCanvasScaleFactor() / cte->GetScale();
                    ppm /= 2;
                }
                else {
                    double rw, rh; // width, height
                    int ww, wh;    // chart window width, height

                    // width in nm
                    DistanceBearingMercator( cte->GetLatMin(), cte->GetLonMin(), cte->GetLatMin(),
                              cte->GetLonMax(), NULL, &rw );

                    // height in nm
                    DistanceBearingMercator( cte->GetLatMin(), cte->GetLonMin(), cte->GetLatMax(),
                             cte->GetLonMin(), NULL, &rh );

                    cc->GetSize( &ww, &wh );
                    ppm = wxMin(ww/(rw*1852), wh/(rh*1852)) * ( 100 - fabs( clat ) ) / 90;
                    ppm = wxMin(ppm, 1.0);
                }
                cc->SetVPScale( ppm );

                cc->ReloadVP();

                ut_index++;
                if( ut_index > ut_index_max )
                    exit(0);
            }
            else {
                _exit(0);
            }
        }
    }
    g_tick++;

//      Listen for quitflag to be set, requesting application close
    if( quitflag ) {
        wxLogMessage( _T("Got quitflag from SIGNAL") );
        FrameTimer1.Stop();
        Close();
        return;
    }

    if( bDBUpdateInProgress ) return;

    FrameTimer1.Stop();

    //  If tracking carryover was found in config file, enable tracking as soon as
    //  GPS become valid
    if(g_bDeferredStartTrack){
        if(!g_bTrackActive){
            if(bGPSValid){
                gFrame->TrackOn();
                g_bDeferredStartTrack = false;
            }
        }
        else {                                  // tracking has been manually activated
            g_bDeferredStartTrack = false;
        }
    }

//  Update and check watchdog timer for GPS data source
    gGPS_Watchdog--;
    if( gGPS_Watchdog <= 0 ) {
        bGPSValid = false;
        if( gGPS_Watchdog == 0  ){
            wxString msg;
            msg.Printf( _T("   ***GPS Watchdog timeout at Lat:%g   Lon: %g"), gLat, gLon );
            wxLogMessage(msg);
            // There is no valid fix, we need to invalidate the fix time
            m_fixtime = -1;
        }
        gSog = NAN;
        gCog = NAN;
        gRmcDate.Empty();
        gRmcTime.Empty();
    }

//  Update and check watchdog timer for Mag Heading data source
    gHDx_Watchdog--;
    if( gHDx_Watchdog <= 0 ) {
        gHdm = NAN;
        if( g_nNMEADebug && ( gHDx_Watchdog == 0 ) ) wxLogMessage(
                _T("   ***HDx Watchdog timeout...") );
    }

//  Update and check watchdog timer for True Heading data source
    gHDT_Watchdog--;
    if( gHDT_Watchdog <= 0 ) {
        g_bHDT_Rx = false;
        gHdt = NAN;
        if( g_nNMEADebug && ( gHDT_Watchdog == 0 ) ) wxLogMessage(
                _T("   ***HDT Watchdog timeout...") );
    }

    //  Update and check watchdog timer for Magnetic Variation data source
    gVAR_Watchdog--;
    if( gVAR_Watchdog <= 0 ) {
        g_bVAR_Rx = false;
        if( g_nNMEADebug && ( gVAR_Watchdog == 0 ) ) wxLogMessage(
            _T("   ***VAR Watchdog timeout...") );
    }
    //  Update and check watchdog timer for GSV, GGA and SignalK (Satellite data)
    gSAT_Watchdog--;
    if( gSAT_Watchdog <= 0 ) {
        g_bSatValid = false;
        g_SatsInView = 0;
        g_priSats = 99;
        if( g_nNMEADebug && ( gSAT_Watchdog == 0 ) ) wxLogMessage(
                _T("   ***SAT Watchdog timeout...") );
    }

    //    Build and send a Position Fix event to PlugIns
    if( g_pi_manager )
    {
        GenericPosDatEx GPSData;
        GPSData.kLat = gLat;
        GPSData.kLon = gLon;
        GPSData.kCog = gCog;
        GPSData.kSog = gSog;
        GPSData.kVar = gVar;
        GPSData.kHdm = gHdm;
        GPSData.kHdt = gHdt;
        GPSData.nSats = g_SatsInView;

        GPSData.FixTime = m_fixtime;

        g_pi_manager->SendPositionFixToAllPlugIns( &GPSData );
    }

    //   Check for anchorwatch alarms                                 // pjotrc 2010.02.15
    if( pAnchorWatchPoint1 ) {
        double dist;
        double brg;
        DistanceBearingMercator( pAnchorWatchPoint1->m_lat, pAnchorWatchPoint1->m_lon, gLat, gLon,
                &brg, &dist );
        double d = g_nAWMax;
        ( pAnchorWatchPoint1->GetName() ).ToDouble( &d );
        d = AnchorDistFix( d, AnchorPointMinDist, g_nAWMax );
        bool toofar = false;
        bool tooclose = false;
        if( d >= 0.0 ) toofar = ( dist * 1852. > d );
        if( d < 0.0 ) tooclose = ( dist * 1852 < -d );

        if( tooclose || toofar )
            AnchorAlertOn1 = true;
        else
            AnchorAlertOn1 = false;
    } else
        AnchorAlertOn1 = false;

    if( pAnchorWatchPoint2 ) {
        double dist;
        double brg;
        DistanceBearingMercator( pAnchorWatchPoint2->m_lat, pAnchorWatchPoint2->m_lon, gLat, gLon,
                &brg, &dist );

        double d = g_nAWMax;
        ( pAnchorWatchPoint2->GetName() ).ToDouble( &d );
        d = AnchorDistFix( d, AnchorPointMinDist, g_nAWMax );
        bool toofar = false;
        bool tooclose = false;
        if( d >= 0 ) toofar = ( dist * 1852. > d );
        if( d < 0 ) tooclose = ( dist * 1852 < -d );

        if( tooclose || toofar ) AnchorAlertOn2 = true;
        else
            AnchorAlertOn2 = false;
    } else
        AnchorAlertOn2 = false;

    if( (pAnchorWatchPoint1 || pAnchorWatchPoint2) && !bGPSValid )
        AnchorAlertOn1 = true;

//  Send current nav status data to log file on every half hour   // pjotrc 2010.02.09

    wxDateTime lognow = wxDateTime::Now();   // pjotrc 2010.02.09
    int hourLOC = lognow.GetHour();
    int minuteLOC = lognow.GetMinute();
    lognow.MakeGMT();
    int minuteUTC = lognow.GetMinute();
    int second = lognow.GetSecond();

    wxTimeSpan logspan = lognow.Subtract( g_loglast_time );
    if( ( logspan.IsLongerThan( wxTimeSpan( 0, 30, 0, 0 ) ) ) || ( minuteUTC == 0 )
            || ( minuteUTC == 30 ) ) {
        if( logspan.IsLongerThan( wxTimeSpan( 0, 1, 0, 0 ) ) ) {
            wxString day = lognow.FormatISODate();
            wxString utc = lognow.FormatISOTime();
            wxString navmsg = _T("LOGBOOK:  ");
            navmsg += day;
            navmsg += _T(" ");
            navmsg += utc;
            navmsg += _T(" UTC ");

            if( bGPSValid ) {
                wxString data;
                data.Printf( _T(" GPS Lat %10.5f Lon %10.5f "), gLat, gLon );
                navmsg += data;

                wxString cog;
                if( std::isnan(gCog) ) cog.Printf( _T("COG ----- ") );
                else
                    cog.Printf( _T("COG %10.5f "), gCog );

                wxString sog;
                if( std::isnan(gSog) ) sog.Printf( _T("SOG -----  ") );
                else
                    sog.Printf( _T("SOG %6.2f ") + getUsrSpeedUnit(), toUsrSpeed( gSog ) );

                navmsg += cog;
                navmsg += sog;
            } else {
                wxString data;
                data.Printf( _T(" DR Lat %10.5f Lon %10.5f"), gLat, gLon );
                navmsg += data;
            }
            wxLogMessage( navmsg );
            g_loglast_time = lognow;

            int bells = ( hourLOC % 4 ) * 2;     // 2 bells each hour
            if( minuteLOC != 0 ) bells++;       // + 1 bell on 30 minutes
            if( !bells ) bells = 8;     // 0 is 8 bells

            if( g_bPlayShipsBells && ( ( minuteLOC == 0 ) || ( minuteLOC == 30 ) ) ) {
                m_BellsToPlay = bells;
                wxCommandEvent ev(BELLS_PLAYED_EVTYPE);
                wxPostEvent(this, ev);
            }
        }
    }

    if( ShouldRestartTrack() )
        TrackDailyRestart();

    // If no alerts are on, then safe to resume sleeping
    if(g_bSleep && !AnchorAlertOn1 && !AnchorAlertOn2){
        FrameTimer1.Start( TIMER_GFRAME_1, wxTIMER_CONTINUOUS );
        return;
    }

//      Update the Toolbar Status windows and lower status bar the first time watchdog times out
    if( ( gGPS_Watchdog == 0 ) || ( gSAT_Watchdog == 0 ) ) {
        wxString sogcog( _T("SOG --- ") + getUsrSpeedUnit() + + _T("     ") + _T(" COG ---\u00B0") );
        if( GetStatusBar() ) SetStatusText( sogcog, STAT_FIELD_SOGCOG );

        gCog = 0.0;                                 // say speed is zero to kill ownship predictor
    }

//TODO
//  Not needed?
#if 0
#if !defined(__WXGTK__) && !defined(__WXQT__)
    {
        double cursor_lat, cursor_lon;
        GetPrimaryCanvas()->GetCursorLatLon( &cursor_lat, &cursor_lon );
        GetPrimaryCanvas()->SetCursorStatus(cursor_lat, cursor_lon);
    }
#endif
#endif

//      Update the chart database and displayed chart
    bool bnew_view = false;

//    Do the chart update based on the global update period currently set
//    If in COG UP mode, the chart update is handled by COG Update timer
    if( /*!g_bCourseUp &&*/ (0 != g_ChartUpdatePeriod ) ) {
        if (0 == m_ChartUpdatePeriod--) {
            bnew_view = DoChartUpdate();
            m_ChartUpdatePeriod = g_ChartUpdatePeriod;
        }
    }

    nBlinkerTick++;

    // This call sends autopilot output strings to output ports.
    bool bactiveRouteUpdate = g_pRouteMan->UpdateProgress();

    // For each canvas....
    for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
        ChartCanvas *cc = g_canvasArray.Item(i);
        if(cc){

            cc->DrawBlinkObjects();

//      Update the active route, if any, as determined above
            if( bactiveRouteUpdate ) {
        //    This RefreshRect will cause any active routepoint to blink
                if( g_pRouteMan->GetpActiveRoute() )
                    cc->RefreshRect( g_blink_rect, false );
            }

//  Force own-ship drawing parameters
            cc->SetOwnShipState( SHIP_NORMAL );

            if( cc->GetQuiltMode() ) {
                double erf = cc->GetQuiltMaxErrorFactor();
                if( erf > 0.02 )
                    cc->SetOwnShipState( SHIP_LOWACCURACY );
            } else {
                if( cc->m_singleChart ) {
                    if( cc->m_singleChart->GetChart_Error_Factor() > 0.02 )
                        cc->SetOwnShipState( SHIP_LOWACCURACY );
                }
            }

            if( !bGPSValid )
                cc->SetOwnShipState( SHIP_INVALID );

            if( bGPSValid != m_last_bGPSValid ) {
                if(!g_bopengl)
                    cc->UpdateShips();

                bnew_view = true;                  // force a full Refresh()
            }
        }
    }

    m_last_bGPSValid = bGPSValid;

    //    If any PlugIn requested dynamic overlay callbacks, force a full canvas refresh
    //    thus, ensuring at least 1 Hz. callback.
    bool brq_dynamic = false;
    if( g_pi_manager ) {
        ArrayOfPlugIns *pplugin_array = g_pi_manager->GetPlugInArray();
        for( unsigned int i = 0; i < pplugin_array->GetCount(); i++ ) {
            PlugInContainer *pic = pplugin_array->Item( i );
            if( pic->m_bEnabled && pic->m_bInitState ) {
                if( pic->m_cap_flag & WANTS_DYNAMIC_OPENGL_OVERLAY_CALLBACK ) {
                    brq_dynamic = true;
                    break;
                }
            }
        }

        if( brq_dynamic )
            bnew_view = true;
    }


    //  Make sure we get a redraw and alert sound on AnchorWatch excursions.
    if(AnchorAlertOn1 || AnchorAlertOn2)
        bnew_view = true;

    // For each canvas....
    for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
            ChartCanvas *cc = g_canvasArray.Item(i);
            if(cc){
                
                if(g_bopengl) {
#ifdef ocpnUSE_GL
					if (cc->GetglCanvas()) {
						if (m_fixtime - cc->GetglCanvas()->m_last_render_time > 0)
							bnew_view = true;
					}

                    if( AnyAISTargetsOnscreen( cc, cc->GetVP() ) )
                        bnew_view = true;

                    if(bnew_view) /* full frame in opengl mode */
                        cc->Refresh(false);
#endif
                } else {
//  Invalidate the ChartCanvas window appropriately
//    In non-follow mode, invalidate the rectangles containing the AIS targets and the ownship, etc...
//    In follow mode, if there has already been a full screen refresh, there is no need to check ownship or AIS,
//       since they will be always drawn on the full screen paint.

                if( ( !cc->m_bFollow ) || (cc->GetUpMode() != NORTH_UP_MODE) ) {
                    cc->UpdateShips();
                    cc->UpdateAIS();
                    cc->UpdateAlerts();
                } else {
                    if( !bnew_view ) {                   // There has not been a Refresh() yet.....
                        cc->UpdateAIS();
                        cc->UpdateAlerts();
                    }
                }
            }
        }
    }

    if( g_pais_query_dialog_active && g_pais_query_dialog_active->IsShown() )
        g_pais_query_dialog_active->UpdateText();

    // Refresh AIS target list every 5 seconds to avoid blinking
    if( g_pAISTargetList && ( 0 == ( g_tick % ( 5 ) ) ) )
        g_pAISTargetList->UpdateAISTargetList();

    //  Pick up any change Toolbar status displays
    UpdateGPSCompassStatusBoxes();
    UpdateAISTool();

    if( console && console->IsShown() ) {
//            console->Raise();
        console->RefreshConsoleData();
    }

    //  This little hack fixes a problem seen with some UniChrome OpenGL drivers
    //  We need a deferred resize to get glDrawPixels() to work right.
    //  So we set a trigger to generate a resize after 5 seconds....
    //  See the "UniChrome" hack elsewhere
    if( m_bdefer_resize ) {
        if( 0 == ( g_tick % ( 5 ) ) ) {
            printf( "___RESIZE\n" );
            SetSize( m_defer_size );
            g_pauimgr->Update();
            m_bdefer_resize = false;
        }
    }

#ifdef __OCPN__ANDROID__

    // Update the navobj file on a fixed schedule (5 minutes)
    // This will do nothing if the navobj.changes file is empty and clean
    if(((g_tick % g_FlushNavobjChangesTimeout) == 0) || g_FlushNavobjChanges){
        if(pConfig && pConfig->IsChangesFileDirty()){
            androidShowBusyIcon();
            wxStopWatch update_sw;
            pConfig->UpdateNavObj( true );
            wxString msg = wxString::Format(_T("OpenCPN periodic navobj update took %ld ms."), update_sw.Time());
            wxLogMessage( msg );
            qDebug() << msg.mb_str();
            g_FlushNavobjChanges = false;
            androidHideBusyIcon();
        }
    }

#endif

    if (g_unit_test_2)
        FrameTimer1.Start( TIMER_GFRAME_1*3, wxTIMER_CONTINUOUS );
    else 
        FrameTimer1.Start( TIMER_GFRAME_1, wxTIMER_CONTINUOUS );
}

double MyFrame::GetMag(double a)
{
    if(!std::isnan(gVar)){
        if((a - gVar) >360.)
            return (a - gVar - 360.);
        else
            return ((a - gVar) >= 0.) ? (a - gVar) : (a - gVar + 360.);
    }
    else{
        if((a - g_UserVar) >360.)
            return (a - g_UserVar - 360.);
        else
            return ((a - g_UserVar) >= 0.) ? (a - g_UserVar) : (a - g_UserVar + 360.);
    }
}

double MyFrame::GetMag(double a, double lat, double lon)
{
    double Variance = std::isnan( gVar ) ? g_UserVar : gVar;
    if(g_pi_manager && g_pi_manager->IsPlugInAvailable(_T("WMM"))){

        // Request variation at a specific lat/lon

        // Note that the requested value is returned sometime later in the event stream,
        // so there may be invalid data returned on the first call to this method.
        // In the case of rollover windows, the value is requested continuously, so will be correct very soon.
        wxDateTime now = wxDateTime::Now();
        SendJSON_WMM_Var_Request(lat, lon, now);
        if ( fabs(gQueryVar) < 360.0 )   // Don't use WMM variance if not updated yet
            Variance = gQueryVar;
    }
    if((a - Variance ) > 360.)
        return (a - Variance - 360.);
    else
        return ((a - Variance) >= 0.) ? (a - Variance) : (a - Variance + 360.);
}

bool MyFrame::SendJSON_WMM_Var_Request(double lat, double lon, wxDateTime date)
{
    if(g_pi_manager){
        wxJSONValue v;
        v[_T("Lat")] = lat;
        v[_T("Lon")] = lon;
        v[_T("Year")] = date.GetYear();
        v[_T("Month")] = date.GetMonth();
        v[_T("Day")] = date.GetDay();

        g_pi_manager->SendJSONMessageToAllPlugins(_T("WMM_VARIATION_REQUEST"), v);
        return true;
    }
    else
        return false;
}

void MyFrame::TouchAISActive( void )
{
    // .. for each canvas...
    for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
        ChartCanvas *cc = g_canvasArray.Item(i);
        if(cc)
            cc->TouchAISToolActive();
    }
}

void MyFrame::UpdateAISTool( void )
{
    if(!g_pAIS) return;

    // .. for each canvas...
    for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
        ChartCanvas *cc = g_canvasArray.Item(i);
        if(cc)
            cc->UpdateAISTBTool();
    }
}

//    Cause refresh of active Tide/Current data, if displayed
void MyFrame::OnFrameTCTimer( wxTimerEvent& event )
{
    // ..For each canvas...
    for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
        ChartCanvas *cc = g_canvasArray.Item(i);
        if(cc)
            cc->SetbTCUpdate( true );
    }

    RefreshAllCanvas( false );
}

//    Keep and update the Viewport rotation angle according to average COG for COGUP mode
void MyFrame::OnFrameCOGTimer( wxTimerEvent& event )
{
    // ..For each canvas...
    bool b_rotate = false;
    for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
        ChartCanvas *cc = g_canvasArray.Item(i);
        if(cc)
            b_rotate |= (cc->GetUpMode() != NORTH_UP_MODE);
    }

    if(!b_rotate){
        FrameCOGTimer.Stop();
        return;
    }


    DoCOGSet();

    //    Restart the timer, max frequency is 10 hz.
    int period_ms = 100;
    if( g_COGAvgSec > 0 )
        period_ms = g_COGAvgSec * 1000;
    FrameCOGTimer.Start( period_ms, wxTIMER_CONTINUOUS );
}

void MyFrame::DoCOGSet( void )
{
    // ..For each canvas...
    for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
        ChartCanvas *cc = g_canvasArray.Item(i);
        if(cc)
            cc->DoCanvasCOGSet();
    }

}

void RenderShadowText( wxDC *pdc, wxFont *pFont, wxString& str, int x, int y )
{
#ifdef DrawText
#undef DrawText
#define FIXIT
#endif

    wxFont oldfont = pdc->GetFont(); // save current font

    pdc->SetFont( *pFont );
    pdc->SetTextForeground( GetGlobalColor( _T("CHGRF") ) );
    pdc->SetBackgroundMode( wxTRANSPARENT );

    pdc->DrawText( str, x, y + 1 );
    pdc->DrawText( str, x, y - 1 );
    pdc->DrawText( str, x + 1, y );
    pdc->DrawText( str, x - 1, y );

    pdc->SetTextForeground( GetGlobalColor( _T("CHBLK") ) );

    pdc->DrawText( str, x, y );

    pdc->SetFont( oldfont );                  // restore last font

}

//TODO How does this relate to per-canvas rotation?
void MyFrame::UpdateRotationState( double rotation )
{
    //  If rotated manually, we switch to NORTHUP
    g_bCourseUp = false;

    if(fabs(rotation) > .001){
        SetMenubarItemState( ID_MENU_CHART_COGUP, false );
        SetMenubarItemState( ID_MENU_CHART_NORTHUP, true );
        if(m_pMenuBar){
            m_pMenuBar->SetLabel( ID_MENU_CHART_NORTHUP, _("Rotated Mode") );
        }
    }
    else{
        SetMenubarItemState( ID_MENU_CHART_COGUP, g_bCourseUp );
        SetMenubarItemState( ID_MENU_CHART_NORTHUP, !g_bCourseUp );
        if(m_pMenuBar){
            m_pMenuBar->SetLabel( ID_MENU_CHART_NORTHUP, _("North Up Mode") );
        }
    }

    UpdateGPSCompassStatusBoxes( true );
    DoChartUpdate();
}


void MyFrame::UpdateGPSCompassStatusBoxes( bool b_force_new )
{
    // ..For each canvas...
    for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
        ChartCanvas *cc = g_canvasArray.Item(i);
        if(cc)
            cc->UpdateGPSCompassStatusBox( b_force_new );
    }
}

//    Application memory footprint management

int MyFrame::GetApplicationMemoryUse( void )
{
    int memsize = -1;
#ifdef __linux__

    //    Use a contrived ps command to get the virtual memory size associated with this process
    wxWindow *fWin = wxWindow::FindFocus();

    wxArrayString outputArray;
    wxString cmd(_T("ps --no-headers -o vsize "));
    unsigned long pid = wxGetProcessId();
    wxString cmd1;
    cmd1.Printf(_T("%ld"), pid);
    cmd += cmd1;
    wxExecute(cmd, outputArray);

    if(outputArray.GetCount())
    {
        wxString s = outputArray.Item(0);
        long vtmp;
        if(s.ToLong(&vtmp))
        memsize = vtmp;
    }

    if(fWin)
    fWin->SetFocus();

#endif

#ifdef __WXMSW__
    HANDLE hProcess;
    PROCESS_MEMORY_COUNTERS pmc;

    unsigned long processID = wxGetProcessId();

    hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID );
    if( NULL == hProcess ) return 0;

    if( GetProcessMemoryInfo( hProcess, &pmc, sizeof( pmc ) ) ) {
        /*
         printf( "\tPageFaultCount: 0x%08X\n", pmc.PageFaultCount );
         printf( "\tPeakWorkingSetSize: 0x%08X\n",
         pmc.PeakWorkingSetSize );
         printf( "\tWorkingSetSize: 0x%08X\n", pmc.WorkingSetSize );
         printf( "\tQuotaPeakPagedPoolUsage: 0x%08X\n",
         pmc.QuotaPeakPagedPoolUsage );
         printf( "\tQuotaPagedPoolUsage: 0x%08X\n",
         pmc.QuotaPagedPoolUsage );
         printf( "\tQuotaPeakNonPagedPoolUsage: 0x%08X\n",
         pmc.QuotaPeakNonPagedPoolUsage );
         printf( "\tQuotaNonPagedPoolUsage: 0x%08X\n",
         pmc.QuotaNonPagedPoolUsage );
         printf( "\tPagefileUsage: 0x%08X\n", pmc.PagefileUsage );
         printf( "\tPeakPagefileUsage: 0x%08X\n",
         pmc.PeakPagefileUsage );
         */
        memsize = pmc.WorkingSetSize / 1024;
    }

    CloseHandle( hProcess );

#endif

    return memsize;
}



double MyFrame::GetBestVPScale( ChartBase *pchart )
{
    return GetPrimaryCanvas()->GetBestVPScale( pchart );
}


void MyFrame::SetChartUpdatePeriod( )
{
    //    Set the chart update period based upon chart skew and skew compensator

    g_ChartUpdatePeriod = 0;            // General default

    // In non-GL, singlele-chart mode, rotation of skewed charts is very slow
    //  So we need to use a slower update time constant to preserve adequate UI performance
    bool bskewdc = false;
        for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
            ChartCanvas *cc = g_canvasArray.Item(i);
            if(cc){
                if( !g_bopengl && !cc->GetVP().b_quilt){
                    if ( fabs(cc->GetVP().skew) > 0.0001)
                        bskewdc = true;
                }
                if(cc->m_bFollow)
                    g_ChartUpdatePeriod = 1;
            }
        }

    if (bskewdc)
        g_ChartUpdatePeriod = g_SkewCompUpdatePeriod;


    m_ChartUpdatePeriod = g_ChartUpdatePeriod;
}

void MyFrame::SetChartThumbnail( int index )
{/*
    //TODO
#if 0
    if( bDBUpdateInProgress ) return;

    if( NULL == pCurrentStack ) return;
    assert(ChartData != 0);

    if( NULL == pthumbwin ) return;

    if( NULL == cc1 ) return;

    bool bneedmove = false;

    if( index == -1 ) {
        wxRect thumb_rect_in_parent = pthumbwin->GetRect();

        pthumbwin->pThumbChart = NULL;
        pthumbwin->Show( false );
        cc1->RefreshRect( thumb_rect_in_parent, FALSE );
    }

<<<<<<< HEAD
    //    Search the no-show array
    bool b_is_in_noshow = false;
    for( unsigned int i = 0; i < g_quilt_noshow_index_array.size(); i++ ) {
        if( g_quilt_noshow_index_array[i] == selected_dbIndex ) // chart is in the noshow list
=======
    else
        if( index < pCurrentStack->nEntry ) {
            if( ( ChartData->GetCSChartType( pCurrentStack, index ) == CHART_TYPE_KAP )
                    || ( ChartData->GetCSChartType( pCurrentStack, index ) == CHART_TYPE_GEO )
                    || ( ChartData->GetCSChartType( pCurrentStack, index ) == CHART_TYPE_PLUGIN ) ) {
                ChartBase *new_pThumbChart = ChartData->OpenChartFromStack( pCurrentStack, index );
                if( new_pThumbChart )         // chart opened ok
>>>>>>> multicanvas
                {

                    ThumbData *pTD = new_pThumbChart->GetThumbData( 150, 150, gLat, gLon );
                    if( pTD ) {
                        pthumbwin->pThumbChart = new_pThumbChart;

                        pthumbwin->Resize();
                        pthumbwin->Show( true );
                        pthumbwin->Refresh( FALSE );
                        pthumbwin->Move( wxPoint( 4, 4 ) );
                        bneedmove = true;
                    }

                    else {
                        wxLogMessage(
                                _T("    chart1.cpp:SetChartThumbnail...Could not create thumbnail") );
                        pthumbwin->pThumbChart = NULL;
                        pthumbwin->Show( false );
                        cc1->Refresh( FALSE );
                    }

                } else                            // some problem opening chart
                {
                    wxString fp = ChartData->GetFullPath( pCurrentStack, index );
                    fp.Prepend( _T("    chart1.cpp:SetChartThumbnail...Could not open chart ") );
                    wxLogMessage( fp );
                    pthumbwin->pThumbChart = NULL;
                    pthumbwin->Show( false );
                    cc1->Refresh( FALSE );
                }

            } else {
                ChartBase *new_pThumbChart = ChartData->OpenChartFromStack( pCurrentStack, index,
                        THUMB_ONLY );

                pthumbwin->pThumbChart = new_pThumbChart;

                if( new_pThumbChart ) {
                    ThumbData *pTD = new_pThumbChart->GetThumbData( 200, 200, gLat, gLon );
                    if( pTD ) {
                        pthumbwin->Resize();
                        pthumbwin->Show( true );
                        pthumbwin->Refresh( true );
                        pthumbwin->Move( wxPoint( 4, 4 ) );
                        bneedmove = true;
                    } else
                        pthumbwin->Show( false );

                    cc1->Refresh( FALSE );
                }
            }

            if(bneedmove && pthumbwin){         // Adjust position to avoid bad overlap
                wxPoint pos = wxPoint(4,4);
                
                wxPoint tLocn = ClientToScreen(pos);
                wxRect tRect = wxRect(tLocn.x, tLocn.y, pthumbwin->GetSize().x, pthumbwin->GetSize().y);

                // Simplistic overlap avoidance works best when toolbar is horizontal near the top of screen.
                // Other difficult cases simply center the thumbwin on the canvas....
                if( g_MainToolbar && !g_MainToolbar->isSubmergedToGrabber()){
                    if( g_MainToolbar->GetScreenRect().Intersects( tRect ) ) {
                        wxPoint tbpos = cc1->ScreenToClient(g_MainToolbar->GetPosition());
                        pos = wxPoint(4, g_MainToolbar->GetSize().y + tbpos.y + 4);
                        tLocn = ClientToScreen(pos);
                    }
                }

                pthumbwin->Move( pos );
                
            }

        }
#endif */
}

void MyFrame::UpdateControlBar( ChartCanvas *cc )
{
    if( !cc )
        return;
    cc->UpdateCanvasControlBar();
}

void MyFrame::selectChartDisplay( int type, int family)
{
    // ..For each canvas...
    for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
        ChartCanvas *cc = g_canvasArray.Item(i);
        if(cc)
            cc->selectCanvasChartDisplay( type, family );
    }

     UpdateGlobalMenuItems(); // update the state of the menu items (checkmarks etc)
}



//----------------------------------------------------------------------------------
//      DoChartUpdate
//      Create a chartstack based on current lat/lon.
//      Return true if a Refresh(false) was called within.
//----------------------------------------------------------------------------------
bool MyFrame::DoChartUpdate( void )
{
    bool return_val = false;

    // ..For each canvas...
    for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
        ChartCanvas *cc = g_canvasArray.Item(i);
        if(cc)
            return_val |= cc->DoCanvasUpdate();
    }

    return return_val;

}

void MyFrame::MouseEvent( wxMouseEvent& event )
{
    int x, y;
    event.GetPosition( &x, &y );

}




//      Memory monitor support

#ifdef __WXMAC__
#include <mach/mach.h>
#include <mach/message.h>  // for mach_msg_type_number_t
#include <mach/kern_return.h>  // for kern_return_t
#include <mach/task_info.h>
#include <stdio.h>
#include <malloc/malloc.h>
#endif

#ifdef __WXGTK__
#include <malloc.h>
#endif

int g_lastMemTick = -1;
    
/* Return total system RAM and size of program */
/* Values returned are in kilobytes            */
bool GetMemoryStatus( int *mem_total, int *mem_used )
{
#ifdef __OCPN__ANDROID__
    return androidGetMemoryStatus( mem_total, mem_used );
#endif

#if defined(__linux__)

//      Use filesystem /proc/self/statm to determine memory status
//	Provides information about memory usage, measured in pages.  The columns are:
//	size       total program size (same as VmSize in /proc/[pid]/status)
//	resident   resident set size (same as VmRSS in /proc/[pid]/status)
//	share      shared pages (from shared mappings)
//	text       text (code)
//	lib        library (unused in Linux 2.6)
//	data       data + stack
//	dt         dirty pages (unused in Linux 2.6)

    wxTextFile file;
    wxString file_name;

    if(mem_used)
    {
        *mem_used = 0;
        file_name = _T("/proc/self/statm");
        if(file.Open(file_name))
        {
            wxString str = file.GetFirstLine();
            wxStringTokenizer tkm(str, _T(" "));
            wxString mem = tkm.GetNextToken();
            mem = tkm.GetNextToken();
            long mem_extract = 0;
            if (mem.Len()) {
                mem.ToLong(&mem_extract);
                *mem_used = mem_extract *4; // XXX assume 4K page
            }
        }
    }

    if(mem_total)
    {
        *mem_total = 0;
        wxTextFile file_info;
        file_name = _T("/proc/meminfo");
        if(file_info.Open(file_name))
        {
            bool b_found = false;
            wxString str;
            for ( str = file_info.GetFirstLine(); !file_info.Eof(); str = file_info.GetNextLine() )
            {
                wxStringTokenizer tk(str, _T(" :"));
                while ( tk.HasMoreTokens() )
                {
                    wxString token = tk.GetNextToken();
                    if(token == _T("MemTotal"))
                    {
                        wxStringTokenizer tkm(str, _T(" "));
                        wxString mem = tkm.GetNextToken();
                        long mem_extract = 0;
                        while(mem.Len())
                        {
                            mem.ToLong(&mem_extract);
                            if(mem_extract)
                            break;
                            mem = tkm.GetNextToken();
                        }

                        *mem_total = mem_extract;
                        b_found = true;
                        break;
                    }
                    else
                    break;
                }
                if(b_found)
                break;
            }
        }
    }

           struct mallinfo mi;

           mi = mallinfo();

           //printf("Total non-mmapped bytes (arena):       %d\n", mi.arena);
           //printf("# of free chunks (ordblks):            %d\n", mi.ordblks);
           //printf("# of free fastbin blocks (smblks):     %d\n", mi.smblks);
           //printf("# of mapped regions (hblks):           %d\n", mi.hblks);
           //printf("Bytes in mapped regions (hblkhd):      %d\n", mi.hblkhd);
           //printf("Max. total allocated space (usmblks):  %d\n", mi.usmblks);
           //printf("Free bytes held in fastbins (fsmblks): %d\n", mi.fsmblks);
           //printf("Total allocated space (uordblks):      %d\n", mi.uordblks / 1000);
           //printf("Total free space (fordblks):           %d\n", mi.fordblks);
           //printf("Topmost releasable block (keepcost):   %d\n", mi.keepcost);

           //printf("\n");

           if(mem_used)
               *mem_used = mi.uordblks / 1024;

           //printf("mem_used (Mb):  %d\n", *mem_used / 1024);
	   return true;
#endif /* linux */

#ifdef __WXMSW__
    HANDLE hProcess;
    PROCESS_MEMORY_COUNTERS pmc;

    unsigned long processID = wxGetProcessId();

    if( mem_used ) {
        hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID );

        if( hProcess && GetProcessMemoryInfo( hProcess, &pmc, sizeof( pmc ) ) ) {
            /*
             printf( "\tPageFaultCount: 0x%08X\n", pmc.PageFaultCount );
             printf( "\tPeakWorkingSetSize: 0x%08X\n",
             pmc.PeakWorkingSetSize );
             printf( "\tWorkingSetSize: 0x%08X\n", pmc.WorkingSetSize );
             printf( "\tQuotaPeakPagedPoolUsage: 0x%08X\n",
             pmc.QuotaPeakPagedPoolUsage );
             printf( "\tQuotaPagedPoolUsage: 0x%08X\n",
             pmc.QuotaPagedPoolUsage );
             printf( "\tQuotaPeakNonPagedPoolUsage: 0x%08X\n",
             pmc.QuotaPeakNonPagedPoolUsage );
             printf( "\tQuotaNonPagedPoolUsage: 0x%08X\n",
             pmc.QuotaNonPagedPoolUsage );
             printf( "\tPagefileUsage: 0x%08X\n", pmc.PagefileUsage );
             printf( "\tPeakPagefileUsage: 0x%08X\n",
             pmc.PeakPagefileUsage );
             */
            *mem_used = pmc.WorkingSetSize / 1024;
        }

        CloseHandle( hProcess );
    }

    if( mem_total ) {
        MEMORYSTATUSEX statex;

        statex.dwLength = sizeof( statex );

        GlobalMemoryStatusEx( &statex );
        /*
         _tprintf (TEXT("There is  %*ld percent of memory in use.\n"),
         WIDTH, statex.dwMemoryLoad);
         _tprintf (TEXT("There are %*I64d total Kbytes of physical memory.\n"),
         WIDTH, statex.ullTotalPhys/DIV);
         _tprintf (TEXT("There are %*I64d free Kbytes of physical memory.\n"),
         WIDTH, statex.ullAvailPhys/DIV);
         _tprintf (TEXT("There are %*I64d total Kbytes of paging file.\n"),
         WIDTH, statex.ullTotalPageFile/DIV);
         _tprintf (TEXT("There are %*I64d free Kbytes of paging file.\n"),
         WIDTH, statex.ullAvailPageFile/DIV);
         _tprintf (TEXT("There are %*I64d total Kbytes of virtual memory.\n"),
         WIDTH, statex.ullTotalVirtual/DIV);
         _tprintf (TEXT("There are %*I64d free Kbytes of virtual memory.\n"),
         WIDTH, statex.ullAvailVirtual/DIV);
         */

        *mem_total = statex.ullTotalPhys / 1024;
    }
    return true;
#endif

#ifdef __WXMAC__

    if(g_tick != g_lastMemTick){
      malloc_zone_pressure_relief(NULL, 0);

      int bytesInUse = 0;
      int blocksInUse = 0;
      int sizeAllocated = 0;

        malloc_statistics_t stats;
        stats.blocks_in_use = 0;
        stats.size_in_use = 0;
        stats.max_size_in_use = 0;
        stats.size_allocated = 0;
        malloc_zone_statistics(NULL, &stats);
        bytesInUse += stats.size_in_use;
        blocksInUse += stats.blocks_in_use;
        sizeAllocated += stats.size_allocated;

        g_memUsed = sizeAllocated >> 10;

        //printf("mem_used (Mb):  %d   %d \n", g_tick, g_memUsed / 1024);
        g_lastMemTick = g_tick;
    }

    if(mem_used)
       *mem_used = g_memUsed;
    if(mem_total) {
        *mem_total = 4000;
        FILE * fpIn = popen("sysctl -n hw.memsize", "r");
        if (fpIn)
        {
            double pagesUsed = 0.0, totalPages = 0.0;
            char buf[64];
            if(fgets(buf, sizeof(buf), fpIn) != NULL)
            {
                *mem_total = atol(buf) >> 10;
            }
        }
    }

    return true;
#endif

    if (mem_used)
	*mem_used = 0;
    if (mem_total)
	*mem_total = 0;
    return false;
}

void MyFrame::DoPrint( void )
{
    if( NULL == g_printData ) {
        g_printData = new wxPrintData;
        g_printData->SetOrientation( wxLANDSCAPE );
        g_pageSetupData = new wxPageSetupDialogData;
    }

    wxPrintDialogData printDialogData( *g_printData );
    printDialogData.EnablePageNumbers( false );

    wxPrinter printer( &printDialogData );

    MyPrintout printout( wxT("Chart Print") );

    //  In OperGL mode, make the bitmap capture of the screen before the print method starts,
    //  so as to be sure the "Abort..." dialog does not appear on the image
    if(g_bopengl)
        printout.GenerateGLbmp( );

    if( !printer.Print( this, &printout, true ) ) {
        if( wxPrinter::GetLastError() == wxPRINTER_ERROR ) OCPNMessageBox(NULL,
                _("There was a problem printing.\nPerhaps your current printer is not set correctly?"),
#ifdef __WXOSX__
                _T("OpenCPN"), wxOK | wxICON_EXCLAMATION );
#else
                _T("OpenCPN"), wxOK );
#endif
//        else
//            OCPNMessageBox(_T("Print Cancelled"), _T("OpenCPN"), wxOK);
    } else {
        ( *g_printData ) = printer.GetPrintDialogData().GetPrintData();
    }

// Pass two printout objects: for preview, and possible printing.
    /*
     wxPrintDialogData printDialogData(* g_printData);
     wxPrintPreview *preview = new wxPrintPreview(new MyPrintout, new MyPrintout, & printDialogData);
     if (!preview->Ok())
     {
     delete preview;
     OCPNMessageBox(_T("There was a problem previewing.\nPerhaps your current printer is not set correctly?"), _T("Previewing"), wxOK);
     return;
     }

     wxPreviewFrame *frame = new wxPreviewFrame(preview, this, _T("Demo Print Preview"), wxPoint(100, 100), wxSize(600, 650));
     frame->Centre(wxBOTH);
     frame->Initialize();
     frame->Show();
     */

#ifdef __WXGTK__
    SurfaceAllCanvasToolbars();
    GetPrimaryCanvas()->SetFocus();
    Raise();                      // I dunno why...
#endif
}

wxDateTime gTimeSource;

void MyFrame::OnEvtPlugInMessage( OCPN_MsgEvent & event )
{
    wxString message_ID = event.GetID();
    wxString message_JSONText = event.GetJSONText();

    //  We are free to use or ignore any or all of the PlugIn messages flying thru this pipe tee.

    //  We can possibly use the estimated magnetic variation if WMM_pi is present and active
    //  and we have no other source of Variation
    if(!g_bVAR_Rx)
    {
        if(message_ID == _T("WMM_VARIATION_BOAT"))
        {

        // construct the JSON root object
            wxJSONValue  root;
        // construct a JSON parser
            wxJSONReader reader;

        // now read the JSON text and store it in the 'root' structure
        // check for errors before retreiving values...
            int numErrors = reader.Parse( message_JSONText, &root );
            if ( numErrors > 0 )  {
//              const wxArrayString& errors = reader.GetErrors();
                return;
            }

            // get the DECL value from the JSON message
            wxString decl = root[_T("Decl")].AsString();
            double decl_val;
            decl.ToDouble(&decl_val);

            gVar = decl_val;
        }
    }

    if(message_ID == _T("WMM_VARIATION"))
    {

        // construct the JSON root object
        wxJSONValue  root;
        // construct a JSON parser
        wxJSONReader reader;

        // now read the JSON text and store it in the 'root' structure
        // check for errors before retreiving values...
        int numErrors = reader.Parse( message_JSONText, &root );
        if ( numErrors > 0 )  {
            //              const wxArrayString& errors = reader.GetErrors();
            return;
        }

        // get the DECL value from the JSON message
        wxString decl = root[_T("Decl")].AsString();
        double decl_val;
        decl.ToDouble(&decl_val);

        gQueryVar = decl_val;
    }
    
    if(message_ID == _T("GRIB_TIMELINE"))
    {
        wxJSONReader r;
        wxJSONValue v;
        r.Parse(message_JSONText, &v);
        if (v[_T("Day")].AsInt() == -1)
            gTimeSource = wxInvalidDateTime;
        else
            gTimeSource.Set (v[_T("Day")].AsInt(), (wxDateTime::Month)v[_T("Month")].AsInt(), 
                    v[_T("Year")].AsInt(), v[_T("Hour")].AsInt(), v[_T("Minute")].AsInt(), 
                    v[_T("Second")].AsInt());
    }
    if(message_ID == _T("OCPN_TRACK_REQUEST"))
    {
        wxJSONValue  root;
        wxJSONReader reader;
        wxString trk_id = wxEmptyString;

        int numErrors = reader.Parse( message_JSONText, &root );
        if ( numErrors > 0 )
            return;

        if(root.HasMember(_T("Track_ID")))
            trk_id = root[_T("Track_ID")].AsString();

        wxJSONValue v;
        v[_T("Track_ID")] = trk_id;
        for(TrackList::iterator it = pTrackList->begin(); it != pTrackList->end(); it++)
        {
            wxString name = wxEmptyString;
            if((*it)->m_GUID == trk_id)
            {
                name = (*it)->GetName();
                if(name.IsEmpty())
                {
                    TrackPoint *rp = (*it)->GetPoint( 0 );
                    if( rp && rp->GetCreateTime().IsValid() )
                        name = rp->GetCreateTime().FormatISODate() + _T(" ") + rp->GetCreateTime().FormatISOTime();
                    else
                        name = _("(Unnamed Track)");
                }

/*                To avoid memory problems send a single trackpoint. It's up to the plugin to collect the data. */
                int i = 1;
                v[_T("error")] = false;
                v[_T("TotalNodes")] = (*it)->GetnPoints();
                for(int j = 0; j< (*it)->GetnPoints(); j++)
                {
                    TrackPoint *tp = (*it)->GetPoint(j);
                    v[_T("lat")] = tp->m_lat;
                    v[_T("lon")] = tp->m_lon;
                    v[_T("NodeNr")] = i;
                    i++;
                    wxString msg_id( _T("OCPN_TRACKPOINTS_COORDS") );
                    g_pi_manager->SendJSONMessageToAllPlugins( msg_id, v );
                }
                return;
            }
            v[_T("error")] = true;

            wxString msg_id( _T("OCPN_TRACKPOINTS_COORDS") );
            g_pi_manager->SendJSONMessageToAllPlugins( msg_id, v );
        }
    }
    else if(message_ID == _T("OCPN_ROUTE_REQUEST"))
    {
        wxJSONValue  root;
        wxJSONReader reader;
        wxString guid = wxEmptyString;

        int numErrors = reader.Parse( message_JSONText, &root );
        if ( numErrors > 0 )  {
            return;
        }

        if(root.HasMember(_T("GUID")))
            guid = root[_T("GUID")].AsString();

        wxJSONValue v;
        v[_T("GUID")] = guid;
        for(RouteList::iterator it = pRouteList->begin(); it != pRouteList->end(); it++)
        {
            wxString name = wxEmptyString;

            if((*it)->m_GUID == guid)
            {
                name = (*it)->m_RouteNameString;
                if(name.IsEmpty())
                    name = _("(Unnamed Route)");

                v[_T("Name")] = name;
                v[_T("error")] = false;
                wxJSONValue w;
                int i = 0;
                for(RoutePointList::iterator itp = (*it)->pRoutePointList->begin(); itp != (*it)->pRoutePointList->end(); itp++)
                {
                    w[i][_T("lat")] = (*itp)->m_lat;
                    w[i][_T("lon")] = (*itp)->m_lon;
                    w[i][_T("Name")] = (*itp)->GetName();
                    w[i][_T("Description")] = (*itp)->GetDescription();
                    w[i][_T("GUID")] = (*itp)->m_GUID;
                    w[i][_T("ArrivalRadius")] = (*itp)->GetWaypointArrivalRadius();
                    wxHyperlinkListNode *node = (*itp)->m_HyperlinkList->GetFirst();
                    if(node)
                    {
                        int n = 1;
                        while(node)
                        {
                            Hyperlink *httpLink = node->GetData();
                            v[i][_T("WPLink")+wxString::Format(_T("%d"),n)] = httpLink->Link;
                            v[i][_T("WPLinkDesciption")+wxString::Format(_T("%d"),n++)] = httpLink->DescrText;
                            node = node->GetNext();
                        }
                    }
                    i++;
                }
                v[_T("waypoints")] = w;
                wxString msg_id( _T("OCPN_ROUTE_RESPONSE") );
                g_pi_manager->SendJSONMessageToAllPlugins( msg_id, v );
                return;
            }
        }

        v[_T("error")] = true;

        wxString msg_id( _T("OCPN_ROUTE_RESPONSE") );
        g_pi_manager->SendJSONMessageToAllPlugins( msg_id, v );
    }
    else if(message_ID == _T("OCPN_ROUTELIST_REQUEST"))
    {
        wxJSONValue  root;
        wxJSONReader reader;
        bool route = true;

        int numErrors = reader.Parse( message_JSONText, &root );
        if ( numErrors > 0 )
            return;

        if(root.HasMember(_T("mode")))
        {
            wxString str = root[_T("mode")].AsString();
            if( str == _T("Track")) route = false;

            wxJSONValue v; int i = 1;
            if(route) {
                for(RouteList::iterator it = pRouteList->begin(); it != pRouteList->end(); it++)
                {
                    wxString name = (*it)->m_RouteNameString;
                    if(name.IsEmpty())
                        name = _("(Unnamed Route)");

                    v[i][_T("error")] = false;
                    v[i][_T("name")] = name;
                    v[i][_T("GUID")] = (*it)->m_GUID;
                    v[i][_T("active")] = (*it)->IsActive();
                    i++;
                }
            } else { // track
                for(TrackList::iterator it = pTrackList->begin(); it != pTrackList->end(); it++)
                {
                    wxString name = (*it)->GetName();
                    if(name.IsEmpty())
                    {
                        TrackPoint *tp = (*it)->GetPoint( 0 );
                        if( tp && tp->GetCreateTime().IsValid() )
                            name = tp->GetCreateTime().FormatISODate() + _T(" ")
                                + tp->GetCreateTime().FormatISOTime();
                        else
                            name = _("(Unnamed Track)");
                    }
                    v[i][_T("error")] = false;
                    v[i][_T("name")] = name;
                    v[i][_T("GUID")] = (*it)->m_GUID;
                    v[i][_T("active")] = g_pActiveTrack == (*it);
                    i++;

                }
            }
            wxString msg_id( _T("OCPN_ROUTELIST_RESPONSE") );
            g_pi_manager->SendJSONMessageToAllPlugins( msg_id, v );
        }
        else
        {
            wxJSONValue v;
            v[0][_T("error")] = true;
            wxString msg_id( _T("OCPN_ROUTELIST_RESPONSE") );
            g_pi_manager->SendJSONMessageToAllPlugins( msg_id, v );
        }
    }
    else if(message_ID == _T("OCPN_ACTIVE_ROUTELEG_REQUEST"))
    {
        wxJSONValue v;
        v[0][_T("error")] = true;
        if( g_pRouteMan->GetpActiveRoute() )
        {
            if( g_pRouteMan->m_bDataValid )
            {
                v[0][_T("error")] = false;
                v[0][_T("range")] = g_pRouteMan->GetCurrentRngToActivePoint();
                v[0][_T("bearing")] = g_pRouteMan->GetCurrentBrgToActivePoint();
                v[0][_T("XTE")] = g_pRouteMan->GetCurrentXTEToActivePoint();
                v[0][_T("active_route_GUID")] = g_pRouteMan->GetpActiveRoute()->GetGUID();
                v[0][_T("active_waypoint_lat")] = g_pRouteMan->GetpActiveRoute()->m_pRouteActivePoint->GetLatitude();
                v[0][_T("active_waypoint_lon")] = g_pRouteMan->GetpActiveRoute()->m_pRouteActivePoint->GetLongitude();
            }
        }
        wxString msg_id( _T("OCPN_ACTIVE_ROUTELEG_RESPONSE") );
        g_pi_manager->SendJSONMessageToAllPlugins( msg_id, v );
    }

}

void MyFrame::OnEvtTHREADMSG( OCPN_ThreadMessageEvent & event )
{
    wxLogMessage( wxString(event.GetSString().c_str(), wxConvUTF8 ));
}


bool MyFrame::EvalPriority(const wxString & message, DataStream *pDS )
{
    bool bret = true;
    wxString msg_type = message.Mid(1, 5);

    wxString stream_name;
    int stream_priority = 0;
    if( pDS ){
        stream_priority = pDS->GetPriority();
        stream_name = pDS->GetPort();
    }

    //  If the message type has never been seen before...
    if( NMEA_Msg_Hash.find( msg_type ) == NMEA_Msg_Hash.end() ) {
        NMEA_Msg_Container *pcontainer = new NMEA_Msg_Container;
        pcontainer-> current_priority = -1;     //  guarantee to execute the next clause
        pcontainer->stream_name = stream_name;
        pcontainer->receipt_time = wxDateTime::Now();

        NMEA_Msg_Hash[msg_type] = pcontainer;
    }

    NMEA_Msg_Container *pcontainer = NMEA_Msg_Hash[msg_type];
    wxString old_port = pcontainer->stream_name;

    int old_priority = pcontainer->current_priority;

    //  If the message has been seen before, and the priority is greater than or equal to current priority,
    //  then simply update the record
    if( stream_priority >= pcontainer->current_priority )
    {
        pcontainer->receipt_time = wxDateTime::Now();
        pcontainer-> current_priority = stream_priority;
        pcontainer->stream_name = stream_name;

        bret = true;
    }

    //  If the message has been seen before, and the priority is less than the current priority,
    //  then if the time since the last recorded message is greater than GPS_TIMEOUT_SECONDS
    //  then update the record with the new priority and stream.
    //  Otherwise, ignore the message as too low a priority
    else
    {
        if( (wxDateTime::Now().GetTicks() - pcontainer->receipt_time.GetTicks()) > GPS_TIMEOUT_SECONDS )
        {
            pcontainer->receipt_time = wxDateTime::Now();
            pcontainer-> current_priority = stream_priority;
            pcontainer->stream_name = stream_name;

            bret = true;
        }
        else
            bret = false;
    }

    wxString new_port = pcontainer->stream_name;

    //  If the data source or priority has changed for this message type, emit a log entry
    if (pcontainer->current_priority != old_priority || new_port != old_port )
    {
         wxString logmsg = wxString::Format(_T("Changing NMEA Datasource for %s to %s (Priority: %i)"),
                                            msg_type.c_str(),
                                            new_port.c_str(),
                                            pcontainer->current_priority);
         wxLogMessage(logmsg );

         if (NMEALogWindow::Get().Active())
         {
             wxDateTime now = wxDateTime::Now();
             wxString ss = now.FormatISOTime();
             ss.Append( _T(" ") );
             ss.Append( logmsg );
             ss.Prepend( _T("<RED>") );

             NMEALogWindow::Get().Add(ss);
             NMEALogWindow::Get().Refresh(false);
         }
    }
    return bret;
}

static void UpdatePositionCalculatedSogCog()                                                                         
{
    wxDateTime now = wxDateTime::Now();
    if( last_own_ship_sog_cog_calc_ts.IsValid() ) {
        wxLongLong time_diff = now.Subtract(last_own_ship_sog_cog_calc_ts).GetMilliseconds();
        if( time_diff / 1000 >= g_own_ship_sog_cog_calc_damp_sec ) {
            double brg, dist;
            DistanceBearingMercator( gLat, gLon, last_own_ship_sog_cog_calc_lat, last_own_ship_sog_cog_calc_lon, &brg, &dist );
            double tSog = dist / (time_diff.ToDouble() / 3600000.);

            // Guard against really fast (i.e. non-sense VDR playback speed) data updates with slow averaging constant
            if(tSog < 100.){
                gCog = brg;
                gSog = tSog;
            }

            last_own_ship_sog_cog_calc_lat = gLat;
            last_own_ship_sog_cog_calc_lon = gLon;
            last_own_ship_sog_cog_calc_ts = now;
        }
    } else {
        last_own_ship_sog_cog_calc_lat = gLat;
        last_own_ship_sog_cog_calc_lon = gLon;
        last_own_ship_sog_cog_calc_ts = now;
    }
}

void MyFrame::setSatelitesInView(int no)
{
    wxLogDebug(wxString::Format(_T("SatsInView: %d"), no));
    g_SatsInView = no;
    gSAT_Watchdog = sat_watchdog_timeout_ticks;
    g_bSatValid = true;
}

static bool ParsePosition(const LATLONG &Position)
{
    bool ll_valid = true;
    double llt = Position.Latitude.Latitude;
    if( !std::isnan(llt) )
    {
        int lat_deg_int = (int) ( llt / 100 );
        double lat_deg = lat_deg_int;
        double lat_min = llt - ( lat_deg * 100 );
        gLat = lat_deg + ( lat_min / 60. );
        if( Position.Latitude.Northing == South )
            gLat = -gLat;
    }
    else
        ll_valid = false;
    
    double lln = Position.Longitude.Longitude;
    if( !std::isnan(lln) )
    {
        int lon_deg_int = (int) ( lln / 100 );
        double lon_deg = lon_deg_int;
        double lon_min = lln - ( lon_deg * 100 );
        gLon = lon_deg + ( lon_min / 60. );
        if( Position.Longitude.Easting == West )
            gLon = -gLon;
    }
    else
        ll_valid = false;

    if( ll_valid && g_own_ship_sog_cog_calc ) {
        UpdatePositionCalculatedSogCog();
    }

    return ll_valid;
}

void MyFrame::OnEvtOCPN_NMEA( OCPN_DataStreamEvent & event )
{
    wxString sfixtime;
    bool pos_valid = false, cog_sog_valid = false;
    bool bis_recognized_sentence = true;

    wxString str_buf = event.ProcessNMEA4Tags();

    if( g_nNMEADebug && ( g_total_NMEAerror_messages < g_nNMEADebug ) )
    {
        g_total_NMEAerror_messages++;
        wxString msg( _T("MEH.NMEA Sentence received...") );
        msg.Append( str_buf );
        wxLogMessage( msg );
    }

    //  The message must be at least reasonably formed...
    if( (str_buf[0] != '$')  &&  (str_buf[0] != '!') )
        return;

    if( event.GetStream() )
    {
        if(!event.GetStream()->ChecksumOK(event.GetNMEAString()) )
        {
            if( g_nNMEADebug && ( g_total_NMEAerror_messages < g_nNMEADebug ) )
            {
                g_total_NMEAerror_messages++;
                wxString msg( _T(">>>>>>NMEA Sentence Checksum Bad...") );
                msg.Append( str_buf );
                wxLogMessage( msg );
            }
            return;
        }
    }

    bool b_accept = EvalPriority( str_buf, event.GetStream() );
    if( !b_accept )
        return;

    m_NMEA0183 << str_buf;

    if( m_NMEA0183.PreParse() )
    {
        wxString IDs[] = {_T("RMC"), _T("HDT"), _T("HDG"), _T("HDM"),
                          _T("VTG"), _T("GSV"), _T("GGA"), _T("GLL")};
        enum {RMC, HDT, HDG, HDM, VTG, GSV, GGA, GLL, ID_NUM };

        int id;
        int num = g_bUseGLL ? ID_NUM : GLL;
        for(id=0; id<num; id++)
            if( m_NMEA0183.LastSentenceIDReceived == IDs[id] )
                break;

        if(id == num) // avoid parsing if we won't use it
            return;

        if( m_NMEA0183.Parse() )
        {
            switch(id)
            {
            case RMC:
                if( m_NMEA0183.Rmc.IsDataValid == NTrue )
                {
                    pos_valid = ParsePosition(m_NMEA0183.Rmc.Position);

                    // course is not valid in this case
                    // but also my gps occasionally outputs RMC
                    // messages with valid lat and lon but
                    // 0.0 for speed and course which messes up the filter
                    if(!g_own_ship_sog_cog_calc && m_NMEA0183.Rmc.SpeedOverGroundKnots > 0) {
                        gSog = m_NMEA0183.Rmc.SpeedOverGroundKnots;
                        gCog = m_NMEA0183.Rmc.TrackMadeGoodDegreesTrue;
                        cog_sog_valid = true;
                    }

                    // Any device sending VAR=0.0 can be assumed to not really know 
                    // what the actual variation is, so in this case we use WMM if available
                    if( (!std::isnan(m_NMEA0183.Rmc.MagneticVariation)) && 
                              0.0 != m_NMEA0183.Rmc.MagneticVariation )
                    {
                        if (m_NMEA0183.Rmc.MagneticVariationDirection == East)
                            gVar = m_NMEA0183.Rmc.MagneticVariation;
                        else
                            if (m_NMEA0183.Rmc.MagneticVariationDirection == West)
                                gVar = -m_NMEA0183.Rmc.MagneticVariation;

                        g_bVAR_Rx = true;
                        gVAR_Watchdog = gps_watchdog_timeout_ticks;
                    }

                    sfixtime = m_NMEA0183.Rmc.UTCTime;
                    gRmcTime = sfixtime;
                    gRmcDate = m_NMEA0183.Rmc.Date;
                }
                break;

            case HDT:
                gHdt = m_NMEA0183.Hdt.DegreesTrue;
                if( !std::isnan(m_NMEA0183.Hdt.DegreesTrue) )
                {
                    g_bHDT_Rx = true;
                    gHDT_Watchdog = gps_watchdog_timeout_ticks;
                }
                break;

            case HDG:
                gHdm = m_NMEA0183.Hdg.MagneticSensorHeadingDegrees;
                if( !std::isnan(m_NMEA0183.Hdg.MagneticSensorHeadingDegrees) )
                    gHDx_Watchdog = gps_watchdog_timeout_ticks;

                // Any device sending VAR=0.0 can be assumed to not really know 
                // what the actual variation is, so in this case we use WMM if available
                if( (!std::isnan(m_NMEA0183.Hdg.MagneticVariationDegrees)) &&
                     0.0 != m_NMEA0183.Hdg.MagneticVariationDegrees )
                {
                    if( m_NMEA0183.Hdg.MagneticVariationDirection == East )
                        gVar = m_NMEA0183.Hdg.MagneticVariationDegrees;
                    else if( m_NMEA0183.Hdg.MagneticVariationDirection == West )
                        gVar = -m_NMEA0183.Hdg.MagneticVariationDegrees;

                    g_bVAR_Rx = true;
                    gVAR_Watchdog = gps_watchdog_timeout_ticks;
                }
                break;

            case HDM:
                gHdm = m_NMEA0183.Hdm.DegreesMagnetic;
                if( !std::isnan(m_NMEA0183.Hdm.DegreesMagnetic) )
                    gHDx_Watchdog = gps_watchdog_timeout_ticks;
                break;

            case VTG:
                // should we allow either Sog or Cog but not both to be valid?
                if( !g_own_ship_sog_cog_calc && !std::isnan(m_NMEA0183.Vtg.SpeedKnots) )
                    gSog = m_NMEA0183.Vtg.SpeedKnots;
                if( !g_own_ship_sog_cog_calc && !std::isnan(m_NMEA0183.Vtg.TrackDegreesTrue) )
                    gCog = m_NMEA0183.Vtg.TrackDegreesTrue;
                if( !g_own_ship_sog_cog_calc && !std::isnan(m_NMEA0183.Vtg.SpeedKnots) &&
                    !std::isnan(m_NMEA0183.Vtg.TrackDegreesTrue) ) {
                    gCog = m_NMEA0183.Vtg.TrackDegreesTrue;
                    cog_sog_valid = true;
                    gGPS_Watchdog = gps_watchdog_timeout_ticks;
                }
                break;

            case GSV:
                if (g_priSats >= 4) {
                    if (m_NMEA0183.Gsv.MessageNumber == 1) {
                        // Some GNSS print SatsInView in message #1 only
                        setSatelitesInView (m_NMEA0183.Gsv.SatsInView);
                        g_priSats = 4;
                    }
                }
                break;

            case GGA:
                if( m_NMEA0183.Gga.GPSQuality > 0 )
                {
                    pos_valid = ParsePosition(m_NMEA0183.Gga.Position);
                    sfixtime = m_NMEA0183.Gga.UTCTime;
                    if (g_priSats >= 1) {
                        setSatelitesInView(m_NMEA0183.Gga.NumberOfSatellitesInUse);
                        g_priSats = 1;
                    }
                }
                break;

            case GLL:
                if( m_NMEA0183.Gll.IsDataValid == NTrue )
                {
                    pos_valid = ParsePosition(m_NMEA0183.Gll.Position);
                    sfixtime = m_NMEA0183.Gll.UTCTime;  
                }
                break;
            }

            if(pos_valid)
            {
                gGPS_Watchdog = gps_watchdog_timeout_ticks;
                wxDateTime now = wxDateTime::Now();
                m_fixtime = now.GetTicks();
            }

        } else if( g_nNMEADebug ) {
            wxString msg( _T("   ") );
            msg.Append( m_NMEA0183.ErrorMessage );
            msg.Append( _T(" : ") );
            msg.Append( str_buf );
            wxLogMessage( msg );
        }
    }
        //      Process ownship (AIVDO) messages from any source
    else if(str_buf.Mid( 1, 5 ).IsSameAs( _T("AIVDO") ) )
    {
        GenericPosDatEx gpd;
        AIS_Error nerr = AIS_GENERIC_ERROR;
        if(g_pAIS)
            nerr = g_pAIS->DecodeSingleVDO(str_buf, &gpd, &m_VDO_accumulator);

        if(nerr == AIS_NoError)
        {
            if( !std::isnan(gpd.kLat) )
                gLat = gpd.kLat;
            if( !std::isnan(gpd.kLon) )
                gLon = gpd.kLon;

            if( !g_own_ship_sog_cog_calc ) {
                gCog = gpd.kCog;
                gSog = gpd.kSog;
            } else {
                UpdatePositionCalculatedSogCog();
            }
            cog_sog_valid = true;

            if( !std::isnan(gpd.kHdt) )
            {
                gHdt = gpd.kHdt;
                g_bHDT_Rx = true;
                gHDT_Watchdog = gps_watchdog_timeout_ticks;
            }

            if( !std::isnan(gpd.kLat) && !std::isnan(gpd.kLon) )
            {
                gGPS_Watchdog = gps_watchdog_timeout_ticks;
                wxDateTime now = wxDateTime::Now();
                m_fixtime = now.GetTicks();

                pos_valid = true;
            }
        }
        else
        {
            if( g_nNMEADebug && ( g_total_NMEAerror_messages < g_nNMEADebug ) )
            {
                g_total_NMEAerror_messages++;
                wxString msg( _T("   Invalid AIVDO Sentence...") );
                msg.Append( str_buf );
                wxLogMessage( msg );
            }
        }
    }
    else
    {
        bis_recognized_sentence = false;
        if( g_nNMEADebug && ( g_total_NMEAerror_messages < g_nNMEADebug ) )
        {
            g_total_NMEAerror_messages++;
            wxString msg( _T("   Unrecognized NMEA Sentence...") );
            msg.Append( str_buf );
            wxLogMessage( msg );
        }
    }

    if( g_own_ship_sog_cog_calc )
        cog_sog_valid = true;

    if( bis_recognized_sentence ) PostProcessNMEA( pos_valid, cog_sog_valid, sfixtime );
}

void MyFrame::PostProcessNMEA( bool pos_valid, bool cog_sog_valid, const wxString &sfixtime )
{
    if(cog_sog_valid) {
        //    Maintain average COG for Course Up Mode
        if( !std::isnan(gCog) ) {
            if( g_COGAvgSec > 0 ) {
                //    Make a hole
                for( int i = g_COGAvgSec - 1; i > 0; i-- )
                    COGTable[i] = COGTable[i - 1];
                COGTable[0] = gCog;

                double sum = 0., count=0;
                for( int i = 0; i < g_COGAvgSec; i++ ) {
                    double adder = COGTable[i];
                    if(std::isnan(adder))
                        continue;

                    if( fabs( adder - g_COGAvg ) > 180. ) {
                        if( ( adder - g_COGAvg ) > 0. ) adder -= 360.;
                        else
                            adder += 360.;
                    }

                    sum += adder;
                    count++;
                }
                sum /= count;

                if( sum < 0. ) sum += 360.;
                else
                    if( sum >= 360. ) sum -= 360.;

                g_COGAvg = sum;
            }
            else
                g_COGAvg = gCog;
        }

        FilterCogSog();
    }

    //    If gSog is greater than some threshold, we determine that we are "cruising"
    if( gSog > 3.0 ) g_bCruising = true;

    //    Here is the one place we try to create gHdt from gHdm and gVar,
    //    but only if NMEA HDT sentence is not being received

    if( !g_bHDT_Rx ) {
        if( !std::isnan(gHdm)) {
            //Set gVar if needed from manual entry. gVar will be overwritten if
            // WMM plugin is available
            if( std::isnan(gVar) && (g_UserVar != 0.0) ) gVar = g_UserVar;
            gHdt = gHdm + gVar;
            if (gHdt < 0)
                gHdt += 360.0;
            else if (gHdt >= 360)
                gHdt -= 360.0;
            gHDT_Watchdog = gps_watchdog_timeout_ticks;
        }
    }

    if( pos_valid ) {
        if( g_nNMEADebug ) {
            wxString msg( _T("PostProcess NMEA with valid position") );
            wxLogMessage( msg );
        }

        //      Maintain the validity flags
        bool last_bGPSValid = bGPSValid;
        bGPSValid = true;
        if( !last_bGPSValid ) UpdateGPSCompassStatusBoxes();

        //      Show a little heartbeat tick in StatusWindow0 on NMEA events
        //      But no faster than 10 hz.
        unsigned long uiCurrentTickCount;
        m_MMEAeventTime.SetToCurrent();
        uiCurrentTickCount = m_MMEAeventTime.GetMillisecond() / 100;           // tenths of a second
        uiCurrentTickCount += m_MMEAeventTime.GetTicks() * 10;
        if( uiCurrentTickCount > m_ulLastNEMATicktime + 1 ) {
            m_ulLastNEMATicktime = uiCurrentTickCount;

            if( tick_idx++ > 6 ) tick_idx = 0;
        }
    }

//    Show gLat/gLon in StatusWindow0

    if( NULL != GetStatusBar() ) {
        if(pos_valid) {
            char tick_buf[2];
            tick_buf[0] = nmea_tick_chars[tick_idx];
            tick_buf[1] = 0;

            wxString s1( tick_buf, wxConvUTF8 );
            s1 += _(" Ship ");
            s1 += toSDMM( 1, gLat );
            s1 += _T("   ");
            s1 += toSDMM( 2, gLon );

            if(STAT_FIELD_TICK >= 0 )
                SetStatusText( s1, STAT_FIELD_TICK );
            
        }
        
        if(cog_sog_valid) {
            wxString sogcog;
            if( std::isnan(gSog) ) sogcog.Printf( _T("SOG --- ") + getUsrSpeedUnit() + _T("     ") );
            else
                sogcog.Printf( _T("SOG %2.2f ") + getUsrSpeedUnit() + _T("  "), toUsrSpeed( gSog ) );

            wxString cogs;
            if( std::isnan(gCog) )
                cogs.Printf( wxString( "COG ---\u00B0", wxConvUTF8 ) );
            else {
                if( g_bShowTrue )
                    cogs << wxString::Format( wxString("COG %03d°  ", wxConvUTF8 ), (int)gCog );
                if( g_bShowMag )
                    cogs << wxString::Format( wxString("COG %03d°(M)  ", wxConvUTF8 ), (int)gFrame->GetMag( gCog ) );
            }

            sogcog.Append( cogs );
            SetStatusText( sogcog, STAT_FIELD_SOGCOG );
        }


    }

#ifdef ocpnUPDATE_SYSTEM_TIME

//      Use the fix time to update the local system clock, only once per session
    if (!m_bTimeIsSet){
        if (!s_bSetSystemTime) {m_bTimeIsSet = true; return;}
        wxDateTime Fix_Time( wxDateTime::Now());

        if( 6 == sfixtime.Len() || 6 == sfixtime.find('.') ){       // perfectly recognised format?
            wxString a;
            long b;
            a = sfixtime.Mid( 0, 2 );
            if( a.ToLong( &b ) ) Fix_Time.SetHour( (wxDateTime::wxDateTime_t)b );
            a = sfixtime.Mid( 2, 2 );
            if( a.ToLong( &b ) ) Fix_Time.SetMinute( (wxDateTime::wxDateTime_t)b );
            a = sfixtime.Mid( 4, 2 );
            if( a.ToLong( &b ) ) Fix_Time.SetSecond( (wxDateTime::wxDateTime_t)b );
        }else return; //not a good sfixtime format
        
        time_t TimeOff = Fix_Time.GetTicks() - wxDateTime::Now().GetTicks();
        
        if(g_bHasHwClock){ // if a realtime hardwareclock isavailable we only check for time and a max of 2 hours of to prevent bogus info from some gps devices
            if( (abs(TimeOff) > 20) && (abs(TimeOff) < 7200) ){
                wxString msg;
                msg.Printf(_T("Setting system time, delta t is %d seconds"), TimeOff);
                wxLogMessage(msg);
#ifdef __WXMSW__
                //      Fix up the fix_time to convert to GMT
                Fix_Time = Fix_Time.ToGMT();

                //    Code snippet following borrowed from wxDateCtrl, MSW
                const wxDateTime::Tm tm( Fix_Time.GetTm() );
                SYSTEMTIME stm;
                stm.wYear = (WXWORD) tm.year;
                stm.wMonth = (WXWORD) ( tm.mon - wxDateTime::Jan + 1 );
                stm.wDay = tm.mday;
                stm.wDayOfWeek = 0;
                stm.wHour = Fix_Time.GetHour();
                stm.wMinute = tm.min;
                stm.wSecond = tm.sec;
                stm.wMilliseconds = 0;

                ::SetSystemTime( &stm );            // in GMT
#else
                //      This contortion sets the system date/time on POSIX host
                //      Requires the following line in /etc/sudoers
                //      "nav ALL=NOPASSWD:/bin/date *" (where nav is your username)
                //      or "%sudo ALL=NOPASSWD:/bin/date *"
                wxString CommandStr("sudo /bin/date +%T --utc --set=\"");
                CommandStr.Append( Fix_Time.Format("%T") );
                CommandStr.Append( "\"");
                msg.Printf(_T("Linux command is:"));
                msg += CommandStr;
                wxLogMessage(msg);
                wxExecute(CommandStr, wxEXEC_ASYNC);
#endif      //__WXMSW__
            }
            m_bTimeIsSet = true;
        }
        else{ //no hw-clock set both date and time
            if( gRmcDate.Len() == 6 ){
#if !defined(__WXMSW__) // not for windows
                wxString a;
                long b;
                Fix_Time.SetMonth((wxDateTime::Month)2);
                a = gRmcDate.Mid( 0, 2 );
                if( a.ToLong( &b ) ) Fix_Time.SetDay(b);
                a = gRmcDate.Mid( 2, 2 );
                if( a.ToLong( &b ) ) Fix_Time.SetMonth((wxDateTime::Month)(b-1));
                a = gRmcDate.Mid( 4, 2 );
                if( a.ToLong( &b ) ) Fix_Time.SetYear(b+2000);//TODO fix this before the year 2100
                wxString msg;
                wxString CommandStr("sudo /bin/date --utc --set=\"");
                CommandStr.Append( Fix_Time.Format("%D %T\"") );
                msg.Printf(_T("Set Date/Time, Linux command is: %s"), CommandStr );
                wxLogMessage(msg);
                wxExecute(CommandStr, wxEXEC_ASYNC);
#endif // !__WXMSW__
                    m_bTimeIsSet = true;
                }
            }
        }
#endif            //ocpnUPDATE_SYSTEM_TIME
}

void MyFrame::FilterCogSog( void )
{            
    if( g_bfilter_cogsog && !g_own_ship_sog_cog_calc ) {
        //    Simple averaging filter for COG
        double cog_last = gCog;       // most recent reported value

        //    Make a hole in array
        for( int i = g_COGFilterSec - 1; i > 0; i-- )
            COGFilterTable[i] = COGFilterTable[i - 1];
        COGFilterTable[0] = cog_last;

        //    If the lastest data is undefined, leave it
        if( !std::isnan(cog_last) ) {
            //
            double sum = 0., count = 0;
            for( int i = 0; i < g_COGFilterSec; i++ ) {
                double adder = COGFilterTable[i];
                if(std::isnan(adder))
                    continue;

                if( fabs( adder - cog_last ) > 180. ) {
                    if( ( adder - cog_last ) > 0. ) adder -= 360.;
                    else
                        adder += 360.;
                }

                sum += adder;
                count++;
            }
            sum /= count;

            if( sum < 0. ) sum += 360.;
            else
                if( sum >= 360. ) sum -= 360.;

            gCog = sum;
        }

        //    Simple averaging filter for SOG
        double sog_last = gSog;       // most recent reported value

        //    Make a hole in array
        for( int i = g_SOGFilterSec - 1; i > 0; i-- )
            SOGFilterTable[i] = SOGFilterTable[i - 1];
        SOGFilterTable[0] = sog_last;


        //    If the data are undefined, leave the array intact
        if( !std::isnan(gSog) ) {
            double sum = 0., count = 0;
            for( int i = 0; i < g_SOGFilterSec; i++ ) {
                if(std::isnan(SOGFilterTable[i]))
                    continue;

                sum += SOGFilterTable[i];
                count++;
            }
            sum /= count;

            gSog = sum;
        }
    }
}

void MyFrame::StopSockets( void )
{
//TODO: Can be removed?
}

void MyFrame::ResumeSockets( void )
{
//TODO: Can be removed?
}

void MyFrame::LoadHarmonics()
{
    if(!ptcmgr) {
        ptcmgr = new TCMgr;
        ptcmgr->LoadDataSources(TideCurrentDataSet);
    }
    else {
        bool b_newdataset = false;

        //      Test both ways
        wxArrayString test = ptcmgr->GetDataSet();
        for(unsigned int i=0 ; i < test.GetCount() ; i++) {
            bool b_foundi = false;
            for(unsigned int j=0 ; j < TideCurrentDataSet.GetCount() ; j++) {
                if(TideCurrentDataSet[j] == test[i]) {
                    b_foundi = true;
                    break;              // j loop
                }
            }
            if(!b_foundi) {
                b_newdataset = true;
                break;                  //  i loop
            }
        }

        test = TideCurrentDataSet;
        for(unsigned int i=0 ; i < test.GetCount() ; i++) {
            bool b_foundi = false;
            for(unsigned int j=0 ; j < ptcmgr->GetDataSet().GetCount() ; j++) {
                if(ptcmgr->GetDataSet()[j] == test[i]) {
                    b_foundi = true;
                    break;              // j loop
                }
            }
            if(!b_foundi) {
                b_newdataset = true;
                break;                  //  i loop
            }
        }

        if(b_newdataset)
            ptcmgr->LoadDataSources(TideCurrentDataSet);
    }
}

Route *pAISMOBRoute;

void MyFrame::ActivateAISMOBRoute( AIS_Target_Data *ptarget )
{
    if(!ptarget)
        return;

    //    The MOB point
    wxDateTime mob_time = wxDateTime::Now();
    wxString mob_label( _( "AIS MAN OVERBOARD" ) );
    mob_label += _(" at ");
    mob_label += mob_time.FormatTime();
    mob_label += _(" on ");
    mob_label += mob_time.FormatISODate();

    RoutePoint *pWP_MOB = new RoutePoint( ptarget->Lat, ptarget->Lon, _T ( "mob" ), mob_label, wxEmptyString );
    pWP_MOB->SetShared( true );
    pWP_MOB->m_bIsolatedMark = true;
    pSelect->AddSelectableRoutePoint( ptarget->Lat, ptarget->Lon, pWP_MOB );
    pConfig->AddNewWayPoint( pWP_MOB, -1 );       // use auto next num
    pWP_MOB->SetUseSca(false); //Do not use scaled hiding for MOB

    /* We want to start tracking any MOB in range (Which will trigger false alarms with messages received over the network etc., but will a) not discard nearby event even in case our GPS is momentarily unavailable and b) work even when the boat is stationary, in which case some GPS units do not provide COG)
    if( bGPSValid && !std::isnan(gCog) && !std::isnan(gSog) ) { */
        RoutePoint *pWP_src = new RoutePoint( gLat, gLon, g_default_wp_icon,
                                              wxString( _( "Own ship" ) ), wxEmptyString );
        pSelect->AddSelectableRoutePoint( gLat, gLon, pWP_src );
        pWP_MOB->SetUseSca(false); //Do not use scaled hiding for MOB
        pAISMOBRoute = new Route();
        pRouteList->Append( pAISMOBRoute );

        pAISMOBRoute->AddPoint( pWP_src );
        pAISMOBRoute->AddPoint( pWP_MOB );

        pSelect->AddSelectableRouteSegment(ptarget->Lat, ptarget->Lon, gLat, gLon, pWP_src, pWP_MOB, pAISMOBRoute );

        pAISMOBRoute->m_RouteNameString = _("Temporary AISMOB Route");
        pAISMOBRoute->m_RouteStartString = _("Present own ship");
        pAISMOBRoute->m_RouteEndString = mob_label;

        pAISMOBRoute->m_bDeleteOnArrival = false;

        pAISMOBRoute->SetRouteArrivalRadius( -1.0 );                    // never arrives

        if( g_pRouteMan->GetpActiveRoute() )
            g_pRouteMan->DeactivateRoute();
        //       g_pRouteMan->ActivateRoute( pAISMOBRoute, pWP_MOB );

        wxJSONValue v;
        v[_T("GUID")] = pAISMOBRoute->m_GUID;
        wxString msg_id( _T("OCPN_MAN_OVERBOARD") );
        g_pi_manager->SendJSONMessageToAllPlugins( msg_id, v );
    //}

    if( pRouteManagerDialog && pRouteManagerDialog->IsShown() ) {
        pRouteManagerDialog->UpdateRouteListCtrl();
        pRouteManagerDialog->UpdateWptListCtrl();
    }

    RefreshAllCanvas( false );

    wxString mob_message( _( "AIS MAN OVERBOARD" ) );
    mob_message += _(" Time: ");
    mob_message += mob_time.Format();
    mob_message += _("  Ownship Position: ");
    mob_message += toSDMM( 1, gLat );
    mob_message += _T("   ");
    mob_message += toSDMM( 2, gLon );
    mob_message += _("  MOB Position: ");
    mob_message += toSDMM( 1, ptarget->Lat );
    mob_message += _T("   ");
    mob_message += toSDMM( 2, ptarget->Lon );
    wxLogMessage( mob_message );

}

void MyFrame::UpdateAISMOBRoute( AIS_Target_Data *ptarget )
{
    if(pAISMOBRoute && ptarget)
    {
        //   Update Current Ownship point
        RoutePoint *OwnPoint = pAISMOBRoute->GetPoint( 1 );
        OwnPoint->m_lat = gLat;
        OwnPoint->m_lon = gLon;

        pSelect->DeleteSelectableRoutePoint( OwnPoint );
        pSelect->AddSelectableRoutePoint( gLat, gLon, OwnPoint );

        //   Update Current MOB point
        RoutePoint *MOB_Point = pAISMOBRoute->GetPoint( 2 );
        MOB_Point->m_lat = ptarget->Lat;
        MOB_Point->m_lon = ptarget->Lon;

        pSelect->DeleteSelectableRoutePoint( MOB_Point );
        pSelect->AddSelectableRoutePoint( ptarget->Lat, ptarget->Lon, MOB_Point );

        pSelect->UpdateSelectableRouteSegments( OwnPoint );
        pSelect->UpdateSelectableRouteSegments( MOB_Point );
    }

    RefreshAllCanvas( false );

    if( ptarget ){
        wxDateTime mob_time = wxDateTime::Now();

        wxString mob_message( _( "AIS MAN OVERBOARD UPDATE" ) );
        mob_message += _(" Time: ");
        mob_message += mob_time.Format();
        mob_message += _("  Ownship Position: ");
        mob_message += toSDMM( 1, gLat );
        mob_message += _T("   ");
        mob_message += toSDMM( 2, gLon );
        mob_message += _("  MOB Position: ");
        mob_message += toSDMM( 1, ptarget->Lat );
        mob_message += _T("   ");
        mob_message += toSDMM( 2, ptarget->Lon );

        wxLogMessage( mob_message );
    }

}

#if 0  // alte Version
void MyFrame::applySettingsString( wxString settings)
{
    //  Save some present values
    int last_UIScaleFactor = g_GUIScaleFactor;
    bool previous_expert = g_bUIexpert;
    g_last_ChartScaleFactor = g_ChartScaleFactor;

    //  Parse the passed settings string
    bool bproc_InternalGPS = false;
    bool benable_InternalGPS = false;

    int rr = GENERIC_CHANGED;

    // extract chart directories
    ArrayOfCDI NewDirArray;

    if(ChartData){
        wxStringTokenizer tkd(settings, _T(";"));
        while ( tkd.HasMoreTokens() ){
            wxString token = tkd.GetNextToken();

            if(token.StartsWith( _T("ChartDir"))){
                wxString dir = token.AfterFirst(':');
                if(dir.Length()){
                    ChartDirInfo cdi;
                    cdi.fullpath = dir.Trim();
                    cdi.magic_number = ChartData->GetMagicNumberCached(dir.Trim());
                    NewDirArray.Add(cdi);
                }
            }
        }

        // Scan for changes
        if(!ChartData->CompareChartDirArray( NewDirArray )){
            rr |= VISIT_CHARTS;
            rr |= CHANGE_CHARTS;
            wxLogMessage(_T("Chart Dir List change detected"));
        }
    }


    wxStringTokenizer tk(settings, _T(";"));
    while ( tk.HasMoreTokens() )
    {
        wxString token = tk.GetNextToken();
        wxString val = token.AfterFirst(':');

        //  Binary switches

        if(token.StartsWith( _T("prefb_lookahead"))){
            g_bLookAhead = val.IsSameAs(_T("1"));
        }
        else if(token.StartsWith( _T("prefb_quilt"))){
            g_bQuiltEnable = val.IsSameAs(_T("1"));
        }
        else if(token.StartsWith( _T("prefb_lockwp"))){
            g_bWayPointPreventDragging = val.IsSameAs(_T("1"));
        }
        else if(token.StartsWith( _T("prefb_showdepthunits"))){
            g_bShowDepthUnits = val.IsSameAs(_T("1"));
        }
        else if(token.StartsWith( _T("prefb_confirmdelete"))){
            g_bConfirmObjectDelete = val.IsSameAs(_T("1"));
        }
        else if(token.StartsWith( _T("prefb_showgrid"))){
            g_bDisplayGrid = val.IsSameAs(_T("1"));
        }
        else if(token.StartsWith( _T("prefb_showoutlines"))){
            g_bShowOutlines = val.IsSameAs(_T("1"));
        }
        else if(token.StartsWith( _T("prefb_expertmode"))){
            g_bUIexpert = val.IsSameAs(_T("1"));
        }
        else if(token.StartsWith( _T("prefb_internalGPS"))){
            bproc_InternalGPS = true;
            benable_InternalGPS = val.IsSameAs(_T("1"));
        }
        else if(token.StartsWith( _T("prefs_navmode"))){
            bool bPrevMode = g_bCourseUp;
            bool new_val = val.IsSameAs(_T("Course Up"));
            if(bPrevMode != new_val)
                ToggleCourseUp(GetPrimaryCanvas());
        }


        //  Strings, etc.

        else if(token.StartsWith( _T("prefs_UIScaleFactor"))){
            double a;
            if(val.ToDouble(&a))
                g_GUIScaleFactor = wxRound( (a / 10.) - 5.);
        }

        else if(token.StartsWith( _T("prefs_chartScaleFactor"))){
            double a;
            if(val.ToDouble(&a)){
                g_ChartScaleFactor = wxRound( (a / 10.) - 5.);
                g_ChartScaleFactorExp = g_Platform->getChartScaleFactorExp( g_ChartScaleFactor );
            }
        }

        else if(token.StartsWith( _T("prefs_chartInitDir"))){
            *pInit_Chart_Dir = val;
        }

#ifdef USE_S57
        if(ps52plib){
            float conv = 1;
            int depthUnit = ps52plib->m_nDepthUnitDisplay;
            if ( depthUnit == 0 ) // feet
                conv = 0.3048f; // international definiton of 1 foot is 0.3048 metres
            else if ( depthUnit == 2 ) // fathoms
                conv = 0.3048f * 6; // 1 fathom is 6 feet

            if(token.StartsWith( _T("prefb_showsound"))){
                bool old_val = ps52plib->m_bShowSoundg;
                ps52plib->m_bShowSoundg = val.IsSameAs(_T("1"));
                if(old_val != ps52plib->m_bShowSoundg)
                    rr |= S52_CHANGED;
            }
            else if(token.StartsWith( _T("prefb_showSCAMIN"))){
                bool old_val = ps52plib->m_bUseSCAMIN;
                ps52plib->m_bUseSCAMIN = val.IsSameAs(_T("1"));
                if(old_val != ps52plib->m_bUseSCAMIN)
                    rr |= S52_CHANGED;
            }
            else if(token.StartsWith( _T("prefb_showimptext"))){
                bool old_val = ps52plib->m_bShowS57ImportantTextOnly;
                ps52plib->m_bShowS57ImportantTextOnly = val.IsSameAs(_T("1"));
                if(old_val != ps52plib->m_bShowS57ImportantTextOnly)
                    rr |= S52_CHANGED;
            }
            else if(token.StartsWith( _T("prefb_showlightldesc"))){
                bool old_val = ps52plib->m_bShowLdisText;
                ps52plib->m_bShowLdisText = val.IsSameAs(_T("1"));
                if(old_val != ps52plib->m_bShowLdisText)
                    rr |= S52_CHANGED;
                }
            else if(token.StartsWith( _T("prefb_showATONLabels"))){
                bool old_val = ps52plib->m_bShowAtonText;
                ps52plib->m_bShowAtonText = val.IsSameAs(_T("1"));
                if(old_val != ps52plib->m_bShowAtonText)
                    rr |= S52_CHANGED;
            }

            else if(token.StartsWith( _T("prefs_displaycategory"))){
                _DisCat old_nset = ps52plib->GetDisplayCategory();

                _DisCat nset = DISPLAYBASE;
                if(wxNOT_FOUND != val.Lower().Find(_T("base")))
                    nset = DISPLAYBASE;
                else if(wxNOT_FOUND != val.Lower().Find(_T("mariner")))
                    nset = MARINERS_STANDARD;
                else if(wxNOT_FOUND != val.Lower().Find(_T("standard")))
                    nset = STANDARD;
                else if(wxNOT_FOUND != val.Lower().Find(_T("all")))
                    nset = OTHER;

                if(nset != old_nset){
                    rr |= S52_CHANGED;
                    ps52plib-> SetDisplayCategory( nset );
                }
            }

            else if(token.StartsWith( _T("prefs_shallowdepth"))){
                double old_dval = S52_getMarinerParam( S52_MAR_SHALLOW_CONTOUR );
                double dval;
                if(val.ToDouble(&dval)){
                    if(fabs(dval - old_dval) > .1){
                        S52_setMarinerParam( S52_MAR_SHALLOW_CONTOUR, dval * conv );
                        rr |= S52_CHANGED;
                    }
                }
            }

            else if(token.StartsWith( _T("prefs_safetydepth"))){
                double old_dval = S52_getMarinerParam( S52_MAR_SAFETY_CONTOUR );
                double dval;
                if(val.ToDouble(&dval)){
                    if(fabs(dval - old_dval) > .1){
                        S52_setMarinerParam( S52_MAR_SAFETY_CONTOUR, dval * conv );
                        rr |= S52_CHANGED;
                    }
                }
            }

            else if(token.StartsWith( _T("prefs_deepdepth"))){
                double old_dval = S52_getMarinerParam( S52_MAR_DEEP_CONTOUR );
                double dval;
                if(val.ToDouble(&dval)){
                    if(fabs(dval - old_dval) > .1){
                        S52_setMarinerParam( S52_MAR_DEEP_CONTOUR, dval * conv );
                        rr |= S52_CHANGED;
                    }
                }
            }

            else if(token.StartsWith( _T("prefs_vectorgraphicsstyle"))){
                LUPname old_LUP = ps52plib->m_nSymbolStyle;

                if(wxNOT_FOUND != val.Lower().Find(_T("paper")))
                    ps52plib->m_nSymbolStyle = PAPER_CHART;
                else if(wxNOT_FOUND != val.Lower().Find(_T("simplified")))
                    ps52plib->m_nSymbolStyle = SIMPLIFIED;

                if(old_LUP != ps52plib->m_nSymbolStyle)
                    rr |= S52_CHANGED;

            }

            else if(token.StartsWith( _T("prefs_vectorboundarystyle"))){
                LUPname old_LUP = ps52plib->m_nBoundaryStyle;

                if(wxNOT_FOUND != val.Lower().Find(_T("plain")))
                    ps52plib->m_nBoundaryStyle = PLAIN_BOUNDARIES;
                else if(wxNOT_FOUND != val.Lower().Find(_T("symbolized")))
                    ps52plib->m_nBoundaryStyle = SYMBOLIZED_BOUNDARIES;

                if(old_LUP != ps52plib->m_nBoundaryStyle)
                    rr |= S52_CHANGED;

            }

            else if(token.StartsWith( _T("prefs_vectorchartcolors"))){
                double old_dval = S52_getMarinerParam( S52_MAR_TWO_SHADES );

                if(wxNOT_FOUND != val.Lower().Find(_T("2")))
                    S52_setMarinerParam( S52_MAR_TWO_SHADES, 1. );
                else if(wxNOT_FOUND != val.Lower().Find(_T("4")))
                    S52_setMarinerParam( S52_MAR_TWO_SHADES, 0. );

                double new_dval = S52_getMarinerParam( S52_MAR_TWO_SHADES );
                if(fabs(new_dval - old_dval) > .1){
                    rr |= S52_CHANGED;
                }
            }
        }
#endif        
    }

    // Process Connections
    if(g_pConnectionParams && bproc_InternalGPS){

        //  Does the connection already exist?
        ConnectionParams *pExistingParams = NULL;
        ConnectionParams *cp = NULL;

        for ( size_t i = 0; i < g_pConnectionParams->Count(); i++ )
        {
            ConnectionParams *xcp = g_pConnectionParams->Item(i);
            if(INTERNAL_GPS == xcp->Type){
                pExistingParams = xcp;
                cp = xcp;
                break;
            }
        }

        bool b_action = true;
        if(pExistingParams){
            if(pExistingParams->bEnabled == benable_InternalGPS)
                b_action = false;                    // nothing to do...
            else
                cp->bEnabled = benable_InternalGPS;
        }
        else if(benable_InternalGPS){           //  Need a new Params
            // make a generic config string for InternalGPS.
            wxString sGPS = _T("2;3;;0;0;;0;1;0;0;;0;;1;0;0;0;0");          // 17 parms
            ConnectionParams *new_params = new ConnectionParams(sGPS);

            new_params->bEnabled = benable_InternalGPS;
            g_pConnectionParams->Add(new_params);
            cp = new_params;
        }


        if(b_action && cp){                               // something to do?

            // Terminate and remove any existing stream with the same port name
            DataStream *pds_existing = g_pMUX->FindStream( cp->GetDSPort() );
            if(pds_existing)
                g_pMUX->StopAndRemoveStream( pds_existing );


            if( cp->bEnabled ) {
                dsPortType port_type = cp->IOSelect;
                DataStream *dstr = new DataStream( g_pMUX,
                                                       cp->Type,
                                                       cp->GetDSPort(),
                                                       wxString::Format(wxT("%i"), cp->Baudrate),
                                                       port_type,
                                                       cp->Priority,
                                                       cp->Garmin);
                dstr->SetInputFilter(cp->InputSentenceList);
                dstr->SetInputFilterType(cp->InputSentenceListType);
                dstr->SetOutputFilter(cp->OutputSentenceList);
                dstr->SetOutputFilterType(cp->OutputSentenceListType);
                dstr->SetChecksumCheck(cp->ChecksumCheck);

                g_pMUX->AddStream(dstr);

                cp->b_IsSetup = true;
            }
        }
    }



    // And apply the changes
    pConfig->UpdateSettings();

    //  Might need to rebuild symbols
    if(g_last_ChartScaleFactor != g_ChartScaleFactor)
        rr |= S52_CHANGED;

#ifdef USE_S57
    if(rr & S52_CHANGED){
        if(ps52plib){
            ps52plib->FlushSymbolCaches();
            ps52plib->ClearCNSYLUPArray();      // some CNSY depends on renderer (e.g. CARC)
            ps52plib->GenerateStateHash();
        }
    }
#endif

    ProcessOptionsDialog( rr,  &NewDirArray );

    // Try to detect if the toolbar is changing, to avoid a rebuild if not necessary.

    bool b_newToolbar = false;

    if(g_GUIScaleFactor != last_UIScaleFactor)
        b_newToolbar = true;

    if(previous_expert != g_bUIexpert)
        b_newToolbar = true;

    if(rr & TOOLBAR_CHANGED)
        b_newToolbar = true;


    if(b_newToolbar){
        // .. for each canvas...
        for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
            ChartCanvas *cc = g_canvasArray.Item(i);
            if(cc)
                cc->DestroyToolbar();
        }
    }


    //  We do this is one case only to remove an orphan recovery window
#ifdef __OCPN__ANDROID__
     if(previous_expert && !g_bUIexpert){
         androidForceFullRepaint();
     }
#endif

    if(previous_expert != g_bUIexpert)
        g_Platform->applyExpertMode(g_bUIexpert);

    //  We set the compass size first, since that establishes the available space for the toolbar.
    SetGPSCompassScale();
    // ..For each canvas...
    for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
        ChartCanvas *cc = g_canvasArray.Item(i);
        if(cc)
            cc->GetCompass()->SetScaleFactor(g_compass_scalefactor);
    }
    UpdateGPSCompassStatusBoxes( true );

    if(b_newToolbar){
        g_Platform->ShowBusySpinner();

        SetAllToolbarScale();
        RequestNewToolbars(true);    // Force rebuild, to pick up bGUIexpert and scale settings.

        g_Platform->HideBusySpinner();

        RequestNewMasterToolbar( true );
    }

    SurfaceAllCanvasToolbars();

    gFrame->Raise();

    InvalidateAllGL();
    DoChartUpdate();
    UpdateControlBar( GetPrimaryCanvas());
    Refresh();


#if defined(__WXOSX__) || defined(__WXQT__)
    if( g_MainToolbar )
        g_MainToolbar->Raise();
#endif

    if(console)
        console->Raise();

    Refresh( false );

    if (NMEALogWindow::Get().Active())
        NMEALogWindow::Get().GetTTYWindow()->Raise();

}
#endif

void MyFrame::applySettingsString( wxString settings)
{
    //  Save some present values
    int last_UIScaleFactor = g_GUIScaleFactor;
    bool previous_expert = g_bUIexpert;
    g_last_ChartScaleFactor = g_ChartScaleFactor;
    ArrayOfCDI *pNewDirArray = new ArrayOfCDI;

//    int rr = g_Platform->platformApplyPrivateSettingsString( settings, pNewDirArray); // Neu
    int rr = GENERIC_CHANGED;  // aus Alt

    // And apply the changes
    pConfig->UpdateSettings();

    //  Might need to rebuild symbols
    if(g_last_ChartScaleFactor != g_ChartScaleFactor)
        rr |= S52_CHANGED;

    if(rr & S52_CHANGED){
        if(ps52plib){
            ps52plib->FlushSymbolCaches();
            ps52plib->ClearCNSYLUPArray();      // some CNSY depends on renderer (e.g. CARC)
            ps52plib->GenerateStateHash();
        }
    }

    ProcessOptionsDialog( rr,  pNewDirArray );

    // Try to detect if the toolbar is changing, to avoid a rebuild if not necessary.

    bool b_newToolbar = false;

    if(g_GUIScaleFactor != last_UIScaleFactor)
        b_newToolbar = true;

    if(previous_expert != g_bUIexpert)
        b_newToolbar = true;

    if(rr & TOOLBAR_CHANGED)
        b_newToolbar = true;
    

    if(b_newToolbar){
        // .. for each canvas...
        for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
            ChartCanvas *cc = g_canvasArray.Item(i);
            if(cc)
                cc->DestroyToolbar();
        }
    }


    //  We do this is one case only to remove an orphan recovery window
#ifdef __OCPN__ANDROID__
     if(previous_expert && !g_bUIexpert){
         androidForceFullRepaint();
     }
#endif

    if(previous_expert != g_bUIexpert)
        g_Platform->applyExpertMode(g_bUIexpert);

    //  We set the compass size first, since that establishes the available space for the toolbar.
    SetGPSCompassScale();
    // ..For each canvas...
    for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
        ChartCanvas *cc = g_canvasArray.Item(i);
        if(cc)
            cc->GetCompass()->SetScaleFactor(g_compass_scalefactor);
    }
    UpdateGPSCompassStatusBoxes( true );

    if(b_newToolbar){
        g_Platform->ShowBusySpinner();

        SetAllToolbarScale();
        RequestNewToolbars(true);    // Force rebuild, to pick up bGUIexpert and scale settings.

        g_Platform->HideBusySpinner();
        
        RequestNewMasterToolbar( true );
    }

    SurfaceAllCanvasToolbars();

    gFrame->Raise();

    InvalidateAllGL();
    DoChartUpdate();
    UpdateControlBar( GetPrimaryCanvas());
    Refresh();


#if defined(__WXOSX__) || defined(__WXQT__)
    if( g_MainToolbar )
        g_MainToolbar->Raise();

    for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
        ChartCanvas *cc = g_canvasArray.Item(i);
         if(cc && cc->GetMUIBar())
             cc->GetMUIBar()->Raise();
    }

#endif

    if(console)
        console->Raise();

    Refresh( false );

    if (NMEALogWindow::Get().Active())
        NMEALogWindow::Get().GetTTYWindow()->Raise();

}

#ifdef wxHAS_POWER_EVENTS
void MyFrame::OnSuspending(wxPowerEvent& event)
{
 //   wxDateTime now = wxDateTime::Now();
 //   printf("OnSuspending...%d\n", now.GetTicks());

    wxLogMessage(_T("System suspend starting..."));
}

void MyFrame::OnSuspended(wxPowerEvent& WXUNUSED(event))
{
//    wxDateTime now = wxDateTime::Now();
//    printf("OnSuspended...%d\n", now.GetTicks());
    wxLogMessage(_T("System is going to suspend."));

}

void MyFrame::OnSuspendCancel(wxPowerEvent& WXUNUSED(event))
{
//    wxDateTime now = wxDateTime::Now();
//    printf("OnSuspendCancel...%d\n", now.GetTicks());
    wxLogMessage(_T("System suspend was cancelled."));
}

int g_last_resume_ticks;
void MyFrame::OnResume(wxPowerEvent& WXUNUSED(event))
{
    wxDateTime now = wxDateTime::Now();
//    printf("OnResume...%d\n", now.GetTicks());
    wxLogMessage(_T("System resumed from suspend."));

    if((now.GetTicks() - g_last_resume_ticks) > 5){
        wxLogMessage(_T("Restarting streams."));
 //       printf("   Restarting streams\n");
        g_last_resume_ticks = now.GetTicks();
        if(g_pMUX){
            g_pMUX->ClearStreams();

            g_pMUX->StartAllStreams();
        }
    }

    //  If OpenGL is enabled, Windows Resume does not properly refresh the application GL context.
    //  We need to force a Resize event that actually does something.
    if(g_bopengl){
        if( IsMaximized() ){            // This is not real pretty on-screen, but works
            Maximize(false);
            wxYield();
            Maximize(true);
        }
        else {
            wxSize sz = GetSize();
            SetSize( wxSize(sz.x - 1, sz.y));
            wxYield();
            SetSize( sz );
        }
    }

}
#endif // wxHAS_POWER_EVENTS






//----------------------------------------------------------------------------------------------------------
//   Master Toolbar support
//----------------------------------------------------------------------------------------------------------

void MyFrame::RequestNewMasterToolbar(bool bforcenew)
{
    bool btbRebuild = false;

    bool b_reshow = true;
    if( g_MainToolbar ) {
        b_reshow = g_MainToolbar->IsShown();
        float ff = fabs(g_MainToolbar->GetScaleFactor() - g_toolbar_scalefactor);
        if((ff > 0.01f) || bforcenew){
            g_MainToolbar->DestroyToolBar();
            delete g_MainToolbar;
            g_MainToolbar = NULL;
        }

        btbRebuild = true;
    }

    if( !g_MainToolbar ) {
        g_MainToolbar = new ocpnFloatingToolbarDialog( this,
                                                       wxPoint( -1, -1/*g_maintoolbar_x, g_maintoolbar_y*/ ), wxTB_VERTICAL, g_toolbar_scalefactor );
        g_MainToolbar->SetCornerRadius( 5 );
        g_MainToolbar->SetBackGroundColorString( _T("GREY3")  );
        g_MainToolbar->SetToolbarHideMethod( TOOLBAR_HIDE_TO_FIRST_TOOL );
        g_MainToolbar->SetToolConfigString(g_toolbarConfig);
        g_MainToolbar->EnableRolloverBitmaps( false );

        g_MainToolbar->CreateConfigMenu();
//        g_MainToolbar->MoveDialogInScreenCoords(wxPoint(g_maintoolbar_x, g_maintoolbar_y), wxPoint(0, 0));
        g_bmasterToolbarFull = true;

    }

    if( g_MainToolbar ) {
        CreateMasterToolbar();
        g_MainToolbar->RestoreRelativePosition( g_maintoolbar_x, g_maintoolbar_y );
        g_MainToolbar->SetColorScheme(global_color_scheme);
        g_MainToolbar->Show(b_reshow && g_bshowToolbar);
    }

    if(btbRebuild){
        g_MainToolbar->SetAutoHide(g_bAutoHideToolbar);
        g_MainToolbar->SetAutoHideTimer(g_nAutoHideToolbar);
    }

}

bool MyFrame::CollapseGlobalToolbar()
{
    if(g_MainToolbar){
        m_nMasterToolCountShown = 1;
        g_MainToolbar->SetToolShowCount(m_nMasterToolCountShown);
        g_MainToolbar->GetToolbar()->InvalidateBitmaps();
        g_MainToolbar->Realize();
        g_bmasterToolbarFull = false;
        return true;
    }
    else
        return false;
}


bool MyFrame::GetMasterToolItemShow( int toolid )
{
    if(g_bmasterToolbarFull)
        return true;
    else
        return false;
}

ocpnToolBarSimple *MyFrame::CreateMasterToolbar()
{
    ocpnToolBarSimple *tb = NULL;

    if( g_MainToolbar )
        tb = g_MainToolbar->GetToolbar();

    if( !tb )
        return 0;

    ocpnStyle::Style* style = g_StyleManager->GetCurrentStyle();

    ToolbarItemContainer *tic= new ToolbarItemContainer( ID_MASTERTOGGLE,
                                                         style->GetToolIcon( _T("MUI_menu"), TOOLICON_NORMAL),
                                                         wxITEM_NORMAL, _("Hide Toolbar"), _T("MUI_menu"));
    tic->m_bRequired = true;

    g_MainToolbar->AddToolItem(tic);

    tic= new ToolbarItemContainer( ID_SETTINGS,
                                   style->GetToolIcon( _T("MUI_settings"), TOOLICON_NORMAL),
                                   wxITEM_NORMAL, _("Options"), _T("MUI_settings"));
    g_MainToolbar->AddToolItem(tic);

    tic= new ToolbarItemContainer( ID_MENU_ROUTE_NEW,
                                   style->GetToolIcon( _T("MUI_route"), TOOLICON_NORMAL),
                                   style->GetToolIcon( _T("MUI_route"), TOOLICON_TOGGLED),
                                   wxITEM_CHECK, wxString( _("Create Route") ) << _T(" (Ctrl-R)"), _T("MUI_route") );

    g_MainToolbar->AddToolItem(tic);

    tic= new ToolbarItemContainer( ID_ROUTEMANAGER,
                                   style->GetToolIcon( _T("MUI_RMD"), TOOLICON_NORMAL),
                                   wxITEM_NORMAL, _("Route & Mark Manager"), _T("MUI_RMD"));
    g_MainToolbar->AddToolItem(tic);

    tic= new ToolbarItemContainer( ID_TRACK,
                                   style->GetToolIcon( _T("MUI_track"), TOOLICON_NORMAL),
                                   style->GetToolIcon( _T("MUI_track"), TOOLICON_TOGGLED),
                                   wxITEM_CHECK, _("Enable Tracking"), _T("MUI_track"));
    g_MainToolbar->AddToolItem(tic);

    tic= new ToolbarItemContainer( ID_COLSCHEME,
                                   style->GetToolIcon( _T("MUI_colorscheme"), TOOLICON_NORMAL),
                                   wxITEM_NORMAL, _("Change Color Scheme"), _T("MUI_colorscheme"));
    g_MainToolbar->AddToolItem(tic);
    //if( GetMasterToolItemShow(ID_COLSCHEME) ){
      //  tb->AddTool( ID_COLSCHEME, _T("MUI_colorscheme"), style->GetToolIcon( _T("MUI_colorscheme"), TOOLICON_NORMAL ),
        //    tipString, wxITEM_NORMAL );
        //tb->SetToolTooltipHiViz( ID_COLSCHEME, true );  // cause the Tooltip to always be visible, whatever
                                                        //  the colorscheme
    //}

    tic= new ToolbarItemContainer( ID_PRINT,
                                   style->GetToolIcon( _T("MUI_print"), TOOLICON_NORMAL),
                                   wxITEM_NORMAL, _("Print Chart"), _T("MUI_print"));
    g_MainToolbar->AddToolItem(tic);

    tic= new ToolbarItemContainer( ID_ABOUT,
                                   style->GetToolIcon( _T("MUI_help"), TOOLICON_NORMAL),
                                   wxITEM_NORMAL, _("About OpenCPN"), _T("MUI_help"));
    g_MainToolbar->AddToolItem(tic);

    //      Add any PlugIn toolbar tools that request default positioning
    AddDefaultPositionPlugInTools();

    //  And finally add the MOB tool
    tic= new ToolbarItemContainer( ID_MOB,
                                   style->GetToolIcon( _T("mob_btn"), TOOLICON_NORMAL),
                                   wxITEM_NORMAL, wxString( _("Drop MOB Marker") ) << _(" (Ctrl-Space)"), _T("mob_btn"));
    g_MainToolbar->AddToolItem(tic);

    // Build the toolbar
    g_MainToolbar->RebuildToolbar();

    // Realize() the toolbar for current geometry
    style->Unload();
    g_MainToolbar->Realize();




    //  Set PlugIn tool toggle states
    ArrayOfPlugInToolbarTools tool_array = g_pi_manager->GetPluginToolbarToolArray();
    for( unsigned int i = 0; i < tool_array.GetCount(); i++ ) {
        PlugInToolbarToolContainer *pttc = tool_array.Item( i );
        if( !pttc->b_viz )
            continue;

        if( pttc->kind == wxITEM_CHECK )
            tb->ToggleTool( pttc->id, pttc->b_toggle );
    }


    return tb;
}

bool MyFrame::CheckAndAddPlugInTool()
{
    if( !g_pi_manager ) return false;

    bool bret = false;
    ocpnToolBarSimple *tb = NULL;

    if( g_MainToolbar )
        tb = g_MainToolbar->GetToolbar();

    if(!tb)
        return false;

    int n_tools = tb->GetToolsCount();

    //    Walk the PlugIn tool spec array, checking the requested position
    //    If a tool has been requested by a plugin at this position, add it
    ArrayOfPlugInToolbarTools tool_array = g_pi_manager->GetPluginToolbarToolArray();

    for( unsigned int i = 0; i < tool_array.GetCount(); i++ ) {
        PlugInToolbarToolContainer *pttc = tool_array.Item( i );
        if( pttc->position == n_tools ) {
            wxBitmap *ptool_bmp;

            switch( global_color_scheme ){
                case GLOBAL_COLOR_SCHEME_DAY:
                    ptool_bmp = pttc->bitmap_day;
                    ;
                    break;
                case GLOBAL_COLOR_SCHEME_DUSK:
                    ptool_bmp = pttc->bitmap_dusk;
                    break;
                case GLOBAL_COLOR_SCHEME_NIGHT:
                    ptool_bmp = pttc->bitmap_night;
                    break;
                default:
                    ptool_bmp = pttc->bitmap_day;
                    break;
            }

            ToolbarItemContainer *tic= new ToolbarItemContainer( pttc->id, *( ptool_bmp ), pttc->kind, pttc->shortHelp, _T(""));
            
            tic->m_NormalIconSVG = pttc->pluginNormalIconSVG;
            tic->m_RolloverIconSVG = pttc->pluginRolloverIconSVG;
            tic->m_ToggledIconSVG = pttc->pluginToggledIconSVG;
            tic->m_bPlugin = true;

            bret = true;
        }
    }

    //    If we added a tool, call again (recursively) to allow for adding adjacent tools
    if( bret ) while( CheckAndAddPlugInTool() ) { /* nothing to do */
    }

    return bret;
}

bool MyFrame::AddDefaultPositionPlugInTools()
{
    if( !g_pi_manager ) return false;

    bool bret = false;

    //    Walk the PlugIn tool spec array, checking the requested position
    //    If a tool has been requested by a plugin at this position, add it
    ArrayOfPlugInToolbarTools tool_array = g_pi_manager->GetPluginToolbarToolArray();

    for( unsigned int i = 0; i < tool_array.GetCount(); i++ ) {
        PlugInToolbarToolContainer *pttc = tool_array.Item( i );

        //      Tool is currently tagged as invisible
        if( !pttc->b_viz )
            continue;

        if( pttc->position == -1 )                  // PlugIn has requested default positioning
                {
            wxBitmap *ptool_bmp;

            switch( global_color_scheme ){
                case GLOBAL_COLOR_SCHEME_DAY:
                    ptool_bmp = pttc->bitmap_day;
                    break;
                case GLOBAL_COLOR_SCHEME_DUSK:
                    ptool_bmp = pttc->bitmap_dusk;
                    break;
                case GLOBAL_COLOR_SCHEME_NIGHT:
                    ptool_bmp = pttc->bitmap_night;
                    break;
                default:
                    ptool_bmp = pttc->bitmap_day;
                    break;
            }


            ToolbarItemContainer *tic= new ToolbarItemContainer( pttc->id, *( ptool_bmp ), pttc->kind, pttc->shortHelp, _T(""));
            
            tic->m_NormalIconSVG = pttc->pluginNormalIconSVG;
            tic->m_RolloverIconSVG = pttc->pluginRolloverIconSVG;
            tic->m_ToggledIconSVG = pttc->pluginToggledIconSVG;
            tic->m_bPlugin = true;

            g_MainToolbar->AddToolItem(tic);

            bret = true;
        }
    }
    return bret;
}










//----------------------------------------------------------------------------------------------------------
//      Application-wide CPL Error handler
//----------------------------------------------------------------------------------------------------------
#ifdef USE_S57
void MyCPLErrorHandler( CPLErr eErrClass, int nError, const char * pszErrorMsg )

{
    char msg[256];

    if( eErrClass == CE_Debug )
    snprintf( msg, 255, "CPL: %s", pszErrorMsg );
    else
        if( eErrClass == CE_Warning )
        snprintf( msg, 255, "CPL Warning %d: %s", nError, pszErrorMsg );
        else
            snprintf( msg, 255, "CPL ERROR %d: %s", nError, pszErrorMsg );

    wxString str( msg, wxConvUTF8 );
    wxLogMessage( str );
}
#endif

//----------------------------------------------------------------------------------------------------------
//      Printing Framework Support
//----------------------------------------------------------------------------------------------------------

bool MyPrintout::OnPrintPage( int page )
{
    wxDC *dc = GetDC();
    if( dc ) {
        if( page == 1 ) DrawPageOne( dc );

        return true;
    } else
        return false;
}

bool MyPrintout::OnBeginDocument( int startPage, int endPage )
{
    if( !wxPrintout::OnBeginDocument( startPage, endPage ) ) return false;

    return true;
}

void MyPrintout::GetPageInfo( int *minPage, int *maxPage, int *selPageFrom, int *selPageTo )
{
    *minPage = 1;
    *maxPage = 1;
    *selPageFrom = 1;
    *selPageTo = 1;
}

bool MyPrintout::HasPage( int pageNum )
{
    return ( pageNum == 1 );
}

void MyPrintout::DrawPageOne( wxDC *dc )
{

    // Get the Size of the Chart Canvas
    int sx, sy;
    gFrame->GetFocusCanvas()->GetClientSize( &sx, &sy );                       // of the canvas

    float maxX = sx;
    float maxY = sy;

    // Let's have at least some device units margin
    float marginX = 50;
    float marginY = 50;

    // Add the margin to the graphic size
    maxX += ( 2 * marginX );
    maxY += ( 2 * marginY );

    // Get the size of the DC in pixels
    int w, h;
    dc->GetSize( &w, &h );

    // Calculate a suitable scaling factor
    float scaleX = (float) ( w / maxX );
    float scaleY = (float) ( h / maxY );

    // Use x or y scaling factor, whichever fits on the DC
    float actualScale = wxMin(scaleX,scaleY);

    // Calculate the position on the DC for centring the graphic
    float posX = (float) ( ( w - ( maxX * actualScale ) ) / 2.0 );
    float posY = (float) ( ( h - ( maxY * actualScale ) ) / 2.0 );

    posX = wxMax(posX, marginX);
    posY = wxMax(posY, marginY);

    // Set the scale and origin
    dc->SetUserScale( actualScale, actualScale );
    dc->SetDeviceOrigin( (long) posX, (long) posY );

//  Get the latest bitmap as rendered by the ChartCanvas

    if(g_bopengl) {
#ifdef ocpnUSE_GL
        if(m_GLbmp.IsOk()){
            wxMemoryDC mdc;
            mdc.SelectObject( m_GLbmp );
            dc->Blit( 0, 0, m_GLbmp.GetWidth(), m_GLbmp.GetHeight(), &mdc, 0, 0 );
            mdc.SelectObject( wxNullBitmap );
        }
#endif
    }
    else {

//  And Blit/scale it onto the Printer DC
        wxMemoryDC mdc;
        mdc.SelectObject( *( gFrame->GetFocusCanvas()->pscratch_bm ) );

        dc->Blit( 0, 0, gFrame->GetFocusCanvas()->pscratch_bm->GetWidth(), gFrame->GetFocusCanvas()->pscratch_bm->GetHeight(), &mdc, 0, 0 );

        mdc.SelectObject( wxNullBitmap );
    }

}

void MyPrintout::GenerateGLbmp( )
{
    if(g_bopengl) {
#ifdef ocpnUSE_GL
        int gsx = gFrame->GetFocusCanvas()->GetglCanvas()->GetSize().x;
        int gsy = gFrame->GetFocusCanvas()->GetglCanvas()->GetSize().y;

        unsigned char *buffer = (unsigned char *)malloc( gsx * gsy * 4 );
        glReadPixels(0, 0, gsx, gsy, GL_RGBA, GL_UNSIGNED_BYTE, buffer );

        unsigned char *e = (unsigned char *)malloc( gsx * gsy * 3 );

        if(buffer && e){
            for( int p = 0; p < gsx*gsy; p++ ) {
                e[3*p+0] = buffer[4*p+0];
                e[3*p+1] = buffer[4*p+1];
                e[3*p+2] = buffer[4*p+2];
            }
        }
        free(buffer);

        wxImage image( gsx,gsy );
        image.SetData(e);
        wxImage mir_imag = image.Mirror( false );
        m_GLbmp = wxBitmap( mir_imag );
#endif
    }
}



//---------------------------------------------------------------------------------------
//
//        GPS Positioning Device Detection
//
//---------------------------------------------------------------------------------------

/*
 *     Enumerate all the serial ports on the system
 *
 *     wxArrayString *EnumerateSerialPorts(void)

 *     Very system specific, unavoidably.
 */

#if (__UNIX__) && !defined(__OCPN__ANDROID__) && !defined(__WXOSX__)
extern "C" int wait(int *);                     // POSIX wait() for process

#include <termios.h>
#include <sys/ioctl.h>
#ifdef __linux__
#include <linux/serial.h>
#endif

#endif

// ****************************************
// Fulup devices selection with scandir
// ****************************************

// reserve 4 pattern for plugins
char* devPatern[] = {
  NULL,NULL,NULL,NULL,
  NULL,NULL,NULL,NULL, (char*)-1};


// This function allow external plugin to search for a special device name
// ------------------------------------------------------------------------
int paternAdd (const char* patern) {
  int ind;

  // snan table for a free slot inside devpatern table
  for (ind=0; devPatern[ind] != (char*)-1; ind++)
       if (devPatern[ind] == NULL) break;

  // table if full
  if  (devPatern [ind] == (char*) -1) return -1;

  // store a copy of the patern in case calling routine had it on its stack
  devPatern [ind]  = strdup (patern);
  return 0;
}


#if defined(__UNIX__) && !defined(__OCPN__ANDROID__) && !defined(__WXOSX__)
// This filter verify is device is withing searched patern and verify it is openable
// -----------------------------------------------------------------------------------
int paternFilter (const struct dirent * dir) {
 char* res = NULL;
 char  devname [272];
 int   fd, ind;

  // search if devname fits with searched paterns
  for (ind=0; devPatern [ind] != (char*)-1; ind++) {
     if (devPatern [ind] != NULL) res=(char*)strcasestr(dir->d_name,devPatern [ind]);
     if (res != NULL) break;
  }

  // File does not fit researched patern
  if (res == NULL) return 0;

  // Check if we may open this file
  snprintf (devname, sizeof (devname), "/dev/%s", dir->d_name);
  fd = open(devname, O_RDWR|O_NDELAY|O_NOCTTY);

  // device name is pointing to a real device
  if(fd >= 0) {
    close (fd);
    return 1;
  }

  // file is not valid
  perror (devname);
  return 0;
}

int isTTYreal(const char *dev)
{
#ifdef __NetBSD__
    if (strncmp("/dev/tty0", dev, 9) == 0)
	return 1;
    if (strncmp("/dev/ttyU", dev, 9) == 0)
	return 1;
    if (strcmp("/dev/gps", dev) == 0)
	return 1;
    return 0;
#else /* !NetBSD */
    struct serial_struct serinfo;
    int ret = 0;

    int fd = open(dev, O_RDWR | O_NONBLOCK | O_NOCTTY);

    // device name is pointing to a real device
    if(fd >= 0) {
        if (ioctl(fd, TIOCGSERIAL, &serinfo)==0) {
            // If device type is no PORT_UNKNOWN we accept the port
            if (serinfo.type != PORT_UNKNOWN)
                ret = 1;
        }
        close (fd);
    }

    return ret;
#endif /* !NetBSD */
}


#endif

#ifdef __MINGW32__ // do I need this because of mingw, or because I am running mingw under wine?
# ifndef GUID_CLASS_COMPORT
DEFINE_GUID(GUID_CLASS_COMPORT, 0x86e0d1e0L, 0x8089, 0x11d0, 0x9c, 0xe4, 0x08, 0x00, 0x3e, 0x30, 0x1f, 0x73);
# endif
#endif

wxArrayString *EnumerateSerialPorts( void )
{
    wxArrayString *preturn = new wxArrayString;
#ifdef ocpnUSE_NEWSERIAL
    std::vector<serial::PortInfo> ports = serial::list_ports();
    for(std::vector<serial::PortInfo>::iterator it = ports.begin(); it != ports.end(); ++it) {
        wxString port((*it).port);
        if( (*it).description.length() > 0 && (*it).description != "n/a" ) {
            port.Append(_T(" - "));
            wxString s_description = wxString::FromUTF8( ((*it).description).c_str());
            port.Append( s_description );
        }
        preturn->Add(port);
    }
#ifdef __WXMSW__
    //    Search for Garmin device driver on Windows platforms
    HDEVINFO hdeviceinfo = INVALID_HANDLE_VALUE;
    hdeviceinfo = SetupDiGetClassDevs( (GUID *) &GARMIN_DETECT_GUID, NULL, NULL,
                                      DIGCF_PRESENT | DIGCF_INTERFACEDEVICE );
    if( hdeviceinfo != INVALID_HANDLE_VALUE ) {
        
        if(GarminProtocolHandler::IsGarminPlugged()){
            wxLogMessage( _T("EnumerateSerialPorts() Found Garmin USB Device.") );
            preturn->Add( _T("Garmin-USB") );         // Add generic Garmin selectable device
        }
    }
#endif // __WXMSW__
#else
#if defined(__UNIX__) && !defined(__OCPN__ANDROID__) && !defined(__WXOSX__)

    //Initialize the pattern table
    if( devPatern[0] == NULL ) {
        paternAdd ( "ttyUSB" );
        paternAdd ( "ttyACM" );
        paternAdd ( "ttyGPS" );
        paternAdd ( "refcom" );
    }

 //  Looking for user privilege openable devices in /dev
 //  Fulup use scandir to improve user experience and support new generation of AIS devices.

      wxString sdev;
      int ind, fcount;
      struct dirent **filelist = {0};

      // scan directory filter is applied automatically by this call
      fcount = scandir("/dev", &filelist, paternFilter, alphasort);

      for(ind = 0; ind < fcount; ind++)  {
       wxString sdev (filelist[ind]->d_name, wxConvUTF8);
       sdev.Prepend (_T("/dev/"));

       preturn->Add (sdev);
       free(filelist[ind]);
      }

      free(filelist);

//        We try to add a few more, arbitrarily, for those systems that have fixed, traditional COM ports

#ifdef __linux__
    if( isTTYreal("/dev/ttyS0") )
        preturn->Add( _T("/dev/ttyS0") );

    if( isTTYreal("/dev/ttyS1") )
        preturn->Add( _T("/dev/ttyS1") );
#endif /* linux */


#endif

#ifdef PROBE_PORTS__WITH_HELPER

    /*
     *     For modern Linux/(Posix??) systems, we may use
     *     the system files /proc/tty/driver/serial
     *     and /proc/tty/driver/usbserial to identify
     *     available serial ports.
     *     A complicating factor is that most (all??) linux
     *     systems require root privileges to access these files.
     *     We will use a helper program method here, despite implied vulnerability.
     */

    char buf[256]; // enough to hold one line from serial devices list
    char left_digit;
    char right_digit;
    int port_num;
    FILE *f;

    pid_t pID = vfork();

    if (pID == 0)// child
    {
//    Temporarily gain root privileges
        seteuid(file_user_id);

//  Execute the helper program
        execlp("ocpnhelper", "ocpnhelper", "-SB", NULL);

//  Return to user privileges
        seteuid(user_user_id);

        wxLogMessage(_T("Warning: ocpnhelper failed...."));
        _exit(0);// If exec fails then exit forked process.
    }

    wait(NULL);                  // for the child to quit

//    Read and parse the files

    /*
     * see if we have any traditional ttySx ports available
     */
    f = fopen("/var/tmp/serial", "r");

    if (f != NULL)
    {
        wxLogMessage(_T("Parsing copy of /proc/tty/driver/serial..."));

        /* read in each line of the file */
        while(fgets(buf, sizeof(buf), f) != NULL)
        {
            wxString sm(buf, wxConvUTF8);
            sm.Prepend(_T("   "));
            sm.Replace(_T("\n"), _T(" "));
            wxLogMessage(sm);

            /* if the line doesn't start with a number get the next line */
            if (buf[0] < '0' || buf[0] > '9')
            continue;

            /*
             * convert digits to an int
             */
            left_digit = buf[0];
            right_digit = buf[1];
            if (right_digit < '0' || right_digit > '9')
            port_num = left_digit - '0';
            else
            port_num = (left_digit - '0') * 10 + right_digit - '0';

            /* skip if "unknown" in the string */
            if (strstr(buf, "unknown") != NULL)
            continue;

            /* upper limit of 15 */
            if (port_num > 15)
            continue;

            /* create string from port_num  */

            wxString s;
            s.Printf(_T("/dev/ttyS%d"), port_num);

            /*  add to the output array  */
            preturn->Add(wxString(s));

        }

        fclose(f);
    }

    /*
     * Same for USB ports
     */
    f = fopen("/var/tmp/usbserial", "r");

    if (f != NULL)
    {
        wxLogMessage(_T("Parsing copy of /proc/tty/driver/usbserial..."));

        /* read in each line of the file */
        while(fgets(buf, sizeof(buf), f) != NULL)
        {

            wxString sm(buf, wxConvUTF8);
            sm.Prepend(_T("   "));
            sm.Replace(_T("\n"), _T(" "));
            wxLogMessage(sm);

            /* if the line doesn't start with a number get the next line */
            if (buf[0] < '0' || buf[0] > '9')
            continue;

            /*
             * convert digits to an int
             */
            left_digit = buf[0];
            right_digit = buf[1];
            if (right_digit < '0' || right_digit > '9')
            port_num = left_digit - '0';
            else
            port_num = (left_digit - '0') * 10 + right_digit - '0';

            /* skip if "unknown" in the string */
            if (strstr(buf, "unknown") != NULL)
            continue;

            /* upper limit of 15 */
            if (port_num > 15)
            continue;

            /* create string from port_num  */

            wxString s;
            s.Printf(_T("/dev/ttyUSB%d"), port_num);

            /*  add to the output array  */
            preturn->Add(wxString(s));

        }

        fclose(f);
    }

    //    As a fallback, in case seteuid doesn't work....
    //    provide some defaults
    //    This is currently the case for GTK+, which
    //    refuses to run suid.  sigh...

    if(preturn->IsEmpty())
    {
        preturn->Add( _T("/dev/ttyS0"));
        preturn->Add( _T("/dev/ttyS1"));
        preturn->Add( _T("/dev/ttyUSB0"));
        preturn->Add( _T("/dev/ttyUSB1"));
        preturn->Add( _T("/dev/ttyACM0"));
        preturn->Add( _T("/dev/ttyACM1"));
    }

//    Clean up the temporary files created by helper.
    pid_t cpID = vfork();

    if (cpID == 0)// child
    {
//    Temporarily gain root privileges
        seteuid(file_user_id);

//  Execute the helper program
        execlp("ocpnhelper", "ocpnhelper", "-U", NULL);

//  Return to user privileges
        seteuid(user_user_id);
        _exit(0);// If exec fails then exit forked process.
    }

#endif      // __WXGTK__
#ifdef __WXOSX__
#include "macutils.h"
    char* paPortNames[MAX_SERIAL_PORTS];
    int iPortNameCount;

    memset(paPortNames,0x00,sizeof(paPortNames));
    iPortNameCount = FindSerialPortNames(&paPortNames[0],MAX_SERIAL_PORTS);
    for (int iPortIndex=0; iPortIndex<iPortNameCount; iPortIndex++)
    {
        wxString sm(paPortNames[iPortIndex], wxConvUTF8);
        preturn->Add(sm);
        free(paPortNames[iPortIndex]);
    }
#endif      //__WXOSX__
#ifdef __WXMSW__
    /* ************************************************************************
     * Windows provides no system level enumeration of available serial ports
     * There are several ways of doing this.
     *
     *************************************************************************/

#include <windows.h>

    //    Method 1:  Use GetDefaultCommConfig()
    // Try first {g_nCOMPortCheck} possible COM ports, check for a default configuration
    //  This method will not find some Bluetooth SPP ports
    for( int i = 1; i < g_nCOMPortCheck; i++ ) {
        wxString s;
        s.Printf( _T("COM%d"), i );

        COMMCONFIG cc;
        DWORD dwSize = sizeof(COMMCONFIG);
        if( GetDefaultCommConfig( s.fn_str(), &cc, &dwSize ) )
            preturn->Add( wxString( s ) );
    }

#if 0
    // Method 2:  Use FileOpen()
    // Try all 255 possible COM ports, check to see if it can be opened, or if
    // not, that an expected error is returned.

    BOOL bFound;
    for (int j=1; j<256; j++)
    {
        char s[20];
        sprintf(s, "\\\\.\\COM%d", j);

        // Open the port tentatively
        BOOL bSuccess = FALSE;
        HANDLE hComm = ::CreateFile(s, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);

        //  Check for the error returns that indicate a port is there, but not currently useable
        if (hComm == INVALID_HANDLE_VALUE)
        {
            DWORD dwError = GetLastError();

            if (dwError == ERROR_ACCESS_DENIED ||
                    dwError == ERROR_GEN_FAILURE ||
                    dwError == ERROR_SHARING_VIOLATION ||
                    dwError == ERROR_SEM_TIMEOUT)
            bFound = TRUE;
        }
        else
        {
            bFound = TRUE;
            CloseHandle(hComm);
        }

        if (bFound)
        preturn->Add(wxString(s));
    }
#endif

    // Method 3:  WDM-Setupapi
    //  This method may not find XPort virtual ports,
    //  but does find Bluetooth SPP ports

    GUID *guidDev = (GUID*) &GUID_CLASS_COMPORT;

    HDEVINFO hDevInfo = INVALID_HANDLE_VALUE;

    hDevInfo = SetupDiGetClassDevs( guidDev,
                                     NULL,
                                     NULL,
                                     DIGCF_PRESENT | DIGCF_DEVICEINTERFACE );

    if(hDevInfo != INVALID_HANDLE_VALUE) {

        BOOL bOk = TRUE;
        SP_DEVICE_INTERFACE_DATA ifcData;

        ifcData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
        for (DWORD ii=0; bOk; ii++) {
            bOk = SetupDiEnumDeviceInterfaces(hDevInfo, NULL, guidDev, ii, &ifcData);
            if (bOk) {
            // Got a device. Get the details.

                SP_DEVINFO_DATA devdata = {sizeof(SP_DEVINFO_DATA)};
                bOk = SetupDiGetDeviceInterfaceDetail(hDevInfo,
                                                      &ifcData, NULL, 0, NULL, &devdata);

                //      We really only need devdata
                if( !bOk ) {
                    if( GetLastError() == 122)  //ERROR_INSUFFICIENT_BUFFER, OK in this case
                        bOk = true;
                }

                //      We could get friendly name and/or description here
                TCHAR fname[256] = {0};
                TCHAR desc[256] ={0};
                if (bOk) {
                    BOOL bSuccess = SetupDiGetDeviceRegistryProperty(
                        hDevInfo, &devdata, SPDRP_FRIENDLYNAME, NULL,
                        (PBYTE)fname, sizeof(fname), NULL);

                    bSuccess = bSuccess && SetupDiGetDeviceRegistryProperty(
                        hDevInfo, &devdata, SPDRP_DEVICEDESC, NULL,
                        (PBYTE)desc, sizeof(desc), NULL);
                }

                //  Get the "COMn string from the registry key
                if(bOk) {
                    bool bFoundCom = false;
                    TCHAR dname[256];
                    HKEY hDeviceRegistryKey = SetupDiOpenDevRegKey(hDevInfo, &devdata,
                                                                   DICS_FLAG_GLOBAL, 0,
                                                                   DIREG_DEV, KEY_QUERY_VALUE);
                    if(INVALID_HANDLE_VALUE != hDeviceRegistryKey) {
                            DWORD RegKeyType;
                            wchar_t    wport[80];
                            LPCWSTR cstr = wport;
                            MultiByteToWideChar( 0, 0, "PortName", -1, wport, 80);
                            DWORD len = sizeof(dname);

                            int result = RegQueryValueEx(hDeviceRegistryKey, cstr,
                                                        0, &RegKeyType, (PBYTE)dname, &len );
                            if( result == 0 )
                                bFoundCom = true;
                    }

                    if( bFoundCom ) {
                        wxString port( dname, wxConvUTF8 );

                        //      If the port has already been found, remove the prior entry
                        //      in favor of this entry, which will have descriptive information appended
                        for( unsigned int n=0 ; n < preturn->GetCount() ; n++ ) {
                            if((preturn->Item(n)).IsSameAs(port)){
                                preturn->RemoveAt( n );
                                break;
                            }
                        }
                        wxString desc_name( desc, wxConvUTF8 );         // append "description"
                        port += _T(" ");
                        port += desc_name;

                        preturn->Add( port );
                    }
                }
            }
        }//for
    }// if


//    Search for Garmin device driver on Windows platforms

    HDEVINFO hdeviceinfo = INVALID_HANDLE_VALUE;

    hdeviceinfo = SetupDiGetClassDevs( (GUID *) &GARMIN_DETECT_GUID, NULL, NULL,
            DIGCF_PRESENT | DIGCF_INTERFACEDEVICE );

    if( hdeviceinfo != INVALID_HANDLE_VALUE ) {

        if(GarminProtocolHandler::IsGarminPlugged()){
            wxLogMessage( _T("EnumerateSerialPorts() Found Garmin USB Device.") );
            preturn->Add( _T("Garmin-USB") );         // Add generic Garmin selectable device
        }
    }

#if 0
    SP_DEVICE_INTERFACE_DATA deviceinterface;
    deviceinterface.cbSize = sizeof(deviceinterface);

    if (SetupDiEnumDeviceInterfaces(hdeviceinfo,
                    NULL,
                    (GUID *) &GARMIN_DETECT_GUID,
                    0,
                    &deviceinterface))
    {
        wxLogMessage(_T("Found Garmin Device."));

        preturn->Add(_T("GARMIN"));         // Add generic Garmin selectable device
    }
#endif

#endif      //__WXMSW__
#endif //ocpnUSE_NEWSERIAL
    return preturn;
}


bool CheckSerialAccess( void )
{
    bool bret = true;
#if defined(__UNIX__) && !defined(__OCPN__ANDROID__)

#if 0
    termios ttyset_old;
    termios ttyset;
    termios ttyset_check;

    // Get a list of the ports
    wxArrayString *ports = EnumerateSerialPorts();
    if( ports->GetCount() == 0 )
        bret = false;

    for(unsigned int i=0 ; i < ports->GetCount() ; i++){
        wxCharBuffer buf = ports->Item(i).ToUTF8();

        //      For the first real port found, try to open it, write some config, and
        //      be sure it reads back correctly.
        if( isTTYreal( buf.data() ) ){
            int fd = open(buf.data(), O_RDWR | O_NONBLOCK | O_NOCTTY);

            // device name is pointing to a real device
            if(fd > 0) {

                if (isatty(fd) != 0)
                {
                    /* Save original terminal parameters */
                    tcgetattr(fd,&ttyset_old);
                    // Write some data
                    memcpy(&ttyset, &ttyset_old, sizeof(termios));

                    ttyset.c_cflag &=~ CSIZE;
                    ttyset.c_cflag |= CSIZE & CS7;

                    tcsetattr(fd, TCSANOW, &ttyset);

                    // Read it back
                    tcgetattr(fd, &ttyset_check);
                    if(( ttyset_check.c_cflag & CSIZE) != CS7 ){
                        bret = false;
                    }
                    else {
                            // and again
                        ttyset.c_cflag &=~ CSIZE;
                        ttyset.c_cflag |= CSIZE & CS8;

                        tcsetattr(fd, TCSANOW, &ttyset);

                            // Read it back
                        tcgetattr(fd, &ttyset_check);
                        if(( ttyset_check.c_cflag & CSIZE) != CS8 ){
                            bret = false;
                        }
                    }

                    tcsetattr(fd, TCSANOW, &ttyset_old);
                }

                close (fd);
            }   // if open
        }
    }

#endif  // 0

    //  Who owns /dev/ttyS0?
    bret = false;

    wxArrayString result1;
    wxExecute(_T("stat -c %G /dev/ttyS0"), result1);
    if(!result1.size())
        wxExecute(_T("stat -c %G /dev/ttyUSB0"), result1);

    if(!result1.size())
        wxExecute(_T("stat -c %G /dev/ttyACM0"), result1);

    wxString msg1 = _("OpenCPN requires access to serial ports to use serial NMEA data.\n");
    if(!result1.size()) {
        wxString msg = msg1 + _("No Serial Ports can be found on this system.\n\
You must install a serial port (modprobe correct kernel module) or plug in a usb serial device.\n");

        OCPNMessageBox ( NULL, msg, wxString( _("OpenCPN Info") ), wxICON_INFORMATION | wxOK, 30 );
        return false;
    }

    //  Is the current user in this group?
    wxString user = wxGetUserId(), group = result1[0];

    wxArrayString result2;
    wxExecute(_T("groups ") + user, result2);

    if(result2.size()) {
        wxString user_groups = result2[0];

        if(user_groups.Find(group) != wxNOT_FOUND)
            bret = true;
    }

#ifdef FLATPAK
    return bret;
#endif

    if(!bret){

        wxString msg = msg1 + _("\
You do currently not have permission to access the serial ports on this system.\n\n\
It is suggested that you exit OpenCPN now,\n\
and add yourself to the correct group to enable serial port access.\n\n\
You may do so by executing the following command from the linux command line:\n\n\
                sudo usermod -a -G ");

        msg += group;
        msg += _T(" ");
        msg += user;
        msg += _T("\n");

        OCPNMessageBox ( NULL, msg, wxString( _("OpenCPN Info") ), wxICON_INFORMATION | wxOK, 30 );
    }

#endif  // (__UNIX__) && !defined(__OCPN__ANDROID__)

    return bret;
}

void appendOSDirSlash( wxString* pString )
{
    wxChar sep = wxFileName::GetPathSeparator();
    if( pString->Last() != sep ) pString->Append( sep );
}

/* ************************************************************************
 * Global color management routines
 *
 *************************************************************************/

wxColour GetGlobalColor(wxString colorName)
{
    wxColour ret_color;

#ifdef USE_S57
    //    Use the S52 Presentation library if present
    if( ps52plib )
        ret_color = ps52plib->getwxColour( colorName );
#endif
    if( !ret_color.Ok() && pcurrent_user_color_hash )
        ret_color = ( *pcurrent_user_color_hash )[colorName];

    //    Default
    if( !ret_color.Ok() ) {
        ret_color.Set( 128, 128, 128 );  // Simple Grey
        wxLogMessage(_T("Warning: Color not found ") + colorName);
        // Avoid duplicate warnings:
        if (pcurrent_user_color_hash)
            ( *pcurrent_user_color_hash )[colorName] = ret_color;
    }

    return ret_color;
}

static const char *usercolors[] = { "Table:DAY", "GREEN1;120;255;120;", "GREEN2; 45;150; 45;",
        "GREEN3;200;220;200;", "GREEN4;  0;255;  0;", "BLUE1; 170;170;255;", "BLUE2;  45; 45;170;",
        "BLUE3;   0;  0;255;", "GREY1; 200;200;200;", "GREY2; 230;230;230;", "RED1;  220;200;200;",
        "UBLCK;   0;  0;  0;", "UWHIT; 255;255;255;", "URED;  255;  0;  0;", "UGREN;   0;255;  0;",
        "YELO1; 243;229; 47;", "YELO2; 128; 80;  0;", "TEAL1;   0;128;128;", "GREEN5;170;254;  0;",
        "COMPT; 245;247;244",
#ifdef __WXOSX__
        "DILG0; 255;255;255;",              // Dialog Background white
#else
        "DILG0; 238;239;242;",              // Dialog Background white
#endif
        "DILG1; 212;208;200;",              // Dialog Background
        "DILG2; 255;255;255;",              // Control Background
        "DILG3;   0;  0;  0;",              // Text
        "UITX1;   0;  0;  0;",              // Menu Text, derived from UINFF

        "CHGRF; 163; 180; 183;",
        "UINFM; 197;  69; 195;",
        "UINFG; 104; 228;  86;",
        "UINFF; 125; 137; 140;",
        "UINFR; 241;  84; 105;",
        "SHIPS;   7;   7;   7;",
        "CHYLW; 244; 218;  72;",
        "CHWHT; 212; 234; 238;",

        "UDKRD; 124; 16;  0;",
        "UARTE; 200;  0;  0;",              // Active Route, Grey on Dusk/Night

        "NODTA; 163; 180; 183;",
        "CHBLK;   7;   7;   7;",
        "SNDG1; 125; 137; 140;",            // Tiefenzahlen Grau (125;137;140)
        "SNDG2;   7;   7;   7;",            // Tiefenzahlen Schwarz
        "SCLBR; 235; 125;  54;",
        "UIBDR; 125; 137; 140;",
        "UINFB;  58; 120; 240;",
        "UINFD;   7;   7;   7;",
        "UINFO; 235; 125;  54;",
        "PLRTE; 220;  64;  37;",
        "CHMGD; 197; 69; 195;",
        "UIBCK; 212; 234; 238;",

        "DASHB; 255;255;255;",              // Dashboard Instr background
        "DASHL; 190;190;190;",              // Dashboard Instr Label
        "DASHF;  50; 50; 50;",              // Dashboard Foreground
        "DASHR; 200;  0;  0;",              // Dashboard Red
        "DASHG;   0;200;  0;",              // Dashboard Green
        "DASHN; 200;120;  0;",              // Dashboard Needle
        "DASH1; 204;204;255;",              // Dashboard Illustrations
        "DASH2; 122;131;172;",              // Dashboard Illustrations
        "COMP1; 211;211;211;",              // Compass Window Background
        
        "GREY3;  40; 40; 40;",              // MUIBar/TB background
        "BLUE4; 100;100;200;",              // Canvas Focus Bar
        "VIO01; 171; 33;141;",
        "VIO02; 209;115;213;",



        "Table:DUSK", "GREEN1; 60;128; 60;", "GREEN2; 22; 75; 22;", "GREEN3; 80;100; 80;",
        "GREEN4;  0;128;  0;", "BLUE1;  80; 80;160;", "BLUE2;  30; 30;120;", "BLUE3;   0;  0;128;",
        "GREY1; 100;100;100;", "GREY2; 128;128;128;", "RED1;  150;100;100;", "UBLCK;   0;  0;  0;",
        "UWHIT; 255;255;255;", "URED;  120; 54; 11;", "UGREN;  35;110; 20;", "YELO1; 120;115; 24;",
        "YELO2;  64; 40;  0;", "TEAL1;   0; 64; 64;", "GREEN5; 85;128; 0;",
        "COMPT; 124;126;121",

        "CHGRF;  41; 46; 46;",
        "UINFM;  58; 20; 57;",
        "UINFG;  35; 76; 29;",
        "UINFF;  41; 46; 46;",
        "UINFR;  80; 28; 35;",
        "SHIPS;  71; 78; 79;",
        "CHYLW;  81; 73; 24;",
        "CHWHT;  71; 78; 79;",

        "DILG0; 110;110;110;",              // Dialog Background
        "DILG1; 110;110;110;",              // Dialog Background
        "DILG2;   0;  0;  0;",              // Control Background
        "DILG3; 130;130;130;",              // Text
        "UITX1;  41; 46; 46;",              // Menu Text, derived from UINFF
        "UDKRD;  80;  0;  0;",
        "UARTE;  64; 64; 64;",              // Active Route, Grey on Dusk/Night

        "NODTA;  41;  46;  46;",
        "CHBLK;  54;  60;  61;",
        "SNDG1;  41;  46;  46;",
        "SNDG2;  71;  78;  79;",
        "SCLBR;  75;  38;  19;",
        "UIBDR;  54;  60;  61;",
        "UINFB;  19;  40;  80;",
        "UINFD;  71;  78;  79;",
        "UINFO;  75;  38;  19;",
        "PLRTE;  73;  21;  12;",
        "CHMGD; 74; 58; 81;",
        "UIBCK; 7; 7; 7;",

        "DASHB;  77; 77; 77;",              // Dashboard Instr background
        "DASHL;  54; 54; 54;",              // Dashboard Instr Label
        "DASHF;   0;  0;  0;",              // Dashboard Foreground
        "DASHR;  58; 21; 21;",              // Dashboard Red
        "DASHG;  21; 58; 21;",              // Dashboard Green
        "DASHN; 100; 50;  0;",              // Dashboard Needle
        "DASH1;  76; 76;113;",              // Dashboard Illustrations
        "DASH2;  48; 52; 72;",              // Dashboard Illustrations
        "COMP1; 107;107;107;",              // Compass Window Background

        "GREY3;  20; 20; 20;",              // MUIBar/TB background
        "BLUE4;  80; 80;160;",              // Canvas Focus Bar
        "VIO01; 128; 25;108;",
        "VIO02; 171; 33;141;",

        "Table:NIGHT", "GREEN1; 30; 80; 30;", "GREEN2; 15; 60; 15;", "GREEN3; 12; 23;  9;",
        "GREEN4;  0; 64;  0;", "BLUE1;  60; 60;100;", "BLUE2;  22; 22; 85;", "BLUE3;   0;  0; 40;",
        "GREY1;  48; 48; 48;", "GREY2;  32; 32; 32;", "RED1;  100; 50; 50;", "UWHIT; 255;255;255;",
        "UBLCK;   0;  0;  0;", "URED;   60; 27;  5;", "UGREN;  17; 55; 10;", "YELO1;  60; 65; 12;",
        "YELO2;  32; 20;  0;", "TEAL1;   0; 32; 32;", "GREEN5; 44; 64; 0;",
        "COMPT;  48; 49; 51",
        "DILG0;  80; 80; 80;",              // Dialog Background
        "DILG1;  80; 80; 80;",              // Dialog Background
        "DILG2;   0;  0;  0;",              // Control Background
        "DILG3;  65; 65; 65;",              // Text
        "UITX1;  31; 34; 35;",              // Menu Text, derived from UINFF
        "UDKRD;  50;  0;  0;",
        "UARTE;  64; 64; 64;",              // Active Route, Grey on Dusk/Night

        "CHGRF;  16; 18; 18;",
        "UINFM;  52; 18; 52;",
        "UINFG;  22; 24;  7;",
        "UINFF;  31; 34; 35;",
        "UINFR;  59; 17; 10;",
        "SHIPS;  37; 41; 41;",
        "CHYLW;  31; 33; 10;",
        "CHWHT;  37; 41; 41;",

        "NODTA;   7;   7;   7;",
        "CHBLK;  31;  34;  35;",
        "SNDG1;  31;  34;  35;",
        "SNDG2;  43;  48;  48;",
        "SCLBR;  52;  28;  12;",
        "UIBDR;  31;  34;  35;",
        "UINFB;  21;  29;  69;",
        "UINFD;  43;  48;  58;",
        "UINFO;  52;  28;  12;",
        "PLRTE;  66;  19;  11;",
        "CHMGD; 52; 18; 52;",
        "UIBCK; 7; 7; 7;",

        "DASHB;   0;  0;  0;",              // Dashboard Instr background
        "DASHL;  20; 20; 20;",              // Dashboard Instr Label
        "DASHF;  64; 64; 64;",              // Dashboard Foreground
        "DASHR;  70; 15; 15;",              // Dashboard Red
        "DASHG;  15; 70; 15;",              // Dashboard Green
        "DASHN;  17; 80; 56;",              // Dashboard Needle
        "DASH1;  48; 52; 72;",              // Dashboard Illustrations
        "DASH2;  36; 36; 53;",              // Dashboard Illustrations
        "COMP1;  24; 24; 24;",              // Compass Window Background

        "GREY3;  10; 10; 10;",              // MUIBar/TB background
        "BLUE4;  70; 70;140;",              // Canvas Focus Bar
        "VIO01;  85; 16; 72;",
        "VIO02; 128; 25;108;",
        
        "*****" };

int get_static_line( char *d, const char **p, int index, int n )
{
    if( !strcmp( p[index], "*****" ) ) return 0;

    strncpy( d, p[index], n );
    return strlen( d );
}

static void InitializeUserColors( void )
{
    const char **p = usercolors;
    char buf[81];
    int index = 0;
    char TableName[20];
    colTable *ctp;
    colTable *ct;
    int R, G, B;

    UserColorTableArray = new wxArrayPtrVoid;
    UserColourHashTableArray = new wxArrayPtrVoid;

    //    Create 3 color table entries
    ct = new colTable;
    ct->tableName = new wxString( _T("DAY") );
    ct->color = new wxArrayPtrVoid;
    UserColorTableArray->Add( (void *) ct );

    ct = new colTable;
    ct->tableName = new wxString( _T("DUSK") );
    ct->color = new wxArrayPtrVoid;
    UserColorTableArray->Add( (void *) ct );

    ct = new colTable;
    ct->tableName = new wxString( _T("NIGHT") );
    ct->color = new wxArrayPtrVoid;
    UserColorTableArray->Add( (void *) ct );

    while( ( get_static_line( buf, p, index, sizeof(buf) - 1 ) ) ) {
        if( !strncmp( buf, "Table", 5 ) ) {
            sscanf( buf, "Table:%s", TableName );

            for( unsigned int it = 0; it < UserColorTableArray->GetCount(); it++ ) {
                ctp = (colTable *) ( UserColorTableArray->Item( it ) );
                if( !strcmp( TableName, ctp->tableName->mb_str() ) ) {
                    ct = ctp;
                    break;
                }
            }

        } else {
            char name[21];
            int j = 0;
            while( buf[j] != ';' && j < 20 ) {
                name[j] = buf[j];
                j++;
            }
            name[j] = 0;

            S52color *c = new S52color;
            strcpy( c->colName, name );

            sscanf( &buf[j], ";%i;%i;%i", &R, &G, &B );
            c->R = (char) R;
            c->G = (char) G;
            c->B = (char) B;

            ct->color->Add( c );

        }

        index++;
    }

    //    Now create the Hash tables

    for( unsigned int its = 0; its < UserColorTableArray->GetCount(); its++ ) {
        wxColorHashMap *phash = new wxColorHashMap;
        UserColourHashTableArray->Add( (void *) phash );

        colTable *ctp = (colTable *) ( UserColorTableArray->Item( its ) );

        for( unsigned int ic = 0; ic < ctp->color->GetCount(); ic++ ) {
            S52color *c2 = (S52color *) ( ctp->color->Item( ic ) );

            wxColour c( c2->R, c2->G, c2->B );
            wxString key( c2->colName, wxConvUTF8 );
            ( *phash )[key] = c;

        }
    }

    //    Establish a default hash table pointer
    //    in case a color is needed before ColorScheme is set
    pcurrent_user_color_hash = (wxColorHashMap *) UserColourHashTableArray->Item( 0 );
}

static void DeInitializeUserColors( void )
{
    unsigned int i;
    for( i = 0; i < UserColorTableArray->GetCount(); i++ ) {
        colTable *ct = (colTable *) UserColorTableArray->Item( i );

        for( unsigned int j = 0; j < ct->color->GetCount(); j++ ) {
            S52color *c = (S52color *) ct->color->Item( j );
            delete c;                     //color
        }

        delete ct->tableName;               // wxString
        delete ct->color;                   // wxArrayPtrVoid

        delete ct;                          // colTable
    }

    delete UserColorTableArray;

    for( i = 0; i < UserColourHashTableArray->GetCount(); i++ ) {
        wxColorHashMap *phash = (wxColorHashMap *) UserColourHashTableArray->Item( i );
        delete phash;
    }

    delete UserColourHashTableArray;

}

#ifdef __WXMSW__

#define NCOLORS 40

typedef struct _MSW_COLOR_SPEC {
    int COLOR_NAME;
    wxString S52_RGB_COLOR;
    int SysRGB_COLOR;
} MSW_COLOR_SPEC;

MSW_COLOR_SPEC color_spec[] = { { COLOR_MENU, _T("UIBCK"), 0 }, { COLOR_MENUTEXT, _T("UITX1"), 0 },
        { COLOR_BTNSHADOW, _T("UIBCK"), 0 },                        // Menu Frame
        { -1, _T(""), 0 } };

void SaveSystemColors()
{
    /*
     color_3dface = pGetSysColor(COLOR_3DFACE);
     color_3dhilite = pGetSysColor(COLOR_3DHILIGHT);
     color_3dshadow = pGetSysColor(COLOR_3DSHADOW);
     color_3ddkshadow = pGetSysColor(COLOR_3DDKSHADOW);
     color_3dlight = pGetSysColor(COLOR_3DLIGHT);
     color_activecaption = pGetSysColor(COLOR_ACTIVECAPTION);
     color_gradientactivecaption = pGetSysColor(27); //COLOR_3DLIGHT);
     color_captiontext = pGetSysColor(COLOR_CAPTIONTEXT);
     color_windowframe = pGetSysColor(COLOR_WINDOWFRAME);
     color_inactiveborder = pGetSysColor(COLOR_INACTIVEBORDER);
     */
    //    Record the default system color in my substitution structure
    MSW_COLOR_SPEC *pcspec = &color_spec[0];
    while( pcspec->COLOR_NAME != -1 ) {
        pcspec->SysRGB_COLOR = pGetSysColor( pcspec->COLOR_NAME );
        pcspec++;
    }
}

void RestoreSystemColors()
{
    int element[NCOLORS];
    int rgbcolor[NCOLORS];
    int i = 0;

    MSW_COLOR_SPEC *pcspec = &color_spec[0];
    while( pcspec->COLOR_NAME != -1 ) {
        element[i] = pcspec->COLOR_NAME;
        rgbcolor[i] = pcspec->SysRGB_COLOR;

        pcspec++;
        i++;
    }

    pSetSysColors( i, (unsigned long *) &element[0], (unsigned long *) &rgbcolor[0] );

}

#endif

void SetSystemColors( ColorScheme cs )
{//---------------
#ifdef __WXMSW__
    int element[NCOLORS];
    int rgbcolor[NCOLORS];
    int i = 0;
    if( ( GLOBAL_COLOR_SCHEME_DUSK == cs ) || ( GLOBAL_COLOR_SCHEME_NIGHT == cs ) ) {
        MSW_COLOR_SPEC *pcspec = &color_spec[0];
        while( pcspec->COLOR_NAME != -1 ) {
            wxColour color = GetGlobalColor( pcspec->S52_RGB_COLOR );
            rgbcolor[i] = ( color.Red() << 16 ) + ( color.Green() << 8 ) + color.Blue();
            element[i] = pcspec->COLOR_NAME;

            i++;
            pcspec++;
        }

        pSetSysColors( i, (unsigned long *) &element[0], (unsigned long *) &rgbcolor[0] );

    } else {         // for daylight colors, use default windows colors as saved....

        RestoreSystemColors();
    }
#endif
}

wxColor GetDimColor(wxColor c)
{
    if( (global_color_scheme == GLOBAL_COLOR_SCHEME_DAY) || (global_color_scheme == GLOBAL_COLOR_SCHEME_RGB))
        return c;
    
    float factor = 1.0;
    if(global_color_scheme == GLOBAL_COLOR_SCHEME_DUSK)
        factor = 0.5;
    if(global_color_scheme == GLOBAL_COLOR_SCHEME_NIGHT)
        factor = 0.25;
    
    wxImage::RGBValue rgb( c.Red(), c.Green(), c.Blue() );
    wxImage::HSVValue hsv = wxImage::RGBtoHSV( rgb );
    hsv.value = hsv.value * factor;
    wxImage::RGBValue nrgb = wxImage::HSVtoRGB( hsv );

    return wxColor( nrgb.red, nrgb.green, nrgb.blue );
}

BEGIN_EVENT_TABLE(OCPNMessageDialog, wxDialog)
EVT_BUTTON(wxID_YES, OCPNMessageDialog::OnYes)
EVT_BUTTON(wxID_NO, OCPNMessageDialog::OnNo)
EVT_BUTTON(wxID_CANCEL, OCPNMessageDialog::OnCancel)
EVT_CLOSE(OCPNMessageDialog::OnClose)
END_EVENT_TABLE()


OCPNMessageDialog::OCPNMessageDialog( wxWindow *parent,
                                                const wxString& message,
                                                const wxString& caption,
                                                long style,
                                                const wxPoint& pos)
: wxDialog( parent, wxID_ANY, caption, pos, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP )
{
    m_style = style;
    wxFont *qFont = GetOCPNScaledFont(_("Dialog"));
    SetFont( *qFont );

    wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *icon_text = new wxBoxSizer( wxHORIZONTAL );

#if wxUSE_STATBMP
    // 1) icon
    if (style & wxICON_MASK)
    {
        wxBitmap bitmap;
        switch ( style & wxICON_MASK )
        {
            default:
                wxFAIL_MSG(_T("incorrect log style"));
                // fall through

            case wxICON_ERROR:
                bitmap = wxArtProvider::GetIcon(wxART_ERROR, wxART_MESSAGE_BOX);
                break;

            case wxICON_INFORMATION:
                bitmap = wxArtProvider::GetIcon(wxART_INFORMATION, wxART_MESSAGE_BOX);
                break;

            case wxICON_WARNING:
                bitmap = wxArtProvider::GetIcon(wxART_WARNING, wxART_MESSAGE_BOX);
                break;

            case wxICON_QUESTION:
                bitmap = wxArtProvider::GetIcon(wxART_QUESTION, wxART_MESSAGE_BOX);
                break;
        }
        wxStaticBitmap *icon = new wxStaticBitmap(this, wxID_ANY, bitmap);
        icon_text->Add( icon, 0, wxCENTER );
    }
#endif // wxUSE_STATBMP

#if wxUSE_STATTEXT
    // 2) text
    icon_text->Add( CreateTextSizer( message ), 0, wxALIGN_CENTER | wxLEFT, 10 );

    topsizer->Add( icon_text, 1, wxCENTER | wxLEFT|wxRIGHT|wxTOP, 10 );
#endif // wxUSE_STATTEXT

    // 3) buttons
    int AllButtonSizerFlags = wxOK|wxCANCEL|wxYES|wxNO|wxHELP|wxNO_DEFAULT;
    int center_flag = wxEXPAND;
    if (style & wxYES_NO)
        center_flag = wxALIGN_CENTRE;
    wxSizer *sizerBtn = CreateSeparatedButtonSizer(style & AllButtonSizerFlags);
    if ( sizerBtn )
        topsizer->Add(sizerBtn, 0, center_flag | wxALL, 10 );

    SetAutoLayout( true );
    SetSizer( topsizer );

    topsizer->SetSizeHints( this );
    topsizer->Fit( this );
    wxSize size( GetSize() );
    if (size.x < size.y*3/2)
    {
        size.x = size.y*3/2;
        SetSize( size );
    }

    Centre( wxBOTH | wxCENTER_FRAME);
}

wxColour GetDialogColor(DialogColor color)
{
    wxColour col = *wxRED;

    bool bUseSysColors = false;
    bool bIsDarkMode = false;
#ifdef __WXOSX__
    if( wxPlatformInfo::Get().CheckOSVersion(10, 14) )
        bUseSysColors = true;
#endif
#ifdef __WXGTK__
    bUseSysColors= true;
#endif

    if(bUseSysColors) {
        wxColour bg = wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE);
        if(bg.Red() < 128) {
            bIsDarkMode = true;
        }

    }

    switch(color) {
        case DLG_BACKGROUND:
            if(bUseSysColors && bIsDarkMode) {
                col = wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE);
            } else {
                col = GetGlobalColor("DILG0");
            }
            break;
        case DLG_SELECTED_BACKGROUND:
            if(bUseSysColors && bIsDarkMode) {
                col = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
            } else {
                col = GetGlobalColor("DILG1");
            }
            break;
        case DLG_UNSELECTED_BACKGROUND:
            if(bUseSysColors && bIsDarkMode) {
                col = wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE);
            } else {
                col = GetGlobalColor("DILG0");
            }
            break;
        case DLG_ACCENT:
        case DLG_SELECTED_ACCENT:
            if(bUseSysColors && bIsDarkMode) {
                col = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
            } else {
                col = GetGlobalColor("DILG3");
            }
            break;
        case DLG_UNSELECTED_ACCENT:
            if(bUseSysColors && bIsDarkMode) {
                col = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
            } else {
                col = GetGlobalColor("DILG1");
            }
            break;
        case DLG_TEXT:
            if(bUseSysColors && bIsDarkMode) {
                col = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
            } else {
                col = GetGlobalColor("DILG3");
            }
            break;
        case DLG_HIGHLIGHT:
            if(bUseSysColors && bIsDarkMode) {
                col = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
            } else {
                col = GetGlobalColor("UIBCK");
            }
            break;
    }
    return col;
}

void OCPNMessageDialog::OnYes(wxCommandEvent& WXUNUSED(event))
{
    SetReturnCode(wxID_YES);
    EndModal( wxID_YES );
}

void OCPNMessageDialog::OnNo(wxCommandEvent& WXUNUSED(event))
{
    SetReturnCode(wxID_NO);
    EndModal( wxID_NO );
}

void OCPNMessageDialog::OnCancel(wxCommandEvent& WXUNUSED(event))
{
    // Allow cancellation via ESC/Close button except if
    // only YES and NO are specified.
    if ( (m_style & wxYES_NO) != wxYES_NO || (m_style & wxCANCEL) )
    {
        SetReturnCode(wxID_CANCEL);
        EndModal( wxID_CANCEL );
    }
}

void OCPNMessageDialog::OnClose( wxCloseEvent& event )
{
    SetReturnCode(wxID_CANCEL);
    EndModal( wxID_CANCEL );
}




class TimedMessageBox:wxEvtHandler
{
public:
    TimedMessageBox(wxWindow* parent, const wxString& message,
                    const wxString& caption = _T("Message box"), long style = wxOK | wxCANCEL,
                    int timeout_sec = -1, const wxPoint& pos = wxDefaultPosition );
    ~TimedMessageBox();
    int GetRetVal(void){ return ret_val; }
    void OnTimer(wxTimerEvent &evt);

    wxTimer     m_timer;
    OCPNMessageDialog *dlg;
    int         ret_val;

    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(TimedMessageBox, wxEvtHandler)
EVT_TIMER(-1, TimedMessageBox::OnTimer)
END_EVENT_TABLE()

TimedMessageBox::TimedMessageBox(wxWindow* parent, const wxString& message,
                                 const wxString& caption, long style, int timeout_sec, const wxPoint& pos )
{
    ret_val = 0;
    m_timer.SetOwner( this, -1 );

    if(timeout_sec > 0)
        m_timer.Start( timeout_sec * 1000, wxTIMER_ONE_SHOT );

    dlg = new OCPNMessageDialog( parent, message, caption, style, pos );
    dlg->ShowModal();

    int ret= dlg->GetReturnCode();

    //  Not sure why we need this, maybe on wx3?
    if( ((style & wxYES_NO) == wxYES_NO) && (ret == wxID_OK))
        ret = wxID_YES;

    delete dlg;
    dlg = NULL;

    ret_val = ret;
}


TimedMessageBox::~TimedMessageBox()
{
}

void TimedMessageBox::OnTimer(wxTimerEvent &evt)
{
    if( dlg )
        dlg->EndModal( wxID_CANCEL );
}






int OCPNMessageBox( wxWindow *parent, const wxString& message, const wxString& caption, int style,
                    int timeout_sec, int x, int y  )
{
    int ret =  wxID_OK;

    TimedMessageBox tbox(parent, message, caption, style, timeout_sec, wxPoint( x, y )  );
    ret = tbox.GetRetVal() ;

    return ret;
}

//               A helper function to check for proper parameters of anchor watch
//
double AnchorDistFix( double const d, double const AnchorPointMinDist,
        double const AnchorPointMaxDist )   //  pjotrc 2010.02.22
{
    if( d >= 0.0 ) if( d < AnchorPointMinDist ) return AnchorPointMinDist;
    else
        if( d > AnchorPointMaxDist ) return AnchorPointMaxDist;
        else
            return d;

    else
        //if ( d < 0.0 )
        if( d > -AnchorPointMinDist ) return -AnchorPointMinDist;
        else
            if( d < -AnchorPointMaxDist ) return -AnchorPointMaxDist;
            else
                return d;
}

//      Auto timed popup Window implementation

BEGIN_EVENT_TABLE(TimedPopupWin, wxWindow) EVT_PAINT(TimedPopupWin::OnPaint)
EVT_TIMER(POPUP_TIMER, TimedPopupWin::OnTimer)

END_EVENT_TABLE()

// Define a constructor
TimedPopupWin::TimedPopupWin( wxWindow *parent, int timeout ) :
wxWindow( parent, wxID_ANY, wxPoint( 0, 0 ), wxSize( 1, 1 ), wxNO_BORDER )
{
    m_pbm = NULL;

    m_timer_timeout.SetOwner( this, POPUP_TIMER );
    m_timeout_sec = timeout;
    isActive = false;
    Hide();
}

TimedPopupWin::~TimedPopupWin()
{
    delete m_pbm;
}
void TimedPopupWin::OnTimer( wxTimerEvent& event )
{
    if( IsShown() )
        Hide();
}


void TimedPopupWin::SetBitmap( wxBitmap &bmp )
{
    delete m_pbm;
    m_pbm = new wxBitmap( bmp );

    // Retrigger the auto timeout
    if( m_timeout_sec > 0 )
        m_timer_timeout.Start( m_timeout_sec * 1000, wxTIMER_ONE_SHOT );
}

void TimedPopupWin::OnPaint( wxPaintEvent& event )
{
    int width, height;
    GetClientSize( &width, &height );
    wxPaintDC dc( this );

    wxMemoryDC mdc;
    mdc.SelectObject( *m_pbm );
    dc.Blit( 0, 0, width, height, &mdc, 0, 0 );

}


//      Console supporting printf functionality for Windows GUI app

#ifdef __WXMSW__
static const WORD MAX_CONSOLE_LINES = 500;  // maximum mumber of lines the output console should have

//#ifdef _DEBUG

void RedirectIOToConsole()

{

    int hConHandle;

    wxIntPtr lStdHandle;

    CONSOLE_SCREEN_BUFFER_INFO coninfo;

    FILE *fp;

    // allocate a console for this app

    AllocConsole();

    // set the screen buffer to be big enough to let us scroll text

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
    coninfo.dwSize.Y = MAX_CONSOLE_LINES;
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE),coninfo.dwSize);

    // redirect unbuffered STDOUT to the console

    lStdHandle = (wxIntPtr)GetStdHandle(STD_OUTPUT_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    fp = _fdopen( hConHandle, "w" );
    *stdout = *fp;
    setvbuf( stdout, NULL, _IONBF, 0 );


    // redirect unbuffered STDIN to the console

    lStdHandle = (wxIntPtr)GetStdHandle(STD_INPUT_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    fp = _fdopen( hConHandle, "r" );
    *stdin = *fp;
    setvbuf( stdin, NULL, _IONBF, 0 );

    // redirect unbuffered STDERR to the console

    lStdHandle = (wxIntPtr)GetStdHandle(STD_ERROR_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    fp = _fdopen( hConHandle, "w" );
    *stderr = *fp;
    setvbuf( stderr, NULL, _IONBF, 0 );

    // make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog point to console as well

    //ios::sync_with_stdio();

}

//#endif
#endif


#ifdef __WXMSW__
bool TestGLCanvas(wxString prog_dir)
{
#ifdef __MSVC__
    wxString test_app = prog_dir;
    test_app += _T("ocpn_gltest1.exe");

    if(::wxFileExists(test_app)){
        long proc_return = ::wxExecute(test_app, wxEXEC_SYNC);
        printf("OpenGL Test Process returned %0X\n", proc_return);
        if(proc_return == 0)
            printf("GLCanvas OK\n");
        else
            printf("GLCanvas failed to start, disabling OpenGL.\n");

        return (proc_return == 0);
    }
    else
        return true;
#else
    /* until we can get the source to ocpn_gltest1 assume true for mingw */
    return true;
#endif
}
#endif



wxFont *GetOCPNScaledFont( wxString item, int default_size )
{
    wxFont *dFont = FontMgr::Get().GetFont( item, default_size );
    int req_size = dFont->GetPointSize();

    if( g_bresponsive ){
        //      Adjust font size to be no smaller than xx mm actual size
        double scaled_font_size = dFont->GetPointSize();

        {

            double points_per_mm  = g_Platform->getFontPointsperPixel() * g_Platform->GetDisplayDPmm();
            double min_scaled_font_size = 3 * points_per_mm;    // smaller than 3 mm is unreadable
            int nscaled_font_size = wxMax( wxRound(scaled_font_size), min_scaled_font_size );

            if(req_size >= nscaled_font_size)
                return dFont;
            else{
                wxFont *qFont = FontMgr::Get().FindOrCreateFont( nscaled_font_size,
                                                             dFont->GetFamily(),
                                                             dFont->GetStyle(),
                                                             dFont->GetWeight());
                return qFont;
            }
        }
    }
    return dFont;
}

wxFont GetOCPNGUIScaledFont( wxString item )
{
    wxFont *dFont = FontMgr::Get().GetFont( item, 0 );
    int req_size = dFont->GetPointSize();
    wxFont qFont = *dFont;

    if( g_bresponsive ){
       double postmult =  exp( g_GUIScaleFactor * (0.693 / 5.0) );       //  exp(2)
       double scaled_font_size = dFont->GetPointSize() * postmult;

       double points_per_mm  = g_Platform->getFontPointsperPixel() * g_Platform->GetDisplayDPmm();
       double min_scaled_font_size = 3 * points_per_mm;    // smaller than 3 mm is unreadable
       int nscaled_font_size = wxMax( wxRound(scaled_font_size), min_scaled_font_size );

//        wxFont *qFont = wxTheFontList->FindOrCreateFont( nscaled_font_size,
//                                                                  dFont->GetFamily(),
//                                                                  dFont->GetStyle(),
//                                                                  dFont->GetWeight());
       qFont.SetPointSize(nscaled_font_size);
    }

    return qFont;
}

OCPN_ThreadMessageEvent::OCPN_ThreadMessageEvent(wxEventType commandType, int id)
:wxEvent(id, commandType)
{
}

OCPN_ThreadMessageEvent::~OCPN_ThreadMessageEvent()
{
}

wxEvent* OCPN_ThreadMessageEvent::Clone() const
{
    OCPN_ThreadMessageEvent *newevent=new OCPN_ThreadMessageEvent(*this);
    newevent->m_string=this->m_string;
    return newevent;
}




#if 0
/* ************************************************************************
 * Serial port enumeration routines
 *
 * The EnumSerialPort function will populate an array of SSerInfo structs,
 * each of which contains information about one serial port present in
 * the system. Note that this code must be linked with setupapi.lib,
 * which is included with the Win32 SDK.
 *
 * by Zach Gorman <gormanjz@hotmail.com>
 *
 * Copyright (c) 2002 Archetype Auction Software, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following condition is
 * met: Redistributions of source code must retain the above copyright
 * notice, this condition and the following disclaimer.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ARCHETYPE AUCTION SOFTWARE OR ITS
 * AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ************************************************************************/

// For MFC
#include <stdafx.h>

// The next 3 includes are needed for serial port enumeration
#include <objbase.h>
#include <initguid.h>
#include <Setupapi.h>

#include "EnumSerial.h"

// The following define is from ntddser.h in the DDK. It is also
// needed for serial port enumeration.
#ifndef GUID_CLASS_COMPORT
DEFINE_GUID(GUID_CLASS_COMPORT, 0x86e0d1e0L, 0x8089, 0x11d0, 0x9c, 0xe4, \
0x08, 0x00, 0x3e, 0x30, 0x1f, 0x73);
#endif


struct SSerInfo {
    SSerInfo() : bUsbDevice(FALSE) {}
    CString strDevPath;          // Device path for use with CreateFile()
    CString strPortName;         // Simple name (i.e. COM1)
    CString strFriendlyName;     // Full name to be displayed to a user
    BOOL bUsbDevice;             // Provided through a USB connection?
    CString strPortDesc;         // friendly name without the COMx
};

//---------------------------------------------------------------
// Helpers for enumerating the available serial ports.
// These throw a CString on failure, describing the nature of
// the error that occurred.

void EnumPortsWdm(CArray<SSerInfo,SSerInfo&> &asi);
void EnumPortsWNt4(CArray<SSerInfo,SSerInfo&> &asi);
void EnumPortsW9x(CArray<SSerInfo,SSerInfo&> &asi);
void SearchPnpKeyW9x(HKEY hkPnp, BOOL bUsbDevice,
                     CArray<SSerInfo,SSerInfo&> &asi);


//---------------------------------------------------------------
// Routine for enumerating the available serial ports.
// Throws a CString on failure, describing the error that
// occurred. If bIgnoreBusyPorts is TRUE, ports that can't
// be opened for read/write access are not included.

void EnumSerialPorts(CArray<SSerInfo,SSerInfo&> &asi, BOOL bIgnoreBusyPorts)
{
    // Clear the output array
    asi.RemoveAll();

    // Use different techniques to enumerate the available serial
    // ports, depending on the OS we're using
    OSVERSIONINFO vi;
    vi.dwOSVersionInfoSize = sizeof(vi);
    if (!::GetVersionEx(&vi)) {
        CString str;
        str.Format("Could not get OS version. (err=%lx)",
                   GetLastError());
        throw str;
    }
    // Handle windows 9x and NT4 specially
    if (vi.dwMajorVersion < 5) {
        if (vi.dwPlatformId == VER_PLATFORM_WIN32_NT)
            EnumPortsWNt4(asi);
        else
            EnumPortsW9x(asi);
    }
    else {
        // Win2k and later support a standard API for
        // enumerating hardware devices.
        EnumPortsWdm(asi);
    }

    for (int ii=0; ii<asi.GetSize(); ii++)
    {
        SSerInfo& rsi = asi[ii];
        if (bIgnoreBusyPorts) {
            // Only display ports that can be opened for read/write
            HANDLE hCom = CreateFile(rsi.strDevPath,
                                     GENERIC_READ | GENERIC_WRITE,
                                     0,    /* comm devices must be opened w/exclusive-access */
                                     NULL, /* no security attrs */
                                     OPEN_EXISTING, /* comm devices must use OPEN_EXISTING */
                                     0,    /* not overlapped I/O */
                                     NULL  /* hTemplate must be NULL for comm devices */
            );
            if (hCom == INVALID_HANDLE_VALUE) {
                // It can't be opened; remove it.
                asi.RemoveAt(ii);
                ii--;
                continue;
            }
            else {
                // It can be opened! Close it and add it to the list
                ::CloseHandle(hCom);
            }
        }

        // Come up with a name for the device.
        // If there is no friendly name, use the port name.
        if (rsi.strFriendlyName.IsEmpty())
            rsi.strFriendlyName = rsi.strPortName;

        // If there is no description, try to make one up from
            // the friendly name.
            if (rsi.strPortDesc.IsEmpty()) {
                // If the port name is of the form "ACME Port (COM3)"
                // then strip off the " (COM3)"
                rsi.strPortDesc = rsi.strFriendlyName;
                int startdex = rsi.strPortDesc.Find(" (");
                int enddex = rsi.strPortDesc.Find(")");
                if (startdex > 0 && enddex ==
                    (rsi.strPortDesc.GetLength()-1))
                    rsi.strPortDesc = rsi.strPortDesc.Left(startdex);
            }
    }
}

// Helpers for EnumSerialPorts

void EnumPortsWdm(CArray<SSerInfo,SSerInfo&> &asi)
{
    CString strErr;
    // Create a device information set that will be the container for
    // the device interfaces.
    GUID *guidDev = (GUID*) &GUID_CLASS_COMPORT;

    HDEVINFO hDevInfo = INVALID_HANDLE_VALUE;
    SP_DEVICE_INTERFACE_DETAIL_DATA *pDetData = NULL;

    try {
        hDevInfo = SetupDiGetClassDevs( guidDev,
                                        NULL,
                                        NULL,
                                        DIGCF_PRESENT | DIGCF_DEVICEINTERFACE
        );

        if(hDevInfo == INVALID_HANDLE_VALUE)
        {
            strErr.Format("SetupDiGetClassDevs failed. (err=%lx)",
                          GetLastError());
            throw strErr;
        }

        // Enumerate the serial ports
        BOOL bOk = TRUE;
        SP_DEVICE_INTERFACE_DATA ifcData;
        DWORD dwDetDataSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA) + 256;
        pDetData = (SP_DEVICE_INTERFACE_DETAIL_DATA*) new char[dwDetDataSize];
        // This is required, according to the documentation. Yes,
        // it's weird.
        ifcData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
        pDetData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
        for (DWORD ii=0; bOk; ii++) {
            bOk = SetupDiEnumDeviceInterfaces(hDevInfo,
                                              NULL, guidDev, ii, &ifcData);
            if (bOk) {
                // Got a device. Get the details.
                SP_DEVINFO_DATA devdata = {sizeof(SP_DEVINFO_DATA)};
                bOk = SetupDiGetDeviceInterfaceDetail(hDevInfo,
                                                      &ifcData, pDetData, dwDetDataSize, NULL, &devdata);
                if (bOk) {
                    CString strDevPath(pDetData->DevicePath);
                    // Got a path to the device. Try to get some more info.
                    TCHAR fname[256];
                    TCHAR desc[256];
                    BOOL bSuccess = SetupDiGetDeviceRegistryProperty(
                        hDevInfo, &devdata, SPDRP_FRIENDLYNAME, NULL,
                        (PBYTE)fname, sizeof(fname), NULL);
                    bSuccess = bSuccess && SetupDiGetDeviceRegistryProperty(
                        hDevInfo, &devdata, SPDRP_DEVICEDESC, NULL,
                        (PBYTE)desc, sizeof(desc), NULL);
                    BOOL bUsbDevice = FALSE;
                    TCHAR locinfo[256];
                    if (SetupDiGetDeviceRegistryProperty(
                        hDevInfo, &devdata, SPDRP_LOCATION_INFORMATION, NULL,
                        (PBYTE)locinfo, sizeof(locinfo), NULL))
                    {
                        // Just check the first three characters to determine
                        // if the port is connected to the USB bus. This isn't
                        // an infallible method; it would be better to use the
                        // BUS GUID. Currently, Windows doesn't let you query
                        // that though (SPDRP_BUSTYPEGUID seems to exist in
                        // documentation only).
                        bUsbDevice = (strncmp(locinfo, "USB", 3)==0);
                    }
                    if (bSuccess) {
                        // Add an entry to the array
                        SSerInfo si;
                        si.strDevPath = strDevPath;
                        si.strFriendlyName = fname;
                        si.strPortDesc = desc;
                        si.bUsbDevice = bUsbDevice;
                        asi.Add(si);
                    }

                }
                else {
                    strErr.Format("SetupDiGetDeviceInterfaceDetail failed. (err=%lx)",
                                  GetLastError());
                    throw strErr;
                }
            }
            else {
                DWORD err = GetLastError();
                if (err != ERROR_NO_MORE_ITEMS) {
                    strErr.Format("SetupDiEnumDeviceInterfaces failed. (err=%lx)", err);
                    throw strErr;
                }
            }
        }
    }
    catch (CString strCatchErr) {
        strErr = strCatchErr;
    }

    if (pDetData != NULL)
        delete [] (char*)pDetData;
    if (hDevInfo != INVALID_HANDLE_VALUE)
        SetupDiDestroyDeviceInfoList(hDevInfo);

    if (!strErr.IsEmpty())
        throw strErr;
}

void EnumPortsWNt4(CArray<SSerInfo,SSerInfo&> &asi)
{
    // NT4's driver model is totally different, and not that
    // many people use NT4 anymore. Just try all the COM ports
    // between 1 and 16
    SSerInfo si;
    for (int ii=1; ii<=16; ii++) {
        CString strPort;
        strPort.Format("COM%d",ii);
        si.strDevPath = CString("\\\\.\\") + strPort;
        si.strPortName = strPort;
        asi.Add(si);
    }
}

void EnumPortsW9x(CArray<SSerInfo,SSerInfo&> &asi)
{
    // Look at all keys in HKLM\Enum, searching for subkeys named
    // *PNP0500 and *PNP0501. Within these subkeys, search for
    // sub-subkeys containing value entries with the name "PORTNAME"
    // Search all subkeys of HKLM\Enum\USBPORTS for PORTNAME entries.

    // First, open HKLM\Enum
    HKEY hkEnum = NULL;
    HKEY hkSubEnum = NULL;
    HKEY hkSubSubEnum = NULL;

    try {
        if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Enum", 0, KEY_READ,
            &hkEnum) != ERROR_SUCCESS)
            throw CString("Could not read from HKLM\\Enum");

        // Enumerate the subkeys of HKLM\Enum
            char acSubEnum[128];
            DWORD dwSubEnumIndex = 0;
            DWORD dwSize = sizeof(acSubEnum);
            while (RegEnumKeyEx(hkEnum, dwSubEnumIndex++, acSubEnum, &dwSize,
                NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
            {
                HKEY hkSubEnum = NULL;
                if (RegOpenKeyEx(hkEnum, acSubEnum, 0, KEY_READ,
                    &hkSubEnum) != ERROR_SUCCESS)
                    throw CString("Could not read from HKLM\\Enum\\")+acSubEnum;

                // Enumerate the subkeys of HKLM\Enum\*\, looking for keys
                    // named *PNP0500 and *PNP0501 (or anything in USBPORTS)
                    BOOL bUsbDevice = (strcmp(acSubEnum,"USBPORTS")==0);
                    char acSubSubEnum[128];
                    dwSize = sizeof(acSubSubEnum);  // set the buffer size
                    DWORD dwSubSubEnumIndex = 0;
                    while (RegEnumKeyEx(hkSubEnum, dwSubSubEnumIndex++, acSubSubEnum,
                        &dwSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
                    {
                        BOOL bMatch = (strcmp(acSubSubEnum,"*PNP0500")==0 ||
                        strcmp(acSubSubEnum,"*PNP0501")==0 ||
                        bUsbDevice);
                        if (bMatch) {
                            HKEY hkSubSubEnum = NULL;
                            if (RegOpenKeyEx(hkSubEnum, acSubSubEnum, 0, KEY_READ,
                                &hkSubSubEnum) != ERROR_SUCCESS)
                                throw CString("Could not read from HKLM\\Enum\\") +
                                acSubEnum + "\\" + acSubSubEnum;
                            SearchPnpKeyW9x(hkSubSubEnum, bUsbDevice, asi);
                            RegCloseKey(hkSubSubEnum);
                            hkSubSubEnum = NULL;
                        }

                        dwSize = sizeof(acSubSubEnum);  // restore the buffer size
                    }

                    RegCloseKey(hkSubEnum);
                    hkSubEnum = NULL;
                    dwSize = sizeof(acSubEnum); // restore the buffer size
            }
    }
    catch (CString strError) {
        if (hkEnum != NULL)
            RegCloseKey(hkEnum);
        if (hkSubEnum != NULL)
            RegCloseKey(hkSubEnum);
        if (hkSubSubEnum != NULL)
            RegCloseKey(hkSubSubEnum);
        throw strError;
    }

    RegCloseKey(hkEnum);
}

void SearchPnpKeyW9x(HKEY hkPnp, BOOL bUsbDevice,
                     CArray<SSerInfo,SSerInfo&> &asi)
{
    // Enumerate the subkeys of the given PNP key, looking for values with
    // the name "PORTNAME"
    // First, open HKLM\Enum
    HKEY hkSubPnp = NULL;

    try {
        // Enumerate the subkeys of HKLM\Enum\*\PNP050[01]
        char acSubPnp[128];
        DWORD dwSubPnpIndex = 0;
        DWORD dwSize = sizeof(acSubPnp);
        while (RegEnumKeyEx(hkPnp, dwSubPnpIndex++, acSubPnp, &dwSize,
            NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
        {
            HKEY hkSubPnp = NULL;
            if (RegOpenKeyEx(hkPnp, acSubPnp, 0, KEY_READ,
                &hkSubPnp) != ERROR_SUCCESS)
                throw CString("Could not read from HKLM\\Enum\\...\\")
                + acSubPnp;

            // Look for the PORTNAME value
                char acValue[128];
                dwSize = sizeof(acValue);
                if (RegQueryValueEx(hkSubPnp, "PORTNAME", NULL, NULL, (BYTE*)acValue,
                    &dwSize) == ERROR_SUCCESS)
                {
                    CString strPortName(acValue);

                    // Got the portname value. Look for a friendly name.
                    CString strFriendlyName;
                    dwSize = sizeof(acValue);
                    if (RegQueryValueEx(hkSubPnp, "FRIENDLYNAME", NULL, NULL, (BYTE*)acValue,
                        &dwSize) == ERROR_SUCCESS)
                        strFriendlyName = acValue;

                    // Prepare an entry for the output array.
                        SSerInfo si;
                        si.strDevPath = CString("\\\\.\\") + strPortName;
                        si.strPortName = strPortName;
                        si.strFriendlyName = strFriendlyName;
                        si.bUsbDevice = bUsbDevice;

                        // Overwrite duplicates.
                        BOOL bDup = FALSE;
                        for (int ii=0; ii<asi.GetSize() && !bDup; ii++)
                        {
                            if (asi[ii].strPortName == strPortName) {
                                bDup = TRUE;
                                asi[ii] = si;
                            }
                        }
                        if (!bDup) {
                            // Add an entry to the array
                            asi.Add(si);
                        }
                }

                RegCloseKey(hkSubPnp);
                hkSubPnp = NULL;
                dwSize = sizeof(acSubPnp);  // restore the buffer size
        }
    }
    catch (CString strError) {
        if (hkSubPnp != NULL)
            RegCloseKey(hkSubPnp);
        throw strError;
    }
}

#endif


bool ReloadLocale()
{
    bool ret = false;

#if wxUSE_XLOCALE    
    ret = (!g_Platform->ChangeLocale( g_locale, plocale_def_lang, &plocale_def_lang ).IsEmpty());
#endif
    return ret;
}


void ApplyLocale()
{
    FontMgr::Get().SetLocale( g_locale );
    FontMgr::Get().ScrubList(); 

    //  Close and re-init various objects to allow new locale to show.
    delete g_options;
    g_options = NULL;
    g_pOptions = NULL;


    if( pRoutePropDialog ) {
        pRoutePropDialog->Hide();
        pRoutePropDialog->Destroy();
        pRoutePropDialog = NULL;
    }

    if( pRouteManagerDialog ) {
        pRouteManagerDialog->Hide();
        pRouteManagerDialog->Destroy();
        pRouteManagerDialog = NULL;
    }

    if(console)
        console->SetColorScheme( global_color_scheme );

    if( g_pais_query_dialog_active ){
        g_pais_query_dialog_active->Destroy();
        g_pais_query_dialog_active = NULL;
    }

    if( g_pais_alert_dialog_active ){
        g_pais_alert_dialog_active->Destroy();
        g_pais_alert_dialog_active = NULL;
    }


    if( g_pAISTargetList ) {
        if(g_pauimgr) g_pauimgr->DetachPane(g_pAISTargetList);
        g_pAISTargetList->Disconnect_decoder();
        g_pAISTargetList->Destroy();
        g_pAISTargetList = NULL;
    }

    //  Process the menubar, if present.
    if ( gFrame->m_pMenuBar ) {     // remove the menu bar if it is presently enabled
            gFrame->SetMenuBar( NULL );
            gFrame->m_pMenuBar->Destroy();
            gFrame->m_pMenuBar = NULL;
    }
    gFrame->BuildMenuBar();

    //  Give all canvas a chance to update, if needed
    for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
        ChartCanvas *cc = g_canvasArray.Item(i);
        if(cc)
            cc->CanvasApplyLocale();
    }

    // Capture a copy of the current perspective
    //  So that we may restore PlugIn window sizes, position, visibility, etc.
    wxString perspective;
    pConfig->SetPath( _T ( "/AUI" ) );
    pConfig->Read( _T ( "AUIPerspective" ), &perspective );

    //  Compliant Plugins will reload their locale message catalog during the Init() method.
    //  So it is sufficient to simply deactivate, and then re-activate, all "active" plugins.
    g_pi_manager->DeactivateAllPlugIns();
    g_pi_manager->UpdatePlugIns();


    //         // Make sure the perspective saved in the config file is "reasonable"
    //         // In particular, the perspective should have an entry for every
    //         // windows added to the AUI manager so far.
    //         // If any are not found, then use the default layout
    //         
    bool bno_load = false;
    wxAuiPaneInfoArray pane_array_val = g_pauimgr->GetAllPanes();

    for( unsigned int i = 0; i < pane_array_val.GetCount(); i++ ) {
        wxAuiPaneInfo pane = pane_array_val[i];
        if( perspective.Find( pane.name ) == wxNOT_FOUND ) {
            bno_load = true;
            break;
        }
    }

     if( !bno_load )
         g_pauimgr->LoadPerspective( perspective, false );

    g_pauimgr->Update();

    if(gFrame){
        gFrame->RequestNewToolbars( true );
        gFrame->RequestNewMasterToolbar( true );
    }
}


BEGIN_EVENT_TABLE(OCPN_TimedHTMLMessageDialog, wxDialog)
EVT_BUTTON(wxID_YES, OCPN_TimedHTMLMessageDialog::OnYes)
EVT_BUTTON(wxID_NO, OCPN_TimedHTMLMessageDialog::OnNo)
EVT_BUTTON(wxID_CANCEL, OCPN_TimedHTMLMessageDialog::OnCancel)
EVT_CLOSE(OCPN_TimedHTMLMessageDialog::OnClose)
EVT_TIMER(-1, OCPN_TimedHTMLMessageDialog::OnTimer)

END_EVENT_TABLE()


OCPN_TimedHTMLMessageDialog::OCPN_TimedHTMLMessageDialog( wxWindow *parent,
                                                    const wxString& message,
                                                    const wxString& caption,
                                                    int tSeconds,
                                                    long style,
                                                    bool bFixedFont,
                                                    const wxPoint& pos)
: wxDialog( parent, wxID_ANY, caption, pos, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP )
{
    m_style = style;
    if(bFixedFont){
        wxFont *dFont = GetOCPNScaledFont_PlugIn(_("Dialog"));
        double font_size = dFont->GetPointSize();
        wxFont *qFont = wxTheFontList->FindOrCreateFont( font_size,wxFONTFAMILY_TELETYPE, dFont->GetStyle(), dFont->GetWeight());
        SetFont( *qFont );
    }

    wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );

    msgWindow = new wxHtmlWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                                wxHW_SCROLLBAR_AUTO | wxHW_NO_SELECTION );
    msgWindow->SetBorders( 30 );

    topsizer->Add( msgWindow, 1, wxEXPAND, 5 );

    wxString html;
    html << message;

    wxCharBuffer buf = html.ToUTF8();
    if( buf.data() )                            // string OK?
       msgWindow->SetPage( html );

    // 3) buttons
       int AllButtonSizerFlags = wxOK|wxCANCEL|wxYES|wxNO|wxHELP|wxNO_DEFAULT;
       int center_flag = wxEXPAND;
       if (style & wxYES_NO)
           center_flag = wxALIGN_CENTRE;
       wxSizer *sizerBtn = CreateSeparatedButtonSizer(style & AllButtonSizerFlags);
       if ( sizerBtn )
           topsizer->Add(sizerBtn, 0, center_flag | wxALL, 10 );

       SetSizer( topsizer );

       topsizer->Fit( this );

       RecalculateSize();
//       wxSize szyv = msgWindow->GetVirtualSize();

//       SetClientSize(szyv.x + 20, szyv.y + 20); 

       CentreOnScreen();

       //msgWindow->SetBackgroundColour(wxColour(191, 183, 180));
       msgWindow->SetBackgroundColour(GetBackgroundColour());

       m_timer.SetOwner( this, -1 );

       if(tSeconds > 0)
           m_timer.Start( tSeconds * 1000, wxTIMER_ONE_SHOT );

}

void OCPN_TimedHTMLMessageDialog::RecalculateSize( void )
{
    wxSize esize;
    esize.x = GetCharWidth() * 60;
    int sx, sy;
    ::wxDisplaySize(&sx, &sy);
    esize.x = wxMin(esize.x, sx * 6 / 10);
    esize.y = -1;
    SetClientSize(esize);     // This will force a recalc of internal representation

    int height1 = msgWindow->GetInternalRepresentation()->GetHeight();

    int client_size_y = wxMin(::wxGetDisplaySize().y - 100, height1 + 70);    // Must fit on screen

    SetClientSize(wxSize(esize.x, client_size_y ));   // constant is 2xBorders + a little slop.

}

void OCPN_TimedHTMLMessageDialog::OnYes(wxCommandEvent& WXUNUSED(event))
{
    SetReturnCode(wxID_YES);
    if(IsModal())
        EndModal( wxID_YES );
    else
        Hide();
}

void OCPN_TimedHTMLMessageDialog::OnNo(wxCommandEvent& WXUNUSED(event))
{
    SetReturnCode(wxID_NO);
    if(IsModal())
        EndModal( wxID_NO );
    else
        Hide();
}

void OCPN_TimedHTMLMessageDialog::OnCancel(wxCommandEvent& WXUNUSED(event))
{
    // Allow cancellation via ESC/Close button except if
    // only YES and NO are specified.
    if ( (m_style & wxYES_NO) != wxYES_NO || (m_style & wxCANCEL) )
    {
        SetReturnCode(wxID_CANCEL);
        EndModal( wxID_CANCEL );
    }
}

void OCPN_TimedHTMLMessageDialog::OnClose( wxCloseEvent& event )
{
    SetReturnCode(wxID_CANCEL);
    if(IsModal())
        EndModal( wxID_CANCEL );
    else
        Hide();
}

void OCPN_TimedHTMLMessageDialog::OnTimer(wxTimerEvent &evt)
{
    if(IsModal())
        EndModal( wxID_YES );
    else
        Hide();
}

