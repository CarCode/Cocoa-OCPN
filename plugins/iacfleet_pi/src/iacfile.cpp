/***************************************************************************
 * $Id: IACFile.cpp, v1.0 2010/08/05 SethDart Exp $
 *
 * Project:  OpenCPN
 * Purpose:  IACFleet Plugin
 * Author:   Carsten Borchardt
 *
 ***************************************************************************
 *   Copyright (C) 2010 by DCarsten Borchardt                              *
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

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers

#include <wx/filename.h>

#include "iacfile.h"

#include <wx/arrimpl.cpp> // this is a magic incantation which must be done!
WX_DEFINE_OBJARRAY(IACSystems);
WX_DEFINE_OBJARRAY(IACPressureSystems);
WX_DEFINE_OBJARRAY(IACFrontalSystems);
WX_DEFINE_OBJARRAY(IACIsobarSystems);
WX_DEFINE_OBJARRAY(IACTropicalSystems);
WX_DEFINE_OBJARRAY(GeoPoints);

// Is the given point in the vp ??
// because of a the vp-implementation we need
// to check the latitude three times! (+-360)
static bool PointInLLBox( PlugIn_ViewPort *vp, double x, double y )
{
    if (  ((x >= (vp->lon_min) && x <= (vp->lon_max))
            || ((x+360.) >= (vp->lon_min) && (x+360.) <= (vp->lon_max))
            || ((x-360.) >= (vp->lon_min) && (x-360.) <= (vp->lon_max))
          ) && (y >= (vp->lat_min) && y <= (vp->lat_max)) )
        return TRUE;
    return FALSE;
}

//---------------------------------------------
// IACFile implementation
//---------------------------------------------
IACFile::IACFile( void )
{
    Invalidate();
}

IACFile::IACFile( wxInputStream &stream )
{
    Invalidate();
    m_isok = Read(stream);
}

IACFile::~IACFile( void )
{
}

void IACFile::Invalidate( void )
{
    m_tokens.Clear();
    m_tokensI = 0;
    m_isok = false;
    m_RawData.Clear();
    m_pressure.Clear();
    m_frontal.Clear();
    m_isobars.Clear();
    m_tropical.Clear();

    m_minlat = 999.9;
    m_maxlat = -999.9;
    m_minlone = 999.9;
    m_maxlone = -999.9;
    m_minlonw = 999.9;
    m_maxlonw = -999.9;
    m_positionsType = -1;
    m_newlineTokens.clear();
}

wxFileInputStream* IACFile::GetStream( wxString &filename )
{
    wxFileName file(filename);
    if( file.FileExists() && (file.GetSize() < IACMaxSize) )
    {
        return new wxFileInputStream(filename);
    }
    else
    {
        return NULL;
    }
}

bool IACFile::Read( wxInputStream &stream )
{
    bool isok = false; //true if minimum one token was read from file
    Invalidate();
    wxString token;
    m_tokensI = 0;
    if( stream.IsOk() )
    {
        for(;;)
        {
            token = ReadToken(stream);
#ifdef __WXOSX__   // end of IAC
            if (token == "19191") break;
#endif
            if( !token.IsEmpty() )
            {
                m_tokens.Add(token);
                m_tokensI++;
                isok = true;
            }
            else
            {
                break;
            }
        }
    }
    m_tokensI = 0;
    
    //    for (std::vector<size_t>::iterator it = m_newlineTokens.begin(); it != m_newlineTokens.end(); ++it)
    //    {
    //        wxMessageBox( wxString::Format( _T("ID: %i :"), *it ) + m_tokens[*it] );
    //    }
    if( isok )
    {
        // decode tokens if some were found
        isok = Decode();
    }
    m_isok = isok;
    return isok;
}

wxString IACFile::ToString( void )
{
    wxString t;
    size_t i;
    t.Append(_("IAC Fleet Code issued at ") + m_issueDate + _T("\n"));

    // Pressure Systems
    t.Append(_("\n\nPressure systems:"));
    t.Append(wxT(  "\n================="));
    for( i=0; i<m_pressure.Count(); i++ )
    {
        t.Append(wxT("\n\n") + m_pressure[i].ToString());
    }
    t.Append(_("\n\nFrontal systems:"));
    t.Append(wxT(  "\n================"));
    for(i=0; i<m_frontal.Count(); i++)
    {
        t.Append(wxT("\n\n") + m_frontal[i].ToString());
    }
    t.Append(_("\n\nTropical systems:"));
    t.Append(wxT(  "\n================="));
    for(i=0; i<m_tropical.Count(); i++)
    {
        t.Append(wxT("\n\n") + m_tropical[i].ToString());
    }

    t.Append(_("\n\nIsobars:"));
    t.Append(wxT(  "\n================="));
    for(i=0; i<m_isobars.Count(); i++)
    {
        t.Append(wxT("\n\n") + m_isobars[i].ToString());
    }
    return t;
}

bool IACFile::Decode( void )
{
    // init
    m_tokensI = 0;
    bool res = ReadHeader();
    if( res )
    {
        res = ParseSections();
    }
    return res;
}

bool IACFile::ReadHeader( void )
{
    bool data = !tokenFind(_T("10001"), true).IsEmpty();
    if( !data ) //If it is not analysis, lets check for NOAA 24 hour forecast
    {
        m_tokensI = 0;
        data = !tokenFind(_T("65556"), true).IsEmpty();
    }
    if( data )
    {
        wxString datasetstr = tokenFind(_T("333??"));
        if( !datasetstr.IsEmpty() )
        {
            m_positionsType = IACFile::TokenNumber(datasetstr, 3, 2);
            // header found, read time
            wxString timestr = tokenFind(_T("0????"));
            if( !timestr.IsEmpty() )
            {
                // parse time, format 0DDHH, DD=date, HH=UTC hours
                m_issueDate = _("Day ") + timestr.Mid(1, 2) +
                _(" Hour ") + timestr.Mid(3, 2) +
                _T(" UTC");
                return true;
            }
        }
    }
    return false;
}

bool IACFile::ParsePositions( IACSystem &sys, int section )
{
    wxString token;
    wxString lasttoken;
    bool firsttime = true;

    for(;;)
    {
        lasttoken = token;
        token = tokenFind();
        bool morepos = true;
        // heuristic about when the last postion is read
        // either on illegal octant "4" or by a step of more than 1
        // in octant
        int lastoct = TokenNumber(lasttoken, 0, 1);
        int oct     = TokenNumber(token, 0, 1);
        int diff = abs(lastoct - oct);

        if( m_positionsType == POS_OCTANTS && oct == 4 )
        {
            morepos = false;
        }
        else if( m_positionsType == POS_OCTANTS && (diff > 1) && (diff < 8) )
        {
            morepos = false;
        }
        else if( section == SECTION_FRONTAL && token.Matches(_T("66???"))
                && ( m_positionsType == 88 || m_newlineTokens.size() < 10 //In case the file seems formatted with new lines, we take advantage of it and say that new system must start on new line
                    || std::find(m_newlineTokens.begin(), m_newlineTokens.end(), m_tokensI - 1 ) != m_newlineTokens.end() ) )
        {
            morepos = false;
        }
        else if( section == SECTION_PRESSURE && token.StartsWith(_T("8"))
                && ( m_positionsType == 88 || m_newlineTokens.size() < 10 //In case the file seems formatted with new lines, we take advantage of it and say that new system must start on new line
                    || std::find(m_newlineTokens.begin(), m_newlineTokens.end(), m_tokensI - 1 ) != m_newlineTokens.end() ) )
        {
            morepos = false;
        }
        else if( token.Matches( _T("999??") ) )
        {
            morepos = false;
        }
        else if( section == SECTION_ISOBAR && ( token.Matches(_T("440??")) || token.Matches(_T("449??")) || token.Matches(_T("448??")) ) )
        {
            morepos = false;
        }
        else if( token == _T("19191"))
        {
            morepos = false;
        }

        /*
        // lets be even more strict and limit positions to
        // SW-Pacific
         if( !((oct == 6) || (oct == 7)))
        {
            morepos = false;
        }

        GeoPoint pos(token); // decode position
        // Position must be in the area bounded by 0S..35S and 120W/150E
        if( (pos.y < -35.0) ||
                (pos.y > 0.0) ||
                ((pos.x > -120.0) && (pos.x < 150))
          )
        {
            morepos = false;
        }
         */
        if( !token.IsEmpty() && (firsttime || morepos ) )
        {
            // ignore double entries following eachother
            if( token != lasttoken )
            {
                GeoPoint pos( token, m_positionsType );
                sys.m_positions.Add(pos);
                if( pos.x >= 0.0)
                {
                    if( pos.x < m_minlone )
                        m_minlone = pos.x;
                    if( pos.x > m_maxlone )
                        m_maxlone = pos.x;
                }
                else
                {
                    if( pos.x < m_minlonw )
                        m_minlonw = pos.x;
                    if( pos.x > m_maxlonw )
                        m_maxlonw = pos.x;
                }
                if( pos.y < m_minlat )
                    m_minlat = pos.y;
                if( pos.y > m_maxlat )
                    m_maxlat = pos.y;
//                if( pos.x < 0.1 && pos.x > -0.1 )
//                    wxMessageBox( token );
            }
            else
            {
                // we got two identical position entries
                // meaning "no more"
                // stop parsing positions, but read one
                // more token to stay in sync
                if( m_positionsType == 88 )
                    token = tokenFind();
                //break;
            }
        }
        else
        {
            break;
        }
        if( firsttime )
        {
            firsttime = false;
        }
    }
    PushbackToken();
    if( token.IsEmpty() )
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool IACFile::ParseMovement( IACSystem &sys )
{
    wxString token;

    token = tokenFind();
    if( !token.IsEmpty() )
    {
        // if invalid movement, ignore, push back
        int dir = 10 * TokenNumber(token, 1, 2);
        if( dir < 361 )
        {

            sys.m_movement  = TokenNumber(token, 0, 1);
            sys.m_direction = dir;
            sys.m_speed     = TokenNumber(token, 3, 2);
            return true;
        }
        else
        {
            // invalif movement, push back
            PushbackToken();
            return false;
        }

    }
    else
    {
        return false;
    }
}

