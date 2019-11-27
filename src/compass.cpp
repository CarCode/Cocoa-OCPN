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
 ***************************************************************************/

#include "wx/wxprec.h"
#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers
// #include "config.h" // NEU
#include "ocpn_types.h"
#include "compass.h"
#include "chcanv.h"
#include "styles.h"

#include "dychart.h"


extern ocpnStyle::StyleManager* g_StyleManager;
extern ChartCanvas *cc1;  // ALT
extern bool bGPSValid;
extern bool g_bSatValid;
extern int g_SatsInView;
extern bool g_bCourseUp;  // ALT
extern MyFrame *gFrame;
extern bool g_bopengl;

ocpnCompass::ocpnCompass()
{
//    m_parent = parent;  // NEU
//    m_bshowGPS = bShowGPS;  // NEU

     ocpnStyle::Style* style = g_StyleManager->GetCurrentStyle();
     _img_compass = style->GetIcon( _T("CompassRose") );
     _img_gpsRed = style->GetIcon( _T("gpsRed") );

    m_rose_angle = -999;  // force a refresh when first used

    m_pStatBoxToolStaticBmp = NULL;

     m_rect = wxRect(style->GetCompassXOffset(), style->GetCompassYOffset(),
             _img_compass.GetWidth() + _img_gpsRed.GetWidth() + style->GetCompassLeftMargin() * 2
                     + style->GetToolSeparation(),
                     _img_compass.GetHeight() + style->GetCompassTopMargin() + style->GetCompassBottomMargin() );

#ifdef ocpnUSE_GL
    texobj = 0;
#endif

    m_scale = 1.0;  // NEU
//    m_cs = GLOBAL_COLOR_SCHEME_RGB;  // NEU
}

ocpnCompass::~ocpnCompass()
{
#ifdef ocpnUSE_GL
    if(texobj){
        glDeleteTextures(1, &texobj);
        texobj = 0;
    }
#endif

    delete m_pStatBoxToolStaticBmp;
}

void ocpnCompass::Paint( ocpnDC& dc )
{
#ifdef __WXOSX__
    if(!m_StatBmp.IsOk())
        CreateBmp(true);
//            m_StatBmp.Create( width, compassBg.GetHeight() + topmargin + style->GetCompassBottomMargin() );
#endif
    if(m_shown && m_StatBmp.IsOk()){ // m_StatBmp.IsOk() ist NICHT OK, warum???? siehe oben
#if defined(ocpnUSE_GLES) || defined(ocpnUSE_GL)  // GLES does not do ocpnDC::DrawBitmap(), so use texture
#ifdef __WXOSX__
/*      if(!texobj)
          wxMessageBox("kein texobj","Fehler", wxOK | wxICON_INFORMATION); */
#endif
        if(g_bopengl && texobj){  // texobj nicht da, warum????
            glBindTexture( GL_TEXTURE_2D, texobj );
            glEnable( GL_TEXTURE_2D );

            glBegin( GL_QUADS );

            glTexCoord2f( 0, 0 );  glVertex2i( m_rect.x, m_rect.y ); // links oben
            glTexCoord2f( 1, 0 );  glVertex2i( m_rect.x + m_rect.width, m_rect.y );  // rechts oben
            glTexCoord2f( 1, 1 );  glVertex2i( m_rect.x + m_rect.width, m_rect.y + m_rect.height );  // rechts unten
            glTexCoord2f( 0, 1 );  glVertex2i( m_rect.x, m_rect.y + m_rect.height );  // links unten

            glEnd();
            glDisable( GL_TEXTURE_2D );

        }
        else {
         dc.DrawBitmap( m_StatBmp, m_rect.x, m_rect.y, true );  // true
        }

#else
    dc.DrawBitmap( m_StatBmp, m_rect.x, m_rect.y, true );  // true
#endif        
    }

}

bool ocpnCompass::MouseEvent( wxMouseEvent& event )
{
    if(!m_shown || !m_rect.Contains(event.GetPosition()))
        return false;

    if (event.LeftDown())
        gFrame->ToggleCourseUp();  // ALT
//        m_parent->ToggleCourseUp( );  // NEU
    return true;
}

void ocpnCompass::SetColorScheme( ColorScheme cs )
{
    UpdateStatus( true );
//        m_cs = cs;  //  NEU
}

