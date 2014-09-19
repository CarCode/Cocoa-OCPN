/***************************************************************************
 *
 * Project:  OpenCPN
 *
 ***************************************************************************
 *   Copyright (C) 2013 by David S. Register                               *
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

#include "wx/wxprec.h"

#include <wx/dcscreen.h>
#include <wx/tokenzr.h>

#include "routeman.h"
#include "chcanv.h"
#include "RoutePoint.h"
#include "multiplexer.h"
#include "navutil.h"
#include "FontMgr.h"
#include "cutil.h"

extern WayPointman *pWayPointMan;
extern bool g_bIsNewLayer;
extern int g_LayerIdx;
extern ChartCanvas *cc1;
extern Routeman *g_pRouteMan;
extern wxRect g_blink_rect;
extern Multiplexer *g_pMUX;
extern MyFrame *gFrame;
extern bool g_btouch;
extern bool g_bresponsive;

#include <wx/listimpl.cpp>
WX_DEFINE_LIST ( RoutePointList );

RoutePoint::RoutePoint()
{
    m_pbmIcon = NULL;

    //  Nice defaults
    m_seg_len = 0.0;
    m_seg_vmg = 0.0;
    m_seg_etd = wxInvalidDateTime;
    m_bDynamicName = false;
    m_bPtIsSelected = false;
    m_bIsBeingEdited = false;
    m_bIsActive = false;
    m_bBlink = false;
    m_bIsInRoute = false;
    m_bIsInTrack = false;
    m_CreateTimeX = wxDateTime::Now();
    m_GPXTrkSegNo = 1;
    m_bIsolatedMark = false;
    m_bShowName = true;
    m_bKeepXRoute = false;
    m_bIsVisible = true;
    m_bIsListed = true;
    CurrentRect_in_DC = wxRect( 0, 0, 0, 0 );
    m_NameLocationOffsetX = -10;
    m_NameLocationOffsetY = 8;
    m_pMarkFont = NULL;
    m_btemp = false;
    m_SelectNode = NULL;
    m_ManagerNode = NULL;
    
    m_HyperlinkList = new HyperlinkList;

    m_GUID = pWayPointMan->CreateGUID( this );

    m_IconName = wxEmptyString;
    ReLoadIcon();

    m_MarkName = wxEmptyString;

    m_bIsInLayer = false;
    m_LayerID = 0;
}

// Copy Constructor
RoutePoint::RoutePoint( RoutePoint* orig )
{
    m_MarkName = orig->GetName();
    m_lat = orig->m_lat;
    m_lon = orig->m_lon;
    m_seg_len = orig->m_seg_len;
    m_seg_vmg = orig->m_seg_vmg;
    m_seg_etd = orig->m_seg_etd;
    m_bDynamicName = orig->m_bDynamicName;
    m_bPtIsSelected = orig->m_bPtIsSelected;
    m_bIsBeingEdited = orig->m_bIsBeingEdited;
    m_bIsActive = orig->m_bIsActive;
    m_bBlink = orig->m_bBlink;
    m_bIsInRoute = orig->m_bIsInRoute;
    m_bIsInTrack = orig->m_bIsInTrack;
    m_CreateTimeX = orig->m_CreateTimeX;
    m_GPXTrkSegNo = orig->m_GPXTrkSegNo;
    m_bIsolatedMark = orig->m_bIsolatedMark;
    m_bShowName = orig->m_bShowName;
    m_bKeepXRoute = orig->m_bKeepXRoute;
    m_bIsVisible = orig->m_bIsVisible;
    m_bIsListed = orig->m_bIsListed;
    CurrentRect_in_DC = orig->CurrentRect_in_DC;
    m_NameLocationOffsetX = orig->m_NameLocationOffsetX;
    m_NameLocationOffsetY = orig->m_NameLocationOffsetY;
    m_pMarkFont = orig->m_pMarkFont;
    m_MarkDescription = orig->m_MarkDescription;
    m_btemp = orig->m_btemp;

    m_HyperlinkList = new HyperlinkList;
    m_IconName = orig->m_IconName;
    ReLoadIcon();

    m_bIsInLayer = orig->m_bIsInLayer;
    m_GUID = pWayPointMan->CreateGUID( this );
    
    m_SelectNode = NULL;
    m_ManagerNode = NULL;
    
}

RoutePoint::RoutePoint( double lat, double lon, const wxString& icon_ident, const wxString& name,
        const wxString &pGUID, bool bAddToList )
{
    //  Establish points
    m_lat = lat;
    m_lon = lon;

    //      Normalize the longitude, to fix any old poorly formed points
    if( m_lon < -180. ) m_lon += 360.;
    else
        if( m_lon > 180. ) m_lon -= 360.;

    //  Nice defaults
    m_seg_len = 0.0;
    m_seg_vmg = 0.0;
    m_seg_etd = wxInvalidDateTime;
    m_bDynamicName = false;
    m_bPtIsSelected = false;
    m_bIsBeingEdited = false;
    m_bIsActive = false;
    m_bBlink = false;
    m_bIsInRoute = false;
    m_bIsInTrack = false;
    m_CreateTimeX = wxDateTime::Now();
    m_GPXTrkSegNo = 1;
    m_bIsolatedMark = false;
    m_bShowName = true;
    m_bKeepXRoute = false;
    m_bIsVisible = true;
    m_bIsListed = true;
    CurrentRect_in_DC = wxRect( 0, 0, 0, 0 );
    m_NameLocationOffsetX = -10;
    m_NameLocationOffsetY = 8;
    m_pMarkFont = NULL;
    m_btemp = false;

    m_SelectNode = NULL;
    m_ManagerNode = NULL;
    
    m_HyperlinkList = new HyperlinkList;

    if( !pGUID.IsEmpty() )
        m_GUID = pGUID;
    else
        m_GUID = pWayPointMan->CreateGUID( this );

    //      Get Icon bitmap
    m_IconName = icon_ident;
    ReLoadIcon();

    SetName( name );

    //  Possibly add the waypoint to the global list maintained by the waypoint manager

    if( bAddToList && NULL != pWayPointMan )
        pWayPointMan->AddRoutePoint( this );

    m_bIsInLayer = g_bIsNewLayer;
    if( m_bIsInLayer ) {
        m_LayerID = g_LayerIdx;
        m_bIsListed = false;
    } else
        m_LayerID = 0;
}

RoutePoint::~RoutePoint( void )
{
//  Remove this point from the global waypoint list
    if( NULL != pWayPointMan )
        pWayPointMan->RemoveRoutePoint( this );

    if( m_HyperlinkList ) {
        m_HyperlinkList->DeleteContents( true );
        delete m_HyperlinkList;
    }
}

wxDateTime RoutePoint::GetCreateTime()
{
    if(!m_CreateTimeX.IsValid()) {
        if(m_timestring.Len())
            ParseGPXDateTime( m_CreateTimeX, m_timestring );
    }
    return m_CreateTimeX;
}

void RoutePoint::SetCreateTime( wxDateTime dt )
{
    m_CreateTimeX = dt;
}


void RoutePoint::SetName(const wxString & name)
{
    m_MarkName = name;
    CalculateNameExtents();
}

void RoutePoint::CalculateNameExtents( void )
{
    if( m_pMarkFont ) {
        wxScreenDC dc;

        dc.SetFont( *m_pMarkFont );
        m_NameExtents = dc.GetTextExtent( m_MarkName );
    } else
        m_NameExtents = wxSize( 0, 0 );

}

void RoutePoint::ReLoadIcon( void )
{
    m_pbmIcon = pWayPointMan->GetIconBitmap( m_IconName );

#ifdef ocpnUSE_GL
    m_wpBBox_chart_scale = -1;

    m_iTextTexture = 0;
#endif
}

void RoutePoint::Draw( ocpnDC& dc, wxPoint *rpn )
{
    wxPoint r;
    wxRect hilitebox;

    cc1->GetCanvasPointPix( m_lat, m_lon, &r );

    //  return the home point in this dc to allow "connect the dots"
    if( NULL != rpn ) *rpn = r;

    if( !m_bIsVisible /*&& !m_bIsInTrack*/)     // pjotrc 2010.02.13, 2011.02.24
        return;

    //    Optimization, especially apparent on tracks in normal cases
    if( m_IconName == _T("empty") && !m_bShowName && !m_bPtIsSelected ) return;

    wxPen *pen;
    if( m_bBlink ) pen = g_pRouteMan->GetActiveRoutePointPen();
    else
        pen = g_pRouteMan->GetRoutePointPen();

