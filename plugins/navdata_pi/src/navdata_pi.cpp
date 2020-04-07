/* *************************************************************************
 *
 * Project:  OpenCPN - plugin navdata_pi
 * Purpose:  ROUTE Plugin
 * Author:   Walbert Schulpen (SaltyPaws)
 *
 ***************************************************************************
 *   Copyright (C) 2012-2016 by Walbert Schulpen                           *
 *   $EMAIL$                                                               *
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include "navdata_pi.h"
#include "icons.h"

#include "GL/gl.h"

#include "../../../include/wx/jsonreader.h"
#include "../../../include/wx/jsonwriter.h"

// the class factories, used to create and destroy instances of the PlugIn

extern "C" DECL_EXP opencpn_plugin* create_pi(void *ppimgr)
{
    return new navdata_pi(ppimgr);
}

extern "C" DECL_EXP void destroy_pi(opencpn_plugin* p)
{
    delete p;
}
// static variables
wxString       g_shareLocn;
wxString       g_activeRouteGuid;
wxString       g_selectedPointGuid;
int            g_blinkTrigger;
int            g_selectedPointCol;
bool           g_showTripData;
bool           g_withSog;
wxPoint        g_scrollPos;

int NextPow2(int size)
{
    int n = size-1;          // compute dimensions needed as next larger power of 2
    int shift = 1;
    while ((n+1) & n){
        n |= n >> shift;
        shift <<= 1;
    }
    return n + 1;
}

wxWindow *GetNAVCanvas()
{
    wxWindow *wx;
    // If multicanvas are active, render the overlay on the Left canvas only
    if(GetCanvasCount() > 1)            // multi?
        wx = GetCanvasByIndex(0);
    else
        wx = GetOCPNCanvasWindow();
    return wx;
}

//-------------------------------------------------------
//          PlugIn initialization and de-init
//-------------------------------------------------------

navdata_pi::navdata_pi(void *ppimgr)
      :opencpn_plugin_116(ppimgr), wxTimer(this)
{
      // Create the PlugIn icons
      initialize_images();
}

navdata_pi::~navdata_pi(void)
{
    delete _img_active;
    delete _img_inactive;
    delete _img_toggled;
    delete _img_target;
    delete _img_activewpt;
    delete _img_targetwpt;
    delete _img_setting;
    delete m_vp;
 }

int navdata_pi::Init(void){
    //connect timers
    m_lenghtTimer.Connect(wxEVT_TIMER, wxTimerEventHandler(navdata_pi::OnTripLenghtTimer), NULL, this );
    m_rotateTimer.Connect(wxEVT_TIMER, wxTimerEventHandler(navdata_pi::OnRotateTimer), NULL, this );

    AddLocaleCatalog( _T("opencpn-navdata_pi") );

    m_pTable = NULL;
    m_vp = NULL;
    m_ptripData = NULL;

    g_selectedPointGuid = wxEmptyString;
    m_gTrkGuid = wxEmptyString;
    g_activeRouteGuid = wxEmptyString;
    m_gMustRotate = false;
    m_gHasRotated = false;
    g_blinkTrigger = 0;
	m_gWmmVar = NAN;

    //find and store share path
#ifdef __WXOSX__
    g_shareLocn =*GetpPrivateApplicationDataLocation() +
#else
    g_shareLocn =*GetpSharedDataLocation() +
#endif
                    _T("plugins") + wxFileName::GetPathSeparator() +
                    _T("navdata_pi") + wxFileName::GetPathSeparator()
                    +_T("data") + wxFileName::GetPathSeparator();

    //    This PlugIn needs a toolbar icon, so request its insertion
    wxString active = g_shareLocn + _T("active.svg");
    wxString toggled = g_shareLocn + _T("toggled.svg");
    if( wxFile::Exists( active) && wxFile::Exists( toggled) )
        m_leftclick_tool_id  = InsertPlugInToolSVG(_T(""), active, active, toggled,
                    wxITEM_CHECK, _("Navigation data"), _T(""), NULL, CALCULATOR_TOOL_POSITION, 0, this);
    else
        m_leftclick_tool_id  = InsertPlugInTool(_T(""), _img_active, _img_toggled,
                        wxITEM_CHECK, _("Navigation data"), _T(""), NULL, CALCULATOR_TOOL_POSITION, 0, this);

    return (WANTS_OVERLAY_CALLBACK          |
            WANTS_ONPAINT_VIEWPORT          |
            WANTS_OPENGL_OVERLAY_CALLBACK   |
            WANTS_TOOLBAR_CALLBACK          |
            INSTALLS_TOOLBAR_TOOL           |
            WANTS_PREFERENCES               |
            WANTS_PLUGIN_MESSAGING          |
            WANTS_NMEA_EVENTS               |
            WANTS_CONFIG                    |
            WANTS_MOUSE_EVENTS
           );
}

bool navdata_pi::DeInit(void)
{
    //disconnect timers
    m_lenghtTimer.Disconnect(wxEVT_TIMER, wxTimerEventHandler(navdata_pi::OnTripLenghtTimer), NULL, this );
    m_rotateTimer.Disconnect(wxEVT_TIMER, wxTimerEventHandler(navdata_pi::OnRotateTimer), NULL, this );

    if(m_ptripData){
        if(!m_ptripData->m_isEnded){
            m_ptripData->m_totalDist += GetDistFromLastTrkPoint(m_gLat, m_gLon);
            m_ptripData->m_isEnded = true;
            m_ptripData->m_endTime = wxDateTime::Now();
            //wxString s = wxString::Format(wxString(_T("Do you want to save this trip\nlenght = %f")), m_ptripData->m_totalDist);
            //OCPNMessageBox_PlugIn(GetNAVCanvas(),s, _T("answer"));
        }
        delete m_ptripData;
        m_ptripData = NULL;
    }

    if( m_pTable ){
		CloseDataTable();
    }
	return true;
}

int navdata_pi::GetAPIVersionMajor()
{
      return MY_API_VERSION_MAJOR;
}

int navdata_pi::GetAPIVersionMinor()
{
      return MY_API_VERSION_MINOR;
}

int navdata_pi::GetPlugInVersionMajor()
{
      return PLUGIN_VERSION_MAJOR;
}

int navdata_pi::GetPlugInVersionMinor()
{
      return PLUGIN_VERSION_MINOR;
}

wxBitmap *navdata_pi::GetPlugInBitmap()
{
      return _img_active;;
}

wxString navdata_pi::GetCommonName()
{
      return _T("NAVDATA");
}

wxString navdata_pi::GetShortDescription()
{
      return _("Navigation data plugin for OpenCPN");
}

wxString navdata_pi::GetLongDescription()
{
      return _("Navigation data plugin for OpenCPN\nShows RNG (range),TTG (time to go) and ETA (estimated time of arrival)\nfor all active route points.\nAlso shows current trip summary:\nstart time, time spent, distance, mean speed since departure...");
}

int navdata_pi::GetToolbarToolCount(void)
{
      return 1;
}

void navdata_pi::LoadocpnConfig()
{
      wxFileConfig *pConf = GetOCPNConfigObject();
      if(pConf)
      {
		  bool showtrue, showmag;
          pConf->SetPath(_T("/Settings"));
		  pConf->Read(_T("ShowTrue"), &showtrue, 0);
          pConf->Read(_T("ShowMag"), &showmag, 0);
          pConf->Read(_T("UserMagVariation"), &m_ocpnUserVar, 0);
          pConf->Read(_T("DistanceFormat"), &m_ocpnDistFormat, 0);
          pConf->Read(_T("SpeedFormat"), &m_ocpnSpeedFormat, 0);
          pConf->Read(_T("OpenGL"), &m_ocpnOpenGL, 1);
          if(IsTouchInterface_PlugIn()){
              pConf->Read( _T ( "SelectionRadiusTouchMM" ), &m_selectionRadiusMM);
              m_selectionRadiusMM = wxMax(m_selectionRadiusMM, 1.0);
          } else {
              pConf->Read( _T ( "SelectionRadiusMM" ), &m_selectionRadiusMM);
              m_selectionRadiusMM = wxMax(m_selectionRadiusMM, 0.5);
          }
          m_ocpnStyleBrg = showmag ? showtrue ? 2 : 1 : 0;
	  }
}

void navdata_pi::SetColorScheme(PI_ColorScheme cs)
{
    if( m_pTable ){
        m_pTable->DimGridDialog();
        RequestRefresh( m_pTable );
    }
}

void navdata_pi::SetPluginMessage(wxString &message_id, wxString &message_body)
{
    if(message_id == _T("OCPN_TRACKPOINTS_COORDS"))
    {
		//compute the lenght of the track if requested by "m_lenghtTimer"
        wxJSONValue  root;
        wxJSONReader reader;
        int rnumErrors = reader.Parse( message_body, &root );
        if ( rnumErrors > 0 ) return;
        if( root[_T("error")].AsBool() ){
            if(m_ptripData->m_isEnded && GetOcpnDailyTrack( NULL, NULL)){
                /*when "DailyTrack" is set it is impossible to get the last track point as
                * the GUI of the extended track is unknown so get the distance from the last known
                * track point to the current boat position*/
                m_ptripData->m_tempDist = 0;
                m_ptripData->m_totalDist += GetDistFromLastTrkPoint( m_end_gLat, m_end_gLon );
                if ( m_pTable )
                    m_pTable->UpdateTripData( m_ptripData );
                m_gTrkGuid = wxEmptyString;
            }
            return;
        }
        if( root[_T("Track_ID")].AsString() == m_gTrkGuid ) {
            int NodeNr = root[_T("NodeNr")].AsInt();
            int TotalNodes = root[_T("TotalNodes")].AsInt();
            if( NodeNr >= m_gNodeNbr ){ //skip track point already treated
                double lat = root[_T("lat")].AsDouble();
                double lon = root[_T("lon")].AsDouble();
                if( NodeNr > TRACKPOINT_FIRST ) //more than one track point
                    m_ptripData->m_totalDist += GetDistFromLastTrkPoint(lat, lon);
                m_oldtpLat = lat;
                m_oldtpLon = lon;
                m_gNodeNbr = NodeNr + 1;
            } // NodeNr >= m_gNodeNbr
            if( NodeNr == TotalNodes ) { //the last track point
                /* 1)if the boat has moved compute and add distance from the last created point
                 * 2) if we are still on first track point and no movement has been made,
                 *  change continuously the starting time except if we are in rotation situation
                 * 3)if the trip is ended, get the last created point then stop calc
                 * 4)then update the trip data and eventually re-start the lenght timer for
                 *  the next lap;*/
                double dist = 0.;
                if(m_oldtpLat != m_gLat || m_oldtpLon != m_gLon){ //the boat has moved
                    if(!m_ptripData->m_isEnded){
                        dist = GetDistFromLastTrkPoint(m_gLat, m_gLon);
                        if( dist < .001 ) //no significant movement
                            dist = 0.;
                    }
                } //
                if( TotalNodes == TRACKPOINT_FIRST ){ //only one point created
					if (!m_gHasRotated) {
						if (dist == 0.) { //no movement
                            m_ptripData->m_startDate = wxDateTime::Now();
						}
					}
                } else //
                    m_gHasRotated = false;
                m_ptripData->m_endTime = wxDateTime::Now();
                m_ptripData->m_tempDist = dist;
				if ( m_pTable )
                    m_pTable->UpdateTripData(m_ptripData);
                if( !m_ptripData->m_isEnded ) //re-start lenght calc
                    m_lenghtTimer.Start(TIMER_INTERVAL_SECOND, wxTIMER_ONE_SHOT);
                else //end of Trip
                    m_gTrkGuid = wxEmptyString;
            }//NodeNr == TotalNodes
        }
    }
    else if(message_id == _T("OCPN_RTE_ACTIVATED"))
    {
        // construct the JSON root object
        wxJSONValue  root;
        // construct a JSON parser
        wxJSONReader reader;
        // now read the JSON text and store it in the 'root' structure
        // check for errors before retreiving values...
        int rnumErrors = reader.Parse( message_body, &root );
        if ( rnumErrors == 0 )  {
            // get route GUID values from the JSON message
            g_activeRouteGuid = root[_T("GUID")].AsString();
        }
    }

    else if(message_id == _T("OCPN_TRK_ACTIVATED"))
    {
        wxJSONValue  root;
        wxJSONReader reader;
        int rnumErrors = reader.Parse( message_body, &root );
        if ( rnumErrors == 0 )  {
            m_gTrkGuid = root[_T("GUID")].AsString();
            m_gNodeNbr = 0;
            if( m_pTable )
                m_pTable->UpdateTripData();
            if( m_gMustRotate ){
                m_gHasRotated = true;
            } else {
                m_gHasRotated = false;
                if( m_ptripData ){
                    delete m_ptripData;
                    m_ptripData = NULL;
                }
                m_ptripData = new TripData();
            }
			m_gMustRotate = false;
			m_lenghtTimer.Start( TIMER_INTERVAL_MSECOND, wxTIMER_ONE_SHOT );
            m_rotateTimer.Start( TIMER_INTERVAL_10SECOND, wxTIMER_ONE_SHOT);
        }
    }

    else if(message_id == _T("OCPN_WPT_ACTIVATED"))
    {
        wxJSONValue  p1root;
        wxJSONReader p1reader;
        int pnumErrors = p1reader.Parse( message_body, &p1root );
        if ( pnumErrors == 0 ){
            m_activePointGuid = p1root[_T("GUID")].AsString();
            if( m_pTable ){
                m_pTable->UpdateRouteData( m_activePointGuid, m_gLat, m_gLon, m_gCog, m_gSog );
                m_pTable->SetTableSizePosition(false);
            }
        }
    }

    else if(message_id == _T("OCPN_WPT_ARRIVED"))
    {
        wxJSONValue  p2root;
        wxJSONReader p2reader;
        int p2numErrors = p2reader.Parse( message_body, &p2root );
        if ( p2numErrors == 0 ) {
            if( p2root.HasMember(_T("Next_WP"))){
                m_activePointGuid = p2root[_T("GUID")].AsString();
                if( m_pTable )
                    m_pTable->UpdateRouteData( m_activePointGuid, m_gLat, m_gLon, m_gCog, m_gSog );
            }
        }

    }

    else if(message_id == _T("OCPN_RTE_DEACTIVATED") || message_id == _T("OCPN_RTE_ENDED") )
    {
        m_activePointGuid = wxEmptyString;
        g_activeRouteGuid = wxEmptyString;

        if(m_pTable){
            m_pTable->UpdateRouteData();
            m_pTable->SetTableSizePosition(false);
        }
    }

    else if(message_id == _T("OCPN_TRK_DEACTIVATED"))
    {
        m_rotateTimer.Stop();

        if(m_gMustRotate) return;

        m_end_gLat = m_gLat;
        m_end_gLon = m_gLon;
        m_ptripData->m_isEnded = true;
        m_ptripData->m_endTime = wxDateTime::Now();

        //get the last created track point to complete trip data
        m_lenghtTimer.Start(TIMER_INTERVAL_10MSECOND, wxTIMER_ONE_SHOT);

    }

    else if(message_id == _T("WMM_VARIATION_BOAT"))
    {
        wxJSONValue  root;
        wxJSONReader reader;
        int numErrors = reader.Parse( message_body, &root );
        if ( numErrors == 0 )
            m_gWmmVar = root[_T("Decl")].AsDouble();
    }
}

