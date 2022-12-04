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
wxString       g_activeRouteGuid;
wxString       g_activePointGuid;
wxString       g_selectedPointGuid;
double         g_Lat;
double         g_Lon;
double         g_Cog;
double         g_Sog;
int            g_ocpnDistFormat;
int            g_ocpnSpeedFormat;
wxColour       g_labelColour;
wxColour       g_valueColour;
wxFont         g_labelFont;
wxFont         g_valueFont;

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
    if(_img_active)
    delete _img_active;
    if( _img_toggled)
    delete _img_toggled;
    if(_img_target)
    delete _img_target;
    if(m_vp[0])
    delete m_vp[0];
    if(m_vp[1])
    delete m_vp[1];
 }

int navdata_pi::Init(void){

    AddLocaleCatalog( _T("opencpn-navdata_pi") );

    m_isPluginActive = false;
    m_console = NULL;
    m_selectablePoint = false;
    g_selectedPointGuid = wxEmptyString;
    g_activeRouteGuid = wxEmptyString;
    m_blinkTrigger = 0;
    //allow multi-canvas
    m_vp[0] = new PlugIn_ViewPort;
    if(GetCanvasCount() > 1)
        m_vp[1] = new PlugIn_ViewPort;
    else
        m_vp[1] = NULL;

    //to do: get it from style
    m_defLabelColor.Set( 50, 240, 50);

    //find and store share path
#ifdef __WXOSX__
    m_shareLocn =*GetpPrivateApplicationDataLocation() + wxFileName::GetPathSeparator() +
#else
    m_shareLocn = *GetpSharedDataLocation() +
#endif
                    _T("plugins") + wxFileName::GetPathSeparator() +
                    _T("navdata_pi") + wxFileName::GetPathSeparator()
                    + _T("data") + wxFileName::GetPathSeparator();

    //    This PlugIn needs a toolbar icon, so request its insertion
    wxString active = m_shareLocn + _T("active.svg");
    wxString toggled = m_shareLocn + _T("toggled.svg");
    if( wxFile::Exists( active) && wxFile::Exists( toggled) )
        m_leftclick_tool_id  = InsertPlugInToolSVG(_T(""), active, active, toggled,
                    wxITEM_CHECK, _("Navdata: Unlock/Lock Point Selection"), _T(""), NULL, NAVDATA_TOOL_POSITION, 0, this);
    else
        m_leftclick_tool_id  = InsertPlugInTool(_T(""), _img_active, _img_toggled,
                    wxITEM_CHECK, _("Navdata: Unlock/Lock Point Selection"), _T(""), NULL, NAVDATA_TOOL_POSITION, 0, this);

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
    if( m_console ){
        delete m_console;
        m_console = NULL;
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
      return _("Navigation data plugin for OpenCPN\nShows RNG (range),TTG (time to go) and ETA (estimated time of arrival)\nto an intermediate route point of the active route.");
}

int navdata_pi::GetToolbarToolCount(void)
{
      return 1;
}

void navdata_pi::SetColorScheme(PI_ColorScheme cs)
{

//    if(m_console)
//        m_console->SetColorScheme();
}

void navdata_pi::SetPluginMessage(wxString &message_id, wxString &message_body)
{
    if(message_id == _T("OCPN_RTE_ACTIVATED"))
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

    else if(message_id == _T("OCPN_WPT_ACTIVATED"))
    {
        wxJSONValue  p1root;
        wxJSONReader p1reader;
        int pnumErrors = p1reader.Parse( message_body, &p1root );
        if ( pnumErrors == 0 ){
            g_activePointGuid = p1root[_T("GUID")].AsString();
            if( m_console && m_console->IsShown() )
                CheckRoutePointSelectable();
        }
    }

    else if(message_id == _T("OCPN_WPT_ARRIVED"))
    {
        wxJSONValue  p2root;
        wxJSONReader p2reader;
        int p2numErrors = p2reader.Parse( message_body, &p2root );
        if ( p2numErrors == 0 ) {
            if( p2root.HasMember(_T("Next_WP"))){
                g_activePointGuid = p2root[_T("GUID")].AsString();
                if( m_console && m_console->IsShown() )
                    CheckRoutePointSelectable();
            }
        }
    }

    else if(message_id == _T("OCPN_RTE_DEACTIVATED") || message_id == _T("OCPN_RTE_ENDED") )
    {
        m_selectablePoint = false;
        g_selectedPointGuid = wxEmptyString;
        g_activePointGuid = wxEmptyString;
        g_activeRouteGuid = wxEmptyString;
        if( m_console ){
            delete m_console;
            m_console = NULL;
        }
    }
    else if(message_id == _T("OpenCPN Config"))
    {
        LoadocpnConfig();
    }
}

void navdata_pi::LoadocpnConfig()
{
    wxFileConfig *pConf = GetOCPNConfigObject();
    if(pConf)
    {
        float radiusPixel;
        pConf->SetPath(_T("/Settings"));
        pConf->Read(_T("DistanceFormat"), &g_ocpnDistFormat, 0);
        pConf->Read(_T("SpeedFormat"), &g_ocpnSpeedFormat, 0);
        if(IsTouchInterface_PlugIn()){
            pConf->Read( _T ("SelectionRadiusTouchMM"), &radiusPixel);
            m_ocpnSelRadiusMM = wxMax(radiusPixel, 1.0);
        } else {
            pConf->Read( _T ("SelectionRadiusMM"), &radiusPixel);
            m_ocpnSelRadiusMM = wxMax(radiusPixel, 0.5);
        }
    }

    bool changeFont = false;
    wxFont  lfont = *OCPNGetFont( _("Console Legend"), 0);
    if( g_labelFont != lfont ) {
        changeFont = true;
        g_labelFont = lfont;
    }
    wxFont  vfont = *OCPNGetFont(_("Console Value"), 0);
    if( g_valueFont != vfont ){
        changeFont = true;
        g_valueFont = vfont;
    }
    wxColour back_color;
    GetGlobalColor(_T("UBLCK"), &back_color);
    wxColour lcol = GetFontColour_PlugIn( _("Console Legend") );
    if( g_labelColour != lcol ) {
        wxColor ncol = lcol;
        if( (abs(ncol.Red() - back_color.Red()) < 5) &&
                    (abs(ncol.Green() - back_color.Blue()) < 5) &&
                    (abs(ncol.Blue() - back_color.Blue()) < 5)) {
            if(g_labelColour != m_defLabelColor) {
                g_labelColour = m_defLabelColor;
            }
        } else
            g_labelColour = lcol;
    }
    wxColour vcol = GetFontColour_PlugIn( _("Console Value") );
    if( g_valueColour != vcol ) {
        wxColor mcol = vcol;
        if( (abs(mcol.Red() - back_color.Red()) < 5) &&
                (abs(mcol.Green() - back_color.Blue()) < 5) &&
                (abs(mcol.Blue() - back_color.Blue()) < 5)) {
            if(g_valueColour != m_defLabelColor) {
                g_valueColour = m_defLabelColor;
            }
        } else
            g_valueColour = vcol;
    }

//    if(changeFont){
//        if(m_console) m_console->UpdateFonts();
//    }

    return;
}

void navdata_pi::CheckRoutePointSelectable()
{
    //check if the selected point is still after the new active point
    if( !m_selectablePoint ) return;
    bool past = false, selectable;
    std::unique_ptr<PlugIn_Route> r;
    r = GetRoute_Plugin( g_activeRouteGuid );
    wxPlugin_WaypointListNode *node = r.get()->pWaypointList->GetFirst();
    while( node ){
        PlugIn_Waypoint *wpt = node->GetData();
        if(wpt) {
            selectable = past;
            if( wpt->m_GUID == g_activePointGuid )
                past = true;
            if( wpt->m_GUID == g_selectedPointGuid ){
                if( !selectable ){
                    m_console->Show(false);
                    g_selectedPointGuid = wxEmptyString;
                    m_selectablePoint = false;
                }
                break;
            }
        }
        node = node->GetNext();
    }

}

void navdata_pi::SetPositionFix(PlugIn_Position_Fix &pfix)
{
    static int new_canvas_nbr = GetCanvasCount();
    g_Lat = pfix.Lat;
    g_Lon = pfix.Lon;
    g_Cog = pfix.Cog;
    g_Sog = pfix.Sog;
    m_blinkTrigger++;
    //when the canvas number has changed, do nothing, this could create a crash
    if(GetCanvasCount() == new_canvas_nbr ){
        if( (m_console && m_console->IsShown()))
            m_console->UpdateRouteData();
    } else {
        new_canvas_nbr = GetCanvasCount();
        delete m_vp[1];
        if(new_canvas_nbr > 1){ //initialise view port for right canvas
            m_vp[1] = new PlugIn_ViewPort;
            GetCanvasByIndex(1)->Refresh();
        } else                  //close vp for right canvas
            m_vp[1] = NULL;
    }
}

bool navdata_pi::MouseEventHook( wxMouseEvent &event )
{
    if( !m_isPluginActive )
        return false;
    if( g_activeRouteGuid.IsEmpty() )
        return false;
    if( event.Dragging() ){
        m_blinkTrigger = 0;
        return false;
    }
    if(IsTouchInterface_PlugIn()){
        if( !event.LeftUp() )
            return false;
    } else {
        if( !event.LeftDown() )
            return false;
    }
    if( !m_vp[GetCanvasIndexUnderMouse()] )
        return false;

    wxPoint p = event.GetPosition();
    double plat, plon;
    GetCanvasLLPix( m_vp[GetCanvasIndexUnderMouse()], p, &plat, &plon);
    float selectRadius = GetSelectRadius( m_vp[GetCanvasIndexUnderMouse()] );
    double dist_from_cursor = IDLE_STATE_NUMBER;
    /*walk along the route to find the nearest point guid from the cursor
     * way point visibility parameter unuseful here is use to
     * store if the selected is before or after the active point*/
    wxString SelGuid = wxEmptyString;
    wxString pointLabel;
    bool past = false;
    std::unique_ptr<PlugIn_Route> r;
    r = GetRoute_Plugin( g_activeRouteGuid );
    wxPlugin_WaypointListNode *node = r.get()->pWaypointList->GetFirst();
    while( node ){
        PlugIn_Waypoint *wpt = node->GetData();
        if(wpt) {
            wpt->m_IsVisible = past;
            if( wpt->m_GUID == g_activePointGuid )
                past = true;
            double of_lat = fabs(plat - wpt->m_lat);
            double of_lon = fabs(plon - wpt->m_lon);
            if( (of_lat < selectRadius) &&  (of_lon < selectRadius) ){
                double dis = sqrt( pow(of_lat,2) + pow(of_lon,2) ) ;
                if( dis < dist_from_cursor ) {
                    SelGuid = wpt->m_GUID;
                    pointLabel = wpt->m_MarkName;
                    m_selectablePoint = wpt->m_IsVisible;
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
        if( m_selectablePoint ){
            m_blinkTrigger = 1;
            for( int i = 0; i < GetCanvasCount(); i++ ){
                GetCanvasByIndex(i)->Refresh();
            }
            if( !m_console )
                m_console = new RouteCanvas( GetOCPNCanvasWindow(), this);
            m_console->m_pointName =  pointLabel;
//            m_console->ShowWithFreshFonts();
//            m_console->SetColorScheme();
        } else {
            if( m_console )
                m_console->Show(false);
        }
        return m_selectablePoint;
    }
    return false;
}

float navdata_pi::GetSelectRadius(PlugIn_ViewPort *vp)
{
    int w, h;
    ::wxDisplaySize(&w, &h);
    unsigned int radiusPixel = (w / PlugInGetDisplaySizeMM()) * m_ocpnSelRadiusMM;
    double  canvasScaleFactor = wxMax( w, h) / (PlugInGetDisplaySizeMM() / 1000.);
    double trueScalePPM = canvasScaleFactor / vp->chart_scale;
    float selectRadius =  radiusPixel / (trueScalePPM * 1852 * 60);
    return selectRadius;
}

bool navdata_pi::RenderOverlayMultiCanvas( wxDC &dc, PlugIn_ViewPort *vp, int canvasIndex)
{
    if(m_vp[canvasIndex])
        *m_vp[canvasIndex] = *vp;

    if(!m_selectablePoint)
        return false;

    wxDC *pdc = (&dc);      //inform render of non GL mode

    return RenderTargetPoint( pdc , vp);

}

bool navdata_pi::RenderGLOverlayMultiCanvas( wxGLContext *pcontext, PlugIn_ViewPort *vp, int canvasIndex)
{
    if(m_vp[canvasIndex])
        *m_vp[canvasIndex] = *vp;

    if(!m_selectablePoint)
        return false;

    return RenderTargetPoint( NULL , vp );		 //NULL inform renderer of GL mode
}

bool navdata_pi::RenderTargetPoint( wxDC *pdc, PlugIn_ViewPort *vp )
{
    if( !vp ) return false;
    if( m_blinkTrigger & 1 || pdc ) {
        //way point position
        std::unique_ptr<PlugIn_Waypoint> rp;
        rp = GetWaypoint_Plugin(g_selectedPointGuid);
        if( !rp ) return false;
        wxPoint p;
        GetCanvasPixLL( vp, &p, rp.get()->m_lat, rp.get()->m_lon );
        //icon image
        float scale =  GetOCPNChartScaleFactor_Plugin();
        wxImage image;
        wxBitmap bmp;
        int  w = _img_target->GetWidth() * scale;
        int  h = _img_target->GetHeight() * scale;
        int px = p.x - w/2;
        int py = p.y - h/2;
        wxString file = m_shareLocn + _T("target.svg");
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
            pdc->DrawBitmap( image, px, py );		//Don't work properly in this mode! (no blinking)
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
            float xs = px;
            float ys = py;
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

void navdata_pi::OnToolbarToolCallback(int id)
{
    m_isPluginActive = !m_isPluginActive;
    SetToolbarItemState(m_leftclick_tool_id, m_isPluginActive);
}
