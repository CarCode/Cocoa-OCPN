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
#include <wx/arrimpl.cpp> // this is a magic incantation which must be done!

#include "iacfile.h"

WX_DEFINE_OBJARRAY(IACSystems);
WX_DEFINE_OBJARRAY(IACPressureSystems);
WX_DEFINE_OBJARRAY(IACFrontalSystems);
WX_DEFINE_OBJARRAY(IACIsobarSystems);
WX_DEFINE_OBJARRAY(IACTropicalSystems);
WX_DEFINE_OBJARRAY(GeoPoints);

// Is the given point in the vp ??
// because of a the vp-implementation we need
// to check the latitude three times! (+-360) 
static bool PointInLLBox(PlugIn_ViewPort *vp, double x, double y)
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
IACFile::IACFile(void){
      Invalidate();
}

IACFile::IACFile(wxInputStream &stream){
      IACFile();
      m_isok = Read(stream);
      }

IACFile::~IACFile(void){
      }

void IACFile::Invalidate(void){
      m_tokens.Clear();
      m_tokensI=0;
      m_isok=false;
      m_RawData.Clear();
      m_pressure.Clear();
      m_frontal.Clear();
      m_isobars.Clear();
      m_tropical.Clear();


}
wxFileInputStream* IACFile::GetStream(wxString &filename){
      wxFileName file(filename);
      if(file.FileExists()
            && (file.GetSize() < IACMaxSize)){
            return new wxFileInputStream(filename); 
      }else{
            return NULL;
      }
}

bool IACFile::Read(wxInputStream &stream){
      bool isok=false; //true if minimum one token was read frim file
      Invalidate();
      wxString token;
      if (stream.IsOk()){
            for(;;){
                  token = ReadToken(stream);
                  if(! token.IsEmpty()){
                        m_tokens.Add(token);
                        isok=true;
                  }else{
                        break;
                  }
            }
      }
      if(isok){
            // decode tokens if some were found
            isok = Decode();
      }
      m_isok=isok;
      return isok;
}

wxString IACFile::ToString(void){
      wxString t;
      size_t i;
      t.Append(_T("IAC Fleet Code issued at ") + m_issueDate + _T("\n"));

      // Pressure Systems
      t.Append(wxT("\n\nPressure systems:"));
      t.Append(wxT(  "\n================="));
      for(i=0;i<m_pressure.Count();i++){
            t.Append(wxT("\n\n") + m_pressure[i].ToString());
      }
      t.Append(wxT("\n\nFrontal systems:"));
      t.Append(wxT(  "\n================"));
      for(i=0;i<m_frontal.Count();i++){
            t.Append(wxT("\n\n") + m_frontal[i].ToString());
      }
      t.Append(wxT("\n\nTropical systems:"));
      t.Append(wxT(  "\n================="));
      for(i=0;i<m_tropical.Count();i++){
            t.Append(wxT("\n\n") + m_tropical[i].ToString());
      }   

      t.Append(wxT("\n\nIsobars:"));
      t.Append(wxT(  "\n================="));
      for(i=0;i<m_isobars.Count();i++){
            t.Append(wxT("\n\n") + m_isobars[i].ToString());
      }
      return t;
}

bool IACFile::Decode(void){
      bool res=true;
      // init 
      m_tokensI=0;
      res=ReadHeader();
      if (res){
            res=ParseSections();
      }
      return res;
}

bool IACFile::ReadHeader(void){
      if(! tokenFind(_T("10001"),true).IsEmpty()){
            if(! tokenFind(_T("33388")).IsEmpty()){
                  // header found, read time
                  wxString timestr=tokenFind(_T("0????"));
                  if(! timestr.IsEmpty()){
                        // parse time, format 0DDHH, DD=date, HH=UTC hours
                        m_issueDate = _("Day ") + timestr.Mid(1,2) +
                                      _(" Hour ")+ timestr.Mid(3,2) +
                                      _T(" UTC");
                        return true;
                  }
            }
      }     
      return false;
}