double navdata_pi::GetDistFromLastTrkPoint(double lat, double lon)
{
    double dist = 0.;
        const double deltaLat = m_oldtpLat - lat;
        if ( fabs( deltaLat ) > OFFSET_LAT )
            dist = DistGreatCircle_Plugin( m_oldtpLat, m_oldtpLon, lat, lon );
        else
            dist = DistGreatCircle_Plugin( m_oldtpLat + copysign( OFFSET_LAT, deltaLat ), m_oldtpLon,  lat, lon );

        return dist;
}

void navdata_pi::SetPositionFix(PlugIn_Position_Fix &pfix)
{
    //update route data
    m_gLat = pfix.Lat;
    m_gLon = pfix.Lon;
    m_gCog = pfix.Cog;
    m_gSog = pfix.Sog;
    g_blinkTrigger++;
    if( m_pTable ){
        m_pTable->UpdateRouteData( m_activePointGuid, m_gLat, m_gLon, m_gCog, m_gSog );
    }
}

void navdata_pi::SetVP(PlugIn_ViewPort *vp)
{
    if( m_vp != vp ) {
        delete m_vp;
        m_vp = new PlugIn_ViewPort(*vp);
    }
}

bool navdata_pi::MouseEventHook( wxMouseEvent &event )
{
    if( g_activeRouteGuid.IsEmpty() )
        return false;
    if( !m_pTable )
        return false;
    if( event.Dragging() ){
        g_blinkTrigger = 0;
        return false;
    }

    if(IsTouchInterface_PlugIn()){
        if(event.LeftDown() || event.RightDown()) {
			m_pTable->m_pDataTable->m_stopLoopTimer.Start(TIMER_INTERVAL_MSECOND, wxTIMER_ONE_SHOT);
            return false;
        }
        if( !event.LeftUp() )
            return false;
    } else {
        if( !event.LeftDown() )
            return false;
    }
    if( !m_vp )  //this should never happens but ...
        return false;
    wxPoint p = event.GetPosition();
    double plat, plon;
    GetCanvasLLPix( m_vp, p, &plat, &plon);
    float selectRadius = GetSelectRadius();
    double dist_from_cursor = IDLE_STATE_NUMBER;
    /* walk along the route to find the selected wpt guid
     * way point visibility parameter unuseful here is use to
     * store if the selected is before or after the active point*/
    wxString SelGuid = wxEmptyString;
    bool past = false, pastactive = false;
    std::unique_ptr<PlugIn_Route> r;
    r = GetRoute_Plugin( g_activeRouteGuid );
    wxPlugin_WaypointListNode *node = r.get()->pWaypointList->GetFirst();
    while( node ){
        PlugIn_Waypoint *wpt = node->GetData();
        if(wpt) {
            wpt->m_IsVisible = past;
            if( wpt->m_GUID == m_activePointGuid )
                past = true;
            double of_lat = fabs(plat - wpt->m_lat);
            double of_lon = fabs(plon - wpt->m_lon);
            if( (of_lat < selectRadius) &&  (of_lon < selectRadius) ){
                //we must select the nearest point from the cursor/finger, not the first (nether the last)
                double dis = sqrt( pow(of_lat,2) + pow(of_lon,2) ) ;
                if( dis < dist_from_cursor ) {
                    SelGuid = wpt->m_GUID;
                    pastactive = wpt->m_IsVisible;
                    dist_from_cursor = dis;
                }
            }
        }
        node = node->GetNext();
    }
    if(SelGuid.IsEmpty())
        return false;
    if(g_selectedPointGuid != SelGuid){
        g_selectedPointGuid = SelGuid;
        m_pTable->SetTargetFlag( true );
        if( pastactive ){
            g_blinkTrigger = 1;
            RequestRefresh(GetNAVCanvas());
            m_pTable->UpdateRouteData( m_activePointGuid, m_gLat, m_gLon, m_gCog, m_gSog );
        }
        if(!m_ocpnOpenGL)
            pastactive = false;

        return pastactive;
    }
    return false;
}