//    Substitue icon?
    wxBitmap *pbm;
    if( ( m_bIsActive ) && ( m_IconName != _T("mob") ) ) pbm = pWayPointMan->GetIconBitmap(
            _T ( "activepoint" ) );
    else
        pbm = m_pbmIcon;

    int sx2 = pbm->GetWidth() / 2;
    int sy2 = pbm->GetHeight() / 2;

//    Calculate the mark drawing extents
    wxRect r1( r.x - sx2, r.y - sy2, sx2 * 2, sy2 * 2 );           // the bitmap extents

    if( m_bShowName ) {
        if( 0 == m_pMarkFont ) {
            m_pMarkFont = FontMgr::Get().GetFont( _( "Marks" ) );
            m_FontColor = FontMgr::Get().GetFontColor( _( "Marks" ) );
            CalculateNameExtents();
        }

        if( m_pMarkFont ) {
            wxRect r2( r.x + m_NameLocationOffsetX, r.y + m_NameLocationOffsetY, m_NameExtents.x,
                    m_NameExtents.y );
            r1.Union( r2 );
        }
    }

    hilitebox = r1;
    hilitebox.x -= r.x;
    hilitebox.y -= r.y;
    float radius;
    if( g_btouch ){
        hilitebox.Inflate( 20 );
        radius = 20.0f;
    }
    else{
        hilitebox.Inflate( 4 );
        radius = 4.0f;
    }

    wxColour hi_colour = pen->GetColour();
    unsigned char transparency = 100;
    if( m_bIsBeingEdited ){
        hi_colour = GetGlobalColor( _T ( "YELO1" ) );
        transparency = 150;
    }
    
        
    //  Highlite any selected point
    if( m_bPtIsSelected || m_bIsBeingEdited) {
        AlphaBlending( dc, r.x + hilitebox.x, r.y + hilitebox.y, hilitebox.width, hilitebox.height, radius,
                hi_colour, transparency );
    }

    bool bDrawHL = false;

    if( m_bBlink && ( gFrame->nBlinkerTick & 1 ) ) bDrawHL = true;

    if( ( !bDrawHL ) && ( NULL != m_pbmIcon ) ) {
        dc.DrawBitmap( *pbm, r.x - sx2, r.y - sy2, true );
        // on MSW, the dc Bounding box is not updated on DrawBitmap() method.
        // Do it explicitely here for all platforms.
        dc.CalcBoundingBox( r.x - sx2, r.y - sy2 );
        dc.CalcBoundingBox( r.x + sx2, r.y + sy2 );
    }

    if( m_bShowName ) {
        if( m_pMarkFont ) {
            dc.SetFont( *m_pMarkFont );
            dc.SetTextForeground( m_FontColor );

            dc.DrawText( m_MarkName, r.x + m_NameLocationOffsetX, r.y + m_NameLocationOffsetY );
        }
    }

    //  Save the current draw rectangle in the current DC
    //    This will be useful for fast icon redraws
    CurrentRect_in_DC.x = r.x + hilitebox.x;
    CurrentRect_in_DC.y = r.y + hilitebox.y;
    CurrentRect_in_DC.width = hilitebox.width;
    CurrentRect_in_DC.height = hilitebox.height;

    if( m_bBlink ) g_blink_rect = CurrentRect_in_DC;               // also save for global blinker

}