bool IACFile::ParseSections( void )
{
    wxString token;
    do
    {
        token = tokenFind(_T("999??"), true);
        if( !token.IsEmpty() )
        {
            //?? 00=>Pressure Systems
            //   11=>Frontal Systems
            //   22=>Isobars
            //   55=>Tropical Section
            int section = TokenNumber(token, 3, 2);
            switch(section)
            {
            case SECTION_PRESSURE:
                ParsePressureSection();
                break;
            case SECTION_FRONTAL:
                ParseFrontalSection();
                break;
            case SECTION_ISOBAR:
                ParseIsobarSection();
                break;
            case SECTION_TROPICAL:
                ParseTropicalSection();
                break;
            default:
                break;
            }
        }
    }
    while( !token.IsEmpty() );
    return true;
}
bool IACFile::ParsePressureSection(void)
{
    wxString token;
    for(;;)
    {
        token = tokenFind(_T("8????"));
        if( !token.IsEmpty( ) )
        {            // parse pressure system token
//            if( std::find(m_newlineTokens.begin(), m_newlineTokens.end(), m_tokensI) == m_newlineTokens.end() )
//                wxMessageBox(_T("FP: ") + token);
            IACPressureSystem sys;
            sys.m_type = TokenNumber(token, 1, 1);
            sys.m_char = TokenNumber(token, 2, 1);
            sys.m_val  = TokenNumber(token, 3, 2);
            sys.m_int  = -1;
            // guess pressure offset
            if( sys.m_type == 1 && sys.m_val > 30 )
            {
                sys.m_val += 900;
            }
            else if( sys.m_type == 5 && sys.m_val < 70 )
            {
                sys.m_val += 1000;
            }
            else if( sys.m_val > 50 )
            {
                sys.m_val += 900;
            }
            else
            {
                sys.m_val += 1000;
            }

            ParsePositions(sys, SECTION_PRESSURE);
            if( !m_tokens[m_tokensI].StartsWith(_T("8")) )
                ParseMovement(sys);
            m_pressure.Add(sys);
        }
        else
        {
            PushbackToken();
            break;
        }
    }
    return true;
}