float navdata_pi::GetSelectRadius()
{
    int w, h;
    ::wxDisplaySize(&w, &h);
    unsigned int radiusPixel = (w / PlugInGetDisplaySizeMM()) * m_selectionRadiusMM;
    double  canvasScaleFactor = wxMax( w, h) / (PlugInGetDisplaySizeMM() / 1000.);
    double trueScalePPM = canvasScaleFactor / m_vp->chart_scale;
    float selectRadius =  radiusPixel / (trueScalePPM * 1852 * 60);
    return selectRadius;
}

bool navdata_pi::RenderOverlayMultiCanvas( wxDC &dc, PlugIn_ViewPort *vp, int canvasIndex)
{
    // If multicanvas are active, render the overlay on the left canvas only
    if(GetCanvasCount() > 1 && canvasIndex != 0)           // multi?
        return false;
    if( g_activeRouteGuid.IsEmpty() )
        return false;
    if( !m_pTable )
        return false;

    SetVP( vp );

    if( g_selectedPointCol == wxNOT_FOUND )
        return false;

   wxDC *pdc = (&dc);      //inform render of non GL mode

    return RenderTargetPoint( pdc );

}

bool navdata_pi::RenderGLOverlayMultiCanvas( wxGLContext *pcontext, PlugIn_ViewPort *vp, int canvasIndex)
{
    // If multicanvas are active, render the overlay on the left canvas only
    if(GetCanvasCount() > 1 && canvasIndex != 0)           // multi?
        return false;
    if( g_activeRouteGuid.IsEmpty() )
        return false;
    if( !m_pTable )
        return false;

    SetVP( vp );

    if( g_selectedPointCol == wxNOT_FOUND )
        return false;

    return RenderTargetPoint( NULL );		 //NULL inform renderer of GL mode
}

