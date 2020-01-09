/* **************************************************************************
 * $Id: iacfile.h, v1.0 2010/08/05 SethDart Exp $
 *
 * Project:  OpenCPN
 * Purpose:  IACFleet Plugin
 * Author:   Carsten Borchardt
 *
 ***************************************************************************
 *   Copyright (C) 2010 by Carsten Borchardt                               *
 *   Carsten@sy-fortytwo.de                                                *
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

#ifndef _IACFILE_H_
#define _IACFILE_H_

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers

#include <wx/dynarray.h>
#include <wx/gdicmn.h>
#include <wx/object.h>
#include <wx/stream.h>
#include <wx/wfstream.h>
#include "TexFont.h"

#include "../../../include/ocpn_plugin.h"
#ifdef ocpnUSE_GL
#ifdef __WXMSW__
#include <GL/glu.h>
#include "GL/gl.h"            // local copy for Windows
#else

#ifndef __OCPN__ANDROID__
#include <GL/gl.h>
#include <GL/glu.h>
#else
#include "GL/gl_private.h"
#include "qopengl.h"                  // this gives us the qt runtime gles2.h
#endif

#endif
#endif

#include <algorithm>
#include <vector>

#define ISOBAR_WIDTH        2
#define FRONT_WIDTH         5
#define GL_MIN_LINE_WIDTH   1.5
#define NUMBERS_FONT_SIZE   9
#define SYSTEMS_FONT_SIZE   15

#define QUASISTATIONARY_SURFACE 0
#define QUASISTATIONARY_ABOVE_SURFACE 1
#define WARM_SURFACE 2
#define WARM_ABOVE_SURFACE 3
#define COLD_SURFACE 4
#define COLD_ABOVE_SURFACE 5
#define OCCLUSION 6
#define INSTABILITY_LINE 7
#define INTERTROPICAL 8
#define CONVERGENCE_LINE 9

#define SECTION_PRESSURE 0
#define SECTION_FRONTAL 11
#define SECTION_ISOBAR 22
#define SECTION_TROPICAL 55

#define POS_NH_HALF_DEG 0
#define POS_OCTANTS 88

#define MAX_FILESIZE 20000

class GeoPoint : public wxRealPoint {
public:
    static const double INVALID_KOORD;
    GeoPoint( double lon = INVALID_KOORD, double lat = INVALID_KOORD ) { Set(lon, lat); }
    GeoPoint( wxString &token, int coordsys ) { Set( token, coordsys ); }
    void Set( wxString &token, int coordsys );
    void Set( double lon = INVALID_KOORD, double lat = INVALID_KOORD ) { x = lon; y = lat; }
    wxString ToString( void );
    bool MatchPosition( GeoPoint &refPos, double deviation );
};

WX_DECLARE_OBJARRAY(GeoPoint, GeoPoints);

class IACSystem
{
public:
    IACSystem( void );
    virtual ~IACSystem();
    //void SetMovement(unsigned int m, unsigned int d, unsigned int s);
    virtual wxString ToString( bool includePosition = true ) const;
    virtual bool Draw( wxDC *dc, PlugIn_ViewPort *vp, TexFont &numfont, TexFont &sysfont );
    wxString GetTab( const wxChar*(tab[]), size_t index ) const;
    wxString PositionsToString( void ) const;
    bool DrawPositions( wxDC *dc, PlugIn_ViewPort *vp );
    virtual bool FindAtPos( GeoPoint &pos, double deviation );
    GeoPoints   m_positions;
    int         m_type;
    int         m_char;
    int         m_val;
    int         m_int;
    unsigned int m_movement;  //
    unsigned int m_direction; // in deg
    unsigned int m_speed;     // in knots, 99=unknown

protected:
    wxColor     m_isoLineColor;
    float       m_isoLineWidth;

private:
    virtual wxString GetType( size_t index ) const;
    virtual wxString GetShortType( size_t index ) const;
    virtual wxString GetCharacteristic( size_t index ) const;
    virtual wxString GetValue( void ) const {return wxEmptyString;}
    virtual wxString GetIntensity( void ) const {return wxEmptyString;}
    wxString GetMovement( void ) const;
};

class IACPressureSystem : public IACSystem
{
private:
    wxString GetType( size_t index ) const;
    wxString GetShortType( size_t index ) const;
    wxString GetCharacteristic( size_t index ) const;
    wxString GetValue( void ) const;
};

class IACFrontalSystem : public IACSystem
{
public:
private:
    wxString GetType( size_t index ) const;
    wxString GetCharacteristic( size_t index ) const;
    wxString GetIntensity( void ) const;
    bool Draw( wxDC *dc, PlugIn_ViewPort *vp, TexFont &numfont, TexFont &sysfont );
};

class IACIsobarSystem : public IACSystem
{
public:
    wxString ToString( bool includePosition = true ) const;
    bool Draw( wxDC *dc, PlugIn_ViewPort *vp, TexFont &numfont, TexFont &sysfont );
};

class IACTropicalSystem : public IACSystem
{
public:
private:
    wxString GetType( size_t index ) const;
    wxString GetShortType( size_t index ) const;
    wxString GetIntensity( void ) const;
    wxString GetCharacteristic( size_t index ) const;
    wxString GetValue( void ) const;
    bool Draw( wxDC *dc, PlugIn_ViewPort *vp, TexFont &numfont, TexFont &sysfont );
};


WX_DECLARE_OBJARRAY(IACSystem, IACSystems);
WX_DECLARE_OBJARRAY(IACPressureSystem, IACPressureSystems);
WX_DECLARE_OBJARRAY(IACFrontalSystem, IACFrontalSystems);
WX_DECLARE_OBJARRAY(IACIsobarSystem, IACIsobarSystems);
WX_DECLARE_OBJARRAY(IACTropicalSystem, IACTropicalSystems);

class IACFile
{
public:
    IACFile();
    IACFile( wxInputStream &stream );
    ~IACFile();

    wxString ToString( void );
    wxString GetIssueDate( void ) { return m_issueDate; }
    bool IsForecast(void) { return m_RawData.StartsWith(_T("F")); }

    bool Read( wxInputStream &stream );
    bool IsOk(void) { return m_isok; }
    bool Draw( wxDC *dc, PlugIn_ViewPort *vp );
    static int TokenNumber( wxString &token, size_t start, size_t end );
    static wxFileInputStream* GetStream( wxString &filename );
    wxString &GetRawData( void ) { return m_RawData; }
    static const size_t IACMaxSize = MAX_FILESIZE;

    void Invalidate( void );
    IACSystem * FindSystem( GeoPoint &pos, double deviation );

private:
    IACSystem * FindSystem( IACSystems &systems, GeoPoint &pos, double deviation );
    wxString ReadToken( wxInputStream &file );
    wxString tokenFind( const wxChar* match = wxT("?????"), bool skip = false );
    void PushbackToken( void );
    bool Decode( void );

    bool Parse( void );
    bool ReadHeader( void );
    bool ParsePositions( IACSystem &sys, int section );
    bool ParseMovement( IACSystem &sys );
    bool ParseSections( void );
    bool ParsePressureSection( void );
    bool ParseFrontalSection( void );
    bool ParseIsobarSection( void );
    bool ParseTropicalSection( void );
    GeoPoint ParsePosition( void );
    bool DrawSystems( wxDC *dc, PlugIn_ViewPort *vp, IACSystems &iacsystem );

    bool              m_isok;
    wxArrayString     m_tokens;
    size_t            m_tokensI;
    wxString          m_filename;
    wxString          m_RawData;
    wxString          m_issueDate;
    IACPressureSystems m_pressure;
    IACFrontalSystems  m_frontal;
    IACIsobarSystems   m_isobars;
    IACTropicalSystems m_tropical;
    TexFont m_TexFontNumbers;
    TexFont m_TexFontSystems;
    double m_minlat;
    double m_maxlat;
    double m_minlone;
    double m_maxlone;
    double m_minlonw;
    double m_maxlonw;
    int m_positionsType;
    std::vector<size_t> m_newlineTokens;
};
#endif