bool IACFile::ParseFrontalSection( void )
{
    wxString token;
    for(;;)
    {
        token = tokenFind(_T("66???"));
        if( !token.IsEmpty() )
        {
            // parse pressure system token
            IACFrontalSystem sys;
            sys.m_type = TokenNumber(token, 2, 1);
            sys.m_val  = -1;
            sys.m_int  = TokenNumber(token, 3, 1);
            sys.m_char = TokenNumber(token, 4, 1);


            ParsePositions(sys, SECTION_FRONTAL);
            ParseMovement(sys);
            m_frontal.Add(sys);
        }
        else
        {
            // no more frontal systems.
            PushbackToken();
            break;
        }
    }
    return true;
}

bool IACFile::ParseIsobarSection( void )
{
    wxString token;
    for(;;)
    {
        token = tokenFind(_T("44???"));
        if( !token.IsEmpty() )
        {
            IACIsobarSystem sys;
            sys.m_val = TokenNumber(token, 2, 3);
            if( sys.m_val < 500 )
            {
                sys.m_val += 1000;
            }

            // Position
            ParsePositions(sys, SECTION_ISOBAR);
            m_isobars.Add(sys);
        }
        else
        {
            // no more frontal systems.
            PushbackToken();
            break;
        }
    }
    return true;
}

bool IACFile::ParseTropicalSection( void )
{
    wxString token;
    for(;;)
    {
        token = tokenFind(_T("55???"));
        if( !token.IsEmpty() )
        {
            // parse pressure system token
            IACTropicalSystem sys;
            sys.m_type = TokenNumber(token, 2, 1);
            sys.m_int  = TokenNumber(token, 3, 1);
            sys.m_char = TokenNumber(token, 4, 1);

            // tropical system token MAY be followed by 555PP pressure token
            // in this case it is a tropical LOW or cyclone
            token = tokenFind(_T("555??"));
            if( token.IsEmpty() )
            {
                // no, push back token - it is a position
                PushbackToken();
            }
            else
            {
                sys.m_val = TokenNumber(token, 3, 2);
                // guess pressure offset
                if( sys.m_val > 50 )
                {
                    sys.m_val += 900;
                }
                else
                {
                    sys.m_val += 1000;
                }
            }
            ParsePositions(sys, SECTION_TROPICAL);
            ParseMovement(sys);
            m_tropical.Add(sys);
        }
        else
        {
            PushbackToken();
            break;
        }
    }
    return true;
}

int IACFile::TokenNumber( wxString &token, size_t start, size_t end )
{
    unsigned long ul;
    if( token.Mid(start,end).ToULong(&ul) )
    {
        //success
        return ul;
    }
    else
    {
        //failure
        return -1;
    }
}

wxString IACFile::tokenFind( const wxChar* match, bool skip )
{
    while( m_tokensI < m_tokens.GetCount() )
    {
        wxString token = m_tokens[m_tokensI++];
        if( token.Matches(match) )
        {
            // found
            return token;
        }
        if( !skip )
        {
            break;
        }
    }
    // not found
    return wxEmptyString;
}

void IACFile::PushbackToken( void )
{
    if( m_tokensI > 0 )
    {
        m_tokensI--;
    }
}

wxString IACFile::ReadToken( wxInputStream &file )
{
    // 0 = read chars until digit
    // 1 = read digits until no digit
    // 2 = token found
    wxString token = wxEmptyString;

    int mode = 0;

    while( file.IsOk() && mode != 2 )
    {
        int c = file.GetC();

        if( c != wxEOF)
        {
            if( c == '\n' && m_tokensI > 0)
            {
                m_newlineTokens.push_back( m_tokensI + 1 );
            }
            m_RawData.Append( (char)c );
            switch( mode )
            {
            case 0:
                if( isdigit(c) )
                {
                    token.Append( (char)c );
                    mode = 1;
                }
                break;
            case 1:
                if( isdigit(c) || c == '/' )
                {
                    token.Append( (char)c );
                }
                else
                {
                    if( token.Len() == 5 )
                    {
                        // token found!!
                        mode = 2;
                    }
                    else
                    {
                        token.Empty();
                    }
                }
                break;
            case 2:
                mode = 0;
                break;
            }// case
        }
    }// while
    if( mode != 2 )
    {
        token.Empty();
    }

    return token;
}

