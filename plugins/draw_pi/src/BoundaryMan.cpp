/***************************************************************************
 * 
 * Project:  OpenCPN
 * Purpose:  Boundary Manager
 * Author:   Jon Gough
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


#include "BoundaryMan.h"
#include "Boundary.h"
#include "BoundaryPoint.h"
#include "PointMan.h"
#include "ocpn_draw_pi.h"
#ifdef __WXOSX__
#include <math.h>
#else
#include "math.h"
#endif

extern PathList         *g_pPathList;
extern BoundaryList     *g_pBoundaryList;
extern ODPointList      *g_pODPointList;
extern PointMan         *g_pODPointMan;



wxString BoundaryMan::FindPointInBoundary( double lat, double lon, int type )
{
    wxBoundaryListNode *boundary_node = g_pBoundaryList->GetFirst();
    Boundary *pboundary = NULL;
    bool bInPoly = false;
    ODPoint *pop;
    int k = 0;
    wxString l_GUID = wxEmptyString;
    
    while( boundary_node ) {
        bool    l_bNext = false;
        pboundary = boundary_node->GetData();
        switch (type) {
            case ID_BOUNDARY_ANY:
                l_bNext = false;
                break;
            case ID_BOUNDARY_EXCLUSION:
                if(!pboundary->m_bExclusionBoundary) l_bNext = true;
                break;
            case ID_BOUNDARY_INCLUSION:
                if(!pboundary->m_bInclusionBoundary) l_bNext = true;
                break;
            case ID_BOUNDARY_NIETHER:
                if(pboundary->m_bExclusionBoundary || pboundary->m_bInclusionBoundary) l_bNext = true;
                break;
        }
        
        if(!l_bNext) {
            k++;
            int i, j;
            j = pboundary->m_pODPointList->GetCount();
            double *polyX;
            double *polyY;
            polyX = new double[j];
            polyY = new double[j];
            
            wxODPointListNode *OCPNpoint_node = ( pboundary->m_pODPointList )->GetFirst();
            wxODPointListNode *OCPNpoint_last_node = ( pboundary->m_pODPointList )->GetLast();
            i = 0;
            while( OCPNpoint_node ) {
                pop = OCPNpoint_node->GetData();
                polyX[i] = pop->m_lon;
                polyY[i] = pop->m_lat;
                i++;
                OCPNpoint_node = OCPNpoint_node->GetNext();           // next OD point
                if(OCPNpoint_node == OCPNpoint_last_node) break;
            }
            bInPoly = pointInPolygon(i, polyX, polyY, lon, lat);
            if(bInPoly) {
                l_GUID = pboundary->m_GUID;
                break;
            }
            delete [] polyX;
            delete [] polyY;
        }
        boundary_node = boundary_node->GetNext();                         // next boundary
    }
    
    if(bInPoly) return l_GUID;
    else return wxT("");
}

bool BoundaryMan::FindPointInBoundary( Boundary *pBoundary, double lat, double lon )
{
    bool bInPoly = false;
    int i, j;
    j = pBoundary->m_pODPointList->GetCount();
    double *polyX;
    double *polyY;
    polyX = new double[j];
    polyY = new double[j];
    
    wxODPointListNode *OCPNpoint_node = ( pBoundary->m_pODPointList )->GetFirst();
    wxODPointListNode *OCPNpoint_last_node = ( pBoundary->m_pODPointList )->GetLast();
    if(pBoundary->m_pODPointList->GetCount() > 3) {
        i = 0;
        while( OCPNpoint_node ) {
            ODPoint *pop = OCPNpoint_node->GetData();
            polyX[i] = pop->m_lon;
            polyY[i] = pop->m_lat;
            i++;
            OCPNpoint_node = OCPNpoint_node->GetNext();           // next OD point
            if(OCPNpoint_node == OCPNpoint_last_node) break;
        }
        bInPoly = pointInPolygon(i, polyX, polyY, lon, lat);
#ifdef __WXOSX__
    }
    delete [] polyX;
    delete [] polyY;
#else
        delete [] polyX;
        delete [] polyY;
    }
#endif
    return bInPoly;
}

bool BoundaryMan::FindPointInBoundary( wxString l_GUID, double lat, double lon )
{
    bool bInPoly = false;
    bool bBoundaryFound = false;
    int i;
    double *polyX = NULL;
    double *polyY = NULL;
    Boundary *pboundary = NULL;
    
    wxBoundaryListNode *pboundary_node = g_pBoundaryList->GetFirst();
    while( pboundary_node ) {
        pboundary = (Boundary *)pboundary_node->GetData();
        if(pboundary->m_GUID == l_GUID) {
            bBoundaryFound = true;
            break;
        }
        pboundary = (Boundary *)pboundary_node->GetNext();
    }
    if(pboundary && bBoundaryFound) {
        polyX = new double[pboundary->m_pODPointList->GetCount()];
        polyY = new double[pboundary->m_pODPointList->GetCount()];
        wxODPointListNode *OCPNpoint_node = ( pboundary->m_pODPointList )->GetFirst();
        wxODPointListNode *OCPNpoint_last_node = ( pboundary->m_pODPointList )->GetLast();
        i = 0;
        while( OCPNpoint_node ) {
            ODPoint *pop = OCPNpoint_node->GetData();
            polyX[i] = pop->m_lon;
            polyY[i] = pop->m_lat;
            i++;
            OCPNpoint_node = OCPNpoint_node->GetNext();           // next OD point
            if(OCPNpoint_node == OCPNpoint_last_node) break;
        }
        bInPoly = pointInPolygon(i, polyX, polyY, lon, lat);
    } 
    
    if(polyX) delete [] polyX;
    if(polyY) delete [] polyY;
    
    return bInPoly;
}

wxString BoundaryMan::FindPointInBoundaryPoint( double lat, double lon, int type )
{
    bool bInPoly = false;
    ODPoint *pop;
    wxString l_GUID = wxEmptyString;
    
    wxODPointListNode *ODpoint_node = g_pODPointMan->GetODPointList()->GetFirst();
    BoundaryPoint *l_pBoundaryPoint = NULL;
    while( ODpoint_node ) {
        bool l_bNext = false;
        pop = (ODPoint *) ODpoint_node->GetData();
        if(pop->m_sTypeString == wxT("Boundary Point")) {
            l_pBoundaryPoint = (BoundaryPoint *) ODpoint_node->GetData();
            switch (type) {
                case ID_BOUNDARY_ANY:
                    l_bNext = false;
                    break;
                case ID_BOUNDARY_EXCLUSION:
                    if(!l_pBoundaryPoint->m_bExclusionBoundaryPoint) l_bNext = true;
                    break;
                case ID_BOUNDARY_INCLUSION:
                    if(!l_pBoundaryPoint->m_bInclusionBoundaryPoint) l_bNext = true;
                    break;
                case ID_BOUNDARY_NIETHER:
                    if(!l_pBoundaryPoint->m_bExclusionBoundaryPoint && !l_pBoundaryPoint->m_bInclusionBoundaryPoint) l_bNext = true;
                    break;
            }
            if(l_pBoundaryPoint->m_bShowODPointRangeRings && l_pBoundaryPoint->m_iODPointRangeRingsNumber > 0 && !l_bNext) {
                double l_dRangeRingSize = l_pBoundaryPoint->m_iODPointRangeRingsNumber * l_pBoundaryPoint->m_fODPointRangeRingsStep;
                double brg;
                double l_dPointDistance;
                DistanceBearingMercator_Plugin( l_pBoundaryPoint->m_lat, l_pBoundaryPoint->m_lon, lat, lon, &brg, &l_dPointDistance );
                //l_dPointDistance = sqrt(((pboundarypoint->m_lat - lat) * (pboundarypoint->m_lat - lat)) + ((pboundarypoint->m_lon - lon) * (pboundarypoint->m_lon - lon)));
                if( l_dRangeRingSize > l_dPointDistance ) {
                    l_GUID = l_pBoundaryPoint->m_GUID;
                    bInPoly = true;
                    break;
                }
            }
        }
        ODpoint_node = ODpoint_node->GetNext();
    }
    
    if(bInPoly) return l_GUID;
    else return wxT("");
}

bool BoundaryMan::FindPointInBoundaryPoint( BoundaryPoint *pBoundaryPoint, double lat, double lon )
{
    bool bInPoly = false;
    
    if(pBoundaryPoint->m_iODPointRangeRingsNumber > 0) {
        double l_dRangeRingSize = pBoundaryPoint->m_iODPointRangeRingsNumber * pBoundaryPoint->m_fODPointRangeRingsStep;
        double brg;
        double l_dPointDistance;
        DistanceBearingMercator_Plugin( pBoundaryPoint->m_lat, pBoundaryPoint->m_lon, lat, lon, &brg, &l_dPointDistance );
        if( l_dRangeRingSize > l_dPointDistance ) {
            bInPoly = true;
        }
    }
    
    return bInPoly;
}

bool BoundaryMan::FindPointInBoundaryPoint( wxString l_GUID, double lat, double lon )
{
    bool bInPoly = false;
    
    ODPoint *pPoint = g_pODPointMan->FindODPointByGUID( l_GUID );
    if(pPoint->m_sTypeString == wxT("Boundary Point")) {
        BoundaryPoint *pBoundaryPoint = (BoundaryPoint *) pPoint;
        if(pBoundaryPoint->m_iODPointRangeRingsNumber > 0) {
            double l_dRangeRingSize = pBoundaryPoint->m_iODPointRangeRingsNumber * pBoundaryPoint->m_fODPointRangeRingsStep;
            double brg;
            double l_dPointDistance;
            DistanceBearingMercator_Plugin( pBoundaryPoint->m_lat, pBoundaryPoint->m_lon, lat, lon, &brg, &l_dPointDistance );
            if( l_dRangeRingSize > l_dPointDistance )
                bInPoly = true;
        }
    }
        
    return bInPoly;
}

//  Parameters for this function:
//
//  int    polyCorners  =  how many corners the polygon has
//  double  polyX[]      =  horizontal coordinates of corners
//  double  polyY[]      =  vertical coordinates of corners
//  double  x, y         =  point to be tested
//
//  The function will return YES if the point x,y is inside the polygon, or
//  NO if it is not.  If the point is exactly on the edge of the polygon,
//  then the function may return YES or NO.
//
//  Note that division by zero is avoided because the division is protected
//  by the "if" clause which surrounds it.

bool BoundaryMan::pointInPolygon(int polyCorners, double *polyX, double *polyY, double x, double y) 
{
    
    int   i, j=polyCorners-1 ;
    bool  oddNodes=false      ;
    
    for (i=0; i<polyCorners; i++) {
        if (((polyY[i]< y && polyY[j]>=y)
            ||   (polyY[j]< y && polyY[i]>=y))
            &&  (polyX[i]<=x || polyX[j]<=x)) {
            oddNodes^=(polyX[i]+(y-polyY[i])/(polyY[j]-polyY[i])*(polyX[j]-polyX[i])<x); }
            j=i; 
        
    }
            
    return oddNodes; 
        
}