bool navdata_pi::RenderTargetPoint( wxDC *pdc )  // Das ist der aktive Zu-Wegpunkt (blinkend)
{
    if( g_blinkTrigger & 1 ) {
        //way point position
        wxPoint2DDouble pp = m_pTable->GetSelPointPos();
        wxPoint r;
        GetCanvasPixLL( m_vp, &r, pp.m_x, pp.m_y );
        //icon image
        float scale =  GetOCPNChartScaleFactor_Plugin();
        wxImage image;
        wxBitmap bmp;
        int  w = _img_target->GetWidth() * scale;
        int  h = _img_target->GetHeight() * scale;
        int rx = r.x - w/2;
        int ry = r.y - h/2;
        wxString file = g_shareLocn + _T("target.svg");
        if( wxFile::Exists( file ) ){
            bmp = GetBitmapFromSVGFile( file, w, h);
        } else {
            wxImage im = _img_target->ConvertToImage();
            wxBitmap bmp = wxBitmap(im.Scale( w, h) );
        }
#ifdef __WXOSX__
        if(!bmp.Ok())
        return false;
#endif
        image = bmp.ConvertToImage();
        //control image
        unsigned char *d = image.GetData();
        if (d == 0)
            return false;
        //draw
        if( pdc ){                // no GL
//            pdc->DrawBitmap( image, rx, ry, true );	//	Don't work properly in this mode! Reaktiviert!
        }
#ifdef ocpnUSE_SVG
        else {                    // GL
            //create texture
            unsigned int IconTexture;
            glGenTextures(1, &IconTexture);
            glBindTexture(GL_TEXTURE_2D, IconTexture);

            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );

            unsigned char *a = image.GetAlpha();
#ifdef __WXOSX__
            unsigned char mr = '\0', mg = '\0', mb = '\0';
#else
            unsigned char mr, mg, mb;
#endif
            if(!a)
                image.GetOrFindMaskColour( &mr, &mg, &mb );
            unsigned char *e = new unsigned char[4 * w * h];
            for( int y = 0; y < h; y++ ) {
                for( int x = 0; x < w; x++ ) {
                    unsigned char r, g, b;
                    int off = ( y * w + x );
                    r = d[off * 3 + 0];
                    g = d[off * 3 + 1];
                    b = d[off * 3 + 2];
                    e[off * 4 + 0] = r;
                    e[off * 4 + 1] = g;
                    e[off * 4 + 2] = b;
                    e[off * 4 + 3] =  a ? a[off] : ( ( r == mr ) && ( g == mg ) && ( b == mb ) ? 0 : 255 );
                }
            }
            unsigned int glw = NextPow2(w);
            unsigned int glh = NextPow2(h);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, glw, glh,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h,
                    GL_RGBA, GL_UNSIGNED_BYTE, e);
            delete []e;
            //draw
            glBindTexture(GL_TEXTURE_2D, IconTexture);

            glEnable(GL_TEXTURE_2D);
            glEnable(GL_BLEND);
            glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glColor3f(1, 1, 1);

            float ws = w;
            float hs = h;
            float xs = rx;
            float ys = ry;
            float u = (float)w/glw, v = (float)h/glh;

            glBegin(GL_QUADS);
            glTexCoord2f(0, 0); glVertex2f(xs, ys);
            glTexCoord2f(u, 0); glVertex2f(xs+ws, ys);
            glTexCoord2f(u, v); glVertex2f(xs+ws, ys+hs);
            glTexCoord2f(0, v); glVertex2f(xs, ys+hs);
            glEnd();

            glDisable(GL_BLEND);
            glDisable(GL_TEXTURE_2D);
        }