// draw the fleet code data
// returns true if anything was drawn
bool IACFile::Draw( wxDC *dc, PlugIn_ViewPort *vp )
{
    bool retval = false;
    // draw only if file was successfully read and decoded
    if( IsOk() )
    {
        wxColor colour;
        GetGlobalColor ( _T ( "SNDG1" ), &colour );
        wxPoint p1, p2, p3, p4;
        double minlon, maxlon;
        if( m_minlone < 999 )
            minlon = m_minlone;
        else
            minlon = m_minlonw;
        if( m_maxlonw > -999 )
            maxlon = m_maxlonw;
        else
            maxlon = m_maxlone;
        if( -179.0 < m_minlonw && m_minlonw < 0 && 179.0 > m_maxlone && m_maxlone > 0 )
        {
            minlon = m_minlonw;
            maxlon = m_maxlone;
        }

        GetCanvasPixLL(vp, &p1, m_minlat, minlon);
        GetCanvasPixLL(vp, &p2, m_maxlat, minlon);
        GetCanvasPixLL(vp, &p3, m_maxlat, maxlon);
        GetCanvasPixLL(vp, &p4, m_minlat, maxlon);
        if( dc )
        {
            if( p3.x > 0 && p1.x < vp->pix_width )
            {
                dc->SetPen( wxPen( colour, ISOBAR_WIDTH ) );
                wxPoint points[] = { p1, p2, p3, p4, p1 };
                dc->DrawLines( 5, points );
            }
        }
        else
        {
            wxFont font_numbers( NUMBERS_FONT_SIZE, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL );
            m_TexFontNumbers.Build(font_numbers);
            wxFont font_systems( SYSTEMS_FONT_SIZE, wxFONTFAMILY_ROMAN, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_BOLD );
            m_TexFontSystems.Build(font_systems);

            if( p3.x > 0 && p1.x < vp->pix_width )
            {
                glColor3ub(colour.Red() , colour.Green(), colour.Blue());
                glBegin(GL_LINE_LOOP);
                glVertex2i(p1.x, p1.y);
                glVertex2i(p2.x, p2.y);
                glVertex2i(p3.x, p3.y);
                glVertex2i(p4.x, p4.y);
                glEnd();
            }
        }
        // This magic initialisation to the random numer generator
        // ensures that the (random) positions where some text
        // is written does not change between several calls to "Draw"
        // I love this kind of crazy code - it is like in the good
        // old days of programming where everything was allowed.
        srand(77);
        retval |= DrawSystems( dc, vp, (IACSystems &)m_pressure );
        retval |= DrawSystems( dc, vp, (IACSystems &)m_frontal );
        retval |= DrawSystems( dc, vp, (IACSystems &)m_isobars );
        retval |= DrawSystems( dc, vp, (IACSystems &)m_tropical );
    }
    return retval;
}

bool IACFile::DrawSystems( wxDC *dc, PlugIn_ViewPort *vp, IACSystems &iacsystem )
{
    bool retval = false;
    // loop over all systems
    for( size_t Index = 0; Index < iacsystem.GetCount(); Index++ )
    {
        retval |= iacsystem[Index].Draw( dc, vp, m_TexFontNumbers, m_TexFontSystems );
    }
    return retval;
}

IACSystem * IACFile::FindSystem( GeoPoint &pos, double deviation )
{
    IACSystem *pIACSystem = NULL;
    pIACSystem = FindSystem( (IACSystems&)m_pressure, pos, deviation );
    if( NULL == pIACSystem )
    {
        pIACSystem = FindSystem( (IACSystems&)m_frontal, pos, deviation );
        if( NULL == pIACSystem )
        {
            pIACSystem = FindSystem( (IACSystems&)m_tropical, pos, deviation) ;
        }
    }
    return pIACSystem;
}

IACSystem * IACFile::FindSystem( IACSystems &systems, GeoPoint &pos, double deviation )
{
    IACSystem * pSystem = NULL;
    for( size_t i = systems.GetCount(); i != 0; i-- )
    {
        if( systems[i - 1].FindAtPos(pos, deviation) )
        {
            // found system
            pSystem = &(systems[i - 1]);
        }
    }
    return pSystem;
}


//---------------------------
// implementatin of GeoPoint
//---------------------------
const double GeoPoint::INVALID_KOORD = 9999.9;


// initialize from IAC lat/lon token
void GeoPoint::Set( wxString &token, int coordsys )
{
    if( token.Len() == 5 )
    {
        if( coordsys == POS_OCTANTS )
        {
            int oct = IACFile::TokenNumber(token, 0, 1);
            int lat = IACFile::TokenNumber(token, 1, 2);
            int lon = IACFile::TokenNumber(token, 3, 2);
            int ns = (oct > 3) ? -1 : +1; // sign for south(-1) or north(+1)
            lat *= ns;            // lat is now in degrees
            
            // eliminate north/south
            if( oct > 4 )
            {
                oct -= 5;
            }
            switch( oct )
            {
            case 0: // West 0..90
                lon = - lon;
                break;
            case 1: // West 90..180
                if ( lon < 90 )
                {
                    lon = - (100 + lon);
                }
                else
                {
                    lon = - lon;
                }
                break;
            case 2: // East 90 .. 180
                if( lon < 90 )
                {
                    lon = 100 + lon;
                }
                break;
            case 3: // East 0..90
                break;
            }
            Set( double(lon), double(lat) );
        }
        else if( coordsys == POS_NH_HALF_DEG )
        {
            int k = IACFile::TokenNumber(token, 4, 1);
            double lat = IACFile::TokenNumber(token, 0, 2);
            double lon = IACFile::TokenNumber(token, 2, 2);
            switch( k )
            {
            //East longitudes 0-99, west longitudes 100-180
            case 0: //As sent
                break;
            case 1: //Add 1/2 deg to lat
                lat += 0.5;
                break;
            case 2: //Add 1/2 deg to lon
                lon += 0.5;
                break;
            case 3: //Add 1/2 deg to lat and lon
                lat += 0.5;
                lon += 0.5;
                break;
            case 4: //Whole degrees
                break;
            case 5: //As sent
                lon = -lon;
                break;
            //East longitudes 10-180, west longitudes 0-99
            case 6: //Add 1/2 deg to lat
                lat += 0.5;
                lon = -lon;
                break;
            case 7: //Add 1/2 deg to lon
                lon += 0.5;
                lon = -lon;
                break;
            case 8: //Add 1/2 deg to lat and lon
                lat += 0.5;
                lon += 0.5;
                lon = -lon;
                break;
            case 9: //Whole degrees
                lon = -lon;
                break;
            }
            Set( lon, lat );
        }
        else
            Set();
    }
    else
    {
        Set(); // invalid token. set to invalid koordinates
    }
}