bool IACFile::ParsePositions(IACSystem &sys){
      wxString token;
      wxString lasttoken;
      bool firsttime=true;
      
      for(;;){
            lasttoken=token;
            token=tokenFind();
            bool morepos=true;
            // heuristic about when the last postion is read
            // either on illegal octant "4" or by a step of more than 1
            // in octant
            int lastoct=TokenNumber(lasttoken,0,1);
            int oct    =TokenNumber(token,0,1);
            if(oct == 4){
                  morepos=false;
            }
            int diff = abs(lastoct-oct);
            if((diff > 1) && (diff < 8)){
                  morepos=false;
            }


            // lets be even more strict and limit positions to 
            // SW-Pacific
            if( !((oct==6) || (oct==7))){
                  morepos=false;
            }

            GeoPoint pos(token); // decode position
            // Position must be in the area bounded by 0S..35S and 120W/150E
            if((pos.y < -35.0) ||
               (pos.y > 0.0) ||
               ((pos.x > -120.0) && (pos.x < 150))
               ){
                     morepos=false;
            }

            if(!token.IsEmpty() && 
                  (firsttime || morepos ) 
                  ){
                  // ignore double entries following eachother
                  if(token != lasttoken){
                        GeoPoint pos(token);
                        sys.m_positions.Add(pos);
                  }else{
                        // we got two identical position entries
                        // meaning "no more"
                        // stop parsing positions, but read one
                        // more token to stay in sync
                        token=tokenFind();
                        break;
                  }
            }else{
                  break;
            }
            if(firsttime){
                  firsttime=false;
            }      
      }
      PushbackToken();
      if (token.IsEmpty()){
            return false;
      }else{
            return true;
      }
}

bool IACFile::ParseMovement(IACSystem &sys){
      wxString token;

      token=tokenFind();
      if(!token.IsEmpty()){
            // if invalid movement, ignore, push back
            int dir = 10 * TokenNumber(token,1,2);
            if(dir < 361){
                  
                  sys.m_movement =TokenNumber(token,0,1);
                  sys.m_direction= dir;
                  sys.m_speed    =TokenNumber(token,3,2);
                  return true;
            }else{
                  // invalif movement, push back
                  PushbackToken();
                  return false;
            }

      }else{
            return false;
      }
}

bool IACFile::ParseSections(void){
      wxString token;
      do {
            token=tokenFind(_T("999??"),true);
            if (!token.IsEmpty()){
                  //?? 00=>Pressure Systems 
                  //   11=>Frontal Systems
                  //   22=>Isobars
                  //   55=>Tropical Section
                  int section = TokenNumber(token,3,2);
                  switch(section){
                        case 0:
                              ParsePressureSection();
                              break;
                        case 11:
                              ParseFrontalSection();
                              break;
                        case 22:
                              ParseIsobarSection();
                              break;
                        case 55:
                              ParseTropicalSection();
                              break;
                        default:
                              break;
                  }
            }
      }while(!token.IsEmpty());
      return true;
}
bool IACFile::ParsePressureSection(void){
      wxString token;
      for(;;){            
            token=tokenFind(_T("8????"));
            if (!token.IsEmpty()){
                  // parse pressure system token
                  IACPressureSystem sys;
                  sys.m_type=TokenNumber(token,1,1);
                  sys.m_char=TokenNumber(token,2,1);
                  sys.m_val =TokenNumber(token,3,2);      
                  sys.m_int = -1;
                  // guess pressure offset 
                  if(sys.m_val > 50){
                        sys.m_val+=900;
                  }else{
                        sys.m_val += 1000;
                  }

                  ParsePositions(sys);
                  ParseMovement(sys);
                  m_pressure.Add(sys);
            }else{
                  PushbackToken();
                  break;
            }
      }
      return true; 
}
bool IACFile::ParseFrontalSection(void){
      wxString token;
      for(;;){            
            token=tokenFind(_T("66???"));
            if (!token.IsEmpty()){
                  // parse pressure system token
                  IACFrontalSystem sys;
                  sys.m_type=TokenNumber(token,2,1);
                  sys.m_val = -1;
                  sys.m_int =TokenNumber(token,3,1);
                  sys.m_char=TokenNumber(token,4,1);


                  ParsePositions(sys);
                  ParseMovement(sys);
                  m_frontal.Add(sys);
            }else{
                  // no more frontal systems.
                  PushbackToken();
                  break;
            }
      }
      return true;
}
bool IACFile::ParseIsobarSection(void){
      wxString token;
      for(;;){            
            token=tokenFind(_T("44???"));
            if (!token.IsEmpty()){
                  IACIsobarSystem sys;
                  sys.m_val =TokenNumber(token,2,3);
                  if(sys.m_val < 500){
                        sys.m_val+=1000;
                  }

                  // Position
                  ParsePositions(sys);
                  m_isobars.Add(sys);
            }else{
                  // no more frontal systems.
                  PushbackToken();
                  break;
            }
      }
      return true;
}
bool IACFile::ParseTropicalSection(void){
      wxString token;
      for(;;){            
            token=tokenFind(_T("55???"));
            if (!token.IsEmpty()){
                  // parse pressure system token
                  IACTropicalSystem sys;
                  sys.m_type=TokenNumber(token,2,1);
                  sys.m_int =TokenNumber(token,3,1);
                  sys.m_char=TokenNumber(token,4,1);

                  // tropical system token MAY be followed by 555PP pressure token
                  // in this case it is a tropical LOW or cyclone
                  token = tokenFind(_T("555??")); 
                  if(token.IsEmpty()){
                        // no, push back token - it is a position
                        PushbackToken();
                  }else{
                        sys.m_val = TokenNumber(token,3,2);
                        // guess pressure offset 
                        if(sys.m_val > 50){
                              sys.m_val+=900;
                        }else{
                              sys.m_val += 1000;
                        }
                  }
                  ParsePositions(sys);
                  ParseMovement(sys);
                  m_tropical.Add(sys);
            }else{
                  PushbackToken();
                  break;
            }
      }
      return true;
}