#endif
    }

    return true;
}

void navdata_pi::OnTripLenghtTimer( wxTimerEvent & event)
{
    if( m_gTrkGuid == wxEmptyString ) return;
    wxJSONValue v;
    v[_T("Track_ID")] = m_gTrkGuid;
    wxJSONWriter w;
    wxString out;
    w.Write(v, out);
    SendPluginMessage( _T("OCPN_TRACK_REQUEST"), out );
}

void navdata_pi::OnRotateTimer( wxTimerEvent & event)
{
    int rotateTime, rotateTimeType;
    if( !GetOcpnDailyTrack( &rotateTime, &rotateTimeType ) ) return;

    /* find when the track rotation will happen by first starting timer for an hour, then
     * for a second until 10 s before rotation time then eventually start lenght
     * timer to complete computation before the current track was canceled by the rotation
     * process. this will allow to keep the whole trip data even if the rotation happened*/

    size_t nexRotate;
    time_t now = wxDateTime::Now().GetTicks();
    time_t today = wxDateTime::Today().GetTicks();
    int rotate_at = 0;
    switch( rotateTimeType )
    {
        case TIME_TYPE_LMT:
            rotate_at = rotateTime + wxRound(m_gLon * 3600. / 15.);
            break;
        case TIME_TYPE_COMPUTER:
            rotate_at = rotateTime;
            break;
        case TIME_TYPE_UTC:
#ifdef __WXOSX__
            int utc_offset = (int)wxDateTime::Now().GetTicks() - (int)wxDateTime::Now().ToUTC().GetTicks();
#else
            int utc_offset = wxDateTime::Now().GetTicks() - wxDateTime::Now().ToUTC().GetTicks();
#endif
            rotate_at = rotateTime + utc_offset;
            break;
    }
    if( rotate_at > 86400 )
        rotate_at -= 86400;
    else if (rotate_at < 0 )
        rotate_at += 86400;

    if( now - today > rotate_at)
        nexRotate = today + rotate_at + 86400;
    else
        nexRotate = today + rotate_at;

    time_t to_rotate = (nexRotate - now) * 1000;

    if( to_rotate >  TIMER_INTERVAL_HOUR + TIMER_INTERVAL_10SECOND )
        m_rotateTimer.Start( TIMER_INTERVAL_HOUR, wxTIMER_ONE_SHOT );
    else if( to_rotate > TIMER_INTERVAL_10SECOND )
        m_rotateTimer.Start( TIMER_INTERVAL_SECOND, wxTIMER_ONE_SHOT );
    else {
        m_gMustRotate = true;
        m_lenghtTimer.Start( TIMER_INTERVAL_MSECOND, wxTIMER_ONE_SHOT );
    }
}