wxString GeoPoint::ToString( void )
{
    wxString t;
    // latitude
    unsigned int latdeg = floor(fabs(y));
    wxChar       lats   = (y < 0) ? 'S' : 'N';
    unsigned int londeg = floor(fabs(x));
    wxChar       lons   = (x < 0) ? 'W' : 'E';
    t.Printf(wxT("%02u%c %03u%c"), latdeg, lats, londeg, lons);
    return t;
}

bool GeoPoint::MatchPosition( GeoPoint &refPos, double deviation )
{
    if( (fabs(x - refPos.x) <= deviation) && (fabs(y - refPos.y) <= deviation) )
    {
        return true;
    }
    else
    {
        return false;
    }
}


//-----------------------------
// implementation of IACSystem
//-----------------------------

IACSystem::IACSystem( void ):
    m_positions(),
    m_type(-1),
    m_char(-1),
    m_val(-1),
    m_int(-1),
    m_movement(99),
    m_direction(99),
    m_speed(99),
    m_isoLineWidth(2)
{
    m_isoLineColor = *wxBLACK;
}

bool IACSystem::Draw( wxDC *dc, PlugIn_ViewPort *vp, TexFont &numfont, TexFont &sysfont )
{
    bool hasDrawn=false;
    if( dc )
    {
        if( m_positions.GetCount() == 1 )
        {
            GeoPoint &Pos = m_positions[0];
            if( PointInLLBox(vp, Pos.x, Pos.y) )
            {
                wxPoint p;
                GetCanvasPixLL(vp, &p, Pos.y, Pos.x);
                wxColour colour;
                wxString msg1 = GetShortType(m_type);
                if( !msg1.IsEmpty() )
                {
                    hasDrawn = true;
                    GetGlobalColor ( _T ( "UBLCK" ), &colour );
                    dc->SetTextForeground( colour );
                    wxFont sfont = dc->GetFont();

                    wxFont *font1 = wxTheFontList->FindOrCreateFont ( SYSTEMS_FONT_SIZE,
                            wxFONTFAMILY_ROMAN, wxNORMAL, wxFONTWEIGHT_BOLD,
                            FALSE, wxString ( _T ( "Arial" ) ) );
                    dc->SetFont(*font1);
                    wxSize s1 = dc->GetTextExtent(msg1);
                    dc->DrawText(msg1, p.x - (s1.GetWidth() / 2), p.y - (s1.GetHeight() / 2));
                    wxFont *font2 = wxTheFontList->FindOrCreateFont ( NUMBERS_FONT_SIZE,
                            wxFONTFAMILY_SWISS, wxITALIC, wxFONTWEIGHT_NORMAL,
                            FALSE, wxString ( _T ( "Arial" ) ) );
                    dc->SetFont(*font2);
                    wxString msg2 = GetValue();
                    if( !msg2.IsEmpty() )
                    {
                        wxSize s2 = dc->GetTextExtent(msg2);
                        dc->DrawText(msg2,
                                p.x - (s2.GetWidth() / 2),
                                p.y + (s1.GetHeight() / 2) + (s2.GetHeight() / 2));

                        dc->SetFont(sfont);
                    }
                }
            }
        }
        else
        {
            wxColour colour;
            wxPen pen;
            pen = dc->GetPen();
            GetGlobalColor ( _T ( "GREEN2" ), &colour );
            dc->SetPen(wxPen( colour, m_isoLineWidth ));
            DrawPositions( dc, vp );
            dc->SetPen(pen);
        }
    }
    else
    {
        if( m_positions.GetCount() == 1 )
        {
            GeoPoint &Pos = m_positions[0];
            if( PointInLLBox(vp, Pos.x, Pos.y) )
            {
                wxPoint p;
                GetCanvasPixLL(vp, &p, Pos.y, Pos.x);
                wxColour colour;
                wxString msg1 = GetShortType(m_type);
                if( !msg1.IsEmpty() )
                {
                    hasDrawn = true;
                    glEnable( GL_BLEND );
                    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
                    
                    wxPoint p;
                    GetCanvasPixLL(vp, &p, Pos.y, Pos.x);
                    int w, h;
                    sysfont.GetTextExtent(msg1, &w, &h);
                    
                    int xd = p.x - (w / 2);
                    int yd = p.y - (h / 2);
                    
                    GetGlobalColor ( _T ( "UBLCK" ), &colour );
                    glColor3ub(colour.Red() , colour.Green(), colour.Blue());

                    glEnable( GL_TEXTURE_2D );
                    sysfont.RenderString(msg1, xd, yd);
                    wxString msg2 = GetValue();
                    if( !msg2.IsEmpty() )
                    {
                        int w1, h1;
                        numfont.GetTextExtent(msg2, &w1, &h1);
                        numfont.RenderString(msg2, p.x - (w1 / 2), p.y + (h / 2) + (h1 / 2));
                    }
                    glDisable( GL_TEXTURE_2D );
                    glDisable( GL_BLEND );
                }
            }
        }
        else
        {
            wxColour colour = m_isoLineColor;
            GetGlobalColor ( _T ( "GREEN2" ), &m_isoLineColor );
            DrawPositions( dc, vp );
            m_isoLineColor = colour;
        }
    }
    return hasDrawn;
}