int IACFile::TokenNumber(wxString &token, size_t start, size_t end){
      unsigned long ul;
      if (token.Mid(start,end).ToULong(&ul)){
            //success
            return ul;
      }else{
            //failure
            return -1;
      }
}

wxString IACFile::tokenFind(const wxChar* match,bool skip){
      while(m_tokensI < m_tokens.GetCount()){
            wxString token=m_tokens[m_tokensI++];
            if( token.Matches(match)){
                  // found
                  return token;
            }
            if (!skip){
                  break;
            }
      }
      // not found
      return wxEmptyString;
}

void IACFile::PushbackToken(void){
      if (m_tokensI > 0){
            m_tokensI--;
      }
}

wxString IACFile::ReadToken(wxInputStream &file){
      // 0 = read chars until digit
      // 1 = read digits until no digit
      // 2 = token found
      wxString token = wxEmptyString;
#ifdef __WXOSX__
      char c;
#else
    int c;
#endif
      int mode = 0;

      while((file.IsOk()) && (mode != 2) ){
            c=file.GetC();
#ifdef __WXOSX__
          if( c != wxEOF) m_RawData.Append(c);
#else
            m_RawData.Append(c);
#endif
            if( c != wxEOF){
                  switch(mode){
                        case 0:
                              if (isdigit(c)){
                                    token.Append(c);
                                    mode = 1;
                              }
                              break;
                        case 1:     
                              if (isdigit(c)){

                                  token.Append(c);

                              }else{
                                    if(token.Len() == 5 ){
                                          // token found!!
                                          mode = 2;
                                    }else{
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
      if (mode != 2){
            token.Empty();
      }

      return token;
}

// draw the fleet code data
// returns true if anything was drawn
#ifdef __WXOSX__
bool IACFile::Draw(wxDC *pmdc, PlugIn_ViewPort *vp){
#else
bool IACFile::Draw(wxMemoryDC *pmdc, PlugIn_ViewPort *vp){
#endif
      bool retval = false;
      // draw only if file was successfully read and decoded
      if(IsOk()){
            // This magic initialisation to the random numer generator
            // ensures that the (random) positions where some text
            // is written does not change between several calls to "Draw"
            // I love this kind of crazy code - it is like in the good
            // old days of programming where everything was allowed.
            srand(77); 
            retval |= DrawSystems(pmdc,vp,(IACSystems &)m_pressure);
            retval |= DrawSystems(pmdc,vp,(IACSystems &)m_frontal);
            retval |= DrawSystems(pmdc,vp,(IACSystems &)m_isobars);
            retval |= DrawSystems(pmdc,vp,(IACSystems &)m_tropical);
      }
      return retval;
}
#ifdef __WXOSX__
bool IACFile::DrawSystems(wxDC *pmdc, PlugIn_ViewPort *vp,IACSystems &iacsystem){    
#else
bool IACFile::DrawSystems(wxMemoryDC *pmdc, PlugIn_ViewPort *vp,IACSystems &iacsystem){
#endif
      bool retval = false;
      // loop over all systems
      for(size_t Index = 0; Index < iacsystem.GetCount(); Index++){
            retval |= iacsystem[Index].Draw(pmdc,vp);     
      }
      return retval;
}

IACSystem * IACFile::FindSystem(GeoPoint &pos,double deviation){
      IACSystem *pIACSystem=NULL;
      pIACSystem = FindSystem((IACSystems&)m_pressure,pos,deviation);
      if(NULL == pIACSystem){
            pIACSystem = FindSystem((IACSystems&)m_frontal,pos,deviation);
            if(NULL == pIACSystem){
                  pIACSystem = FindSystem((IACSystems&)m_tropical,pos,deviation);
            }
      }
      return pIACSystem;
}

IACSystem * IACFile::FindSystem(IACSystems &systems,GeoPoint &pos,double deviation){
      IACSystem * pSystem = NULL;
      for (size_t i=systems.GetCount();i!=0;i--){
            if(systems[i-1].FindAtPos(pos,deviation)){
                  // found system
                  pSystem = &(systems[i-1]);
            }
      }
      return pSystem;
}


//---------------------------
// implementatin of GeoPoint
//---------------------------
const double GeoPoint::INVALID_KOORD=9999.9;


// initialize from IAC lat/lon token
void GeoPoint::Set(wxString &token){
      if(token.Len() == 5){
            int oct=IACFile::TokenNumber(token,0,1);
            int lat=IACFile::TokenNumber(token,1,2);
            int lon=IACFile::TokenNumber(token,3,2);
            int ns=(oct>3)?-1:+1; // sign for south(-1) or north(+1)
            lat *= ns;            // lat is now in degrees


            // eliminate north/south
            if(oct > 4){
                  oct -= 5;
            }
            switch(oct){
                  case 0: // West 0..90
                        lon = - lon;
                        break;
                  case 1: // West 90..180
                        if (lon < 90){
                              lon = - (100 + lon);
                        }else{
                              lon = - lon;
                        }
                        break;
                  case 2: // East 90 .. 180
                        if (lon < 90){
                              lon = 100 + lon;
                        }
                        break;
                  case 3: // East 0..90
                        break;
            }
            Set(double(lon),double(lat));
      }else{
            Set(); // invalid token. set to invalid koordinates
      }
}

wxString GeoPoint::ToString(void){
      wxString t;
      // latitude
      unsigned int latdeg=floor(fabs(y));
      wxChar       lats  = (y<0)?'S':'N';
      unsigned int londeg=floor(fabs(x));
      wxChar       lons  = (x<0)?'W':'E';
      t.Printf(wxT("%02u%c %03u%c"),latdeg,lats,londeg,lons);
      return t;
}
bool GeoPoint::MatchPosition(GeoPoint &refPos, double deviation){
      if( (fabs(x-refPos.x) <= deviation) &&
          (fabs(y-refPos.y) <= deviation)){
            return true;
      }else{
            return false;
      }
}


//-----------------------------
// implementation of IACSystem
//-----------------------------

IACSystem::IACSystem(void):
m_positions(),
m_type(-1),
m_char(-1),
m_val(-1),
m_int(-1),
m_movement(99),
m_direction(99),
m_speed(99)
{
}
#ifdef __WXOSX__
bool IACSystem::Draw(wxDC *pmdc, PlugIn_ViewPort *vp){
#else
bool IACSystem::Draw(wxMemoryDC *pmdc, PlugIn_ViewPort *vp){
#endif
      bool hasDrawn=false;
      if (m_positions.GetCount() > 0){
            GeoPoint &Pos = m_positions[0];
            if(PointInLLBox(vp, Pos.x, Pos.y)){
                  wxPoint p;
                  GetCanvasPixLL(vp, &p, Pos.y, Pos.x);
                  wxColour colour;
                  wxString msg1=GetShortType(m_type);
                  if(!msg1.IsEmpty()){
                        hasDrawn=true;
                        GetGlobalColor ( _T ( "UBLCK" ), &colour );
                        pmdc->SetTextForeground( colour );
                        wxFont sfont = pmdc->GetFont();

                        wxFont *font1 = wxTheFontList->FindOrCreateFont ( 15, 
                              wxFONTFAMILY_SWISS,wxNORMAL,  wxFONTWEIGHT_BOLD,
                              FALSE, wxString ( _T ( "Arial" ) ) ); 
                        pmdc->SetFont(*font1);
                        wxSize s1 = pmdc->GetTextExtent(msg1);
                        pmdc->DrawText(msg1, p.x - (s1.GetWidth()/2),p.y - (s1.GetHeight()/2));
                        wxFont *font2 = wxTheFontList->FindOrCreateFont ( 8, 
                              wxFONTFAMILY_SWISS,wxITALIC,  wxFONTWEIGHT_NORMAL,
                              FALSE, wxString ( _T ( "Arial" ) ) ); 
                        pmdc->SetFont(*font2);
                        wxString msg2=GetValue();
                        if (!msg2.IsEmpty()){
                              wxSize s2 = pmdc->GetTextExtent(msg2);
                              pmdc->DrawText(msg2,
                                    p.x - (s2.GetWidth()/2),
                                    p.y + (s1.GetHeight()/2) + (s2.GetHeight()/2));
                        
                              pmdc->SetFont(sfont);
                        }
                  }
            }
      }
      return hasDrawn;
}
bool IACSystem::DrawPositions(wxMemoryDC *pmdc, PlugIn_ViewPort *vp){
      bool hasDrawn = false;
      for( size_t pointIndex=0; pointIndex <(m_positions.GetCount()-1) ;pointIndex++){
            GeoPoint &startP = m_positions[pointIndex];
            GeoPoint &endP   = m_positions[pointIndex+1];
            if(PointInLLBox(vp, startP.x, startP.y) || 
               PointInLLBox(vp, endP.x, endP.y)
               ){
                  wxPoint startpl;
                  wxPoint endpl;
                  GetCanvasPixLL(vp, &startpl, startP.y, startP.x);
                  GetCanvasPixLL(vp, &endpl, endP.y, endP.x);
                  pmdc->DrawLine(startpl, endpl);
                  hasDrawn = true;
            }
      }
      return hasDrawn;
}


wxString IACSystem::GetTab(const wxChar*(tab[10]),size_t index)const{
      if(index < 10){
            return wxString(tab[index]);
      }else{
            return wxEmptyString;
      }
}

wxString IACSystem::GetMovement(void) const{
      static const wxChar* (tab[])={
            wxT("-"),wxT("stationary"),wxT("little change"),wxT("stopping"),wxT("retarding"),
            wxT("curving to left"),wxT("recurving"),wxT("accelerating"),
            wxT("curving to right"),wxT("expected to recurve")
      };
      wxString t;
      if(m_movement < 10){
            // valid movement
            t = wxT("Movement: ") + GetTab(tab,m_movement); 
            if(m_movement != 1){ // not stationary, add dir and speed
                  wxString hlp;
                  hlp.Printf(wxT(", %uDeg "),m_direction);
                  t.Append(hlp);
                  if(m_speed >= 99){
                        t.Append(wxT("with unknown speed"));
                  }else{
                        hlp.Printf(wxT("with %uknots"),m_speed);
                        t.Append(hlp);
                  }
            }
      }
      return t;
}

wxString IACSystem::ToString(bool includePosition) const{
      wxString t;
      t.Append(GetType(m_type) + wxT(" "));
      if(!GetValue().IsEmpty()){
            t.Append(wxT("(") + GetValue() + wxT(") "));
      }
      if(!GetIntensity().IsEmpty()){
            t.Append(wxT(" ") + GetIntensity() + wxT(" "));
      }
      t.Append(GetCharacteristic(m_char) + wxT("\n"));
      if(includePosition){
            t.Append(PositionsToString() + wxT("\n"));
      }
      t.Append(GetMovement());
      return t;
}

bool IACSystem::FindAtPos(GeoPoint &pos, double deviation){
      bool found=false;
      // loop over positions and see if it matches 
      for (size_t i=m_positions.GetCount();i!=0;i--){
            if (m_positions[i-1].MatchPosition(pos,deviation)){
                  found=true;
                  break;
            }
            
      }
      return found;
}


wxString IACSystem::GetType(size_t index) const{
      return(wxEmptyString);
}
wxString IACSystem::GetShortType(size_t index) const{
      return(wxEmptyString);
}
wxString IACSystem::GetCharacteristic(size_t index) const{
      return(wxEmptyString);
}

wxString IACSystem::PositionsToString(void) const{
      wxString t;
      for (size_t i=0;i<m_positions.Count();i++){
            if( i != 0){
                  t.Append(wxT(" : "));
            }
            t.Append(m_positions[i].ToString());
      }
      return t;
}

wxString IACPressureSystem::GetValue(void) const{
      wxString t;
      if(m_val >=0){
            t.Printf(wxT("%4u hPa"),m_val);
      }
      return t;
}
wxString IACPressureSystem::GetType(size_t index) const{
      static const wxChar* (tab[])={
            wxT("Complex Low"),wxT("Low"),wxT("Secondary Low"),wxT("Trough"),wxT("Wave"),
            wxT("High"),wxT("Uniform pressure"),wxT("Ridge"),wxT("Col"),wxT("Tropical Storm")
      };
      return(wxString(tab[index]));
}
wxString IACPressureSystem::GetShortType(size_t index) const{
      static const wxChar* (tab[])={
            wxT("L"),wxT("L"),wxT("L"),wxT("T"),wxT("W"),
            wxT("H"),wxT("U"),wxT("R"),wxT("C"),wxT("TS")
      };
      return(wxString(tab[index]));
}
wxString IACPressureSystem::GetCharacteristic(size_t index) const{
      static const wxChar* (tab[])={
            wxT(""),wxT("weakening"),wxT("little change"),wxT("intensifying"),wxT("complex"),
            wxT("forming"),wxT("weakening but not disappearing"),wxT("general rise"),
            wxT("general fall"),wxT("position doubtful")
      };
      return(wxString(tab[index]));
}

wxString IACFrontalSystem::GetType(size_t index) const{
      static const wxChar* (tab[])={
            wxT("Quasistationary at surface"),wxT("Quasistationary above surface"),
            wxT("Warm surface"),wxT("Warm above surface"),
            wxT("Cold surface"),wxT("Cold above surface"),wxT("Occlusion"),
            wxT("Instability line"),wxT("Intertropical"),wxT("Convergence line")
      };
      return(wxString(tab[index]));
}
wxString IACFrontalSystem::GetCharacteristic(size_t index) const{
      static const wxChar* (tab[])={
            wxT(""),wxT("frontal area decreasing"),wxT("little change"),wxT("frontal area increasing"),
            wxT("intertropical"),wxT("forming"),wxT("quasistationary"),
            wxT("with waves"),wxT("diffuse"),wxT("strong, increasing")
      };
      return(wxString(tab[index]));
}
wxString IACFrontalSystem::GetIntensity(void) const{
      static const wxChar* (tab[])={
            wxT(""),
            wxT("weak, decreasing"),wxT("weak, no change"),wxT("weak, increasing"),
            wxT("moderate, decreasing"),wxT("moderate, no change"),wxT("moderate, increasing"),
            wxT("strong, decreasing"),wxT("strong, no change"),wxT("strong, increasing")
      };
      if (m_int>0){
            return(GetTab(tab,m_int));
      }else{
            return wxEmptyString;
      }
}
bool IACFrontalSystem::Draw(wxMemoryDC *pmdc, PlugIn_ViewPort *vp){
      wxColour colour;
      GetGlobalColor ( _T ( "BLUE3" ), &colour );
      wxPen pen( colour, 2);
      pmdc->SetPen(pen);
      pmdc->SetBrush(*wxTRANSPARENT_BRUSH);
      bool hasDrawn=DrawPositions(pmdc,vp);
      return hasDrawn;
}

wxString IACTropicalSystem::GetType(size_t index) const{
      static const wxChar* (tab[])={
            wxT("Intertropical convergence zone"),wxT("Shear line"),
            wxT("Line or Zone of convergence"),wxT("Axis of doldrum belt"),
            wxT("Through in westerlies"),wxT("Through in easterlies"),wxT("LOW area"),
            wxT("Surge line"),wxT("Divergence zone"),wxT("Tropical cyclone")
      };
      return(GetTab(tab,index));
}

wxString IACTropicalSystem::GetShortType(size_t index) const{
      static const wxChar* (tab[])={
            wxT(""),wxT("SL"),wxT(""),wxT(""),wxT(""),
            wxT(""),wxT("TL"),wxT(""),wxT(""),wxT("TC")
      };
      return(wxString(tab[index]));
}
wxString IACTropicalSystem::GetCharacteristic(size_t index) const{
      static const wxChar* (tab[])={
            wxT(""),wxT("diffuse"),wxT("sharply defines"),wxT("quasistationary"),
            wxT("existance certain"),wxT("existance uncertain"),wxT("firmation expected"),
            wxT("position certain"),wxT("position uncertain"),wxT("monement doubtful")
      };
      return(GetTab(tab,index));
}
wxString IACTropicalSystem::GetIntensity(void) const{
      static const wxChar* (tab[])={
            wxT("-"),wxT("weak, decreasing"),wxT("weak, no change"),wxT("weak, increasing"),
            wxT("moderate, decreasing"),wxT("moderate, no change"),wxT("moderate, increasing"),
            wxT("strong, decreasing"),wxT("strong, no change"),wxT("strong, increasing")
      };
      if (m_int>0){
            return(GetTab(tab,m_int));
      }else{
            return wxEmptyString;
      }
}
wxString IACTropicalSystem::GetValue(void) const{
      wxString t;
      if (m_val >=0){
            t.Printf(wxT("%4u hPa"),m_val);
      }
      return t;
}

bool IACTropicalSystem::Draw(wxMemoryDC *pmdc, PlugIn_ViewPort *vp){
      wxColour colour;
      GetGlobalColor ( _T ( "YELO1" ), &colour );
      wxPen pen( colour, 2);
      pmdc->SetPen(pen);
      pmdc->SetBrush(*wxTRANSPARENT_BRUSH);
      bool hasDrawn=DrawPositions(pmdc,vp);

      hasDrawn |= IACSystem::Draw(pmdc,vp);
      return hasDrawn;
}

wxString IACIsobarSystem::ToString(bool includePosition) const{
      wxString t;
      t.Printf(wxT("Isobar (%4uhPa):\n"),m_val);
      if(includePosition){
            t.Append(PositionsToString());     
      }
      return t;
}
bool IACIsobarSystem::Draw(wxMemoryDC *pmdc, PlugIn_ViewPort *vp){
     
      wxColour colour;
      GetGlobalColor ( _T ( "GREY1" ), &colour );
      wxPen pen( colour, 1);
      pmdc->SetPen(pen);
      pmdc->SetBrush(*wxTRANSPARENT_BRUSH);

      // draw isobar text at a random position of the isoline to prevent
      // text clutter. Getting the random positiuon must be before 
      // "if" because otherwise the positions depend on the viewport
      int randomPositionIndex = rand() % m_positions.Count();
      bool hasDrawn=DrawPositions(pmdc,vp);
      if(hasDrawn){
            GeoPoint &Pos = m_positions[randomPositionIndex];
            if(PointInLLBox(vp, Pos.x, Pos.y)){
                  wxPoint p;
                  GetCanvasPixLL(vp, &p, Pos.y, Pos.x);
                  pmdc->SetTextForeground( colour );
                  wxFont *font = wxTheFontList->FindOrCreateFont ( 8, 
                        wxFONTFAMILY_SWISS,wxNORMAL,  wxFONTWEIGHT_NORMAL,
                        FALSE, wxString ( _T ( "Arial" ) ) ); 
                  pmdc->SetFont(*font);
                  wxString msg;
                  msg.Printf(wxT("%u"),m_val);
                  wxSize s1 = pmdc->GetTextExtent(msg);
                  pmdc->DrawText(msg, p.x - (s1.GetWidth()/2),p.y - (s1.GetHeight()));
            }
      }
      return hasDrawn;
}