double navdata_pi::GetMag(double a)
{
    if(!std::isnan(m_gWmmVar)) {
        if((a - m_gWmmVar) >360.)
            return (a - m_gWmmVar - 360.);
        else
            return ((a - m_gWmmVar) >= 0.) ? (a - m_gWmmVar) : (a - m_gWmmVar + 360.);
    }
    else {
        if((a - m_ocpnUserVar) >360.)
            return (a - m_ocpnUserVar - 360.);
        else
            return ((a - m_ocpnUserVar) >= 0.) ? (a - m_ocpnUserVar) : (a - m_ocpnUserVar + 360.);
    }
}

void navdata_pi::OnToolbarToolCallback(int id)
{
	if (m_pTable) {
        SetToolbarItemState(m_leftclick_tool_id, false);
		CloseDataTable();
	}
	else {
		SetToolbarItemState(m_leftclick_tool_id, true);

        LoadocpnConfig();
		long style = wxCAPTION | wxRESIZE_BORDER;
        m_pTable = new DataTable(GetNAVCanvas(), wxID_ANY, wxEmptyString, wxDefaultPosition,
			wxDefaultSize, style, this);
		DimeWindow(m_pTable); //apply colour scheme
		m_pTable->InitDataTable();
		m_pTable->UpdateRouteData(m_activePointGuid, m_gLat, m_gLon, m_gCog, m_gSog);
        m_pTable->UpdateTripData(m_ptripData);
		m_pTable->SetTableSizePosition(true);
		m_pTable->Show();
	}
}