bool IACSystem::DrawPositions( wxDC *dc, PlugIn_ViewPort *vp )
{
    bool hasDrawn = false;
    if( dc )
    {
        for( size_t pointIndex = 0; pointIndex < m_positions.GetCount() - 1 ; pointIndex++ )
        {
            GeoPoint &startP = m_positions[pointIndex];
            GeoPoint &endP   = m_positions[pointIndex+1];
            if( PointInLLBox(vp, startP.x, startP.y) || PointInLLBox(vp, endP.x, endP.y) )
            {
                wxPoint startpl;
                wxPoint endpl;
                GetCanvasPixLL(vp, &startpl, startP.y, startP.x);
                GetCanvasPixLL(vp, &endpl, endP.y, endP.x);
                dc->DrawLine(startpl, endpl);
                hasDrawn = true;
            }
        }
    }
    else
    {
        //      Enable anti-aliased lines, at best quality
        glEnable( GL_LINE_SMOOTH );
        glEnable( GL_BLEND );
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
        glLineWidth( m_isoLineWidth );

        glColor4ub( m_isoLineColor.Red(), m_isoLineColor.Green(), m_isoLineColor.Blue(), 255 );
        glBegin( GL_LINES );
        wxPoint startpl;
        wxPoint endpl;
        for( size_t pointIndex = 0; pointIndex < m_positions.GetCount() - 1 ; pointIndex++ )
        {
            GeoPoint &startP = m_positions[pointIndex];
            GeoPoint &endP   = m_positions[pointIndex+1];
            if( PointInLLBox(vp, startP.x, startP.y) || PointInLLBox(vp, endP.x, endP.y) )
            {
                GetCanvasPixLL(vp, &startpl, startP.y, startP.x);
                GetCanvasPixLL(vp, &endpl, endP.y, endP.x);
                glVertex2d(startpl.x, startpl.y);
                glVertex2d(endpl.x, endpl.y);
                hasDrawn = true;
            }
        }
        glEnd();
    }
    return hasDrawn;
}


wxString IACSystem::GetTab( const wxChar*(tab[10]), size_t index ) const
{
    if( index < 10 )
    {
        return wxString(tab[index]);
    }
    else
    {
        return wxEmptyString;
    }
}

wxString IACSystem::GetMovement( void ) const
{
    static const wxChar* (tab[])=
    {
        wxT("-"), _("stationary"), _("little change"), _("stopping"), _("retarding"),
        _("curving to left"), _("recurving"), _("accelerating"),
        _("curving to right"), _("expected to recurve")
    };
    wxString t;
    if( m_movement < 10 )
    {
        // valid movement
        t = _("Movement: ") + GetTab(tab,m_movement);
        if( m_movement != 1 )  // not stationary, add dir and speed
        {
            wxString hlp;
            hlp.Printf(wxT(", %u\u00B0 "),m_direction);
            t.Append(hlp);
            if(m_speed >= 99)
            {
                t.Append(_("with unknown speed"));
            }
            else
            {
                hlp.Printf(_("with %uknots"),m_speed);
                t.Append(hlp);
            }
        }
    }
    return t;
}

wxString IACSystem::ToString( bool includePosition ) const
{
    wxString t;
    t.Append(GetType(m_type) + wxT(" "));
    if(!GetValue().IsEmpty())
    {
        t.Append(wxT("(") + GetValue() + wxT(") "));
    }
    if(!GetIntensity().IsEmpty())
    {
        t.Append(wxT(" ") + GetIntensity() + wxT(" "));
    }
    if( m_char >= 0 )
    {
        t.Append(GetCharacteristic(m_char) + wxT("\n"));
    }
    if(includePosition)
    {
        t.Append(PositionsToString() + wxT("\n"));
    }
    t.Append(GetMovement());
    return t;
}

bool IACSystem::FindAtPos( GeoPoint &pos, double deviation )
{
    bool found=false;
    // loop over positions and see if it matches
    for( size_t i = m_positions.GetCount(); i != 0; i-- )
    {
        if( m_positions[i - 1].MatchPosition(pos, deviation) )
        {
            found=true;
            break;
        }

    }
    return found;
}


wxString IACSystem::GetType( size_t index ) const
{
    return(wxEmptyString);
}

wxString IACSystem::GetShortType( size_t index ) const
{
    return(wxEmptyString);
}

wxString IACSystem::GetCharacteristic( size_t index ) const
{
    return(wxEmptyString);
}

wxString IACSystem::PositionsToString( void ) const
{
    wxString t;
    for( size_t i = 0; i < m_positions.Count(); i++ )
    {
        if( i != 0)
        {
            t.Append(wxT(" : "));
        }
        t.Append(m_positions[i].ToString());
    }
    return t;
}

wxString IACPressureSystem::GetValue( void ) const
{
    wxString t;
    if( m_val >= 0 )
    {
        t.Printf(wxT("%4u hPa"), m_val);
    }
    return t;
}

wxString IACPressureSystem::GetType( size_t index ) const
{
    static const wxChar* (tab[])=
    {
#ifdef __WXOSX__
        _("Complex Low"), _("Tief"), _("Secondary Low"), _("Trough"), _("Wave"),
#else
        _("Complex Low"), _("Low"), _("Secondary Low"), _("Trough"), _("Wave"),
#endif
        _("High"), _("Uniform pressure"), _("Ridge"), _("Col"), _("Tropical Storm")
    };
    return(wxString(tab[index]));
}

