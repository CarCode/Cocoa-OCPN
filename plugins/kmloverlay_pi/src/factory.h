/***************************************************************************
 * $Id: factory.h, v0.1 2012-02-08 SethDart Exp $
 *
 * Project:  OpenCPN
 * Purpose:  KML overlay plugin
 * Author:   Jean-Eudes Onfray
 *
 ***************************************************************************
 *   Copyright (C) 2012 by Jean-Eudes Onfray                               *
 *   je@onfray.fr                                                          *
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
 ***************************************************************************
 */

#ifndef _KMLOverlayFactory_H_
#define _KMLOverlayFactory_H_

#include <wx/wxprec.h>

#ifndef  WX_PRECOMP
  #include <wx/wx.h>
#endif //precompiled headers

#include <kml/engine.h>
#include "../../../include/ocpn_plugin.h"

class KMLOverlayFactory
{
public:
      KMLOverlayFactory();
      ~KMLOverlayFactory();

      bool RenderOverlay( wxDC &dc, PlugIn_ViewPort *vp );
      bool RenderGLOverlay( wxGLContext *pcontext, PlugIn_ViewPort *vp );

      bool Add( wxString filename, bool visible );
      void SetVisibility( int idx, bool visible );
      void Delete( int idx );
      wxString GetFilename( int idx );
      bool GetVisibility( int idx );
      int GetCount();

private:
      class Container
      {
      public:
            Container(wxString filename, bool visible);
            bool Parse();
            bool Render( wxDC &dc, PlugIn_ViewPort *vp );
            bool RenderGL( wxGLContext *pcontext, PlugIn_ViewPort *vp );
            void SetVisibility( bool visible );
            wxString GetFilename();
            bool GetVisibility();

      private:
            const kmldom::StylePtr GetFeatureStylePtr( const kmldom::FeaturePtr& feature );
            void DoDrawCircle( wxPen pen, wxBrush brush, wxPoint pt, int radius );
            void DoDrawLines( wxPen pen, int n, wxPoint points[] );
            void DoDrawPolygon( wxPen pen, wxBrush brush, int n, wxPoint points[] );
            void DoDrawBitmap( const wxBitmap &bitmap, wxCoord x, wxCoord y, bool usemask );
            void RenderPoint( const kmldom::PointPtr& point, const kmldom::StylePtr& style );
            void RenderLineString( const kmldom::LineStringPtr& linestring, const kmldom::StylePtr& style );
            void RenderLinearRing( const kmldom::LinearRingPtr& linearring, const kmldom::StylePtr& style );
            void RenderPolygon( const kmldom::PolygonPtr& polygon, const kmldom::StylePtr& style );
            void RenderGroundOverlay( const kmldom::GroundOverlayPtr& groundoverlay );
            void RenderGeometry( const kmldom::GeometryPtr& geometry, const kmldom::StylePtr& style );
            void RenderFeature( const kmldom::FeaturePtr& feature );
            bool DoRender();
            wxDC            *m_pdc;
            wxGLContext     *m_pcontext;
            PlugIn_ViewPort *m_pvp;
            bool       m_ready;
            wxString   m_filename;
            bool       m_visible;
            kmlengine::KmzFilePtr m_kmz_file;
            kmlengine::KmlFilePtr m_kml_file;
            kmldom::FeaturePtr m_root;

      };
      WX_DEFINE_ARRAY(Container *, ContainerArray);

      ContainerArray m_Objects;

};

#endif