#ifdef ocpnUSE_GL
void RoutePoint::DrawGL( ViewPort &vp, OCPNRegion &region )
{
    if( !m_bIsVisible )
    return;

    //    Optimization, especially apparent on tracks in normal cases
    if( m_IconName == _T("empty") && !m_bShowName && !m_bPtIsSelected ) return;

    if(m_wpBBox.GetValid() &&
       vp.chart_scale == m_wpBBox_chart_scale &&
       vp.rotation == m_wpBBox_rotation) {
        /* see if this waypoint can intersect with bounding box */
        LLBBox vpBBox = vp.GetBBox();
        if( vpBBox.IntersectOut( m_wpBBox ) ) {
            /* try with vp crossing IDL */
            if(vpBBox.GetMinX() < -180 && vpBBox.GetMaxX() > -180) {
                wxPoint2DDouble xlate( -360., 0. );
                wxBoundingBox test_box2 = m_wpBBox;
                test_box2.Translate( xlate );
                if( vp.GetBBox().IntersectOut( test_box2 ) )
                    return;
            } else 
            if(vpBBox.GetMinX() < 180 && vpBBox.GetMaxX() > 180) {
                wxPoint2DDouble xlate( 360., 0. );
                wxBoundingBox test_box2 = m_wpBBox;
                test_box2.Translate( xlate );
                if( vp.GetBBox().IntersectOut( test_box2 ) )
                    return;
            } else
                return;
        }
    }

    wxPoint r;
    wxRect hilitebox;
    unsigned char transparency = 150;

    cc1->GetCanvasPointPix( m_lat, m_lon, &r );

//    Substitue icon?
    wxBitmap *pbm;
    if( ( m_bIsActive ) && ( m_IconName != _T("mob") ) )
        pbm = pWayPointMan->GetIconBitmap(  _T ( "activepoint" ) );
    else
        pbm = m_pbmIcon;

    int sx2 = pbm->GetWidth() / 2;
    int sy2 = pbm->GetHeight() / 2;

//    Calculate the mark drawing extents
    wxRect r1( r.x - sx2, r.y - sy2, sx2 * 2, sy2 * 2 );           // the bitmap extents

    wxRect r3 = r1;
    if( m_bShowName ) {
        if( !m_pMarkFont ) {
            m_pMarkFont = FontMgr::Get().GetFont( _( "Marks" ) );
            m_FontColor = FontMgr::Get().GetFontColor( _( "Marks" ) );
            CalculateNameExtents();
        }

        if( m_pMarkFont ) {
            wxRect r2( r.x + m_NameLocationOffsetX, r.y + m_NameLocationOffsetY,
                       m_NameExtents.x, m_NameExtents.y );
            r3.Union( r2 );
        }
    }

    hilitebox = r3;
    hilitebox.x -= r.x;
    hilitebox.y -= r.y;
    float radius;
    if( g_btouch ){
        hilitebox.Inflate( 20 );
        radius = 20.0f;
    }
    else{
        hilitebox.Inflate( 4 );
        radius = 4.0f;
    }
    
    /* update bounding box */
    if(!m_wpBBox.GetValid() || vp.chart_scale != m_wpBBox_chart_scale || vp.rotation != m_wpBBox_rotation) {
        double lat1, lon1, lat2, lon2;
        cc1->GetCanvasPixPoint(r.x+hilitebox.x, r.y+hilitebox.y+hilitebox.height, lat1, lon1);
        cc1->GetCanvasPixPoint(r.x+hilitebox.x+hilitebox.width, r.y+hilitebox.y, lat2, lon2);

        m_wpBBox.SetMin(lon1, lat1);
        m_wpBBox.SetMax(lon2, lat2);
        m_wpBBox_chart_scale = vp.chart_scale;
        m_wpBBox_rotation = vp.rotation;
    }

    if(region.Contains(r3) == wxOutRegion)
        return;

    ocpnDC dc;

    //  Highlite any selected point
    if( m_bPtIsSelected ) {
        wxColour hi_colour;
        if( m_bBlink ){
            wxPen *pen = g_pRouteMan->GetActiveRoutePointPen();
            hi_colour = pen->GetColour();
        }
        else{
            hi_colour = GetGlobalColor( _T ( "YELO1" ) );
        }
        
        AlphaBlending( dc, r.x + hilitebox.x, r.y + hilitebox.y, hilitebox.width, hilitebox.height, radius,
                       hi_colour, transparency );
    }
    
    bool bDrawHL = false;

    if( m_bBlink && ( gFrame->nBlinkerTick & 1 ) ) bDrawHL = true;

    if( ( !bDrawHL ) && ( NULL != m_pbmIcon ) ) {
        int glw, glh;
        unsigned int IconTexture = pWayPointMan->GetIconTexture( pbm, glw, glh );
        
        glBindTexture(GL_TEXTURE_2D, IconTexture);
        
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
        
        glColor3f(1, 1, 1);
        
        int x = r1.x, y = r1.y, w = r1.width, h = r1.height;
        float u = (float)w/glw, v = (float)h/glh;
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex2f(x, y);
        glTexCoord2f(u, 0); glVertex2f(x+w, y);
        glTexCoord2f(u, v); glVertex2f(x+w, y+h);
        glTexCoord2f(0, v); glVertex2f(x, y+h);
        glEnd();
        glDisable(GL_BLEND);
        glDisable(GL_TEXTURE_2D);
    }

    if( m_bShowName && m_pMarkFont ) {
        int w = m_NameExtents.x, h = m_NameExtents.y;
        if(!m_iTextTexture && w && h) {
            wxBitmap tbm(w, h); /* render text on dc */
            wxMemoryDC dc;
            dc.SelectObject( tbm );               
            dc.SetBackground( wxBrush( *wxBLACK ) );
            dc.Clear();
            dc.SetFont( *m_pMarkFont );
            dc.SetTextForeground( *wxWHITE );
            dc.DrawText( m_MarkName, 0, 0);
            dc.SelectObject( wxNullBitmap );
            
            /* make alpha texture for text */
            wxImage image = tbm.ConvertToImage();
            unsigned char *d = image.GetData();
            unsigned char *e = new unsigned char[w * h];
            for( int p = 0; p < w*h; p++)
                e[p] = d[3*p + 0];
            
            /* create texture for rendered text */
            glGenTextures(1, &m_iTextTexture);
            glBindTexture(GL_TEXTURE_2D, m_iTextTexture);
            
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

            m_iTextTextureWidth = NextPow2(w);
            m_iTextTextureHeight = NextPow2(h);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, m_iTextTextureWidth, m_iTextTextureHeight,
                         0, GL_ALPHA, GL_UNSIGNED_BYTE, NULL);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h,
                            GL_ALPHA, GL_UNSIGNED_BYTE, e);
            delete [] e;
        }

        if(m_iTextTexture) {
            /* draw texture with text */
            glBindTexture(GL_TEXTURE_2D, m_iTextTexture);
            
            glEnable(GL_TEXTURE_2D);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
            glColor3ub(m_FontColor.Red(), m_FontColor.Green(), m_FontColor.Blue());
            
            int x = r.x + m_NameLocationOffsetX, y = r.y + m_NameLocationOffsetY;
            float u = (float)w/m_iTextTextureWidth, v = (float)h/m_iTextTextureHeight;
            glBegin(GL_QUADS);
            glTexCoord2f(0, 0); glVertex2f(x, y);
            glTexCoord2f(u, 0); glVertex2f(x+w, y);
            glTexCoord2f(u, v); glVertex2f(x+w, y+h);
            glTexCoord2f(0, v); glVertex2f(x, y+h);
            glEnd();
            glDisable(GL_BLEND);
            glDisable(GL_TEXTURE_2D);
        }
    }
    
    if( m_bBlink ) g_blink_rect = CurrentRect_in_DC;               // also save for global blinker
    
    //    This will be useful for fast icon redraws
    CurrentRect_in_DC.x = r.x + hilitebox.x;
    CurrentRect_in_DC.y = r.y + hilitebox.y;
    CurrentRect_in_DC.width = hilitebox.width;
    CurrentRect_in_DC.height = hilitebox.height;

    if( m_bBlink ) g_blink_rect = CurrentRect_in_DC;               // also save for global blinker
}
#endif