wxString IACPressureSystem::GetShortType( size_t index ) const
{
    static const wxChar* (tab[])=
    {
        wxT("L"), wxT("L"), wxT("L"), wxT("T"), wxT("W"),
        wxT("H"), wxT("U"), wxT("R"), wxT("C"), wxT("TS")
    };
    return(wxString(tab[index]));
}

wxString IACPressureSystem::GetCharacteristic( size_t index ) const
{
    static const wxChar* (tab[])=
    {
        wxEmptyString, _("weakening"), _("little change"), _("intensifying"), _("complex"),
        _("forming"), _("weakening but not disappearing"), _("general rise"),
        _("general fall"), _("position doubtful")
    };
    return(wxString(tab[index]));
}

wxString IACFrontalSystem::GetType( size_t index ) const
{
    static const wxChar* (tab[])=
    {
        _("Quasistationary at surface"), _("Quasistationary above surface"),
        _("Warm surface"), _("Warm above surface"),
        _("Cold surface"), _("Cold above surface"), _("Occlusion"),
        _("Instability line"), _("Intertropical"), _("Convergence line")
    };
    return(wxString(tab[index]));
}

wxString IACFrontalSystem::GetCharacteristic( size_t index ) const
{
    static const wxChar* (tab[])=
    {
        wxEmptyString, _("frontal area decreasing"), _("little change"), _("frontal area increasing"),
        _("intertropical"), _("forming"), _("quasistationary"),
        _("with waves"), _("diffuse"), _("strong, increasing")
    };
    return(wxString(tab[index]));
}

wxString IACFrontalSystem::GetIntensity( void ) const
{
    static const wxChar* (tab[])=
    {
        wxEmptyString,
        _("weak, decreasing"), _("weak, no change"), _("weak, increasing"),
        _("moderate, decreasing"), _("moderate, no change"), _("moderate, increasing"),
        _("strong, decreasing"), _("strong, no change"), _("strong, increasing")
    };
    if( m_int > 0 )
    {
        return(GetTab(tab, m_int));
    }
    else
    {
        return wxEmptyString;
    }
}

bool IACFrontalSystem::Draw( wxDC *dc, PlugIn_ViewPort *vp, TexFont &numfont, TexFont &sysfont )
{
    bool hasDrawn = false;
    wxColour colour;
    if( m_type == WARM_SURFACE || m_type == WARM_ABOVE_SURFACE )
        GetGlobalColor ( _T ( "URED" ), &colour );
    else if ( m_type == COLD_SURFACE || m_type == COLD_ABOVE_SURFACE )
        GetGlobalColor ( _T ( "BLUE3" ), &colour );
    else if ( m_type == OCCLUSION )
        GetGlobalColor ( _T ( "CHMGD" ), &colour );
    else
        GetGlobalColor ( _T ( "DASHN" ), &colour );

    if( dc )
    {
        wxPen pen( colour, FRONT_WIDTH);
        dc->SetPen(pen);
        dc->SetBrush(*wxTRANSPARENT_BRUSH);
        hasDrawn = DrawPositions( dc, vp );
    }
    else
    {
        m_isoLineColor = colour;
        m_isoLineWidth = wxMax(FRONT_WIDTH, GL_MIN_LINE_WIDTH);
        hasDrawn = DrawPositions( dc, vp );
    }
    return hasDrawn;
}

wxString IACTropicalSystem::GetType( size_t index ) const
{
    static const wxChar* (tab[])=
    {
        _("Intertropical convergence zone"), _("Shear line"),
        _("Line or Zone of convergence"), _("Axis of doldrum belt"),
        _("Through in westerlies"), _("Through in easterlies"), _("LOW area"),
        _("Surge line"), _("Divergence zone"), _("Tropical cyclone")
    };
    return(GetTab(tab,index));
}

wxString IACTropicalSystem::GetShortType( size_t index ) const
{
    static const wxChar* (tab[])=
    {
        _T("ICZ"), _T("SL"), _T("LC"), _T("ADB"), _T("TW"),
        _T("TE"), _T("TL"), _T("SU"), _T("LD"), _T("TC")
    };
    return(wxString(tab[index]));
}

wxString IACTropicalSystem::GetCharacteristic( size_t index ) const
{
    static const wxChar* (tab[])=
    {
        wxEmptyString, _("diffuse"), _("sharply defines"), _("quasistationary"),
        _("existance certain"), _("existance uncertain"), _("formation expected"),
        _("position certain"), _("position uncertain"), _("movement doubtful")
    };
    return(GetTab(tab, index));
}

wxString IACTropicalSystem::GetIntensity( void ) const
{
    static const wxChar* (tab[])=
    {
        wxT("-"), _("weak, decreasing"), _("weak, no change"), _("weak, increasing"),
        _("moderate, decreasing"), _("moderate, no change"), _("moderate, increasing"),
        _("strong, decreasing"), _("strong, no change"), _("strong, increasing")
    };
    if( m_int > 0 )
    {
        return(GetTab(tab, m_int));
    }
    else
    {
        return wxEmptyString;
    }
}

wxString IACTropicalSystem::GetValue( void ) const
{
    wxString t;
    if( m_val >= 0 )
    {
        t.Printf(wxT("%4u hPa"),m_val);
    }
    return t;
}

