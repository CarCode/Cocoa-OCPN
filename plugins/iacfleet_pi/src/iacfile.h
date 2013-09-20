/***************************************************************************
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

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include <wx/stream.h>
#include <wx/wfstream.h>
#include <wx/dynarray.h>
#include <wx/object.h>
#include <wx/gdicmn.h>

#include "../../../include/ocpn_plugin.h"

class GeoPoint:public wxRealPoint{
public:
      static const double INVALID_KOORD;
      GeoPoint(double lon=INVALID_KOORD, double lat=INVALID_KOORD){Set(lon,lat);}
      GeoPoint(wxString &token){Set(token);}
      void Set(wxString &token);
      void Set(double lon=INVALID_KOORD, double lat=INVALID_KOORD){x=lon;y=lat;}
      wxString ToString(void);
      bool MatchPosition(GeoPoint &refPos,double deviation);
};

WX_DECLARE_OBJARRAY(GeoPoint,GeoPoints);

class IACSystem{
public:
      IACSystem(void);
      //void SetMovement(unsigned int m, unsigned int d, unsigned int s);
#ifdef __WXOSX__
      wxString ToString(bool includePosition=true) const;
      bool Draw(wxDC *pmdc, PlugIn_ViewPort *vp);
#else
      virtual wxString ToString(bool includePosition=true) const;
      virtual bool Draw(wxMemoryDC *pmdc, PlugIn_ViewPort *vp);
#endif
      wxString GetTab(const wxChar*(tab[]),size_t index) const;
      wxString PositionsToString(void) const;
      bool DrawPositions(wxMemoryDC *pmdc, PlugIn_ViewPort *vp);
#ifdef __WXOSX__
      bool FindAtPos(GeoPoint &pos, double deviation);
#else
      virtual bool FindAtPos(GeoPoint &pos, double deviation);
#endif
      GeoPoints   m_positions;
      int         m_type;
      int         m_char;
      int         m_val;
      int         m_int;
      unsigned int m_movement;  // 
      unsigned int m_direction; // in deg
      unsigned int m_speed;     // in knots, 99=unknown

private:
#ifdef __WXOSX__
      wxString GetType(size_t index) const;
      wxString GetShortType(size_t index) const;
      wxString GetCharacteristic(size_t index) const;
      wxString GetValue(void) const {return wxEmptyString;}
      wxString GetIntensity(void) const {return wxEmptyString;}
#else
      virtual wxString GetType(size_t index) const;
      virtual wxString GetShortType(size_t index) const;
      virtual wxString GetCharacteristic(size_t index) const;
      virtual wxString GetValue(void) const {return wxEmptyString;}
      virtual wxString GetIntensity(void) const {return wxEmptyString;}
#endif
      wxString GetMovement(void) const;
};

class IACPressureSystem: public IACSystem{
private:
      wxString GetType(size_t index) const;
      wxString GetShortType(size_t index) const;
      wxString GetCharacteristic(size_t index) const;
      wxString GetValue(void) const;
};

class IACFrontalSystem: public IACSystem{
public:
private:
      wxString GetType(size_t index) const;
      wxString GetCharacteristic(size_t index) const;
      wxString GetIntensity(void) const;
      bool Draw(wxMemoryDC *pmdc, PlugIn_ViewPort *vp);
};

class IACIsobarSystem: public IACSystem{
public:
      wxString ToString(bool includePosition=true) const;
      bool Draw(wxMemoryDC *pmdc, PlugIn_ViewPort *vp);
};

class IACTropicalSystem: public IACSystem{
public:
private:
      wxString GetType(size_t index) const;
      wxString GetShortType(size_t index) const;
      wxString GetIntensity(void) const;
      wxString GetCharacteristic(size_t index) const;
      wxString GetValue(void) const;
      bool Draw(wxMemoryDC *pmdc, PlugIn_ViewPort *vp);
};


WX_DECLARE_OBJARRAY(IACSystem, IACSystems);
WX_DECLARE_OBJARRAY(IACPressureSystem, IACPressureSystems);
WX_DECLARE_OBJARRAY(IACFrontalSystem, IACFrontalSystems);
WX_DECLARE_OBJARRAY(IACIsobarSystem, IACIsobarSystems);
WX_DECLARE_OBJARRAY(IACTropicalSystem, IACTropicalSystems);

class IACFile{
public:
      IACFile();
      IACFile(wxInputStream &stream); 
      ~IACFile();

      wxString ToString(void);
      wxString GetIssueDate(void){return m_issueDate;}

      bool Read(wxInputStream &stream);
      bool IsOk(void){return m_isok;}
#ifdef __WXOSX__
    bool Draw(wxDC *pmdc, PlugIn_ViewPort *vp);
#else
      bool Draw(wxMemoryDC *pmdc, PlugIn_ViewPort *vp);
#endif
      static int TokenNumber(wxString &token, size_t start, size_t end);
      static wxFileInputStream* GetStream(wxString &filename);
      wxString &GetRawData(void){return m_RawData;}
      static const size_t IACMaxSize=10000;

      void Invalidate(void);
      IACSystem * FindSystem(GeoPoint &pos,double deviation);



private:
      IACSystem * FindSystem(IACSystems &systems,GeoPoint &pos,double deviation);
      wxString ReadToken(wxInputStream &file);
      wxString tokenFind(const wxChar* match=wxT("?????"),bool skip=false);
      void PushbackToken(void);
      bool Decode(void);

      bool Parse(void);
      bool ReadHeader(void);
      bool ParsePositions(IACSystem &sys);
      bool ParseMovement(IACSystem &sys);
      bool ParseSections(void);
      bool ParsePressureSection(void);
      bool ParseFrontalSection(void);
      bool ParseIsobarSection(void);
      bool ParseTropicalSection(void);
      GeoPoint ParsePosition(void);
#ifdef __WXOSX__
    bool DrawSystems(wxDC *pmdc, PlugIn_ViewPort *vp,IACSystems &iacsystem);
#else
      bool DrawSystems(wxMemoryDC *pmdc, PlugIn_ViewPort *vp,IACSystems &iacsystem);
#endif

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

      };
#endif