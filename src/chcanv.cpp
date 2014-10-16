/***************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  Chart Canvas
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

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers
#include "wx/image.h"
#include <wx/graphics.h>
#include <wx/listbook.h>
#include <wx/clipbrd.h>
#include <wx/aui/aui.h>

#include "dychart.h"

#include <wx/listimpl.cpp>

#include "chcanv.h"

#include "geodesic.h"
#include "styles.h"
#include "routeman.h"
#include "statwin.h"
#include "navutil.h"
#include "kml.h"
#include "concanv.h"
#include "thumbwin.h"
#include "chartdb.h"
#include "chartimg.h"
#include "chart1.h"
#include "cutil.h"
#include "routeprop.h"
#include "TrackPropDlg.h"
#include "tcmgr.h"
#include "routemanagerdialog.h"
#include "pluginmanager.h"
#include "ocpn_pixel.h"
#include "ocpndc.h"
#include "undo.h"
#include "toolbar.h"
#include "multiplexer.h"
#include "timers.h"
#include "tide_time.h"
#include "glTextureDescriptor.h"
#include "ChInfoWin.h"
#include "Quilt.h"
#include "SelectItem.h"
#include "Select.h"
#include "FontMgr.h"
#include "AIS_Decoder.h"
#include "AIS_Target_Data.h"
#include "AISTargetAlertDialog.h"
#include "SendToGpsDlg.h"
#include "compasswin.h"
#include "OCPNRegion.h"
#include "gshhs.h"

#ifdef ocpnUSE_GL
#include "glChartCanvas.h"
#endif

#ifdef USE_S57
#include "cm93.h"                   // for chart outline draw
#include "s57chart.h"               // for ArrayOfS57Obj
#include "s52plib.h"
#endif

#include "ais.h"

#ifdef __MSVC__
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__ )
#define new DEBUG_NEW
#endif

#ifndef __WXMSW__
#include <signal.h>
#include <setjmp.h>


extern struct sigaction sa_all;
extern struct sigaction sa_all_old;

extern sigjmp_buf           env;                    // the context saved by sigsetjmp();
#endif

#include <vector>

//    Profiling support
//#include "/usr/include/valgrind/callgrind.h"

// ----------------------------------------------------------------------------
// Useful Prototypes
// ----------------------------------------------------------------------------
extern bool G_FloatPtInPolygon ( MyFlPoint *rgpts, int wnumpts, float x, float y ) ;
extern void catch_signals(int signo);
extern bool GetMemoryStatus(int *mem_total, int *mem_used);

extern ChartBase        *Current_Vector_Ch;
extern ChartBase        *Current_Ch;
extern double           g_ChartNotRenderScaleFactor;
extern double           gLat, gLon, gCog, gSog, gHdt;
extern double           vLat, vLon;
extern ChartDB          *ChartData;
extern bool             bDBUpdateInProgress;
extern ColorScheme      global_color_scheme;
extern bool             g_bHDTValid;
extern int              g_nbrightness;

extern ConsoleCanvas    *console;

extern RouteList        *pRouteList;
extern MyConfig         *pConfig;
extern Select           *pSelect;
extern Routeman         *g_pRouteMan;
extern ThumbWin         *pthumbwin;
extern TCMgr            *ptcmgr;
extern Select           *pSelectTC;
extern Select           *pSelectAIS;
extern WayPointman      *pWayPointMan;
extern MarkInfoImpl     *pMarkPropDialog;
extern RouteProp        *pRoutePropDialog;
extern TrackPropDlg     *pTrackPropDialog;
extern MarkInfoImpl     *pMarkInfoDialog;
extern Track            *g_pActiveTrack;
extern bool             g_bConfirmObjectDelete;
extern bool             g_bPreserveScaleOnX;

extern IDX_entry        *gpIDX;
extern int               gpIDXn;

extern RoutePoint       *pAnchorWatchPoint1;
extern RoutePoint       *pAnchorWatchPoint2;
extern double           AnchorPointMinDist;
extern bool             AnchorAlertOn1;
extern bool             AnchorAlertOn2;
extern wxString         g_AW1GUID;
extern wxString         g_AW2GUID;
extern int              g_nAWDefault;
extern int              g_nAWMax;
extern int              g_iDistanceFormat;

extern ocpnFloatingToolbarDialog *g_FloatingToolbarDialog;
extern RouteManagerDialog *pRouteManagerDialog;
extern GoToPositionDialog *pGoToPositionDialog;
extern wxString GetLayerName(int id);
extern wxString         g_uploadConnection;

extern bool             bDrawCurrentValues;

#ifdef USE_S57
extern s52plib          *ps52plib;
extern CM93OffsetDialog  *g_pCM93OffsetDialog;
#endif

extern bool             bGPSValid;
extern bool             g_bShowOutlines;
extern bool             g_bShowDepthUnits;

extern AIS_Decoder      *g_pAIS;
extern bool             g_bShowAIS;
extern bool             g_bShowAreaNotices;
extern int              g_Show_Target_Name_Scale;

extern MyFrame          *gFrame;
extern StatWin          *stats;
extern ocpnFloatingCompassWindow *g_FloatingCompassDialog;

extern int              g_iNavAidRadarRingsNumberVisible;
extern float            g_fNavAidRadarRingsStep;
extern int              g_pNavAidRadarRingsStepUnits;
extern bool             g_bWayPointPreventDragging;
extern bool             g_bEnableZoomToCursor;

extern AISTargetAlertDialog    *g_pais_alert_dialog_active;
extern AISTargetQueryDialog    *g_pais_query_dialog_active;
extern int              g_ais_query_dialog_x, g_ais_query_dialog_y;

extern int              g_S57_dialog_sx, g_S57_dialog_sy;

extern CM93DSlide       *pCM93DetailSlider;
extern bool             g_bShowCM93DetailSlider;
extern int              g_cm93detail_dialog_x, g_cm93detail_dialog_y;
extern int              g_cm93_zoom_factor;

extern bool             g_b_overzoom_x;                      // Allow high overzoom
extern bool             g_bDisplayGrid;

extern bool             g_bUseGreenShip;

extern ChartCanvas      *cc1;

extern int              g_OwnShipIconType;
extern double           g_n_ownship_length_meters;
extern double           g_n_ownship_beam_meters;
extern double           g_n_gps_antenna_offset_y;
extern double           g_n_gps_antenna_offset_x;
extern int              g_n_ownship_min_mm;

extern wxPlatformInfo   *g_pPlatform;

extern bool             g_bUseRaster;
extern bool             g_bUseVector;
extern bool             g_bUseCM93;

extern bool             g_bCourseUp;
extern double           g_COGAvg;               // only needed for debug....

extern int              g_click_stop;
extern double           g_ownship_predictor_minutes;
extern double           g_ownship_HDTpredictor_miles;

extern ArrayOfInts      g_quilt_noshow_index_array;
extern ChartStack       *pCurrentStack;
extern bool              g_bquiting;
extern AISTargetListDialog *g_pAISTargetList;
extern wxString         g_sAIS_Alert_Sound_File;

extern PlugInManager    *g_pi_manager;

extern wxAuiManager      *g_pauimgr;

extern bool             g_bskew_comp;
extern bool             g_bopengl;
extern bool             g_bdisable_opengl;

extern bool             g_bFullScreenQuilt;
extern wxProgressDialog *s_ProgDialog;

extern bool             g_bsmoothpanzoom;

bool                    g_bDebugOGL;

extern bool             g_b_assume_azerty;

extern int              g_GroupIndex;
extern ChartGroupArray  *g_pGroupArray;
extern wxString         g_default_wp_icon;

extern int              g_current_arrow_scale;

extern S57QueryDialog   *g_pObjectQueryDialog;
extern ocpnStyle::StyleManager* g_StyleManager;
extern Multiplexer      *g_pMUX;
extern wxArrayOfConnPrm *g_pConnectionParams;

extern OCPN_Sound        g_anchorwatch_sound;

extern bool              g_bShowMag;
extern bool              g_btouch;
extern bool              g_bresponsive;

#ifdef ocpnUSE_GL
extern ocpnGLOptions g_GLOptions;
#endif

wxProgressDialog *pprog;
bool b_skipout;
wxSize pprog_size;

wxArrayString compress_msg_array;
extern wxSize pprog_size;

//  TODO why are these static?
static int mouse_x;
static int mouse_y;
static bool mouse_leftisdown;

int r_gamma_mult;
int g_gamma_mult;
int b_gamma_mult;
int gamma_state;
bool g_brightness_init;
int   last_brightness;

int                      g_cog_predictor_width;


// "Curtain" mode parameters
wxDialog                *g_pcurtain;

#define MIN_BRIGHT 10
#define MAX_BRIGHT 100

//    Constants for right click menus
enum
{
    ID_DEF_MENU_MAX_DETAIL = 1,
    ID_DEF_MENU_SCALE_IN,
    ID_DEF_MENU_SCALE_OUT,
    ID_DEF_MENU_DROP_WP,
    ID_DEF_MENU_QUERY,
    ID_DEF_MENU_MOVE_BOAT_HERE,
    ID_DEF_MENU_GOTO_HERE,
    ID_DEF_MENU_GOTOPOSITION,

    ID_WP_MENU_GOTO,
    ID_WP_MENU_DELPOINT,
    ID_WP_MENU_PROPERTIES,
    ID_RT_MENU_ACTIVATE,
    ID_RT_MENU_DEACTIVATE,
    ID_RT_MENU_INSERT,
    ID_RT_MENU_APPEND,
    ID_RT_MENU_COPY,
    ID_TK_MENU_COPY,
    ID_WPT_MENU_COPY,
    ID_WPT_MENU_SENDTOGPS,
    ID_WPT_MENU_SENDTONEWGPS,
    ID_PASTE_WAYPOINT,
    ID_PASTE_ROUTE,
    ID_PASTE_TRACK,
    ID_RT_MENU_DELETE,
    ID_RT_MENU_REVERSE,
    ID_RT_MENU_DELPOINT,
    ID_RT_MENU_ACTPOINT,
    ID_RT_MENU_DEACTPOINT,
    ID_RT_MENU_ACTNXTPOINT,
    ID_RT_MENU_REMPOINT,
    ID_RT_MENU_PROPERTIES,
    ID_RT_MENU_SENDTOGPS,
    ID_RT_MENU_SENDTONEWGPS,
    ID_WP_MENU_SET_ANCHORWATCH,
    ID_WP_MENU_CLEAR_ANCHORWATCH,
    ID_DEF_MENU_AISTARGETLIST,

    ID_RC_MENU_SCALE_IN,
    ID_RC_MENU_SCALE_OUT,
    ID_RC_MENU_ZOOM_IN,
    ID_RC_MENU_ZOOM_OUT,
    ID_RC_MENU_FINISH,
    ID_DEF_MENU_AIS_QUERY,
    ID_DEF_MENU_AIS_CPA,
    ID_DEF_MENU_AISSHOWTRACK,
    ID_DEF_MENU_ACTIVATE_MEASURE,
    ID_DEF_MENU_DEACTIVATE_MEASURE,

    ID_UNDO,
    ID_REDO,

    ID_DEF_MENU_CM93OFFSET_DIALOG,

    ID_TK_MENU_PROPERTIES,
    ID_TK_MENU_DELETE,
    ID_WP_MENU_ADDITIONAL_INFO,

    ID_DEF_MENU_QUILTREMOVE,
    ID_DEF_MENU_COGUP,
    ID_DEF_MENU_NORTHUP,
    ID_DEF_MENU_TIDEINFO,
    ID_DEF_MENU_CURRENTINFO,

    ID_DEF_MENU_GROUPBASE,

    ID_DEF_MENU_LAST
};

//------------------------------------------------------------------------------
//    ViewPort Implementation
//------------------------------------------------------------------------------
ViewPort::ViewPort()
{
    bValid = false;
    skew = 0.;
    view_scale_ppm = 1;
    rotation = 0.;
    b_quilt = false;
    pix_height = pix_width = 0;
    b_MercatorProjectionOverride = false;
}

wxPoint ViewPort::GetPixFromLL( double lat, double lon ) const
{
    double easting, northing;
    double xlon = lon;

    /*  Make sure lon and lon0 are same phase */
    if( xlon * clon < 0. ) {
        if( xlon < 0. ) xlon += 360.;
        else
            xlon -= 360.;
    }

    if( fabs( xlon - clon ) > 180. ) {
        if( xlon > clon ) xlon -= 360.;
        else
            xlon += 360.;
    }

    if( PROJECTION_TRANSVERSE_MERCATOR == m_projection_type ) {
        //    We calculate northings as referenced to the equator
        //    And eastings as though the projection point is midscreen.

        double tmeasting, tmnorthing;
        double tmceasting, tmcnorthing;
        toTM( clat, clon, 0., clon, &tmceasting, &tmcnorthing );
        toTM( lat, xlon, 0., clon, &tmeasting, &tmnorthing );

//            tmeasting -= tmceasting;
//            tmnorthing -= tmcnorthing;

        northing = tmnorthing - tmcnorthing;
        easting = tmeasting - tmceasting;
    } else if( PROJECTION_POLYCONIC == m_projection_type ) {

        //    We calculate northings as referenced to the equator
        //    And eastings as though the projection point is midscreen.
        double pceasting, pcnorthing;
        toPOLY( clat, clon, 0., clon, &pceasting, &pcnorthing );

        double peasting, pnorthing;
        toPOLY( lat, xlon, 0., clon, &peasting, &pnorthing );

        easting = peasting;
        northing = pnorthing - pcnorthing;
    }

    else
        toSM( lat, xlon, clat, clon, &easting, &northing );

    if( !wxFinite(easting) || !wxFinite(northing) ) return wxPoint( 0, 0 );

    double epix = easting * view_scale_ppm;
    double npix = northing * view_scale_ppm;
    double dxr = epix;
    double dyr = npix;

    //    Apply VP Rotation
    double angle = rotation;

    if(!g_bskew_comp)
        angle += skew;
        
    if( angle ) {
        dxr = epix * cos( angle ) + npix * sin( angle );
        dyr = npix * cos( angle ) - epix * sin( angle );
    }
    wxPoint r;
    //    We definitely need a round() function here
    r.x = (int) wxRound( ( pix_width / 2 ) + dxr );
    r.y = (int) wxRound( ( pix_height / 2 ) - dyr );

    return r;
}

wxPoint2DDouble ViewPort::GetDoublePixFromLL( double lat, double lon )
{
    double easting, northing;
    double xlon = lon;

    /*  Make sure lon and lon0 are same phase */
    if( xlon * clon < 0. ) {
        if( xlon < 0. ) xlon += 360.;
        else
            xlon -= 360.;
    }

    if( fabs( xlon - clon ) > 180. ) {
        if( xlon > clon ) xlon -= 360.;
        else
            xlon += 360.;
    }

    if( PROJECTION_TRANSVERSE_MERCATOR == m_projection_type ) {
        //    We calculate northings as referenced to the equator
        //    And eastings as though the projection point is midscreen.

        double tmeasting, tmnorthing;
        double tmceasting, tmcnorthing;
        toTM( clat, clon, 0., clon, &tmceasting, &tmcnorthing );
        toTM( lat, xlon, 0., clon, &tmeasting, &tmnorthing );

        northing = tmnorthing - tmcnorthing;
        easting = tmeasting - tmceasting;
    } else if( PROJECTION_POLYCONIC == m_projection_type ) {

        //    We calculate northings as referenced to the equator
        //    And eastings as though the projection point is midscreen.
        double pceasting, pcnorthing;
        toPOLY( clat, clon, 0., clon, &pceasting, &pcnorthing );

        double peasting, pnorthing;
        toPOLY( lat, xlon, 0., clon, &peasting, &pnorthing );

        easting = peasting;
        northing = pnorthing - pcnorthing;
    }

    else
        toSM( lat, xlon, clat, clon, &easting, &northing );

    if( !wxFinite(easting) || !wxFinite(northing) ) return wxPoint( 0, 0 );

    double epix = easting * view_scale_ppm;
    double npix = northing * view_scale_ppm;
    double dxr = epix;
    double dyr = npix;

    //    Apply VP Rotation
    double angle = rotation;
    if(!g_bskew_comp)
        angle += skew;

    if( angle ) {
        dxr = epix * cos( angle ) + npix * sin( angle );
        dyr = npix * cos( angle ) - epix * sin( angle );
    }

    wxPoint2DDouble r;
    //    We definitely need a round() function here
    r.m_x = ( ( pix_width / 2 ) + dxr );
    r.m_y = ( ( pix_height / 2 ) - dyr );

    return r;
}

void ViewPort::GetLLFromPix( const wxPoint &p, double *lat, double *lon )
{
    int dx = p.x - ( pix_width / 2 );
    int dy = ( pix_height / 2 ) - p.y;

    double xpr = dx;
    double ypr = dy;

    //    Apply VP Rotation
    double angle = rotation;
    if(!g_bskew_comp)
        angle += skew;

    if( angle ) {
        xpr = ( dx * cos( angle ) ) - ( dy * sin( angle ) );
        ypr = ( dy * cos( angle ) ) + ( dx * sin( angle ) );
    }
    double d_east = xpr / view_scale_ppm;
    double d_north = ypr / view_scale_ppm;

    double slat, slon;
    if( PROJECTION_TRANSVERSE_MERCATOR == m_projection_type ) {
        double tmceasting, tmcnorthing;
        toTM( clat, clon, 0., clon, &tmceasting, &tmcnorthing );

        fromTM( d_east, d_north + tmcnorthing, 0., clon, &slat, &slon );
    } else if( PROJECTION_POLYCONIC == m_projection_type ) {
        double polyeasting, polynorthing;
        toPOLY( clat, clon, 0., clon, &polyeasting, &polynorthing );

        fromPOLY( d_east, d_north + polynorthing, 0., clon, &slat, &slon );
    }

    //TODO  This could be fromSM_ECC to better match some Raster charts
    //      However, it seems that cm93 (and S57) prefer no eccentricity correction
    //      Think about it....
    else
        fromSM( d_east, d_north, clat, clon, &slat, &slon );

    *lat = slat;

    if( slon < -180. ) slon += 360.;
    else if( slon > 180. ) slon -= 360.;
    *lon = slon;
}

OCPNRegion ViewPort::GetVPRegionIntersect( const OCPNRegion &Region, size_t nPoints, float *llpoints,
        int chart_native_scale, wxPoint *ppoints )
{
    //  Calculate the intersection between a given OCPNRegion (Region) and a polygon specified by lat/lon points.

    //    If the viewpoint is highly overzoomed wrt to chart native scale, the polygon region may be huge.
    //    This can be very expensive, and lead to crashes on some platforms (gtk in particular)
    //    So, look for this case and handle appropriately with respect to the given Region

    if( chart_scale < chart_native_scale / 10 ) {
        //    Make a positive definite vp
        ViewPort vp_positive = *this;
        while( vp_positive.vpBBox.GetMinX() < 0 ) {
            vp_positive.clon += 360.;
            wxPoint2DDouble t( 360., 0. );
            vp_positive.vpBBox.Translate( t );
        }

        //    Scan the points one-by-one, so that we can get min/max to make a bbox
        float *pfp = llpoints;
        float lon_max = -10000.;
        float lon_min = 10000.;
        float lat_max = -10000.;
        float lat_min = 10000.;

        for( unsigned int ip = 0; ip < nPoints; ip++ ) {
            lon_max = wxMax(lon_max, pfp[1]);
            lon_min = wxMin(lon_min, pfp[1]);
            lat_max = wxMax(lat_max, pfp[0]);
            lat_min = wxMin(lat_min, pfp[0]);

            pfp += 2;
        }

        wxBoundingBox chart_box( lon_min, lat_min, lon_max, lat_max );

        //    Case:  vpBBox is completely outside the chart box, or vice versa
        //    Return an empty region
        if( chart_box.IntersectOut( (wxBoundingBox&) vp_positive.vpBBox ) ) {
            if( chart_box.IntersectOut( (wxBoundingBox&) vpBBox ) ) {
                // try again with the chart translated 360
                wxPoint2DDouble rtw( 360., 0. );
                wxBoundingBox trans_box = chart_box;
                trans_box.Translate( rtw );

                if( trans_box.IntersectOut( (wxBoundingBox&) vp_positive.vpBBox ) ) {
                    if( trans_box.IntersectOut( (wxBoundingBox&) vpBBox ) ) {
                        return OCPNRegion();
                    }
                }
            }
        }

        //    Case:  vpBBox is completely inside the chart box
        //      Note that this test is not perfect, and will fail for some charts.
        //      The chart coverage may be  essentially triangular, and the viewport box
        //      may be in the "cut off" segment of the chart_box, and not actually
        //      exhibit any true overlap.  Results will be reported incorrectly.
        //      How to fix: maybe scrub the chart points and see if it is likely that
        //      a region may be safely built and intersection tested.

        if( _IN == chart_box.Intersect( (wxBoundingBox&) vp_positive.vpBBox ) ) {
            return Region;
        }

        if(_IN == chart_box.Intersect((wxBoundingBox&)vpBBox))
        {
            return Region;
        }

        //    The ViewPort and the chart region overlap in some way....
        //    Create the intersection of the two bboxes
        //    Boxes must be same phase
        while( chart_box.GetMinX() < 0 ) {
            wxPoint2DDouble t( 360., 0. );
            chart_box.Translate( t );
        }

        double cb_minlon = wxMax(chart_box.GetMinX(), vp_positive.vpBBox.GetMinX());
        double cb_maxlon = wxMin(chart_box.GetMaxX(), vp_positive.vpBBox.GetMaxX());
        double cb_minlat = wxMax(chart_box.GetMinY(), vp_positive.vpBBox.GetMinY());
        double cb_maxlat = wxMin(chart_box.GetMaxY(), vp_positive.vpBBox.GetMaxY());

        if( cb_maxlon < cb_minlon ) cb_maxlon += 360.;

        wxPoint p1 = GetPixFromLL( cb_maxlat, cb_minlon );  // upper left
        wxPoint p2 = GetPixFromLL( cb_minlat, cb_maxlon );   // lower right

        OCPNRegion r( p1, p2 );
        r.Intersect( Region );
        return r;
    }

    //    More "normal" case

    wxPoint *pp;

    //    Use the passed point buffer if available
    if( ppoints == NULL ) pp = new wxPoint[nPoints];
    else
        pp = ppoints;

    float *pfp = llpoints;

    
    wxPoint p = GetPixFromLL( pfp[0], pfp[1] );
    int poly_x_max = p.x;
    int poly_y_max = p.y;
    int poly_x_min = p.x;
    int poly_y_min = p.y;
    
    for( unsigned int ip = 0; ip < nPoints; ip++ ) {
        wxPoint p = GetPixFromLL( pfp[0], pfp[1] );
        pp[ip] = p;
        poly_x_max = wxMax(poly_x_max, p.x);
        poly_y_max = wxMax(poly_y_max, p.y);
        poly_x_min = wxMin(poly_x_min, p.x);
        poly_y_min = wxMin(poly_y_min, p.y);
        pfp += 2;
    }
 
    //  We want to avoid processing regions with very large rectangle counts,
    //  so make some tests for special cases

    
    //  First, calculate whether any segment of the input polygon intersects the specified Region
    bool b_intersect = false;
    OCPNRegionIterator screen_region_it1( Region );
    while( screen_region_it1.HaveRects() ) {
        wxRect rect = screen_region_it1.GetRect();
        
        for(size_t i=0 ; i < nPoints-1 ; i++){
            int x0 = pp[i].x;  int y0 = pp[i].y; int x1 = pp[i+1].x; int y1 = pp[i+1].y;
            if( ((x0 < rect.x) && (x1 < rect.x)) ||
                ((x0 > rect.x+rect.width) && (x1 > rect.x+rect.width)) )
                continue;
            
            if( ((y0 < rect.y) && (y1 < rect.y)) ||
                ((y0 > rect.y+rect.height) && (y1 > rect.y+rect.height)) )
                continue;
            
            b_intersect = true;
            break;
        }

        // Check segment, last point back to first point
        if(!b_intersect){
            int x0 = pp[nPoints-1].x;  int y0 = pp[nPoints-1].y; int x1 = pp[0].x; int y1 = pp[0].y;
            if( ((x0 < rect.x) && (x1 < rect.x)) ||
               ((x0 > rect.x+rect.width) && (x1 > rect.x+rect.width)) ){
            }
            else{
                if( ((y0 < rect.y) && (y1 < rect.y)) ||
                   ((y0 > rect.y+rect.height) && (y1 > rect.y+rect.height)) ){
                }
                else{
                    b_intersect = true;
                }
            }
        }

        screen_region_it1.NextRect();
    }

    //  If there is no itersection, we need to consider the case where
    //  the subject polygon is entirely within the Region
    bool b_contained = false;
    if(!b_intersect){
        OCPNRegionIterator screen_region_it2( Region );
        while( screen_region_it2.HaveRects() ) {
            wxRect rect = screen_region_it2.GetRect();
 
            for(size_t i=0 ; i < nPoints-1 ; i++){
                int x0 = pp[i].x;  int y0 = pp[i].y;
                if((x0 < rect.x) || (x0 > rect.x+rect.width))
                    continue;
                
                if((y0 < rect.y) || (y0 > rect.y+rect.height))
                    continue;
                
                b_contained = true;
                break;
            }
            screen_region_it2.NextRect();
        }
    }
    
#if 1    
    // and here is the payoff
    if(!b_contained && !b_intersect){
        //  Two cases to consider
        wxRect rpoly( poly_x_min, poly_y_min, poly_x_max - poly_x_min , poly_y_max - poly_y_min);
        wxRect rRegion = Region.GetBox();
        if(rpoly.Contains(rRegion)){
            //  subject poygon may be large enough to fully encompass the target Region,
            //  but it might not, especially for irregular or concave charts.
            //  So we cannot shortcut here
        }
        else{
        //  Subject polygon is entirely outside of target Region
        //  so the intersection must be empty.
            if( NULL == ppoints ) delete[] pp;
            wxRegion r;
            return r;
        }
    }
    else if(b_contained && !b_intersect){
        //  subject polygon is entirely withing the target Region,
        //  so the intersection is the subject polygon
        OCPNRegion r = OCPNRegion( nPoints, pp );
        if( NULL == ppoints ) delete[] pp;
        return r;
    }
        
#endif    
        
        
    
#ifdef __WXGTK__
    sigaction(SIGSEGV, NULL, &sa_all_old);             // save existing action for this signal

    struct sigaction temp;
    sigaction(SIGSEGV, NULL, &temp);// inspect existing action for this signal

    temp.sa_handler = catch_signals;// point to my handler
    sigemptyset(&temp.sa_mask);// make the blocking set
    // empty, so that all
    // other signals will be
    // unblocked during my handler
    temp.sa_flags = 0;
    sigaction(SIGSEGV, &temp, NULL);

    if(sigsetjmp(env, 1))//  Something in the below code block faulted....
    {
        sigaction(SIGSEGV, &sa_all_old, NULL);        // reset signal handler

        return Region;

    }

    else
    {

        OCPNRegion r = OCPNRegion(nPoints, pp);
        if(NULL == ppoints)
            delete[] pp;

        sigaction(SIGSEGV, &sa_all_old, NULL);        // reset signal handler
        r.Intersect(Region);
        return r;
    }

#else
    OCPNRegion r = OCPNRegion( nPoints, pp );

    if( NULL == ppoints ) delete[] pp;

    r.Intersect( Region );
    return r;

#endif
}

wxRect ViewPort::GetVPRectIntersect( size_t n, float *llpoints )
{
    //  Calculate the intersection between the currect VP screen
    //  and the bounding box of a polygon specified by lat/lon points.

    float *pfp = llpoints;

    wxBoundingBox point_box;
    for( unsigned int ip = 0; ip < n; ip++ ) {
        point_box.Expand(pfp[1], pfp[0]);
        pfp += 2;
    }

    wxPoint pul = GetPixFromLL( point_box.GetMaxY(), point_box.GetMinX() );
    wxPoint plr = GetPixFromLL( point_box.GetMinY(), point_box.GetMaxX() );

    OCPNRegion r( pul, plr );
    OCPNRegion rs(rv_rect);

    r.Intersect(rs);

    return r.GetBox();


}

void ViewPort::SetBoxes( void )
{

    //  In the case where canvas rotation is applied, we need to define a larger "virtual" pixel window size to ensure that
    //  enough chart data is fatched and available to fill the rotated screen.
    rv_rect = wxRect( 0, 0, pix_width, pix_height );

    //  Specify the minimum required rectangle in unrotated screen space which will supply full screen data after specified rotation
    if( ( g_bskew_comp && ( fabs( skew ) > .001 ) ) || ( fabs( rotation ) > .001 ) ) {

        double rotator = rotation;
        if(g_bskew_comp)
            rotator -= skew;

        int dy = wxRound(
                     fabs( pix_height * cos( rotator ) ) + fabs( pix_width * sin( rotator ) ) );
        int dx = wxRound(
                     fabs( pix_width * cos( rotator ) ) + fabs( pix_height * sin( rotator ) ) );

        //  It is important for MSW build that viewport pixel dimensions be multiples of 4.....
        if( dy % 4 ) dy += 4 - ( dy % 4 );
        if( dx % 4 ) dx += 4 - ( dx % 4 );

        //  Grow the source rectangle appropriately
        if( fabs( rotator ) > .001 ) rv_rect.Inflate( ( dx - pix_width ) / 2,
                    ( dy - pix_height ) / 2 );

    }

    //  Compute Viewport lat/lon reference points for co-ordinate hit testing

    //  This must be done in unrotated space with respect to full unrotated screen space calculated above
    double rotation_save = rotation;
    SetRotationAngle( 0. );

    double lat_ul, lat_ur, lat_lr, lat_ll;
    double lon_ul, lon_ur, lon_lr, lon_ll;

    GetLLFromPix( wxPoint( rv_rect.x, rv_rect.y ), &lat_ul, &lon_ul );
    GetLLFromPix( wxPoint( rv_rect.x + rv_rect.width, rv_rect.y ), &lat_ur, &lon_ur );
    GetLLFromPix( wxPoint( rv_rect.x + rv_rect.width, rv_rect.y + rv_rect.height ), &lat_lr,
                  &lon_lr );
    GetLLFromPix( wxPoint( rv_rect.x, rv_rect.y + rv_rect.height ), &lat_ll, &lon_ll );

    if( clon < 0. ) {
        if( ( lon_ul > 0. ) && ( lon_ur < 0. ) ) {
            lon_ul -= 360.;
            lon_ll -= 360.;
        }
    } else {
        if( ( lon_ul > 0. ) && ( lon_ur < 0. ) ) {
            lon_ur += 360.;
            lon_lr += 360.;
        }
    }

    if( lon_ur < lon_ul ) {
        lon_ur += 360.;
        lon_lr += 360.;
    }

    if( lon_ur > 360. ) {
        lon_ur -= 360.;
        lon_lr -= 360.;
        lon_ul -= 360.;
        lon_ll -= 360.;
    }

    double dlat_min = lat_ul;
    dlat_min = fmin ( dlat_min, lat_ur );
    dlat_min = fmin ( dlat_min, lat_lr );
    dlat_min = fmin ( dlat_min, lat_ll );

    double dlon_min = lon_ul;
    dlon_min = fmin ( dlon_min, lon_ur );
    dlon_min = fmin ( dlon_min, lon_lr );
    dlon_min = fmin ( dlon_min, lon_ll );

    double dlat_max = lat_ul;
    dlat_max = fmax ( dlat_max, lat_ur );
    dlat_max = fmax ( dlat_max, lat_lr );
    dlat_max = fmax ( dlat_max, lat_ll );

    double dlon_max = lon_ur;
    dlon_max = fmax ( dlon_max, lon_ul );
    dlon_max = fmax ( dlon_max, lon_lr );
    dlon_max = fmax ( dlon_max, lon_ll );

    //  Set the viewport lat/lon bounding box appropriately
    vpBBox.SetMin( dlon_min, dlat_min );
    vpBBox.SetMax( dlon_max, dlat_max );

    // Restore the rotation angle
    SetRotationAngle( rotation_save );
}

void ViewPort::SetBBoxDirect( double latmin, double lonmin, double latmax, double lonmax)
{
    vpBBox.SetMin( lonmin, latmin );
    vpBBox.SetMax( lonmax, latmax );
}

//------------------------------------------------------------------------------
//    ChartCanvas Implementation
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE ( ChartCanvas, wxWindow )
    EVT_PAINT ( ChartCanvas::OnPaint )
    EVT_ACTIVATE ( ChartCanvas::OnActivate )
    EVT_SIZE ( ChartCanvas::OnSize )
    EVT_MOUSE_EVENTS ( ChartCanvas::MouseEvent )
    EVT_TIMER ( DBLCLICK_TIMER, ChartCanvas::MouseTimedEvent )
    EVT_TIMER ( PAN_TIMER, ChartCanvas::PanTimerEvent )
    EVT_TIMER ( MOVEMENT_TIMER, ChartCanvas::MovementTimerEvent )
    EVT_TIMER ( MOVEMENT_STOP_TIMER, ChartCanvas::MovementStopTimerEvent )
    EVT_TIMER ( CURTRACK_TIMER, ChartCanvas::OnCursorTrackTimerEvent )
    EVT_TIMER ( ROT_TIMER, ChartCanvas::RotateTimerEvent )
    EVT_TIMER ( ROPOPUP_TIMER, ChartCanvas::OnRolloverPopupTimerEvent )
    EVT_KEY_DOWN(ChartCanvas::OnKeyDown )
    EVT_KEY_UP(ChartCanvas::OnKeyUp )
    EVT_CHAR(ChartCanvas::OnKeyChar)
    EVT_MOUSE_CAPTURE_LOST(ChartCanvas::LostMouseCapture )

    EVT_MENU ( ID_DEF_MENU_MAX_DETAIL,         ChartCanvas::PopupMenuHandler )
    EVT_MENU ( ID_DEF_MENU_SCALE_IN,           ChartCanvas::PopupMenuHandler )
    EVT_MENU ( ID_DEF_MENU_SCALE_OUT,          ChartCanvas::PopupMenuHandler )
    EVT_MENU ( ID_DEF_MENU_QUERY,              ChartCanvas::PopupMenuHandler )
    EVT_MENU ( ID_DEF_MENU_DROP_WP,            ChartCanvas::PopupMenuHandler )
    EVT_MENU ( ID_DEF_MENU_MOVE_BOAT_HERE,     ChartCanvas::PopupMenuHandler )
    EVT_MENU ( ID_DEF_MENU_GOTO_HERE,          ChartCanvas::PopupMenuHandler )
    EVT_MENU ( ID_DEF_MENU_GOTOPOSITION,       ChartCanvas::PopupMenuHandler )
    EVT_MENU ( ID_DEF_MENU_COGUP,              ChartCanvas::PopupMenuHandler )
    EVT_MENU ( ID_DEF_MENU_NORTHUP,            ChartCanvas::PopupMenuHandler )

    EVT_MENU ( ID_RT_MENU_ACTIVATE,     ChartCanvas::PopupMenuHandler )
    EVT_MENU ( ID_RT_MENU_DEACTIVATE,   ChartCanvas::PopupMenuHandler )
    EVT_MENU ( ID_RT_MENU_INSERT,       ChartCanvas::PopupMenuHandler )
    EVT_MENU ( ID_RT_MENU_APPEND,       ChartCanvas::PopupMenuHandler )
    EVT_MENU ( ID_RT_MENU_COPY,         ChartCanvas::PopupMenuHandler )
    EVT_MENU ( ID_TK_MENU_COPY,         ChartCanvas::PopupMenuHandler )
    EVT_MENU ( ID_RT_MENU_SENDTOGPS,    ChartCanvas::PopupMenuHandler )
    EVT_MENU ( ID_RT_MENU_SENDTONEWGPS, ChartCanvas::PopupMenuHandler )
    EVT_MENU ( ID_WPT_MENU_COPY,        ChartCanvas::PopupMenuHandler )
    EVT_MENU ( ID_WPT_MENU_SENDTOGPS,   ChartCanvas::PopupMenuHandler )
    EVT_MENU ( ID_WPT_MENU_SENDTONEWGPS,ChartCanvas::PopupMenuHandler )
    EVT_MENU ( ID_PASTE_WAYPOINT,       ChartCanvas::PopupMenuHandler )
    EVT_MENU ( ID_PASTE_ROUTE,          ChartCanvas::PopupMenuHandler )
    EVT_MENU ( ID_PASTE_TRACK,          ChartCanvas::PopupMenuHandler )
    EVT_MENU ( ID_RT_MENU_DELETE,       ChartCanvas::PopupMenuHandler )
    EVT_MENU ( ID_RT_MENU_REVERSE,      ChartCanvas::PopupMenuHandler )

    EVT_MENU ( ID_RT_MENU_DELPOINT,     ChartCanvas::PopupMenuHandler )
    EVT_MENU ( ID_RT_MENU_REMPOINT,     ChartCanvas::PopupMenuHandler )
    EVT_MENU ( ID_RT_MENU_ACTPOINT,     ChartCanvas::PopupMenuHandler )
    EVT_MENU ( ID_RT_MENU_DEACTPOINT,   ChartCanvas::PopupMenuHandler )
    EVT_MENU ( ID_RT_MENU_ACTNXTPOINT,  ChartCanvas::PopupMenuHandler )
    EVT_MENU ( ID_RT_MENU_PROPERTIES,   ChartCanvas::PopupMenuHandler )
    EVT_MENU ( ID_WP_MENU_SET_ANCHORWATCH,    ChartCanvas::PopupMenuHandler )
    EVT_MENU ( ID_WP_MENU_CLEAR_ANCHORWATCH,  ChartCanvas::PopupMenuHandler )
    EVT_MENU ( ID_DEF_MENU_AISTARGETLIST,     ChartCanvas::PopupMenuHandler )

    EVT_MENU ( ID_RC_MENU_SCALE_IN,     ChartCanvas::PopupMenuHandler )
    EVT_MENU ( ID_RC_MENU_SCALE_OUT,    ChartCanvas::PopupMenuHandler )
    EVT_MENU ( ID_RC_MENU_ZOOM_IN,      ChartCanvas::PopupMenuHandler )
    EVT_MENU ( ID_RC_MENU_ZOOM_OUT,     ChartCanvas::PopupMenuHandler )
    EVT_MENU ( ID_RC_MENU_FINISH,       ChartCanvas::PopupMenuHandler )
    EVT_MENU ( ID_DEF_MENU_AIS_QUERY,   ChartCanvas::PopupMenuHandler )
    EVT_MENU ( ID_DEF_MENU_AIS_CPA,     ChartCanvas::PopupMenuHandler )
    EVT_MENU ( ID_DEF_MENU_AISSHOWTRACK, ChartCanvas::PopupMenuHandler )

    EVT_MENU ( ID_UNDO,                 ChartCanvas::PopupMenuHandler )
    EVT_MENU ( ID_REDO,                 ChartCanvas::PopupMenuHandler )

    EVT_MENU ( ID_DEF_MENU_ACTIVATE_MEASURE,   ChartCanvas::PopupMenuHandler )
    EVT_MENU ( ID_DEF_MENU_DEACTIVATE_MEASURE, ChartCanvas::PopupMenuHandler )

    EVT_MENU ( ID_DEF_MENU_CM93OFFSET_DIALOG,   ChartCanvas::PopupMenuHandler )

    EVT_MENU ( ID_WP_MENU_GOTO,               ChartCanvas::PopupMenuHandler )
    EVT_MENU ( ID_WP_MENU_DELPOINT,           ChartCanvas::PopupMenuHandler )
    EVT_MENU ( ID_WP_MENU_PROPERTIES,         ChartCanvas::PopupMenuHandler )
    EVT_MENU ( ID_WP_MENU_ADDITIONAL_INFO,    ChartCanvas::PopupMenuHandler )

    EVT_MENU ( ID_TK_MENU_PROPERTIES,       ChartCanvas::PopupMenuHandler )
    EVT_MENU ( ID_TK_MENU_DELETE,           ChartCanvas::PopupMenuHandler )

    EVT_MENU ( ID_DEF_MENU_QUILTREMOVE,     ChartCanvas::PopupMenuHandler )

    EVT_MENU ( ID_DEF_MENU_TIDEINFO,        ChartCanvas::PopupMenuHandler )
    EVT_MENU ( ID_DEF_MENU_CURRENTINFO,     ChartCanvas::PopupMenuHandler )
    EVT_MENU ( ID_DEF_MENU_GROUPBASE,       ChartCanvas::PopupMenuHandler )
END_EVENT_TABLE()

// Define a constructor for my canvas
ChartCanvas::ChartCanvas ( wxFrame *frame ) :
    wxWindow ( frame, wxID_ANY,    wxPoint ( 20,20 ), wxSize ( 5,5 ), wxSIMPLE_BORDER )
{
    parent_frame = ( MyFrame * ) frame;       // save a pointer to parent

    SetBackgroundColour ( GetGlobalColor ( _T ( "NODTA" ) ) );
    SetBackgroundStyle ( wxBG_STYLE_CUSTOM );  // on WXMSW, this prevents flashing on color scheme change

    m_bDrawingRoute = false;
    m_bRouteEditing = false;
    m_bMarkEditing = false;
    m_bIsInRadius = false;

    m_bFollow = false;
    m_bTCupdate = false;
    m_bAppendingRoute = false;          // was true in MSW, why??
    pThumbDIBShow = NULL;
    m_bShowCurrent = false;
    m_bShowTide = false;
    bShowingCurrent = false;
    pCwin = NULL;
    warp_flag = false;
    m_bzooming = false;
    m_b_paint_enable = true;

    pss_overlay_bmp = NULL;
    pss_overlay_mask = NULL;
    m_bChartDragging = false;
    m_bMeasure_Active = false;
    m_pMeasureRoute = NULL;
    m_pRouteRolloverWin = NULL;
    m_pAISRolloverWin = NULL;
    m_bedge_pan = false;
    
    m_pCIWin = NULL;

    m_pSelectedRoute              = NULL;
    m_pSelectedTrack              = NULL;
    m_pRoutePointEditTarget       = NULL;
    m_pFoundPoint                 = NULL;
    m_pMouseRoute                 = NULL;
    m_prev_pMousePoint            = NULL;
    m_pEditRouteArray             = NULL;
    m_pFoundRoutePoint            = NULL;
    m_pFoundRoutePointSecond      = NULL;

    m_pRolloverRouteSeg           = NULL;
    m_bsectors_shown              = false;
    
    m_bbrightdir = false;
    r_gamma_mult = 1;
    g_gamma_mult = 1;
    b_gamma_mult = 1;


    m_pos_image_user_day        = NULL;
    m_pos_image_user_dusk       = NULL;
    m_pos_image_user_night      = NULL;
    m_pos_image_user_grey_day   = NULL;
    m_pos_image_user_grey_dusk  = NULL;
    m_pos_image_user_grey_night = NULL;

    m_zoom_factor = 1;
    m_rotation_speed = 0;
    m_mustmove = 0;

    m_pos_image_user_yellow_day = NULL;
    m_pos_image_user_yellow_dusk = NULL;
    m_pos_image_user_yellow_night = NULL;

    undo = new Undo;

    VPoint.Invalidate();

    m_glcc = NULL;
    m_pGLcontext = NULL;
    
    g_ChartNotRenderScaleFactor = 2.0;

#ifdef ocpnUSE_GL
    if ( !g_bdisable_opengl )
    {
        wxLogMessage( _T("Creating glChartCanvas") );
        m_glcc = new glChartCanvas(this);

    #if wxCHECK_VERSION(2, 9, 0)
        m_pGLcontext = new wxGLContext(m_glcc);
        m_glcc->SetContext(m_pGLcontext);
    #else
        m_pGLcontext = m_glcc->GetContext();
    #endif
    }
#endif

    singleClickEventIsValid = false;

//    Build the cursors

    ocpnStyle::Style* style = g_StyleManager->GetCurrentStyle();

#if defined( __WXGTK__) || defined(__WXOSX__)

    wxImage ICursorLeft = style->GetIcon( _T("left") ).ConvertToImage();
    wxImage ICursorRight = style->GetIcon( _T("right") ).ConvertToImage();
    wxImage ICursorUp = style->GetIcon( _T("up") ).ConvertToImage();
    wxImage ICursorDown = style->GetIcon( _T("down") ).ConvertToImage();
    wxImage ICursorPencil = style->GetIcon( _T("pencil") ).ConvertToImage();
    wxImage ICursorCross = style->GetIcon( _T("cross") ).ConvertToImage();

//#if wxCHECK_VERSION(2, 8, 12)
//#else
    ICursorLeft.ConvertAlphaToMask(128);
    ICursorRight.ConvertAlphaToMask(128);
    ICursorUp.ConvertAlphaToMask(128);
    ICursorDown.ConvertAlphaToMask(128);
    ICursorPencil.ConvertAlphaToMask(10);
    ICursorCross.ConvertAlphaToMask(10);
//#endif

    if ( ICursorLeft.Ok() )
    {
        ICursorLeft.SetOption ( wxIMAGE_OPTION_CUR_HOTSPOT_X, 0 );
        ICursorLeft.SetOption ( wxIMAGE_OPTION_CUR_HOTSPOT_Y, 15 );
        pCursorLeft = new wxCursor ( ICursorLeft );
    }
    else
        pCursorLeft = new wxCursor ( wxCURSOR_ARROW );

    if ( ICursorRight.Ok() )
    {
        ICursorRight.SetOption ( wxIMAGE_OPTION_CUR_HOTSPOT_X, 31 );
        ICursorRight.SetOption ( wxIMAGE_OPTION_CUR_HOTSPOT_Y, 15 );
        pCursorRight = new wxCursor ( ICursorRight );
    }
    else
        pCursorRight = new wxCursor ( wxCURSOR_ARROW );

    if ( ICursorUp.Ok() )
    {
        ICursorUp.SetOption ( wxIMAGE_OPTION_CUR_HOTSPOT_X, 15 );
        ICursorUp.SetOption ( wxIMAGE_OPTION_CUR_HOTSPOT_Y, 0 );
        pCursorUp = new wxCursor ( ICursorUp );
    }
    else
        pCursorUp = new wxCursor ( wxCURSOR_ARROW );

    if ( ICursorDown.Ok() )
    {
        ICursorDown.SetOption ( wxIMAGE_OPTION_CUR_HOTSPOT_X, 15 );
        ICursorDown.SetOption ( wxIMAGE_OPTION_CUR_HOTSPOT_Y, 31 );
        pCursorDown = new wxCursor ( ICursorDown );
    }
    else
        pCursorDown = new wxCursor ( wxCURSOR_ARROW );

    if ( ICursorPencil.Ok() )
    {
        ICursorPencil.SetOption ( wxIMAGE_OPTION_CUR_HOTSPOT_X, 0 );
        ICursorPencil.SetOption ( wxIMAGE_OPTION_CUR_HOTSPOT_Y, 16);
        pCursorPencil = new wxCursor ( ICursorPencil );
    }
    else
        pCursorPencil = new wxCursor ( wxCURSOR_ARROW );

    if ( ICursorCross.Ok() )
    {
        ICursorCross.SetOption ( wxIMAGE_OPTION_CUR_HOTSPOT_X, 13 );
        ICursorCross.SetOption ( wxIMAGE_OPTION_CUR_HOTSPOT_Y, 12);
        pCursorCross = new wxCursor ( ICursorCross );
    }
    else
        pCursorCross = new wxCursor ( wxCURSOR_ARROW );

#else

    wxImage ICursorLeft = style->GetIcon( _T("left") ).ConvertToImage();
    wxImage ICursorRight = style->GetIcon( _T("right") ).ConvertToImage();
    wxImage ICursorUp = style->GetIcon( _T("up") ).ConvertToImage();
    wxImage ICursorDown = style->GetIcon( _T("down") ).ConvertToImage();
    wxImage ICursorPencil = style->GetIcon( _T("pencil") ).ConvertToImage();
    wxImage ICursorCross = style->GetIcon( _T("cross") ).ConvertToImage();

    if( ICursorLeft.Ok() ) {
        ICursorLeft.SetOption( wxIMAGE_OPTION_CUR_HOTSPOT_X, 0 );
        ICursorLeft.SetOption( wxIMAGE_OPTION_CUR_HOTSPOT_Y, 15 );
        pCursorLeft = new wxCursor( ICursorLeft );
    } else
        pCursorLeft = new wxCursor( wxCURSOR_ARROW );

    if( ICursorRight.Ok() ) {
        ICursorRight.SetOption( wxIMAGE_OPTION_CUR_HOTSPOT_X, 31 );
        ICursorRight.SetOption( wxIMAGE_OPTION_CUR_HOTSPOT_Y, 15 );
        pCursorRight = new wxCursor( ICursorRight );
    } else
        pCursorRight = new wxCursor( wxCURSOR_ARROW );

    if( ICursorUp.Ok() ) {
        ICursorUp.SetOption( wxIMAGE_OPTION_CUR_HOTSPOT_X, 15 );
        ICursorUp.SetOption( wxIMAGE_OPTION_CUR_HOTSPOT_Y, 0 );
        pCursorUp = new wxCursor( ICursorUp );
    } else
        pCursorUp = new wxCursor( wxCURSOR_ARROW );

    if( ICursorDown.Ok() ) {
        ICursorDown.SetOption( wxIMAGE_OPTION_CUR_HOTSPOT_X, 15 );
        ICursorDown.SetOption( wxIMAGE_OPTION_CUR_HOTSPOT_Y, 31 );
        pCursorDown = new wxCursor( ICursorDown );
    } else
        pCursorDown = new wxCursor( wxCURSOR_ARROW );

    if( ICursorPencil.Ok() ) {
        ICursorPencil.SetOption( wxIMAGE_OPTION_CUR_HOTSPOT_X, 0 );
        ICursorPencil.SetOption( wxIMAGE_OPTION_CUR_HOTSPOT_Y, 15 );
        pCursorPencil = new wxCursor( ICursorPencil );
    } else
        pCursorPencil = new wxCursor( wxCURSOR_ARROW );

    if( ICursorCross.Ok() ) {
        ICursorCross.SetOption( wxIMAGE_OPTION_CUR_HOTSPOT_X, 13 );
        ICursorCross.SetOption( wxIMAGE_OPTION_CUR_HOTSPOT_Y, 12 );
        pCursorCross = new wxCursor( ICursorCross );
    } else
        pCursorCross = new wxCursor( wxCURSOR_ARROW );

#endif      // MSW, X11
    pCursorArrow = new wxCursor( wxCURSOR_ARROW );

    SetCursor( *pCursorArrow );

    pPanTimer = new wxTimer( this, PAN_TIMER );
    pPanTimer->Stop();

    pMovementTimer = new wxTimer( this, MOVEMENT_TIMER );
    pMovementTimer->Stop();

    pMovementStopTimer = new wxTimer( this, MOVEMENT_STOP_TIMER );
    pMovementStopTimer->Stop();

    pRotDefTimer = new wxTimer( this, ROT_TIMER );
    pRotDefTimer->Stop();

    m_DoubleClickTimer = new wxTimer( this, DBLCLICK_TIMER );
    m_DoubleClickTimer->Stop();

    m_panx = m_pany = 0;
    m_panspeed = 0;

    pCurTrackTimer = new wxTimer( this, CURTRACK_TIMER );
    pCurTrackTimer->Stop();
    m_curtrack_timer_msec = 10;

    m_wheelzoom_stop_oneshot = 0;
    m_last_wheel_dir = 0;
    
    m_RolloverPopupTimer.SetOwner( this, ROPOPUP_TIMER );

    m_rollover_popup_timer_msec = 20;

    m_b_rot_hidef = true;

//    Set up current arrow drawing factors
    int mmx, mmy;
    wxDisplaySizeMM( &mmx, &mmy );

    int sx, sy;
    wxDisplaySize( &sx, &sy );

    m_pix_per_mm = ( (double) sx ) / ( (double) mmx );

    int mm_per_knot = 10;
    current_draw_scaler = mm_per_knot * m_pix_per_mm * g_current_arrow_scale / 100.0;
    pscratch_bm = NULL;
    proute_bm = NULL;

    m_prot_bm = NULL;

// Set some benign initial values

    m_cs = GLOBAL_COLOR_SCHEME_DAY;
    VPoint.clat = 0;
    VPoint.clon = 0;
    VPoint.view_scale_ppm = 1;
    VPoint.Invalidate();

    m_canvas_scale_factor = 1.;

    m_canvas_width = 1000;

    m_overzoomTextWidth = 0;
    m_overzoomTextHeight = 0;

//    Create the default world chart
    pWorldBackgroundChart = new GSHHSChart;

//    Create the default depth unit emboss maps
    m_pEM_Feet = NULL;
    m_pEM_Meters = NULL;
    m_pEM_Fathoms = NULL;

    CreateDepthUnitEmbossMaps( GLOBAL_COLOR_SCHEME_DAY );

    m_pEM_OverZoom = NULL;
    SetOverzoomFont();
    CreateOZEmbossMapData( GLOBAL_COLOR_SCHEME_DAY );

//    Build icons for tide/current points
    m_bmTideDay = style->GetIcon( _T("tidesml") );

//    Dusk
    m_bmTideDusk = CreateDimBitmap( m_bmTideDay, .50 );

//    Night
    m_bmTideNight = CreateDimBitmap( m_bmTideDay, .20 );

//    Build Dusk/Night  ownship icons
    double factor_dusk = 0.5;
    double factor_night = 0.25;

    //Red
    m_os_image_red_day = style->GetIcon( _T("ship-red") ).ConvertToImage();

    int rimg_width = m_os_image_red_day.GetWidth();
    int rimg_height = m_os_image_red_day.GetHeight();

    m_os_image_red_dusk = m_os_image_red_day.Copy();
    m_os_image_red_night = m_os_image_red_day.Copy();

    for( int iy = 0; iy < rimg_height; iy++ ) {
        for( int ix = 0; ix < rimg_width; ix++ ) {
            if( !m_os_image_red_day.IsTransparent( ix, iy ) ) {
                wxImage::RGBValue rgb( m_os_image_red_day.GetRed( ix, iy ),
                                       m_os_image_red_day.GetGreen( ix, iy ),
                                       m_os_image_red_day.GetBlue( ix, iy ) );
                wxImage::HSVValue hsv = wxImage::RGBtoHSV( rgb );
                hsv.value = hsv.value * factor_dusk;
                wxImage::RGBValue nrgb = wxImage::HSVtoRGB( hsv );
                m_os_image_red_dusk.SetRGB( ix, iy, nrgb.red, nrgb.green, nrgb.blue );

                hsv = wxImage::RGBtoHSV( rgb );
                hsv.value = hsv.value * factor_night;
                nrgb = wxImage::HSVtoRGB( hsv );
                m_os_image_red_night.SetRGB( ix, iy, nrgb.red, nrgb.green, nrgb.blue );
            }
        }
    }

    //Grey
    m_os_image_grey_day = style->GetIcon( _T("ship-red") ).ConvertToImage().ConvertToGreyscale();

    int gimg_width = m_os_image_grey_day.GetWidth();
    int gimg_height = m_os_image_grey_day.GetHeight();

    m_os_image_grey_dusk = m_os_image_grey_day.Copy();
    m_os_image_grey_night = m_os_image_grey_day.Copy();

    for( int iy = 0; iy < gimg_height; iy++ ) {
        for( int ix = 0; ix < gimg_width; ix++ ) {
            if( !m_os_image_grey_day.IsTransparent( ix, iy ) ) {
                wxImage::RGBValue rgb( m_os_image_grey_day.GetRed( ix, iy ),
                                       m_os_image_grey_day.GetGreen( ix, iy ),
                                       m_os_image_grey_day.GetBlue( ix, iy ) );
                wxImage::HSVValue hsv = wxImage::RGBtoHSV( rgb );
                hsv.value = hsv.value * factor_dusk;
                wxImage::RGBValue nrgb = wxImage::HSVtoRGB( hsv );
                m_os_image_grey_dusk.SetRGB( ix, iy, nrgb.red, nrgb.green, nrgb.blue );

                hsv = wxImage::RGBtoHSV( rgb );
                hsv.value = hsv.value * factor_night;
                nrgb = wxImage::HSVtoRGB( hsv );
                m_os_image_grey_night.SetRGB( ix, iy, nrgb.red, nrgb.green, nrgb.blue );
            }
        }
    }


    // Yellow
    m_os_image_yellow_day = m_os_image_red_day.Copy();

    gimg_width = m_os_image_yellow_day.GetWidth();
    gimg_height = m_os_image_yellow_day.GetHeight();

    m_os_image_yellow_dusk = m_os_image_red_day.Copy();
    m_os_image_yellow_night = m_os_image_red_day.Copy();

    for( int iy = 0; iy < gimg_height; iy++ ) {
        for( int ix = 0; ix < gimg_width; ix++ ) {
            if( !m_os_image_yellow_day.IsTransparent( ix, iy ) ) {
                wxImage::RGBValue rgb( m_os_image_yellow_day.GetRed( ix, iy ),
                                       m_os_image_yellow_day.GetGreen( ix, iy ),
                                       m_os_image_yellow_day.GetBlue( ix, iy ) );
                wxImage::HSVValue hsv = wxImage::RGBtoHSV( rgb );
                hsv.hue += 60./360.;             //shift to yellow
                wxImage::RGBValue nrgb = wxImage::HSVtoRGB( hsv );
                m_os_image_yellow_day.SetRGB( ix, iy, nrgb.red, nrgb.green, nrgb.blue );

                hsv = wxImage::RGBtoHSV( rgb );
                hsv.value = hsv.value * factor_dusk;
                hsv.hue += 60./360.;             // shift to yellow
                nrgb = wxImage::HSVtoRGB( hsv );
                m_os_image_yellow_dusk.SetRGB( ix, iy, nrgb.red, nrgb.green, nrgb.blue );

                hsv = wxImage::RGBtoHSV( rgb );
                hsv.hue += 60./360.;             //shift to yellow
                hsv.value = hsv.value * factor_night;
                nrgb = wxImage::HSVtoRGB( hsv );
                m_os_image_yellow_night.SetRGB( ix, iy, nrgb.red, nrgb.green, nrgb.blue );
            }
        }
    }


    //  Set initial pointers to ownship images
    m_pos_image_red = &m_os_image_red_day;
    m_pos_image_yellow = &m_os_image_yellow_day;

    //  Look for user defined ownship image
    //  This may be found in the shared data location along with other user defined icons.
    //  and will be called "ownship.xpm" or "ownship.png"
    if( pWayPointMan && pWayPointMan->DoesIconExist( _T("ownship") ) ) {
        wxBitmap *pbmp = pWayPointMan->GetIconBitmap( _T("ownship") );
        m_pos_image_user_day = new wxImage;
        *m_pos_image_user_day = pbmp->ConvertToImage();
        m_pos_image_user_day->InitAlpha();

        int gimg_width = m_pos_image_user_day->GetWidth();
        int gimg_height = m_pos_image_user_day->GetHeight();

        // Make dusk and night images
        m_pos_image_user_dusk = new wxImage;
        m_pos_image_user_night = new wxImage;

        *m_pos_image_user_dusk = m_pos_image_user_day->Copy();
        *m_pos_image_user_night = m_pos_image_user_day->Copy();

        for( int iy = 0; iy < gimg_height; iy++ ) {
            for( int ix = 0; ix < gimg_width; ix++ ) {
                if( !m_pos_image_user_day->IsTransparent( ix, iy ) ) {
                    wxImage::RGBValue rgb( m_pos_image_user_day->GetRed( ix, iy ),
                                           m_pos_image_user_day->GetGreen( ix, iy ),
                                           m_pos_image_user_day->GetBlue( ix, iy ) );
                    wxImage::HSVValue hsv = wxImage::RGBtoHSV( rgb );
                    hsv.value = hsv.value * factor_dusk;
                    wxImage::RGBValue nrgb = wxImage::HSVtoRGB( hsv );
                    m_pos_image_user_dusk->SetRGB( ix, iy, nrgb.red, nrgb.green, nrgb.blue );

                    hsv = wxImage::RGBtoHSV( rgb );
                    hsv.value = hsv.value * factor_night;
                    nrgb = wxImage::HSVtoRGB( hsv );
                    m_pos_image_user_night->SetRGB( ix, iy, nrgb.red, nrgb.green, nrgb.blue );
                }
            }
        }

        //  Make some alternate greyed out day/dusk/night images
        m_pos_image_user_grey_day = new wxImage;
        *m_pos_image_user_grey_day = m_pos_image_user_day->ConvertToGreyscale();

        m_pos_image_user_grey_dusk = new wxImage;
        m_pos_image_user_grey_night = new wxImage;

        *m_pos_image_user_grey_dusk = m_pos_image_user_grey_day->Copy();
        *m_pos_image_user_grey_night = m_pos_image_user_grey_day->Copy();

        for( int iy = 0; iy < gimg_height; iy++ ) {
            for( int ix = 0; ix < gimg_width; ix++ ) {
                if( !m_pos_image_user_grey_day->IsTransparent( ix, iy ) ) {
                    wxImage::RGBValue rgb( m_pos_image_user_grey_day->GetRed( ix, iy ),
                                           m_pos_image_user_grey_day->GetGreen( ix, iy ),
                                           m_pos_image_user_grey_day->GetBlue( ix, iy ) );
                    wxImage::HSVValue hsv = wxImage::RGBtoHSV( rgb );
                    hsv.value = hsv.value * factor_dusk;
                    wxImage::RGBValue nrgb = wxImage::HSVtoRGB( hsv );
                    m_pos_image_user_grey_dusk->SetRGB( ix, iy, nrgb.red, nrgb.green, nrgb.blue );

                    hsv = wxImage::RGBtoHSV( rgb );
                    hsv.value = hsv.value * factor_night;
                    nrgb = wxImage::HSVtoRGB( hsv );
                    m_pos_image_user_grey_night->SetRGB( ix, iy, nrgb.red, nrgb.green, nrgb.blue );
                }
            }
        }

        //  Make a yellow image for rendering under low accuracy chart conditions
        m_pos_image_user_yellow_day = new wxImage;
        m_pos_image_user_yellow_dusk = new wxImage;
        m_pos_image_user_yellow_night = new wxImage;

        *m_pos_image_user_yellow_day = m_pos_image_user_grey_day->Copy();
        *m_pos_image_user_yellow_dusk = m_pos_image_user_grey_day->Copy();
        *m_pos_image_user_yellow_night = m_pos_image_user_grey_day->Copy();

        for( int iy = 0; iy < gimg_height; iy++ ) {
            for( int ix = 0; ix < gimg_width; ix++ ) {
                if( !m_pos_image_user_grey_day->IsTransparent( ix, iy ) ) {
                    wxImage::RGBValue rgb( m_pos_image_user_grey_day->GetRed( ix, iy ),
                                           m_pos_image_user_grey_day->GetGreen( ix, iy ),
                                           m_pos_image_user_grey_day->GetBlue( ix, iy ) );

                    //  Simply remove all "blue" from the greyscaled image...
                    //  so, what is not black becomes yellow.
                    wxImage::HSVValue hsv = wxImage::RGBtoHSV( rgb );
                    wxImage::RGBValue nrgb = wxImage::HSVtoRGB( hsv );
                    m_pos_image_user_yellow_day->SetRGB( ix, iy, nrgb.red, nrgb.green, 0 );

                    hsv = wxImage::RGBtoHSV( rgb );
                    hsv.value = hsv.value * factor_dusk;
                    nrgb = wxImage::HSVtoRGB( hsv );
                    m_pos_image_user_yellow_dusk->SetRGB( ix, iy, nrgb.red, nrgb.green, 0 );

                    hsv = wxImage::RGBtoHSV( rgb );
                    hsv.value = hsv.value * factor_night;
                    nrgb = wxImage::HSVtoRGB( hsv );
                    m_pos_image_user_yellow_night->SetRGB( ix, iy, nrgb.red, nrgb.green, 0 );
                }
            }
        }

    }

    m_pBrightPopup = NULL;
    m_pQuilt = new Quilt();
    
#ifdef ocpnUSE_GL
    if ( !g_bdisable_opengl )
        m_pQuilt->EnableHighDefinitionZoom( true );
#endif    
        
}

ChartCanvas::~ChartCanvas()
{

    delete pThumbDIBShow;

//    Delete Cursors
    delete pCursorLeft;
    delete pCursorRight;
    delete pCursorUp;
    delete pCursorDown;
    delete pCursorArrow;
    delete pCursorPencil;
    delete pCursorCross;

    delete pPanTimer;
    delete pMovementTimer;
    delete pMovementStopTimer;
    delete pCurTrackTimer;
    delete pRotDefTimer;
    delete m_DoubleClickTimer;

    delete m_pRouteRolloverWin;
    delete m_pAISRolloverWin;
    delete m_pBrightPopup;

    delete m_pCIWin;

    delete pscratch_bm;

    m_dc_route.SelectObject( wxNullBitmap );
    delete proute_bm;

    delete pWorldBackgroundChart;
    delete pss_overlay_bmp;

    delete m_pEM_Feet;
    delete m_pEM_Meters;
    delete m_pEM_Fathoms;

    delete m_pEM_OverZoom;
//        delete m_pEM_CM93Offset;

    delete m_pQuilt;

    delete m_prot_bm;

    delete m_pos_image_user_day;
    delete m_pos_image_user_dusk;
    delete m_pos_image_user_night;
    delete m_pos_image_user_grey_day;
    delete m_pos_image_user_grey_dusk;
    delete m_pos_image_user_grey_night;
    delete m_pos_image_user_yellow_day;
    delete m_pos_image_user_yellow_dusk;
    delete m_pos_image_user_yellow_night;

    delete undo;
#ifdef ocpnUSE_GL
    if( !g_bdisable_opengl )
        delete m_glcc;
#endif

}

void ChartCanvas::OnEvtCompressProgress( OCPN_CompressProgressEvent & event )
{
    wxString msg(event.m_string.c_str(), wxConvUTF8);
    if(compress_msg_array.GetCount() > (unsigned int)event.thread )
    {
        compress_msg_array.RemoveAt(event.thread);
        compress_msg_array.Insert( msg, event.thread);
    }
    else
        compress_msg_array.Add(msg);
    
    
    wxString combined_msg;
    for(unsigned int i=0 ; i < compress_msg_array.GetCount() ; i++) {
        combined_msg += compress_msg_array[i];
        combined_msg += _T("\n");
    }
    
    bool skip = false;
    pprog->Update(event.count-1, combined_msg, &skip );
    pprog->SetSize(pprog_size);
    if(skip)
        b_skipout = skip;
    
}

void ChartCanvas::InvalidateGL()
{
#ifdef ocpnUSE_GL
        if(g_bopengl)
            glChartCanvas::Invalidate();
#endif
}

int ChartCanvas::GetCanvasChartNativeScale()
{
    int ret = 1;
    if( !VPoint.b_quilt ) {
        if( Current_Ch ) ret = Current_Ch->GetNativeScale();
    } else
        ret = (int) m_pQuilt->GetRefNativeScale();

    return ret;

}

ChartBase* ChartCanvas::GetChartAtCursor() {
    ChartBase* target_chart;
    if( Current_Ch && ( Current_Ch->GetChartFamily() == CHART_FAMILY_VECTOR ) )
        target_chart = Current_Ch;
    else
        if( VPoint.b_quilt )
            target_chart = cc1->m_pQuilt->GetChartAtPix( wxPoint( mouse_x, mouse_y ) );
        else
            target_chart = NULL;
    return target_chart;
}

ChartBase* ChartCanvas::GetOverlayChartAtCursor() {
    ChartBase* target_chart;
    if( VPoint.b_quilt )
        target_chart = cc1->m_pQuilt->GetOverlayChartAtPix( wxPoint( mouse_x, mouse_y ) );
    else
        target_chart = NULL;
    return target_chart;
}

int ChartCanvas::FindClosestCanvasChartdbIndex( int scale )
{
    int new_dbIndex = -1;
    if( !VPoint.b_quilt ) {
        if( pCurrentStack ) {
            for( int i = 0; i < pCurrentStack->nEntry; i++ ) {
                int sc = ChartData->GetStackChartScale( pCurrentStack, i, NULL, 0 );
                if( sc >= scale ) {
                    new_dbIndex = pCurrentStack->GetDBIndex( i );
                    break;
                }
            }
        }
    } else {
        //    Using the current quilt, select a useable reference chart
        //    Said chart will be in the extended (possibly full-screen) stack,
        //    And will have a scale equal to or just greater than the stipulated value
        unsigned int im = m_pQuilt->GetExtendedStackIndexArray().GetCount();
        if( im > 0 ) {
            for( unsigned int is = 0; is < im; is++ ) {
                const ChartTableEntry &m = ChartData->GetChartTableEntry(
                                               m_pQuilt->GetExtendedStackIndexArray().Item( is ) );
                if( ( m.GetScale() >= scale )/* && (m_reference_family == m.GetChartFamily())*/) {
                    new_dbIndex = m_pQuilt->GetExtendedStackIndexArray().Item( is );
                    break;
                }
            }
        }
    }

    return new_dbIndex;
}

void ChartCanvas::EnablePaint(bool b_enable)
{
    m_b_paint_enable = b_enable;
#ifdef ocpnUSE_GL
    if(m_glcc)
        m_glcc->EnablePaint(b_enable);
#endif
}

bool ChartCanvas::IsQuiltDelta()
{
    return m_pQuilt->IsQuiltDelta( VPoint );
}

ArrayOfInts ChartCanvas::GetQuiltIndexArray( void )
{
    return m_pQuilt->GetQuiltIndexArray();;
}

void ChartCanvas::SetQuiltMode( bool b_quilt )
{
    VPoint.b_quilt = b_quilt;
    VPoint.b_FullScreenQuilt = g_bFullScreenQuilt;
}

bool ChartCanvas::GetQuiltMode( void )
{
    return VPoint.b_quilt;
}

int ChartCanvas::GetQuiltReferenceChartIndex(void)
{
    return m_pQuilt->GetRefChartdbIndex();
}

void ChartCanvas::InvalidateAllQuiltPatchs( void )
{
    m_pQuilt->InvalidateAllQuiltPatchs();
}

ChartBase *ChartCanvas::GetLargestScaleQuiltChart()
{
    return m_pQuilt->GetLargestScaleChart();
}

ChartBase *ChartCanvas::GetFirstQuiltChart()
{
    return m_pQuilt->GetFirstChart();
}

ChartBase *ChartCanvas::GetNextQuiltChart()
{
    return m_pQuilt->GetNextChart();
}

int ChartCanvas::GetQuiltChartCount()
{
    return m_pQuilt->GetnCharts();
}

void ChartCanvas::SetQuiltChartHiLiteIndex( int dbIndex )
{
    m_pQuilt->SetHiliteIndex( dbIndex );
}

ArrayOfInts ChartCanvas::GetQuiltCandidatedbIndexArray( bool flag1, bool flag2 )
{
    return m_pQuilt->GetCandidatedbIndexArray( flag1, flag2 );
}

int ChartCanvas::GetQuiltRefChartdbIndex( void )
{
    return m_pQuilt->GetRefChartdbIndex();
}

ArrayOfInts ChartCanvas::GetQuiltExtendedStackdbIndexArray()
{
    return m_pQuilt->GetExtendedStackIndexArray();
}

ArrayOfInts ChartCanvas::GetQuiltEclipsedStackdbIndexArray()
{
    return m_pQuilt->GetEclipsedStackIndexArray();
}

void ChartCanvas::InvalidateQuilt( void )
{
    return m_pQuilt->Invalidate();
}

double ChartCanvas::GetQuiltMaxErrorFactor()
{
    return m_pQuilt->GetMaxErrorFactor();
}

bool ChartCanvas::IsChartQuiltableRef( int db_index )
{
    return m_pQuilt->IsChartQuiltableRef( db_index );
}

bool ChartCanvas::IsChartLargeEnoughToRender( ChartBase* chart, ViewPort& vp )
{
    double chartMaxScale = chart->GetNormalScaleMax( GetCanvasScaleFactor(), GetCanvasWidth() );
    return ( chartMaxScale*g_ChartNotRenderScaleFactor > vp.chart_scale );
}

void ChartCanvas::CancelMeasureRoute()
{
    m_bMeasure_Active = false;
    m_nMeasureState = 0;
    g_pRouteMan->DeleteRoute( m_pMeasureRoute );
    m_pMeasureRoute = NULL;
}

ViewPort &ChartCanvas::GetVP()
{
    return VPoint;
}

void ChartCanvas::SetVP(ViewPort &vp)
{
    VPoint = vp;
}

void ChartCanvas::OnKeyChar( wxKeyEvent &event )
{
    int key_char = event.GetKeyCode();
    
    //      Handle both QWERTY and AZERTY keyboard separately for a few control codes
    if( !g_b_assume_azerty ) {
        switch( key_char ) {
            case ']':
                RotateCanvas( 1 );
                break;
                
            case '[':
                RotateCanvas( -1 );
                break;
                
            case '\\':
                DoRotateCanvas(0);
                break;
        }
    }
#if 0    
    else {
        switch( key_char ) {
            case 43:
                ZoomCanvas( 2.0 );
                break;
            
            case 54:                     // '-'  alpha/num pad
            case 56:                     // '_'  alpha/num pad
                ZoomCanvas( 0.5 );
                break;
        }
    }
#endif    
}    



void ChartCanvas::OnKeyDown( wxKeyEvent &event )
{
    m_modkeys = event.GetModifiers();

    int panspeed = m_modkeys == wxMOD_ALT ? 2 : 100;

    // HOTKEYS
    switch( event.GetKeyCode() ) {
    case WXK_ALT:
        m_modkeys |= wxMOD_ALT;
        break;

    case WXK_CONTROL:
        m_modkeys |= wxMOD_CONTROL;
        break;

    case WXK_LEFT:
        if( m_modkeys == wxMOD_CONTROL ) parent_frame->DoStackDown();
        else if(g_bsmoothpanzoom) {
            StartTimedMovement();
            m_panx = -1;
        } else
            PanCanvas( -panspeed, 0 );
        break;

    case WXK_UP:
        if(g_bsmoothpanzoom) {
            StartTimedMovement();
            m_pany = -1;
        } else
            PanCanvas( 0, -panspeed );
        break;

    case WXK_RIGHT:
        if( m_modkeys == wxMOD_CONTROL ) parent_frame->DoStackUp();
        else if(g_bsmoothpanzoom) {
            StartTimedMovement();
            m_panx = 1;
        } else
            PanCanvas( panspeed, 0 );
        break;

    case WXK_DOWN:
        if(g_bsmoothpanzoom) {
            StartTimedMovement();
            m_pany = 1;
        } else
            PanCanvas( 0, panspeed );
        break;

    case WXK_F2:
        parent_frame->TogglebFollow();
        break;

    case WXK_F3: {
        parent_frame->ToggleENCText();
        break;
    }
    case WXK_F4:
        if( !parent_frame->nRoute_State )   // no measure tool if currently creating route
        {
            if( m_bMeasure_Active ) {
                g_pRouteMan->DeleteRoute( m_pMeasureRoute );
                m_pMeasureRoute = NULL;
            }

            m_bMeasure_Active = true;
            m_nMeasureState = 1;
            SetCursor( *pCursorPencil );
            Refresh();
        }
        break;

    case WXK_F5:
        parent_frame->ToggleColorScheme();
        break;

    case WXK_F6: {
        int mod = m_modkeys & wxMOD_SHIFT;
        if( mod != m_brightmod ) {
            m_brightmod = mod;
            m_bbrightdir = !m_bbrightdir;
        }

        if( !m_bbrightdir ) {
            g_nbrightness -= 10;
            if( g_nbrightness <= MIN_BRIGHT ) {
                g_nbrightness = MIN_BRIGHT;
                m_bbrightdir = true;
            }
        } else {
            g_nbrightness += 10;
            if( g_nbrightness >= MAX_BRIGHT ) {
                g_nbrightness = MAX_BRIGHT;
                m_bbrightdir = false;
            }
        }

        SetScreenBrightness( g_nbrightness );
        ShowBrightnessLevelTimedPopup( g_nbrightness / 10, 1, 10 );

        SetFocus();             // just in case the external program steals it....
        gFrame->Raise();        // And reactivate the application main

        break;
    }

    case WXK_F7:
        parent_frame->DoStackDown();
        break;

    case WXK_F8:
        parent_frame->DoStackUp();
        break;

    case WXK_F9: {
        parent_frame->ToggleQuiltMode();
        ReloadVP();
        break;
    }

    case WXK_F11:
        parent_frame->ToggleFullScreen();
        break;
#ifdef __WXOSX__
    case WXK_ESCAPE:
//        if( IsFullScreen() )
            parent_frame->ToggleFullScreen();
        break;
#endif
    case WXK_F12: {
//        if( m_modkeys == wxMOD_ALT )
//            m_nMeasureState = *(int *)(0);          // generate a fault for testing

        parent_frame->ToggleChartOutlines();
        break;
    }

    //NUMERIC PAD
    case WXK_NUMPAD_ADD:              // '+' on NUM PAD
    case WXK_PAGEUP:
        ZoomCanvas( 2.0 );
        break;

    case WXK_NUMPAD_SUBTRACT:   // '-' on NUM PAD
    case WXK_PAGEDOWN:
        ZoomCanvas( .5 );
        break;

    default:
        break;

    }

    if( event.GetKeyCode() < 128 )            //ascii
    {
        int key_char = event.GetKeyCode();

        //      Handle both QWERTY and AZERTY keyboard separately for a few control codes
        if( !g_b_assume_azerty ) {
            switch( key_char ) {
            case '+': case '=':
                ZoomCanvas( 2.0 );
                break;

            case '-': case '_':
                ZoomCanvas( 0.5 );
                break;

            case ']':
//                RotateCanvas( 1 );
                break;
                
            case '[':
//                RotateCanvas( -1 );
                break;
                
            case '\\':
//                DoRotateCanvas(0);
                break;
            }
        } else {
            switch( key_char ) {
            case 43:
                ZoomCanvas( 2.0 );
                break;

            case 54:                     // '-'  alpha/num pad
            case 56:                     // '_'  alpha/num pad
                ZoomCanvas( 0.5 );
                break;
            }
        }

        if( m_modkeys == wxMOD_CONTROL )
            key_char -= 64;

        switch( key_char ) {
        case 'A':
            parent_frame->ToggleAnchor();
            break;

        case 'D': {
                int x,y;
                event.GetPosition( &x, &y );
                bool cm93IsAvailable = ( Current_Ch && ( Current_Ch->GetChartType() == CHART_TYPE_CM93COMP ) );
                if( VPoint.b_quilt ) {
                    ChartBase *pChartTest = m_pQuilt->GetChartAtPix( wxPoint( x, y ) );
                    if( pChartTest ) {
                        if( pChartTest->GetChartType() == CHART_TYPE_CM93 ) cm93IsAvailable = true;
                        if( pChartTest->GetChartType() == CHART_TYPE_CM93COMP ) cm93IsAvailable = true;
                    }
                }

                if( cm93IsAvailable ) {
                    if( !pCM93DetailSlider ) {
                        pCM93DetailSlider = new CM93DSlide( this, -1, 0,
                                -CM93_ZOOM_FACTOR_MAX_RANGE, CM93_ZOOM_FACTOR_MAX_RANGE,
                                wxPoint( g_cm93detail_dialog_x, g_cm93detail_dialog_y ),
                                wxDefaultSize, wxSIMPLE_BORDER, _("CM93 Detail Level") );
                    }
                    pCM93DetailSlider->Show( !pCM93DetailSlider->IsShown() );
                }
                break;
            }

       case 'L':
            parent_frame->ToggleLights();
            break;

        case 'O':
            parent_frame->ToggleChartOutlines();
            break;

#if 0
        case 'R':
            parent_frame->ToggleRocks();
            break;
#endif
        case 'S':
            parent_frame->ToggleSoundings();
            break;

        case 'T':
            parent_frame->ToggleENCText();
            break;

        case 1:                      // Ctrl A
            parent_frame->TogglebFollow();
            break;

        case 2:                      // Ctrl B
            if( stats ) {
                if( stats->IsShown() )
                    stats->Hide();
                else {
                    stats->Move(0,0);
                    stats->RePosition();
                    stats->Show();
                    gFrame->Raise();
                }
                Refresh();
            }
            break;
#ifdef __WXOSX__
        case 10:
                parent_frame->startHelp();  // Ctrl J
                break;
#endif

        case 13:             // Ctrl M // Drop Marker at cursor
        {
            double lat, lon;
            lat = m_cursor_lat;
            lon = m_cursor_lon;
            RoutePoint *pWP = new RoutePoint( lat, lon, g_default_wp_icon, wxEmptyString,
                                              GPX_EMPTY_STRING );
            pWP->m_bIsolatedMark = true;                      // This is an isolated mark
            pSelect->AddSelectableRoutePoint( lat, lon, pWP );
            pConfig->AddNewWayPoint( pWP, -1 );    // use auto next num

            if( pRouteManagerDialog && pRouteManagerDialog->IsShown() ) pRouteManagerDialog->UpdateWptListCtrl();
            undo->BeforeUndoableAction( Undo_CreateWaypoint, pWP, Undo_HasParent, NULL );
            undo->AfterUndoableAction( NULL );
            InvalidateGL();
            Refresh( false );
            break;
        }

        case 14:             // Ctrl N - Activate next waypoint in a route
        {
            if( Route * r = g_pRouteMan->GetpActiveRoute() ) {
                int indexActive = r->GetIndexOf( r->m_pRouteActivePoint );
                if( ( indexActive + 1 ) <= r->GetnPoints() ) {
                    g_pRouteMan->ActivateNextPoint( r, true );
                    InvalidateGL();
                    Refresh( false );
                }
            }
            break;
        }

        case 15:             // Ctrl O - Drop Marker at boat's position
        {
            RoutePoint *pWP = new RoutePoint( gLat, gLon, g_default_wp_icon, wxEmptyString,
                                              GPX_EMPTY_STRING );
            pWP->m_bIsolatedMark = true;                      // This is an isolated mark
            pSelect->AddSelectableRoutePoint( gLat, gLon, pWP );
            pConfig->AddNewWayPoint( pWP, -1 );    // use auto next num

            if( pRouteManagerDialog && pRouteManagerDialog->IsShown() ) pRouteManagerDialog->UpdateWptListCtrl();
            undo->BeforeUndoableAction( Undo_CreateWaypoint, pWP, Undo_HasParent, NULL );
            undo->AfterUndoableAction( NULL );
            InvalidateGL();
            Refresh( false );
            break;
        }

        case -32:                     // Ctrl Space            //    Drop MOB
        {
            if( m_modkeys == wxMOD_CONTROL ) parent_frame->ActivateMOB();

            break;
        }

        case 17:                       // Ctrl Q
            parent_frame->Close();
            return;

        case 18:                       // Ctrl R
            gFrame->nRoute_State = 1;
            cc1->SetCursor( *cc1->pCursorPencil );
            return;

        case 20:                       // Ctrl T
            if( NULL == pGoToPositionDialog ) // There is one global instance of the Go To Position Dialog
                pGoToPositionDialog = new GoToPositionDialog( this );
            pGoToPositionDialog->Show();
            break;

        case 25:                       // Ctrl Y
            if( undo->AnythingToRedo() ) {
                undo->RedoNextAction();
                InvalidateGL();
                Refresh( false );
            }
            break;

        case 26:                       // Ctrl Z
            if( undo->AnythingToUndo() ) {
                undo->UndoLastAction();
                InvalidateGL();
                Refresh( false );
            }
            break;

        case 27:
            // Generic break
            if( m_bMeasure_Active ) {
                m_bMeasure_Active = false;
                m_nMeasureState = 0;
                g_pRouteMan->DeleteRoute( m_pMeasureRoute );
                m_pMeasureRoute = NULL;
                SetCursor( *pCursorArrow );
                gFrame->SurfaceToolbar();
                InvalidateGL();
                Refresh( false );
            }

            if( parent_frame->nRoute_State )         // creating route?
            {
                FinishRoute();
                gFrame->SurfaceToolbar();
                InvalidateGL();
                Refresh( false );
            }

            break;

        case 7:                       // Ctrl G
            switch( gamma_state ) {
            case ( 0 ):
                r_gamma_mult = 0;
                g_gamma_mult = 1;
                b_gamma_mult = 0;
                gamma_state = 1;
                break;
            case ( 1 ):
                r_gamma_mult = 1;
                g_gamma_mult = 0;
                b_gamma_mult = 0;
                gamma_state = 2;
                break;
            case ( 2 ):
                r_gamma_mult = 1;
                g_gamma_mult = 1;
                b_gamma_mult = 1;
                gamma_state = 0;
                break;
            }
            SetScreenBrightness( g_nbrightness );

            break;

        case 9:                      // Ctrl I
            if( g_FloatingCompassDialog ) {
                if( g_FloatingCompassDialog->IsShown() ) {
                    g_FloatingCompassDialog->Hide();
                } else {
                    g_FloatingCompassDialog->Show();
                }
                gFrame->Raise();
                Refresh();
            }
            break;

        default:
            break;

        }           // switch
    }

#ifndef __WXMAC__
    event.Skip();
#endif
}

void ChartCanvas::OnKeyUp( wxKeyEvent &event )
{
    switch( event.GetKeyCode() ) {
    case WXK_LEFT:
    case WXK_RIGHT:
        m_panx = 0;
        if(!m_pany)
            m_panspeed = 0;
        break;

    case WXK_UP:
    case WXK_DOWN:
        m_pany = 0;
        if(!m_panx)
            m_panspeed = 0;
        break;

    case WXK_NUMPAD_ADD:              // '+' on NUM PAD
    case WXK_NUMPAD_SUBTRACT:   // '-' on NUM PAD
    case WXK_PAGEUP:
    case WXK_PAGEDOWN:
        if(m_mustmove)
            DoMovement(m_mustmove);

        m_zoom_factor = 1;
        break;

    case WXK_ALT:
        m_modkeys &= ~wxMOD_ALT;
        break;

    case WXK_CONTROL:
        m_modkeys &= ~wxMOD_CONTROL;
        break;

    }

    if( event.GetKeyCode() < 128 )            //ascii
    {
        int key_char = event.GetKeyCode();

        //      Handle both QWERTY and AZERTY keyboard separately for a few control codes
        if( !g_b_assume_azerty ) {
            switch( key_char ) {
            case '+':     case '=':    
            case '-':     case '_':
              case 54:    case 56:    // '_'  alpha/num pad
                DoMovement(m_mustmove);

                m_zoom_factor = 1;
                break;
            case '[': case ']':
                DoMovement(m_mustmove);
                m_rotation_speed = 0;
                break;
            }
        } else {
            switch( key_char ) {
            case 43:
            case 54:                     // '-'  alpha/num pad
            case 56:                     // '_'  alpha/num pad
                DoMovement(m_mustmove);

                m_zoom_factor = 1;
                break;
            }
        }
    }
    event.Skip();
}

void ChartCanvas::StopMovement( )
{
    m_panx = m_pany = 0;
    m_panspeed = 0;
    m_zoom_factor = 1;
    m_rotation_speed = 0;
    m_mustmove = 0;
    SetFocus();
    gFrame->Raise();
}

/* instead of integrating in timer callbacks
   (which do not always get called fast enough)
   we can perform the integration of movement
   at each render frame based on the time change */
bool ChartCanvas::StartTimedMovement( bool stoptimer )
{
    // Start/restart the stop movement timer
    if(stoptimer)
        pMovementStopTimer->Start( 1000, wxTIMER_ONE_SHOT ); 

    if(m_panx || m_pany || m_zoom_factor!=1 || m_rotation_speed) {
        // already moving, gets called again because of key-repeat event
        return false;
    }

    m_last_movement_time = wxDateTime::UNow();

    /* jumpstart because paint gets called right away, if we want first frame to move */
//    m_last_movement_time -= wxTimeSpan::Milliseconds(100);

    Refresh( false );

    return true;
}

void ChartCanvas::DoTimedMovement()
{
    if(!m_panx && !m_pany && m_zoom_factor==1 && !m_rotation_speed)
        return; /* not moving */

    wxDateTime now = wxDateTime::UNow();
    long dt = 0;
    if(m_last_movement_time.IsValid())
        dt = (now - m_last_movement_time).GetMilliseconds().ToLong();

    m_last_movement_time = now;
        
    if(dt > 500) /* if we are running very slow, don't integrate too fast */
        dt = 500;

    DoMovement(dt);
}

void ChartCanvas::DoMovement( long dt )
{
    /* if we get here quickly assume 1ms so that some movement occurs */
    if(dt == 0)
        dt = 1;

    m_mustmove -= dt;
    if(m_mustmove < 0)
        m_mustmove = 0;

    if(m_panx || m_pany) {
        const double slowpan = .1, maxpan = 2;
#ifdef __WXOSX__
        if( m_modkeys == wxMOD_CONTROL )
#else
        if( m_modkeys == wxMOD_ALT )
#endif
            m_panspeed = slowpan;
        else {
            m_panspeed += (double)dt/500; /* apply acceleration */
            m_panspeed = wxMin( maxpan, m_panspeed );
        }
        PanCanvas( m_panspeed * m_panx * dt, m_panspeed * m_pany * dt);
    }

    if(m_zoom_factor != 1) {
        double alpha = 400, beta = 1.5;
        double zoom_factor = (exp(dt / alpha) - 1) / beta + 1;
#ifdef __WXOSX__
        if( m_modkeys == wxMOD_CONTROL )
#else
        if( m_modkeys == wxMOD_ALT )
#endif
            zoom_factor = pow(zoom_factor, .15);

        if(m_zoom_factor < 1)
            zoom_factor = 1/zoom_factor;
        
        //  Try to hit the zoom target exactly.
        if(zoom_factor > 1){
            if(  VPoint.chart_scale / zoom_factor <= m_zoom_target)
                zoom_factor = VPoint.chart_scale / m_zoom_target;
        }

        else if(zoom_factor < 1){
            if(  VPoint.chart_scale / zoom_factor >= m_zoom_target)
                zoom_factor = VPoint.chart_scale / m_zoom_target;
        }
        
        DoZoomCanvas( zoom_factor, m_bzooming_to_cursor );
        
        if(m_wheelzoom_stop_oneshot > 0 &&
           m_wheelstopwatch.Time() > m_wheelzoom_stop_oneshot){
            m_wheelzoom_stop_oneshot = 0;
            StopMovement( );
        }
        
        //      Don't overshoot the zoom target.
        if(zoom_factor > 1){
            if(  VPoint.chart_scale <= m_zoom_target){
                m_wheelzoom_stop_oneshot = 0;
                StopMovement( );
            }
        }
        else if(zoom_factor < 1){
            if(  VPoint.chart_scale >= m_zoom_target){
                m_wheelzoom_stop_oneshot = 0;
                StopMovement( );
            }
        }
        
                
    }

    if( m_rotation_speed ) { /* in degrees per second */
        double speed = m_rotation_speed;
        if( m_modkeys == wxMOD_ALT)
            speed /= 10;
        DoRotateCanvas( VPoint.rotation + speed * PI / 180 * dt / 1000.0);
    }
}

void ChartCanvas::SetColorScheme( ColorScheme cs )
{
    //    Setup ownship image pointers
    switch( cs ) {
    case GLOBAL_COLOR_SCHEME_DAY:
        m_pos_image_red = &m_os_image_red_day;
        m_pos_image_grey = &m_os_image_grey_day;
        m_pos_image_yellow = &m_os_image_yellow_day;
        m_pos_image_user = m_pos_image_user_day;
        m_pos_image_user_grey = m_pos_image_user_grey_day;
        m_pos_image_user_yellow = m_pos_image_user_yellow_day;
        break;
    case GLOBAL_COLOR_SCHEME_DUSK:
        m_pos_image_red = &m_os_image_red_dusk;
        m_pos_image_grey = &m_os_image_grey_dusk;
        m_pos_image_yellow = &m_os_image_yellow_dusk;
        m_pos_image_user = m_pos_image_user_dusk;
        m_pos_image_user_grey = m_pos_image_user_grey_dusk;
        m_pos_image_user_yellow = m_pos_image_user_yellow_dusk;
        break;
    case GLOBAL_COLOR_SCHEME_NIGHT:
        m_pos_image_red = &m_os_image_red_night;
        m_pos_image_grey = &m_os_image_grey_night;
        m_pos_image_yellow = &m_os_image_yellow_night;
        m_pos_image_user = m_pos_image_user_night;
        m_pos_image_user_grey = m_pos_image_user_grey_night;
        m_pos_image_user_yellow = m_pos_image_user_yellow_night;
        break;
    default:
        m_pos_image_red = &m_os_image_red_day;
        m_pos_image_grey = &m_os_image_grey_day;
        m_pos_image_yellow = &m_os_image_yellow_day;
        m_pos_image_user = m_pos_image_user_day;
        m_pos_image_user_grey = m_pos_image_user_grey_day;
        m_pos_image_user_yellow = m_pos_image_user_yellow_day;
        break;
    }

    CreateDepthUnitEmbossMaps( cs );
    CreateOZEmbossMapData( cs );

#ifdef ocpnUSE_GL
    if( g_bopengl && m_glcc ){
        m_glcc->ClearAllRasterTextures();
        m_glcc->FlushFBO(); 
    }
#endif
    SetbTCUpdate( true );                        // force re-render of tide/current locators

    ReloadVP();

    m_cs = cs;
}

wxBitmap ChartCanvas::CreateDimBitmap( wxBitmap &Bitmap, double factor )
{
    wxImage img = Bitmap.ConvertToImage();
    int sx = img.GetWidth();
    int sy = img.GetHeight();

    wxImage new_img( img );

    for( int i = 0; i < sx; i++ ) {
        for( int j = 0; j < sy; j++ ) {
            if( !img.IsTransparent( i, j ) ) {
                new_img.SetRGB( i, j, (unsigned char) ( img.GetRed( i, j ) * factor ),
                                (unsigned char) ( img.GetGreen( i, j ) * factor ),
                                (unsigned char) ( img.GetBlue( i, j ) * factor ) );
            }
        }
    }

    wxBitmap ret = wxBitmap( new_img );

    return ret;

}

void ChartCanvas::ShowBrightnessLevelTimedPopup( int brightness, int min, int max )
{
    wxFont *pfont = wxTheFontList->FindOrCreateFont( 40, wxDEFAULT, wxNORMAL, wxBOLD );

    if( !m_pBrightPopup ) {
        //    Calculate size
        int x, y;
        GetTextExtent( _T("MAX"), &x, &y, NULL, NULL, pfont );

        m_pBrightPopup = new TimedPopupWin( this, 3);

        m_pBrightPopup->SetSize(x, y);
        m_pBrightPopup->Move(120,120);
    }

    int bmpsx = m_pBrightPopup->GetSize().x;
    int bmpsy = m_pBrightPopup->GetSize().y;

    wxBitmap bmp( bmpsx, bmpsx );
    wxMemoryDC mdc( bmp );

    mdc.SetTextForeground( GetGlobalColor( _T("GREEN4") ) );
    mdc.SetBackground( wxBrush( GetGlobalColor( _T("UINFD") ) ) );
    mdc.SetPen( wxPen( wxColour( 0, 0, 0 ) ) );
    mdc.SetBrush( wxBrush( GetGlobalColor( _T("UINFD") ) ) );
    mdc.Clear();

    mdc.DrawRectangle( 0, 0, bmpsx, bmpsy );

    mdc.SetFont( *pfont );
    wxString val;

    if( brightness == max ) val = _T("MAX");
    else
        if( brightness == min ) val = _T("MIN");
        else
            val.Printf( _T("%3d"), brightness );

    mdc.DrawText( val, 0, 0 );

    mdc.SelectObject( wxNullBitmap );

    m_pBrightPopup->SetBitmap( bmp );
    m_pBrightPopup->Show();
    m_pBrightPopup->Refresh();


}


void ChartCanvas::RotateTimerEvent( wxTimerEvent& event )
{
    m_b_rot_hidef = true;
    ReloadVP();
}

void ChartCanvas::OnRolloverPopupTimerEvent( wxTimerEvent& event )
{
    bool b_need_refresh = false;

    //  Handle the AIS Rollover Window first
    bool showAISRollover = false;
    if( g_pAIS && g_pAIS->GetNumTargets() && g_bShowAIS ) {
        SelectItem *pFind = pSelectAIS->FindSelection( m_cursor_lat, m_cursor_lon,
                                                       SELTYPE_AISTARGET );
        if( pFind ) {
            int FoundAIS_MMSI = (long) pFind->m_pData1; // cast to long avoids problems with 64bit compilers
            AIS_Target_Data *ptarget = g_pAIS->Get_Target_Data_From_MMSI( FoundAIS_MMSI );

            if( ptarget ) {
                showAISRollover = true;

                if( NULL == m_pAISRolloverWin ) {
                    m_pAISRolloverWin = new RolloverWin( this, 10 );
                    m_pAISRolloverWin->IsActive( false );
                    b_need_refresh = true;
                }

                //      Sometimes the mouse moves fast enough to get over a new AIS target before
                //      the one-shot has fired to remove the old target.
                //      Result:  wrong target data is shown.
                //      Detect this case,close the existing rollover ASAP, and restart the timer.
                if( m_pAISRolloverWin && m_pAISRolloverWin->IsActive() &&
                    m_AISRollover_MMSI && (m_AISRollover_MMSI != FoundAIS_MMSI) ){
                    m_RolloverPopupTimer.Start( 50, wxTIMER_ONE_SHOT );
                    m_pAISRolloverWin->IsActive( false );
                    m_AISRollover_MMSI = 0;
                    Refresh();
                    return;
                }

                m_AISRollover_MMSI = FoundAIS_MMSI;

                if( !m_pAISRolloverWin->IsActive() ) {

                    wxString s = ptarget->GetRolloverString();
                    m_pAISRolloverWin->SetString( s );

                    wxSize win_size = GetSize();
                    if( console->IsShown() ) win_size.x -= console->GetSize().x;
                    m_pAISRolloverWin->SetBestPosition( mouse_x, mouse_y, 16, 16, AIS_ROLLOVER, win_size );

                    m_pAISRolloverWin->SetBitmap( AIS_ROLLOVER );
                    m_pAISRolloverWin->IsActive( true );
                    b_need_refresh = true;
                }
            }
        }
        else {
            m_AISRollover_MMSI = 0;
            showAISRollover = false;
        }
    }

    //  Maybe turn the rollover off
    if( m_pAISRolloverWin && m_pAISRolloverWin->IsActive() && !showAISRollover ) {
        m_pAISRolloverWin->IsActive( false );
        m_AISRollover_MMSI = 0;
        b_need_refresh = true;
    }

    // Now the Route info rollover
    // Show the route segment info
    bool showRollover = false;

    if( NULL == m_pRolloverRouteSeg ) {
        //    Get a list of all selectable sgements, and search for the first visible segment as the rollover target.

        SelectableItemList SelList = pSelect->FindSelectionList( m_cursor_lat, m_cursor_lon,
                                     SELTYPE_ROUTESEGMENT );
        wxSelectableItemListNode *node = SelList.GetFirst();
        while( node ) {
            SelectItem *pFindSel = node->GetData();

            Route *pr = (Route *) pFindSel->m_pData3;        //candidate

            if( pr && pr->IsVisible() ) {
                m_pRolloverRouteSeg = pFindSel;
                showRollover = true;

                if( NULL == m_pRouteRolloverWin ) {
                    m_pRouteRolloverWin = new RolloverWin( this );
                    m_pRouteRolloverWin->IsActive( false );
                }

                if( !m_pRouteRolloverWin->IsActive() ) {
                    wxString s;
                    RoutePoint *segShow_point_a = (RoutePoint *) m_pRolloverRouteSeg->m_pData1;
                    RoutePoint *segShow_point_b = (RoutePoint *) m_pRolloverRouteSeg->m_pData2;

                    double brg, dist;
                    DistanceBearingMercator( segShow_point_b->m_lat, segShow_point_b->m_lon,
                                             segShow_point_a->m_lat, segShow_point_a->m_lon, &brg, &dist );

                    if( !pr->m_bIsInLayer )
                        s.Append( _("Route: ") );
                    else
                        s.Append( _("Layer Route: ") );

                    if( pr->m_RouteNameString.IsEmpty() ) s.Append( _("(unnamed)") );
                    else
                        s.Append( pr->m_RouteNameString );

                    s << _T("\n") << _("Total Length: ") << FormatDistanceAdaptive( pr->m_route_length)
                    << _T("\n") << _("Leg: from ") << segShow_point_a->GetName()
                    << _(" to ") << segShow_point_b->GetName()
                    << _T("\n");

                    if( g_bShowMag )
                        s << wxString::Format( wxString("%03d°(M)  ", wxConvUTF8 ), (int)gFrame->GetTrueOrMag( brg ) );
                    else
                        s << wxString::Format( wxString("%03d°  ", wxConvUTF8 ), (int)gFrame->GetTrueOrMag( brg ) );

                    s << FormatDistanceAdaptive( dist );

                    // Compute and display cumulative distance from route start point to current
                    // leg end point.

                    if( segShow_point_a != pr->pRoutePointList->GetFirst()->GetData() ) {
                        wxRoutePointListNode *node = (pr->pRoutePointList)->GetFirst()->GetNext();
                        RoutePoint *prp;
                        float dist_to_endleg = 0;
                        wxString t;

                        while( node ) {
                            prp = node->GetData();
                            dist_to_endleg += prp->m_seg_len;
                            if( prp->IsSame( segShow_point_a ) ) break;
                            node = node->GetNext();
                        }
                        s << _T(" (+") << FormatDistanceAdaptive( dist_to_endleg ) << _T(")");
                    }

                    m_pRouteRolloverWin->SetString( s );

                    wxSize win_size = GetSize();
                    if( console->IsShown() ) win_size.x -= console->GetSize().x;
                    m_pRouteRolloverWin->SetBestPosition( mouse_x, mouse_y, 16, 16, LEG_ROLLOVER,
                                                     win_size );
                    m_pRouteRolloverWin->SetBitmap( LEG_ROLLOVER );
                    m_pRouteRolloverWin->IsActive( true );
                    b_need_refresh = true;
                    showRollover = true;
                    break;
                }
            } else
                node = node->GetNext();
        }
    } else {
        //    Is the cursor still in select radius?
        if( !pSelect->IsSelectableSegmentSelected( m_cursor_lat, m_cursor_lon,
                m_pRolloverRouteSeg ) ) showRollover = false;
        else
            showRollover = true;
    }

    //    If currently creating a route, do not show this rollover window
    if( parent_frame->nRoute_State )
        showRollover = false;

    //    Similar for AIS target rollover window
    if( m_pAISRolloverWin && m_pAISRolloverWin->IsActive() )
        showRollover = false;

    if( m_pRouteRolloverWin && m_pRouteRolloverWin->IsActive() && !showRollover ) {
        m_pRouteRolloverWin->IsActive( false );
        m_pRolloverRouteSeg = NULL;
        m_pRouteRolloverWin->Destroy();
        m_pRouteRolloverWin = NULL;
        b_need_refresh = true;
    } else if( m_pRouteRolloverWin && showRollover ) {
        m_pRouteRolloverWin->IsActive( true );
        b_need_refresh = true;
    }

    if( b_need_refresh )
        Refresh();
}

void ChartCanvas::OnCursorTrackTimerEvent( wxTimerEvent& event )
{
#ifdef USE_S57
    if( s57_CheckExtendedLightSectors( mouse_x, mouse_y, VPoint, extendedSectorLegs ) ){
        if(!m_bsectors_shown) {
            ReloadVP( false );
            m_bsectors_shown = true;
        }
    }
    else {
        if( m_bsectors_shown ) {
            ReloadVP( false );
            m_bsectors_shown = false;
        }
    }
#endif

//      This is here because GTK status window update is expensive..
//            cairo using pango rebuilds the font every time so is very inefficient
//      Anyway, only update the status bar when this timer expires
#ifdef __WXGTK__
    {
        //    Check the absolute range of the cursor position
        //    There could be a window wherein the chart geoereferencing is not valid....
        double cursor_lat, cursor_lon;
        cc1->GetCanvasPixPoint ( mouse_x, mouse_y, cursor_lat, cursor_lon );

        if((fabs(cursor_lat) < 90.) && (fabs(cursor_lon) < 360.))
        {
            while(cursor_lon < -180.)
                cursor_lon += 360.;

            while(cursor_lon > 180.)
                cursor_lon -= 360.;

            SetCursorStatus(cursor_lat, cursor_lon);
        }
    }
#endif
}

void ChartCanvas::SetCursorStatus( double cursor_lat, double cursor_lon )
{
    if ( !parent_frame->m_pStatusBar )
        return;

    wxString s1;
    s1 += _T(" ");
    s1 += toSDMM(1, cursor_lat);
    s1 += _T("   ");
    s1 += toSDMM(2, cursor_lon);
    parent_frame->SetStatusText ( s1, STAT_FIELD_CURSOR_LL );
    
    double brg, dist;
    wxString s;
    DistanceBearingMercator(cursor_lat, cursor_lon, gLat, gLon, &brg, &dist);
    if( g_bShowMag )
        s.Printf( wxString("%03d°(M)  ", wxConvUTF8 ), (int)gFrame->GetTrueOrMag( brg ) );
    else
        s.Printf( wxString("%03d°  ", wxConvUTF8 ), (int)gFrame->GetTrueOrMag( brg ) );
    
    s << FormatDistanceAdaptive( dist );
    parent_frame->SetStatusText ( s, STAT_FIELD_CURSOR_BRGRNG );
}

void ChartCanvas::GetCursorLatLon( double *lat, double *lon )
{
    double clat, clon;
    GetCanvasPixPoint( mouse_x, mouse_y, clat, clon );
    *lat = clat;
    *lon = clon;
}

void ChartCanvas::GetCanvasPointPix( double rlat, double rlon, wxPoint *r )
{
    // If the Current Chart is a raster chart, and the
    // requested lat/long is within the boundaries of the chart,
    // and the VP is not rotated,
    // then use the embedded BSB chart georeferencing algorithm
    // for greater accuracy
    // Additionally, use chart embedded georef if the projection is TMERC
    //  i.e. NOT MERCATOR and NOT POLYCONIC

    // If for some reason the chart rejects the request by returning an error,
    // then fall back to Viewport Projection estimate from canvas parameters
    bool bUseVP = true;

    if( Current_Ch && ( Current_Ch->GetChartFamily() == CHART_FAMILY_RASTER )
        && ( ( ( fabs( GetVP().rotation ) < .0001 ) &&
               ( ( !g_bskew_comp || ( fabs( GetVP().skew ) < .0001 ) ) ) )
             || ( ( Current_Ch->GetChartProjectionType() != PROJECTION_MERCATOR )
                  && ( Current_Ch->GetChartProjectionType() != PROJECTION_TRANSVERSE_MERCATOR )
                  && ( Current_Ch->GetChartProjectionType() != PROJECTION_POLYCONIC ) ) ) )
    {
        ChartBaseBSB *Cur_BSB_Ch = dynamic_cast<ChartBaseBSB *>( Current_Ch );
//                        bool bInside = G_FloatPtInPolygon ( ( MyFlPoint * ) Cur_BSB_Ch->GetCOVRTableHead ( 0 ),
//                                                            Cur_BSB_Ch->GetCOVRTablenPoints ( 0 ), rlon, rlat );
//                        bInside = true;
//                        if ( bInside )
        if( Cur_BSB_Ch ) {
            //    This is a Raster chart....
            //    If the VP is changing, the raster chart parameters may not yet be setup
            //    So do that before accessing the chart's embedded georeferencing
            Cur_BSB_Ch->SetVPRasterParms( GetVP() );
            int rpixxd, rpixyd;
            if( 0 == Cur_BSB_Ch->latlong_to_pix_vp( rlat, rlon, rpixxd, rpixyd, GetVP() ) ) {
                r->x = rpixxd;
                r->y = rpixyd;
                bUseVP = false;
            }
        }
    }

    //    if needed, use the VPoint scaling estimator,
    if( bUseVP ) {
        wxPoint p = GetVP().GetPixFromLL( rlat, rlon );
        *r = p;
    }

}

void ChartCanvas::GetCanvasPixPoint( int x, int y, double &lat, double &lon )
{
    // If the Current Chart is a raster chart, and the
    // requested x,y is within the boundaries of the chart,
    // and the VP is not rotated,
    // then use the embedded BSB chart georeferencing algorithm
    // for greater accuracy
    // Additionally, use chart embedded georef if the projection is TMERC
    //  i.e. NOT MERCATOR and NOT POLYCONIC

    // If for some reason the chart rejects the request by returning an error,
    // then fall back to Viewport Projection  estimate from canvas parameters
    bool bUseVP = true;

    if( Current_Ch && ( Current_Ch->GetChartFamily() == CHART_FAMILY_RASTER )
        && ( ( ( fabs( GetVP().rotation ) < .0001 ) &&
               ( ( !g_bskew_comp || ( fabs( GetVP().skew ) < .0001 ) ) ) )
             || ( ( Current_Ch->GetChartProjectionType() != PROJECTION_MERCATOR )
                  && ( Current_Ch->GetChartProjectionType() != PROJECTION_TRANSVERSE_MERCATOR )
                  && ( Current_Ch->GetChartProjectionType() != PROJECTION_POLYCONIC ) ) ) )
    {
        ChartBaseBSB *Cur_BSB_Ch = dynamic_cast<ChartBaseBSB *>( Current_Ch );

        // TODO     maybe need iterative process to validate bInside
        //          first pass is mercator, then check chart boundaries

        if( Cur_BSB_Ch ) {
            //    This is a Raster chart....
            //    If the VP is changing, the raster chart parameters may not yet be setup
            //    So do that before accessing the chart's embedded georeferencing
            Cur_BSB_Ch->SetVPRasterParms( GetVP() );

            double slat, slon;
            if( 0 == Cur_BSB_Ch->vp_pix_to_latlong( GetVP(), x, y, &slat, &slon ) ) {
                lat = slat;

                if( slon < -180. ) slon += 360.;
                else if( slon > 180. ) slon -= 360.;

                lon = slon;
                bUseVP = false;
            }
        }

    }

    //    if needed, use the VPoint scaling estimator
    if( bUseVP ) {
        GetVP().GetLLFromPix( wxPoint( x, y ), &lat, &lon );
    }
}

void ChartCanvas::ZoomCanvas( double factor, bool can_zoom_to_cursor, bool stoptimer )
{
    m_bzooming_to_cursor = can_zoom_to_cursor && g_bEnableZoomToCursor;

    if( g_bsmoothpanzoom ) {
        if(StartTimedMovement(stoptimer)) {
            m_mustmove += 150; /* for quick presses register as 200 ms duration */
            m_zoom_factor = factor;
//            m_zoom_target =  VPoint.chart_scale / factor;
        }
        m_zoom_target =  VPoint.chart_scale / factor;
    } else {
#ifdef __WXOSX__
        if( m_modkeys == wxMOD_CONTROL )
#else
        if( m_modkeys == wxMOD_ALT )
#endif
            factor = pow(factor, .15);
        
        DoZoomCanvas( factor );
    }

    extendedSectorLegs.clear();
}

void ChartCanvas::DoZoomCanvas( double factor,  bool can_zoom_to_cursor )
{
    /* TODO: queue the quilted loading code to a background thread
       so yield is never called from here, and also rendering is not delayed */

    //    Cannot allow Yield() re-entrancy here
    if( m_bzooming ) return;
    m_bzooming = true;

    //  Capture current cursor position for zoom to cursor
    double zlat = m_cursor_lat;
    double zlon = m_cursor_lon;

    double proposed_scale_onscreen = GetCanvasScaleFactor() / ( GetVPScale() * factor );
    bool b_do_zoom = false;
    
    if(factor > 1)
    {
        b_do_zoom = true;

        double zoom_factor = factor;

        double min_allowed_scale = 500.0;                // meters per meter
        
        ChartBase *pc = NULL;

        if( !VPoint.b_quilt ) {
            pc = Current_Ch;
        } else {
            int new_db_index = m_pQuilt->AdjustRefOnZoomIn( proposed_scale_onscreen );
            if( new_db_index >= 0 )
                pc = ChartData->OpenChartFromDB( new_db_index, FULL_INIT );

            if(pCurrentStack)
                pCurrentStack->SetCurrentEntryFromdbIndex( new_db_index ); // highlite the correct bar entry
        }

        if( pc ) {
            min_allowed_scale = pc->GetNormalScaleMin( GetCanvasScaleFactor(), false/*g_b_overzoom_x*/ );
            
            double target_scale_ppm = GetVPScale() * zoom_factor;
            double new_scale_ppm = target_scale_ppm; //pc->GetNearestPreferredScalePPM(target_scale_ppm);
            
            proposed_scale_onscreen = GetCanvasScaleFactor() / new_scale_ppm;
            
            //  Query the chart to determine the appropriate zoom range
            if( proposed_scale_onscreen < min_allowed_scale ) {
                if( min_allowed_scale == GetCanvasScaleFactor() / ( GetVPScale() ) ) {
                    m_zoom_factor = 1; /* stop zooming */
                    b_do_zoom = false;
                } else
                    proposed_scale_onscreen = min_allowed_scale;
            }
            
            m_last_max_scale = min_allowed_scale;
        }
        else {
            proposed_scale_onscreen = wxMax( proposed_scale_onscreen, m_last_max_scale);
            
        }
            
        
    } else if(factor < 1) {
        double zoom_factor = 1/factor;

        b_do_zoom = true;

        ChartBase *pc = NULL;

        bool b_smallest = false;

        if( !VPoint.b_quilt ) {             // not quilted
            pc = Current_Ch;

            if( ChartData && pc ) {
                //      If Current_Ch is not on the screen, unbound the zoomout
                LLBBox viewbox = VPoint.GetBBox();
                wxBoundingBox chart_box;
                int current_index = ChartData->FinddbIndex( pc->GetFullPath() );
                double max_allowed_scale;

                max_allowed_scale = GetCanvasScaleFactor() / m_absolute_min_scale_ppm;

                //  We can allow essentially unbounded zoomout in single chart mode
//                if( ChartData->GetDBBoundingBox( current_index, &chart_box ) &&
//                    !viewbox.IntersectOut( chart_box ) )
//                    //  Clamp the minimum scale zoom-out to the value specified by the chart
//                    max_allowed_scale = wxMin(max_allowed_scale, 4.0 *
//                                              pc->GetNormalScaleMax( GetCanvasScaleFactor(),
//                                                                     GetCanvasWidth() ) );
                if(proposed_scale_onscreen > max_allowed_scale) {
                    m_zoom_factor = 1; /* stop zooming */
                    proposed_scale_onscreen = max_allowed_scale;
                }
            }

        } else {
            int new_db_index = m_pQuilt->AdjustRefOnZoomOut( proposed_scale_onscreen );
            if( new_db_index >= 0 ) pc = ChartData->OpenChartFromDB( new_db_index, FULL_INIT );

            if(pCurrentStack)
                pCurrentStack->SetCurrentEntryFromdbIndex( new_db_index ); // highlite the correct bar entry
            
            b_smallest = m_pQuilt->IsChartSmallestScale( new_db_index );

            if( ( !g_bPreserveScaleOnX ) &&  (b_smallest || (0 == m_pQuilt->GetExtendedStackCount())))
                proposed_scale_onscreen = wxMin(proposed_scale_onscreen,
                                                GetCanvasScaleFactor() / m_absolute_min_scale_ppm);
        }

        //set a minimum scale
        if( ( GetCanvasScaleFactor() / proposed_scale_onscreen ) < m_absolute_min_scale_ppm )
            b_do_zoom = false;
    }

    if( b_do_zoom ) {
        if( can_zoom_to_cursor && g_bEnableZoomToCursor) {
            //  Arrange to combine the zoom and pan into one operation for smoother appearance
            SetVPScale( GetCanvasScaleFactor() / proposed_scale_onscreen, false );   // adjust, but deferred refresh
            wxPoint r;
            GetCanvasPointPix( zlat, zlon, &r );
            PanCanvas( r.x - mouse_x, r.y - mouse_y );  // this will give the Refresh()
            ClearbFollow();      // update the follow flag
        }
        else
            SetVPScale( GetCanvasScaleFactor() / proposed_scale_onscreen );
        
    }
    
    m_bzooming = false;
    
}

void ChartCanvas::RotateCanvas( double dir )
{
    if(g_bCourseUp)
        g_bCourseUp = false;

    if(g_bsmoothpanzoom) {
        if(StartTimedMovement()) {
            m_mustmove += 150; /* for quick presses register as 200 ms duration */
            m_rotation_speed = dir*60;
        }
    } else {
        double speed = dir*10;
        if( m_modkeys == wxMOD_ALT)
            speed /= 20;
        DoRotateCanvas(VPoint.rotation + PI/180 * speed);
    }
}

void ChartCanvas::DoRotateCanvas( double rotation )
{
    while(rotation < 0) rotation += 2*PI;
    while(rotation > 2*PI) rotation -= 2*PI;

    SetVPRotation( rotation );
    ReloadVP();
    parent_frame->UpdateGPSCompassStatusBox( false );
}

void ChartCanvas::ClearbFollow( void )
{
    m_bFollow = false;      // update the follow flag
    parent_frame->SetToolbarItemState( ID_FOLLOW, false );
}

bool ChartCanvas::PanCanvas( int dx, int dy )
{
    double dlat, dlon;
    wxPoint p;
//      CALLGRIND_START_INSTRUMENTATION

    extendedSectorLegs.clear();

    GetCanvasPointPix( GetVP().clat, GetVP().clon, &p );
    GetCanvasPixPoint( p.x + dx, p.y + dy, dlat, dlon );

    if( dlon > 360. ) dlon -= 360.;
    if( dlon < -360. ) dlon += 360.;

    //    This should not really be necessary, but round-trip georef on some charts is not perfect,
    //    So we can get creep on repeated unidimensional pans, and corrupt chart cacheing.......

    //    But this only works on north-up projections
    if( ( ( fabs( GetVP().skew ) < .001 ) ) && ( fabs( GetVP().rotation ) < .001 ) ) {

        if( dx == 0 ) dlon = GetVP().clon;
        if( dy == 0 ) dlat = GetVP().clat;
    }

    int cur_ref_dbIndex = m_pQuilt->GetRefChartdbIndex();
    SetViewPoint( dlat, dlon, VPoint.view_scale_ppm, VPoint.skew, VPoint.rotation );

//      vLat = dlat;
//      vLon = dlon;

    if( VPoint.b_quilt ) {
        int new_ref_dbIndex = m_pQuilt->GetRefChartdbIndex();
        if( ( new_ref_dbIndex != cur_ref_dbIndex ) && ( new_ref_dbIndex != -1 ) ) {
            //Tweak the scale slightly for a new ref chart
            ChartBase *pc = ChartData->OpenChartFromDB( new_ref_dbIndex, FULL_INIT );
            if( pc ) {
                double tweak_scale_ppm = pc->GetNearestPreferredScalePPM( VPoint.view_scale_ppm );
                SetVPScale( tweak_scale_ppm );
            }
        }
    }

    ClearbFollow();      // update the follow flag

    Refresh( false );

    pCurTrackTimer->Start( m_curtrack_timer_msec, wxTIMER_ONE_SHOT );

    return true;
}

void ChartCanvas::ReloadVP( bool b_adjust )
{
    if( g_brightness_init ) SetScreenBrightness( g_nbrightness );

    LoadVP( VPoint, b_adjust );
}

void ChartCanvas::LoadVP( ViewPort &vp, bool b_adjust )
{
#ifdef ocpnUSE_GL
    if( g_bopengl ) {
        glChartCanvas::Invalidate();
#ifdef __WXOSX__
      if (m_glcc != NULL)
#endif
        if( m_glcc->GetSize().x != VPoint.pix_width || m_glcc->GetSize().y != VPoint.pix_height ) m_glcc->SetSize(
                VPoint.pix_width, VPoint.pix_height );
    }
    else
#endif
    {
        m_cache_vp.Invalidate();
        m_bm_cache_vp.Invalidate();
    }

    VPoint.Invalidate();

    if( m_pQuilt ) m_pQuilt->Invalidate();

    SetViewPoint( vp.clat, vp.clon, vp.view_scale_ppm, vp.skew, vp.rotation, b_adjust );

}

void ChartCanvas::SetQuiltRefChart( int dbIndex )
{
    m_pQuilt->SetReferenceChart( dbIndex );
    VPoint.Invalidate();
    m_pQuilt->Invalidate();
}

double ChartCanvas::GetBestStartScale(int dbi_hint, const ViewPort &vp)
{
    if(m_pQuilt)
        return m_pQuilt->GetBestStartScale(dbi_hint, vp);
    else
        return vp.view_scale_ppm;
}

//      Verify and adjust the current reference chart,
//      so that it will not lead to excessive overzoom or underzoom onscreen
int ChartCanvas::AdjustQuiltRefChart( void )
{
    int ret = -1;
    if(m_pQuilt){
        ChartBase *pc = ChartData->OpenChartFromDB( m_pQuilt->GetRefChartdbIndex(), FULL_INIT );
        if( pc ) {
            double min_ref_scale = pc->GetNormalScaleMin( m_canvas_scale_factor, false );
            double max_ref_scale = pc->GetNormalScaleMax( m_canvas_scale_factor, m_canvas_width );
            
            if( VPoint.chart_scale < min_ref_scale )  {
                ret = m_pQuilt->AdjustRefOnZoomIn( VPoint.chart_scale );
            }
            else if( VPoint.chart_scale > max_ref_scale )  {
                ret = m_pQuilt->AdjustRefOnZoomOut( VPoint.chart_scale );
            }
            else {
                bool brender_ok = IsChartLargeEnoughToRender( pc, VPoint );
                
                int ref_family = pc->GetChartFamily();
                
                if( !brender_ok ) {
                    unsigned int target_stack_index = 0;
                    int target_stack_index_check = m_pQuilt->GetExtendedStackIndexArray().Index(  m_pQuilt->GetRefChartdbIndex() ); // Lookup
                    
                    if( wxNOT_FOUND != target_stack_index_check )
                        target_stack_index = target_stack_index_check;
                    
                    int extended_array_count = m_pQuilt->GetExtendedStackIndexArray().GetCount();
                    while( ( !brender_ok )  && ( (int)target_stack_index < ( extended_array_count - 1 ) ) ) {
                        target_stack_index++;
                        int test_db_index = m_pQuilt->GetExtendedStackIndexArray().Item( target_stack_index );
                    
                        if( ( ref_family == ChartData->GetDBChartFamily( test_db_index ) )
                            && IsChartQuiltableRef( test_db_index ) ) {
                        //    open the target, and check the min_scale
                            ChartBase *ptest_chart = ChartData->OpenChartFromDB( test_db_index, FULL_INIT );
                            if( ptest_chart ){
                                brender_ok = IsChartLargeEnoughToRender( ptest_chart, VPoint );
                            }
                        }
                    }
                    
                    if(brender_ok){             // found a better reference chart
                        int new_db_index = m_pQuilt->GetExtendedStackIndexArray().Item( target_stack_index );
                        if( ( ref_family == ChartData->GetDBChartFamily( new_db_index ) )
                            && IsChartQuiltableRef( new_db_index ) ) {
                            m_pQuilt->SetReferenceChart( new_db_index );
                            ret = new_db_index;
                        }
                        else
                            ret =m_pQuilt->GetRefChartdbIndex();
                    }
                    else
                        ret = m_pQuilt->GetRefChartdbIndex();
                    
                }
                else
                    ret = m_pQuilt->GetRefChartdbIndex();
            }
        }
        else
            ret = -1;
    }
    
    return ret;
}


void ChartCanvas::UpdateCanvasOnGroupChange( void )
{
    delete pCurrentStack;
    pCurrentStack = NULL;
    pCurrentStack = new ChartStack;
    ChartData->BuildChartStack( pCurrentStack, VPoint.clat, VPoint.clon );

    if( m_pQuilt ) {
        m_pQuilt->Compose( VPoint );
    }
}

bool ChartCanvas::SetVPScale( double scale, bool refresh )
{
    return SetViewPoint( VPoint.clat, VPoint.clon, scale, VPoint.skew, VPoint.rotation, true, refresh );
}

bool ChartCanvas::SetViewPoint( double lat, double lon )
{
    return SetViewPoint( lat, lon, VPoint.view_scale_ppm, VPoint.skew, VPoint.rotation );
}

bool ChartCanvas::SetViewPoint( double lat, double lon, double scale_ppm, double skew,
                                double rotation, bool b_adjust, bool b_refresh )
{
    bool b_ret = false;

    if(skew > PI) /* so our difference tests work, put in range of +-Pi */
        skew -= 2*PI;

    //  Any sensible change?
    if( ( fabs( VPoint.view_scale_ppm - scale_ppm ) < 1e-9 )
            && ( fabs( VPoint.skew - skew ) < 1e-9 )
            && ( fabs( VPoint.rotation - rotation ) < 1e-9 ) && ( fabs( VPoint.clat - lat ) < 1e-9 )
            && ( fabs( VPoint.clon - lon ) < 1e-9 ) && VPoint.IsValid() ) return false;

    VPoint.SetProjectionType( PROJECTION_MERCATOR );            // default

    VPoint.Validate();                     // Mark this ViewPoint as OK

    //    Take a local copy of the last viewport
    ViewPort last_vp = VPoint;

    VPoint.skew = skew;
    VPoint.clat = lat;
    VPoint.clon = lon;
    VPoint.view_scale_ppm = scale_ppm;
    SetVPRotation( rotation );

    if( ( VPoint.pix_width <= 0 ) || ( VPoint.pix_height <= 0 ) )    // Canvas parameters not yet set
        return false;

    //  Has the Viewport scale changed?  If so, invalidate the vp
    if( last_vp.view_scale_ppm != scale_ppm ) {
        m_cache_vp.Invalidate();

#ifdef ocpnUSE_GL
        if( g_bopengl )
            glChartCanvas::Invalidate();
#endif        
    }

    //  A preliminary value, may be tweaked below
    VPoint.chart_scale = m_canvas_scale_factor / ( scale_ppm );

    // recompute cursor position
    GetCanvasPixPoint( mouse_x, mouse_y, m_cursor_lat, m_cursor_lon );
    if(g_pi_manager) g_pi_manager->SendCursorLatLonToAllPlugIns( m_cursor_lat, m_cursor_lon );

    if( !VPoint.b_quilt && Current_Ch ) {

        VPoint.SetProjectionType( Current_Ch->GetChartProjectionType() );
        VPoint.SetBoxes();

        //  Allow the chart to adjust the new ViewPort for performance optimization
        //  This will normally be only a fractional (i.e.sub-pixel) adjustment...
        if( b_adjust ) Current_Ch->AdjustVP( last_vp, VPoint );

        // If there is a sensible change in the chart render, refresh the whole screen
        if( ( !m_cache_vp.IsValid() ) || ( m_cache_vp.view_scale_ppm != VPoint.view_scale_ppm ) ) {
            Refresh( false );
            b_ret = true;
        } else {
            wxPoint cp_last, cp_this;
            GetCanvasPointPix( m_cache_vp.clat, m_cache_vp.clon, &cp_last );
            GetCanvasPointPix( VPoint.clat, VPoint.clon, &cp_this );

            if( cp_last != cp_this ) {
                Refresh( false );
                b_ret = true;
            }
        }
    }

    //  Handle the quilted case
    if( VPoint.b_quilt ) {

        if( last_vp.view_scale_ppm != scale_ppm ) m_pQuilt->InvalidateAllQuiltPatchs();

        //  Create the quilt
        if( ChartData && ChartData->IsValid() ) {
            if( !pCurrentStack ) return false;

            int current_db_index = -1;
            current_db_index = pCurrentStack->GetCurrentEntrydbIndex();       // capture the current

            ChartData->BuildChartStack( pCurrentStack, lat, lon );
            pCurrentStack->SetCurrentEntryFromdbIndex( current_db_index );

            //   Check to see if the current quilt reference chart is in the new stack
            int current_ref_stack_index = -1;
            for( int i = 0; i < pCurrentStack->nEntry; i++ ) {
                if( m_pQuilt->GetRefChartdbIndex() == pCurrentStack->GetDBIndex( i ) ) current_ref_stack_index =
                        i;
            }

            if( g_bFullScreenQuilt ) {
                current_ref_stack_index = m_pQuilt->GetRefChartdbIndex();
            }

            //    If the new stack does not contain the current ref chart....
            if( ( -1 == current_ref_stack_index ) && ( m_pQuilt->GetRefChartdbIndex() >= 0 ) ) {
                const ChartTableEntry &cte_ref = ChartData->GetChartTableEntry(
                                                     m_pQuilt->GetRefChartdbIndex() );
                int target_scale = cte_ref.GetScale();
                int target_type = cte_ref.GetChartType();
                int candidate_stack_index;

                //    reset the ref chart in a way that does not lead to excessive underzoom, for performance reasons
                //    Try to find a chart that is the same type, and has a scale of just smaller than the current ref chart

                candidate_stack_index = 0;
                while( candidate_stack_index <= pCurrentStack->nEntry - 1 ) {
                    const ChartTableEntry &cte_candidate = ChartData->GetChartTableEntry(
                            pCurrentStack->GetDBIndex( candidate_stack_index ) );
                    int candidate_scale = cte_candidate.GetScale();
                    int candidate_type = cte_candidate.GetChartType();

                    if( ( candidate_scale >= target_scale ) && ( candidate_type == target_type ) ) break;

                    candidate_stack_index++;
                }

                //    If that did not work, look for a chart of just larger scale and same type
                if( candidate_stack_index >= pCurrentStack->nEntry ) {
                    candidate_stack_index = pCurrentStack->nEntry - 1;
                    while( candidate_stack_index >= 0 ) {
                        const ChartTableEntry &cte_candidate = ChartData->GetChartTableEntry(
                                pCurrentStack->GetDBIndex( candidate_stack_index ) );
                        int candidate_scale = cte_candidate.GetScale();
                        int candidate_type = cte_candidate.GetChartType();

                        if( ( candidate_scale <= target_scale )
                                && ( candidate_type == target_type ) ) break;

                        candidate_stack_index--;
                    }
                }

                // and if that did not work, chose stack entry 0
                if( ( candidate_stack_index >= pCurrentStack->nEntry )
                        || ( candidate_stack_index < 0 ) ) candidate_stack_index = 0;

                int new_ref_index = pCurrentStack->GetDBIndex( candidate_stack_index );

                m_pQuilt->SetReferenceChart( new_ref_index ); //maybe???

            }


            // Preset the VPoint projection type to match what the quilt projection type will be
            int ref_db_index = m_pQuilt->GetRefChartdbIndex();
            int proj = ChartData->GetDBChartProj( ref_db_index );

            // Always keep the default Mercator projection if the reference chart is
            // not in the PatchList or the scale is too small for it to render.

            bool renderable = true;
            ChartBase* referenceChart = ChartData->OpenChartFromDB( ref_db_index, FULL_INIT );
            if( referenceChart ) {
                double chartMaxScale = referenceChart->GetNormalScaleMax( cc1->GetCanvasScaleFactor(), cc1->GetCanvasWidth() );
                renderable = chartMaxScale*1.5 > VPoint.chart_scale;
            }

            VPoint.b_MercatorProjectionOverride = ( m_pQuilt->GetnCharts() == 0 || !renderable );

            if( ! VPoint.b_MercatorProjectionOverride ) VPoint.SetProjectionType( proj );

            VPoint.SetBoxes();

            //    If this quilt will be a perceptible delta from the existing quilt, then refresh the entire screen
            if( m_pQuilt->IsQuiltDelta( VPoint ) ) {
                //  Allow the quilt to adjust the new ViewPort for performance optimization
                //  This will normally be only a fractional (i.e. sub-pixel) adjustment...
                if( b_adjust ) m_pQuilt->AdjustQuiltVP( last_vp, VPoint );

                ChartData->ClearCacheInUseFlags();
                unsigned long hash1 = m_pQuilt->GetXStackHash();
                m_pQuilt->Compose( VPoint );

                //      If the extended chart stack has changed, invalidate any cached render bitmap
                if(m_pQuilt->GetXStackHash() != hash1) {
//                    m_bm_cache_vp.Invalidate();
//                    InvalidateGL();
                }

                ChartData->PurgeCacheUnusedCharts( 0.7 );

                if(b_refresh)
                    Refresh( false );
                
                b_ret = true;
            }
            parent_frame->UpdateControlBar();
        }

        VPoint.skew = 0.;  // Quilting supports 0 Skew
    }

    if( !VPoint.GetBBox().GetValid() ) VPoint.SetBoxes();

    if( VPoint.GetBBox().GetValid() ) {

        //    Calculate the on-screen displayed actual scale
        //    by a simple traverse northward from the center point
        //    of roughly 10 % of the Viewport extent
        double tlat, tlon;
        wxPoint r, r1;
        double delta_y = ( VPoint.GetBBox().GetMaxY() - VPoint.GetBBox().GetMinY() ) * 60.0 * .10; // roughly 10 % of lat range, in NM

        //  Make sure the two points are in phase longitudinally
        double lon_norm = VPoint.clon;
        if( lon_norm > 180. ) lon_norm -= 360;
        else if( lon_norm < -180. ) lon_norm += 360.;

        ll_gc_ll( VPoint.clat, lon_norm, 0, delta_y, &tlat, &tlon );

        GetCanvasPointPix( tlat, tlon, &r1 );
        GetCanvasPointPix( VPoint.clat, lon_norm, &r );

        m_true_scale_ppm = sqrt(
                               pow( (double) ( r.y - r1.y ), 2 ) + pow( (double) ( r.x - r1.x ), 2 ) )
                           / ( delta_y * 1852. );

        //        A fall back in case of very high zoom-out, giving delta_y == 0
        //        which can probably only happen with vector charts
        if( 0.0 == m_true_scale_ppm ) m_true_scale_ppm = scale_ppm;

        //        Another fallback, for highly zoomed out charts
        //        This adjustment makes the displayed TrueScale correspond to the
        //        same algorithm used to calculate the chart zoom-out limit for ChartDummy.
        if( scale_ppm < 1e-4 ) m_true_scale_ppm = scale_ppm;

        if( m_true_scale_ppm ) VPoint.chart_scale = m_canvas_scale_factor / ( m_true_scale_ppm );
        else
            VPoint.chart_scale = 1.0;

        if( parent_frame->m_pStatusBar ) {
            double true_scale_display = floor( VPoint.chart_scale / 100. ) * 100.;
            wxString text;

            if( Current_Ch ) {
                double chart_native_ppm = m_canvas_scale_factor / Current_Ch->GetNativeScale();
                double scale_factor = scale_ppm / chart_native_ppm;
                if( scale_factor > 1.0 ) text.Printf( _("Scale %4.0f (%1.1fx)"),
                                                          true_scale_display, scale_factor );
                else
                    text.Printf( _("Scale %4.0f (%1.2fx)"), true_scale_display,
                                 scale_factor );
            } else
                text.Printf( _("Scale %4.0f"), true_scale_display );

            parent_frame->SetStatusText( text, STAT_FIELD_SCALE );
        }
    }

    //  Maintain global vLat/vLon
    vLat = VPoint.clat;
    vLon = VPoint.clon;

    return b_ret;
}

//          Static Icon definitions for some symbols requiring scaling/rotation/translation
//          Very specific wxDC draw commands are necessary to properly render these icons...See the code in ShipDraw()

//      This icon was adapted and scaled from the S52 Presentation Library version 3_03.
//     Symbol VECGND02

static int s_png_pred_icon[] = { -10, -10, -10, 10, 10, 10, 10, -10 };

//      This ownship icon was adapted and scaled from the S52 Presentation Library version 3_03
//      Symbol OWNSHP05
static int s_ownship_icon[] = { 5, -42, 11, -28, 11, 42, -11, 42, -11, -28, -5, -42, -11, 0, 11, 0,
                                0, 42, 0, -42
                              };

wxColour ChartCanvas::PredColor()
{ 
    if( SHIP_NORMAL == m_ownship_state )
        return GetGlobalColor( _T ( "URED" ) );
    return GetGlobalColor( _T ( "GREY1" ) );
}

wxColour ChartCanvas::ShipColor()
{ 
    //      Establish ship color
    //     It changes color based on GPS and Chart accuracy/availability

    if( SHIP_NORMAL != m_ownship_state )
        return GetGlobalColor( _T ( "GREY1" ) );

    if( SHIP_LOWACCURACY == m_ownship_state )
        return GetGlobalColor( _T ( "YELO1" ) );

    return GetGlobalColor( _T ( "URED" ) );         // default is OK
}

void ChartCanvas::ShipDrawLargeScale( ocpnDC& dc, wxPoint lShipMidPoint )
{

    dc.SetPen( wxPen( PredColor(), 2 ) );

    if( SHIP_NORMAL == m_ownship_state )
        dc.SetBrush( wxBrush( ShipColor(), wxTRANSPARENT ) );
    else
        dc.SetBrush( wxBrush( GetGlobalColor( _T ( "YELO1" ) ) ) );
 
    dc.DrawEllipse( lShipMidPoint.x - 10, lShipMidPoint.y - 10, 20, 20 );
    dc.DrawEllipse( lShipMidPoint.x - 6, lShipMidPoint.y - 6, 12, 12 );

    dc.DrawLine( lShipMidPoint.x - 12, lShipMidPoint.y, lShipMidPoint.x + 12, lShipMidPoint.y );
    dc.DrawLine( lShipMidPoint.x, lShipMidPoint.y - 12, lShipMidPoint.x, lShipMidPoint.y + 12 );
}

void ChartCanvas::ShipIndicatorsDraw( ocpnDC& dc, float lpp,
                                      wxPoint GPSOffsetPixels,
                                      wxPoint lGPSPoint, wxPoint lHeadPoint,
                                      float img_height, float cog_rad,
                                      wxPoint lPredPoint, bool b_render_hdt,
                                      wxPoint lShipMidPoint)
{
    // draw course over ground if they are longer than the ship
    if( !wxIsNaN(gCog) && !wxIsNaN(gSog) ) {
        if( lpp >= img_height / 2 ) {
            const double png_pred_icon_scale_factor = .4;
            wxPoint icon[4];

            for( int i = 0; i < 4; i++ ) {
                int j = i * 2;
                double pxa = (double) ( s_png_pred_icon[j] );
                double pya = (double) ( s_png_pred_icon[j + 1] );

                pya *= png_pred_icon_scale_factor;
                pxa *= png_pred_icon_scale_factor;

                double px = ( pxa * sin( cog_rad ) ) + ( pya * cos( cog_rad ) );
                double py = ( pya * sin( cog_rad ) ) - ( pxa * cos( cog_rad ) );

                icon[i].x = (int) wxRound( px ) + lPredPoint.x + GPSOffsetPixels.x;
                icon[i].y = (int) wxRound( py ) + lPredPoint.y + GPSOffsetPixels.y;
            }

            //      COG Predictor
            wxDash dash_long[2];
            dash_long[0] = (int) ( 3.0 * m_pix_per_mm );  //8// Long dash  <---------+
            dash_long[1] = (int) ( 1.5 * m_pix_per_mm );  //2// Short gap            |

            wxPen ppPen2( PredColor(), g_cog_predictor_width, wxUSER_DASH );
            ppPen2.SetDashes( 2, dash_long );
            dc.SetPen( ppPen2 );
            dc.StrokeLine( lGPSPoint.x + GPSOffsetPixels.x, lGPSPoint.y + GPSOffsetPixels.y,
                           lPredPoint.x + GPSOffsetPixels.x, lPredPoint.y + GPSOffsetPixels.y );

            wxDash dash_long3[2];
            dash_long3[0] = g_cog_predictor_width * dash_long[0];
            dash_long3[1] = g_cog_predictor_width * dash_long[1];

            if( g_cog_predictor_width > 1 ) {
                wxPen ppPen3( GetGlobalColor( _T ( "UBLCK" ) ), 1, wxUSER_DASH );
                ppPen3.SetDashes( 2, dash_long3 );
                dc.SetPen( ppPen3 );
                dc.StrokeLine( lGPSPoint.x + GPSOffsetPixels.x, lGPSPoint.y + GPSOffsetPixels.y,
                               lPredPoint.x + GPSOffsetPixels.x, lPredPoint.y + GPSOffsetPixels.y );
            }
            wxPen ppPen1( GetGlobalColor( _T ( "UBLCK" ) ), 1, wxSOLID );
            dc.SetPen( ppPen1 );
            dc.SetBrush( wxBrush( PredColor() ) ); //*wxWHITE_BRUSH);

            dc.StrokePolygon( 4, icon );
        }
    }

    //      HDT Predictor
    if( b_render_hdt ) {
        wxDash dash_short[2];
        dash_short[0] = (int) ( 1.5 * m_pix_per_mm );  // Short dash  <---------+
        dash_short[1] = (int) ( 1.8 * m_pix_per_mm );  // Short gap            |

        wxPen ppPen2( PredColor(), 2, wxUSER_DASH );
        ppPen2.SetDashes( 2, dash_short );

        dc.SetPen( ppPen2 );
        dc.StrokeLine( lGPSPoint.x + GPSOffsetPixels.x, lGPSPoint.y + GPSOffsetPixels.y,
                       lHeadPoint.x + GPSOffsetPixels.x, lHeadPoint.y + GPSOffsetPixels.y );

        wxPen ppPen1( PredColor(), 2, wxSOLID );
        dc.SetPen( ppPen1 );
        dc.SetBrush( wxBrush( GetGlobalColor( _T ( "GREY2" ) ) ) );

        dc.StrokeCircle( lHeadPoint.x + GPSOffsetPixels.x, lHeadPoint.y + GPSOffsetPixels.y, 4 );
    }

    // Draw radar rings if activated
    if( g_iNavAidRadarRingsNumberVisible ) {
        double factor = 1.00;
        if( g_pNavAidRadarRingsStepUnits == 1 )          // nautical miles
            factor = 1 / 1.852;

        factor *= g_fNavAidRadarRingsStep;

        double tlat, tlon;
        wxPoint r;
        ll_gc_ll( gLat, gLon, 0, factor, &tlat, &tlon );
        GetCanvasPointPix( tlat, tlon, &r );

        double lpp = sqrt( pow( (double) (lGPSPoint.x - r.x), 2) +
                           pow( (double) (lGPSPoint.y - r.y), 2 ) );
        int pix_radius = (int) lpp;

        wxPen ppPen1( GetGlobalColor( _T ( "URED" ) ), 2 );
        dc.SetPen( ppPen1 );
        dc.SetBrush( wxBrush( GetGlobalColor( _T ( "URED" ) ), wxTRANSPARENT ) );

        for( int i = 1; i <= g_iNavAidRadarRingsNumberVisible; i++ )
            dc.StrokeCircle( lGPSPoint.x, lGPSPoint.y, i * pix_radius );
    }
}

void ChartCanvas::ComputeShipScaleFactor(float icon_hdt,
                                         int ownShipWidth, int ownShipLength, 
                                         wxPoint &lShipMidPoint,
                                         wxPoint &GPSOffsetPixels, wxPoint lGPSPoint,
                                         float &scale_factor_x, float &scale_factor_y)
{
    float screenResolution = (float) ::wxGetDisplaySize().y / ::wxGetDisplaySizeMM().y;

    //  Calculate the true ship length in exact pixels
    double ship_bow_lat, ship_bow_lon;
    ll_gc_ll( gLat, gLon, icon_hdt, g_n_ownship_length_meters / 1852., &ship_bow_lat,
              &ship_bow_lon );
    wxPoint lShipBowPoint;
    wxPoint2DDouble b_point = GetVP().GetDoublePixFromLL( ship_bow_lat, ship_bow_lon );
    wxPoint2DDouble a_point = GetVP().GetDoublePixFromLL( gLat, gLon );
    
    float shipLength_px = sqrtf( powf( (float) (b_point.m_x - a_point.m_x), 2) +
                                 powf( (float) (b_point.m_y - a_point.m_y), 2) );
    
    //  And in mm
    float shipLength_mm = shipLength_px / screenResolution;
    
    //  Set minimum ownship drawing size
    float ownship_min_mm = g_n_ownship_min_mm;
    ownship_min_mm = wxMax(ownship_min_mm, 1.0);
    
    //  Calculate Nautical Miles distance from midships to gps antenna
    float hdt_ant = icon_hdt + 180.;
    float dy = ( g_n_ownship_length_meters / 2 - g_n_gps_antenna_offset_y ) / 1852.;
    float dx = g_n_gps_antenna_offset_x / 1852.;
    if( g_n_gps_antenna_offset_y > g_n_ownship_length_meters / 2 )      //reverse?
    {
        hdt_ant = icon_hdt;
        dy = -dy;
    }
    
    //  If the drawn ship size is going to be clamped, adjust the gps antenna offsets
    if( shipLength_mm < ownship_min_mm ) {
        dy /= shipLength_mm / ownship_min_mm;
        dx /= shipLength_mm / ownship_min_mm;
    }

    double ship_mid_lat, ship_mid_lon, ship_mid_lat1, ship_mid_lon1;
    
    ll_gc_ll( gLat, gLon, hdt_ant, dy, &ship_mid_lat, &ship_mid_lon );
    ll_gc_ll( ship_mid_lat, ship_mid_lon, icon_hdt - 90., dx, &ship_mid_lat1, &ship_mid_lon1 );

    GetCanvasPointPix( ship_mid_lat1, ship_mid_lon1, &lShipMidPoint );
    GPSOffsetPixels.x = lShipMidPoint.x - lGPSPoint.x;
    GPSOffsetPixels.y = lShipMidPoint.y - lGPSPoint.y;
    
    float scale_factor = shipLength_px / ownShipLength;
    
    //  Calculate a scale factor that would produce a reasonably sized icon
    float scale_factor_min = ownship_min_mm / ( ownShipLength / screenResolution );
    
    //  And choose the correct one
    scale_factor = wxMax(scale_factor, scale_factor_min);
    
    scale_factor_y = scale_factor;
    scale_factor_x = scale_factor_y * ( (float) ownShipLength / ownShipWidth )
        / ( (float) g_n_ownship_length_meters / g_n_ownship_beam_meters );
}

void ChartCanvas::ShipDraw( ocpnDC& dc )
{
    if( !GetVP().IsValid() ) return;

    int drawit = 0;
    wxPoint lGPSPoint, lShipMidPoint, lPredPoint, lHeadPoint, GPSOffsetPixels(0,0);

//    Is ship in Vpoint?

    if( GetVP().GetBBox().PointInBox( gLon, gLat, 0 ) ) drawit++;                             // yep

//    Calculate ownship Position Predictor

    double pred_lat, pred_lon;

    //  COG/SOG may be undefined in NMEA data stream
    float pCog = gCog;
    if( wxIsNaN(pCog) )
        pCog = 0.0;
    float pSog = gSog;
    if( wxIsNaN(pSog) )
        pSog = 0.0;

    ll_gc_ll( gLat, gLon, pCog, pSog * g_ownship_predictor_minutes / 60., &pred_lat, &pred_lon );

    GetCanvasPointPix( gLat, gLon, &lGPSPoint );
    lShipMidPoint = lGPSPoint;
    GetCanvasPointPix( pred_lat, pred_lon, &lPredPoint );

    float cog_rad = atan2f( (float) ( lPredPoint.y - lShipMidPoint.y ),
                            (float) ( lPredPoint.x - lShipMidPoint.x ) );
    cog_rad += (float)PI;

    float lpp = sqrtf( powf( (float) (lPredPoint.x - lShipMidPoint.x), 2) +
                       powf( (float) (lPredPoint.y - lShipMidPoint.y), 2) );

//    Is predicted point in the VPoint?
    if( GetVP().GetBBox().PointInBox( pred_lon, pred_lat, 0 ) ) drawit++;                     // yep

    //  Draw the icon rotated to the COG
    //  or to the Hdt if available
    float icon_hdt = pCog;
    if( !wxIsNaN( gHdt ) ) icon_hdt = gHdt;

    //  COG may be undefined in NMEA data stream
    if( wxIsNaN(icon_hdt) ) icon_hdt = 0.0;

//    Calculate the ownship drawing angle icon_rad using an assumed 10 minute predictor
    double osd_head_lat, osd_head_lon;
    wxPoint osd_head_point;

    ll_gc_ll( gLat, gLon, icon_hdt, pSog * 10. / 60., &osd_head_lat, &osd_head_lon );

    GetCanvasPointPix( gLat, gLon, &lShipMidPoint );
    GetCanvasPointPix( osd_head_lat, osd_head_lon, &osd_head_point );

    float icon_rad = atan2f( (float) ( osd_head_point.y - lShipMidPoint.y ),
                             (float) ( osd_head_point.x - lShipMidPoint.x ) );
    icon_rad += (float)PI;

    if( pSog < 0.2 ) icon_rad = ( ( icon_hdt + 90. ) * PI / 180. ) + GetVP().rotation;

//    Calculate ownship Heading pointer as a predictor
    double hdg_pred_lat, hdg_pred_lon;

    ll_gc_ll( gLat, gLon, icon_hdt, g_ownship_HDTpredictor_miles, &hdg_pred_lat,
              &hdg_pred_lon );

    GetCanvasPointPix( gLat, gLon, &lShipMidPoint );
    GetCanvasPointPix( hdg_pred_lat, hdg_pred_lon, &lHeadPoint );

//    Should we draw the Head vector?
//    Compare the points lHeadPoint and lPredPoint
//    If they differ by more than n pixels, and the head vector is valid, then render the head vector

    float ndelta_pix = 10.;
    bool b_render_hdt = false;
    if( !wxIsNaN( gHdt ) ) {
        float dist = sqrtf( powf(  (float) (lHeadPoint.x - lPredPoint.x), 2) +
                            powf(  (float) (lHeadPoint.y - lPredPoint.y), 2) );
        if( dist > ndelta_pix && !wxIsNaN(gSog) )
            b_render_hdt = true;
    }

//    Another draw test ,based on pixels, assuming the ship icon is a fixed nominal size
//    and is just barely outside the viewport        ....
    wxBoundingBox bb_screen( 0, 0, GetVP().pix_width, GetVP().pix_height );
    if( bb_screen.PointInBox( lShipMidPoint, 20 ) ) drawit++;

    // And one more test to catch the case where COG line crosses the screen,
    // but ownship and pred point are both off

    if( GetVP().GetBBox().LineIntersect( wxPoint2DDouble( gLon, gLat ),
                                         wxPoint2DDouble( pred_lon, pred_lat ) ) ) drawit++;

//    Do the draw if either the ship or prediction is within the current VPoint
    if( !drawit )
        return;

    int img_height;

    if( GetVP().chart_scale > 300000 )             // According to S52, this should be 50,000
    {
        ShipDrawLargeScale(dc, lShipMidPoint);
        img_height = 20;
    } else {

        wxImage pos_image;

        //      Substitute user ownship image if found
        if( m_pos_image_user )
            pos_image = m_pos_image_user->Copy();
        else if( SHIP_NORMAL == m_ownship_state )
            pos_image = m_pos_image_red->Copy();
        if( SHIP_LOWACCURACY == m_ownship_state )
            pos_image = m_pos_image_yellow->Copy();
        else if( SHIP_NORMAL != m_ownship_state )
            pos_image = m_pos_image_grey->Copy();


            //      Substitute user ownship image if found
        if( m_pos_image_user ) {
            pos_image = m_pos_image_user->Copy();
                
            if( SHIP_LOWACCURACY == m_ownship_state ) 
                pos_image = m_pos_image_user_yellow->Copy();
            else if( SHIP_NORMAL != m_ownship_state )
                pos_image = m_pos_image_user_grey->Copy();
        }

        img_height = pos_image.GetHeight();

        if( g_n_ownship_beam_meters > 0.0 &&
            g_n_ownship_length_meters > 0.0 &&
            g_OwnShipIconType > 0 ) // use large ship
        {
            int ownShipWidth = 22; // Default values from s_ownship_icon
            int ownShipLength= 84;
            if( g_OwnShipIconType == 1 ) {
                ownShipWidth = pos_image.GetWidth();
                ownShipLength= pos_image.GetHeight();
            }

            float scale_factor_x, scale_factor_y;
            ComputeShipScaleFactor
                (icon_hdt, ownShipWidth, ownShipLength, lShipMidPoint,
                 GPSOffsetPixels, lGPSPoint, scale_factor_x, scale_factor_y);

            if( g_OwnShipIconType == 1 ) { // Scaled bitmap
                pos_image.Rescale( ownShipWidth * scale_factor_x, ownShipLength * scale_factor_y,
                                   wxIMAGE_QUALITY_HIGH );
                wxPoint rot_ctr( pos_image.GetWidth() / 2, pos_image.GetHeight() / 2 );
                wxImage rot_image = pos_image.Rotate( -( icon_rad - ( PI / 2. ) ), rot_ctr, true );

                // Simple sharpening algorithm.....
                for( int ip = 0; ip < rot_image.GetWidth(); ip++ )
                    for( int jp = 0; jp < rot_image.GetHeight(); jp++ )
                        if( rot_image.GetAlpha( ip, jp ) > 64 ) rot_image.SetAlpha( ip, jp, 255 );

                wxBitmap os_bm( rot_image );

                int w = os_bm.GetWidth();
                int h = os_bm.GetHeight();
                img_height = h;

                dc.DrawBitmap( os_bm, lShipMidPoint.x - w / 2, lShipMidPoint.y - h / 2, true );

                // Maintain dirty box,, missing in __WXMSW__ library
                dc.CalcBoundingBox( lShipMidPoint.x - w / 2, lShipMidPoint.y - h / 2 );
                dc.CalcBoundingBox( lShipMidPoint.x - w / 2 + w, lShipMidPoint.y - h / 2 + h );
            }

            else if( g_OwnShipIconType == 2 ) { // Scaled Vector
                wxPoint ownship_icon[10];

                for( int i = 0; i < 10; i++ ) {
                    int j = i * 2;
                    float pxa = (float) ( s_ownship_icon[j] );
                    float pya = (float) ( s_ownship_icon[j + 1] );
                    pya *= scale_factor_y;
                    pxa *= scale_factor_x;

                    float px = ( pxa * sinf( icon_rad ) ) + ( pya * cosf( icon_rad ) );
                    float py = ( pya * sinf( icon_rad ) ) - ( pxa * cosf( icon_rad ) );

                    ownship_icon[i].x = (int) ( px ) + lShipMidPoint.x;
                    ownship_icon[i].y = (int) ( py ) + lShipMidPoint.y;
                }

                wxPen ppPen1( GetGlobalColor( _T ( "UBLCK" ) ), 1, wxSOLID );
                dc.SetPen( ppPen1 );
                dc.SetBrush( wxBrush( ShipColor() ) );

                dc.StrokePolygon( 6, &ownship_icon[0], 0, 0 );

                //     draw reference point (midships) cross
                dc.StrokeLine( ownship_icon[6].x, ownship_icon[6].y, ownship_icon[7].x,
                               ownship_icon[7].y );
                dc.StrokeLine( ownship_icon[8].x, ownship_icon[8].y, ownship_icon[9].x,
                               ownship_icon[9].y );
            }

            img_height = ownShipLength * scale_factor_y;

            //      Reference point, where the GPS antenna is
            int circle_rad = 3;
            if( m_pos_image_user ) circle_rad = 1;

            dc.SetPen( wxPen( GetGlobalColor( _T ( "UBLCK" ) ), 1 ) );
            dc.SetBrush( wxBrush( GetGlobalColor( _T ( "UIBCK" ) ) ) );
            dc.StrokeCircle( lGPSPoint.x, lGPSPoint.y, circle_rad );
        }
        else { // Fixed bitmap icon.
            /* non opengl, or suboptimal opengl via ocpndc: */
            wxPoint rot_ctr( pos_image.GetWidth() / 2, pos_image.GetHeight() / 2 );
            wxImage rot_image = pos_image.Rotate( -( icon_rad - ( PI / 2. ) ), rot_ctr, true );

            // Simple sharpening algorithm.....
            for( int ip = 0; ip < rot_image.GetWidth(); ip++ )
                for( int jp = 0; jp < rot_image.GetHeight(); jp++ )
                    if( rot_image.GetAlpha( ip, jp ) > 64 ) rot_image.SetAlpha( ip, jp, 255 );

            wxBitmap os_bm( rot_image );

            int w = os_bm.GetWidth();
            int h = os_bm.GetHeight();
            img_height = h;

            dc.DrawBitmap( os_bm, lShipMidPoint.x - w / 2, lShipMidPoint.y - h / 2, true );

            //      Reference point, where the GPS antenna is
            int circle_rad = 3;
            if( m_pos_image_user ) circle_rad = 1;

            dc.SetPen( wxPen( GetGlobalColor( _T ( "UBLCK" ) ), 1 ) );
            dc.SetBrush( wxBrush( GetGlobalColor( _T ( "UIBCK" ) ) ) );
            dc.StrokeCircle( lShipMidPoint.x, lShipMidPoint.y, circle_rad );

            // Maintain dirty box,, missing in __WXMSW__ library
            dc.CalcBoundingBox( lShipMidPoint.x - w / 2, lShipMidPoint.y - h / 2 );
            dc.CalcBoundingBox( lShipMidPoint.x - w / 2 + w, lShipMidPoint.y - h / 2 + h );
        }
    }        // ownship draw

    ShipIndicatorsDraw(dc, lpp,  GPSOffsetPixels,
                        lGPSPoint,  lHeadPoint,
                       img_height, cog_rad,
                       lPredPoint,  b_render_hdt, lShipMidPoint);
}

/* @ChartCanvas::CalcGridSpacing
 **
 ** Calculate the major and minor spacing between the lat/lon grid
 **
 ** @param [r] WindowDegrees [float] displayed number of lat or lan in the window
 ** @param [w] MajorSpacing [float &] Major distance between grid lines
 ** @param [w] MinorSpacing [float &] Minor distance between grid lines
 ** @return [void]
 */
void CalcGridSpacing( float WindowDegrees, float& MajorSpacing, float&MinorSpacing )
{
    int tabi; // iterator for lltab

    // table for calculating the distance between the grids
    // [0] width or height of the displayed chart in degrees
    // [1] spacing between major grid lines in degrees
    // [2] spacing between minor grid lines in degrees
    const float lltab[][3] = { { 180.0f, 90.0f, 30.0f }, { 90.0f, 45.0f, 15.0f }, { 60.0f, 30.0f, 10.0f }, {
            20.0f, 10.0f, 2.0f
        }, { 10.0f, 5.0f, 1.0f }, { 4.0f, 2.0f, 30.0f / 60.0f }, {
            2.0f, 1.0f, 20.0f
            / 60.0f
        }, { 1.0f, 0.5f, 10.0f / 60.0f }, { 30.0f / 60.0f, 15.0f / 60.0f, 5.0f / 60.0f }, {
            20.0f
            / 60.0f, 10.0f / 60.0f, 2.0f / 60.0f
        }, { 10.0f / 60.0f, 5.0f / 60.0f, 1.0f / 60.0f }, {
            4.0f
            / 60.0f, 2.0f / 60.0f, 0.5f / 60.0f
        }, { 2.0f / 60.0f, 1.0f / 60.0f, 0.2f / 60.0f }, {
            1.0f / 60.0f,
            0.5f / 60.0f, 0.1f / 60.0f
        }, { 0.4f / 60.0f, 0.2f / 60.0f, 0.05f / 60.0f }, {
            0.0f, 0.1f / 60.0f,
            0.02f / 60.0f
        } // indicates last entry
    };

    for( tabi = 0; lltab[tabi][0] != 0.0; tabi++ ) {
        if( WindowDegrees > lltab[tabi][0] ) {
            break;
        }
    }
    MajorSpacing = lltab[tabi][1]; // major latitude distance
    MinorSpacing = lltab[tabi][2]; // minor latitude distance
    return;
}
/* @ChartCanvas::CalcGridText *************************************
 **
 ** Calculates text to display at the major grid lines
 **
 ** @param [r] latlon [float] latitude or longitude of grid line
 ** @param [r] spacing [float] distance between two major grid lines
 ** @param [r] bPostfix [bool] true for latitudes, false for longitudes
 ** @param [w] text [char*] textbuffer for result, minimum of 12 chars in length
 **
 ** @return [void]
 */
void CalcGridText( float latlon, float spacing, bool bPostfix, char *text )
{
    int deg = (int) fabs( latlon ); // degrees
    float min = fabs( ( fabs( latlon ) - deg ) * 60.0 ); // Minutes
    char postfix;
    const unsigned int BufLen = 12;

    // calculate postfix letter (NSEW)
    if( latlon > 0.0 ) {
        if( bPostfix ) {
            postfix = 'N';
        } else {
#ifdef __WXOSX__
            postfix = 'O';
#else
            postfix = 'E';
#endif
        }
    } else if( latlon < 0.0 ) {
        if( bPostfix ) {
            postfix = 'S';
        } else {
            postfix = 'W';
        }
    } else {
        postfix = ' '; // no postfix for equator and greenwich
    }
    // calculate text, display minutes only if spacing is smaller than one degree

    if( spacing >= 1.0 ) {
        snprintf( text, BufLen, "%3d° %c", deg, postfix );
    } else if( spacing >= ( 1.0 / 60.0 ) ) {
        snprintf( text, BufLen, "%3d°%02.0f %c", deg, min, postfix );
    } else {
        snprintf( text, BufLen, "%3d°%02.2f %c", deg, min, postfix );
    }
    text[BufLen - 1] = '\0';
    return;
}

/* @ChartCanvas::GridDraw *****************************************
 **
 ** Draws major and minor Lat/Lon Grid on the chart
 ** - distance between Grid-lm ines are calculated automatic
 ** - major grid lines will be across the whole chart window
 ** - minor grid lines will be 10 pixel at each edge of the chart window.
 **
 ** @param [w] dc [wxDC&] the wx drawing context
 **
 ** @return [void]
 ************************************************************************/
void ChartCanvas::GridDraw( ocpnDC& dc )
{
    if( !( g_bDisplayGrid && ( fabs( GetVP().rotation ) < 1e-5 )
            && ( ( fabs( GetVP().skew ) < 1e-9 ) || g_bskew_comp ) ) ) return;

    double nlat, elon, slat, wlon;
    float lat, lon;
    float dlat, dlon;
    float gridlatMajor, gridlatMinor, gridlonMajor, gridlonMinor;
    wxCoord w, h;
    wxPen GridPen( GetGlobalColor( _T ( "SNDG1" ) ), 1, wxSOLID );
#ifdef __WXOSX__
    wxFont *font = wxTheFontList->FindOrCreateFont( 12, wxFONTFAMILY_SWISS, wxNORMAL,
                                                   wxFONTWEIGHT_NORMAL, FALSE, wxString( _T ( "Arial" ) ) );
#else
    wxFont *font = wxTheFontList->FindOrCreateFont( 8, wxFONTFAMILY_SWISS, wxNORMAL,
                   wxFONTWEIGHT_NORMAL, FALSE, wxString( _T ( "Arial" ) ) );
#endif
    dc.SetPen( GridPen );
    dc.SetFont( *font );
#ifdef __WXOSX__
    dc.SetTextForeground( GetGlobalColor( _T ( "CHBLK" ) ) );
#else
    dc.SetTextForeground( GetGlobalColor( _T ( "SNDG1" ) ) );
#endif

    w = m_canvas_width;
    h = m_canvas_height;

    GetCanvasPixPoint( 0, 0, nlat, wlon ); // get lat/lon of upper left point of the window
    GetCanvasPixPoint( w, h, slat, elon ); // get lat/lon of lower right point of the window
    dlat = nlat - slat; // calculate how many degrees of latitude are shown in the window
    dlon = elon - wlon; // calculate how many degrees of longitude are shown in the window
    if( dlon < 0.0 ) // concider datum border at 180 degrees longitude
    {
        dlon = dlon + 360.0;
    }
    // calculate distance between latitude grid lines
    CalcGridSpacing( dlat, gridlatMajor, gridlatMinor );

    // calculate position of first major latitude grid line
    lat = ceil( slat / gridlatMajor ) * gridlatMajor;

    // Draw Major latitude grid lines and text
    while( lat < nlat ) {
        wxPoint r;
        char sbuf[12];
        CalcGridText( lat, gridlatMajor, true, sbuf ); // get text for grid line
        GetCanvasPointPix( lat, ( elon + wlon ) / 2, &r );
        dc.DrawLine( 0, r.y, w, r.y, false );                             // draw grid line
        dc.DrawText( wxString( sbuf, wxConvUTF8 ), 0, r.y ); // draw text
        lat = lat + gridlatMajor;

        if( fabs( lat - wxRound( lat ) ) < 1e-5 ) lat = wxRound( lat );
    }

    // calculate position of first minor latitude grid line
    lat = ceil( slat / gridlatMinor ) * gridlatMinor;

    // Draw minor latitude grid lines
    while( lat < nlat ) {
        wxPoint r;
        GetCanvasPointPix( lat, ( elon + wlon ) / 2, &r );
        dc.DrawLine( 0, r.y, 10, r.y, false );
        dc.DrawLine( w - 10, r.y, w, r.y, false );
        lat = lat + gridlatMinor;
    }

    // calculate distance between grid lines
    CalcGridSpacing( dlon, gridlonMajor, gridlonMinor );

    // calculate position of first major latitude grid line
    lon = ceil( wlon / gridlonMajor ) * gridlonMajor;

    // draw major longitude grid lines
    for( int i = 0, itermax = (int) ( dlon / gridlonMajor ); i <= itermax; i++ ) {
        wxPoint r;
        char sbuf[12];
        CalcGridText( lon, gridlonMajor, false, sbuf );
        GetCanvasPointPix( ( nlat + slat ) / 2, lon, &r );
        dc.DrawLine( r.x, 0, r.x, h, false );
        dc.DrawText( wxString( sbuf, wxConvUTF8 ), r.x, 0 );
        lon = lon + gridlonMajor;
        if( lon > 180.0 ) {
            lon = lon - 360.0;
        }

        if( fabs( lon - wxRound( lon ) ) < 1e-5 ) lon = wxRound( lon );

    }

    // calculate position of first minor longitude grid line
    lon = ceil( wlon / gridlonMinor ) * gridlonMinor;
    // draw minor longitude grid lines
    for( int i = 0, itermax = (int) ( dlon / gridlonMinor ); i <= itermax; i++ ) {
        wxPoint r;
        GetCanvasPointPix( ( nlat + slat ) / 2, lon, &r );
        dc.DrawLine( r.x, 0, r.x, 10, false );
        dc.DrawLine( r.x, h - 10, r.x, h, false );
        lon = lon + gridlonMinor;
        if( lon > 180.0 ) {
            lon = lon - 360.0;
        }
    }
}

void ChartCanvas::ScaleBarDraw( ocpnDC& dc )
{
    double blat, blon, tlat, tlon;
    wxPoint r;

    int x_origin = g_bDisplayGrid ? 60 : 20;
    int y_origin = m_canvas_height - 50;

    float dist;
    int count;
    wxPen pen1, pen2;

    if( GetVP().chart_scale > 80000 )        // Draw 10 mile scale as SCALEB11
    {
        dist = 10.0;
        count = 5;
        pen1 = wxPen( GetGlobalColor( _T ( "SNDG2" ) ), 3, wxSOLID );
        pen2 = wxPen( GetGlobalColor( _T ( "SNDG1" ) ), 3, wxSOLID );
    } else                                // Draw 1 mile scale as SCALEB10
    {
        dist = 1.0;
        count = 10;
        pen1 = wxPen( GetGlobalColor( _T ( "SCLBR" ) ), 3, wxSOLID );
        pen2 = wxPen( GetGlobalColor( _T ( "CHDRD" ) ), 3, wxSOLID );
    }

    GetCanvasPixPoint( x_origin, y_origin, blat, blon );
    double rotation = -VPoint.rotation;
    if(!g_bskew_comp)
        rotation -= VPoint.skew;
    ll_gc_ll( blat, blon, rotation * 180 / PI, dist, &tlat, &tlon );
    GetCanvasPointPix( tlat, tlon, &r );
    int l1 = ( y_origin - r.y ) / count;

    for( int i = 0; i < count; i++ ) {
        int y = l1 * i;
        if( i & 1 ) dc.SetPen( pen1 );
        else
            dc.SetPen( pen2 );
        
        dc.DrawLine( x_origin, y_origin - y, x_origin, y_origin - ( y + l1 ) );
    }
}

void ChartCanvas::JaggyCircle( ocpnDC &dc, wxPen pen, int x, int y, int radius )
{
    //    Constants?
    double da_min = 2.;
    double da_max = 6.;
    double ra_min = 0.;
    double ra_max = 40.;

    wxPen pen_save = dc.GetPen();

    wxDateTime now = wxDateTime::Now();

    dc.SetPen( pen );

    int x0, y0, x1, y1;

    x0 = x1 = x + radius;                    // Start point
    y0 = y1 = y;
    double angle = 0.;
    int i = 0;

    while( angle < 360. ) {
        double da = da_min + ( ( (double) rand() / RAND_MAX ) * ( da_max - da_min ) );
        angle += da;

        if( angle > 360. ) angle = 360.;

        double ra = ra_min + ( ( (double) rand() / RAND_MAX ) * ( ra_max - ra_min ) );

        double r;
        if( i % 1 ) r = radius + ra;
        else
            r = radius - ra;

        x1 = (int) ( x + cos( angle * PI / 180. ) * r );
        y1 = (int) ( y + sin( angle * PI / 180. ) * r );

        dc.DrawLine( x0, y0, x1, y1 );

        x0 = x1;
        y0 = y1;

        i++;

    }

    dc.DrawLine( x + radius, y, x1, y1 );             // closure

    dc.SetPen( pen_save );
}

void ChartCanvas::AlertDraw( ocpnDC& dc )
{
// Just for prototyping, visual alert for anchorwatch goes here
    bool play_sound = false;
    if( pAnchorWatchPoint1 && AnchorAlertOn1 ) {
        if( AnchorAlertOn1 ) {
            wxPoint TargetPoint;
            GetCanvasPointPix( pAnchorWatchPoint1->m_lat, pAnchorWatchPoint1->m_lon, &TargetPoint );
            JaggyCircle( dc, wxPen( GetGlobalColor( _T("URED") ), 2 ), TargetPoint.x, TargetPoint.y,
                         100 );
            play_sound = true;
        }
    } else
        AnchorAlertOn1 = false;

    if( pAnchorWatchPoint2 && AnchorAlertOn2 ) {
        if( AnchorAlertOn2 ) {
            wxPoint TargetPoint;
            GetCanvasPointPix( pAnchorWatchPoint2->m_lat, pAnchorWatchPoint2->m_lon, &TargetPoint );
            JaggyCircle( dc, wxPen( GetGlobalColor( _T("URED") ), 2 ), TargetPoint.x, TargetPoint.y,
                         100 );
            play_sound = true;
        }
    } else
        AnchorAlertOn2 = false;


    if( play_sound ) {
        if( !g_anchorwatch_sound.IsOk() ) g_anchorwatch_sound.Create( g_sAIS_Alert_Sound_File );

#ifndef __WXMSW__
        if(g_anchorwatch_sound.IsOk() && !g_anchorwatch_sound.IsPlaying())
            g_anchorwatch_sound.Play();
#else
            if( g_anchorwatch_sound.IsOk() ) g_anchorwatch_sound.Play();
#endif
    } else {
        if( g_anchorwatch_sound.IsOk() ) g_anchorwatch_sound.Stop();
    }

}
// End of prototype anchor watch alerting-----------------------

void ChartCanvas::UpdateShips()
{
    //  Get the rectangle in the current dc which bounds the "ownship" symbol

    wxClientDC dc( this );
    if( !dc.IsOk() ) return;

    wxBitmap test_bitmap( dc.GetSize().x, dc.GetSize().y );
    wxMemoryDC temp_dc( test_bitmap );

    temp_dc.ResetBoundingBox();
    temp_dc.DestroyClippingRegion();
    temp_dc.SetClippingRegion( 0, 0, dc.GetSize().x, dc.GetSize().y );

    // Draw the ownship on the temp_dc
    ocpnDC ocpndc = ocpnDC( temp_dc );
    ShipDraw( ocpndc );

    if( g_pActiveTrack && g_pActiveTrack->IsRunning() ) {
        RoutePoint* p = g_pActiveTrack->GetLastPoint();
        if( p ) {
            wxPoint px;
            cc1->GetCanvasPointPix( p->m_lat, p->m_lon, &px );
            ocpndc.CalcBoundingBox( px.x, px.y );
        }
    }

    ship_draw_rect = wxRect( temp_dc.MinX(), temp_dc.MinY(),
            temp_dc.MaxX() - temp_dc.MinX(),
            temp_dc.MaxY() - temp_dc.MinY() );

    wxRect own_ship_update_rect = ship_draw_rect;

    if( !own_ship_update_rect.IsEmpty() ) {
        //  The required invalidate rectangle is the union of the last drawn rectangle
        //  and this drawn rectangle
        own_ship_update_rect.Union( ship_draw_last_rect );
        own_ship_update_rect.Inflate( 2 );
    }

    if( !own_ship_update_rect.IsEmpty() ) RefreshRect( own_ship_update_rect, false );

    ship_draw_last_rect = ship_draw_rect;

    temp_dc.SelectObject( wxNullBitmap );
}

void ChartCanvas::UpdateAlerts()
{
    //  Get the rectangle in the current dc which bounds the detected Alert targets

    //  Use this dc
    wxClientDC dc( this );

    // Get dc boundary
    int sx, sy;
    dc.GetSize( &sx, &sy );

    //  Need a bitmap
    wxBitmap test_bitmap( sx, sy, -1 );

    // Create a memory DC
    wxMemoryDC temp_dc;
    temp_dc.SelectObject( test_bitmap );

    temp_dc.ResetBoundingBox();
    temp_dc.DestroyClippingRegion();
    temp_dc.SetClippingRegion( wxRect( 0, 0, sx, sy ) );

    // Draw the Alert Targets on the temp_dc
    ocpnDC ocpndc = ocpnDC( temp_dc );
    AlertDraw( ocpndc );

    //  Retrieve the drawing extents
    wxRect alert_rect( temp_dc.MinX(), temp_dc.MinY(), temp_dc.MaxX() - temp_dc.MinX(),
                       temp_dc.MaxY() - temp_dc.MinY() );

    if( !alert_rect.IsEmpty() ) alert_rect.Inflate( 2 );              // clear all drawing artifacts

    if( !alert_rect.IsEmpty() || !alert_draw_rect.IsEmpty() ) {
        //  The required invalidate rectangle is the union of the last drawn rectangle
        //  and this drawn rectangle
        wxRect alert_update_rect = alert_draw_rect;
        alert_update_rect.Union( alert_rect );

        //  Invalidate the rectangular region
        RefreshRect( alert_update_rect, false );
    }

    //  Save this rectangle for next time
    alert_draw_rect = alert_rect;

    temp_dc.SelectObject( wxNullBitmap );      // clean up
}

void ChartCanvas::UpdateAIS()
{
    if(!g_pAIS) return;

    //  Get the rectangle in the current dc which bounds the detected AIS targets

    //  Use this dc
    wxClientDC dc( this );

    // Get dc boundary
    int sx, sy;
    dc.GetSize( &sx, &sy );

    wxRect ais_rect;

    //  How many targets are there?

    //  If more than "some number", it will be cheaper to refresh the entire screen
    //  than to build update rectangles for each target.
    AIS_Target_Hash *current_targets = g_pAIS->GetTargetList();
    if( current_targets->size() > 10 ) {
        ais_rect = wxRect( 0, 0, sx, sy );            // full screen
    } else {
        //  Need a bitmap
        wxBitmap test_bitmap( sx, sy, -1 );

        // Create a memory DC
        wxMemoryDC temp_dc;
        temp_dc.SelectObject( test_bitmap );

        temp_dc.ResetBoundingBox();
        temp_dc.DestroyClippingRegion();
        temp_dc.SetClippingRegion( wxRect( 0, 0, sx, sy ) );

        // Draw the AIS Targets on the temp_dc
        ocpnDC ocpndc = ocpnDC( temp_dc );
        AISDraw( ocpndc );
        AISDrawAreaNotices( ocpndc );

        //  Retrieve the drawing extents
        ais_rect = wxRect( temp_dc.MinX(), temp_dc.MinY(), temp_dc.MaxX() - temp_dc.MinX(),
                           temp_dc.MaxY() - temp_dc.MinY() );

        if( !ais_rect.IsEmpty() ) ais_rect.Inflate( 2 );              // clear all drawing artifacts

        temp_dc.SelectObject( wxNullBitmap );      // clean up

    }

    if( !ais_rect.IsEmpty() || !ais_draw_rect.IsEmpty() ) {
        //  The required invalidate rectangle is the union of the last drawn rectangle
        //  and this drawn rectangle
        wxRect ais_update_rect = ais_draw_rect;
        ais_update_rect.Union( ais_rect );

        //  Invalidate the rectangular region
        RefreshRect( ais_update_rect, false );
    }

    //  Save this rectangle for next time
    ais_draw_rect = ais_rect;

}

void ChartCanvas::OnActivate( wxActivateEvent& event )
{
    ReloadVP();
}

void ChartCanvas::OnSize( wxSizeEvent& event )
{

    GetClientSize( &m_canvas_width, &m_canvas_height );

//    Get some canvas metrics

//          Rescale to current value, in order to rebuild VPoint data structures
//          for new canvas size
    SetVPScale( GetVPScale() );

    double display_size_meters = wxGetDisplaySizeMM().GetWidth() / 1000.; // gives screen size(width) in meters
//        m_canvas_scale_factor = m_canvas_width / display_size_meters;
    m_canvas_scale_factor = wxGetDisplaySize().GetWidth() / display_size_meters;

    m_absolute_min_scale_ppm = m_canvas_width / ( 1.5 * WGS84_semimajor_axis_meters * PI ); // something like 180 degrees

#ifdef USE_S57
    if( ps52plib ) ps52plib->SetPPMM( m_canvas_scale_factor / 1000. );
#endif

    //  Inform the parent Frame that I am being resized...
    gFrame->ProcessCanvasResize();

//    Set up the scroll margins
    xr_margin = m_canvas_width * 95 / 100;
    xl_margin = m_canvas_width * 5 / 100;
    yt_margin = m_canvas_height * 5 / 100;
    yb_margin = m_canvas_height * 95 / 100;

    if( m_pQuilt ) m_pQuilt->SetQuiltParameters( m_canvas_scale_factor, m_canvas_width );

//    Resize the current viewport

    VPoint.pix_width = m_canvas_width;
    VPoint.pix_height = m_canvas_height;

    // Resize the scratch BM
    delete pscratch_bm;
    pscratch_bm = new wxBitmap( VPoint.pix_width, VPoint.pix_height, -1 );

    // Resize the Route Calculation BM
    m_dc_route.SelectObject( wxNullBitmap );
    delete proute_bm;
    proute_bm = new wxBitmap( VPoint.pix_width, VPoint.pix_height, -1 );
    m_dc_route.SelectObject( *proute_bm );

    //  Resize the saved Bitmap
    m_cached_chart_bm.Create( VPoint.pix_width, VPoint.pix_height, -1 );

    //  Resize the working Bitmap
    m_working_bm.Create( VPoint.pix_width, VPoint.pix_height, -1 );

    //  Rescale again, to capture all the changes for new canvas size
    SetVPScale( GetVPScale() );

#ifdef ocpnUSE_GL
    if( /*g_bopengl &&*/ m_glcc ) {
        m_glcc->OnSize( event );
    }
#endif
    //  Invalidate the whole window
    ReloadVP();
}

void ChartCanvas::ShowChartInfoWindow( int x, int y, int dbIndex )
{
    if( dbIndex >= 0 ) {
        if( NULL == m_pCIWin ) {
            m_pCIWin = new ChInfoWin( this );
            m_pCIWin->Hide();
        }

        if( !m_pCIWin->IsShown() || (m_pCIWin->dbIndex != dbIndex) ) {
            wxString s;
            ChartBase *pc = NULL;

            if( ( ChartData->IsChartInCache( dbIndex ) ) && ChartData->IsValid() )
                pc = ChartData->OpenChartFromDB( dbIndex, FULL_INIT );   // this must come from cache

            int char_width, char_height;
            s = ChartData->GetFullChartInfo( pc, dbIndex, &char_width, &char_height );
            m_pCIWin->SetString( s );
            m_pCIWin->FitToChars( char_width, char_height );

            wxPoint p;
            p.x = x;
            if( ( p.x + m_pCIWin->GetWinSize().x ) > m_canvas_width )
                p.x = m_canvas_width - m_pCIWin->GetWinSize().x;

            int statsW, statsH;
            stats->GetSize( &statsW, &statsH );
            p.y = m_canvas_height - statsH - 4 - m_pCIWin->GetWinSize().y;

            m_pCIWin->dbIndex = dbIndex;
            m_pCIWin->SetPosition( p );
            m_pCIWin->SetBitmap();
            m_pCIWin->Refresh();
            m_pCIWin->Show();
        }
    } else {
        HideChartInfoWindow();
    }
}

void ChartCanvas::HideChartInfoWindow( void )
{
    if( m_pCIWin && m_pCIWin->IsShown() ) m_pCIWin->Hide();
}

void ChartCanvas::PanTimerEvent( wxTimerEvent& event )
{
    wxMouseEvent ev( wxEVT_MOTION );
    ev.m_x = mouse_x;
    ev.m_y = mouse_y;
    ev.m_leftDown = mouse_leftisdown;

    wxEvtHandler *evthp = GetEventHandler();

    ::wxPostEvent( evthp, ev );

}

void ChartCanvas::MovementTimerEvent( wxTimerEvent& )
{
    DoTimedMovement();
}

void ChartCanvas::MovementStopTimerEvent( wxTimerEvent& )
{
    StopMovement( );
}

bool ChartCanvas::CheckEdgePan( int x, int y, bool bdragging, int margin, int delta )
{
    bool bft = false;
    int pan_margin = m_canvas_width * margin / 100;
    int pan_timer_set = 200;
    double pan_delta = GetVP().pix_width * delta / 100;
    int pan_x = 0;
    int pan_y = 0;

    if( x > m_canvas_width - pan_margin ) {
        bft = true;
        pan_x = pan_delta;
    }

    else if( x < pan_margin ) {
        bft = true;
        pan_x = -pan_delta;
    }

    if( y < pan_margin ) {
        bft = true;
        pan_y = -pan_delta;
    }

    else if( y > m_canvas_height - pan_margin ) {
        bft = true;
        pan_y = pan_delta;
    }

    //    Of course, if dragging, and the mouse left button is not down, we must stop the event injection
    if( bdragging ) {
        if( !g_btouch )
        {
            wxMouseState state = ::wxGetMouseState();
            if( !state.LeftIsDown() )
                bft = false;
        }
    }
    if( ( bft ) && !pPanTimer->IsRunning() ) {
        PanCanvas( pan_x, pan_y );
        pPanTimer->Start( pan_timer_set, wxTIMER_ONE_SHOT );
        return true;
    }

    //    This mouse event must not be due to pan timer event injector
    //    Mouse is out of the pan zone, so prevent any orphan event injection
    if( ( !bft ) && pPanTimer->IsRunning() ) {
        pPanTimer->Stop();
    }

    return ( false );
}

// Look for waypoints at the current position.
// Used to determine what a mouse event should act on.

void ChartCanvas::FindRoutePointsAtCursor( float selectRadius, bool setBeingEdited )
{
    m_lastRoutePointEditTarget = m_pRoutePointEditTarget;       // save a copy
    m_pRoutePointEditTarget = NULL;
    m_pFoundPoint = NULL;

    SelectItem *pFind = NULL;
    SelectableItemList SelList = pSelect->FindSelectionList( m_cursor_lat, m_cursor_lon,
                                 SELTYPE_ROUTEPOINT );
    wxSelectableItemListNode *node = SelList.GetFirst();
    while( node ) {
        pFind = node->GetData();

        RoutePoint *frp = (RoutePoint *) pFind->m_pData1;

        //    Get an array of all routes using this point
        m_pEditRouteArray = g_pRouteMan->GetRouteArrayContaining( frp );

        // Use route array to determine actual visibility for the point
        bool brp_viz = false;
        if( m_pEditRouteArray ) {
            for( unsigned int ir = 0; ir < m_pEditRouteArray->GetCount(); ir++ ) {
                Route *pr = (Route *) m_pEditRouteArray->Item( ir );
                if( pr->IsVisible() ) {
                    brp_viz = true;
                    break;
                }
            }
        } else
            brp_viz = frp->IsVisible();               // isolated point

        if( brp_viz ) {
            //    Use route array to rubberband all affected routes
            if( m_pEditRouteArray )                 // Editing Waypoint as part of route
            {
                for( unsigned int ir = 0; ir < m_pEditRouteArray->GetCount(); ir++ ) {
                    Route *pr = (Route *) m_pEditRouteArray->Item( ir );
                    pr->m_bIsBeingEdited = setBeingEdited;
                }
                m_bRouteEditing = setBeingEdited;
            } else                                      // editing Mark
            {
                frp->m_bIsBeingEdited = setBeingEdited;
                m_bMarkEditing = setBeingEdited;
            }

            m_pRoutePointEditTarget = frp;
            m_pFoundPoint = pFind;
            break;            // out of the while(node)
        }

        node = node->GetNext();
    }       // while (node)
}

void ChartCanvas::MouseTimedEvent( wxTimerEvent& event )
{
    if( singleClickEventIsValid ) MouseEvent( singleClickEvent );
    singleClickEventIsValid = false;
    m_DoubleClickTimer->Stop();
}

void ChartCanvas::MouseEvent( wxMouseEvent& event )
{
    int x, y;
    int mx, my;

    // Protect from leftUp's coming from event handlers in child
    // windows who return focus to the canvas.
    static bool leftIsDown = false;

    // Protect from very small cursor slips during double click, which produce a
    // single Drag event.
    
    // This code is nonsense...
#if 0    
    static bool lastEventWasDrag = false;

    if( event.Dragging() && !lastEventWasDrag ) {
        lastEventWasDrag = true;
        return;
    }
    lastEventWasDrag = event.Dragging();
#endif

    event.GetPosition( &x, &y );

    // Update modifiers here; some window managers never send the key event
    m_modkeys = 0;
    if(event.ControlDown())
        m_modkeys |= wxMOD_CONTROL;
    if(event.AltDown())
        m_modkeys |= wxMOD_ALT;

#ifdef __WXMSW__
    //TODO Test carefully in other platforms, remove ifdef....
    if( event.ButtonDown() && !HasCapture() ) CaptureMouse();
    if( event.ButtonUp() && HasCapture() ) ReleaseMouse();
#endif

    if(g_pi_manager)
        if(g_pi_manager->SendMouseEventToPlugins( event ))
            return;                     // PlugIn did something, and does not want the canvas to do anything else
    
    // We start with Double Click processing. The first left click just starts a timer and
    // is remembered, then we actually do something if there is a LeftDClick.
    // If there is, the two single clicks are ignored.

    if( event.LeftDClick() && ( cursor_region == CENTER ) ) {
        m_DoubleClickTimer->Start();
        singleClickEventIsValid = false;

        double zlat, zlon;
        GetCanvasPixPoint( x, y, zlat, zlon );

        SelectItem *pFindAIS;
        pFindAIS = pSelectAIS->FindSelection( zlat, zlon, SELTYPE_AISTARGET );

        if( pFindAIS ) {
            m_FoundAIS_MMSI = pFindAIS->GetUserData();
            if( g_pAIS->Get_Target_Data_From_MMSI( m_FoundAIS_MMSI ) ) {
                wxWindow *pwin = wxDynamicCast(this, wxWindow);
                ShowAISTargetQueryDialog( pwin, m_FoundAIS_MMSI );
            }
            return;
        }

        SelectableItemList rpSelList = pSelect->FindSelectionList( zlat, zlon, SELTYPE_ROUTEPOINT );
        wxSelectableItemListNode *node = rpSelList.GetFirst();
        bool b_onRPtarget = false;
        while( node ) {
            SelectItem *pFind = node->GetData();
            RoutePoint *frp = (RoutePoint *) pFind->m_pData1;
            if(m_pRoutePointEditTarget && (frp == m_pRoutePointEditTarget) ){
                b_onRPtarget = true;
                break;
            }
            node = node->GetNext();
        }
        
        //      Double tap with selected RoutePoint or Mark
        bool bt1 = m_bMarkEditing;
        RoutePoint *pp = m_pRoutePointEditTarget;
        
        if(m_pRoutePointEditTarget){
            if( b_onRPtarget ) {
                ShowMarkPropertiesDialog( m_pRoutePointEditTarget );
                return;
            }
            else {
                m_pRoutePointEditTarget->m_bIsBeingEdited = false;
                m_pRoutePointEditTarget->m_bPtIsSelected = false;
                wxRect wp_rect;
                m_pRoutePointEditTarget->CalculateDCRect( m_dc_route, &wp_rect );
                m_pRoutePointEditTarget = NULL;         //cancel selection
                RefreshRect( wp_rect, true );
                return;
             }
        }
        else{
            node = rpSelList.GetFirst();
            if( node ) {
                SelectItem *pFind = node->GetData();
                RoutePoint *frp = (RoutePoint *) pFind->m_pData1;
                if(frp){
                    ShowMarkPropertiesDialog( frp );
                    return;
                }
            }
        }
                
            

        SelectItem* cursorItem;
        cursorItem = pSelect->FindSelection( zlat, zlon, SELTYPE_ROUTESEGMENT );

        if( cursorItem ) {
            Route *pr = (Route *) cursorItem->m_pData3;
            if( pr->IsVisible() ) {
                ShowRoutePropertiesDialog( _("Route Properties"), pr );
                return;
            }
        }

        cursorItem = pSelect->FindSelection( zlat, zlon, SELTYPE_TRACKSEGMENT );

        if( cursorItem ) {
            Route *pr = (Route *) cursorItem->m_pData3;
            if( pr->IsVisible() ) {
                ShowTrackPropertiesDialog( pr );
                return;
            }
        }

        // Found no object to act on, so show chart info.

        ShowObjectQueryWindow( x, y, zlat, zlon );
        return;
    }

    // Capture LeftUp's and time them, unless it already came from the timer.
    if( event.LeftUp() && !singleClickEventIsValid ) {

        // Ignore the second LeftUp after the DClick.
        if( m_DoubleClickTimer->IsRunning() ) {
            m_DoubleClickTimer->Stop();
            return;
        }

        // Save the event for later running if there is no DClick.
        m_DoubleClickTimer->Start( 250, wxTIMER_ONE_SHOT );
        singleClickEvent = event;
        singleClickEventIsValid = true;
        return;
    }

    //  This logic is necessary on MSW to handle the case where
    //  a context (right-click) menu is dismissed without action
    //  by clicking on the chart surface.
    //  We need to avoid an unintentional pan by eating some clicks...
#ifdef __WXMSW__
    if( event.LeftDown() || event.LeftUp() || event.Dragging() ) {
        if( g_click_stop > 0 ) {
            g_click_stop--;
            return;
        }
    }
#endif

    if( s_ProgDialog ) return;

    if(!g_btouch){
        if( ( m_bMeasure_Active && ( m_nMeasureState >= 2 ) ) || ( parent_frame->nRoute_State > 1 )
            || ( parent_frame->nRoute_State ) > 1 ) {
            wxPoint p = ClientToScreen( wxPoint( x, y ) );
            gFrame->SubmergeToolbarIfOverlap( p.x, p.y, 20 );
        }
    }

    //  Kick off the Rotation control timer
    if( g_bCourseUp ) {
        m_b_rot_hidef = false;
        pRotDefTimer->Start( 500, wxTIMER_ONE_SHOT );
    } else
        pRotDefTimer->Stop();

    mouse_x = x;
    mouse_y = y;
    mouse_leftisdown = event.LeftIsDown();

//      Retrigger the route leg / AIS target popup timer
    if( !g_btouch )
    {
        if( m_pRouteRolloverWin && m_pRouteRolloverWin->IsActive() )
            m_RolloverPopupTimer.Start( 10, wxTIMER_ONE_SHOT );               // faster response while the rollover is turned on
        else
            m_RolloverPopupTimer.Start( m_rollover_popup_timer_msec, wxTIMER_ONE_SHOT );
    }

//  Retrigger the cursor tracking timer
    pCurTrackTimer->Start( m_curtrack_timer_msec, wxTIMER_ONE_SHOT );

    mx = x;
    my = y;
    GetCanvasPixPoint( x, y, m_cursor_lat, m_cursor_lon );

    //    Calculate meaningful SelectRadius
    float SelectRadius;
    int sel_rad_pix = 8;
    if(g_btouch)
        sel_rad_pix = 50;

    SelectRadius = sel_rad_pix / ( m_true_scale_ppm * 1852 * 60 );  // Degrees, approximately

//      Show cursor position on Status Bar, if present
//      except for GTK, under which status bar updates are very slow
//      due to Update() call.
//      In this case, as a workaround, update the status window
//      after an interval timer (pCurTrackTimer) pops, which will happen
//      whenever the mouse has stopped moving for specified interval.
//      See the method OnCursorTrackTimerEvent()
#ifndef __WXGTK__
    SetCursorStatus(m_cursor_lat, m_cursor_lon);
#endif

    //  Send the current cursor lat/lon to all PlugIns requesting it
    if( g_pi_manager ) g_pi_manager->SendCursorLatLonToAllPlugIns( m_cursor_lat, m_cursor_lon );

    //        Check for wheel rotation
    // ideally, should be just longer than the time between
    // processing accumulated mouse events from the event queue
    // as would happen during screen redraws.
    int wheel_dir = event.GetWheelRotation();

    if( wheel_dir ) {
        int mouse_wheel_oneshot = abs(wheel_dir)*4;                  //msec
        wheel_dir = wheel_dir > 0 ? 1 : -1; // normalize

        double factor = 2.0;
        if(wheel_dir < 0)
            factor = 1/factor;
        
        if(g_bsmoothpanzoom){
            if( (m_wheelstopwatch.Time() < m_wheelzoom_stop_oneshot) ) {
                if( wheel_dir == m_last_wheel_dir ) {
                    m_wheelzoom_stop_oneshot += mouse_wheel_oneshot;
//                    m_zoom_target /= factor;
                }
                else 
                    StopMovement( );
            }
            else {    
                m_wheelzoom_stop_oneshot = mouse_wheel_oneshot;
                m_wheelstopwatch.Start(0);
//                m_zoom_target =  VPoint.chart_scale / factor;
            }
        }

        m_last_wheel_dir = wheel_dir;
        
        
        ZoomCanvas( factor, true, false );
        
    }

    if(!g_btouch ){
    //    Route Creation Rubber Banding
        if( parent_frame->nRoute_State >= 2 ) {
            r_rband.x = x;
            r_rband.y = y;
            m_bDrawingRoute = true;

            CheckEdgePan( x, y, event.Dragging(), 5, 2 );
            Refresh( false );
        }


    //    Measure Tool Rubber Banding
        if( m_bMeasure_Active && ( m_nMeasureState >= 2 ) ) {
            r_rband.x = x;
            r_rband.y = y;
            m_bDrawingRoute = true;

            CheckEdgePan( x, y, event.Dragging(), 5, 2 );
            Refresh( false );
        }
    }

//          Mouse Clicks

    
    if( event.LeftDown() ) {
        //  This really should not be needed, but....
        //  on Windows, when using wxAUIManager, sometimes the focus is lost
        //  when clicking into another pane, e.g.the AIS target list, and then back to this pane.
        //  Oddly, some mouse events are not lost, however.  Like this one....
        SetFocus();

        last_drag.x = mx;
        last_drag.y = my;
        leftIsDown = true;

        if(!g_btouch){
            if( parent_frame->nRoute_State )                  // creating route?
            {
                double rlat, rlon;

                SetCursor( *pCursorPencil );
                rlat = m_cursor_lat;
                rlon = m_cursor_lon;

                m_bRouteEditing = true;

                if( parent_frame->nRoute_State == 1 ) {
                    m_pMouseRoute = new Route();
                    pRouteList->Append( m_pMouseRoute );
                    r_rband.x = x;
                    r_rband.y = y;
                }

                //    Check to see if there is a nearby point which may be reused
                RoutePoint *pMousePoint = NULL;

                //    Calculate meaningful SelectRadius
                int nearby_sel_rad_pix = 8;
                double nearby_radius_meters = nearby_sel_rad_pix / m_true_scale_ppm;

                RoutePoint *pNearbyPoint = pWayPointMan->GetNearbyWaypoint( rlat, rlon,
                                        nearby_radius_meters );
                if( pNearbyPoint && ( pNearbyPoint != m_prev_pMousePoint )
                        && !pNearbyPoint->m_bIsInTrack && !pNearbyPoint->m_bIsInLayer )
                {
                    int dlg_return;
#ifndef __WXOSX__
                    dlg_return = OCPNMessageBox( this, _("Use nearby waypoint?"),
                                                    _("OpenCPN Route Create"),
                                                    (long) wxYES_NO | wxCANCEL | wxYES_DEFAULT );
#else
                    dlg_return = wxID_YES;
#endif
                    if( dlg_return == wxID_YES ) {
                        pMousePoint = pNearbyPoint;

                        // Using existing waypoint, so nothing to delete for undo.
                        if( parent_frame->nRoute_State > 1 )
                            undo->BeforeUndoableAction( Undo_AppendWaypoint, pMousePoint, Undo_HasParent, NULL );

                        // check all other routes to see if this point appears in any other route
                        // If it appears in NO other route, then it should e considered an isolated mark
                        if( !g_pRouteMan->FindRouteContainingWaypoint( pMousePoint ) ) pMousePoint->m_bKeepXRoute =
                                true;
                    }
                }

                if( NULL == pMousePoint ) {                 // need a new point
                    pMousePoint = new RoutePoint( rlat, rlon, _T("diamond"), _T(""), GPX_EMPTY_STRING );
                    pMousePoint->SetNameShown( false );

                    pConfig->AddNewWayPoint( pMousePoint, -1 );    // use auto next num
                    pSelect->AddSelectableRoutePoint( rlat, rlon, pMousePoint );

                    if( parent_frame->nRoute_State > 1 )
                        undo->BeforeUndoableAction( Undo_AppendWaypoint, pMousePoint, Undo_IsOrphanded, NULL );
                }

                if(m_pMouseRoute){
                    if( parent_frame->nRoute_State == 1 ) {
                        // First point in the route.
                        m_pMouseRoute->AddPoint( pMousePoint );
                    } else {
                        if( m_pMouseRoute->m_NextLegGreatCircle ) {
                            double rhumbBearing, rhumbDist, gcBearing, gcDist;
                            DistanceBearingMercator( rlat, rlon, m_prev_rlat, m_prev_rlon, &rhumbBearing, &rhumbDist );
                            Geodesic::GreatCircleDistBear( m_prev_rlon, m_prev_rlat, rlon, rlat, &gcDist, &gcBearing, NULL );
                            double gcDistNM = gcDist / 1852.0;

                            // Empirically found expression to get reasonable route segments.
                            int segmentCount = (3.0 + (rhumbDist - gcDistNM)) / pow(rhumbDist-gcDistNM-1, 0.5 );

                            wxString msg;
                            msg << _("For this leg the Great Circle route is ")
                                << FormatDistanceAdaptive( rhumbDist - gcDistNM ) << _(" shorter than rhumbline.\n\n")
                                << _("Would you like include the Great Circle routing points for this leg?");

#ifndef __WXOSX__
                            int answer = OCPNMessageBox( this, msg, _("OpenCPN Route Create"), wxYES_NO | wxNO_DEFAULT );
#else
                            int answer = wxID_NO;
#endif

                            if( answer == wxID_YES ) {
                                RoutePoint* gcPoint;
                                RoutePoint* prevGcPoint = m_prev_pMousePoint;
                                wxRealPoint gcCoord;

                                for( int i = 1; i <= segmentCount; i++ ) {
                                    double fraction = (double) i * ( 1.0 / (double) segmentCount );
                                    Geodesic::GreatCircleTravel( m_prev_rlon, m_prev_rlat, gcDist * fraction,
                                            gcBearing, &gcCoord.x, &gcCoord.y, NULL );

                                    if( i < segmentCount ) {
                                        gcPoint = new RoutePoint( gcCoord.y, gcCoord.x, _T("xmblue"), _T(""),
                                                GPX_EMPTY_STRING );
                                        gcPoint->SetNameShown( false );
                                        pConfig->AddNewWayPoint( gcPoint, -1 );
                                        pSelect->AddSelectableRoutePoint( gcCoord.y, gcCoord.x, gcPoint );
                                    } else {
                                        gcPoint = pMousePoint; // Last point, previously exsisting!
                                    }

                                    m_pMouseRoute->AddPoint( gcPoint );
                                    pSelect->AddSelectableRouteSegment( prevGcPoint->m_lat, prevGcPoint->m_lon,
                                            gcPoint->m_lat, gcPoint->m_lon, prevGcPoint, gcPoint, m_pMouseRoute );
                                    prevGcPoint = gcPoint;
                                }

                                undo->CancelUndoableAction( true );

                            } else {
                                m_pMouseRoute->AddPoint( pMousePoint );
                                pSelect->AddSelectableRouteSegment( m_prev_rlat, m_prev_rlon,
                                        rlat, rlon, m_prev_pMousePoint, pMousePoint, m_pMouseRoute );
                                undo->AfterUndoableAction( m_pMouseRoute );
                            }
                        } else {
                            // Ordinary rhumblinesegment.
                            m_pMouseRoute->AddPoint( pMousePoint );
                            pSelect->AddSelectableRouteSegment( m_prev_rlat, m_prev_rlon,
                                    rlat, rlon, m_prev_pMousePoint, pMousePoint, m_pMouseRoute );
                            undo->AfterUndoableAction( m_pMouseRoute );
                        }
                    }
                }

                m_prev_rlat = rlat;
                m_prev_rlon = rlon;
                m_prev_pMousePoint = pMousePoint;
                if(m_pMouseRoute)
                    m_pMouseRoute->m_lastMousePointIndex = m_pMouseRoute->GetnPoints();

                parent_frame->nRoute_State++;
                InvalidateGL();
                Refresh( false );
            }

            else if( m_bMeasure_Active && m_nMeasureState )   // measure tool?
            {
                double rlat, rlon;

                SetCursor( *pCursorPencil );
                rlat = m_cursor_lat;
                rlon = m_cursor_lon;

                if( m_nMeasureState == 1 ) {
                    m_pMeasureRoute = new Route();
                    pRouteList->Append( m_pMeasureRoute );
                    r_rband.x = x;
                    r_rband.y = y;
                }

                RoutePoint *pMousePoint = new RoutePoint( m_cursor_lat, m_cursor_lon,
                        wxString( _T ( "circle" ) ), wxEmptyString, GPX_EMPTY_STRING );
                pMousePoint->m_bShowName = false;

                m_pMeasureRoute->AddPoint( pMousePoint );

                m_prev_rlat = m_cursor_lat;
                m_prev_rlon = m_cursor_lon;
                m_prev_pMousePoint = pMousePoint;
                m_pMeasureRoute->m_lastMousePointIndex = m_pMeasureRoute->GetnPoints();

                m_nMeasureState++;

                InvalidateGL();
                Refresh( false );
            }

            else {
                FindRoutePointsAtCursor( SelectRadius, true );    // Not creating Route
            }
        }  // !g_btouch
        else {                  // g_btouch

           if(( m_bMeasure_Active && m_nMeasureState ) || ( parent_frame->nRoute_State )){

               // if near screen edge, pan with injection
//                if( CheckEdgePan( x, y, true, 5, 10 ) ) {
//                    return;
//                }
                
           }
        }
    }

    if( event.Dragging() ) {
        //in touch screen mode ensure the finger/cursor is on the selected point's radius to allow dragging
        if( g_btouch ) {
            if( m_pRoutePointEditTarget && !m_bIsInRadius ) {
                SelectItem *pFind = NULL;
                SelectableItemList SelList = pSelect->FindSelectionList( m_cursor_lat, m_cursor_lon,
                                                                        +                                 SELTYPE_ROUTEPOINT );
                wxSelectableItemListNode *node = SelList.GetFirst();
                while( node ) {
                    pFind = node->GetData();
                    RoutePoint *frp = (RoutePoint *) pFind->m_pData1;
                    if( m_pRoutePointEditTarget == frp ) m_bIsInRadius = true;
                    node = node->GetNext();
                }
            }
        }

        if( m_bRouteEditing && m_pRoutePointEditTarget ) {
            bool DraggingAllowed = g_btouch ? m_bIsInRadius : true;

            if( NULL == pMarkPropDialog ) {
                if( g_bWayPointPreventDragging ) DraggingAllowed = false;
            } else if( !pMarkPropDialog->IsShown() && g_bWayPointPreventDragging ) DraggingAllowed =
                    false;

            if( m_pRoutePointEditTarget && ( m_pRoutePointEditTarget->m_IconName == _T("mob") ) ) DraggingAllowed =
                    false;

            if( m_pRoutePointEditTarget->m_bIsInLayer ) DraggingAllowed = false;

            if( DraggingAllowed ) {

                if( !undo->InUndoableAction() ) {
                    undo->BeforeUndoableAction( Undo_MoveWaypoint, m_pRoutePointEditTarget,
                            Undo_NeedsCopy, m_pFoundPoint );
                }

                // Get the update rectangle for the union of the un-edited routes
                wxRect pre_rect;

                if( !g_bopengl && m_pEditRouteArray ) {
                    for( unsigned int ir = 0; ir < m_pEditRouteArray->GetCount(); ir++ ) {
                        Route *pr = (Route *) m_pEditRouteArray->Item( ir );
                        //      Need to validate route pointer
                        //      Route may be gone due to drgging close to ownship with
                        //      "Delete On Arrival" state set, as in the case of
                        //      navigating to an isolated waypoint on a temporary route
                        if( g_pRouteMan->IsRouteValid(pr) ) {
                            wxRect route_rect;
                            pr->CalculateDCRect( m_dc_route, &route_rect, VPoint );
                            pre_rect.Union( route_rect );
                        }
                    }
                }

                m_pRoutePointEditTarget->m_lat = m_cursor_lat;     // update the RoutePoint entry
                m_pRoutePointEditTarget->m_lon = m_cursor_lon;
                m_pFoundPoint->m_slat = m_cursor_lat;             // update the SelectList entry
                m_pFoundPoint->m_slon = m_cursor_lon;

                if( CheckEdgePan( x, y, true, 5, 2 ) ) {
                    double new_cursor_lat, new_cursor_lon;
                    GetCanvasPixPoint( x, y, new_cursor_lat, new_cursor_lon );
                    m_pRoutePointEditTarget->m_lat = new_cursor_lat;  // update the RoutePoint entry
                    m_pRoutePointEditTarget->m_lon = new_cursor_lon;
                    m_pFoundPoint->m_slat = new_cursor_lat;           // update the SelectList entry
                    m_pFoundPoint->m_slon = new_cursor_lon;
                }

                //    Update the MarkProperties Dialog, if currently shown
                if( ( NULL != pMarkPropDialog ) && ( pMarkPropDialog->IsShown() ) ) {
                    if( m_pRoutePointEditTarget == pMarkPropDialog->GetRoutePoint() ) pMarkPropDialog->UpdateProperties();
                }

                if(g_bopengl) {
                    InvalidateGL();
                    Refresh( false );
                } else {
                    // Get the update rectangle for the edited route
                    wxRect post_rect;

                    if( m_pEditRouteArray ) {
                        for( unsigned int ir = 0; ir < m_pEditRouteArray->GetCount(); ir++ ) {
                            Route *pr = (Route *) m_pEditRouteArray->Item( ir );
                            if( g_pRouteMan->IsRouteValid(pr) ) {
                                wxRect route_rect;
                                pr->CalculateDCRect( m_dc_route, &route_rect, VPoint );
                                post_rect.Union( route_rect );
                            }
                        }
                    }

                    //    Invalidate the union region
                    pre_rect.Union( post_rect );
                    RefreshRect( pre_rect, false );
                }
            }
        }     // if Route Editing

        else if( m_bMarkEditing && m_pRoutePointEditTarget ) {

            bool DraggingAllowed = g_btouch ? m_bIsInRadius : true;

            if( NULL == pMarkPropDialog ) {
                if( g_bWayPointPreventDragging ) DraggingAllowed = false;
            } else if( !pMarkPropDialog->IsShown() && g_bWayPointPreventDragging ) DraggingAllowed =
                    false;

            if( m_pRoutePointEditTarget
                    && ( m_pRoutePointEditTarget->m_IconName == _T("mob") ) ) DraggingAllowed =
                            false;

            if( m_pRoutePointEditTarget->m_bIsInLayer ) DraggingAllowed = false;

            if( DraggingAllowed ) {
                if( !undo->InUndoableAction() ) {
                    undo->BeforeUndoableAction( Undo_MoveWaypoint, m_pRoutePointEditTarget,
                            Undo_NeedsCopy, m_pFoundPoint );
                }

                //      The mark may be an anchorwatch
                double lpp1 = 0.;
                double lpp2 = 0.;
                double lppmax;

                if( pAnchorWatchPoint1 == m_pRoutePointEditTarget ) {
                    lpp1 = fabs( GetAnchorWatchRadiusPixels( pAnchorWatchPoint1 ) );

                }
                if( pAnchorWatchPoint2 == m_pRoutePointEditTarget ) {
                    lpp2 = fabs( GetAnchorWatchRadiusPixels( pAnchorWatchPoint2 ) );
                }
                lppmax = wxMax(lpp1 + 10, lpp2 + 10);         // allow for cruft

                // Get the update rectangle for the un-edited mark
                wxRect pre_rect;
                m_pRoutePointEditTarget->CalculateDCRect( m_dc_route, &pre_rect );
                if( ( lppmax > pre_rect.width / 2 ) || ( lppmax > pre_rect.height / 2 ) ) pre_rect.Inflate(
                        (int) ( lppmax - ( pre_rect.width / 2 ) ),
                        (int) ( lppmax - ( pre_rect.height / 2 ) ) );
                m_pRoutePointEditTarget->m_lat = m_cursor_lat;    // update the RoutePoint entry
                m_pRoutePointEditTarget->m_lon = m_cursor_lon;
                m_pFoundPoint->m_slat = m_cursor_lat;             // update the SelectList entry
                m_pFoundPoint->m_slon = m_cursor_lon;

                //    Update the MarkProperties Dialog, if currently shown
                if( ( NULL != pMarkPropDialog ) && ( pMarkPropDialog->IsShown() ) ) {
                    if( m_pRoutePointEditTarget == pMarkPropDialog->GetRoutePoint() )
                        pMarkPropDialog->UpdateProperties( true );
                }

                //    Invalidate the union region
                if(g_bopengl) {
                    InvalidateGL();
                    Refresh( false );
                } else {
                    // Get the update rectangle for the edited mark
                    wxRect post_rect;
                    m_pRoutePointEditTarget->CalculateDCRect( m_dc_route, &post_rect );
                    if( ( lppmax > post_rect.width / 2 ) || ( lppmax > post_rect.height / 2 ) )
                        post_rect.Inflate(
                            (int) ( lppmax - ( post_rect.width / 2 ) ),
                            (int) ( lppmax - ( post_rect.height / 2 ) ) );
                    
//                        post_rect.Inflate(200);
                    //    Invalidate the union region
                    pre_rect.Union( post_rect );
                    RefreshRect( pre_rect, false );
                }
            }
        }

        // must be chart dragging...
        else if( leftIsDown ) {
            if( ( last_drag.x != mx ) || ( last_drag.y != my ) ) {
                m_bChartDragging = true;
                PanCanvas( last_drag.x - mx, last_drag.y - my );

                last_drag.x = mx;
                last_drag.y = my;

                if( g_btouch ) {
                    if(( m_bMeasure_Active && m_nMeasureState ) || ( parent_frame->nRoute_State )){
                        //deactivate next LeftUp to ovoid creating an unexpected point
                        m_DoubleClickTimer->Start();
                        singleClickEventIsValid = false;
                    }
                }
            }
        }
    }

    if( event.LeftUp() ) {
        bool b_startedit_route = false;
        bool b_startedit_mark = false;

        if(g_btouch) {
            m_bChartDragging = false;
            m_bIsInRadius = false;
            
            if( parent_frame->nRoute_State )                  // creating route?
            {
                if(m_bedge_pan){
                    m_bedge_pan = false;
                    return;
                }
                
                double rlat, rlon;

                rlat = m_cursor_lat;
                rlon = m_cursor_lon;

                if( m_pRoutePointEditTarget) {
                    m_pRoutePointEditTarget->m_bIsBeingEdited = false;
                    m_pRoutePointEditTarget->m_bPtIsSelected = false;
                    wxRect wp_rect;
                    m_pRoutePointEditTarget->CalculateDCRect( m_dc_route, &wp_rect );
                    RefreshRect( wp_rect, true );
                    m_pRoutePointEditTarget = NULL;
                }
                m_bRouteEditing = true;

                if( parent_frame->nRoute_State == 1 ) {
                    m_pMouseRoute = new Route();
                    m_pMouseRoute->SetHiLite(50);
                    pRouteList->Append( m_pMouseRoute );
                    r_rband.x = x;
                    r_rband.y = y;
                }

                
                //    Check to see if there is a nearby point which may be reused
                RoutePoint *pMousePoint = NULL;

                //    Calculate meaningful SelectRadius
                int nearby_sel_rad_pix = 8;
                double nearby_radius_meters = nearby_sel_rad_pix / m_true_scale_ppm;

                RoutePoint *pNearbyPoint = pWayPointMan->GetNearbyWaypoint( rlat, rlon,
                                                                            nearby_radius_meters );
                if( pNearbyPoint && ( pNearbyPoint != m_prev_pMousePoint )
                    && !pNearbyPoint->m_bIsInTrack && !pNearbyPoint->m_bIsInLayer )
                {
                    int dlg_return;
#ifndef __WXOSX__
                    dlg_return = OCPNMessageBox( this, _("Use nearby waypoint?"),
                                                _("OpenCPN Route Create"),
                                                (long) wxYES_NO | wxCANCEL | wxYES_DEFAULT );
#else
                    dlg_return = wxID_YES;
#endif
                    if( dlg_return == wxID_YES ) {
                        pMousePoint = pNearbyPoint;

                    // Using existing waypoint, so nothing to delete for undo.
                    if( parent_frame->nRoute_State > 1 )
                        undo->BeforeUndoableAction( Undo_AppendWaypoint, pMousePoint, Undo_HasParent, NULL );

                    // check all other routes to see if this point appears in any other route
                    // If it appears in NO other route, then it should e considered an isolated mark
                    if( !g_pRouteMan->FindRouteContainingWaypoint( pMousePoint ) ) pMousePoint->m_bKeepXRoute =
                                                            true;
                    }
                }

                if( NULL == pMousePoint ) {                 // need a new point
                    pMousePoint = new RoutePoint( rlat, rlon, _T("diamond"), _T(""), GPX_EMPTY_STRING );
                    pMousePoint->SetNameShown( false );

                    pConfig->AddNewWayPoint( pMousePoint, -1 );    // use auto next num
                    pSelect->AddSelectableRoutePoint( rlat, rlon, pMousePoint );

                    if( parent_frame->nRoute_State > 1 )
                        undo->BeforeUndoableAction( Undo_AppendWaypoint, pMousePoint, Undo_IsOrphanded, NULL );
                }

                if( parent_frame->nRoute_State == 1 ) {
                    // First point in the route.
                    m_pMouseRoute->AddPoint( pMousePoint );
                } else {
                    if( m_pMouseRoute->m_NextLegGreatCircle ) {
                        double rhumbBearing, rhumbDist, gcBearing, gcDist;
                        DistanceBearingMercator( rlat, rlon, m_prev_rlat, m_prev_rlon, &rhumbBearing, &rhumbDist );
                        Geodesic::GreatCircleDistBear( m_prev_rlon, m_prev_rlat, rlon, rlat, &gcDist, &gcBearing, NULL );
                        double gcDistNM = gcDist / 1852.0;

                        // Empirically found expression to get reasonable route segments.
                        int segmentCount = (3.0 + (rhumbDist - gcDistNM)) / pow(rhumbDist-gcDistNM-1, 0.5 );

                        wxString msg;
                        msg << _("For this leg the Great Circle route is ")
                        << FormatDistanceAdaptive( rhumbDist - gcDistNM ) << _(" shorter than rhumbline.\n\n")
                        << _("Would you like include the Great Circle routing points for this leg?");

#ifndef __WXOSX__
                        int answer = OCPNMessageBox( this, msg, _("OpenCPN Route Create"), wxYES_NO | wxNO_DEFAULT );
#else
                        int answer = wxID_NO;
#endif

                        if( answer == wxID_YES ) {
                            RoutePoint* gcPoint;
                            RoutePoint* prevGcPoint = m_prev_pMousePoint;
                            wxRealPoint gcCoord;

                            for( int i = 1; i <= segmentCount; i++ ) {
                                double fraction = (double) i * ( 1.0 / (double) segmentCount );
                                Geodesic::GreatCircleTravel( m_prev_rlon, m_prev_rlat, gcDist * fraction,
                                                            gcBearing, &gcCoord.x, &gcCoord.y, NULL );

                                if( i < segmentCount ) {
                                    gcPoint = new RoutePoint( gcCoord.y, gcCoord.x, _T("xmblue"), _T(""),
                                                            GPX_EMPTY_STRING );
                                    gcPoint->SetNameShown( false );
                                    pConfig->AddNewWayPoint( gcPoint, -1 );
                                    pSelect->AddSelectableRoutePoint( gcCoord.y, gcCoord.x, gcPoint );
                                } else {
                                    gcPoint = pMousePoint; // Last point, previously exsisting!
                                }

                                m_pMouseRoute->AddPoint( gcPoint );
                                pSelect->AddSelectableRouteSegment( prevGcPoint->m_lat, prevGcPoint->m_lon,
                                                                    gcPoint->m_lat, gcPoint->m_lon, prevGcPoint, gcPoint, m_pMouseRoute );
                                prevGcPoint = gcPoint;
                            }

                            undo->CancelUndoableAction( true );

                        } else {
                            m_pMouseRoute->AddPoint( pMousePoint );
                            pSelect->AddSelectableRouteSegment( m_prev_rlat, m_prev_rlon,
                                                                rlat, rlon, m_prev_pMousePoint, pMousePoint, m_pMouseRoute );
                            undo->AfterUndoableAction( m_pMouseRoute );
                        }
                    } else {
                        // Ordinary rhumblinesegment.
                        m_pMouseRoute->AddPoint( pMousePoint );
                        pSelect->AddSelectableRouteSegment( m_prev_rlat, m_prev_rlon,
                                                            rlat, rlon, m_prev_pMousePoint, pMousePoint, m_pMouseRoute );
                        undo->AfterUndoableAction( m_pMouseRoute );
                    }
                }

                m_prev_rlat = rlat;
                m_prev_rlon = rlon;
                m_prev_pMousePoint = pMousePoint;
                m_pMouseRoute->m_lastMousePointIndex = m_pMouseRoute->GetnPoints();

                parent_frame->nRoute_State++;
                Refresh( true );
            }
            else if( m_bMeasure_Active && m_nMeasureState )   // measure tool?
            {
                if(m_bedge_pan){
                    m_bedge_pan = false;
                    return;
                }
                    
                double rlat, rlon;

                rlat = m_cursor_lat;
                rlon = m_cursor_lon;

                if( m_nMeasureState == 1 ) {
                    m_pMeasureRoute = new Route();
                    pRouteList->Append( m_pMeasureRoute );
                    r_rband.x = x;
                    r_rband.y = y;
                }

                
                RoutePoint *pMousePoint = new RoutePoint( m_cursor_lat, m_cursor_lon,
                                                        wxString( _T ( "circle" ) ), wxEmptyString, GPX_EMPTY_STRING );
                pMousePoint->m_bShowName = false;

                m_pMeasureRoute->AddPoint( pMousePoint );

                m_prev_rlat = m_cursor_lat;
                m_prev_rlon = m_cursor_lon;
                m_prev_pMousePoint = pMousePoint;
                m_pMeasureRoute->m_lastMousePointIndex = m_pMeasureRoute->GetnPoints();

                m_nMeasureState++;

                Refresh( true );
            }
            else {
                bool b_was_editing_mark = m_bMarkEditing;
                bool b_was_editing_route = m_bRouteEditing;
                FindRoutePointsAtCursor( SelectRadius, true );    // Possibly selecting a point in a route for later dragging
                
                if( !b_was_editing_route ) {
                    if( m_pEditRouteArray ) {
                        b_startedit_route = true;

                          
                        //  Hide the route rollover during route point edit, not needed, and may be confusing
                        if( m_pRouteRolloverWin && m_pRouteRolloverWin->IsActive()  ) {
                            m_pRouteRolloverWin->IsActive( false );
                        }
                        
                        wxRect pre_rect;
                        for( unsigned int ir = 0; ir < m_pEditRouteArray->GetCount(); ir++ ) {
                            Route *pr = (Route *) m_pEditRouteArray->Item( ir );
                            //      Need to validate route pointer
                            //      Route may be gone due to drgging close to ownship with
                            //      "Delete On Arrival" state set, as in the case of
                            //      navigating to an isolated waypoint on a temporary route
                            if( g_pRouteMan->IsRouteValid(pr) ) {
//                                pr->SetHiLite(50);
                                wxRect route_rect;
                                pr->CalculateDCRect( m_dc_route, &route_rect, VPoint );
                                pre_rect.Union( route_rect );
                            }
                        }
                        RefreshRect( pre_rect, true );
                    }
                }
                else {
                    b_startedit_route = false; 
                }
                

                //  Mark editing
                if( m_pRoutePointEditTarget ) {

                    if(b_was_editing_mark || b_was_editing_route) {            // kill previous hilight
                        if( m_lastRoutePointEditTarget) {
                            m_lastRoutePointEditTarget->m_bIsBeingEdited = false;
                            m_lastRoutePointEditTarget->m_bPtIsSelected = false;
                            wxRect wp_rect;
                            m_lastRoutePointEditTarget->CalculateDCRect( m_dc_route, &wp_rect );
                            RefreshRect( wp_rect, true );
                        }
                    }
                        
                    if( m_pRoutePointEditTarget) {
                        m_pRoutePointEditTarget->m_bIsBeingEdited = true;
                        m_pRoutePointEditTarget->m_bPtIsSelected = true;
                        wxRect wp_rect;
                        m_pRoutePointEditTarget->CalculateDCRect( m_dc_route, &wp_rect );
                        RefreshRect( wp_rect, true );
                    }
                    
                }
                else {
                    if( m_lastRoutePointEditTarget) {
                        m_lastRoutePointEditTarget->m_bIsBeingEdited = false;
                        m_lastRoutePointEditTarget->m_bPtIsSelected = false;
                        wxRect wp_rect;
                        m_lastRoutePointEditTarget->CalculateDCRect( m_dc_route, &wp_rect );
                        RefreshRect( wp_rect, true );
                    }
                }
            }
            
                //      Check to see if there is a route or AIS target under the cursor
                //      If so, start the rollover timer which creates the popup
                bool b_start_rollover = false;
                if( g_pAIS && g_pAIS->GetNumTargets() && g_bShowAIS ) {
                    SelectItem *pFind = pSelectAIS->FindSelection( m_cursor_lat, m_cursor_lon,
                                                                   SELTYPE_AISTARGET );
                    if( pFind )
                        b_start_rollover = true;
                }
                
                if(!b_start_rollover && !b_startedit_route){
                    SelectableItemList SelList = pSelect->FindSelectionList( m_cursor_lat, m_cursor_lon,
                                                                             SELTYPE_ROUTESEGMENT );
                    wxSelectableItemListNode *node = SelList.GetFirst();
                    while( node ) {
                        SelectItem *pFindSel = node->GetData();
                        
                        Route *pr = (Route *) pFindSel->m_pData3;        //candidate
                        
                        if( pr && pr->IsVisible() ){
                            b_start_rollover = true;
                            break;
                        }
                        node = node->GetNext();
                    }       // while
                }
                
                if( b_start_rollover )
                    m_RolloverPopupTimer.Start( m_rollover_popup_timer_msec, wxTIMER_ONE_SHOT );
                
                
                SelectItem *pFindCurrent = NULL;
                SelectItem *pFindTide = NULL;
                    
                if( m_bShowCurrent )
                        pFindCurrent = pSelectTC->FindSelection( m_cursor_lat, m_cursor_lon, SELTYPE_CURRENTPOINT );
                    
                if( m_bShowTide )                                // look for tide stations
                        pFindTide = pSelectTC->FindSelection( m_cursor_lat, m_cursor_lon, SELTYPE_TIDEPOINT );
                    
                if( pFindCurrent ) {
                        // There may be multiple current entries at the same point.
                        // For example, there often is a current substation (with directions specified)
                        // co-located with its master.  We want to select the substation, so that
                        // the direction will be properly indicated on the graphic.
                        // So, we search the select list looking for IDX_type == 'c' (i.e substation)
                        IDX_entry *pIDX_best_candidate;
                        
                        SelectItem *pFind = NULL;
                        SelectableItemList SelList = pSelectTC->FindSelectionList( m_cursor_lat,
                                                                                   m_cursor_lon, SELTYPE_CURRENTPOINT );
                        
                        //      Default is first entry
                        wxSelectableItemListNode *node = SelList.GetFirst();
                        pFind = node->GetData();
                        pIDX_best_candidate = (IDX_entry *) ( pFind->m_pData1 );
                        
                        if( SelList.GetCount() > 1 ) {
                            node = node->GetNext();
                            while( node ) {
                                pFind = node->GetData();
                                IDX_entry *pIDX_candidate = (IDX_entry *) ( pFind->m_pData1 );
                                if( pIDX_candidate->IDX_type == 'c' ) {
                                    pIDX_best_candidate = pIDX_candidate;
                                    break;
                                }
                                
                                node = node->GetNext();
                            }       // while (node)
                        } else {
                            wxSelectableItemListNode *node = SelList.GetFirst();
                            pFind = node->GetData();
                            pIDX_best_candidate = (IDX_entry *) ( pFind->m_pData1 );
                        }
                        
                        m_pIDXCandidate = pIDX_best_candidate;
                        
                        DrawTCWindow( x, y, (void *) pIDX_best_candidate );
                        Refresh( false );
                }
                    
                else if( pFindTide ) {
                        m_pIDXCandidate = (IDX_entry *) pFindTide->m_pData1;
                        
                        DrawTCWindow( x, y, (void *) pFindTide->m_pData1 );
                        Refresh( false );
                }

        if( m_bRouteEditing/* && !b_startedit_route*/) {            // End of RoutePoint drag
            if( m_pRoutePointEditTarget ) {
                pSelect->UpdateSelectableRouteSegments( m_pRoutePointEditTarget );
                
                if( m_pEditRouteArray ) {
                    for( unsigned int ir = 0; ir < m_pEditRouteArray->GetCount(); ir++ ) {
                        Route *pr = (Route *) m_pEditRouteArray->Item( ir );
                        if( g_pRouteMan->IsRouteValid(pr) ) {
                            pr->FinalizeForRendering();
                            pr->UpdateSegmentDistances();
                            pConfig->UpdateRoute( pr );
                        }
                    }
                }

                //    Update the RouteProperties Dialog, if currently shown
                if( ( NULL != pRoutePropDialog ) && ( pRoutePropDialog->IsShown() ) ) {
                    if( m_pEditRouteArray ) {
                        for( unsigned int ir = 0; ir < m_pEditRouteArray->GetCount(); ir++ ) {
                            Route *pr = (Route *) m_pEditRouteArray->Item( ir );
                            if( g_pRouteMan->IsRouteValid(pr) ) {
                                if( !pr->IsTrack() && pRoutePropDialog->m_pRoute == pr ) {
                                    pRoutePropDialog->SetRouteAndUpdate( pr, true );
                                } else if ( ( NULL != pTrackPropDialog ) && ( pTrackPropDialog->IsShown() ) && pTrackPropDialog->m_pRoute == pr ) {
                                    pTrackPropDialog->SetTrackAndUpdate( pr );
                                }
                            }
                        }
                    }
                }

            }
        }

        if( m_pRoutePointEditTarget ) {
            pConfig->UpdateWayPoint( m_pRoutePointEditTarget );
            undo->AfterUndoableAction( m_pRoutePointEditTarget );
        }
        
        if(!m_pRoutePointEditTarget){
            delete m_pEditRouteArray;
            m_pEditRouteArray = NULL;
            m_bRouteEditing = false;
        }
            
#if 0        
        else if( m_bMarkEditing && !b_startedit_mark) {         // end of Waypoint drag
            if( m_pRoutePointEditTarget ) {
                pConfig->UpdateWayPoint( m_pRoutePointEditTarget );
                undo->AfterUndoableAction( m_pRoutePointEditTarget );
//                m_pRoutePointEditTarget->m_bIsBeingEdited = false;
//                wxRect wp_rect;
//                m_pRoutePointEditTarget->CalculateDCRect( m_dc_route, &wp_rect );
//                m_pRoutePointEditTarget->m_bPtIsSelected = false;
//                RefreshRect( wp_rect, true );
                
            }
//            m_pRoutePointEditTarget = NULL;
//            m_bMarkEditing = false;
            if( !g_FloatingToolbarDialog->IsShown() )
                gFrame->SurfaceToolbar();
        }
#endif
        }       // g_btouch


        else{                   // !g_btouch
        if( m_bRouteEditing ) {            // End of RoutePoint drag
            if( m_pRoutePointEditTarget ) {
                pSelect->UpdateSelectableRouteSegments( m_pRoutePointEditTarget );
                m_pRoutePointEditTarget->m_bBlink = false;
                
                if( m_pEditRouteArray ) {
                    for( unsigned int ir = 0; ir < m_pEditRouteArray->GetCount(); ir++ ) {
                        Route *pr = (Route *) m_pEditRouteArray->Item( ir );
                        if( g_pRouteMan->IsRouteValid(pr) ) {
                            pr->FinalizeForRendering();
                            pr->UpdateSegmentDistances();
                            pr->m_bIsBeingEdited = false;

                            pConfig->UpdateRoute( pr );
                            
                            pr->SetHiLite( 0 );
                        }
                    }
                    Refresh( false );
                }

                //    Update the RouteProperties Dialog, if currently shown
                if( ( NULL != pRoutePropDialog ) && ( pRoutePropDialog->IsShown() ) ) {
                    if( m_pEditRouteArray ) {
                        for( unsigned int ir = 0; ir < m_pEditRouteArray->GetCount(); ir++ ) {
                            Route *pr = (Route *) m_pEditRouteArray->Item( ir );
                            if( g_pRouteMan->IsRouteValid(pr) ) {
                                if( !pr->IsTrack() && pRoutePropDialog->m_pRoute == pr ) {
                                    pRoutePropDialog->SetRouteAndUpdate( pr, true );
                                } else if ( ( NULL != pTrackPropDialog ) && ( pTrackPropDialog->IsShown() ) && pTrackPropDialog->m_pRoute == pr ) {
                                    pTrackPropDialog->SetTrackAndUpdate( pr );
                                }
                            }
                        }
                    }
                }

                m_pRoutePointEditTarget->m_bPtIsSelected = false;
                m_pRoutePointEditTarget->m_bIsBeingEdited = false;
                
                delete m_pEditRouteArray;
                m_pEditRouteArray = NULL;
                undo->AfterUndoableAction( m_pRoutePointEditTarget );
            }

            InvalidateGL();
            m_bRouteEditing = false;
            m_pRoutePointEditTarget = NULL;
            if( !g_FloatingToolbarDialog->IsShown() ) gFrame->SurfaceToolbar();
        }

        else if( m_bMarkEditing) {         // end of Waypoint drag
            if( m_pRoutePointEditTarget ) {
                pConfig->UpdateWayPoint( m_pRoutePointEditTarget );
                undo->AfterUndoableAction( m_pRoutePointEditTarget );
                m_pRoutePointEditTarget->m_bIsBeingEdited = false;
                wxRect wp_rect;
                m_pRoutePointEditTarget->CalculateDCRect( m_dc_route, &wp_rect );
                m_pRoutePointEditTarget->m_bPtIsSelected = false;
                RefreshRect( wp_rect, true );
                
            }
            m_pRoutePointEditTarget = NULL;
            m_bMarkEditing = false;
            if( !g_FloatingToolbarDialog->IsShown() )
                gFrame->SurfaceToolbar();
        }

        else if( leftIsDown ) {  // left click for chart center
            leftIsDown = false;

            if( !g_btouch ){
                if( !m_bChartDragging && !m_bMeasure_Active ) {
                    switch( cursor_region ){
                        case MID_RIGHT: {
                            PanCanvas( 100, 0 );
                            break;
                        }

                        case MID_LEFT: {
                            PanCanvas( -100, 0 );
                            break;
                        }

                        case MID_TOP: {
                            PanCanvas( 0, 100 );
                            break;
                        }

                        case MID_BOT: {
                            PanCanvas( 0, -100 );
                            break;
                        }

                        case CENTER: {
                            PanCanvas( x - GetVP().pix_width / 2, y - GetVP().pix_height / 2 );
                            break;
                        }
                    }
                } else {
                    m_bChartDragging = false;
                }
            }
        }
        }       // !btouch
    }

    if( event.RightDown() ) {
        last_drag.x = mx;
        last_drag.y = my;
        
        if(g_btouch ){
            if( m_pRoutePointEditTarget )
                return;
        }
            

        if( parent_frame->nRoute_State )                     // creating route?
            CanvasPopupMenu( x, y, SELTYPE_ROUTECREATE );
        else                                                  // General Right Click
        {
            // Look for selectable objects
            double slat, slon;
            slat = m_cursor_lat;
            slon = m_cursor_lon;
//                      SelectItem *pFind;
 //           wxClientDC cdc( this );
//            ocpnDC dc( cdc );
#ifdef __WXMAC__
            wxScreenDC sdc;
            ocpnDC dc( sdc );
#else
            wxClientDC cdc( GetParent() );
            ocpnDC dc( cdc );
#endif

            SelectItem *pFindAIS;
            SelectItem *pFindRP;
            SelectItem *pFindRouteSeg;
            SelectItem *pFindTrackSeg;
            SelectItem *pFindCurrent = NULL;
            SelectItem *pFindTide = NULL;

            //    Deselect any current objects
            if( m_pSelectedRoute ) {
                m_pSelectedRoute->m_bRtIsSelected = false;        // Only one selection at a time
                m_pSelectedRoute->DeSelectRoute();
                m_pSelectedRoute->Draw( dc, VPoint );
            }

            if( m_pFoundRoutePoint ) {
                m_pFoundRoutePoint->m_bPtIsSelected = false;
                m_pFoundRoutePoint->Draw( dc );
                RefreshRect( m_pFoundRoutePoint->CurrentRect_in_DC );
            }

            //      Get all the selectable things at the cursor
            pFindAIS = pSelectAIS->FindSelection( slat, slon, SELTYPE_AISTARGET );
            pFindRP = pSelect->FindSelection( slat, slon, SELTYPE_ROUTEPOINT );
            pFindRouteSeg = pSelect->FindSelection( slat, slon, SELTYPE_ROUTESEGMENT );
            pFindTrackSeg = pSelect->FindSelection( slat, slon, SELTYPE_TRACKSEGMENT );

            if( m_bShowCurrent ) pFindCurrent = pSelectTC->FindSelection( slat, slon,
                                                    SELTYPE_CURRENTPOINT );

            if( m_bShowTide )                                // look for tide stations
                pFindTide = pSelectTC->FindSelection( slat, slon, SELTYPE_TIDEPOINT );

            int seltype = 0;

            //    Try for AIS targets first
            if( pFindAIS ) {
                m_FoundAIS_MMSI = pFindAIS->GetUserData();

                //      Make sure the target data is available
                if( g_pAIS->Get_Target_Data_From_MMSI( m_FoundAIS_MMSI ) ) seltype |=
                        SELTYPE_AISTARGET;
            }

            //    Now the various Route Parts

            m_pFoundRoutePoint = NULL;
            if( pFindRP ) {
                RoutePoint *pFirstVizPoint = NULL;
                RoutePoint *pFoundActiveRoutePoint = NULL;
                RoutePoint *pFoundVizRoutePoint = NULL;
                Route *pSelectedActiveRoute = NULL;
                Route *pSelectedVizRoute = NULL;

                //There is at least one routepoint, so get the whole list
                SelectableItemList SelList = pSelect->FindSelectionList( slat, slon,
                                             SELTYPE_ROUTEPOINT );
                wxSelectableItemListNode *node = SelList.GetFirst();
                while( node ) {
                    SelectItem *pFindSel = node->GetData();

                    RoutePoint *prp = (RoutePoint *) pFindSel->m_pData1;        //candidate

                    //    Get an array of all routes using this point
                    wxArrayPtrVoid *proute_array = g_pRouteMan->GetRouteArrayContaining( prp );

                    // Use route array (if any) to determine actual visibility for this point
                    bool brp_viz = false;
                    if( proute_array ) {
                        for( unsigned int ir = 0; ir < proute_array->GetCount(); ir++ ) {
                            Route *pr = (Route *) proute_array->Item( ir );
                            if( pr->IsVisible() ) {
                                brp_viz = true;
                                break;
                            }
                        }
                        if( !brp_viz )                          // is not visible as part of route
                            brp_viz = prp->IsVisible();         //  so treat as isolated point

                    } else
                        brp_viz = prp->IsVisible();               // isolated point

                    if( ( NULL == pFirstVizPoint ) && brp_viz ) pFirstVizPoint = prp;

                    // Use route array to choose the appropriate route
                    // Give preference to any active route, otherwise select the first visible route in the array for this point
                    m_pSelectedRoute = NULL;
                    if( proute_array ) {
                        for( unsigned int ir = 0; ir < proute_array->GetCount(); ir++ ) {
                            Route *pr = (Route *) proute_array->Item( ir );
                            if( pr->m_bRtIsActive ) {
                                pSelectedActiveRoute = pr;
                                pFoundActiveRoutePoint = prp;
                                break;
                            }
                        }

                        if( NULL == pSelectedVizRoute ) {
                            for( unsigned int ir = 0; ir < proute_array->GetCount(); ir++ ) {
                                Route *pr = (Route *) proute_array->Item( ir );
                                if( pr->IsVisible() ) {
                                    pSelectedVizRoute = pr;
                                    pFoundVizRoutePoint = prp;
                                    break;
                                }
                            }
                        }

                        delete proute_array;
                    }

                    node = node->GetNext();
                }

                //      Now choose the "best" selections
                if( pFoundActiveRoutePoint ) {
                    m_pFoundRoutePoint = pFoundActiveRoutePoint;
                    m_pSelectedRoute = pSelectedActiveRoute;
                } else if( pFoundVizRoutePoint ) {
                    m_pFoundRoutePoint = pFoundVizRoutePoint;
                    m_pSelectedRoute = pSelectedVizRoute;
                } else
                    // default is first visible point in list
                    m_pFoundRoutePoint = pFirstVizPoint;

                if( m_pSelectedRoute ) {
                    if( m_pSelectedRoute->IsVisible() ) seltype |= SELTYPE_ROUTEPOINT;
                } else if( m_pFoundRoutePoint ) seltype |= SELTYPE_MARKPOINT;
            }

            // Note here that we use SELTYPE_ROUTESEGMENT to select tracks as well as routes
            // But call the popup handler with identifier appropriate to the type
            if( pFindRouteSeg )                  // there is at least one select item
            {
                SelectableItemList SelList = pSelect->FindSelectionList( slat, slon,
                                             SELTYPE_ROUTESEGMENT );

                if( NULL == m_pSelectedRoute )  // the case where a segment only is selected
                {
                    //  Choose the first visible route containing segment in the list
                    wxSelectableItemListNode *node = SelList.GetFirst();
                    while( node ) {
                        SelectItem *pFindSel = node->GetData();

                        Route *pr = (Route *) pFindSel->m_pData3;
                        if( pr->IsVisible() ) {
                            m_pSelectedRoute = pr;
                            break;
                        }
                        node = node->GetNext();
                    }
                }

                if( m_pSelectedRoute ) {
                    if( NULL == m_pFoundRoutePoint ) m_pFoundRoutePoint =
                            (RoutePoint *) pFindRouteSeg->m_pData1;
                    m_pFoundRoutePointSecond = (RoutePoint *) pFindRouteSeg->m_pData2;

                    m_pSelectedRoute->m_bRtIsSelected = !(seltype & SELTYPE_ROUTEPOINT);
                    if( m_pSelectedRoute->m_bRtIsSelected )
                        m_pSelectedRoute->Draw( dc, GetVP() );
                    seltype |= SELTYPE_ROUTESEGMENT;
                }

            }

            if( pFindTrackSeg ) {
                m_pSelectedTrack = NULL;
                SelectableItemList SelList = pSelect->FindSelectionList( slat, slon,
                                             SELTYPE_TRACKSEGMENT );

                //  Choose the first visible track containing segment in the list
                wxSelectableItemListNode *node = SelList.GetFirst();
                while( node ) {
                    SelectItem *pFindSel = node->GetData();

                    Route *pt = (Route *) pFindSel->m_pData3;
                    if( pt->IsVisible() ) {
                        m_pSelectedTrack = pt;
                        break;
                    }
                    node = node->GetNext();
                }

                if( m_pSelectedTrack ) seltype |= SELTYPE_TRACKSEGMENT;
            }

            bool bseltc = false;
//                      if(0 == seltype)
            {
                if( pFindCurrent ) {
                    // There may be multiple current entries at the same point.
                    // For example, there often is a current substation (with directions specified)
                    // co-located with its master.  We want to select the substation, so that
                    // the direction will be properly indicated on the graphic.
                    // So, we search the select list looking for IDX_type == 'c' (i.e substation)
                    IDX_entry *pIDX_best_candidate;

                    SelectItem *pFind = NULL;
                    SelectableItemList SelList = pSelectTC->FindSelectionList( m_cursor_lat,
                                                 m_cursor_lon, SELTYPE_CURRENTPOINT );

                    //      Default is first entry
                    wxSelectableItemListNode *node = SelList.GetFirst();
                    pFind = node->GetData();
                    pIDX_best_candidate = (IDX_entry *) ( pFind->m_pData1 );

                    if( SelList.GetCount() > 1 ) {
                        node = node->GetNext();
                        while( node ) {
                            pFind = node->GetData();
                            IDX_entry *pIDX_candidate = (IDX_entry *) ( pFind->m_pData1 );
                            if( pIDX_candidate->IDX_type == 'c' ) {
                                pIDX_best_candidate = pIDX_candidate;
                                break;
                            }

                            node = node->GetNext();
                        }       // while (node)
                    } else {
                        wxSelectableItemListNode *node = SelList.GetFirst();
                        pFind = node->GetData();
                        pIDX_best_candidate = (IDX_entry *) ( pFind->m_pData1 );
                    }

                    m_pIDXCandidate = pIDX_best_candidate;

                    if( 0 == seltype ) {
                        DrawTCWindow( x, y, (void *) pIDX_best_candidate );
                        Refresh( false );
                        bseltc = true;
                    } else
                        seltype |= SELTYPE_CURRENTPOINT;
                }

                else if( pFindTide ) {
                    m_pIDXCandidate = (IDX_entry *) pFindTide->m_pData1;

                    if( 0 == seltype ) {
                        DrawTCWindow( x, y, (void *) pFindTide->m_pData1 );
                        Refresh( false );
                        bseltc = true;
                    } else
                        seltype |= SELTYPE_TIDEPOINT;
                }
            }

            if( 0 == seltype ) seltype |= SELTYPE_UNKNOWN;

            if( !bseltc ) CanvasPopupMenu( x, y, seltype );

            // Seth: Is this refresh needed?
            Refresh( false );            // needed for MSW, not GTK  Why??
        }
    }

//    Switch to the appropriate cursor on mouse movement

    wxCursor *ptarget_cursor = pCursorArrow;

    if( ( !parent_frame->nRoute_State )
            && ( !m_bMeasure_Active ) /*&& ( !m_bCM93MeasureOffset_Active )*/) {

        if( x > xr_margin ) {
            ptarget_cursor = pCursorRight;
            cursor_region = MID_RIGHT;
        } else if( x < xl_margin ) {
            ptarget_cursor = pCursorLeft;
            cursor_region = MID_LEFT;
        } else if( y > yb_margin ) {
            ptarget_cursor = pCursorDown;
            cursor_region = MID_TOP;
        } else if( y < yt_margin ) {
            ptarget_cursor = pCursorUp;
            cursor_region = MID_BOT;
        } else {
            ptarget_cursor = pCursorArrow;
            cursor_region = CENTER;
        }
    } else if( m_bMeasure_Active || parent_frame->nRoute_State ) // If Measure tool use Pencil Cursor
        ptarget_cursor = pCursorPencil;

    SetCursor( *ptarget_cursor );

}

void ChartCanvas::LostMouseCapture( wxMouseCaptureLostEvent& event )
{
    SetCursor( *pCursorArrow );
}

//-------------------------------------------------------------------------------
//          Popup Menu Handling
//-------------------------------------------------------------------------------

wxString _menuText( wxString name, wxString shortcut ) {
    wxString menutext;
    menutext << name << _T("\t") << shortcut;
    return menutext;
}

void MenuPrepend( wxMenu *menu, int id, wxString label)
{
    wxMenuItem *item = new wxMenuItem(menu, id, label);
#ifdef __WXMSW__
    wxFont *qFont = GetOCPNScaledFont(_T("Menu"), 10);
    item->SetFont(*qFont);
#endif
    menu->Prepend(item);
}

void MenuAppend( wxMenu *menu, int id, wxString label)
{
    wxMenuItem *item = new wxMenuItem(menu, id, label);
#ifdef __WXMSW__
    wxFont *qFont = GetOCPNScaledFont(_("Menu"), 10);
    item->SetFont(*qFont);
#endif
    menu->Append(item);
}

void SetMenuItemFont(wxMenuItem *item)
{
#ifdef __WXMSW__
    wxFont *qFont = GetOCPNScaledFont(_("Menu"), 10);
    item->SetFont(*qFont);
#endif
}

void ChartCanvas::CanvasPopupMenu( int x, int y, int seltype )
{
    wxMenu* contextMenu = new wxMenu;
    wxMenu* menuWaypoint = new wxMenu( _("Waypoint") );
    wxMenu* menuRoute = new wxMenu( _("Route") );
    wxMenu* menuTrack = new wxMenu( _("Track") );
    wxMenu* menuAIS = new wxMenu( _("AIS") );

    wxMenu *subMenuChart = new wxMenu;

    wxMenu *menuFocus = contextMenu;    // This is the one that will be shown

    popx = x;
    popy = y;

#ifdef __WXGTK__
#ifdef ocpnUSE_GTK_OPTIMIZE
    //  This code changes the background color on the popup context menu
    wxColour back_color = GetGlobalColor(_T("UIBCK"));
    GdkColor color;

    color.red = back_color.Red() << 8;
    color.green = back_color.Green() << 8;
    color.blue = back_color.Blue() << 8;

//    gtk_widget_modify_bg (GTK_WIDGET(contextMenu->m_menu), GTK_STATE_NORMAL, &color);
#endif
#endif

    if( seltype == SELTYPE_ROUTECREATE ) {
#ifndef __WXOSX__
        MenuAppend( contextMenu, ID_RC_MENU_FINISH, _menuText( _( "End Route" ), _T("Esc") ) );
#else
        MenuAppend( contextMenu, ID_RC_MENU_FINISH,  _( "End Route" ) );
#endif
    }

    if( ! m_pMouseRoute ) {
        if( m_bMeasure_Active )
#ifndef __WXOSX__
            MenuPrepend( contextMenu, ID_DEF_MENU_DEACTIVATE_MEASURE, _menuText( _("Measure Off"), _T("Esc") ) );
#else
            MenuPrepend( contextMenu, ID_DEF_MENU_DEACTIVATE_MEASURE,  _("Measure Off") );
#endif
        else
#ifdef __WXOSX__
            MenuPrepend( contextMenu, ID_DEF_MENU_ACTIVATE_MEASURE, _( "Measure" ) + _T("     (fn-F4)") );
#else
            MenuPrepend( contextMenu, ID_DEF_MENU_ACTIVATE_MEASURE, _menuText( _( "Measure" ), _T("F4") ) );
#endif
//            contextMenu->Prepend( ID_DEF_MENU_ACTIVATE_MEASURE, _menuText( _( "Measure" ), _T("F4") ) );
    }

    if( undo->AnythingToUndo() ) {
        wxString undoItem;
        undoItem << _("Undo") << _T(" ") << undo->GetNextUndoableAction()->Description();
        MenuPrepend( contextMenu, ID_UNDO, _menuText( undoItem, _T("Ctrl-Z") ) );
    }

    if( undo->AnythingToRedo() ) {
        wxString redoItem;
        redoItem << _("Redo") << _T(" ") << undo->GetNextRedoableAction()->Description();
        MenuPrepend( contextMenu, ID_REDO, _menuText( redoItem, _T("Ctrl-Y") ) );
    }

    bool ais_areanotice = false;
    if( g_pAIS && g_bShowAIS && g_bShowAreaNotices ) {

        AIS_Target_Hash* an_sources = g_pAIS->GetAreaNoticeSourcesList();

        float vp_scale = GetVPScale();

        for( AIS_Target_Hash::iterator target = an_sources->begin(); target != an_sources->end(); ++target ) {
            AIS_Target_Data* target_data = target->second;
            if( !target_data->area_notices.empty() ) {
                for( AIS_Area_Notice_Hash::iterator ani = target_data->area_notices.begin(); ani != target_data->area_notices.end(); ++ani ) {
                    Ais8_001_22& area_notice = ani->second;

                    wxBoundingBox bbox;

                    for( Ais8_001_22_SubAreaList::iterator sa = area_notice.sub_areas.begin(); sa != area_notice.sub_areas.end(); ++sa ) {
                        switch( sa->shape ) {
                            case AIS8_001_22_SHAPE_CIRCLE: {
                                wxPoint target_point;
                                GetCanvasPointPix( sa->latitude, sa->longitude, &target_point );
                                bbox.Expand( target_point );
                                if( sa->radius_m > 0.0 )
                                    bbox.EnLarge( sa->radius_m * vp_scale );
                                break;
                            }
                            case AIS8_001_22_SHAPE_POLYGON:
                            case AIS8_001_22_SHAPE_POLYLINE: {
                                double lat = sa->latitude;
                                double lon = sa->longitude;
                                for( int i = 0; i < 4; ++i ) {
                                    ll_gc_ll( lat, lon, sa->angles[i], sa->dists_m[i] / 1852.0,
                                              &lat, &lon );
                                    wxPoint target_point;
                                    GetCanvasPointPix( lat, lon, &target_point );
                                    bbox.Expand( target_point );
                                }
                            }
                        }
                    }

                    if( bbox.PointInBox( x, y ) ) {
                        ais_areanotice = true;
                        break;
                    }
                }
            }
        }
    }
    if( !VPoint.b_quilt ) {
        if( parent_frame->GetnChartStack() > 1 ) {
            MenuAppend( contextMenu, ID_DEF_MENU_MAX_DETAIL, _( "Max Detail Here" ) );
            MenuAppend( contextMenu, ID_DEF_MENU_SCALE_IN, _menuText( _( "Scale In" ), _T("F7") ) );
            MenuAppend( contextMenu, ID_DEF_MENU_SCALE_OUT, _menuText( _( "Scale Out" ), _T("F8") ) );
        }

        if( ( Current_Ch && ( Current_Ch->GetChartFamily() == CHART_FAMILY_VECTOR ) ) || ais_areanotice ) {
            MenuAppend( contextMenu, ID_DEF_MENU_QUERY, _( "Object Query..." ) );
        }

    } else {
        ChartBase *pChartTest = m_pQuilt->GetChartAtPix( wxPoint( x, y ) );
        if( ( pChartTest && ( pChartTest->GetChartFamily() == CHART_FAMILY_VECTOR ) ) || ais_areanotice ) {
            MenuAppend( contextMenu, ID_DEF_MENU_QUERY, _( "Object Query..." ) );
        } else {
            if( parent_frame->GetnChartStack() > 1 ) {
                MenuAppend( contextMenu, ID_DEF_MENU_SCALE_IN, _menuText( _( "Scale In" ), _T("F7") ) );
                MenuAppend( contextMenu, ID_DEF_MENU_SCALE_OUT, _menuText( _( "Scale Out" ), _T("F8") ) );
            }
        }
    }

    MenuAppend( contextMenu, ID_DEF_MENU_DROP_WP, _menuText( _( "Drop Mark" ), _T("Ctrl-M") ) );

    if( !bGPSValid ) MenuAppend( contextMenu, ID_DEF_MENU_MOVE_BOAT_HERE, _( "Move Boat Here" ) );

    if( !( g_pRouteMan->GetpActiveRoute() || ( seltype & SELTYPE_MARKPOINT ) ) )
        MenuAppend( contextMenu, ID_DEF_MENU_GOTO_HERE, _( "Navigate To Here" ) );

    MenuAppend( contextMenu, ID_DEF_MENU_GOTOPOSITION, _("Center View...") );

    if( !g_bCourseUp ) MenuAppend( contextMenu, ID_DEF_MENU_COGUP, _("Course Up Mode") );
    else {
        if( !VPoint.b_quilt && Current_Ch && ( fabs( Current_Ch->GetChartSkew() ) > .01 )
            && !g_bskew_comp ) MenuAppend( contextMenu, ID_DEF_MENU_NORTHUP, _("Chart Up Mode") );
        else
            MenuAppend( contextMenu, ID_DEF_MENU_NORTHUP, _("North Up Mode") );
    }

    Kml* kml = new Kml;
    int pasteBuffer = kml->ParsePasteBuffer();
    if( pasteBuffer != KML_PASTE_INVALID ) {
        switch( pasteBuffer ) {
            case KML_PASTE_WAYPOINT: {
                MenuAppend( contextMenu, ID_PASTE_WAYPOINT, _( "Paste Waypoint" ) );
                break;
            }
            case KML_PASTE_ROUTE: {
                MenuAppend( contextMenu, ID_PASTE_ROUTE, _( "Paste Route" ) );
                break;
            }
            case KML_PASTE_TRACK: {
                MenuAppend( contextMenu, ID_PASTE_TRACK, _( "Paste Track" ) );
                break;
            }
            case KML_PASTE_ROUTE_TRACK: {
                MenuAppend( contextMenu, ID_PASTE_ROUTE, _( "Paste Route" ) );
                MenuAppend( contextMenu, ID_PASTE_TRACK, _( "Paste Track" ) );
                break;
            }
        }
    }
    delete kml;

    if( !VPoint.b_quilt && Current_Ch && ( Current_Ch->GetChartType() == CHART_TYPE_CM93COMP ) ) {
        MenuAppend( contextMenu, ID_DEF_MENU_CM93OFFSET_DIALOG, _( "CM93 Offset Dialog..." ) );
    }

    if( ( VPoint.b_quilt ) && ( pCurrentStack && pCurrentStack->b_valid ) ) {
        int dbIndex = m_pQuilt->GetChartdbIndexAtPix( wxPoint( popx, popy ) );
        if( dbIndex != -1 )
            MenuAppend( contextMenu, ID_DEF_MENU_QUILTREMOVE, _( "Hide This Chart" ) );
    }

#ifdef __WXMSW__
    //  If we dismiss the context menu without action, we need to discard some mouse events....
    //  Eat the next 2 button events, which happen as down-up on MSW XP
    g_click_stop = 2;
#endif

    //  ChartGroup SubMenu
    wxMenuItem* subItemChart = contextMenu->AppendSubMenu( subMenuChart, _("Chart Groups") );
    SetMenuItemFont(subItemChart);
    
    if( g_pGroupArray->GetCount() ) {
        wxMenuItem* subItem0 = subMenuChart->AppendRadioItem( ID_DEF_MENU_GROUPBASE, _("All Active Charts") );
        SetMenuItemFont(subItem0);
        
        for( unsigned int i = 0; i < g_pGroupArray->GetCount(); i++ ) {
            subMenuChart->AppendRadioItem( ID_DEF_MENU_GROUPBASE + i + 1,
                                         g_pGroupArray->Item( i )->m_group_name );
            Connect( ID_DEF_MENU_GROUPBASE + i + 1, wxEVT_COMMAND_MENU_SELECTED,
                     (wxObjectEventFunction) (wxEventFunction) &ChartCanvas::PopupMenuHandler );
        }

        subMenuChart->Check( ID_DEF_MENU_GROUPBASE + g_GroupIndex, true );
    }

    //  Add PlugIn Context Menu items
    ArrayOfPlugInMenuItems item_array = g_pi_manager->GetPluginContextMenuItemArray();

    for( unsigned int i = 0; i < item_array.GetCount(); i++ ) {
        PlugInMenuItemContainer *pimis = item_array.Item( i );
        {
            if( pimis->b_viz ) {
                wxMenuItem *pmi = new wxMenuItem( contextMenu, pimis->id,
#ifdef __WXOSX__
                                                 pimis->pmenu_item->GetItemLabel(), pimis->pmenu_item->GetHelp(),
#else
                                                  pimis->pmenu_item->GetLabel(), pimis->pmenu_item->GetHelp(),
#endif
                                                  pimis->pmenu_item->GetKind(), pimis->pmenu_item->GetSubMenu() );
#ifdef __WXMSW__
                pmi->SetFont(pimis->pmenu_item->GetFont());
#endif
                contextMenu->Append( pmi );
                contextMenu->Enable( pimis->id, !pimis->b_grey );

                Connect( pimis->id, wxEVT_COMMAND_MENU_SELECTED,
                         (wxObjectEventFunction) (wxEventFunction) &ChartCanvas::PopupMenuHandler );
            }
        }
    }

    //  This is the default context menu
    menuFocus = contextMenu;

    if( g_pAIS ) {
        MenuAppend( contextMenu, ID_DEF_MENU_AISTARGETLIST, _("AIS Target List...") );

        if( seltype & SELTYPE_AISTARGET ) {
            MenuAppend( menuAIS, ID_DEF_MENU_AIS_QUERY, _( "Target Query..." ) );
            AIS_Target_Data *myptarget = g_pAIS->Get_Target_Data_From_MMSI( m_FoundAIS_MMSI );
            if( myptarget && myptarget->bCPA_Valid && (myptarget->n_alert_state != AIS_ALERT_SET) ) {
                if( myptarget->b_show_AIS_CPA )
                    MenuAppend( menuAIS, ID_DEF_MENU_AIS_CPA, _( "Hide Target CPA" ) );
                else
                    MenuAppend( menuAIS, ID_DEF_MENU_AIS_CPA, _( "Show Target CPA" ) );
            }
            MenuAppend( menuAIS, ID_DEF_MENU_AISTARGETLIST, _("Target List...") );
            if ( 1 /*g_bAISShowTracks*/ ) {
                if( myptarget && myptarget->b_show_track )
                    MenuAppend( menuAIS, ID_DEF_MENU_AISSHOWTRACK, _("Hide Target Track") );
                else
                    MenuAppend( menuAIS, ID_DEF_MENU_AISSHOWTRACK, _("Show Target Track") );
            }
            menuFocus = menuAIS;
        }
    }

    if( seltype & SELTYPE_ROUTESEGMENT ) {
        bool blay = false;
        if( m_pSelectedRoute && m_pSelectedRoute->m_bIsInLayer )
            blay = true;

        if( blay ){
            delete menuRoute;
            menuRoute = new wxMenu( _("Layer Route") );
            MenuAppend( menuRoute, ID_RT_MENU_PROPERTIES, _( "Properties..." ) );
            if( m_pSelectedRoute ) {
                if( m_pSelectedRoute->IsActive() ) {
                    int indexActive = m_pSelectedRoute->GetIndexOf( m_pSelectedRoute->m_pRouteActivePoint );
                    if( ( indexActive + 1 ) <= m_pSelectedRoute->GetnPoints() ) {
                        MenuAppend( menuRoute, ID_RT_MENU_ACTNXTPOINT, _( "Activate Next Waypoint" ) );
                    }
                    MenuAppend( menuRoute, ID_RT_MENU_DEACTIVATE, _( "Deactivate" ) );
                }
                else {
                    MenuAppend( menuRoute, ID_RT_MENU_ACTIVATE, _( "Activate" ) );
                }
            }
        }
        else {
            MenuAppend( menuRoute, ID_RT_MENU_PROPERTIES, _( "Properties..." ) );
            if( m_pSelectedRoute ) {
                if( m_pSelectedRoute->IsActive() ) {
                    int indexActive = m_pSelectedRoute->GetIndexOf( m_pSelectedRoute->m_pRouteActivePoint );
                    if( ( indexActive + 1 ) <= m_pSelectedRoute->GetnPoints() ) {
                        MenuAppend( menuRoute, ID_RT_MENU_ACTNXTPOINT, _( "Activate Next Waypoint" ) );
                    }
                    MenuAppend( menuRoute, ID_RT_MENU_DEACTIVATE, _( "Deactivate" ) );
                }
                else {
                    MenuAppend( menuRoute, ID_RT_MENU_ACTIVATE, _( "Activate" ) );
                }
            }
            MenuAppend( menuRoute, ID_RT_MENU_INSERT, _( "Insert Waypoint" ) );
            MenuAppend( menuRoute, ID_RT_MENU_APPEND, _( "Append Waypoint" ) );
            MenuAppend( menuRoute, ID_RT_MENU_COPY, _( "Copy as KML..." ) );
            MenuAppend( menuRoute, ID_RT_MENU_DELETE, _( "Delete..." ) );
            MenuAppend( menuRoute, ID_RT_MENU_REVERSE, _( "Reverse..." ) );
            wxString port = FindValidUploadPort();
            m_active_upload_port = port;
            wxString item = _( "Send to GPS" );
            if( !port.IsEmpty() ) {
                item.Append( _T(" ( ") );
                item.Append( port );
                item.Append(_T(" )") );
            }
            MenuAppend( menuRoute, ID_RT_MENU_SENDTOGPS, item );

            if( !port.IsEmpty() ) {
                wxString item = _( "Send to new GPS" );
                MenuAppend( menuRoute, ID_RT_MENU_SENDTONEWGPS, item );
            }
                
                
        }
        //      Set this menu as the "focused context menu"
        menuFocus = menuRoute;
    }

    if( seltype & SELTYPE_TRACKSEGMENT ) {
        bool blay = false;
        if( m_pSelectedTrack && m_pSelectedTrack->m_bIsInLayer )
            blay = true;

        if( blay ) {
            delete menuTrack;
            menuTrack = new wxMenu( _("Layer Track") );
            MenuAppend( menuTrack, ID_TK_MENU_PROPERTIES, _( "Properties..." ) );
        }
        else {
            MenuAppend( menuTrack, ID_TK_MENU_PROPERTIES, _( "Properties..." ) );
            MenuAppend( menuTrack, ID_TK_MENU_COPY, _( "Copy As KML" ) );
            MenuAppend( menuTrack, ID_TK_MENU_DELETE, _( "Delete..." ) );
        }

        //      Set this menu as the "focused context menu"
        menuFocus = menuTrack;
    }

    if( seltype & SELTYPE_ROUTEPOINT ) {
        bool blay = false;
        if( m_pFoundRoutePoint && m_pFoundRoutePoint->m_bIsInLayer )
            blay = true;

        if( blay ){
            delete menuWaypoint;
            menuWaypoint = new wxMenu( _("Layer Routepoint") );
            MenuAppend( menuWaypoint, ID_WP_MENU_PROPERTIES, _( "Properties..." ) );

            if( m_pSelectedRoute && m_pSelectedRoute->IsActive() )
                MenuAppend( menuWaypoint, ID_RT_MENU_ACTPOINT, _( "Activate" ) );
        }
        else {
            MenuAppend( menuWaypoint, ID_WP_MENU_PROPERTIES, _( "Properties..." ) );
            if( m_pSelectedRoute && m_pSelectedRoute->IsActive() ) {
                if(m_pSelectedRoute->m_pRouteActivePoint != m_pFoundRoutePoint )
                    MenuAppend( menuWaypoint, ID_RT_MENU_ACTPOINT, _( "Activate" ) );
            }

            if( m_pSelectedRoute && m_pSelectedRoute->IsActive() ) {
                if(m_pSelectedRoute->m_pRouteActivePoint == m_pFoundRoutePoint ) {
                    int indexActive = m_pSelectedRoute->GetIndexOf( m_pSelectedRoute->m_pRouteActivePoint );
                    if( ( indexActive + 1 ) <= m_pSelectedRoute->GetnPoints() )
                        MenuAppend( menuWaypoint, ID_RT_MENU_ACTNXTPOINT, _( "Activate Next Waypoint" ) );
                }
            }
            if( m_pSelectedRoute->GetnPoints() > 2 )
                MenuAppend( menuWaypoint, ID_RT_MENU_REMPOINT, _( "Remove from Route" ) );

            MenuAppend( menuWaypoint, ID_WPT_MENU_COPY, _( "Copy as KML" ) );

            if( m_pFoundRoutePoint->m_IconName != _T("mob") )
                MenuAppend( menuWaypoint, ID_RT_MENU_DELPOINT,  _( "Delete" ) );

            wxString port = FindValidUploadPort();
            m_active_upload_port = port;
            wxString item = _( "Send to GPS" );
            if( !port.IsEmpty() ) {
                item.Append( _T(" ( ") );
                item.Append( port );
                item.Append(_T(" )") );
            }
            MenuAppend( menuWaypoint, ID_WPT_MENU_SENDTOGPS, item );
            
            if( !port.IsEmpty() ) {
                wxString item = _( "Send to new GPS" );
                MenuAppend( menuWaypoint, ID_WPT_MENU_SENDTONEWGPS, item );
            }
            
            
        }
        //      Set this menu as the "focused context menu"
        menuFocus = menuWaypoint;
    }

    if( seltype & SELTYPE_MARKPOINT ) {
        bool blay = false;
        if( m_pFoundRoutePoint && m_pFoundRoutePoint->m_bIsInLayer )
            blay = true;

        if( blay ){
            delete menuWaypoint;
            menuWaypoint = new wxMenu( _("Layer Waypoint") );
            MenuAppend( menuWaypoint, ID_WP_MENU_PROPERTIES, _( "Properties..." ) );
        }
        else {
            MenuAppend( menuWaypoint, ID_WP_MENU_PROPERTIES, _( "Properties..." ) );

            if( !g_pRouteMan->GetpActiveRoute() )
                MenuAppend( menuWaypoint, ID_WP_MENU_GOTO, _( "Navigate To This" ) );

            MenuAppend( menuWaypoint, ID_WPT_MENU_COPY, _( "Copy as KML" ) );
#ifdef __WXOSX__
            if ( m_pFoundRoutePoint != NULL)
#endif
            if( m_pFoundRoutePoint->m_IconName != _T("mob") )
                MenuAppend( menuWaypoint, ID_WP_MENU_DELPOINT, _( "Delete" ) );

            wxString port = FindValidUploadPort();
            m_active_upload_port = port;
            wxString item = _( "Send to GPS" );
            if( !port.IsEmpty() ) {
                item.Append( _T(" ( ") );
                item.Append( port );
                item.Append(_T(" )") );
            }
            MenuAppend( menuWaypoint, ID_WPT_MENU_SENDTOGPS, item );


            if( ( m_pFoundRoutePoint == pAnchorWatchPoint1 ) || ( m_pFoundRoutePoint == pAnchorWatchPoint2 ) )
                MenuAppend( menuWaypoint, ID_WP_MENU_CLEAR_ANCHORWATCH, _( "Clear Anchor Watch" ) );
            else {
                if( !( m_pFoundRoutePoint->m_bIsInLayer )
                    && ( ( NULL == pAnchorWatchPoint1 ) || ( NULL == pAnchorWatchPoint2 ) ) ) {

                    double dist;
                    double brg;
                    DistanceBearingMercator( m_pFoundRoutePoint->m_lat, m_pFoundRoutePoint->m_lon, gLat,
                                         gLon, &brg, &dist );
                    if( dist * 1852. <= g_nAWMax )
                        MenuAppend( menuWaypoint,  ID_WP_MENU_SET_ANCHORWATCH,  _( "Set Anchor Watch" ) );
                }
            }
        }

        //      Set this menu as the "focused context menu"
        menuFocus = menuWaypoint;
    }

    if( ! subMenuChart->GetMenuItemCount() ) contextMenu->Destroy( subItemChart );

    //  Add the Tide/Current selections if the item was not activated by shortcut in right-click handlers
    bool bsep = false;
    if( seltype & SELTYPE_TIDEPOINT ){
        menuFocus->AppendSeparator();
        bsep = true;
        MenuAppend( menuFocus, ID_DEF_MENU_TIDEINFO, _( "Show Tide Information" ) );
    }

    if( seltype & SELTYPE_CURRENTPOINT ) {
        if( !bsep )
            menuFocus->AppendSeparator();
        MenuAppend( menuFocus,  ID_DEF_MENU_CURRENTINFO, _( "Show Current Information" ) );
    }

    //        Invoke the correct focused drop-down menu
    PopupMenu( menuFocus, x, y );


    // Cleanup
    if( ( m_pSelectedRoute ) ) {
        m_pSelectedRoute->m_bRtIsSelected = false;
    }

    m_pSelectedRoute = NULL;

    if( m_pFoundRoutePoint ) {
        m_pFoundRoutePoint->m_bPtIsSelected = false;
    }
    m_pFoundRoutePoint = NULL;

    m_pFoundRoutePointSecond = NULL;

    delete contextMenu;
    delete menuAIS;
    delete menuRoute;
    delete menuTrack;
    delete menuWaypoint;
}

void ChartCanvas::ShowObjectQueryWindow( int x, int y, float zlat, float zlon )
{
    ChartPlugInWrapper *target_plugin_chart = NULL;
    s57chart *Chs57 = NULL;

    ChartBase *target_chart = GetChartAtCursor();
    if( target_chart ){
        if( (target_chart->GetChartType() == CHART_TYPE_PLUGIN) && (target_chart->GetChartFamily() == CHART_FAMILY_VECTOR) )
            target_plugin_chart = dynamic_cast<ChartPlugInWrapper *>(target_chart);
        else
            Chs57 = dynamic_cast<s57chart*>( target_chart );
    }

    std::vector<Ais8_001_22*> area_notices;

    if( g_pAIS && g_bShowAIS && g_bShowAreaNotices ) {
        AIS_Target_Hash* an_sources = g_pAIS->GetAreaNoticeSourcesList();

        float vp_scale = GetVPScale();

        for( AIS_Target_Hash::iterator target = an_sources->begin(); target != an_sources->end(); ++target ) {
            AIS_Target_Data* target_data = target->second;
            if( !target_data->area_notices.empty() ) {
                for( AIS_Area_Notice_Hash::iterator ani = target_data->area_notices.begin(); ani != target_data->area_notices.end(); ++ani ) {
                    Ais8_001_22& area_notice = ani->second;

                    wxBoundingBox bbox;

                    for( Ais8_001_22_SubAreaList::iterator sa = area_notice.sub_areas.begin(); sa != area_notice.sub_areas.end(); ++sa ) {
                        switch( sa->shape ) {
                            case AIS8_001_22_SHAPE_CIRCLE: {
                                wxPoint target_point;
                                GetCanvasPointPix( sa->latitude, sa->longitude, &target_point );
                                bbox.Expand( target_point );
                                if( sa->radius_m > 0.0 )
                                    bbox.EnLarge( sa->radius_m * vp_scale );
                                break;
                            }
                            case AIS8_001_22_SHAPE_POLYGON:
                            case AIS8_001_22_SHAPE_POLYLINE: {
                                for( int i = 0; i < 4; ++i ) {
                                    double lat = sa->latitude;
                                    double lon = sa->longitude;
                                    ll_gc_ll( lat, lon, sa->angles[i], sa->dists_m[i] / 1852.0,
                                              &lat, &lon );
                                    wxPoint target_point;
                                    GetCanvasPointPix( lat, lon, &target_point );
                                    bbox.Expand( target_point );
                                }
                            }
                        }
                    }

                    if( bbox.PointInBox( x, y ) ) {
                        area_notices.push_back( &area_notice );
                    }
                }
            }
        }
    }


    if( target_plugin_chart || Chs57 || !area_notices.empty() ) {
        // Go get the array of all objects at the cursor lat/lon
        int sel_rad_pix = 5;
        float SelectRadius = sel_rad_pix / ( GetVP().view_scale_ppm * 1852 * 60 );

        // Make sure we always get the lights from an object, even if we are currently
        // not displaying lights on the chart.

        SetCursor( wxCURSOR_WAIT );
        bool lightsVis = gFrame->ToggleLights( false );
        if( !lightsVis ) gFrame->ToggleLights( true, true );

        ListOfObjRazRules* rule_list = NULL;
        ListOfPI_S57Obj* pi_rule_list = NULL;
        if( Chs57 )
            rule_list = Chs57->GetObjRuleListAtLatLon( zlat, zlon, SelectRadius, &GetVP() );
        else if( target_plugin_chart )
            pi_rule_list = g_pi_manager->GetPlugInObjRuleListAtLatLon( target_plugin_chart, zlat, zlon, SelectRadius, GetVP() );

        ListOfObjRazRules* overlay_rule_list = NULL;
        ChartBase *overlay_chart = GetOverlayChartAtCursor();
        s57chart *CHs57_Overlay = dynamic_cast<s57chart*>( overlay_chart );

        if( CHs57_Overlay ) {
            overlay_rule_list =
                CHs57_Overlay->GetObjRuleListAtLatLon( zlat, zlon, SelectRadius, &GetVP() );
        }

        if( !lightsVis ) gFrame->ToggleLights( true, true );

        wxString objText;
        wxFont *dFont = FontMgr::Get().GetFont( _("ObjectQuery"), 12 );
        wxString face = dFont->GetFaceName();

        if( NULL == g_pObjectQueryDialog ) {
            g_pObjectQueryDialog = new S57QueryDialog();

            g_pObjectQueryDialog->Create( this, -1, _( "Object Query" ), wxDefaultPosition,
                                          wxSize( g_S57_dialog_sx, g_S57_dialog_sy ) );
            g_pObjectQueryDialog->Centre();
        }

        wxColor bg = g_pObjectQueryDialog->GetBackgroundColour();
        wxColor fg = FontMgr::Get().GetFontColor( _("ObjectQuery") );

        objText.Printf( _T("<html><body bgcolor=#%02x%02x%02x><font color=#%02x%02x%02x face="), bg.Red(), bg.Blue(),
                        bg.Green(), fg.Red(), fg.Blue(), fg.Green() );
        objText += _T("\"");
        objText += face;
        objText += _T("\" ");

        int points = dFont->GetPointSize();
        wxString ss;
        switch (points & 0xFE){
            case 8:  ss = _T("size=\"2\""); break;
            case 10: ss = _T("size=\"3\""); break;
            case 12: ss = _T("size=\"3\""); break;
            case 14: ss = _T("size=\"4\""); break;
            case 16: ss = _T("size=\"4\""); break;
            case 18: ss = _T("size=\"5\""); break;
            case 20: ss = _T("size=\"6\""); break;
            default: ss = _T(" "); break;
        }
        
        if(points > 20)
            ss = _T("size=\"6\"");
        
        objText += ss;
        objText += _T(">");

        if(wxFONTSTYLE_ITALIC == dFont->GetStyle())
            objText += _T("<i>");
        
        if( overlay_rule_list && CHs57_Overlay) {
            objText << CHs57_Overlay->CreateObjDescriptions( overlay_rule_list );
            objText << _T("<hr noshade>");
        }

        for( std::vector< Ais8_001_22* >::iterator an = area_notices.begin(); an != area_notices.end(); ++an ) {
            objText << _T( "<b>AIS Area Notice:</b> " );
            objText << ais8_001_22_notice_names[( *an )->notice_type];
            for( std::vector< Ais8_001_22_SubArea >::iterator sa = ( *an )->sub_areas.begin(); sa != ( *an )->sub_areas.end(); ++sa )
                if( !sa->text.empty() )
                    objText << sa->text;
            objText << _T( "<br>expires: " ) << ( *an )->expiry_time.Format();
            objText << _T( "<hr noshade>" );
        }

        if( Chs57 )
            objText << Chs57->CreateObjDescriptions( rule_list );
        else if( target_plugin_chart )
            objText << g_pi_manager->CreateObjDescriptions( target_plugin_chart, pi_rule_list );

        objText << _T("</font>");
        if(wxFONTSTYLE_ITALIC == dFont->GetStyle())
            objText << _T("</i>");
        
        objText << _T("</body></html>");
        
        g_pObjectQueryDialog->SetHTMLPage( objText );

        g_pObjectQueryDialog->Show();

        if( rule_list )
            rule_list->Clear();
        delete rule_list;

        if( overlay_rule_list )
            overlay_rule_list->Clear();
        delete overlay_rule_list;

        if( pi_rule_list )
            pi_rule_list->Clear();
        delete pi_rule_list;

        SetCursor( wxCURSOR_ARROW );
    }
}

void ChartCanvas::RemovePointFromRoute( RoutePoint* point, Route* route ) {
    //  Rebuild the route selectables
    pSelect->DeleteAllSelectableRoutePoints( route );
    pSelect->DeleteAllSelectableRouteSegments( route );

    route->RemovePoint( point );

    //  Check for 1 point routes. If we are creating a route, this is an undo, so keep the 1 point.
    if( (route->GetnPoints() <= 1) && (parent_frame->nRoute_State == 0) ) {
        pConfig->DeleteConfigRoute( route );
        g_pRouteMan->DeleteRoute( route );
        route = NULL;
    }
    //  Add this point back into the selectables
    pSelect->AddSelectableRoutePoint( point->m_lat, point->m_lon, point );

    if( pRoutePropDialog && ( pRoutePropDialog->IsShown() ) ) {
        pRoutePropDialog->SetRouteAndUpdate( route, true );
    }

    InvalidateGL();
}

void ChartCanvas::ShowMarkPropertiesDialog( RoutePoint* markPoint ) {
    if( NULL == pMarkPropDialog )    // There is one global instance of the MarkProp Dialog
        pMarkPropDialog = new MarkInfoImpl( this );

    if( g_bresponsive ) {

        wxSize canvas_size = cc1->GetSize();
        wxPoint canvas_pos = cc1->GetPosition();
        wxSize fitted_size = pMarkPropDialog->GetSize();;

        if(canvas_size.x < fitted_size.x){
            fitted_size.x = canvas_size.x;
            if(canvas_size.y < fitted_size.y)
                fitted_size.y -= 20;                // scrollbar added
        }
        if(canvas_size.y < fitted_size.y){
            fitted_size.y = canvas_size.y;
            if(canvas_size.x < fitted_size.x)
                fitted_size.x -= 20;                // scrollbar added
        }

        pMarkPropDialog->SetSize( fitted_size );
        pMarkPropDialog->Centre();


        int xp = (canvas_size.x - fitted_size.x)/2;
        int yp = (canvas_size.y - fitted_size.y)/2;

        wxPoint xxp = ClientToScreen(canvas_pos);
//        pMarkPropDialog->Move(xxp.x + xp, xxp.y + yp);

    }

    pMarkPropDialog->SetRoutePoint( markPoint );
    pMarkPropDialog->UpdateProperties();
    if( markPoint->m_bIsInLayer ) {
        wxString caption( _("Waypoint Properties, Layer: ") );
        caption.Append( GetLayerName( markPoint->m_LayerID ) );
        pMarkPropDialog->SetDialogTitle( caption );
    } else
        pMarkPropDialog->SetDialogTitle( _("Waypoint Properties") );

    pMarkPropDialog->Show();
    pMarkPropDialog->InitialFocus();
}

void ChartCanvas::ShowRoutePropertiesDialog(wxString title, Route* selected)
{
    if( NULL == pRoutePropDialog )  // There is one global instance of the RouteProp Dialog
        pRoutePropDialog = new RouteProp( this );

    if( g_bresponsive ) {

        wxSize canvas_size = cc1->GetSize();
        wxPoint canvas_pos = cc1->GetPosition();
        wxSize fitted_size = pRoutePropDialog->GetSize();;

        if(canvas_size.x < fitted_size.x){
            fitted_size.x = canvas_size.x;
            if(canvas_size.y < fitted_size.y)
                fitted_size.y -= 20;                // scrollbar added
        }
        if(canvas_size.y < fitted_size.y){
            fitted_size.y = canvas_size.y;
            if(canvas_size.x < fitted_size.x)
                fitted_size.x -= 20;                // scrollbar added
        }


        pRoutePropDialog->SetSize( fitted_size );
        pRoutePropDialog->Centre();

        int xp = (canvas_size.x - fitted_size.x)/2;
        int yp = (canvas_size.y - fitted_size.y)/2;

        wxPoint xxp = ClientToScreen(canvas_pos);
//        pRoutePropDialog->Move(xxp.x + xp, xxp.y + yp);

    }


    pRoutePropDialog->SetRouteAndUpdate( selected );
    pRoutePropDialog->UpdateProperties();
    if( !selected->m_bIsInLayer )
        pRoutePropDialog->SetDialogTitle( title );
    else {
        wxString caption( title << _T(", Layer: ") );
        caption.Append( GetLayerName( selected->m_LayerID ) );
        pRoutePropDialog->SetDialogTitle( caption );
    }

    pRoutePropDialog->Show();

    Refresh( false );
}

void ChartCanvas::ShowTrackPropertiesDialog( Route* selected )
{
    if( NULL == pTrackPropDialog )  // There is one global instance of the RouteProp Dialog
        pTrackPropDialog = new TrackPropDlg( this );

    pTrackPropDialog->SetTrackAndUpdate( selected );
    pTrackPropDialog->UpdateProperties();

    pTrackPropDialog->Show();

    Refresh( false );
}

void pupHandler_PasteWaypoint() {
    Kml* kml = new Kml();
    ::wxBeginBusyCursor();

    int pasteBuffer = kml->ParsePasteBuffer();
    RoutePoint* pasted = kml->GetParsedRoutePoint();

    int nearby_sel_rad_pix = 8;
    double nearby_radius_meters = nearby_sel_rad_pix / cc1->GetCanvasTrueScale();

    RoutePoint *nearPoint = pWayPointMan->GetNearbyWaypoint( pasted->m_lat, pasted->m_lon,
                               nearby_radius_meters );

    int answer = wxID_NO;
    if( nearPoint && !nearPoint->m_bIsInTrack && !nearPoint->m_bIsInLayer ) {
        wxString msg;
        msg << _("There is an existing waypoint at the same location as the one you are pasting. Would you like to merge the pasted data with it?\n\n");
        msg << _("Answering 'No' will create a new waypoint at the same location.");
        answer = OCPNMessageBox( cc1, msg, _("Merge waypoint?"), (long) wxYES_NO | wxCANCEL | wxNO_DEFAULT );
    }

    if( answer == wxID_YES ) {
        nearPoint->SetName( pasted->GetName() );
        nearPoint->m_MarkDescription = pasted->m_MarkDescription;
        if( pRouteManagerDialog && pRouteManagerDialog->IsShown() ) pRouteManagerDialog->UpdateWptListCtrl();
    }

    if( answer == wxID_NO ) {
        RoutePoint* newPoint = new RoutePoint( pasted );
        newPoint->m_bIsolatedMark = true;
        pSelect->AddSelectableRoutePoint( newPoint->m_lat, newPoint->m_lon, newPoint );
        pConfig->AddNewWayPoint( newPoint, -1 );
        pWayPointMan->AddRoutePoint( newPoint );
        if( pRouteManagerDialog && pRouteManagerDialog->IsShown() ) pRouteManagerDialog->UpdateWptListCtrl();
    }

    cc1->InvalidateGL();
    cc1->Refresh( false );
    delete kml;
    ::wxEndBusyCursor();
}

void pupHandler_PasteRoute() {
    Kml* kml = new Kml();
    ::wxBeginBusyCursor();

    int pasteBuffer = kml->ParsePasteBuffer();
    Route* pasted = kml->GetParsedRoute();
    if( ! pasted ) return;

    int nearby_sel_rad_pix = 8;
    double nearby_radius_meters = nearby_sel_rad_pix / cc1->GetCanvasTrueScale();

    RoutePoint* curPoint;
    RoutePoint* nearPoint;
    RoutePoint* prevPoint = NULL;

    bool mergepoints = false;
    bool createNewRoute = true;
    int existingWaypointCounter = 0;

    for( int i = 1; i <= pasted->GetnPoints(); i++ ) {
		curPoint = pasted->GetPoint( i ); // NB! n starts at 1 !
        nearPoint = pWayPointMan->GetNearbyWaypoint( curPoint->m_lat,
                curPoint->m_lon, nearby_radius_meters );
        if( nearPoint ) {
            mergepoints = true;
            existingWaypointCounter++;
            // Small hack here to avoid both extending RoutePoint and repeating all the GetNearbyWaypoint
            // calculations. Use existin data field in RoutePoint as temporary storage.
            curPoint->m_bPtIsSelected = true;
        }
    }

    int answer = wxID_NO;
    if( mergepoints ) {
        wxString msg;
        msg << _("There are existing waypoints at the same location as some of the ones you are pasting. Would you like to just merge the pasted data into them?\n\n");
        msg << _("Answering 'No' will create all new waypoints for this route.");
        answer = OCPNMessageBox( cc1, msg, _("Merge waypoints?"), (long) wxYES_NO | wxCANCEL | wxYES_DEFAULT );

        if( answer == wxID_CANCEL ) {
            delete kml;
            return;
        }
    }

    // If all waypoints exist since before, and a route with the same name, we don't create a new route.
    if( mergepoints && answer==wxID_YES && existingWaypointCounter==pasted->GetnPoints() ) {

        wxRouteListNode *route_node = pRouteList->GetFirst();
        while( route_node ) {
            Route *proute = route_node->GetData();

            if( pasted->m_RouteNameString == proute->m_RouteNameString ) {
                createNewRoute = false;
                break;
            }
            route_node = route_node->GetNext();
        }
    }

    Route* newRoute = NULL;
    RoutePoint* newPoint;

    if( createNewRoute ) {
        newRoute = new Route();
        newRoute->m_RouteNameString = pasted->m_RouteNameString;
    }

    for( int i = 1; i <= pasted->GetnPoints(); i++ ) {
        curPoint = pasted->GetPoint( i );
        if( answer == wxID_YES && curPoint->m_bPtIsSelected ) {
            curPoint->m_bPtIsSelected = false;
            newPoint = pWayPointMan->GetNearbyWaypoint( curPoint->m_lat, curPoint->m_lon,
                    nearby_radius_meters );
            newPoint->SetName( curPoint->GetName() );
            newPoint->m_MarkDescription = curPoint->m_MarkDescription;

            if( createNewRoute ) newRoute->AddPoint( newPoint );
        } else {
            curPoint->m_bPtIsSelected = false;

            newPoint = new RoutePoint( curPoint );
            newPoint->m_bIsolatedMark = false;
            newPoint->m_IconName = _T("circle");
            newPoint->m_bIsVisible = true;
            newPoint->m_bShowName = false;
            newPoint->m_bKeepXRoute = false;
#ifdef __WXOSX__
            if( createNewRoute ) newRoute->AddPoint( newPoint );
#else
            newRoute->AddPoint( newPoint );
#endif
            pSelect->AddSelectableRoutePoint( newPoint->m_lat, newPoint->m_lon, newPoint );
            pConfig->AddNewWayPoint( newPoint, -1 );
            pWayPointMan->AddRoutePoint( newPoint );
        }
        if( i > 1 && createNewRoute ) pSelect->AddSelectableRouteSegment( prevPoint->m_lat,
                prevPoint->m_lon, curPoint->m_lat, curPoint->m_lon, prevPoint, newPoint, newRoute );
        prevPoint = newPoint;
    }

    if( createNewRoute ) {
        pRouteList->Append( newRoute );
        pConfig->AddNewRoute( newRoute, -1 );    // use auto next num
        newRoute->RebuildGUIDList(); // ensure the GUID list is intact and good

        if( pRoutePropDialog && ( pRoutePropDialog->IsShown() ) ) {
            pRoutePropDialog->SetRouteAndUpdate( newRoute );
            pRoutePropDialog->UpdateProperties();
        }

        if( pRouteManagerDialog && pRouteManagerDialog->IsShown() ) {
            pRouteManagerDialog->UpdateRouteListCtrl();
            pRouteManagerDialog->UpdateWptListCtrl();
        }
        cc1->InvalidateGL();
        cc1->Refresh( false );
    }

    delete kml;
    ::wxEndBusyCursor();
}

void pupHandler_PasteTrack() {
    Kml* kml = new Kml();
    ::wxBeginBusyCursor();

    int pasteBuffer = kml->ParsePasteBuffer();
    Track* pasted = kml->GetParsedTrack();
    if( ! pasted ) return;

    RoutePoint* curPoint;

    Track* newTrack = new Track();
    RoutePoint* newPoint;
    RoutePoint* prevPoint = NULL;

    newTrack->m_RouteNameString = pasted->m_RouteNameString;

    for( int i = 1; i <= pasted->GetnPoints(); i++ ) {
        curPoint = pasted->GetPoint( i );

        newPoint = new RoutePoint( curPoint );
        newPoint->m_bShowName = false;
        newPoint->m_bIsVisible = false;
        newPoint->m_GPXTrkSegNo = 1;

        wxDateTime now = wxDateTime::Now();
        newPoint->SetCreateTime(curPoint->GetCreateTime());

        newTrack->AddPoint( newPoint );

        //    This is a hack, need to undo the action of Route::AddPoint
        newPoint->m_bIsInRoute = false;
        newPoint->m_bIsInTrack = true;

        if( prevPoint )
            pSelect->AddSelectableTrackSegment(
                prevPoint->m_lat, prevPoint->m_lon,
                newPoint->m_lat, newPoint->m_lon,
                prevPoint, newPoint, newTrack );

        prevPoint = newPoint;
    }

    pRouteList->Append( newTrack );
    pConfig->AddNewRoute( newTrack, -1 );    // use auto next num
    newTrack->RebuildGUIDList(); // ensure the GUID list is intact and good

    cc1->InvalidateGL();
    cc1->Refresh( false );
    delete kml;
    ::wxEndBusyCursor();
}

void ChartCanvas::PopupMenuHandler( wxCommandEvent& event )
{
    RoutePoint *pLast;

    wxPoint r;
    double zlat, zlon;

    GetCanvasPixPoint( popx, popy, zlat, zlon );

    switch( event.GetId() ) {
    case ID_DEF_MENU_MAX_DETAIL:
        vLat = zlat;
        vLon = zlon;
        ClearbFollow();

        parent_frame->DoChartUpdate();

        parent_frame->SelectChartFromStack( 0, false, CHART_TYPE_DONTCARE,
                                            CHART_FAMILY_RASTER );
        break;

    case ID_DEF_MENU_SCALE_IN:
        parent_frame->DoStackDown();
        break;

    case ID_DEF_MENU_SCALE_OUT:
        parent_frame->DoStackUp();
        break;

    case ID_UNDO:
        undo->UndoLastAction();
        InvalidateGL();
        Refresh( false );
        break;

    case ID_REDO:
        undo->RedoNextAction();
        InvalidateGL();
        Refresh( false );
        break;

    case ID_DEF_MENU_MOVE_BOAT_HERE:
        gLat = zlat;
        gLon = zlon;
        break;

    case ID_DEF_MENU_GOTO_HERE: {
        RoutePoint *pWP_dest = new RoutePoint( zlat, zlon, g_default_wp_icon, wxEmptyString,
                                               GPX_EMPTY_STRING );
        pSelect->AddSelectableRoutePoint( zlat, zlon, pWP_dest );

        RoutePoint *pWP_src = new RoutePoint( gLat, gLon, g_default_wp_icon, wxEmptyString,
                                              GPX_EMPTY_STRING );
        pSelect->AddSelectableRoutePoint( gLat, gLon, pWP_src );

        Route *temp_route = new Route();
        pRouteList->Append( temp_route );

        temp_route->AddPoint( pWP_src );
        temp_route->AddPoint( pWP_dest );

        pSelect->AddSelectableRouteSegment( gLat, gLon, zlat, zlon, pWP_src, pWP_dest,
                                            temp_route );

        temp_route->m_RouteNameString = _("Temporary GOTO Route");
        temp_route->m_RouteStartString = _("Here");
        ;
        temp_route->m_RouteEndString = _("There");

        temp_route->m_bDeleteOnArrival = true;

        if( g_pRouteMan->GetpActiveRoute() ) g_pRouteMan->DeactivateRoute();

        g_pRouteMan->ActivateRoute( temp_route, pWP_dest );

        break;
    }

    case ID_DEF_MENU_DROP_WP: {
        RoutePoint *pWP = new RoutePoint( zlat, zlon, g_default_wp_icon, wxEmptyString,
                                          GPX_EMPTY_STRING );
        pWP->m_bIsolatedMark = true;                      // This is an isolated mark
        pSelect->AddSelectableRoutePoint( zlat, zlon, pWP );
        pConfig->AddNewWayPoint( pWP, -1 );    // use auto next num

        if( pRouteManagerDialog && pRouteManagerDialog->IsShown() ) pRouteManagerDialog->UpdateWptListCtrl();
        undo->BeforeUndoableAction( Undo_CreateWaypoint, pWP, Undo_HasParent, NULL );
        undo->AfterUndoableAction( NULL );
        InvalidateGL();
        Refresh( false );      // Needed for MSW, why not GTK??
        break;
    }

    case ID_DEF_MENU_AISTARGETLIST:
        ShowAISTargetList();
        break;

    case ID_WP_MENU_GOTO: {
        RoutePoint *pWP_src = new RoutePoint( gLat, gLon, g_default_wp_icon, wxEmptyString,
                                              GPX_EMPTY_STRING );
        pSelect->AddSelectableRoutePoint( gLat, gLon, pWP_src );

        Route *temp_route = new Route();
        pRouteList->Append( temp_route );

        temp_route->AddPoint( pWP_src );
        temp_route->AddPoint( m_pFoundRoutePoint );
        m_pFoundRoutePoint->m_bKeepXRoute = true;

        pSelect->AddSelectableRouteSegment( gLat, gLon, m_pFoundRoutePoint->m_lat,
                                            m_pFoundRoutePoint->m_lon, pWP_src, m_pFoundRoutePoint, temp_route );

        wxString name = m_pFoundRoutePoint->GetName();
        if( name.IsEmpty() ) name = _("(Unnamed Waypoint)");
        wxString rteName = _("Go to ");
        rteName.Append( name );
        temp_route->m_RouteNameString = rteName;
        temp_route->m_RouteStartString = _("Here");
        ;
        temp_route->m_RouteEndString = name;
        temp_route->m_bDeleteOnArrival = true;

        if( g_pRouteMan->GetpActiveRoute() ) g_pRouteMan->DeactivateRoute();

        g_pRouteMan->ActivateRoute( temp_route, m_pFoundRoutePoint );

        break;
    }

    case ID_DEF_MENU_COGUP:
        gFrame->ToggleCourseUp();
        break;

    case ID_DEF_MENU_NORTHUP:
        gFrame->ToggleCourseUp();
        break;

    case ID_DEF_MENU_GOTOPOSITION:
        if( NULL == pGoToPositionDialog ) // There is one global instance of the Go To Position Dialog
            pGoToPositionDialog = new GoToPositionDialog( this );
        pGoToPositionDialog->CheckPasteBufferForPosition();
        pGoToPositionDialog->Show();
        break;

    case ID_WP_MENU_DELPOINT: {
        if( m_pFoundRoutePoint == pAnchorWatchPoint1 ) {
            pAnchorWatchPoint1 = NULL;
            g_AW1GUID.Clear();
        } else if( m_pFoundRoutePoint == pAnchorWatchPoint2 ) {
            pAnchorWatchPoint2 = NULL;
            g_AW2GUID.Clear();
        }

        if( m_pFoundRoutePoint && !( m_pFoundRoutePoint->m_bIsInLayer )
                && ( m_pFoundRoutePoint->m_IconName != _T("mob") ) ) {

            // If the WP belongs to an invisible route, we come here instead of to ID_RT_MENU_DELPOINT
            //  Check it, and if so then remove the point from its routes
            wxArrayPtrVoid *proute_array = g_pRouteMan->GetRouteArrayContaining( m_pFoundRoutePoint );
            if( proute_array ) {
                pWayPointMan->DestroyWaypoint( m_pFoundRoutePoint );
                m_pFoundRoutePoint = NULL;
             }
            else {
                undo->BeforeUndoableAction( Undo_DeleteWaypoint, m_pFoundRoutePoint, Undo_IsOrphanded, m_pFoundPoint );
                pConfig->DeleteWayPoint( m_pFoundRoutePoint );
                pSelect->DeleteSelectablePoint( m_pFoundRoutePoint, SELTYPE_ROUTEPOINT );
                if( NULL != pWayPointMan )
                    pWayPointMan->RemoveRoutePoint( m_pFoundRoutePoint );
                m_pFoundRoutePoint = NULL;
                undo->AfterUndoableAction( NULL );
            }

            if( pMarkPropDialog ) {
                pMarkPropDialog->SetRoutePoint( NULL );
                pMarkPropDialog->UpdateProperties();
            }

            if( pRouteManagerDialog && pRouteManagerDialog->IsShown() )
                pRouteManagerDialog->UpdateWptListCtrl();

            InvalidateGL();
        }
        break;
    }
    case ID_WP_MENU_PROPERTIES:
        ShowMarkPropertiesDialog( m_pFoundRoutePoint );
        break;

    case ID_WP_MENU_CLEAR_ANCHORWATCH:
        if( pAnchorWatchPoint1 == m_pFoundRoutePoint ) {
            pAnchorWatchPoint1 = NULL;
            g_AW1GUID.Clear();
        } else if( pAnchorWatchPoint2 == m_pFoundRoutePoint ) {
            pAnchorWatchPoint2 = NULL;
            g_AW2GUID.Clear();
        }
        break;

    case ID_WP_MENU_SET_ANCHORWATCH:
        if( pAnchorWatchPoint1 == NULL ) {
            pAnchorWatchPoint1 = m_pFoundRoutePoint;
            g_AW1GUID = pAnchorWatchPoint1->m_GUID;
            wxString nn;
            nn = m_pFoundRoutePoint->GetName();
            if( nn.IsNull() ) {
                nn.Printf( _T("%d m"), g_nAWDefault );
                m_pFoundRoutePoint->SetName( nn );
            }
        } else if( pAnchorWatchPoint2 == NULL ) {
            pAnchorWatchPoint2 = m_pFoundRoutePoint;
            g_AW2GUID = pAnchorWatchPoint2->m_GUID;
            wxString nn;
            nn = m_pFoundRoutePoint->GetName();
            if( nn.IsNull() ) {
                nn.Printf( _T("%d m"), g_nAWDefault );
                m_pFoundRoutePoint->SetName( nn );
            }
        }
        break;

    case ID_WP_MENU_ADDITIONAL_INFO:
        if( NULL == pMarkInfoDialog )    // There is one global instance of the MarkInfo Dialog
            pMarkInfoDialog = new MarkInfoImpl( this );

        pMarkInfoDialog->SetRoutePoint( m_pFoundRoutePoint );
        pMarkInfoDialog->UpdateProperties();

        pMarkInfoDialog->Show();
        break;

    case ID_DEF_MENU_ACTIVATE_MEASURE:
//                        WarpPointer(popx,popy);
        m_bMeasure_Active = true;
        m_nMeasureState = 1;
        break;

    case ID_DEF_MENU_DEACTIVATE_MEASURE:
        m_bMeasure_Active = false;
        m_nMeasureState = 0;
        g_pRouteMan->DeleteRoute( m_pMeasureRoute );
        m_pMeasureRoute = NULL;
        gFrame->SurfaceToolbar();
        InvalidateGL();
        Refresh( false );
        break;

#ifdef USE_S57
    case ID_DEF_MENU_CM93OFFSET_DIALOG:
        if( NULL == g_pCM93OffsetDialog ) {
            if( !VPoint.b_quilt && Current_Ch
                    && ( Current_Ch->GetChartType() == CHART_TYPE_CM93COMP ) ) {
                cm93compchart *pch = (cm93compchart *) Current_Ch;
                g_pCM93OffsetDialog = new CM93OffsetDialog( parent_frame, pch );
            }
        }
        g_pCM93OffsetDialog->Show();
        g_pCM93OffsetDialog->UpdateMCOVRList( GetVP() );

        break;

    case ID_DEF_MENU_QUERY: {
        ShowObjectQueryWindow( popx, popy, zlat, zlon );
        break;
    }
#endif
    case ID_DEF_MENU_AIS_QUERY: {
        wxWindow *pwin = wxDynamicCast(this, wxWindow);
        ShowAISTargetQueryDialog( pwin, m_FoundAIS_MMSI );
        break;
    }

    case ID_DEF_MENU_AIS_CPA: {             //TR 2012.06.28: Show AIS-CPA
        AIS_Target_Data *myptarget = g_pAIS->Get_Target_Data_From_MMSI(m_FoundAIS_MMSI); //TR 2012.06.28: Show AIS-CPA
        if ( myptarget )                    //TR 2012.06.28: Show AIS-CPA
            myptarget->Toggle_AIS_CPA();     //TR 2012.06.28: Show AIS-CPA
        break;                              //TR 2012.06.28: Show AIS-CPA
    }

    case ID_DEF_MENU_AISSHOWTRACK: {
        AIS_Target_Data *myptarget = g_pAIS->Get_Target_Data_From_MMSI(m_FoundAIS_MMSI);
        if ( myptarget )
            myptarget->ToggleShowTrack();
        break;
    }

    case ID_DEF_MENU_QUILTREMOVE: {
        if( VPoint.b_quilt ) {
            int dbIndex = m_pQuilt->GetChartdbIndexAtPix( wxPoint( popx, popy ) );
            parent_frame->RemoveChartFromQuilt( dbIndex );

            ReloadVP();

        }

        break;
    }

    case ID_DEF_MENU_CURRENTINFO: {
        DrawTCWindow( popx, popy, (void *) m_pIDXCandidate );
        Refresh( false );

        break;
    }

    case ID_DEF_MENU_TIDEINFO: {
        DrawTCWindow( popx, popy, (void *) m_pIDXCandidate );
        Refresh( false );

        break;
    }
    case ID_RT_MENU_REVERSE: {
        if( m_pSelectedRoute->m_bIsInLayer ) break;

        int ask_return = OCPNMessageBox( this, g_pRouteMan->GetRouteReverseMessage(),
                               _("Rename Waypoints?"), wxYES_NO | wxCANCEL );

        if( ask_return != wxID_CANCEL ) {
            pSelect->DeleteAllSelectableRouteSegments( m_pSelectedRoute );
            m_pSelectedRoute->Reverse( ask_return == wxID_YES );
            pSelect->AddAllSelectableRouteSegments( m_pSelectedRoute );

            pConfig->UpdateRoute( m_pSelectedRoute );

            if( pRoutePropDialog && ( pRoutePropDialog->IsShown() ) ) {
                pRoutePropDialog->SetRouteAndUpdate( m_pSelectedRoute );
                pRoutePropDialog->UpdateProperties();
            }
        }
        break;
    }

    case ID_RT_MENU_DELETE: {
        int dlg_return = wxID_YES;
        if( g_bConfirmObjectDelete ) {
            dlg_return = OCPNMessageBox( this,  _("Are you sure you want to delete this route?"),
                _("OpenCPN Route Delete"), (long) wxYES_NO | wxCANCEL | wxYES_DEFAULT );
        }

        if( dlg_return == wxID_YES ) {
            if( g_pRouteMan->GetpActiveRoute() == m_pSelectedRoute ) g_pRouteMan->DeactivateRoute();

            if( m_pSelectedRoute->m_bIsInLayer ) break;

            pConfig->DeleteConfigRoute( m_pSelectedRoute );
            g_pRouteMan->DeleteRoute( m_pSelectedRoute );
            if( pRoutePropDialog && ( pRoutePropDialog->IsShown()) && (m_pSelectedRoute == pRoutePropDialog->GetRoute()) ) {
                pRoutePropDialog->Hide();
            }

            m_pSelectedRoute = NULL;
            m_pFoundRoutePoint = NULL;
            m_pFoundRoutePointSecond = NULL;

            if( pRouteManagerDialog && pRouteManagerDialog->IsShown() )
                pRouteManagerDialog->UpdateRouteListCtrl();

            if( pMarkPropDialog && pMarkPropDialog->IsShown() ) {
                pMarkPropDialog->ValidateMark();
                pMarkPropDialog->UpdateProperties();
            }

            undo->InvalidateUndo();

            InvalidateGL();
        }
        break;
    }

    case ID_RT_MENU_ACTIVATE: {
        if( g_pRouteMan->GetpActiveRoute() )
            g_pRouteMan->DeactivateRoute();
        
        //  If this is an auto-created MOB route, always select the second point (the MOB)
        // as the destination.
        RoutePoint *best_point;
        if(m_pSelectedRoute){
            if(wxNOT_FOUND == m_pSelectedRoute->m_RouteNameString.Find(_T("MOB")) ){
                best_point = g_pRouteMan->FindBestActivatePoint( m_pSelectedRoute, gLat,
                                 gLon, gCog, gSog );
            }
            else
                best_point = m_pSelectedRoute->GetPoint( 2 );
            
            g_pRouteMan->ActivateRoute( m_pSelectedRoute, best_point );
        }

        m_pSelectedRoute->m_bRtIsSelected = false;

        break;
    }

    case ID_RT_MENU_DEACTIVATE:
        g_pRouteMan->DeactivateRoute();
        m_pSelectedRoute->m_bRtIsSelected = false;

        break;

    case ID_RT_MENU_INSERT:

        if( m_pSelectedRoute->m_bIsInLayer ) break;

        m_pSelectedRoute->InsertPointBefore( m_pFoundRoutePointSecond, zlat, zlon );

        pSelect->DeleteAllSelectableRoutePoints( m_pSelectedRoute );
        pSelect->DeleteAllSelectableRouteSegments( m_pSelectedRoute );

        pSelect->AddAllSelectableRouteSegments( m_pSelectedRoute );
        pSelect->AddAllSelectableRoutePoints( m_pSelectedRoute );

        //    As a special case (which comes up often)...
        //    If the inserted waypoint is on the active leg of an active route
        /*            if(m_pSelectedRoute->m_bRtIsActive)
         {
         if(m_pSelectedRoute->m_nRouteActivePoint == np + 1)
         {
         pNew_Point = m_pSelectedRoute->GetPoint(np + 2);
         pRouteMan->ActivateRoutePoint(m_pSelectedRoute, pNew_Point);
         }
         }
         */
        m_pSelectedRoute->RebuildGUIDList();          // ensure the GUID list is intact and good
        pConfig->UpdateRoute( m_pSelectedRoute );

        if( pRoutePropDialog && ( pRoutePropDialog->IsShown() ) ) {
            pRoutePropDialog->SetRouteAndUpdate( m_pSelectedRoute, true );
        }

        break;

    case ID_RT_MENU_APPEND:

        if( m_pSelectedRoute->m_bIsInLayer ) break;

        m_pMouseRoute = m_pSelectedRoute;
        parent_frame->nRoute_State = m_pSelectedRoute->GetnPoints() + 1;
        m_pMouseRoute->m_lastMousePointIndex = m_pSelectedRoute->GetnPoints();

        pLast = m_pSelectedRoute->GetLastPoint();

        m_prev_rlat = pLast->m_lat;
        m_prev_rlon = pLast->m_lon;
        m_prev_pMousePoint = pLast;

        m_bAppendingRoute = true;

        SetCursor( *pCursorPencil );

        break;

    case ID_RT_MENU_COPY:
        if( m_pSelectedRoute ) Kml::CopyRouteToClipboard( m_pSelectedRoute );
        break;

    case ID_TK_MENU_COPY:
        if( m_pSelectedTrack ) Kml::CopyTrackToClipboard( (Track*)m_pSelectedTrack );
        break;

    case ID_WPT_MENU_COPY:
        if( m_pFoundRoutePoint ) Kml::CopyWaypointToClipboard( m_pFoundRoutePoint );
        break;

    case ID_WPT_MENU_SENDTOGPS:
        if( m_pFoundRoutePoint ) {
             if( m_active_upload_port.Length() )
                 m_pFoundRoutePoint->SendToGPS( m_active_upload_port.BeforeFirst(' '), NULL );
             else {
                 SendToGpsDlg dlg;
                 dlg.SetWaypoint( m_pFoundRoutePoint );

                 dlg.Create( NULL, -1, _( "Send To GPS..." ), _T("") );
                 dlg.ShowModal();
             }
        }
        break;

    case ID_WPT_MENU_SENDTONEWGPS:
        if( m_pFoundRoutePoint ) {
            SendToGpsDlg dlg;
            dlg.SetWaypoint( m_pFoundRoutePoint );
                
            dlg.Create( NULL, -1, _( "Send To GPS..." ), _T("") );
            dlg.ShowModal();
        }
        break;
        
    case ID_RT_MENU_SENDTOGPS:
        if( m_pSelectedRoute ) {
            if( m_active_upload_port.Length() )
                m_pSelectedRoute->SendToGPS( m_active_upload_port.BeforeFirst(' '), true, NULL );
            else {
                SendToGpsDlg dlg;
                dlg.SetRoute( m_pSelectedRoute );

                dlg.Create( NULL, -1, _( "Send To GPS..." ), _T("") );
                dlg.ShowModal();
            }

        }
        break;

    case ID_RT_MENU_SENDTONEWGPS:
        if( m_pSelectedRoute ) {
            SendToGpsDlg dlg;
            dlg.SetRoute( m_pSelectedRoute );
                
            dlg.Create( NULL, -1, _( "Send To GPS..." ), _T("") );
            dlg.ShowModal();
        }
        break;
        
    case ID_PASTE_WAYPOINT:
        pupHandler_PasteWaypoint();
        break;

    case ID_PASTE_ROUTE:
        pupHandler_PasteRoute();
        break;

    case ID_PASTE_TRACK:
        pupHandler_PasteTrack();
        break;

    case ID_RT_MENU_DELPOINT:
        if( m_pSelectedRoute ) {
            if( m_pSelectedRoute->m_bIsInLayer ) break;

            pWayPointMan->DestroyWaypoint( m_pFoundRoutePoint );
            m_pFoundRoutePoint = NULL;

            //    Selected route may have been deleted as one-point route, so check it
            if( !g_pRouteMan->IsRouteValid( m_pSelectedRoute ) ) m_pSelectedRoute = NULL;

            if( pRoutePropDialog && ( pRoutePropDialog->IsShown() ) ) {
                if( m_pSelectedRoute ) {
                    pRoutePropDialog->SetRouteAndUpdate( m_pSelectedRoute, true );
                }
                else
                    pRoutePropDialog->Hide();

            }

            if( pRouteManagerDialog && pRouteManagerDialog->IsShown() ) {
                pRouteManagerDialog->UpdateWptListCtrl();
                pRouteManagerDialog->UpdateRouteListCtrl();
            }

            InvalidateGL();
        }

        break;

    case ID_RT_MENU_REMPOINT:
        if( m_pSelectedRoute ) {
            if( m_pSelectedRoute->m_bIsInLayer ) break;
            RemovePointFromRoute( m_pFoundRoutePoint, m_pSelectedRoute );
            InvalidateGL();
        }
        break;

    case ID_RT_MENU_ACTPOINT:
        if( g_pRouteMan->GetpActiveRoute() == m_pSelectedRoute ) {
            g_pRouteMan->ActivateRoutePoint( m_pSelectedRoute, m_pFoundRoutePoint );
            m_pSelectedRoute->m_bRtIsSelected = false;
        }

        break;

    case ID_RT_MENU_DEACTPOINT:
        break;

    case ID_RT_MENU_ACTNXTPOINT:
        if( g_pRouteMan->GetpActiveRoute() == m_pSelectedRoute ) {
            g_pRouteMan->ActivateNextPoint( m_pSelectedRoute, true );
            m_pSelectedRoute->m_bRtIsSelected = false;
        }

        break;

    case ID_RT_MENU_PROPERTIES: {
        ShowRoutePropertiesDialog( _("Route Properties"), m_pSelectedRoute );
        break;
    }

    case ID_TK_MENU_PROPERTIES: {
        ShowTrackPropertiesDialog( m_pSelectedTrack );
        break;
    }

    case ID_TK_MENU_DELETE: {
        int dlg_return = wxID_YES;
        if( g_bConfirmObjectDelete ) {
            dlg_return = OCPNMessageBox( this, _("Are you sure you want to delete this track?"),
                _("OpenCPN Track Delete"), (long) wxYES_NO | wxCANCEL | wxYES_DEFAULT );
        }

        if( dlg_return == wxID_YES ) {

            if( (Track *) ( m_pSelectedTrack ) == g_pActiveTrack ) parent_frame->TrackOff();

            pConfig->DeleteConfigRoute( m_pSelectedTrack );

            g_pRouteMan->DeleteTrack( m_pSelectedTrack );

            if( pTrackPropDialog && ( pTrackPropDialog->IsShown()) && (m_pSelectedTrack == pTrackPropDialog->GetTrack()) ) {
                pTrackPropDialog->Hide();
            }

            m_pSelectedTrack = NULL;
            m_pFoundRoutePoint = NULL;
            m_pFoundRoutePointSecond = NULL;

            if( pRouteManagerDialog && pRouteManagerDialog->IsShown() ) {
                pRouteManagerDialog->UpdateTrkListCtrl();
                pRouteManagerDialog->UpdateRouteListCtrl();
            }
            InvalidateGL();
        }
        break;
    }

    case ID_RC_MENU_SCALE_IN:
        parent_frame->DoStackDown();
        GetCanvasPointPix( zlat, zlon, &r );
        WarpPointer( r.x, r.y );
        break;

    case ID_RC_MENU_SCALE_OUT:
        parent_frame->DoStackUp();
        GetCanvasPointPix( zlat, zlon, &r );
        WarpPointer( r.x, r.y );
        break;

    case ID_RC_MENU_ZOOM_IN:
        SetVPScale( GetVPScale() * 2 );
        GetCanvasPointPix( zlat, zlon, &r );
        WarpPointer( r.x, r.y );
        break;

    case ID_RC_MENU_ZOOM_OUT:
        SetVPScale( GetVPScale() / 2 );
        GetCanvasPointPix( zlat, zlon, &r );
        WarpPointer( r.x, r.y );
        break;

    case ID_RC_MENU_FINISH:
        FinishRoute();
        gFrame->SurfaceToolbar();
        Refresh( false );
        break;

    default: {
        //  Look for PlugIn Context Menu selections
        //  If found, make the callback
        ArrayOfPlugInMenuItems item_array = g_pi_manager->GetPluginContextMenuItemArray();

        for( unsigned int i = 0; i < item_array.GetCount(); i++ ) {
            PlugInMenuItemContainer *pimis = item_array.Item( i );
            {
                if( pimis->id == event.GetId() ) {
                    if( pimis->m_pplugin ) pimis->m_pplugin->OnContextMenuItemCallback(
                            pimis->id );
                }
            }
        }

        break;
    }
    }           // switch

    //  Chart Groups....
    if( ( event.GetId() >= ID_DEF_MENU_GROUPBASE )
            && ( event.GetId() <= ID_DEF_MENU_GROUPBASE + (int) g_pGroupArray->GetCount() ) ) {
        gFrame->SetGroupIndex( event.GetId() - ID_DEF_MENU_GROUPBASE );
    }

    g_click_stop = 0;    // Context menu was processed, all is well

}

void ChartCanvas::FinishRoute( void )
{
    parent_frame->nRoute_State = 0;
    m_prev_pMousePoint = NULL;

    parent_frame->SetToolbarItemState( ID_ROUTE, false );
    SetCursor( *pCursorArrow );
    m_bDrawingRoute = false;

    if( m_pMouseRoute ) {
        if( m_bAppendingRoute ) 
            pConfig->UpdateRoute( m_pMouseRoute );
        else {
            if( m_pMouseRoute->GetnPoints() > 1 ) {
                pConfig->AddNewRoute( m_pMouseRoute, -1 );    // use auto next num
            } else {
                g_pRouteMan->DeleteRoute( m_pMouseRoute );
                m_pMouseRoute = NULL;
            }

            if( m_pMouseRoute )
                m_pMouseRoute->RebuildGUIDList(); // ensure the GUID list is intact and good
        }
        if( m_pMouseRoute ){
            m_pMouseRoute->RebuildGUIDList(); // ensure the GUID list is intact and good
            m_pMouseRoute->SetHiLite(0);
        }
            

        if( pRoutePropDialog && ( pRoutePropDialog->IsShown() ) ) {
            pRoutePropDialog->SetRouteAndUpdate( m_pMouseRoute, true );
        }

        if( pRouteManagerDialog && pRouteManagerDialog->IsShown() )
            pRouteManagerDialog->UpdateRouteListCtrl();

    }
    m_bAppendingRoute = false;
    m_pMouseRoute = NULL;

    m_pSelectedRoute = NULL;
    m_pFoundRoutePointSecond = NULL;

    undo->InvalidateUndo();
    Refresh(true);
}

void ChartCanvas::ShowAISTargetList( void )
{
    if( NULL == g_pAISTargetList ) {         // There is one global instance of the Dialog
        g_pAISTargetList = new AISTargetListDialog( parent_frame, g_pauimgr, g_pAIS );
    }

    g_pAISTargetList->UpdateAISTargetList();

}

void ChartCanvas::RenderAllChartOutlines( ocpnDC &dc, ViewPort& vp )
{
    if( !g_bShowOutlines ) return;

    int nEntry = ChartData->GetChartTableEntries();

    for( int i = 0; i < nEntry; i++ ) {
        ChartTableEntry *pt = (ChartTableEntry *) &ChartData->GetChartTableEntry( i );

        //    Check to see if the candidate chart is in the currently active group
        bool b_group_draw = false;
        if( g_GroupIndex > 0 ) {
            for( unsigned int ig = 0; ig < pt->GetGroupArray().GetCount(); ig++ ) {
                int index = pt->GetGroupArray().Item( ig );
                if( g_GroupIndex == index ) {
                    b_group_draw = true;
                    break;
                }
            }
        } else
            b_group_draw = true;

        if( b_group_draw ) RenderChartOutline( dc, i, vp );
    }

#ifdef USE_S57
    //        On CM93 Composite Charts, draw the outlines of the next smaller scale cell
    if( Current_Ch && ( Current_Ch->GetChartType() == CHART_TYPE_CM93COMP ) ) {
        cm93compchart *pch = (cm93compchart *) Current_Ch;
        if( pch ) {
            double chart_native_ppm = m_canvas_scale_factor / Current_Ch->GetNativeScale();
            double zoom_factor = GetVP().view_scale_ppm / chart_native_ppm;

            if( zoom_factor > 8.0 ) {
                wxPen mPen( GetGlobalColor( _T("UINFM") ), 2, wxSHORT_DASH );
                dc.SetPen( mPen );
                pch->RenderNextSmallerCellOutlines( dc, GetVP() );
            } else {
                wxPen mPen( GetGlobalColor( _T("UINFM") ), 1, wxSOLID );
                dc.SetPen( mPen );
            } 

            pch->RenderNextSmallerCellOutlines( dc, vp );
        }
    }
#endif
}

void ChartCanvas::RenderChartOutline( ocpnDC &dc, int dbIndex, ViewPort& vp )
{
#ifdef ocpnUSE_GL
    if(g_bopengl) {
        /* opengl version specially optimized */
        m_glcc->RenderChartOutline(dbIndex, vp);
        return;
    }
#endif

    float plylat, plylon;
    float plylat1, plylon1;

    int pixx, pixy, pixx1, pixy1;
    bool b_draw = false;
    double lon_bias = 0.;

    wxBoundingBox box;
    ChartData->GetDBBoundingBox( dbIndex, &box );

    // Don't draw an outline in the case where the chart covers the entire world */
    double lon_diff = box.GetMaxX() - box.GetMinX();
    if(lon_diff == 360)
        return;

    if( !vp.GetBBox().IntersectOut( box ) )              // chart is not outside of viewport
        b_draw = true;

    //  Does simple test fail, and current vp cross international dateline?
    if( !b_draw && ( ( vp.GetBBox().GetMinX() < -180. ) || ( vp.GetBBox().GetMaxX() > 180. ) ) ) {
        //  If so, do an explicit test with alternate phasing
        if( vp.GetBBox().GetMinX() < -180. ) {
            wxPoint2DDouble p( -360., 0 );
            box.Translate( p );
            if( !vp.GetBBox().IntersectOut( box ) )       // chart is not outside of viewport
            {
                b_draw = true;
                lon_bias = -360.;
            }
        } else {
            wxPoint2DDouble p( 360., 0 );
            box.Translate( p );
            if( !vp.GetBBox().IntersectOut( box ) )       // chart is not outside of viewport
            {
                b_draw = true;
                lon_bias = 360.;
            }
        }

    }

    //  Does simple test fail, and chart box cross international dateline?
    if( !b_draw && ( box.GetMinX() < 180. ) && ( box.GetMaxX() > 180. ) ) {
        wxPoint2DDouble p( -360., 0 );
        box.Translate( p );
        if( !vp.GetBBox().IntersectOut( box )  )           // chart is not outside of viewport
        {
            b_draw = true;
            lon_bias = -360.;
        }
    }

    if( !b_draw ) return;

    int nPly = ChartData->GetDBPlyPoint( dbIndex, 0, &plylat, &plylon );

    if( ChartData->GetDBChartType( dbIndex ) == CHART_TYPE_CM93 )
        dc.SetPen( wxPen( GetGlobalColor( _T ( "YELO1" ) ), 1, wxSOLID ) );

    else if( ChartData->GetDBChartFamily( dbIndex ) == CHART_FAMILY_VECTOR )
        dc.SetPen( wxPen( GetGlobalColor( _T ( "UINFG" ) ), 1, wxSOLID ) );

    else
        dc.SetPen( wxPen( GetGlobalColor( _T ( "UINFR" ) ), 1, wxSOLID ) );

    //        Are there any aux ply entries?
    int nAuxPlyEntries = ChartData->GetnAuxPlyEntries( dbIndex );
    if( 0 == nAuxPlyEntries )                 // There are no aux Ply Point entries
    {
        wxPoint r, r1;

        ChartData->GetDBPlyPoint( dbIndex, 0, &plylat, &plylon );
        plylon += lon_bias;

        GetCanvasPointPix( plylat, plylon, &r );
        pixx = r.x;
        pixy = r.y;

        for( int i = 0; i < nPly - 1; i++ ) {
            ChartData->GetDBPlyPoint( dbIndex, i + 1, &plylat1, &plylon1 );
            plylon1 += lon_bias;

            GetCanvasPointPix( plylat1, plylon1, &r1 );
            pixx1 = r1.x;
            pixy1 = r1.y;

            int pixxs1 = pixx1;
            int pixys1 = pixy1;

            bool b_skip = false;

            if( vp.chart_scale > 5e7 ) {
                //    calculate projected distance between these two points in meters
                double dist = sqrt( pow( (double) (pixx1 - pixx), 2 ) +
                                    pow( (double) (pixy1 - pixy), 2 ) ) / vp.view_scale_ppm;
                //    calculate GC distance between these two points in meters
                double distgc = DistGreatCircle( plylat, plylon, plylat1, plylon1 ) * 1852.;

                //    If the distances are nonsense, it means that the scale is very small and the segment wrapped the world
                //    So skip it....
                //    TODO improve this to draw two segments
                if( fabs( dist - distgc ) > 10000. * 1852. )          //lotsa miles
                    b_skip = true;
            }

            ClipResult res = cohen_sutherland_line_clip_i( &pixx, &pixy, &pixx1, &pixy1, 0,
                             vp.pix_width, 0, vp.pix_height );
            if( res != Invisible && !b_skip ) dc.DrawLine( pixx, pixy, pixx1, pixy1, false );

            plylat = plylat1;
            plylon = plylon1;
            pixx = pixxs1;
            pixy = pixys1;
        }

        ChartData->GetDBPlyPoint( dbIndex, 0, &plylat1, &plylon1 );
        plylon1 += lon_bias;

        GetCanvasPointPix( plylat1, plylon1, &r1 );
        pixx1 = r1.x;
        pixy1 = r1.y;

        ClipResult res = cohen_sutherland_line_clip_i( &pixx, &pixy, &pixx1, &pixy1, 0,
                         vp.pix_width, 0, vp.pix_height );
        if( res != Invisible ) dc.DrawLine( pixx, pixy, pixx1, pixy1, false );
    }

    else                              // Use Aux PlyPoints
    {
        wxPoint r, r1;

        int nAuxPlyEntries = ChartData->GetnAuxPlyEntries( dbIndex );
        for( int j = 0; j < nAuxPlyEntries; j++ ) {

            int nAuxPly = ChartData->GetDBAuxPlyPoint( dbIndex, 0, j, &plylat, &plylon );
            GetCanvasPointPix( plylat, plylon, &r );
            pixx = r.x;
            pixy = r.y;

            for( int i = 0; i < nAuxPly - 1; i++ ) {
                ChartData->GetDBAuxPlyPoint( dbIndex, i + 1, j, &plylat1, &plylon1 );

                GetCanvasPointPix( plylat1, plylon1, &r1 );
                pixx1 = r1.x;
                pixy1 = r1.y;

                int pixxs1 = pixx1;
                int pixys1 = pixy1;

                bool b_skip = false;

                if( vp.chart_scale > 5e7 ) {
                    //    calculate projected distance between these two points in meters
                    double dist = sqrt(
                                      (double) ( ( pixx1 - pixx ) * ( pixx1 - pixx ) )
                                      + ( ( pixy1 - pixy ) * ( pixy1 - pixy ) ) ) / vp.view_scale_ppm;
                    //    calculate GC distance between these two points in meters
                    double distgc = DistGreatCircle( plylat, plylon, plylat1, plylon1 ) * 1852.;

                    //    If the distances are nonsense, it means that the scale is very small and the segment wrapped the world
                    //    So skip it....
                    //    TODO improve this to draw two segments
                    if( fabs( dist - distgc ) > 10000. * 1852. )          //lotsa miles
                        b_skip = true;
                }

                ClipResult res = cohen_sutherland_line_clip_i( &pixx, &pixy, &pixx1, &pixy1, 0,
                                 vp.pix_width, 0, vp.pix_height );
                if( res != Invisible && !b_skip ) dc.DrawLine( pixx, pixy, pixx1, pixy1 );

                plylat = plylat1;
                plylon = plylon1;
                pixx = pixxs1;
                pixy = pixys1;
            }

            ChartData->GetDBAuxPlyPoint( dbIndex, 0, j, &plylat1, &plylon1 );
            GetCanvasPointPix( plylat1, plylon1, &r1 );
            pixx1 = r1.x;
            pixy1 = r1.y;

            ClipResult res = cohen_sutherland_line_clip_i( &pixx, &pixy, &pixx1, &pixy1, 0,
                             vp.pix_width, 0, vp.pix_height );
            if( res != Invisible ) dc.DrawLine( pixx, pixy, pixx1, pixy1, false );
        }
    }

}

bool ChartCanvas::PurgeGLCanvasChartCache( ChartBase *pc, bool b_purge_full )
{
#ifdef ocpnUSE_GL
    if( g_bopengl && m_glcc )
        m_glcc->PurgeChartTextures( pc, b_purge_full );
#endif
    return true;
}

wxString ChartCanvas::FormatDistanceAdaptive( double distance ) {
    wxString result;
    int unit = g_iDistanceFormat;
    double usrDistance = toUsrDistance( distance, unit );
    if( usrDistance < 0.1 &&  ( unit == DISTANCE_KM || unit == DISTANCE_MI || unit == DISTANCE_NMI ) ) {
	unit = ( unit == DISTANCE_MI ) ? DISTANCE_FT : DISTANCE_M;
	usrDistance = toUsrDistance( distance, unit );
    }
    wxString format;
    if( usrDistance < 5.0 ) {
        format = _T("%1.2f ");
    } else if( usrDistance < 100.0 ) {
        format = _T("%2.1f ");
    } else if( usrDistance < 1000.0 ) {
        format = _T("%3.0f ");
    } else {
        format = _T("%4.0f ");
    }
    result << wxString::Format(format, usrDistance ) << getUsrDistanceUnit( unit );
    return result;
}

void RenderExtraRouteLegInfo( ocpnDC &dc, wxPoint ref_point, wxString s )
{
    wxFont *dFont = FontMgr::Get().GetFont( _("RouteLegInfoRollover"), 12 );
    dc.SetFont( *dFont );

    int w, h;
    int xp, yp;
    int hilite_offset = 3;
#ifdef __WXMAC__
    wxScreenDC sdc;
    sdc.GetTextExtent(s, &w, &h, NULL, NULL, dFont);
#else
    dc.GetTextExtent( s, &w, &h );
#endif

    xp = ref_point.x - w;
    yp = ref_point.y + h;
    yp += hilite_offset;

    AlphaBlending( dc, xp, yp, w, h, 0.0, GetGlobalColor( _T ( "YELO1" ) ), 172 );

    dc.SetPen( wxPen( GetGlobalColor( _T ( "UBLCK" ) ) ) );
    dc.DrawText( s, xp, yp );
}

void ChartCanvas::RenderRouteLegs( ocpnDC &dc )
{
    if( (parent_frame->nRoute_State >= 2) ||
        (m_pMeasureRoute && m_bMeasure_Active && ( m_nMeasureState >= 2 )) ) {

        double rhumbBearing, rhumbDist, gcBearing, gcBearing2, gcDist;
        DistanceBearingMercator( m_cursor_lat, m_cursor_lon, m_prev_rlat, m_prev_rlon, &rhumbBearing, &rhumbDist );
        Geodesic::GreatCircleDistBear( m_prev_rlon, m_prev_rlat, m_cursor_lon, m_cursor_lat, &gcDist, &gcBearing, &gcBearing2);
        double gcDistm = gcDist / 1852.0;

        if( ( m_prev_rlat == m_cursor_lat ) && ( m_prev_rlon == m_cursor_lon ) ) rhumbBearing = 90.;

        wxPoint destPoint, lastPoint;
        Route* route;
        int state;

        if( m_pMeasureRoute ) {
            route = m_pMeasureRoute;
            state = m_nMeasureState;
        } else {
            route = m_pMouseRoute;
            state = parent_frame->nRoute_State;
        }

        double brg = rhumbBearing;
        double dist = rhumbDist;
        route->m_NextLegGreatCircle = false;
        int milesDiff = rhumbDist - gcDistm;
        if( milesDiff > 1 ) {
            brg = gcBearing;
            dist = gcDistm;
            route->m_NextLegGreatCircle = true;
        }

        if( !g_btouch) {
            route->DrawPointWhich( dc, route->m_lastMousePointIndex, &lastPoint );

            if( route->m_NextLegGreatCircle ) {
                for( int i=1; i<=milesDiff; i++ ) {
                    double p = (double)i * (1.0/(double)milesDiff);
                    double pLat, pLon;
                    Geodesic::GreatCircleTravel( m_prev_rlon, m_prev_rlat, gcDist*p, brg, &pLon, &pLat, &gcBearing2 );
                    destPoint = VPoint.GetPixFromLL( pLat, pLon );
                    route->DrawSegment( dc, &lastPoint, &destPoint, GetVP(), false );
                    lastPoint = destPoint;
                }
            }
            else {
                route->DrawSegment( dc, &lastPoint, &r_rband, GetVP(), false );
            }
        }

        wxString routeInfo;
        if( g_bShowMag )
            routeInfo << wxString::Format( wxString("%03d°(M)  ", wxConvUTF8 ), (int)gFrame->GetTrueOrMag( brg ) );
        else
            routeInfo << wxString::Format( wxString("%03d°  ", wxConvUTF8 ), (int)gFrame->GetTrueOrMag( brg ) );

        routeInfo << _T(" ") << FormatDistanceAdaptive( dist );

        wxFont *dFont = FontMgr::Get().GetFont( _("RouteLegInfoRollover"), 12 );
        dc.SetFont( *dFont );

        int w, h;
        int xp, yp;
        int hilite_offset = 3;
    #ifdef __WXMAC__
        wxScreenDC sdc;
        sdc.GetTextExtent(routeInfo, &w, &h, NULL, NULL, dFont);
    #else
        dc.GetTextExtent( routeInfo, &w, &h );
    #endif
        xp = r_rband.x - w;
        yp = r_rband.y;
        yp += hilite_offset;

        AlphaBlending( dc, xp, yp, w, h, 0.0, GetGlobalColor( _T ( "YELO1" ) ), 172 );

        dc.SetPen( wxPen( GetGlobalColor( _T ( "UBLCK" ) ) ) );
        dc.DrawText( routeInfo, xp, yp );

        wxString s0;
        if( !route->m_bIsInLayer )
            s0.Append( _("Route: ") );
        else
            s0.Append( _("Layer Route: ") );

        s0 += FormatDistanceAdaptive( route->m_route_length + dist );
        RenderExtraRouteLegInfo( dc, r_rband, s0 );
    }
}

void ChartCanvas::WarpPointerDeferred( int x, int y )
{
    warp_x = x;
    warp_y = y;
    warp_flag = true;
}

int spaint;
int s_in_update;
void ChartCanvas::OnPaint( wxPaintEvent& event )
{
    wxPaintDC dc( this );

    //  Paint updates may have been externally disabled (temporarily, to avoid Yield() recursion performance loss)
    //  It is important that the wxPaintDC is built, even if we elect to not process this paint message.
    //  Otherwise, the paint message may not be removed from the message queue, esp on Windows. (FS#1213)
    //  This would lead to a deadlock condition in ::wxYield()

    if(!m_b_paint_enable)
        return;

#ifdef ocpnUSE_GL
    if( !g_bdisable_opengl )
        m_glcc->Show( g_bopengl );

    if( g_bopengl ) {
        if( !s_in_update ) {          // no recursion allowed, seen on lo-spec Mac
            s_in_update++;
            m_glcc->Update();
            s_in_update--;
        }

        return;
    }
#endif

    if( ( GetVP().pix_width == 0 ) || ( GetVP().pix_height == 0 ) ) return;

    wxRegion ru = GetUpdateRegion();

    int rx, ry, rwidth, rheight;
    ru.GetBox( rx, ry, rwidth, rheight );
//        printf("%d Onpaint update region box: %d %d %d %d\n", spaint++, rx, ry, rwidth, rheight);

    wxBoundingBox BltBBox;

#ifdef ocpnUSE_DIBSECTION
    ocpnMemDC temp_dc;
#else
    wxMemoryDC temp_dc;
#endif

    wxRegion rgn_chart( 0, 0, GetVP().pix_width, GetVP().pix_height );

//    In case Thumbnail is shown, set up dc clipper and blt iterator regions
    if( pthumbwin ) {
        int thumbx, thumby, thumbsx, thumbsy;
        pthumbwin->GetPosition( &thumbx, &thumby );
        pthumbwin->GetSize( &thumbsx, &thumbsy );
        wxRegion rgn_thumbwin( thumbx, thumby, thumbsx - 1, thumbsy - 1 );

        if( pthumbwin->IsShown() ) {
            rgn_chart.Subtract( rgn_thumbwin );
            ru.Subtract( rgn_thumbwin );
        }
    }

    //  Is this viewpoint the same as the previously painted one?
    bool b_newview = true;

    if( ( m_cache_vp.view_scale_ppm == VPoint.view_scale_ppm )
            && ( m_cache_vp.rotation == VPoint.rotation ) && ( m_cache_vp.clat == VPoint.clat )
            && ( m_cache_vp.clon == VPoint.clon ) && m_cache_vp.IsValid() ) {
        b_newview = false;
    }

    //  If the ViewPort is rotated, we may be able to use the cached rotated bitmap
    bool b_rcache_ok = false;
    if( fabs( VPoint.rotation ) > 0.01 ) b_rcache_ok = !b_newview;

    //  If in skew compensation mode, with a skewed VP shown, we may be able to use the cached rotated bitmap
    if( g_bskew_comp && ( fabs( VPoint.skew ) > 0.01 ) ) b_rcache_ok = !b_newview;

    //  Make a special VP
    if( VPoint.b_MercatorProjectionOverride ) VPoint.SetProjectionType( PROJECTION_MERCATOR );
    ViewPort svp = VPoint;

    svp.pix_width = svp.rv_rect.width;
    svp.pix_height = svp.rv_rect.height;

//        printf("Onpaint pix %d %d\n", VPoint.pix_width, VPoint.pix_height);
//        printf("OnPaint rv_rect %d %d\n", VPoint.rv_rect.width, VPoint.rv_rect.height);

    OCPNRegion chart_get_region( wxRect( 0, 0, svp.pix_width, svp.pix_height ) );

    //  If we are going to use the cached rotated image, there is no need to fetch any chart data
    //  and this will do it...
    if( b_rcache_ok ) chart_get_region.Clear();

    //  Blit pan acceleration
    if( VPoint.b_quilt )          // quilted
    {
        if( m_pQuilt && !m_pQuilt->IsComposed() ) return;

        if( ( m_working_bm.GetWidth() != svp.pix_width )
                || ( m_working_bm.GetHeight() != svp.pix_height ) ) m_working_bm.Create(
                        svp.pix_width, svp.pix_height, -1 ); // make sure the target is big enoug

        if( fabs( VPoint.rotation ) < 0.01 ) {
            bool b_save = true;

            //  If the saved wxBitmap from last OnPaint is useable
            //  calculate the blit parameters

            //  We can only do screen blit painting if subsequent ViewPorts differ by whole pixels
            //  So, in small scale bFollow mode, force the full screen render.
            //  This seems a hack....There may be better logic here.....

//                  if(m_bFollow)
//                        b_save = false;

            if( m_bm_cache_vp.IsValid() && m_cache_vp.IsValid() /*&& !m_bFollow*/) {
                if( b_newview ) {
                    wxPoint c_old = VPoint.GetPixFromLL( VPoint.clat, VPoint.clon );
                    wxPoint c_new = m_bm_cache_vp.GetPixFromLL( VPoint.clat, VPoint.clon );

                    int dy = c_new.y - c_old.y;
                    int dx = c_new.x - c_old.x;

//                              printf("In OnPaint Trying Blit dx: %d  dy:%d\n\n", dx, dy);

                    if( m_pQuilt->IsVPBlittable( VPoint, dx, dy, true ) ) {
                        if( dx || dy ) {
                            //  Blit the reuseable portion of the cached wxBitmap to a working bitmap
                            temp_dc.SelectObject( m_working_bm );

                            wxMemoryDC cache_dc;
                            cache_dc.SelectObject( m_cached_chart_bm );

                            if( dy > 0 ) {
                                if( dx > 0 ) {
                                    temp_dc.Blit( 0, 0, VPoint.pix_width - dx,
                                                               VPoint.pix_height - dy, &cache_dc, dx, dy );
                                }
                                else {
                                    temp_dc.Blit( -dx, 0, VPoint.pix_width + dx,
                                                  VPoint.pix_height - dy, &cache_dc, 0, dy );
                                }

                            } else {
                                if( dx > 0 ) {
                                    temp_dc.Blit( 0, -dy, VPoint.pix_width - dx,
                                                               VPoint.pix_height + dy, &cache_dc, dx, 0 );
                                }
                                else {
                                    temp_dc.Blit( -dx, -dy, VPoint.pix_width + dx,
                                                  VPoint.pix_height + dy, &cache_dc, 0, 0 );
                                }
                            }

                            OCPNRegion update_region;
                            if( dy ) {
                                if( dy > 0 ) update_region.Union(
                                        wxRect( 0, VPoint.pix_height - dy, VPoint.pix_width, dy ) );
                                else
                                    update_region.Union( wxRect( 0, 0, VPoint.pix_width, -dy ) );
                            }

                            if( dx ) {
                                if( dx > 0 ) update_region.Union(
                                        wxRect( VPoint.pix_width - dx, 0, dx, VPoint.pix_height ) );
                                else
                                    update_region.Union( wxRect( 0, 0, -dx, VPoint.pix_height ) );
                            }

                            //  Render the new region
                            m_pQuilt->RenderQuiltRegionViewOnDC( temp_dc, svp, update_region );
                            cache_dc.SelectObject( wxNullBitmap );
                        } else {
                            //    No sensible (dx, dy) change in the view, so use the cached member bitmap
                            temp_dc.SelectObject( m_cached_chart_bm );
                            b_save = false;

                        }
                        m_pQuilt->ComputeRenderRegion( svp, chart_get_region );

                    } else              // not blitable
                    {
                        temp_dc.SelectObject( m_working_bm );
                        m_pQuilt->RenderQuiltRegionViewOnDC( temp_dc, svp, chart_get_region );
                    }
                } else {
                    //    No change in the view, so use the cached member bitmap2
                    temp_dc.SelectObject( m_cached_chart_bm );
                    b_save = false;
                }
            } else      //cached bitmap is not yet valid
            {
                temp_dc.SelectObject( m_working_bm );
                m_pQuilt->RenderQuiltRegionViewOnDC( temp_dc, svp, chart_get_region );
            }

            //  Save the fully rendered quilt image as a wxBitmap member of this class
            if( b_save ) {
//                        if((m_cached_chart_bm.GetWidth() != svp.pix_width) || (m_cached_chart_bm.GetHeight() != svp.pix_height))
//                              m_cached_chart_bm.Create(svp.pix_width, svp.pix_height, -1); // target wxBitmap is big enough
                wxMemoryDC scratch_dc_0;
                scratch_dc_0.SelectObject( m_cached_chart_bm );
                scratch_dc_0.Blit( 0, 0, svp.pix_width, svp.pix_height, &temp_dc, 0, 0 );

                scratch_dc_0.SelectObject( wxNullBitmap );

                m_bm_cache_vp = VPoint;     // save the ViewPort associated with the cached wxBitmap
            }
        }

        else            // quilted, rotated
        {
            temp_dc.SelectObject( m_working_bm );
            OCPNRegion chart_get_all_region( wxRect( 0, 0, svp.pix_width, svp.pix_height ) );
            m_pQuilt->RenderQuiltRegionViewOnDC( temp_dc, svp, chart_get_all_region );
        }
    }

    else                  // not quilted
    {
        if( !Current_Ch ) {
            dc.SetBackground( wxBrush( *wxLIGHT_GREY ) );
            dc.Clear();
            return;
        }

        if(!chart_get_region.IsEmpty()){
            Current_Ch->RenderRegionViewOnDC( temp_dc, svp, chart_get_region );
        }            
    }
    
    if( !temp_dc.IsOk() ) return;

//    Arrange to render the World Chart vector data behind the rendered current chart
//    so that uncovered canvas areas show at least the world chart.
    OCPNRegion chartValidRegion;
    if( !VPoint.b_quilt )
        Current_Ch->GetValidCanvasRegion( svp, &chartValidRegion ); // Make a region covering the current chart on the canvas
    else
        chartValidRegion = m_pQuilt->GetFullQuiltRenderedRegion();

    temp_dc.DestroyClippingRegion();
    
    //    Copy current chart region
    OCPNRegion backgroundRegion(  0, 0, svp.pix_width, svp.pix_height  );

    if( chartValidRegion.IsOk() )
        backgroundRegion.Subtract( chartValidRegion );

    if( ( ( fabs( GetVP().skew ) < .01 ) || ! g_bskew_comp )
        && ! backgroundRegion.IsEmpty() ) {
        
        //    Associate with temp_dc
        wxRegion *clip_region = backgroundRegion.GetNew_wxRegion();
        temp_dc.SetClippingRegion( *clip_region );
        delete clip_region;

    //    Draw the Background Chart only in the areas NOT covered by the current chart view

        /* unfortunately wxDC::DrawRectangle and wxDC::Clear do not respect
           clipping regions with more than 1 rectangle so... */
        wxColour water = cc1->pWorldBackgroundChart->water;
        temp_dc.SetPen( *wxTRANSPARENT_PEN );
        temp_dc.SetBrush( wxBrush( water ) );
        OCPNRegionIterator upd( backgroundRegion ); // get the update rect list
        while( upd.HaveRects() ) {
            wxRect rect = upd.GetRect();
            temp_dc.DrawRectangle(rect);
            upd.NextRect();
        }

        ocpnDC bgdc( temp_dc );
        double r =         VPoint.rotation;
        SetVPRotation( 0.0 );
        pWorldBackgroundChart->RenderViewOnDC( bgdc, VPoint );
        SetVPRotation( r );
    }

    wxMemoryDC *pChartDC = &temp_dc;
    wxMemoryDC rotd_dc;

    if( ( ( fabs( GetVP().rotation ) > 0.01 ) )
            || ( g_bskew_comp && ( fabs( GetVP().skew ) > 0.01 ) ) ) {

        //  Can we use the current rotated image cache?
        if( !b_rcache_ok ) {
#ifdef __WXMSW__
            wxMemoryDC tbase_dc;
            wxBitmap bm_base( svp.pix_width, svp.pix_height, -1 );
            tbase_dc.SelectObject( bm_base );
            tbase_dc.Blit( 0, 0, svp.pix_width, svp.pix_height, &temp_dc, 0, 0 );
            tbase_dc.SelectObject( wxNullBitmap );
#else
            const wxBitmap &bm_base = temp_dc.GetSelectedBitmap();
#endif

            wxImage base_image;
            if( bm_base.IsOk() ) base_image = bm_base.ConvertToImage();

            //    Use a local static image rotator to improve wxWidgets code profile
            //    Especially, on GTK the wxRound and wxRealPoint functions are very expensive.....
            double angle;
            angle = -GetVP().rotation;
            if(g_bskew_comp)
                angle += GetVP().skew;

            wxImage ri;
            bool b_rot_ok = false;
            if( base_image.IsOk() ) {
                ViewPort rot_vp = GetVP();

                m_b_rot_hidef = false;
//                              if(g_bskew_comp && (fabs(GetVP().skew) > 0.01))
//                                    m_b_rot_hidef = true;

                ri = Image_Rotate( base_image, angle,
                                   wxPoint( GetVP().rv_rect.width / 2, GetVP().rv_rect.height / 2 ),
                                   m_b_rot_hidef, &m_roffset );

                if( ( rot_vp.view_scale_ppm == VPoint.view_scale_ppm )
                        && ( rot_vp.rotation == VPoint.rotation ) && ( rot_vp.clat == VPoint.clat )
                        && ( rot_vp.clon == VPoint.clon ) && rot_vp.IsValid() && ( ri.IsOk() ) ) {
                    b_rot_ok = true;
                }
            }

            if( b_rot_ok ) {
                delete m_prot_bm;
                m_prot_bm = new wxBitmap( ri );

            }

            m_roffset.x += VPoint.rv_rect.x;
            m_roffset.y += VPoint.rv_rect.y;

        }

        if( m_prot_bm && m_prot_bm->IsOk() ) {
            rotd_dc.SelectObject( *m_prot_bm );
            pChartDC = &rotd_dc;
        } else {
            pChartDC = &temp_dc;
            m_roffset = wxPoint( 0, 0 );
        }

    } else {
        pChartDC = &temp_dc;
        m_roffset = wxPoint( 0, 0 );

    }

    wxPoint offset = m_roffset;

    //        Save the PixelCache viewpoint for next time
    m_cache_vp = VPoint;

//    Set up a scratch DC for overlay objects
    wxRegion rgn_blit;
    wxMemoryDC mscratch_dc;
    mscratch_dc.SelectObject( *pscratch_bm );

    mscratch_dc.ResetBoundingBox();
    mscratch_dc.DestroyClippingRegion();
    mscratch_dc.SetClippingRegion( rgn_chart );

    //    Blit the externally invalidated areas of the chart onto the scratch dc
    rgn_blit = ru;
    wxRegionIterator upd( rgn_blit ); // get the update rect list
    while( upd ) {
        wxRect rect = upd.GetRect();

        mscratch_dc.Blit( rect.x, rect.y, rect.width, rect.height, pChartDC, rect.x - offset.x,
                          rect.y - offset.y );
        upd++;
    }

//    Draw the rest of the overlay objects directly on the scratch dc
    ocpnDC scratch_dc( mscratch_dc );
    DrawOverlayObjects( scratch_dc, ru );

    if( m_bShowTide ) DrawAllTidesInBBox( scratch_dc, GetVP().GetBBox(), true, true );

    if( m_bShowCurrent ) DrawAllCurrentsInBBox( scratch_dc, GetVP().GetBBox(), true, true );

#if 0
//  Using yet another bitmap and DC, draw semi-static overlay objects if necessary

    /*    Why go to all this trouble?
     Answer:  Calculating and rendering tides and currents is expensive,
     and the data only change every 15 minutes or so.  So, keep a "mask blit-able"
     copy in persistent storage, and use as necessary.
     */
    if ( m_bShowTide || m_bShowCurrent )         // Showing T/C?
    {
        if ( 1/*b_newview*/|| m_bTCupdate )         // need to update the overlay
        {
            delete pss_overlay_bmp;
            pss_overlay_bmp = DrawTCCBitmap(&mscratch_dc);
        }

        //    blit the semi-static overlay onto the scratch DC if it is needed
        if ( NULL != pss_overlay_bmp )
        {
            wxMemoryDC ssdc_r;
            ssdc_r.SelectObject ( *pss_overlay_bmp );

            OCPNRegionIterator upd_final ( rgn_blit );
            while ( upd_final )
            {
                wxRect rect = upd_final.GetRect();
                mscratch_dc.Blit ( rect.x, rect.y, rect.width, rect.height,
                                   &ssdc_r, rect.x, rect.y, wxCOPY, true );      // Blit with mask
                upd_final ++;
            }

            ssdc_r.SelectObject ( wxNullBitmap );
        }
    }
#endif

    //quiting?
    if( g_bquiting ) {
#ifdef ocpnUSE_DIBSECTION
        ocpnMemDC q_dc;
#else
        wxMemoryDC q_dc;
#endif
        wxBitmap qbm( GetVP().pix_width, GetVP().pix_height );
        q_dc.SelectObject( qbm );

        // Get a copy of the screen
        q_dc.Blit( 0, 0, GetVP().pix_width, GetVP().pix_height, &mscratch_dc, 0, 0 );

        //  Draw a rectangle over the screen with a stipple brush
        wxBrush qbr( *wxBLACK, wxFDIAGONAL_HATCH );
        q_dc.SetBrush( qbr );
        q_dc.DrawRectangle( 0, 0, GetVP().pix_width, GetVP().pix_height );

        // Blit back into source
        mscratch_dc.Blit( 0, 0, GetVP().pix_width, GetVP().pix_height, &q_dc, 0, 0, wxCOPY );

        q_dc.SelectObject( wxNullBitmap );

    }

//    And finally, blit the scratch dc onto the physical dc
    wxRegionIterator upd_final( rgn_blit );
    while( upd_final ) {
        wxRect rect = upd_final.GetRect();
        dc.Blit( rect.x, rect.y, rect.width, rect.height, &mscratch_dc, rect.x, rect.y );
        upd_final++;
    }

    //  Test code to validate the dc drawing rectangle....
    /*
     OCPNRegionIterator upd_ru ( ru ); // get the update rect list
     while ( upd_ru )
     {
     wxRect rect = upd_ru.GetRect();

     dc.SetPen(wxPen(*wxRED));
     dc.SetBrush(wxBrush(*wxRED, wxTRANSPARENT));
     dc.DrawRectangle(rect);
     upd_ru ++ ;
     }
     */

//    Deselect the chart bitmap from the temp_dc, so that it will not be destroyed in the temp_dc dtor
    temp_dc.SelectObject( wxNullBitmap );
//    And for the scratch bitmap
    mscratch_dc.SelectObject( wxNullBitmap );

    dc.DestroyClippingRegion();

    PaintCleanup();

//      CALLGRIND_STOP_INSTRUMENTATION

}

void ChartCanvas::PaintCleanup()
{
//    Handle the current graphic window, if present

    if( pCwin ) {
        pCwin->Show();
        if( m_bTCupdate ) {
            pCwin->Refresh();
            pCwin->Update();
        }
    }

//    And set flags for next time
    m_bTCupdate = false;

//    Handle deferred WarpPointer
    if( warp_flag ) {
        WarpPointer( warp_x, warp_y );
        warp_flag = false;
    }

    // Start movement timer, this runs nearly immediately.
    // the reason we cannot simply call it directly is the
    // refresh events it emits may be blocked from this paint event
    pMovementTimer->Start( 1, wxTIMER_ONE_SHOT ); 
}

#if 0
wxColour GetErrorGraphicColor(double val)
{
    /*
     double valm = wxMin(val_max, val);

     unsigned char green = (unsigned char)(255 * (1 - (valm/val_max)));
     unsigned char red   = (unsigned char)(255 * (valm/val_max));

     wxImage::HSVValue hv = wxImage::RGBtoHSV(wxImage::RGBValue(red, green, 0));

     hv.saturation = 1.0;
     hv.value = 1.0;

     wxImage::RGBValue rv = wxImage::HSVtoRGB(hv);
     return wxColour(rv.red, rv.green, rv.blue);
     */

    //    HTML colors taken from NOAA WW3 Web representation
    wxColour c;
    if((val > 0) && (val < 1)) c.Set(_T("#002ad9"));
    else if((val >= 1) && (val < 2)) c.Set(_T("#006ed9"));
    else if((val >= 2) && (val < 3)) c.Set(_T("#00b2d9"));
    else if((val >= 3) && (val < 4)) c.Set(_T("#00d4d4"));
    else if((val >= 4) && (val < 5)) c.Set(_T("#00d9a6"));
    else if((val >= 5) && (val < 7)) c.Set(_T("#00d900"));
    else if((val >= 7) && (val < 9)) c.Set(_T("#95d900"));
    else if((val >= 9) && (val < 12)) c.Set(_T("#d9d900"));
    else if((val >= 12) && (val < 15)) c.Set(_T("#d9ae00"));
    else if((val >= 15) && (val < 18)) c.Set(_T("#d98300"));
    else if((val >= 18) && (val < 21)) c.Set(_T("#d95700"));
    else if((val >= 21) && (val < 24)) c.Set(_T("#d90000"));
    else if((val >= 24) && (val < 27)) c.Set(_T("#ae0000"));
    else if((val >= 27) && (val < 30)) c.Set(_T("#8c0000"));
    else if((val >= 30) && (val < 36)) c.Set(_T("#870000"));
    else if((val >= 36) && (val < 42)) c.Set(_T("#690000"));
    else if((val >= 42) && (val < 48)) c.Set(_T("#550000"));
    else if( val >= 48) c.Set(_T("#410000"));

    return c;
}

void ChartCanvas::RenderGeorefErrorMap( wxMemoryDC *pmdc, ViewPort *vp)
{
    wxImage gr_image(vp->pix_width, vp->pix_height);
    gr_image.InitAlpha();

    double maxval = -10000;
    double minval = 10000;

    double rlat, rlon;
    double glat, glon;

    GetCanvasPixPoint(0, 0, rlat, rlon);

    for(int i=1; i < vp->pix_height-1; i++)
    {
        for(int j=0; j < vp->pix_width; j++)
        {
            // Reference mercator value
//                  vp->GetMercatorLLFromPix(wxPoint(j, i), &rlat, &rlon);

            // Georef value
            GetCanvasPixPoint(j, i, glat, glon);

            maxval = wxMax(maxval, (glat - rlat));
            minval = wxMin(minval, (glat - rlat));

        }
        rlat = glat;
    }

    GetCanvasPixPoint(0, 0, rlat, rlon);
    for(int i=1; i < vp->pix_height-1; i++)
    {
        for(int j=0; j < vp->pix_width; j++)
        {
            // Reference mercator value
//                  vp->GetMercatorLLFromPix(wxPoint(j, i), &rlat, &rlon);

            // Georef value
            GetCanvasPixPoint(j, i, glat, glon);

            double f = ((glat - rlat)-minval)/(maxval - minval);

            double dy = (f * 40);

            wxColour c = GetErrorGraphicColor(dy);
            unsigned char r = c.Red();
            unsigned char g = c.Green();
            unsigned char b = c.Blue();

            gr_image.SetRGB(j, i, r,g,b);
            if((glat - rlat )!= 0)
                gr_image.SetAlpha(j, i, 128);
            else
                gr_image.SetAlpha(j, i, 255);

        }
        rlat = glat;
    }

    //    Create a Bitmap
    wxBitmap *pbm = new wxBitmap(gr_image);
    wxMask *gr_mask = new wxMask(*pbm, wxColour(0,0,0));
    pbm->SetMask(gr_mask);

    pmdc->DrawBitmap(*pbm, 0,0);

    delete pbm;

}

#endif

void ChartCanvas::CancelMouseRoute()
{
    parent_frame->nRoute_State = 0;
    m_pMouseRoute = NULL;
}

int ChartCanvas::GetNextContextMenuId()
{
    return ID_DEF_MENU_LAST + 100;  //Allowing for 100 dynamic menu item identifiers
}

bool ChartCanvas::SetCursor( const wxCursor &c )
{
#ifdef ocpnUSE_GL
    if( g_bopengl )
        return m_glcc->SetCursor( c );
    else
#endif
        return wxWindow::SetCursor( c );
}

void ChartCanvas::Refresh( bool eraseBackground, const wxRect *rect )
{
    //  Keep the mouse position members up to date
    GetCanvasPixPoint( mouse_x, mouse_y, m_cursor_lat, m_cursor_lon );

    //      Retrigger the route leg popup timer
    //      This handles the case when the chart is moving in auto-follow mode, but no user mouse input is made.
    //      The timer handler may Hide() the popup if the chart moved enough
    //      n.b.  We use slightly longer oneshot value to allow this method's Refresh() to complete before
    //      ptentially getting another Refresh() in the popup timer handler.
    if( (m_pRouteRolloverWin && m_pRouteRolloverWin->IsActive()) || (m_pAISRolloverWin && m_pAISRolloverWin->IsActive()) )
        m_RolloverPopupTimer.Start( 500, wxTIMER_ONE_SHOT );

#ifdef ocpnUSE_GL
    if( g_bopengl ) {
        
        //      We need to invalidate the FBO cache to ensure repaint of "grounded" overlay objects.
        if( eraseBackground && m_glcc->UsingFBO() )
            m_glcc->Invalidate();
        

        m_glcc->Refresh( eraseBackground, NULL ); // We always are going to render the entire screen anyway, so make
        // sure that the window managers understand the invalid area
        // is actually the entire client area.

        //  We need to selectively Refresh some child windows, if they are visible.
        //  Note that some children are refreshed elsewhere on timer ticks, so don't need attention here.

        //      Thumbnail chart
        if( pthumbwin && pthumbwin->IsShown() ) {
            pthumbwin->Raise();
            pthumbwin->Refresh( false );
        }

        //      ChartInfo window
        if( m_pCIWin && m_pCIWin->IsShown() ) {
            m_pCIWin->Raise();
            m_pCIWin->Refresh( false );
        }


    } else
#endif
        wxWindow::Refresh( eraseBackground, rect );

}

void ChartCanvas::Update()
{
    if( g_bopengl ) {
#ifdef ocpnUSE_GL
        m_glcc->Update();
#endif
    } else
        wxWindow::Update();
}

void ChartCanvas::DrawEmboss( ocpnDC &dc, emboss_data *pemboss)
{
    if( !pemboss ) return;
    int x = pemboss->x, y = pemboss->y;
    const double factor = 200;

    wxASSERT_MSG( dc.GetDC(), wxT ( "DrawEmboss has no dc (opengl?)" ) );
    wxMemoryDC *pmdc = dynamic_cast<wxMemoryDC*>( dc.GetDC() );
    wxASSERT_MSG ( pmdc, wxT ( "dc to EmbossCanvas not a memory dc" ) );
    
    //Grab a snipped image out of the chart
    wxMemoryDC snip_dc;
    wxBitmap snip_bmp( pemboss->width, pemboss->height, -1 );
    snip_dc.SelectObject( snip_bmp );
    
    snip_dc.Blit( 0, 0, pemboss->width, pemboss->height, pmdc, x, y );
    snip_dc.SelectObject( wxNullBitmap );
    
    wxImage snip_img = snip_bmp.ConvertToImage();
    
    //  Apply Emboss map to the snip image
    unsigned char* pdata = snip_img.GetData();
    if( pdata ) {
        for( int y = 0; y < pemboss->height; y++ ) {
            int map_index = ( y * pemboss->width );
            for( int x = 0; x < pemboss->width; x++ ) {
                double val = ( pemboss->pmap[map_index] * factor ) / 256.;
                
                int nred = (int) ( ( *pdata ) + val );
                nred = nred > 255 ? 255 : ( nred < 0 ? 0 : nred );
                *pdata++ = (unsigned char) nred;
                
                int ngreen = (int) ( ( *pdata ) + val );
                ngreen = ngreen > 255 ? 255 : ( ngreen < 0 ? 0 : ngreen );
                *pdata++ = (unsigned char) ngreen;
                
                int nblue = (int) ( ( *pdata ) + val );
                nblue = nblue > 255 ? 255 : ( nblue < 0 ? 0 : nblue );
                *pdata++ = (unsigned char) nblue;
                
                map_index++;
            }
        }
    }
    
    //  Convert embossed snip to a bitmap
    wxBitmap emb_bmp( snip_img );

    //  Map to another memoryDC
    wxMemoryDC result_dc;
    result_dc.SelectObject( emb_bmp );
    
    //  Blit to target
    pmdc->Blit( x, y, pemboss->width, pemboss->height, &result_dc, 0, 0 );
    
    result_dc.SelectObject( wxNullBitmap );
}

emboss_data *ChartCanvas::EmbossOverzoomIndicator( ocpnDC &dc )
{
    if( GetQuiltMode() ) {
        double chart_native_ppm;
        chart_native_ppm = m_canvas_scale_factor / m_pQuilt->GetRefNativeScale();

        double zoom_factor = GetVP().view_scale_ppm / chart_native_ppm;

        if( zoom_factor <= 3.9 ) return NULL;
    } else {
        double chart_native_ppm;
        if( Current_Ch ) chart_native_ppm = m_canvas_scale_factor / Current_Ch->GetNativeScale();
        else
            chart_native_ppm = m_true_scale_ppm;

        double zoom_factor = GetVP().view_scale_ppm / chart_native_ppm;
        if( Current_Ch ) {
            if( zoom_factor <= 3.9 ) return NULL;
        }
    }

    if(m_pEM_OverZoom){
        m_pEM_OverZoom->x = 0;
        m_pEM_OverZoom->y = 40;
    }
    return m_pEM_OverZoom;
}

void ChartCanvas::DrawOverlayObjects( ocpnDC &dc, const wxRegion& ru )
{
    GridDraw( dc );

    if( g_pi_manager ) {
        g_pi_manager->SendViewPortToRequestingPlugIns( GetVP() );
        g_pi_manager->RenderAllCanvasOverlayPlugIns( dc, GetVP() );
    }

    AISDrawAreaNotices( dc );
    DrawEmboss( dc, EmbossDepthScale( ) );
    DrawEmboss( dc, EmbossOverzoomIndicator( dc ) );

    DrawAllRoutesInBBox( dc, GetVP().GetBBox(), ru );
    DrawAllWaypointsInBBox( dc, GetVP().GetBBox(), ru, true ); // true draws only isolated marks
    DrawAnchorWatchPoints( dc );

    AISDraw( dc );
    ShipDraw( dc );
    AlertDraw( dc );

    RenderAllChartOutlines( dc, GetVP() );
    RenderRouteLegs( dc );
    ScaleBarDraw( dc );
#ifdef USE_S57
    s57_DrawExtendedLightSectors( dc, VPoint, extendedSectorLegs );
#endif

    if( m_pRouteRolloverWin && m_pRouteRolloverWin->IsActive() ) {
        dc.DrawBitmap( *(m_pRouteRolloverWin->GetBitmap()),
                       m_pRouteRolloverWin->GetPosition().x,
                       m_pRouteRolloverWin->GetPosition().y, false );
    }
    if( m_pAISRolloverWin && m_pAISRolloverWin->IsActive() ) {
        dc.DrawBitmap( *(m_pAISRolloverWin->GetBitmap()),
                m_pAISRolloverWin->GetPosition().x,
                m_pAISRolloverWin->GetPosition().y, false );
    }
}

emboss_data *ChartCanvas::EmbossDepthScale()
{
    if( !g_bShowDepthUnits ) return NULL;

    int depth_unit_type = DEPTH_UNIT_UNKNOWN;

    if( GetQuiltMode() ) {
        wxString s = m_pQuilt->GetQuiltDepthUnit();
        s.MakeUpper();
        if( s == _T("FEET") ) depth_unit_type = DEPTH_UNIT_FEET;
        else if( s.StartsWith( _T("FATHOMS") ) ) depth_unit_type = DEPTH_UNIT_FATHOMS;
        else if( s.StartsWith( _T("METERS") ) ) depth_unit_type = DEPTH_UNIT_METERS;
        else if( s.StartsWith( _T("METRES") ) ) depth_unit_type = DEPTH_UNIT_METERS;
        else if( s.StartsWith( _T("METRIC") ) ) depth_unit_type = DEPTH_UNIT_METERS;
        else if( s.StartsWith( _T("METER") ) ) depth_unit_type = DEPTH_UNIT_METERS;

    } else {
        if( Current_Ch ) {
            depth_unit_type = Current_Ch->GetDepthUnitType();
#ifdef USE_S57
            if( Current_Ch->GetChartFamily() == CHART_FAMILY_VECTOR ) depth_unit_type =
                    ps52plib->m_nDepthUnitDisplay + 1;
#endif
        }
    }

    emboss_data *ped = NULL;
    switch( depth_unit_type ) {
    case DEPTH_UNIT_FEET:
        ped = m_pEM_Feet;
        break;
    case DEPTH_UNIT_METERS:
        ped = m_pEM_Meters;
        break;
    case DEPTH_UNIT_FATHOMS:
        ped = m_pEM_Fathoms;
        break;
    default:
        return NULL;
    }

    ped->x = ( GetVP().pix_width - ped->width );
    ped->y = 40;
    return ped;
}

void ChartCanvas::CreateDepthUnitEmbossMaps( ColorScheme cs )
{
    ocpnStyle::Style* style = g_StyleManager->GetCurrentStyle();
    wxFont font;
    if( style->embossFont == wxEmptyString )
        font = wxFont( 60, wxFONTFAMILY_ROMAN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD );
    else
        font = wxFont( style->embossHeight, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, style->embossFont );

    int emboss_width = 500;
    int emboss_height = 100;

// Free any existing emboss maps
    delete m_pEM_Feet;
    delete m_pEM_Meters;
    delete m_pEM_Fathoms;

// Create the 3 DepthUnit emboss map structures
    m_pEM_Feet = CreateEmbossMapData( font, emboss_width, emboss_height, _("Feet"), cs );
    m_pEM_Meters = CreateEmbossMapData( font, emboss_width, emboss_height, _("Meters"), cs );
    m_pEM_Fathoms = CreateEmbossMapData( font, emboss_width, emboss_height, _("Fathoms"), cs );
}

#define OVERZOOM_TEXT _("OverZoom")

void ChartCanvas::SetOverzoomFont()
{
    ocpnStyle::Style* style = g_StyleManager->GetCurrentStyle();
    int w, h;

    wxFont font;
    if( style->embossFont == wxEmptyString )
        font = wxFont( 40, wxFONTFAMILY_ROMAN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD );
    else
        font = wxFont( style->embossHeight, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, style->embossFont );

    wxClientDC dc( this );
    dc.SetFont( font );
    dc.GetTextExtent( OVERZOOM_TEXT, &w, &h );
    
    while( font.GetPointSize() > 10 && (w > 500 || h > 100) )
    {
        font.SetPointSize( font.GetPointSize() - 1 );
        dc.SetFont( font );
        dc.GetTextExtent( OVERZOOM_TEXT, &w, &h );
    }
    m_overzoomFont = font;
    m_overzoomTextWidth = w;
    m_overzoomTextHeight = h;
}

void ChartCanvas::CreateOZEmbossMapData( ColorScheme cs )
{
    delete m_pEM_OverZoom;

    if( m_overzoomTextWidth > 0 && m_overzoomTextHeight > 0 )
        m_pEM_OverZoom = CreateEmbossMapData( m_overzoomFont, m_overzoomTextWidth + 10, m_overzoomTextHeight + 10, OVERZOOM_TEXT, cs );
}

emboss_data *ChartCanvas::CreateEmbossMapData( wxFont &font, int width, int height,
        const wxChar *str, ColorScheme cs )
{
    int *pmap;

    //  Create a temporary bitmap
    wxBitmap bmp( width, height, -1 );

    // Create a memory DC
    wxMemoryDC temp_dc;
    temp_dc.SelectObject( bmp );

    //  Paint on it
    temp_dc.SetBackground( *wxWHITE_BRUSH );
    temp_dc.SetTextBackground( *wxWHITE );
    temp_dc.SetTextForeground( *wxBLACK );

    temp_dc.Clear();

    temp_dc.SetFont( font );

    int str_w, str_h;
    temp_dc.GetTextExtent( wxString( str, wxConvUTF8 ), &str_w, &str_h );
    temp_dc.DrawText( wxString( str, wxConvUTF8 ), width - str_w - 10, 10 );

    //  Deselect the bitmap
    temp_dc.SelectObject( wxNullBitmap );

    //  Convert bitmap the wxImage for manipulation
    wxImage img = bmp.ConvertToImage();

    double val_factor;
    switch( cs ) {
    case GLOBAL_COLOR_SCHEME_DAY:
    default:
        val_factor = 1;
        break;
    case GLOBAL_COLOR_SCHEME_DUSK:
        val_factor = .5;
        break;
    case GLOBAL_COLOR_SCHEME_NIGHT:
        val_factor = .25;
        break;
    }

    int val;
    int index;
    pmap = (int *) calloc( width * height * sizeof(int), 1 );
    //  Create emboss map by differentiating the emboss image
    //  and storing integer results in pmap
    //  n.b. since the image is B/W, it is sufficient to check
    //  one channel (i.e. red) only
    for( int y = 1; y < height - 1; y++ ) {
        for( int x = 1; x < width - 1; x++ ) {
            val = img.GetRed( x + 1, y + 1 ) - img.GetRed( x - 1, y - 1 );  // range +/- 256
            val = (int) ( val * val_factor );
            index = ( y * width ) + x;
            pmap[index] = val;

        }
    }

    emboss_data *pret = new emboss_data;
    pret->pmap = pmap;
    pret->width = width;
    pret->height = height;

    return pret;
}

//----------------------------------------------------------------------------
//  Get a wxBitmap with wxMask associated containing the semi-static overlays
//----------------------------------------------------------------------------

wxBitmap *ChartCanvas::DrawTCCBitmap( wxDC *pbackground_dc, bool bAddNewSelpoints )
{
    wxBitmap *p_bmp = new wxBitmap( GetVP().pix_width, GetVP().pix_height, -1 );

    //      Here is the new drawing DC
    wxMemoryDC ssdc;
    ssdc.SelectObject( *p_bmp );
    ssdc.SetBackground( *wxWHITE_BRUSH );

    //  if a background dc is provided, use it as wallpaper
    if( pbackground_dc ) ssdc.Blit( 0, 0, GetVP().pix_width, GetVP().pix_height, pbackground_dc, 0,
                                        0 );
    else
        ssdc.Clear();

    //      Believe it or not, it is faster to REDRAW the overlay objects
    //      onto a mono bitmap, and then convert it into a mask bitmap
    //      than it is to create a mask from a colour bmp.
    //      Look at the wx code.  It goes through wxImage conversion, etc...
    //      So, create a mono DC, drawing white-on-black
    wxMemoryDC ssdc_mask;
    wxBitmap mask_bmp( GetVP().pix_width, GetVP().pix_height, 1 );
    ssdc_mask.SelectObject( mask_bmp );

    //      On X11, the drawing is Black on White, and the mask bitmap is inverted before
    //      making into a mask.
    //      On MSW and GTK, the drawing is White on Black, and no inversion is required
    //      Todo....  Some wxWidgets problem with this....
#ifndef __WXX11__
    ssdc_mask.SetBackground( *wxBLACK_BRUSH );
#endif

    ssdc_mask.Clear();

//    Maybe draw the Tide Points
    ocpnDC ossdc( ssdc ), ossdc_mask( ssdc_mask );

    if( m_bShowTide ) {
        // Rebuild Selpoints list on new map
        DrawAllTidesInBBox( ossdc, GetVP().GetBBox(), bAddNewSelpoints || !bShowingTide, true );
        DrawAllTidesInBBox( ossdc_mask, GetVP().GetBBox(), false, true, true );    // onto the mask
        bShowingTide = true;
    } else
        bShowingTide = false;

//    Maybe draw the current arrows
    if( m_bShowCurrent ) {
        // Rebuild Selpoints list on new map
        // and force redraw
        DrawAllCurrentsInBBox( ossdc, GetVP().GetBBox(), bAddNewSelpoints || !bShowingCurrent,
                               true );
        DrawAllCurrentsInBBox( ossdc_mask, GetVP().GetBBox(), false, true, true );  // onto the mask
        bShowingCurrent = true;
    } else
        bShowingCurrent = false;

    ssdc.SelectObject( wxNullBitmap );

#ifdef __WXX11__
    //      Invert the mono bmp, to make a useable mask bmp
    wxMemoryDC ssdc_mask_invert;
    wxBitmap mask_bmp_invert ( GetVP().pix_width, GetVP().pix_height, 1 );
    ssdc_mask_invert.SelectObject ( mask_bmp_invert );
    ssdc_mask_invert.Blit ( 0, 0, GetVP().pix_width, GetVP().pix_height,
                            &ssdc_mask, 0, 0, wxSRC_INVERT );

    ssdc_mask_invert.SelectObject ( wxNullBitmap );
    pss_overlay_mask = new wxMask ( mask_bmp_invert );
    ssdc_mask.SelectObject ( wxNullBitmap );
#else
    ssdc_mask.SelectObject( wxNullBitmap );
    pss_overlay_mask = new wxMask( mask_bmp );
#endif

    //      Create and associate the mask
    p_bmp->SetMask( pss_overlay_mask );

    return p_bmp;
}

extern bool g_bTrackActive;

void ChartCanvas::DrawAllRoutesInBBox( ocpnDC& dc, LLBBox& BltBBox, const wxRegion& clipregion )
{
    Route *active_route = NULL;
    Route *active_track = NULL;

    wxDC *pdc = dc.GetDC();
    if( pdc ) {
        pdc->DestroyClippingRegion();
        wxDCClipper( *pdc, clipregion );
    }

    wxRouteListNode *node = pRouteList->GetFirst();
    while( node ) {
        bool b_run = false;
        bool b_drawn = false;
        Route *pRouteDraw = node->GetData();
        if( pRouteDraw ) {
            if( pRouteDraw->IsTrack() ) {
                Track *trk = (Track *) pRouteDraw;
                if( trk->IsRunning() ) {
                    b_run = true;
                    active_track = pRouteDraw;
                }

                if( pRouteDraw->IsActive() || pRouteDraw->IsSelected() ) active_route = pRouteDraw;

            }

            wxBoundingBox test_box = pRouteDraw->GetBBox();

            if( b_run ) test_box.Expand( gLon, gLat );

            if( !BltBBox.IntersectOut( test_box ) ) // Route is not wholly outside window
            {
                b_drawn = true;

                if( ( pRouteDraw != active_route ) && ( pRouteDraw != active_track ) )
                    pRouteDraw->Draw( dc, GetVP() );
            } else if( pRouteDraw->CrossesIDL() ) {
                wxPoint2DDouble xlate( -360., 0. );
                wxBoundingBox test_box1 = pRouteDraw->GetBBox();
                test_box1.Translate( xlate );
                if( b_run ) test_box1.Expand( gLon, gLat );

                if( !BltBBox.IntersectOut( test_box1 ) ) // Route is not wholly outside window
                {
                    b_drawn = true;
                    if( ( pRouteDraw != active_route ) && ( pRouteDraw != active_track ) ) pRouteDraw->Draw(
                            dc, GetVP() );
                }
            }

            //      Need to quick check for the case where VP crosses IDL
            if( !b_drawn ) {
                if( ( BltBBox.GetMinX() < -180. ) && ( BltBBox.GetMaxX() > -180. ) ) {
                    wxPoint2DDouble xlate( -360., 0. );
                    wxBoundingBox test_box2 = pRouteDraw->GetBBox();
                    test_box2.Translate( xlate );
                    if( !BltBBox.IntersectOut( test_box2 ) ) // Route is not wholly outside window
                    {
                        b_drawn = true;
                        if( ( pRouteDraw != active_route ) && ( pRouteDraw != active_track ) ) pRouteDraw->Draw(
                                dc, GetVP() );
                    }
                } else if( !b_drawn && ( BltBBox.GetMinX() < 180. ) && ( BltBBox.GetMaxX() > 180. ) ) {
                    wxPoint2DDouble xlate( 360., 0. );
                    wxBoundingBox test_box3 = pRouteDraw->GetBBox();
                    test_box3.Translate( xlate );
                    if( !BltBBox.IntersectOut( test_box3 ) ) // Route is not wholly outside window
                    {
                        b_drawn = true;
                        if( ( pRouteDraw != active_route ) && ( pRouteDraw != active_track ) ) pRouteDraw->Draw(
                                dc, GetVP() );
                    }
                }
            }
        }

        node = node->GetNext();
    }

    //  Draw any active or selected route (or track) last, so that is is always on top
    if( active_route ) active_route->Draw( dc, GetVP() );
    if( active_track ) active_track->Draw( dc, GetVP() );
}

void ChartCanvas::DrawAllWaypointsInBBox( ocpnDC& dc, LLBBox& BltBBox, const wxRegion& clipregion,
        bool bDrawMarksOnly )
{
//        wxBoundingBox bbx;
    wxDC *pdc = dc.GetDC();
    if( pdc ) {
        wxDCClipper( *pdc, clipregion );
    }

    wxRoutePointListNode *node = pWayPointMan->GetWaypointList()->GetFirst();

    while( node ) {
        RoutePoint *pWP = node->GetData();
        if( pWP ) {
            if( ( bDrawMarksOnly ) && ( pWP->m_bIsInRoute || pWP->m_bIsInTrack ) ) {
                node = node->GetNext();
                continue;
            } else {
                if( BltBBox.GetValid() ) {
                    if( BltBBox.PointInBox( pWP->m_lon, pWP->m_lat, 0 ) ) pWP->Draw( dc, NULL );
                }
            }
        }

        node = node->GetNext();
    }
}

void ChartCanvas::DrawBlinkObjects( void )
{
    //  All RoutePoints
    wxRect update_rect;
    
    wxRoutePointListNode *node = pWayPointMan->GetWaypointList()->GetFirst();
    
    while( node ) {
        RoutePoint *pWP = node->GetData();
        if( pWP ) {
            if( pWP->m_bBlink ) {
                update_rect.Union( pWP->CurrentRect_in_DC ) ;
            }
        }
        
        node = node->GetNext();
    }
    if( !update_rect.IsEmpty() )
        RefreshRect(update_rect);
}




void ChartCanvas::DrawAnchorWatchPoints( ocpnDC& dc )
{
    // draw anchor watch rings, if activated

    if( pAnchorWatchPoint1 || pAnchorWatchPoint2 ) {
        wxPoint r1, r2;
        wxPoint lAnchorPoint1, lAnchorPoint2;
        double lpp1 = 0.0;
        double lpp2 = 0.0;
        if( pAnchorWatchPoint1 ) {
            lpp1 = GetAnchorWatchRadiusPixels( pAnchorWatchPoint1 );
            GetCanvasPointPix( pAnchorWatchPoint1->m_lat, pAnchorWatchPoint1->m_lon,
                               &lAnchorPoint1 );

        }
        if( pAnchorWatchPoint2 ) {
            lpp2 = GetAnchorWatchRadiusPixels( pAnchorWatchPoint2 );
            GetCanvasPointPix( pAnchorWatchPoint2->m_lat, pAnchorWatchPoint2->m_lon,
                               &lAnchorPoint2 );

        }

        wxPen ppPeng( GetGlobalColor( _T ( "UGREN" ) ), 2 );
        wxPen ppPenr( GetGlobalColor( _T ( "URED" ) ), 2 );

        wxBrush *ppBrush = wxTheBrushList->FindOrCreateBrush( wxColour( 0, 0, 0 ), wxTRANSPARENT );
        dc.SetBrush( *ppBrush );

        if( lpp1 > 0 ) {
            dc.SetPen( ppPeng );
            dc.StrokeCircle( lAnchorPoint1.x, lAnchorPoint1.y, fabs( lpp1 ) );
        }

        if( lpp2 > 0 ) {
            dc.SetPen( ppPeng );
            dc.StrokeCircle( lAnchorPoint2.x, lAnchorPoint2.y, fabs( lpp2 ) );
        }

        if( lpp1 < 0 ) {
            dc.SetPen( ppPenr );
            dc.StrokeCircle( lAnchorPoint1.x, lAnchorPoint1.y, fabs( lpp1 ) );
        }

        if( lpp2 < 0 ) {
            dc.SetPen( ppPenr );
            dc.StrokeCircle( lAnchorPoint2.x, lAnchorPoint2.y, fabs( lpp2 ) );
        }
    }
}

double ChartCanvas::GetAnchorWatchRadiusPixels( RoutePoint *pAnchorWatchPoint )
{
    double lpp = 0.;
    wxPoint r1;
    wxPoint lAnchorPoint;
    double d1 = 0.0;
    double dabs;
    double tlat1, tlon1;

    if( pAnchorWatchPoint ) {
        ( pAnchorWatchPoint->GetName() ).ToDouble( &d1 );
        d1 = AnchorDistFix( d1, AnchorPointMinDist, g_nAWMax );
        dabs = fabs( d1 / 1852. );
        ll_gc_ll( pAnchorWatchPoint->m_lat, pAnchorWatchPoint->m_lon, 0, dabs, &tlat1, &tlon1 );
        GetCanvasPointPix( tlat1, tlon1, &r1 );
        GetCanvasPointPix( pAnchorWatchPoint->m_lat, pAnchorWatchPoint->m_lon, &lAnchorPoint );
        lpp = sqrt( pow( (double) (lAnchorPoint.x - r1.x), 2) +
                    pow( (double) (lAnchorPoint.y - r1.y), 2) );

        //    This is an entry watch
        if( d1 < 0 ) lpp = -lpp;
    }
    return lpp;
}

//------------------------------------------------------------------------------------------
//    Tides Support
//------------------------------------------------------------------------------------------

void ChartCanvas::DrawAllTidesInBBox( ocpnDC& dc, LLBBox& BBox, bool bRebuildSelList,
                                      bool bforce_redraw_tides, bool bdraw_mono_for_mask )
{
    wxPen *pblack_pen = wxThePenList->FindOrCreatePen( GetGlobalColor( _T ( "UINFD" ) ), 1,
                        wxSOLID );
    wxPen *pyelo_pen = wxThePenList->FindOrCreatePen( GetGlobalColor( _T ( "YELO1" ) ), 1,
                       wxSOLID );
    wxPen *pblue_pen = wxThePenList->FindOrCreatePen( GetGlobalColor( _T ( "BLUE2" ) ), 1,
                       wxSOLID );

    wxBrush *pgreen_brush = wxTheBrushList->FindOrCreateBrush( GetGlobalColor( _T ( "GREEN1" ) ),
                            wxSOLID );
//        wxBrush *pblack_brush = wxTheBrushList->FindOrCreateBrush ( GetGlobalColor ( _T ( "UINFD" ) ), wxSOLID );
    wxBrush *brc_1 = wxTheBrushList->FindOrCreateBrush( GetGlobalColor( _T ( "BLUE2" ) ), wxSOLID );
    wxBrush *brc_2 = wxTheBrushList->FindOrCreateBrush( GetGlobalColor( _T ( "YELO1" ) ), wxSOLID );

    wxFont *dFont = FontMgr::Get().GetFont( _("ExtendedTideIcon"), 12 );
    dc.SetTextForeground( FontMgr::Get().GetFontColor( _("ExtendedTideIcon") ) );
    int font_size = wxMax(8, dFont->GetPointSize());
    wxFont *plabelFont = wxTheFontList->FindOrCreateFont( font_size, dFont->GetFamily(),
                         dFont->GetStyle(), dFont->GetWeight() );

    if( bdraw_mono_for_mask ) {
#ifdef __WXX11__
        const wxPen *pmono_pen = wxBLACK_PEN;
        const wxBrush *pmono_brush = wxBLACK_BRUSH;
#else
        const wxPen *pmono_pen = wxWHITE_PEN;
        const wxBrush *pmono_brush = wxWHITE_BRUSH;
#endif

        pblack_pen = (wxPen *) pmono_pen;
        pgreen_brush = (wxBrush *) pmono_brush;
        brc_1 = (wxBrush *) pmono_brush;
        brc_2 = (wxBrush *) pmono_brush;

    }

    dc.SetPen( *pblack_pen );
    dc.SetBrush( *pgreen_brush );

    if( bRebuildSelList ) pSelectTC->DeleteAllSelectableTypePoints( SELTYPE_TIDEPOINT );

    wxBitmap bm;
    switch( m_cs ) {
    case GLOBAL_COLOR_SCHEME_DAY:
        bm = m_bmTideDay;
        break;
    case GLOBAL_COLOR_SCHEME_DUSK:
        bm = m_bmTideDusk;
        break;
    case GLOBAL_COLOR_SCHEME_NIGHT:
        bm = m_bmTideNight;
        break;
    default:
        bm = m_bmTideDay;
        break;
    }

    int bmw = bm.GetWidth();
    int bmh = bm.GetHeight();

    wxDateTime this_now = wxDateTime::Now();
    time_t t_this_now = this_now.GetTicks();

//      if(1/*BBox.GetValid()*/)
    {

        double lon_last = 0.;
        double lat_last = 0.;
        for( int i = 1; i < ptcmgr->Get_max_IDX() + 1; i++ ) {
            const IDX_entry *pIDX = ptcmgr->GetIDX_entry( i );

            char type = pIDX->IDX_type;             // Entry "TCtcIUu" identifier
            if( ( type == 't' ) || ( type == 'T' ) )  // only Tides
            {
                double lon = pIDX->IDX_lon;
                double lat = pIDX->IDX_lat;
                bool b_inbox = false;
                double nlon;

                if( BBox.PointInBox( lon, lat, 0 ) ) {
                    nlon = lon;
                    b_inbox = true;
                } else if( BBox.PointInBox( lon + 360., lat, 0 ) ) {
                    nlon = lon + 360.;
                    b_inbox = true;
                } else if( BBox.PointInBox( lon - 360., lat, 0 ) ) {
                    nlon = lon - 360.;
                    b_inbox = true;
                }

//try to eliminate double entry , but the only good way is to clean the file!
                if( b_inbox && ( lat != lat_last ) && ( lon != lon_last ) ) {

//    Manage the point selection list
                    if( bRebuildSelList ) pSelectTC->AddSelectablePoint( lat, lon, pIDX,
                                SELTYPE_TIDEPOINT );

                    wxPoint r;
                    GetCanvasPointPix( lat, nlon, &r );
//draw standard icons
                    if( GetVP().chart_scale > 500000 ) {

                        if( bdraw_mono_for_mask ) dc.DrawRectangle( r.x - bmw / 2, r.y - bmh / 2,
                                    bmw, bmh );
                        else
                            dc.DrawBitmap( bm, r.x - bmw / 2, r.y - bmh / 2, true );
                    }
//draw "extended" icons
                    else {
//set rectangle size and position (max text lengh)
                        int wx, hx;
                        dc.SetFont( *plabelFont );
                        dc.GetTextExtent( _T("99.9ft "), &wx, &hx );
                        int w = r.x - 6;
                        int h = r.y - 22;
//draw mask
                        if( bdraw_mono_for_mask ) dc.DrawRectangle( r.x - ( wx / 2 ), h, wx,
                                    hx + 45 );
                        //process tides
                        else {
                            if( bforce_redraw_tides ) {
                                float val, nowlev;
                                float ltleve = 0.;
                                float htleve = 0.;
                                time_t tctime;
                                time_t lttime = 0;
                                time_t httime = 0;
                                bool wt;
                                //define if flood or edd in the last ten minutes and verify if data are useable
                                if( ptcmgr->GetTideFlowSens( t_this_now, BACKWARD_TEN_MINUTES_STEP,
                                                             pIDX->IDX_rec_num, nowlev, val, wt ) ) {

                                    //search forward the first HW or LW near "now" ( starting at "now" - ten minutes )
                                    ptcmgr->GetHightOrLowTide(
                                        t_this_now + BACKWARD_TEN_MINUTES_STEP,
                                        FORWARD_TEN_MINUTES_STEP, FORWARD_ONE_MINUTES_STEP, val,
                                        wt, pIDX->IDX_rec_num, val, tctime );
                                    if( wt ) {
                                        httime = tctime;
                                        htleve = val;
                                    } else {
                                        lttime = tctime;
                                        ltleve = val;
                                    }
                                    wt = !wt;

                                    //then search opposite tide near "now"
                                    if( tctime > t_this_now )          // search backward
                                        ptcmgr->GetHightOrLowTide( t_this_now,
                                                                   BACKWARD_TEN_MINUTES_STEP, BACKWARD_ONE_MINUTES_STEP,
                                                                   nowlev, wt, pIDX->IDX_rec_num, val, tctime );
                                    else
                                        // or search forward
                                        ptcmgr->GetHightOrLowTide( t_this_now,
                                                                   FORWARD_TEN_MINUTES_STEP, FORWARD_ONE_MINUTES_STEP,
                                                                   nowlev, wt, pIDX->IDX_rec_num, val, tctime );
                                    if( wt ) {
                                        httime = tctime;
                                        htleve = val;
                                    } else {
                                        lttime = tctime;
                                        ltleve = val;
                                    }

                                    //process tide state  ( %height and flow sens )
                                    float ts = 1 - ( ( nowlev - ltleve ) / ( htleve - ltleve ) );
                                    int hs = ( httime > lttime ) ? -5 : 5;
                                    if( ts > 0.995 || ts < 0.005 ) hs = 0;
                                    int ht_y = (int) ( 45.0 * ts );

                                    //draw yellow rectangle as total amplitude (width = 12 , height = 45 )
                                    dc.SetPen( *pblack_pen );
                                    dc.SetBrush( *brc_2 );
                                    dc.DrawRectangle( w, h, 12, 45 );
                                    //draw blue rectangle as water height
                                    dc.SetPen( *pblue_pen );
                                    dc.SetBrush( *brc_1 );
                                    dc.DrawRectangle( w + 2, h + ht_y, 8, 45 - ht_y );

                                    //draw sens arrows (ensure they are not "under-drawn" by top line of blue rectangle )

                                    int hl;
                                    wxPoint arrow[3];
                                    arrow[0].x = w + 1;
                                    arrow[1].x = w + 5;
                                    arrow[2].x = w + 11;
                                    if( ts > 0.35 || ts < 0.15 )      // one arrow at 3/4 hight tide
                                    {
                                        hl = (int) ( 45.0 * 0.25 ) + h;
                                        arrow[0].y = hl;
                                        arrow[1].y = hl + hs;
                                        arrow[2].y = hl;
                                        if( ts < 0.15 ) dc.SetPen( *pyelo_pen );
                                        else
                                            dc.SetPen( *pblue_pen );

                                        dc.DrawLines( 3, arrow );
                                    }
                                    if( ts > 0.60 || ts < 0.40 )       //one arrow at 1/2 hight tide
                                    {
                                        hl = (int) ( 45.0 * 0.5 ) + h;
                                        arrow[0].y = hl;
                                        arrow[1].y = hl + hs;
                                        arrow[2].y = hl;
                                        if( ts < 0.40 ) dc.SetPen( *pyelo_pen );
                                        else
                                            dc.SetPen( *pblue_pen );
                                        dc.DrawLines( 3, arrow );
                                    }
                                    if( ts < 0.65 || ts > 0.85 )       //one arrow at 1/4 Hight tide
                                    {
                                        hl = (int) ( 45.0 * 0.75 ) + h;
                                        arrow[0].y = hl;
                                        arrow[1].y = hl + hs;
                                        arrow[2].y = hl;
                                        if( ts < 0.65 ) dc.SetPen( *pyelo_pen );
                                        else
                                            dc.SetPen( *pblue_pen );
                                        dc.DrawLines( 3, arrow );
                                    }
                                    //draw tide level text
                                    wxString s;
                                    s.Printf( _T("%3.1f"), nowlev );
                                    Station_Data *pmsd = pIDX->pref_sta_data;           //write unit
                                    if( pmsd ) s.Append(
                                            wxString( pmsd->units_abbrv, wxConvUTF8 ) );
                                    int wx1;
                                    dc.GetTextExtent( s, &wx1, NULL );
                                    dc.DrawText( s, r.x - ( wx1 / 2 ), h + 45 );
                                }
                            }
                        }
                    }
                }
                lon_last = lon;
                lat_last = lat;
            }
        }
    }
}

//------------------------------------------------------------------------------------------
//    Currents Support
//------------------------------------------------------------------------------------------

void ChartCanvas::DrawAllCurrentsInBBox( ocpnDC& dc, LLBBox& BBox, bool bRebuildSelList,
        bool bforce_redraw_currents, bool bdraw_mono_for_mask )
{
    float tcvalue, dir;
    bool bnew_val;
    char sbuf[20];
    wxFont *pTCFont;
    double lon_last = 0.;
    double lat_last = 0.;

    double true_scale_display = floor( VPoint.chart_scale / 100. ) * 100.;
    bDrawCurrentValues =  true_scale_display < g_Show_Target_Name_Scale;

    wxPen *pblack_pen = wxThePenList->FindOrCreatePen( GetGlobalColor( _T ( "UINFD" ) ), 1,
                        wxSOLID );
    wxPen *porange_pen = wxThePenList->FindOrCreatePen( GetGlobalColor( _T ( "UINFO" ) ), 1,
                         wxSOLID );
    wxBrush *porange_brush = wxTheBrushList->FindOrCreateBrush( GetGlobalColor( _T ( "UINFO" ) ),
                             wxSOLID );
    wxBrush *pgray_brush = wxTheBrushList->FindOrCreateBrush( GetGlobalColor( _T ( "UIBDR" ) ),
                           wxSOLID );
    wxBrush *pblack_brush = wxTheBrushList->FindOrCreateBrush( GetGlobalColor( _T ( "UINFD" ) ),
                            wxSOLID );

    double skew_angle = GetVPRotation();

    if( !g_bskew_comp )
        skew_angle += GetVPSkew();

    if( bdraw_mono_for_mask ) {
#ifdef __WXX11__
        const wxPen *pmono_pen = wxBLACK_PEN;
        const wxBrush *pmono_brush = wxBLACK_BRUSH;
#else
        const wxPen *pmono_pen = wxWHITE_PEN;
        const wxBrush *pmono_brush = wxWHITE_BRUSH;
#endif

        pblack_pen = (wxPen *) pmono_pen;
        porange_pen = (wxPen *) pmono_pen;
        porange_brush = (wxBrush *) pmono_brush;
        pgray_brush = (wxBrush *) pmono_brush;
    }

    pTCFont = FontMgr::Get().GetFont( _("CurrentValue"), 12 );
    
    int now = time( NULL );

    if( bRebuildSelList ) pSelectTC->DeleteAllSelectableTypePoints( SELTYPE_CURRENTPOINT );

//     if(1/*BBox.GetValid()*/)
    {

        for( int i = 1; i < ptcmgr->Get_max_IDX() + 1; i++ ) {
            const IDX_entry *pIDX = ptcmgr->GetIDX_entry( i );
            double lon = pIDX->IDX_lon;
            double lat = pIDX->IDX_lat;

            char type = pIDX->IDX_type;             // Entry "TCtcIUu" identifier
            if( ( ( type == 'c' ) || ( type == 'C' ) ) && ( 1/*pIDX->IDX_Useable*/) ) {

//  TODO This is a ---HACK---
//  try to avoid double current arrows.  Select the first in the list only
//  Proper fix is to correct the TCDATA index file for depth indication
                bool b_dup = false;
                if( ( type == 'c' ) && ( lat == lat_last ) && ( lon == lon_last ) ) b_dup = true;

                if( !b_dup && ( BBox.PointInBox( lon, lat, 0 ) ) ) {

//    Manage the point selection list
                    if( bRebuildSelList ) pSelectTC->AddSelectablePoint( lat, lon, pIDX,
                                SELTYPE_CURRENTPOINT );

                    wxPoint r;
                    GetCanvasPointPix( lat, lon, &r );

                    wxPoint d[4];
                    int dd = 6;
                    d[0].x = r.x;
                    d[0].y = r.y + dd;
                    d[1].x = r.x + dd;
                    d[1].y = r.y;
                    d[2].x = r.x;
                    d[2].y = r.y - dd;
                    d[3].x = r.x - dd;
                    d[3].y = r.y;

                    if( ptcmgr->GetTideOrCurrent15( now, i, tcvalue, dir, bnew_val ) ) {
                        porange_pen->SetWidth( 1 );
                        dc.SetPen( *pblack_pen );
                        dc.SetBrush( *porange_brush );
                        dc.DrawPolygon( 4, d );

                        if( type == 'C' ) {
                            dc.SetBrush( *pblack_brush );
                            dc.DrawCircle( r.x, r.y, 2 );
                        }

                        else if( ( type == 'c' ) && ( GetVP().chart_scale < 1000000 ) ) {
                            if( bnew_val || bforce_redraw_currents ) {

//    Get the display pixel location of the current station
                                int pixxc, pixyc;
                                wxPoint cpoint;
                                GetCanvasPointPix( lat, lon, &cpoint );
                                pixxc = cpoint.x;
                                pixyc = cpoint.y;

//    Draw arrow using preset parameters, see mm_per_knot variable
//                                                            double scale = fabs ( tcvalue ) * current_draw_scaler;
//    Adjust drawing size using logarithmic scale
                                double a1 = fabs( tcvalue ) * 10.;
                                a1 = wxMax(1.0, a1);      // Current values less than 0.1 knot
                                // will be displayed as 0
                                double a2 = log10( a1 );

                                double scale = current_draw_scaler * a2;

                                porange_pen->SetWidth( 2 );
                                dc.SetPen( *porange_pen );
                                DrawArrow( dc, pixxc, pixyc,
                                           dir - 90 + ( skew_angle * 180. / PI ), scale / 100 );
// Draw text, if enabled

                                if( bDrawCurrentValues ) {
                                    dc.SetFont( *pTCFont );
                                    snprintf( sbuf, 19, "%3.1f", fabs( tcvalue ) );
                                    dc.DrawText( wxString( sbuf, wxConvUTF8 ), pixxc, pixyc );
                                }
                            }
                        }           // scale
                    }
                    /*          This is useful for debugging the TC database
                     else
                     {
                     dc.SetPen ( *porange_pen );
                     dc.SetBrush ( *pgray_brush );
                     dc.DrawPolygon ( 4, d );
                     }
                     */

                }
                lon_last = lon;
                lat_last = lat;

            }
        }
    }
}

void ChartCanvas::DrawTCWindow( int x, int y, void *pvIDX )
{
    pCwin = new TCWin( this, x, y, pvIDX );

}

#define NUM_CURRENT_ARROW_POINTS 9
static wxPoint CurrentArrowArray[NUM_CURRENT_ARROW_POINTS] = { wxPoint( 0, 0 ), wxPoint( 0, -10 ),
        wxPoint( 55, -10 ), wxPoint( 55, -25 ), wxPoint( 100, 0 ), wxPoint( 55, 25 ), wxPoint( 55,
                10 ), wxPoint( 0, 10 ), wxPoint( 0, 0 )
                                                             };

void ChartCanvas::DrawArrow( ocpnDC& dc, int x, int y, double rot_angle, double scale )
{
    if( scale > 1e-2 ) {

        float sin_rot = sin( rot_angle * PI / 180. );
        float cos_rot = cos( rot_angle * PI / 180. );

        // Move to the first point

        float xt = CurrentArrowArray[0].x;
        float yt = CurrentArrowArray[0].y;

        float xp = ( xt * cos_rot ) - ( yt * sin_rot );
        float yp = ( xt * sin_rot ) + ( yt * cos_rot );
        int x1 = (int) ( xp * scale );
        int y1 = (int) ( yp * scale );

        // Walk thru the point list
        for( int ip = 1; ip < NUM_CURRENT_ARROW_POINTS; ip++ ) {
            xt = CurrentArrowArray[ip].x;
            yt = CurrentArrowArray[ip].y;

            float xp = ( xt * cos_rot ) - ( yt * sin_rot );
            float yp = ( xt * sin_rot ) + ( yt * cos_rot );
            int x2 = (int) ( xp * scale );
            int y2 = (int) ( yp * scale );

            dc.DrawLine( x1 + x, y1 + y, x2 + x, y2 + y );

            x1 = x2;
            y1 = y2;
        }
    }
}

wxString ChartCanvas::FindValidUploadPort()
{
    wxString port;
    //  Try to use the saved persistent upload port first
    if( !g_uploadConnection.IsEmpty() &&  g_uploadConnection.StartsWith(_T("Serial") ) ) {
            port = g_uploadConnection;
    }

    else if( g_pConnectionParams ) {
    // If there is no persistent upload port recorded (yet)
            // then use the first available serial connection which has output defined.
            for( size_t i = 0; i < g_pConnectionParams->Count(); i++ ) {
                ConnectionParams *cp = g_pConnectionParams->Item( i );
                if( (cp->IOSelect != DS_TYPE_INPUT) && cp->Type == SERIAL )
                    port << _T("Serial:") << cp->Port;
            }
    }

    return port;
}

void ShowAISTargetQueryDialog( wxWindow *win, int mmsi )
{
    if( !win ) return;

    if( NULL == g_pais_query_dialog_active ) {
        int pos_x = g_ais_query_dialog_x;
        int pos_y = g_ais_query_dialog_y;

        if( g_pais_query_dialog_active ) {
            delete g_pais_query_dialog_active;
            g_pais_query_dialog_active = new AISTargetQueryDialog();
        }
        else {
            g_pais_query_dialog_active = new AISTargetQueryDialog();
        }

        g_pais_query_dialog_active->Create( win, -1, _( "AIS Target Query" ),
                                            wxPoint( pos_x, pos_y ) );

        g_pais_query_dialog_active->SetMMSI( mmsi );
        g_pais_query_dialog_active->UpdateText();
        wxSize sz = g_pais_query_dialog_active->GetSize();

        bool b_reset_pos = false;
#ifdef __WXMSW__
        //  Support MultiMonitor setups which an allow negative window positions.
        //  If the requested window title bar does not intersect any installed monitor,
        //  then default to simple primary monitor positioning.
        RECT frame_title_rect;
        frame_title_rect.left = pos_x;
        frame_title_rect.top = pos_y;
        frame_title_rect.right = pos_x + sz.x;
        frame_title_rect.bottom = pos_y + 30;

        if( NULL == MonitorFromRect( &frame_title_rect, MONITOR_DEFAULTTONULL ) ) b_reset_pos =
                true;
#else

        //    Make sure drag bar (title bar) of window intersects wxClient Area of screen, with a little slop...
        wxRect window_title_rect;// conservative estimate
        window_title_rect.x = pos_x;
        window_title_rect.y = pos_y;
        window_title_rect.width = sz.x;
        window_title_rect.height = 30;

        wxRect ClientRect = wxGetClientDisplayRect();
        ClientRect.Deflate(60, 60);// Prevent the new window from being too close to the edge
        if(!ClientRect.Intersects(window_title_rect))
            b_reset_pos = true;

#endif

        if( b_reset_pos ) g_pais_query_dialog_active->Move( 50, 200 );

    } else {
        g_pais_query_dialog_active->SetMMSI( mmsi );
        g_pais_query_dialog_active->UpdateText();
    }

    g_pais_query_dialog_active->Show();
}

#ifdef __WXGTK__
#define BRIGHT_XCALIB
#define __OPCPN_USEICC__
#endif

//--------------------------------------------------------------------------------------------------------
//    Screen Brightness Control Support Routines
//
//--------------------------------------------------------------------------------------------------------

#ifdef __OPCPN_USEICC__
int CreateSimpleICCProfileFile(const char *file_name, double co_red, double co_green, double co_blue);

wxString temp_file_name;
#endif

#if 0
class ocpnCurtain: public wxDialog
{
    DECLARE_CLASS( ocpnCurtain )
    DECLARE_EVENT_TABLE()

public:
    ocpnCurtain( wxWindow *parent, wxPoint position, wxSize size, long wstyle );
    ~ocpnCurtain( );
    bool ProcessEvent(wxEvent& event);

};

IMPLEMENT_CLASS ( ocpnCurtain, wxDialog )

BEGIN_EVENT_TABLE(ocpnCurtain, wxDialog)
END_EVENT_TABLE()

ocpnCurtain::ocpnCurtain( wxWindow *parent, wxPoint position, wxSize size, long wstyle )
{
    wxDialog::Create( parent, -1, _T("ocpnCurtain"), position, size, wxNO_BORDER | wxSTAY_ON_TOP );
}

ocpnCurtain::~ocpnCurtain()
{
}

bool ocpnCurtain::ProcessEvent(wxEvent& event)
{
    GetParent()->GetEventHandler()->SetEvtHandlerEnabled(true);
    return GetParent()->GetEventHandler()->ProcessEvent(event);
}
#endif

#ifdef __WIN32__
#include <windows.h>

HMODULE hGDI32DLL;
typedef BOOL (WINAPI *SetDeviceGammaRamp_ptr_type)( HDC hDC, LPVOID lpRampTable );
typedef BOOL (WINAPI *GetDeviceGammaRamp_ptr_type)( HDC hDC, LPVOID lpRampTable );
SetDeviceGammaRamp_ptr_type g_pSetDeviceGammaRamp;            // the API entry points in the dll
GetDeviceGammaRamp_ptr_type g_pGetDeviceGammaRamp;

WORD *g_pSavedGammaMap;

#endif

int InitScreenBrightness( void )
{
#ifdef __WIN32__
    if( g_bopengl ) {
        HDC hDC;
        BOOL bbr;

        if( NULL == hGDI32DLL ) {
            hGDI32DLL = LoadLibrary( TEXT("gdi32.dll") );

            if( NULL != hGDI32DLL ) {
                //Get the entry points of the required functions
                g_pSetDeviceGammaRamp = (SetDeviceGammaRamp_ptr_type) GetProcAddress( hGDI32DLL,
                                        "SetDeviceGammaRamp" );
                g_pGetDeviceGammaRamp = (GetDeviceGammaRamp_ptr_type) GetProcAddress( hGDI32DLL,
                                        "GetDeviceGammaRamp" );

                //    If the functions are not found, unload the DLL and return false
                if( ( NULL == g_pSetDeviceGammaRamp ) || ( NULL == g_pGetDeviceGammaRamp ) ) {
                    FreeLibrary( hGDI32DLL );
                    hGDI32DLL = NULL;
                    return 0;
                }
            }
        }

        //    Interface is ready, so....
        //    Get some storage
        if( !g_pSavedGammaMap ) {
            g_pSavedGammaMap = (WORD *) malloc( 3 * 256 * sizeof(WORD) );

            hDC = GetDC( NULL );                                      // Get the full screen DC
            bbr = g_pGetDeviceGammaRamp( hDC, g_pSavedGammaMap );    // Get the existing ramp table
            ReleaseDC( NULL, hDC );                                       // Release the DC
        }

        //    On Windows hosts, try to adjust the registry to allow full range setting of Gamma table
        //    This is an undocumented Windows hack.....
        wxRegKey *pRegKey =
            new wxRegKey(
            _T("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\ICM") );
        if( !pRegKey->Exists() ) pRegKey->Create();
        pRegKey->SetValue( _T("GdiIcmGammaRange"), 256 );

        g_brightness_init = true;
        return 1;
    }

    else {
        if( NULL == g_pcurtain ) {
            if( gFrame->CanSetTransparent() ) {
                //    Build the curtain window
                g_pcurtain = new wxDialog( cc1, -1, _T(""), wxPoint( 0, 0 ), ::wxGetDisplaySize(),
                                           wxNO_BORDER | wxTRANSPARENT_WINDOW | wxSTAY_ON_TOP | wxDIALOG_NO_PARENT );

                //                  g_pcurtain = new ocpnCurtain(gFrame, wxPoint(0,0),::wxGetDisplaySize(),
                //                      wxNO_BORDER | wxTRANSPARENT_WINDOW |wxSTAY_ON_TOP | wxDIALOG_NO_PARENT);

                g_pcurtain->Hide();

                HWND hWnd = GetHwndOf(g_pcurtain);
                SetWindowLong( hWnd, GWL_EXSTYLE,
                               GetWindowLong( hWnd, GWL_EXSTYLE ) | ~WS_EX_APPWINDOW );
                g_pcurtain->SetBackgroundColour( wxColour( 0, 0, 0 ) );
                g_pcurtain->SetTransparent( 0 );

                g_pcurtain->Maximize();
                g_pcurtain->Show();

                //    All of this is obtuse, but necessary for Windows...
                g_pcurtain->Enable();
                g_pcurtain->Disable();

                gFrame->Disable();
                gFrame->Enable();
                cc1->SetFocus();

            }
        }
        g_brightness_init = true;

        return 1;
    }
#else
    //    Look for "xcalib" application
    wxString cmd ( _T ( "xcalib -version" ) );

    wxArrayString output;
    long r = wxExecute ( cmd, output );
#ifdef __WXOSX__
    if(0 != r)
    {wxLogMessage(_("   External application \"xcalib\" not found. Screen brightness not changed."));}
#else
    if(0 != r)
        wxLogMessage(_("   External application \"xcalib\" not found. Screen brightness not changed."));
#endif

    g_brightness_init = true;
    return 0;
#endif
}

int RestoreScreenBrightness( void )
{
#ifdef __WIN32__

    if( g_pSavedGammaMap ) {
        HDC hDC = GetDC( NULL );                                 // Get the full screen DC
        g_pSetDeviceGammaRamp( hDC, g_pSavedGammaMap );          // Restore the saved ramp table
        ReleaseDC( NULL, hDC );                                  // Release the DC

        free( g_pSavedGammaMap );
        g_pSavedGammaMap = NULL;
    }

    if( g_pcurtain ) {
        g_pcurtain->Close();
        g_pcurtain->Destroy();
        g_pcurtain = NULL;
    }

    g_brightness_init = false;
    return 1;

#endif

#ifdef BRIGHT_XCALIB
    if(g_brightness_init)
    {
        wxString cmd;
        cmd = _T("xcalib -clear");
        wxExecute(cmd, wxEXEC_ASYNC);
        g_brightness_init = false;
    }

    return 1;
#endif

    return 0;
}

//    Set brightness. [0..100]
int SetScreenBrightness( int brightness )
{
#ifdef __WIN32__

    //    Under Windows, we use the SetDeviceGammaRamp function which exists in some (most modern?) versions of gdi32.dll
    //    Load the required library dll, if not already in place
    if( g_bopengl ) {
        if( g_pcurtain ) {
            g_pcurtain->Close();
            g_pcurtain->Destroy();
            g_pcurtain = NULL;
        }

        InitScreenBrightness();

        if( NULL == hGDI32DLL ) {
            // Unicode stuff.....
            wchar_t wdll_name[80];
            MultiByteToWideChar( 0, 0, "gdi32.dll", -1, wdll_name, 80 );
            LPCWSTR cstr = wdll_name;

            hGDI32DLL = LoadLibrary( cstr );

            if( NULL != hGDI32DLL ) {
                //Get the entry points of the required functions
                g_pSetDeviceGammaRamp = (SetDeviceGammaRamp_ptr_type) GetProcAddress( hGDI32DLL,
                                        "SetDeviceGammaRamp" );
                g_pGetDeviceGammaRamp = (GetDeviceGammaRamp_ptr_type) GetProcAddress( hGDI32DLL,
                                        "GetDeviceGammaRamp" );

                //    If the functions are not found, unload the DLL and return false
                if( ( NULL == g_pSetDeviceGammaRamp ) || ( NULL == g_pGetDeviceGammaRamp ) ) {
                    FreeLibrary( hGDI32DLL );
                    hGDI32DLL = NULL;
                    return 0;
                }
            }
        }

        HDC hDC = GetDC( NULL );                          // Get the full screen DC

        /*
         int cmcap = GetDeviceCaps(hDC, COLORMGMTCAPS);
         if (cmcap != CM_GAMMA_RAMP)
         {
         wxLogMessage(_T("    Video hardware does not support brightness control by gamma ramp adjustment."));
         return false;
         }
         */

        int increment = brightness * 256 / 100;

        // Build the Gamma Ramp table
        WORD GammaTable[3][256];

        int table_val = 0;
        for( int i = 0; i < 256; i++ ) {

            GammaTable[0][i] = r_gamma_mult * (WORD) table_val;
            GammaTable[1][i] = g_gamma_mult * (WORD) table_val;
            GammaTable[2][i] = b_gamma_mult * (WORD) table_val;

            table_val += increment;

            if( table_val > 65535 ) table_val = 65535;

        }

        g_pSetDeviceGammaRamp( hDC, GammaTable );          // Set the ramp table
        ReleaseDC( NULL, hDC );                                     // Release the DC

        return 1;
    } else {
        if( g_pSavedGammaMap ) {
            HDC hDC = GetDC( NULL );                                       // Get the full screen DC
            g_pSetDeviceGammaRamp( hDC, g_pSavedGammaMap );          // Restore the saved ramp table
            ReleaseDC( NULL, hDC );                                             // Release the DC
        }

        if( NULL == g_pcurtain ) InitScreenBrightness();

        if( g_pcurtain ) {
            int sbrite = wxMax(1, brightness);
            sbrite = wxMin(100, sbrite);

            g_pcurtain->SetTransparent( ( 100 - sbrite ) * 256 / 100 );
        }
        return 1;
    }

#endif

#ifdef BRIGHT_XCALIB

    if(!g_brightness_init)
    {
        last_brightness = 100;
        g_brightness_init = true;
        temp_file_name = wxFileName::CreateTempFileName(_T(""));
        InitScreenBrightness();
    }

#ifdef __OPCPN_USEICC__
    //  Create a dead simple temporary ICC profile file, with gamma ramps set as desired,
    //  and then activate this temporary profile using xcalib <filename>
    if(!CreateSimpleICCProfileFile ( ( const char * ) temp_file_name.fn_str(),
                                     brightness * r_gamma_mult,
                                     brightness * g_gamma_mult,
                                     brightness * b_gamma_mult ))
    {
        wxString cmd ( _T ( "xcalib " ) );
        cmd += temp_file_name;

        wxExecute ( cmd, wxEXEC_ASYNC );
    }

#else
    //    Or, use "xcalib -co" to set overall contrast value
    //    This is not as nice, since the -co parameter wants to be a fraction of the current contrast,
    //    and values greater than 100 are not allowed.  As a result, increases of contrast must do a "-clear" step
    //    first, which produces objectionable flashing.
    if(brightness > last_brightness)
    {
        wxString cmd;
        cmd = _T("xcalib -clear");
        wxExecute(cmd, wxEXEC_ASYNC);

        ::wxMilliSleep(10);

        int brite_adj = wxMax(1, brightness);
        cmd.Printf(_T("xcalib -co %2d -a"), brite_adj);
        wxExecute(cmd, wxEXEC_ASYNC);
    }
    else
    {
        int brite_adj = wxMax(1, brightness);
        int factor = (brite_adj * 100) / last_brightness;
        factor = wxMax(1, factor);
        wxString cmd;
        cmd.Printf(_T("xcalib -co %2d -a"), factor);
        wxExecute(cmd, wxEXEC_ASYNC);
    }

#endif

    last_brightness = brightness;

#endif

    return 0;
}

#ifdef __OPCPN_USEICC__

#define MLUT_TAG     0x6d4c5554L
#define VCGT_TAG     0x76636774L

int GetIntEndian(unsigned char *s)
{
    int ret;
    unsigned char *p;
    int i;

    p = (unsigned char *)&ret;

    if(1)
        for(i=sizeof(int)-1; i>-1; --i)
            *p++ = s[i];
    else
        for(i=0; i<(int)sizeof(int); ++i)
            *p++ = s[i];

    return ret;
}

unsigned short GetShortEndian(unsigned char *s)
{
    unsigned short ret;
    unsigned char *p;
    int i;

    p = (unsigned char *)&ret;

    if(1)
        for(i=sizeof(unsigned short)-1; i>-1; --i)
            *p++ = s[i];
    else
        for(i=0; i<(int)sizeof(unsigned short); ++i)
            *p++ = s[i];

    return ret;
}

//    Create a very simple Gamma correction file readable by xcalib
int CreateSimpleICCProfileFile(const char *file_name, double co_red, double co_green, double co_blue)
{
    FILE *fp;

    if(file_name)
    {
        fp = fopen(file_name, "wb");
        if(!fp)
            return -1; /* file can not be created */
    }
    else
        return -1; /* filename char pointer not valid */

    //    Write header
    char header[128];
    for(int i=0; i< 128; i++)
        header[i] = 0;

    fwrite(header, 128, 1, fp);

    //    Num tags
    int numTags0 = 1;
    int numTags = GetIntEndian((unsigned char *)&numTags0);
    fwrite(&numTags, 1, 4, fp);

    int tagName0 = VCGT_TAG;
    int tagName = GetIntEndian((unsigned char *)&tagName0);
    fwrite(&tagName, 1, 4, fp);

    int tagOffset0 = 128 + 4 * sizeof(int);
    int tagOffset = GetIntEndian((unsigned char *)&tagOffset0);
    fwrite(&tagOffset, 1, 4, fp);

    int tagSize0 = 1;
    int tagSize = GetIntEndian((unsigned char *)&tagSize0);
    fwrite(&tagSize, 1, 4, fp);

    fwrite(&tagName, 1, 4, fp);// another copy of tag

    fwrite(&tagName, 1, 4, fp);// dummy

    //  Table type

    /* VideoCardGammaTable (The simplest type) */
    int gammatype0 = 0;
    int gammatype = GetIntEndian((unsigned char *)&gammatype0);
    fwrite(&gammatype, 1, 4, fp);

    int numChannels0 = 3;
    unsigned short numChannels = GetShortEndian((unsigned char *)&numChannels0);
    fwrite(&numChannels, 1, 2, fp);

    int numEntries0 = 256;
    unsigned short numEntries = GetShortEndian((unsigned char *)&numEntries0);
    fwrite(&numEntries, 1, 2, fp);

    int entrySize0 = 1;
    unsigned short entrySize = GetShortEndian((unsigned char *)&entrySize0);
    fwrite(&entrySize, 1, 2, fp);

    unsigned char ramp[256];

    //    Red ramp
    for(int i=0; i< 256; i++)
        ramp[i] = i * co_red/100.;
    fwrite(ramp, 256, 1, fp);

    //    Green ramp
    for(int i=0; i< 256; i++)
        ramp[i] = i * co_green/100.;
    fwrite(ramp, 256, 1, fp);

    //    Blue ramp
    for(int i=0; i< 256; i++)
        ramp[i] = i * co_blue/100.;
    fwrite(ramp, 256, 1, fp);

    fclose(fp);

    return 0;
}
#endif // __OPCPN_USEICC__

void DimeControl( wxWindow* ctrl )
{
    if( NULL == ctrl ) return;

    wxColour col, col1, gridline, uitext, udkrd, back_color, text_color;
    col = GetGlobalColor( _T("DILG0") );       // Dialog Background white
    col1 = GetGlobalColor( _T("DILG1") );      // Dialog Background
    back_color = GetGlobalColor( _T("DILG1") );      // Control Background
    text_color = GetGlobalColor( _T("DILG3") );      // Text
    uitext = GetGlobalColor( _T("UITX1") );    // Menu Text, derived from UINFF
    udkrd = GetGlobalColor( _T("UDKRD") );
    gridline = GetGlobalColor( _T("GREY2") );

    DimeControl( ctrl, col, col1, back_color, text_color, uitext, udkrd, gridline );
}

void DimeControl( wxWindow* ctrl, wxColour col, wxColour col1, wxColour back_color,
                  wxColour text_color, wxColour uitext, wxColour udkrd, wxColour gridline )
{
    ColorScheme cs = cc1->GetColorScheme();
    
    //  If the color scheme is DAY or RGB, use the default platform native colour for backgrounds
    wxColour window_back_color = wxNullColour;
    if( cs != GLOBAL_COLOR_SCHEME_DAY && cs != GLOBAL_COLOR_SCHEME_RGB )
        window_back_color = back_color;

    ctrl->SetBackgroundColour( window_back_color );
    
#ifdef __WXMAC__
#if wxCHECK_VERSION(2,9,0)
    if( cs != GLOBAL_COLOR_SCHEME_DAY && cs != GLOBAL_COLOR_SCHEME_RGB )
        ctrl->SetBackgroundColour( back_color );
    else
        ctrl->SetBackgroundColour( wxColour( 0xff, 0xff, 0xff ));
#endif
#endif

    wxWindowList kids = ctrl->GetChildren();
    for( unsigned int i = 0; i < kids.GetCount(); i++ ) {
        wxWindowListNode *node = kids.Item( i );
        wxWindow *win = node->GetData();

        if( win->IsKindOf( CLASSINFO(wxListBox) ) )
            ( (wxListBox*) win )->SetBackgroundColour( col );

        if( win->IsKindOf( CLASSINFO(wxListCtrl) ) )
            ( (wxListCtrl*) win )->SetBackgroundColour( col1 );

        if( win->IsKindOf( CLASSINFO(wxTextCtrl) ) )
            ( (wxTextCtrl*) win )->SetBackgroundColour( col );

        if( win->IsKindOf( CLASSINFO(wxStaticText) ) )
            ( (wxStaticText*) win )->SetForegroundColour( uitext );

        else if( win->IsKindOf( CLASSINFO(wxBitmapComboBox) ) ) {
#if wxCHECK_VERSION(2,9,0) && !wxCHECK_VERSION(3,0,0) // maybe remove as it only works in wx2.9 ?
            if( ( ( wxBitmapComboBox*) win )->GetTextCtrl() )
                ( (wxBitmapComboBox*) win )->GetTextCtrl()->SetBackgroundColour(col);
#else
            ( (wxBitmapComboBox*) win )->SetBackgroundColour( col );
#endif
        }

        else if( win->IsKindOf( CLASSINFO(wxChoice) ) )
            ( (wxChoice*) win )->SetBackgroundColour( col );

        else if( win->IsKindOf( CLASSINFO(wxComboBox) ) )
            ( (wxComboBox*) win )->SetBackgroundColour( col );

        else if( win->IsKindOf( CLASSINFO(wxScrolledWindow) ) )
            ( (wxScrolledWindow*) win )->SetBackgroundColour( window_back_color );

        else if( win->IsKindOf( CLASSINFO(wxGenericDirCtrl) ) )
            ( (wxGenericDirCtrl*) win )->SetBackgroundColour( col1 );

        else if( win->IsKindOf( CLASSINFO(wxListbook) ) )
            ( (wxListbook*) win )->SetBackgroundColour( col1 );

        else if( win->IsKindOf( CLASSINFO(wxTreeCtrl) ) )
            ( (wxTreeCtrl*) win )->SetBackgroundColour( col );

        else if( win->IsKindOf( CLASSINFO(wxRadioButton) ) )
            ( (wxRadioButton*) win )->SetBackgroundColour( window_back_color );

        else if( win->IsKindOf( CLASSINFO(wxNotebook) ) ) {
            ( (wxNotebook*) win )->SetBackgroundColour( col1 );
            ( (wxNotebook*) win )->SetForegroundColour( text_color );
        }

        else if( win->IsKindOf( CLASSINFO(wxButton) ) ) {
            ( (wxButton*) win )->SetBackgroundColour( col1 );
        }

        else if( win->IsKindOf( CLASSINFO(wxToggleButton) ) ) {
            ( (wxToggleButton*) win )->SetBackgroundColour( window_back_color );
        }

        else if( win->IsKindOf( CLASSINFO(wxPanel) ) ) {
//                  ((wxPanel*)win)->SetBackgroundColour(col1);
            if( cs != GLOBAL_COLOR_SCHEME_DAY
                    && cs != GLOBAL_COLOR_SCHEME_RGB ) ( (wxPanel*) win )->SetBackgroundColour(
                            back_color );
            else
                ( (wxPanel*) win )->SetBackgroundColour(
                    wxNullColour );
        }

        else if( win->IsKindOf( CLASSINFO(wxHtmlWindow) ) ) {
            if( cs != GLOBAL_COLOR_SCHEME_DAY
                    && cs != GLOBAL_COLOR_SCHEME_RGB ) ( (wxPanel*) win )->SetBackgroundColour(
                            back_color );
            else
                ( (wxPanel*) win )->SetBackgroundColour(
                    wxNullColour );

        }

        else if( win->IsKindOf( CLASSINFO(wxGrid) ) ) {
            ( (wxGrid*) win )->SetDefaultCellBackgroundColour(
                col1 );
            ( (wxGrid*) win )->SetDefaultCellTextColour(
                uitext );
            ( (wxGrid*) win )->SetLabelBackgroundColour(
                col );
            ( (wxGrid*) win )->SetLabelTextColour(
                uitext );
//            ( (wxGrid*) win )->SetDividerPen(
//                wxPen( col ) );
            ( (wxGrid*) win )->SetGridLineColour(
                gridline );
        }

        else {
            ;
        }

        if( win->GetChildren().GetCount() > 0 ) {
            wxWindow * w = win;
            DimeControl( w, col, col1, back_color, text_color, uitext, udkrd, gridline );
        }
    }
}

