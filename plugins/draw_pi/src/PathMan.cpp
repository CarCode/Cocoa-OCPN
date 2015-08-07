/***************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  Path Manager
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

#include "PathMan.h"

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include "wx/image.h"
#include "wx/tokenzr.h"
#include <wx/progdlg.h>

#include "../../../include/dychart.h"

#include <stdlib.h>
#include <math.h>
#include <time.h>

#include <wx/listimpl.cpp>

#include "ocpn_draw_pi.h"
#include "ODSelect.h"
#include "ODConfig.h"
#include "Boundary.h"
#include "EBL.h"

#include "../../../include/georef.h"
#include "../../../include/pluginmanager.h"
#include "../../../include/cutil.h"

#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/apptrait.h>

//    List definitions for Point Manager Icons
WX_DECLARE_LIST(wxBitmap, markicon_bitmap_list_type);
WX_DECLARE_LIST(wxString, markicon_key_list_type);
WX_DECLARE_LIST(wxString, markicon_description_list_type);

//    List implementation for Point Manager Icons
#include <wx/listimpl.cpp>
WX_DEFINE_LIST(markicon_bitmap_list_type);
WX_DEFINE_LIST(markicon_key_list_type);
WX_DEFINE_LIST(markicon_description_list_type);

extern ocpn_draw_pi *g_ocpn_draw_pi;
extern PathList     *g_pPathList;
extern BoundaryList *g_pBoundaryList;
extern EBLList      *g_pEBLList;
extern ODConfig  *g_pODConfig;
extern ODSelect      *g_pODSelect;
extern PlugInManager  *g_OD_pi_manager;
extern int             g_path_line_width;

//--------------------------------------------------------------------------------
//      Pathman   "Path Manager"
//--------------------------------------------------------------------------------

PathMan::PathMan()
{
    m_pparent_app = NULL;
    pActivePath = NULL;
    pActivePoint = NULL;
    pPathActivatePoint = NULL;
}

PathMan::~PathMan()
{
    if( pPathActivatePoint ) delete pPathActivatePoint;
}

bool PathMan::IsPathValid( Path *pPath )
{
    wxPathListNode *node = g_pPathList->GetFirst();
    while( node ) {
        if( pPath == node->GetData() ) return true;
        node = node->GetNext();
    }
    return false;
}

bool PathMan::ActivatePath(Path *pPathToActivate )
{
    wxString msg_id( wxS("OCPN_PATH_ACTIVATED") );
    wxString msg;
    msg.append( wxS("Name: ") );
    msg.append( pPathToActivate->m_PathNameString.c_str() );
    msg.append( wxS(", GUID: ") );
    msg.append( pPathToActivate->m_GUID );
    SendPluginMessage( msg_id, msg );

    pPathToActivate->m_bPathIsActive = true;
    pPathToActivate->SetActiveColours();

    return true;
}

bool PathMan::DeactivatePath( Path *pPathToDeactivate )
{
    wxString msg_id( _T("OCPN_PATH_DEACTIVATED") );
    wxString msg;
    msg.append( wxS("Name: ") );
    msg.append( pPathToDeactivate->m_PathNameString.c_str() );
    msg.append( wxS(", GUID: ") );
    msg.append( pPathToDeactivate->m_GUID );
    SendPluginMessage( msg_id, msg );

    pPathToDeactivate->m_bPathIsActive = false;
    pPathToDeactivate->SetActiveColours();
//    console->pCDI->ClearBackground();

//    m_bDataValid = false;

    return true;
}

bool PathMan::DeletePath( Path *pPath )
{
    if( pPath ) {

        if( pPath->m_bIsInLayer )
            return false;
            
        ::wxBeginBusyCursor();

        g_pODConfig->DeleteConfigPath( pPath );

        //    Remove the path from associated lists
        g_pODSelect->DeleteAllSelectablePathSegments( pPath );
        g_pPathList->DeleteObject( pPath );
        if(pPath->m_sTypeString == wxT("Boundary")) g_pBoundaryList->DeleteObject( (Boundary *)pPath );
        if(pPath->m_sTypeString == wxT("EBL")) g_pEBLList->DeleteObject( (EBL *)pPath );

        // walk the path, tentatively deleting/marking points used only by this route
        wxODPointListNode *pnode = ( pPath->m_pODPointList )->GetFirst();
        while( pnode ) {
            ODPoint *prp = pnode->GetData();

            // check all other paths to see if this point appears in any other route
            Path *pcontainer_path = FindPathContainingODPoint( prp );

            if( pcontainer_path == NULL && prp->m_bIsInPath ) {
                prp->m_bIsInPath = false;          // Take this point out of this (and only) path
                if( !prp->m_bKeepXPath ) {
//    This does not need to be done with navobj.xml storage, since the waypoints are stored with the route
//                              g_pODConfig->DeleteWayPoint(prp);

                    g_pODSelect->DeleteSelectablePoint( prp, SELTYPE_OCPNPOINT );

                    // Remove all instances of this point from the list.
                    wxODPointListNode *pdnode = pnode;
                    while( pdnode ) {
                        pPath->m_pODPointList->DeleteNode( pdnode );
                        pdnode = pPath->m_pODPointList->Find( prp );
                    }

                    if(prp->m_MarkName == wxT("Boat") && pPath->m_sTypeString == wxT("EBL") ) g_ocpn_draw_pi->m_pEBLBoatPoint = NULL;
                    pnode = NULL;
                    delete prp;
                } else {
                    prp->m_bDynamicName = false;
                    prp->m_bIsolatedMark = true;        // This has become an isolated mark
                    prp->m_bKeepXPath = false;         // and is no longer part of a Boundary
                }

            }
            if( pnode ) pnode = pnode->GetNext();
            else
                pnode = pPath->m_pODPointList->GetFirst();                // restart the list
        }

        delete pPath;

        ::wxEndBusyCursor();

    }
    return true;
}

bool PathMan::DoesPathContainSharedPoints( Path *pPath )
{
    if( pPath ) {
        // walk the route, looking at each point to see if it is used by another route
        // or is isolated
        wxODPointListNode *pnode = ( pPath->m_pODPointList )->GetFirst();
        while( pnode ) {
            ODPoint *prp = pnode->GetData();

            // check all other paths to see if this point appears in any other route
            wxArrayPtrVoid *pRA = GetPathArrayContaining( prp );
            
             if( pRA ) {
                 for( unsigned int ir = 0; ir < pRA->GetCount(); ir++ ) {
                    Path *pb = (Path *) pRA->Item( ir );
                    if( pb == pPath)
                        continue;               // self
                    else 
                        return true;
                }
            }
                
            if( pnode ) pnode = pnode->GetNext();
        }
        
        //      Now walk the path again, looking for isolated type shared waypoints
        pnode = ( pPath->m_pODPointList )->GetFirst();
        while( pnode ) {
            ODPoint *prp = pnode->GetData();
            if( prp->m_bKeepXPath == true )
                return true;
            
           if( pnode ) pnode = pnode->GetNext();
        }
    }
    
    return false;
}
  
wxArrayPtrVoid *PathMan::GetPathArrayContaining( ODPoint *pWP )
{
    wxArrayPtrVoid *pArray = new wxArrayPtrVoid;

    wxPathListNode *path_node = g_pPathList->GetFirst();
    while( path_node ) {
        Path *ppath = path_node->GetData();

        wxODPointListNode *OCPNpoint_node = ( ppath->m_pODPointList )->GetFirst();
        while( OCPNpoint_node ) {
            ODPoint *prp = OCPNpoint_node->GetData();
            if( prp == pWP )                // success
            pArray->Add( (void *) ppath );

            OCPNpoint_node = OCPNpoint_node->GetNext();           // next waypoint
        }

        path_node = path_node->GetNext();                         // next route
    }

    if( pArray->GetCount() ) return pArray;

    else {
        delete pArray;
        return NULL;
    }
}

void PathMan::DeleteAllPaths( void )
{
    ::wxBeginBusyCursor();

    //    Iterate on the RouteList
    wxPathListNode *node = g_pPathList->GetFirst();
    while( node ) {
        Path *ppath = node->GetData();
        if( ppath->m_bIsInLayer ) {
            node = node->GetNext();
            continue;
        }

//        g_pODConfig->m_bSkipChangeSetUpdate = true;
        g_pODConfig->DeleteConfigPath( ppath );
        DeletePath( ppath );
        node = g_pPathList->GetFirst();                   // Path
//        g_pODConfig->m_bSkipChangeSetUpdate = false;
    }

    ::wxEndBusyCursor();

}

Path *PathMan::FindPathContainingODPoint( ODPoint *pWP )
{
    wxPathListNode *node = g_pPathList->GetFirst();
    while( node ) {
        Path *ppath = node->GetData();

        wxODPointListNode *pnode = ( ppath->m_pODPointList )->GetFirst();
        while( pnode ) {
            ODPoint *prp = pnode->GetData();
            if( prp == pWP )  return ppath;
            pnode = pnode->GetNext();
        }

        node = node->GetNext();
    }

    return NULL;                              // not found
}

Path *PathMan::FindPathByGUID( wxString guid )
{
    wxPathListNode *node = g_pPathList->GetFirst();
    while( node ) {
        Path *ppath = node->GetData();
        if(ppath->m_GUID == guid) return ppath;
        node = node->GetNext();
    }
    
    return NULL;                              // not found
}

void PathMan::SetColorScheme( PI_ColorScheme cs )
{
    // Re-Create the pens and colors

    m_pActiveODPointPen = wxThePenList->FindOrCreatePen( wxColour( 0, 0, 255 ), g_path_line_width, wxSOLID );
    m_pODPointPen = wxThePenList->FindOrCreatePen( wxColour( 0, 0, 255 ), g_path_line_width, wxSOLID );

//    Or in something like S-52 compliance
    wxColour tColour;
    GetGlobalColor( wxS("UINFB"), &tColour );
    m_pPathPen = wxThePenList->FindOrCreatePen( tColour, g_path_line_width, wxSOLID );
    m_pPathBrush = wxTheBrushList->FindOrCreateBrush( tColour, wxSOLID );

    GetGlobalColor( wxS("UINFO"), &tColour );
    m_pSelectedPathPen = wxThePenList->FindOrCreatePen( tColour, g_path_line_width, wxSOLID );
    m_pSelectedPathBrush = wxTheBrushList->FindOrCreateBrush( tColour, wxSOLID );

    GetGlobalColor( wxS("UARTE"), &tColour );
    m_pActivePathPen = wxThePenList->FindOrCreatePen( tColour, g_path_line_width, wxSOLID );

    GetGlobalColor( wxS("PLRTE"), &tColour );
    m_pActivePathBrush = wxTheBrushList->FindOrCreateBrush( tColour, wxSOLID );
    m_pActiveODPointBrush =  wxTheBrushList->FindOrCreateBrush( tColour, wxSOLID);

    m_pActiveODPointPen = wxThePenList->FindOrCreatePen( tColour, g_path_line_width, wxSOLID );
    GetGlobalColor( wxS("CHBLK"), &tColour );
    m_pODPointPen = wxThePenList->FindOrCreatePen( tColour, g_path_line_width, wxSOLID );
    m_pODPointBrush = wxTheBrushList->FindOrCreateBrush( tColour, wxSOLID);

}