bool IACTropicalSystem::Draw( wxDC *dc, PlugIn_ViewPort *vp, TexFont &numfont, TexFont &sysfont )
{
    bool hasDrawn = false;
    if( dc )
    {
        wxColour colour;
        GetGlobalColor ( _T ( "YELO1" ), &colour );
        wxPen pen( colour, FRONT_WIDTH);
        dc->SetPen(pen);
        dc->SetBrush(*wxTRANSPARENT_BRUSH);
    }
    else
    {
        GetGlobalColor ( _T ( "YELO1" ), &m_isoLineColor );
        m_isoLineWidth = wxMax(FRONT_WIDTH, GL_MIN_LINE_WIDTH);
//        hasDrawn = DrawPositions( dc, vp );  // Not used
    }
    hasDrawn = DrawPositions( dc, vp );
    if( dc )
    {
        if( m_positions.Count() > 0 )
        {
            GeoPoint &Pos = m_positions[0];
            if( PointInLLBox(vp, Pos.x, Pos.y) )
            {
                wxPoint p;
                GetCanvasPixLL(vp, &p, Pos.y, Pos.x);
                wxColour colour;
                wxString msg1 = GetShortType(m_type);
                if( !msg1.IsEmpty() )
                {
                    hasDrawn = true;
                    GetGlobalColor ( _T ( "SNDG1" ), &colour );
                    dc->SetTextForeground( colour );
                    wxFont sfont = dc->GetFont();
                    
                    wxFont *font1 = wxTheFontList->FindOrCreateFont ( SYSTEMS_FONT_SIZE,
                                                                     wxFONTFAMILY_ROMAN, wxNORMAL, wxFONTWEIGHT_BOLD,
                                                                     FALSE, wxString ( _T ( "Arial" ) ) );
                    dc->SetFont(*font1);
                    wxSize s1 = dc->GetTextExtent(msg1);
                    dc->DrawText(msg1, p.x - (s1.GetWidth() / 2), p.y - (s1.GetHeight() / 2));
                    dc->SetFont(sfont);
                }
            }
        }
    }
    else
    {
        if( m_positions.GetCount() > 0 )
        {
            GeoPoint &Pos = m_positions[0];
            if( PointInLLBox(vp, Pos.x, Pos.y) )
            {
                wxPoint p;
                GetCanvasPixLL(vp, &p, Pos.y, Pos.x);
                wxColour colour;
                wxString msg1 = GetShortType(m_type);
                if( !msg1.IsEmpty() )
                {
                    hasDrawn = true;
                    glEnable( GL_BLEND );
                    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
                    
                    wxPoint p;
                    GetCanvasPixLL(vp, &p, Pos.y, Pos.x);
                    int w, h;
                    sysfont.GetTextExtent(msg1, &w, &h);
                    
                    int xd = p.x - (w / 2);
                    int yd = p.y - (h / 2);
                    
                    GetGlobalColor ( _T ( "SNDG1" ), &colour );
                    glColor3ub(colour.Red() , colour.Green(), colour.Blue());
                    
                    glEnable( GL_TEXTURE_2D );
                    sysfont.RenderString(msg1, xd, yd);
                    glDisable( GL_TEXTURE_2D );
                    glDisable( GL_BLEND );
                }
            }
        }
    }
    return hasDrawn;
}

wxString IACIsobarSystem::ToString( bool includePosition ) const
{
    wxString t;
    t.Printf( _("Isobar (%4uhPa):\n"), m_val );
    if( includePosition )
    {
        t.Append(PositionsToString());
    }
    return t;
}

bool IACIsobarSystem::Draw( wxDC *dc, PlugIn_ViewPort *vp, TexFont &numfont, TexFont &sysfont )
{
    bool hasDrawn = false;
    // draw isobar text at a random position of the isoline to prevent
    // text clutter. Getting the random positiuon must be before
    // "if" because otherwise the positions depend on the viewport
    int randomPositionIndex = rand() % m_positions.Count();

    if( dc )
    {
        wxColour colour;
        GetGlobalColor ( _T ( "CHBLK" ), &colour );
        wxPen pen( colour, ISOBAR_WIDTH);
        dc->SetPen(pen);
        dc->SetBrush(*wxTRANSPARENT_BRUSH);

        hasDrawn = DrawPositions( dc, vp );
        if( hasDrawn )
        {
            GeoPoint &Pos = m_positions[randomPositionIndex];
            if( PointInLLBox(vp, Pos.x, Pos.y) )
            {
                wxPoint p;
                GetCanvasPixLL( vp, &p, Pos.y, Pos.x );
                dc->SetTextForeground( colour );
                wxFont *font = wxTheFontList->FindOrCreateFont ( NUMBERS_FONT_SIZE,
                        wxFONTFAMILY_SWISS, wxNORMAL,  wxFONTWEIGHT_NORMAL,
                        FALSE, wxString ( _T ( "Arial" ) ) );
                dc->SetFont(*font);
                wxString msg;
                msg.Printf(wxT("%u"), m_val);
                wxSize s1 = dc->GetTextExtent(msg);
                dc->DrawText( msg, p.x - (s1.GetWidth() / 2),p.y - (s1.GetHeight()) );
            }
        }
    }
    else
    {
        GetGlobalColor ( _T ( "CHBLK" ), &m_isoLineColor );
        m_isoLineWidth = wxMax(ISOBAR_WIDTH, GL_MIN_LINE_WIDTH);
        hasDrawn = DrawPositions( dc, vp );
        if( hasDrawn )
        {
            wxColour colour;
            GetGlobalColor ( _T ( "CHBLK" ), &colour );
            GeoPoint &Pos = m_positions[randomPositionIndex];
            wxPoint p;
            GetCanvasPixLL( vp, &p, Pos.y, Pos.x );
            wxString msg;
            msg.Printf(wxT("%u"), m_val);
            int w, h;
            glEnable( GL_BLEND );
            glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
            glEnable( GL_TEXTURE_2D );
            glColor3ub(colour.Red() , colour.Green(), colour.Blue());
            numfont.GetTextExtent(msg, &w, &h);
            numfont.RenderString(msg, p.x - (w / 2), p.y - (h / 2));
            glDisable( GL_TEXTURE_2D );
            glDisable( GL_BLEND );
        }
    }
    return hasDrawn;
}