void RoutePoint::SetPosition( double lat, double lon )
{
    m_lat = lat;
    m_lon = lon;
}

void RoutePoint::CalculateDCRect( wxDC& dc, wxRect *prect )
{
    dc.ResetBoundingBox();
    dc.DestroyClippingRegion();

    // Draw the mark on the dc
    ocpnDC odc( dc );
    Draw( odc, NULL );

    //  Retrieve the drawing extents
    prect->x = dc.MinX() - 1;
    prect->y = dc.MinY() - 1;
    prect->width = dc.MaxX() - dc.MinX() + 2; // Mouse Poop?
    prect->height = dc.MaxY() - dc.MinY() + 2;

}

bool RoutePoint::IsSame( RoutePoint *pOtherRP )
{
    bool IsSame = false;

    if( this->m_MarkName == pOtherRP->m_MarkName ) {
        if( fabs( this->m_lat - pOtherRP->m_lat ) < 1.e-6
                && fabs( this->m_lon - pOtherRP->m_lon ) < 1.e-6 ) IsSame = true;
    }
    return IsSame;
}

bool RoutePoint::SendToGPS(const wxString & com_name, wxGauge *pProgress)
{
    int result = 0;
    if( g_pMUX )
        result = g_pMUX->SendWaypointToGPS( this, com_name, pProgress );

    wxString msg;
    if( 0 == result )
        msg = _("Waypoint(s) Transmitted.");
    else{
        if( result == ERR_GARMIN_INITIALIZE )
            msg = _("Error on Waypoint Upload.  Garmin GPS not connected");
        else               
            msg = _("Error on Waypoint Upload.  Please check logfiles...");
    }

    OCPNMessageBox( NULL, msg, _("OpenCPN Info"), wxOK | wxICON_INFORMATION );

    return (result == 0);
}