bool navdata_pi::GetOcpnDailyTrack( int *roTime, int *rotimeType)
{
    bool isdailytrack;
    int rtime, rtimetype;
    wxFileConfig *pConf = GetOCPNConfigObject();
    if(pConf)
    {
        pConf->SetPath(_T("/Settings"));
        pConf->Read(_T("AutomaticDailyTracks"), &isdailytrack, 0);
        if( !isdailytrack ) return false;
        pConf->Read( _T ( "TrackRotateAt" ), &rtime, 0 );
        pConf->Read( _T ( "TrackRotateTimeType" ), &rtimetype, 1);

        if(roTime)
            *roTime = rtime;
        if(rotimeType)
            *rotimeType = rtimetype;

        return true;
    }
    return false;
}

void navdata_pi::CloseDataTable()
{
    SetToolbarItemState( m_leftclick_tool_id, false );
    if( m_pTable ) {
        m_pTable->CloseDialog();
        delete m_pTable;
        m_pTable = NULL;
    }
}

//-------------------------------------------------------------------------------------------------
//                  Trip Data Implementation
//-------------------------------------------------------------------------------------------------
/*
#include <wx/listimpl.cpp>
WX_DEFINE_LIST ( TripData );
*/
TripData::TripData()
{
    m_startDate = wxDateTime::Now();
    m_endTime = wxDateTime::Now();
    m_totalDist = 0.;
    m_tempDist = 0.;
    m_isEnded = false;
}

TripData::~TripData()
{}