void ocpnCompass::UpdateStatus( bool bnew )
{
    if( bnew ){
        m_lastgpsIconName.Clear();        // force an update to occur

        //  We clear the texture so that any onPaint method will not use a stale texture
//#ifdef __WXOSX__
//#if defined ocpnUSE_GLES  || defined (ocpnUSE_GL)
//#else
#ifdef ocpnUSE_GLES
//#endif
        if(g_bopengl){
             if(texobj){
                glDeleteTextures(1, &texobj);
                texobj = 0;
             }
        }
#endif
    }


    CreateBmp( bnew );
}

void ocpnCompass::SetScaleFactor( float factor)
{
    ocpnStyle::Style* style = g_StyleManager->GetCurrentStyle();

    if(factor > 0.1)
        m_scale = factor;
    else
        m_scale = 1.0;

    //  Precalculate the background sizes to get m_rect width/height
    wxBitmap compassBg, gpsBg;
//    int orient = style->GetOrientation();  // Not used
    style->SetOrientation( wxTB_HORIZONTAL );
    if( style->HasBackground() ) {
        compassBg = style->GetNormalBG();
        style->DrawToolbarLineStart( compassBg );
        compassBg = style->SetBitmapBrightness( compassBg );
        gpsBg = style->GetNormalBG();
        style->DrawToolbarLineEnd( gpsBg );
        gpsBg = style->SetBitmapBrightness( gpsBg );
    }

    if(fabs(m_scale-1.0) > 0.1){
        wxImage bg_img = compassBg.ConvertToImage();
        bg_img.Rescale(compassBg.GetWidth() * m_scale, compassBg.GetHeight() *m_scale, wxIMAGE_QUALITY_NORMAL);
        compassBg = wxBitmap( bg_img );

        bg_img = gpsBg.ConvertToImage();
        bg_img.Rescale(gpsBg.GetWidth() * m_scale, gpsBg.GetHeight() *m_scale, wxIMAGE_QUALITY_NORMAL);
        gpsBg = wxBitmap( bg_img );
     }

     int width = compassBg.GetWidth() + gpsBg.GetWidth() + style->GetCompassLeftMargin();
     if( !style->marginsInvisible ) 
         width += style->GetCompassLeftMargin() + style->GetToolSeparation();

     m_rect = wxRect(style->GetCompassXOffset(), style->GetCompassYOffset(),
                    width,
                     compassBg.GetHeight() + style->GetCompassTopMargin() + style->GetCompassBottomMargin());

}


void ocpnCompass::CreateBmp( bool newColorScheme )
{
    if(!m_shown)
        return;

    wxString gpsIconName;
    ocpnStyle::Style* style = g_StyleManager->GetCurrentStyle();

    // In order to draw a horizontal compass window when the toolbar is vertical, we
    // need to save away the sizes and backgrounds for the two icons.

    static wxBitmap compassBg, gpsBg;
    static wxSize toolsize;
    static int topmargin, leftmargin, radius;

    if( ! compassBg.IsOk() || newColorScheme ) {
        int orient = style->GetOrientation();
        style->SetOrientation( wxTB_HORIZONTAL );
        if( style->HasBackground() ) {
            compassBg = style->GetNormalBG();
            style->DrawToolbarLineStart( compassBg );
            compassBg = style->SetBitmapBrightness( compassBg);
//            compassBg = style->SetBitmapBrightness( compassBg, m_cs );  //  NEU
            gpsBg = style->GetNormalBG();
            style->DrawToolbarLineEnd( gpsBg );
            gpsBg = style->SetBitmapBrightness( gpsBg);
//            gpsBg = style->SetBitmapBrightness( gpsBg, m_cs );  // NEU
        }

        if(fabs(m_scale-1.0) > 0.1){
            wxImage bg_img = compassBg.ConvertToImage();
            bg_img.Rescale(compassBg.GetWidth() * m_scale, compassBg.GetHeight() *m_scale, wxIMAGE_QUALITY_NORMAL);
            compassBg = wxBitmap( bg_img );

            bg_img = gpsBg.ConvertToImage();
            bg_img.Rescale(gpsBg.GetWidth() * m_scale, gpsBg.GetHeight() *m_scale, wxIMAGE_QUALITY_NORMAL);
            gpsBg = wxBitmap( bg_img );
        }

        leftmargin = style->GetCompassLeftMargin();
        topmargin = style->GetCompassTopMargin();
        radius = style->GetCompassCornerRadius();

        if( orient /*== wxTB_VERTICAL*/ ) style->SetOrientation( wxTB_VERTICAL ); // NEU ab ==
    }

    bool b_need_refresh = false;

    if( bGPSValid ) {
        if( g_bSatValid ) {
            gpsIconName = _T("gps3Bar");
            if( g_SatsInView <= 8 ) gpsIconName = _T("gps2Bar");
            if( g_SatsInView <= 4 ) gpsIconName = _T("gps1Bar");
            if( g_SatsInView < 0 ) gpsIconName = _T("gpsGry");

        } else
            gpsIconName = _T("gpsGrn");
    } else
        gpsIconName = _T("gpsRed");

    if( m_lastgpsIconName != gpsIconName ) b_need_refresh = true;

    double rose_angle = -999.;

    if( ( fabs( cc1->GetVPRotation() ) > .01 ) || ( fabs( cc1->GetVPSkew() ) > .01 ) ) {  // ALT
        rose_angle = -cc1->GetVPRotation();  // ALT
//    if( ( fabs( m_parent->GetVPRotation() ) > .01 ) || ( fabs( m_parent->GetVPSkew() ) > .01 ) ) {  // NEU
//    rose_angle = -m_parent->GetVPRotation();  // NEU
    } else
        rose_angle = 0.;

    if( fabs( m_rose_angle - rose_angle ) > .1 ) b_need_refresh = true;

    if( !b_need_refresh )
        return;

    int width = compassBg.GetWidth() + gpsBg.GetWidth() + leftmargin;  // ALT
//    int width = compassBg.GetWidth();  // NEU
//    if(m_bshowGPS)  // NEU
//        width += gpsBg.GetWidth() + leftmargin;  // NEU

    if( !style->marginsInvisible ) 
        width += leftmargin + style->GetToolSeparation();

    m_StatBmp.Create( width, compassBg.GetHeight() + topmargin + style->GetCompassBottomMargin() );

    m_rect.width = m_StatBmp.GetWidth();
    m_rect.height = m_StatBmp.GetHeight();

    if( !m_StatBmp.IsOk() )
        return;

    m_MaskBmp = wxBitmap( m_StatBmp.GetWidth(), m_StatBmp.GetHeight() );
    if( style->marginsInvisible ) {
        wxMemoryDC sdc( m_MaskBmp );
        sdc.SetBackground( *wxWHITE_BRUSH );
        sdc.Clear();
        sdc.SetBrush( *wxBLACK_BRUSH );
        sdc.SetPen( *wxBLACK_PEN );
        wxSize maskSize = wxSize(m_MaskBmp.GetWidth() - leftmargin,
                                 m_MaskBmp.GetHeight() - (2 * topmargin));
        sdc.DrawRoundedRectangle( wxPoint( leftmargin, topmargin ), maskSize, radius );
        sdc.SelectObject( wxNullBitmap );
    } else if(radius) {
        wxMemoryDC sdc( m_MaskBmp );
        sdc.SetBackground( *wxWHITE_BRUSH );
        sdc.Clear();
        sdc.SetBrush( *wxBLACK_BRUSH );
        sdc.SetPen( *wxBLACK_PEN );
        sdc.DrawRoundedRectangle( 0, 0, m_MaskBmp.GetWidth(), m_MaskBmp.GetHeight(), radius );
        sdc.SelectObject( wxNullBitmap );
    }
    m_StatBmp.SetMask(new wxMask(m_MaskBmp, *wxWHITE));

    wxMemoryDC mdc;
    mdc.SelectObject( m_StatBmp );
    mdc.SetBackground( wxBrush( GetGlobalColor( _T("COMP1") ), wxBRUSHSTYLE_SOLID ) );
    mdc.Clear();

    mdc.SetPen( wxPen( GetGlobalColor( _T("UITX1") ), 1 ) );
    mdc.SetBrush( wxBrush( GetGlobalColor( _T("UITX1") ), wxBRUSHSTYLE_TRANSPARENT ) );

    if( !style->marginsInvisible )
        mdc.DrawRoundedRectangle( 0, 0, m_StatBmp.GetWidth(), m_StatBmp.GetHeight(),radius );

    wxPoint offset(leftmargin, topmargin);

    //    Build Compass Rose, rotated...
    wxBitmap BMPRose;
    wxPoint after_rotate;

    int cwidth = style->GetToolSize().x * m_scale;
    int cheight = style->GetToolSize().y * m_scale;
    cheight = wxMin(cheight, compassBg.GetHeight());
    cwidth = wxMin( cwidth, cheight );
    cheight = cwidth;

    if( g_bCourseUp )  // ALT
//        if( m_parent->m_bCourseUp )  // NEU
        BMPRose = style->GetIcon( _T("CompassRose"), cwidth, cheight );
    else
        BMPRose = style->GetIcon( _T("CompassRoseBlue"), cwidth, cheight );
    if( ( fabs( cc1->GetVPRotation() ) > .01 ) || ( fabs( cc1->GetVPSkew() ) > .01 ) ) {  // ALT
//    if( ( fabs( m_parent->GetVPRotation() ) > .01 ) || ( fabs( m_parent->GetVPSkew() ) > .01 ) ) {  // NEU

        wxImage rose_img = BMPRose.ConvertToImage();
        wxPoint rot_ctr( cwidth / 2, cheight / 2  );
        wxImage rot_image = rose_img.Rotate( rose_angle, rot_ctr, true, &after_rotate );
        BMPRose = wxBitmap( rot_image ).GetSubBitmap( wxRect( -after_rotate.x, -after_rotate.y, cwidth, cheight ));
    }

    wxBitmap iconBm;

    if( style->HasBackground() ) {
        iconBm = MergeBitmaps( compassBg, BMPRose, wxSize( 0, 0 ) );
    } else {
        iconBm = BMPRose;
    }

    iconBm = ConvertTo24Bit( wxColor(0,0,0), iconBm);

    mdc.DrawBitmap( iconBm, offset );
    offset.x += iconBm.GetWidth();
    offset.x += style->GetToolSeparation();

    m_rose_angle = rose_angle;

//        if(m_bshowGPS){  // NEU
    //  GPS Icon
    int twidth = style->GetToolSize().x * m_scale;
    int theight = style->GetToolSize().y * m_scale;
    theight = wxMin(theight, compassBg.GetHeight());  // (cheight
    int swidth = wxMax( twidth, theight );
    int sheight = wxMin( twidth, theight );

    //  Sometimes, the SVG renderer gets the size wrong due to some internal rounding error.
    //  If so found, it seems to work OK by just reducing the requested size by one pixel....
    wxBitmap gicon = style->GetIcon( gpsIconName, swidth, sheight );
#ifndef __WXOSX__
    if( gicon.GetHeight() != sheight )
        gicon = style->GetIcon( gpsIconName, swidth-1, sheight-1, true );  // -1
#endif
    if( style->HasBackground() ) {
        iconBm = MergeBitmaps( gpsBg, gicon, wxSize( 0, 0 ) );
    } else {
        iconBm = gicon;
    }

    iconBm = ConvertTo24Bit( wxColor(0,0,0), iconBm);
    mdc.DrawBitmap( iconBm, offset );
    mdc.SelectObject( wxNullBitmap );

    m_lastgpsIconName = gpsIconName;
//#ifdef __WXOSX__  //  Wenn ocpnUSE_GL dann schwarzes Rechteck, bei ocpnUSE_GLES (original) dann nichts.
//#if defined(ocpnUSE_GLES)   || defined (ocpnUSE_GL)  // GLES does not do ocpnDC::DrawBitmap(), so use texture
//#else
#if defined(ocpnUSE_GLES)
//#endif
    if(g_bopengl){
        wxImage image = m_StatBmp.ConvertToImage(); // Das Bild (siehe Interfaces/bitmap.h)
        unsigned char *imgdata = image.GetData();  // Bild-Daten (siehe Interfaces/image.h, Array mit RGBRGBRGBRGB...)
        unsigned char *imgalpha = image.GetAlpha();  // Alpha Bild-Daten ??? Return Alpha bei x,y Pixel ???
        int tex_w = image.GetWidth();  // Breite
        int tex_h = image.GetHeight();  // Höhe
// siehe Kapitel 9 red
        GLuint format = GL_RGBA;  // GL_RGBA;
        GLuint internalformat = format;
        int stride = 4;

        if(imgdata){
            unsigned char *teximage = (unsigned char *) malloc( stride * tex_w * tex_h );

            for( int j = 0; j < tex_w * tex_h; j++ ){
                for( int k = 0; k < 3; k++ )
                    teximage[j * stride + k] = imgdata[3*j + k];
                teximage[j * stride + 3] = imgalpha ? imgalpha[j] : 255;      // alpha, alt 3 neu 4 ?
            }

            if(texobj){
                glDeleteTextures(1, &texobj);
                texobj = 0;
            }

            glGenTextures( 1, &texobj );
            glBindTexture( GL_TEXTURE_2D, texobj );

            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );  // GL_LINEAR ); s.u.
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );  // Zeile nicht tot: weiß wird schwarz

            glTexImage2D( GL_TEXTURE_2D, 0, internalformat, tex_w, tex_h, 0,
                        format, GL_UNSIGNED_BYTE, teximage );

            free(teximage);
        }
   }
#endif

}
