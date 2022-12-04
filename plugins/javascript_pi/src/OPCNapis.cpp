/* *************************************************************************
* Project:  OpenCPN
* Purpose:  JavaScript Plugin
* Author:   Tony Voss 16/05/2020
*
* Copyright (C) 2021 by Tony Voss
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License, under which
* you must including a copy of these terms in all copies
* https://www.gnu.org/licenses/gpl-3.0.en.html
***************************************************************************
* updated Oct 2021 to use extended waypoint API 17
*/

#include "duktape.h"
#include "JavaScript_pi.h"
#include "JavaScriptgui_impl.h"
#include <wx/listimpl.cpp>

#include <stdarg.h>

WX_DEFINE_LIST(Plugin_HyperlinkList);
WX_DEFINE_LIST(Plugin_WaypointList);		// used by API16 for tracks
WX_DEFINE_LIST(Plugin_WaypointExList);		// used for API17 for waypoints and routes

/* here define parameters for the OPCNgetGUID options */
/* Not using this at present
 enum {
    NEW,
    WAYPOINTS_ARRAY,
    WAYPOINT_SELECTED,
    ROUTE_SELECTED,
    END_OF_OPCN_GETGUID_OPTIONS
    
};
wxString getNames[] = {
    "NEW",
    "WAYPOINTS_ARRAY",
    "WAYPOINT_SELECTED",
    "ROUTE_SELECTED",
};
 */
/* end of OPCNgetGUID options */

Console *findConsoleByCtx(duk_context *ctx);
wxString extractFunctionName(duk_context *ctx, duk_idx_t idx);
void throwErrorByCtx(duk_context *ctx, wxString message);


wxString getContextDump(duk_context *ctx){ // return duktape context dump as string
    duk_push_context_dump(ctx);
    wxString result = duk_to_string(ctx, -1);
    duk_pop(ctx);
    return result;
    }

PlugIn_Waypoint_Ex * js_duk_to_opcn_waypoint(duk_context *ctx){
    // returns an opcn waypoint constructed from js waypoint on top of duk stack
    duk_size_t listLength, i;
    wxString dump;
    void throwErrorByCtx(duk_context *ctx, wxString message);
    PlugIn_Waypoint_Ex *p_waypoint = new PlugIn_Waypoint_Ex;
    // indenting here represents stack hight - do not reformat
    if (duk_get_prop_string(ctx, -1, "position")){
        if (!duk_get_prop_string(ctx, -1, "latitude")) throwErrorByCtx(ctx, "addSingleWaypoint error: no latitude");
            p_waypoint->m_lat = duk_to_number(ctx, -1);
            duk_pop(ctx);
        if (!duk_get_prop_string(ctx, -1, "longitude"))throwErrorByCtx(ctx, "addSingleWaypoint error: no longitude");
            p_waypoint->m_lon = duk_to_number(ctx, -1);
            duk_pop(ctx);
        duk_pop(ctx);   // done with position
        }
    else throwErrorByCtx(ctx, "addSingleWaypoint error: no position");
    if (duk_get_prop_string(ctx, -1, "markName")) p_waypoint->m_MarkName = duk_to_string(ctx, -1);
        duk_pop(ctx);
    duk_get_prop_string(ctx, -1, "GUID");
        p_waypoint->m_GUID = duk_to_string(ctx, -1);
        duk_pop(ctx);
    if (wxIsEmpty(p_waypoint->m_GUID)) p_waypoint->m_GUID = GetNewGUID();  // if no GUID, provide one
    if (duk_get_prop_string(ctx, -1, "description")) p_waypoint->m_MarkDescription = duk_to_string(ctx, -1);
        duk_pop(ctx);
    if (duk_get_prop_string(ctx, -1, "iconName")) p_waypoint->IconName = duk_to_string(ctx, -1);
        duk_pop(ctx);
    if (duk_get_prop_string(ctx, -1, "minScale")) p_waypoint->scamin = duk_to_number(ctx, -1);
        duk_pop(ctx);
    if (duk_get_prop_string(ctx, -1, "useMinScale")) p_waypoint->b_useScamin = duk_to_boolean(ctx, -1);
        duk_pop(ctx);
    if (duk_get_prop_string(ctx, -1, "isVisible")) p_waypoint->IsVisible = duk_to_boolean(ctx, -1);
        duk_pop(ctx);
    if (duk_get_prop_string(ctx, -1, "isNameVisible")) p_waypoint->IsNameVisible = duk_to_boolean(ctx, -1);
        duk_pop(ctx);
	if ( duk_get_prop_string(ctx, -1, "nRanges")) p_waypoint->nrange_rings = duk_to_int(ctx, -1);
        duk_pop(ctx);            
    if (duk_get_prop_string(ctx, -1, "rangeRingSpace")) p_waypoint->RangeRingSpace = duk_to_number(ctx, -1);
        duk_pop(ctx);    
    if (duk_get_prop_string(ctx, -1, "rangeRingColour")) p_waypoint->RangeRingColor.Set(duk_to_string(ctx, -1));
        duk_pop(ctx);
    if (duk_get_prop_string(ctx, -1, "creationDateTime")) p_waypoint->m_CreateTime.Set((time_t)(duk_to_number(ctx, -1))); // seconds
        duk_pop(ctx);    
    // hyperlink processing
    p_waypoint->m_HyperlinkList = new Plugin_HyperlinkList; // need to initialise to empty list
    if (duk_get_prop_string(ctx, -1, "hyperlinkList")){
        // we do have a hyperlinkList - this is hard work!
        if (duk_is_array(ctx, -1) && (listLength = duk_get_length(ctx, -1)) > 0){   // only process if non-empty array of hyperlinks
            duk_to_object(ctx, -1);
                for (i = 0; i < listLength; i++){
                    Plugin_Hyperlink *p_hyperlink{new Plugin_Hyperlink};
                    duk_get_prop_index(ctx, -1, (unsigned int) i);
                        duk_get_prop_string(ctx, -1, "description");
                        p_hyperlink->DescrText = duk_get_string(ctx, -1);
                        duk_pop(ctx);
                    duk_get_prop_string(ctx, -1, "link");
                        p_hyperlink->Link = duk_get_string(ctx, -1);
                        duk_pop(ctx);
                    duk_get_prop_string(ctx, -1, "type");
                        duk_pop(ctx);
                    p_waypoint->m_HyperlinkList->Append(p_hyperlink);
                    duk_pop(ctx);
                }
            }
        }
        duk_pop(ctx);
    return(p_waypoint);
    }
     
    PlugIn_Waypoint * js_duk_to_opcn_trackpoint(duk_context *ctx){
    // returns an opcn trackpoint constructed from js waypoint on top of duk stack
    void throwErrorByCtx(duk_context *ctx, wxString message);
        
    PlugIn_Waypoint *p_waypoint = new PlugIn_Waypoint;
    // indenting here represents stack hight - do not reformat
        if (duk_get_prop_string(ctx, -1, "position")){
            if (!duk_get_prop_string(ctx, -1, "latitude")) throwErrorByCtx(ctx, "add/upateTrack error: no latitude");
                p_waypoint->m_lat = duk_to_number(ctx, -1);
                duk_pop(ctx);
            if (!duk_get_prop_string(ctx, -1, "longitude"))throwErrorByCtx(ctx, "add/updateTrack error: no longitude");
                p_waypoint->m_lon = duk_to_number(ctx, -1);
                duk_pop(ctx);
            duk_pop(ctx);   // done with position
            }
        else throwErrorByCtx(ctx, "addtrackpoint error: no position");
        if (duk_get_prop_string(ctx, -1, "creationDateTime")) {
            p_waypoint->m_CreateTime.Set((time_t) duk_to_number(ctx, -1));
            }
        duk_pop(ctx);
    return(p_waypoint);
    }

PlugIn_Route_Ex * js_duk_to_opcn_route(duk_context *ctx, bool createGUID){
    // returns an opcn route constructed from js route on top of duk stack
    // get a GUID if none provided and createGUID is true
    void throwErrorByCtx(duk_context *ctx, wxString message);
    duk_size_t listLength, i;
    bool ret;
    PlugIn_Route_Ex *p_route = new PlugIn_Route_Ex();
    // indenting here represents stack hight - do not reformat
    duk_get_prop_string(ctx, -1, "name");
        p_route->m_NameString = duk_to_string(ctx, -1);
        duk_pop(ctx);
    duk_get_prop_string(ctx, -1, "from");
        p_route->m_StartString = duk_to_string(ctx, -1);
        duk_pop(ctx);
    duk_get_prop_string(ctx, -1, "to");
        p_route->m_EndString = duk_to_string(ctx, -1);
        duk_pop(ctx);
    ret = duk_get_prop_string(ctx, -1, "GUID");
        p_route->m_GUID = duk_to_string(ctx, -1);
        if (ret == 0 || wxIsEmpty(p_route->m_GUID) || createGUID) {
            p_route->m_GUID = GetNewGUID(); // if no GUID, provide one if allowed
            }
        duk_pop(ctx);
    if (duk_get_prop_string(ctx, -1, "waypoints")){
        if (duk_is_array(ctx, -1)){
            // and it is an array
            listLength = duk_get_length(ctx, -1);
            if (listLength < 2) throwErrorByCtx(ctx, "OCPNadd/add/updateupdateRoute error: less than two routepoints");
            p_route->pWaypointList = new Plugin_WaypointExList; // need to initialise to empty list
            duk_to_object(ctx, -1);
                for (i = 0; i < listLength; i++){   // do waypoints array
                    duk_get_prop_index(ctx, -1, (unsigned int) i);
                    PlugIn_Waypoint_Ex *p_waypoint = js_duk_to_opcn_waypoint(ctx);
                        p_route->pWaypointList->Append(p_waypoint);
                        duk_pop(ctx);
                }
            }
        }
    else throwErrorByCtx(ctx, "OCPNupdateRoute error: called without routepoints");
    duk_pop(ctx);
    return(p_route);
    }
    
PlugIn_Track * js_duk_to_opcn_track(duk_context *ctx, bool createGUID){
    // returns an opcn track constructed from js track on top of duk stack
    // get a GUID if none provided and createGUID is true
    void throwErrorByCtx(duk_context *ctx, wxString message);
    duk_size_t listLength, i;
    bool ret;
    PlugIn_Track *p_track = new PlugIn_Track();
    // indenting here represents stack hight - do not reformat
    duk_get_prop_string(ctx, -1, "name");
        p_track->m_NameString = duk_to_string(ctx, -1);
        duk_pop(ctx);
    duk_get_prop_string(ctx, -1, "from");
        p_track->m_StartString = duk_to_string(ctx, -1);
        duk_pop(ctx);
    duk_get_prop_string(ctx, -1, "to");
        p_track->m_EndString = duk_to_string(ctx, -1);
        duk_pop(ctx);
    ret = duk_get_prop_string(ctx, -1, "GUID");
        p_track->m_GUID = duk_to_string(ctx, -1);
        if (ret == 0 || wxIsEmpty(p_track->m_GUID) || createGUID) {
            p_track->m_GUID = GetNewGUID();  // if no GUID, provide one if allowed
            }
        duk_pop(ctx);
    if (duk_get_prop_string(ctx, -1, "waypoints")){
        if (duk_is_array(ctx, -1)){
            // and it is an array
            listLength = duk_get_length(ctx, -1);
            if (listLength < 2) throwErrorByCtx(ctx, "OCPNadd/updateTrack error: less than two trackpoints");
            p_track->pWaypointList = new Plugin_WaypointList; // need to initialise to empty list
            duk_to_object(ctx, -1);
                for (i = 0; i < listLength; i++){   // do waypoints array
                    duk_get_prop_index(ctx, -1, (unsigned int) i);
                    PlugIn_Waypoint *p_waypoint = js_duk_to_opcn_trackpoint(ctx);
                        p_track->pWaypointList->Append(p_waypoint);
                        duk_pop(ctx);
                }
            }
        }
    else throwErrorByCtx(ctx, "OCPNadd/updateTrack error: called without trackpoints");
    duk_pop(ctx);
    return(p_track);
    }


void ocpn_waypoint_to_js_duk(duk_context *ctx, PlugIn_Waypoint_Ex *p_waypoint){
    // constructs a JavaScript waypoint from an ocpn waypoint leaving it on the top of the stack
    Plugin_Hyperlink *p_hyperlink = new Plugin_Hyperlink();
    // extra indentation here shows stack depth - do not reformat!
    duk_push_object(ctx); // construct the waypoint object
        duk_push_object(ctx); // construct the waypoint object
            duk_push_number(ctx, p_waypoint->m_lat);
                duk_put_prop_literal(ctx, -2, "latitude");
            duk_push_number(ctx, p_waypoint->m_lon);
                duk_put_prop_literal(ctx, -2, "longitude");
            duk_put_prop_literal(ctx, -2, "position");
        duk_push_string(ctx, p_waypoint->m_MarkName);
            duk_put_prop_literal(ctx, -2, "markName");
        duk_push_string(ctx, p_waypoint->m_GUID);
            duk_put_prop_literal(ctx, -2, "GUID");
        duk_push_string(ctx, p_waypoint->m_MarkDescription);
            duk_put_prop_literal(ctx, -2, "description");
        duk_push_boolean(ctx, p_waypoint->IsVisible);
            duk_put_prop_literal(ctx, -2, "isVisible");
        duk_push_string(ctx, p_waypoint->IconName);
            duk_put_prop_literal(ctx, -2, "iconName");
        duk_push_string(ctx, p_waypoint->IconDescription);
            duk_put_prop_literal(ctx, -2, "iconDescription");
        duk_push_boolean(ctx, p_waypoint->IsNameVisible);
            duk_put_prop_literal(ctx, -2, "isNameVisible");
        duk_push_int(ctx, p_waypoint->nrange_rings);
        	duk_put_prop_literal(ctx, -2, "nRanges");
        duk_push_number(ctx, p_waypoint->RangeRingSpace);
        	duk_put_prop_literal(ctx, -2, "rangeRingSpace");
        duk_push_string(ctx, p_waypoint->RangeRingColor.GetAsString(wxC2S_HTML_SYNTAX));
        	duk_put_prop_literal(ctx, -2, "rangeRingColour");       	
        duk_push_number(ctx, p_waypoint->scamin);
        	duk_put_prop_literal(ctx, -2, "minScale");
        duk_push_boolean(ctx, p_waypoint->b_useScamin);
        	duk_put_prop_literal(ctx, -2, "useMinScale");		        	
        duk_push_boolean(ctx, p_waypoint->GetFSStatus());
        	duk_put_prop_literal(ctx, -2, "isFreeStanding");
        duk_push_boolean(ctx, p_waypoint->IsActive);
        	duk_put_prop_literal(ctx, -2, "isActive");
        duk_push_int(ctx, p_waypoint->GetRouteMembershipCount());
        	duk_put_prop_literal(ctx, -2, "routeCount");         	 	
	    duk_push_number(ctx, p_waypoint->m_CreateTime.GetTicks());	// seconds since 1st Jan 1970
            duk_put_prop_literal(ctx, -2, "creationDateTime");
        duk_idx_t arr_idx = duk_push_array(ctx); // the hyperlinkList array
        if (p_waypoint->m_HyperlinkList ){  // only attempt this if list of not empty
            wxPlugin_HyperlinkListNode *linknode = p_waypoint->m_HyperlinkList->GetFirst();
            for (duk_idx_t i = 0; linknode; linknode = linknode->GetNext(), i++){
                    p_hyperlink = linknode->GetData();
                    duk_push_object(ctx); // the link object
                        duk_push_string(ctx, p_hyperlink->DescrText);
                            duk_put_prop_literal(ctx, -2, "description");
                        duk_push_string(ctx, p_hyperlink->Link);
                            duk_put_prop_literal(ctx, -2, "link");
                        duk_push_string(ctx, p_hyperlink->Type);
                            duk_put_prop_literal(ctx, -2, "type");
                        duk_put_prop_index(ctx, arr_idx, i);
                    }
            }
        duk_put_prop_literal(ctx, -2, "hyperlinkList");
    delete p_hyperlink;
}

void ocpn_trackpoint_to_js_duk(duk_context *ctx, PlugIn_Waypoint *p_waypoint){
    // constructs a JavaScript trackpoint from an ocpn waypoint leaving it on the top of the stack
    // extra indentation here shows stack depth - do not reformat!
    duk_push_object(ctx); // construct the waypoint object
        duk_push_object(ctx); // construct the waypoint object
            duk_push_number(ctx, p_waypoint->m_lat);
                duk_put_prop_literal(ctx, -2, "latitude");
            duk_push_number(ctx, p_waypoint->m_lon);
                duk_put_prop_literal(ctx, -2, "longitude");
            duk_put_prop_literal(ctx, -2, "position");
	    duk_push_number(ctx, p_waypoint->m_CreateTime.GetTicks());
            duk_put_prop_literal(ctx, -2, "creationDateTime");
    }
    
static duk_ret_t getMessageNames(duk_context *ctx) {  // get message names seen
    Console *pConsole = findConsoleByCtx(ctx);
    
    duk_push_string(ctx, pConsole->getMessagesString());
    return 1;  // returns one arg
    }

static duk_ret_t onMessageName(duk_context *ctx) {  // to wait for message - save function to call
    duk_idx_t nargs = duk_get_top(ctx);  // number of args in call
    Console *pConsole = findConsoleByCtx(ctx);
    if (nargs > 2) throwErrorByCtx(ctx, "OCPNonMessageName bad call");
    if (nargs == 0) { // empty call - cancel any waiting callback
        size_t messageCount = pConsole->mMessages.GetCount();
        if (messageCount > 0){
            for(unsigned int index = 0; index < messageCount; index++){
                pConsole->mMessages[index].functionName = wxEmptyString;
                }
            }
        pConsole->mWaitingCached = false;
        if (!pConsole->mRunningMain && !pConsole->isWaiting()){
            pConsole->wrapUp(DONE);
            }
        return(0);
        }
    duk_require_function(ctx, 0);
    duk_require_string(ctx, 1);
//    TRACE(5, pConsole->dukDump());
//    TRACE(5, "About to register waiting for message");
//    TRACE (5, findConsoleByCtx(ctx)->dukDump());
    int index = pConsole->OCPNmessageIndex(wxString(duk_to_string(ctx, 1)));
    pConsole->mMessages[index].functionName = extractFunctionName(ctx, 0);
//    TRACE(5, "Registered");
    duk_pop_2(ctx);
    pConsole->mWaitingCached = pConsole->mWaiting = true;
    return 0;  // returns no arg
}

static duk_ret_t onNMEAsentence(duk_context *ctx) {  // to wait for NMEA message - save function to call
    duk_idx_t nargs = duk_get_top(ctx);  // number of args in call
    Console *pConsole = findConsoleByCtx(ctx);
    if (nargs == 0) { // empty call - cancel any waiting callback
        pConsole->m_NMEAmessageFunction = wxEmptyString;
        pConsole->mWaitingCached = false;
        return(0);
        }

    if (pConsole->m_NMEAmessageFunction != wxEmptyString){
        // request already outstanding
        throwErrorByCtx(ctx, "OCPNonNMEAsentence called with call outstanding");
        }
    else{
        duk_require_function(ctx, 0);
        pConsole->m_NMEAmessageFunction = extractFunctionName(ctx,0);
        pConsole->mWaitingCached = pConsole->mWaiting = true;
        }
    return 0;  // returns no arg
    }
    
static duk_ret_t onActiveLeg(duk_context *ctx) {  // to wait for active leg message - save function to call
    duk_idx_t nargs = duk_get_top(ctx);  // number of args in call
    Console *pConsole = findConsoleByCtx(ctx);
    if (nargs == 0) { // empty call - cancel any waiting callback
        pConsole->m_activeLegFunction = wxEmptyString;
        pConsole->mWaitingCached = false;
        return(0);
        }

    if (pConsole->m_activeLegFunction != wxEmptyString){
        // request already outstanding
        throwErrorByCtx(ctx, "OCPNonActiveLeg called with call outstanding");
        }
    else{
        duk_require_function(ctx, 0);
        pConsole->m_activeLegFunction = extractFunctionName(ctx,0);
        pConsole->mWaitingCached = pConsole->mWaiting = true;
        }
    return 0;  // returns no arg
    }

static duk_ret_t getNavigation(duk_context *ctx) {  // gets latest navigation data and constructs navigation object
    extern JavaScript_pi *pJavaScript_pi;
    // ****  Indenting here shows stack depth - do not re-indent this section ****
    duk_push_object(ctx);
        duk_push_number(ctx, pJavaScript_pi->m_positionFix.FixTime);
            duk_put_prop_literal(ctx, -2, "fixTime");
        duk_push_object(ctx);                                  // start of position
            duk_push_number(ctx, pJavaScript_pi->m_positionFix.Lat);
                duk_put_prop_literal(ctx, -2, "latitude");
            duk_push_number(ctx, pJavaScript_pi->m_positionFix.Lon);
                duk_put_prop_literal(ctx, -2, "longitude");
            duk_put_prop_literal(ctx, -2, "position");             // end of position
        duk_push_number(ctx, pJavaScript_pi->m_positionFix.Sog);
            duk_put_prop_literal(ctx, -2, "SOG");
        duk_push_number(ctx, pJavaScript_pi->m_positionFix.Cog);
            duk_put_prop_literal(ctx, -2, "COG");
        duk_push_number(ctx, pJavaScript_pi->m_positionFix.Var);
            duk_put_prop_literal(ctx, -2, "variation");
        duk_push_number(ctx, pJavaScript_pi->m_positionFix.Hdm);
            duk_put_prop_literal(ctx, -2, "HDM");
        duk_push_number(ctx, pJavaScript_pi->m_positionFix.Hdt);
            duk_put_prop_literal(ctx, -2, "HDT");
        duk_push_int(ctx, pJavaScript_pi->m_positionFix.nSats);
            duk_put_prop_literal(ctx, -2, "nSats");
    return 1;  // returns one arg
}

static duk_ret_t getNavigationK(duk_context *ctx) {  // gets latest navigation data and constructs navigation object Signak K formatted
    extern JavaScript_pi *pJavaScript_pi;
    wxString thisTime = wxDateTime(pJavaScript_pi->m_positionFix.FixTime).FormatISOCombined().c_str();

    // ****  Indenting here shows stack depth - do not re-indent this section ****
    duk_push_object(ctx);
        duk_push_object(ctx);                               // start of SOG
            duk_push_number(ctx, pJavaScript_pi->m_positionFix.Sog);
            duk_put_prop_literal(ctx, -2, "value");
            duk_push_string(ctx, "OCPN");
            duk_put_prop_literal(ctx, -2, "sentence");
            duk_push_string(ctx, thisTime);
            duk_put_prop_literal(ctx, -2, "timestamp");
            duk_put_prop_literal(ctx, -2, "speedOverGround"); // end of SOG
        duk_push_object(ctx);                               // start of COG
            duk_push_number(ctx, pJavaScript_pi->m_positionFix.Cog);
            duk_put_prop_literal(ctx, -2, "value");
            duk_push_string(ctx, "OCPN");
            duk_put_prop_literal(ctx, -2, "sentence");
            duk_push_string(ctx, thisTime);
            duk_put_prop_literal(ctx, -2, "timestamp");
            duk_put_prop_literal(ctx, -2, "courseOverGround"); // end of COG
        duk_push_object(ctx);                                // start of position
            duk_push_object(ctx);                              // start of value
                duk_push_number(ctx, pJavaScript_pi->m_positionFix.Lat);
                duk_put_prop_literal(ctx, -2, "latitude");
                duk_push_number(ctx, pJavaScript_pi->m_positionFix.Lon);
                duk_put_prop_literal(ctx, -2, "longitude");
            duk_put_prop_literal(ctx, -2, "value");             // end of value
            duk_push_string(ctx, "OCPN");                       // start of nSats
            duk_put_prop_literal(ctx, -2, "sentence");
            duk_push_int(ctx, pJavaScript_pi->m_positionFix.nSats);
            duk_put_prop_literal(ctx, -2, "numberSatellites");  // end of nSats
            duk_push_string(ctx, thisTime);
            duk_put_prop_literal(ctx, -2, "timestamp");
        duk_put_prop_literal(ctx, -2, "position");              // end of position
    duk_push_object(ctx);                                   // start of heading
        duk_push_object(ctx);                                // start of heading true
            duk_push_number(ctx, pJavaScript_pi->m_positionFix.Hdt);
            duk_put_prop_literal(ctx, -2, "value");
            duk_push_string(ctx, "OCPN");
            duk_put_prop_literal(ctx, -2, "sentence");
            duk_push_string(ctx, thisTime);
            duk_put_prop_literal(ctx, -2, "timestamp");
            duk_put_prop_literal(ctx, -2, "headingTrue");
        duk_push_object(ctx);                                   // start of heading magnetic
            duk_push_number(ctx, pJavaScript_pi->m_positionFix.Hdm);
            duk_put_prop_literal(ctx, -2, "value");
            duk_push_string(ctx, "OCPN");
            duk_put_prop_literal(ctx, -2, "sentence");
            duk_push_string(ctx, thisTime);
            duk_put_prop_literal(ctx, -2, "timestamp");
            duk_put_prop_literal(ctx, -2, "headingMagnetic");
        duk_put_prop_literal(ctx, -2, "heading");              // end of heading
    duk_push_object(ctx);                                   // start of variation
        duk_push_number(ctx, pJavaScript_pi->m_positionFix.Var);
        duk_put_prop_literal(ctx, -2, "value");
        duk_push_string(ctx, "OCPN");
        duk_put_prop_literal(ctx, -2, "sentence");
        duk_push_string(ctx, thisTime);
        duk_put_prop_literal(ctx, -2, "timestamp");
        duk_put_prop_literal(ctx, -2, "variation");
    return 1;  // returns one arg
}

/* static duk_ret_t getActiveLeg(duk_context *ctx) {  // gets active leg data and constructs object
    extern JavaScript_pi *pJavaScript_pi;
    // ****  Indenting here shows stack depth - do not re-indent this section ****
    duk_push_object(ctx);
        duk_push_string(ctx, pJavaScript_pi->Plugin_Active_Leg_Info.wp_name);
            duk_put_prop_literal(ctx, -2, "markName");
        duk_push_number(ctx, pJavaScript_pi->Plugin_Active_Leg_Info.Btw);
            duk_put_prop_literal(ctx, -2, "bearing");
        duk_push_number(ctx, pJavaScript_pi->Plugin_Active_Leg_Info.Dtw;
            duk_put_prop_literal(ctx, -2, "distance");
        duk_push_number(ctx, pJavaScript_pi->Plugin_Active_Leg_Info.Xte);
            duk_put_prop_literal(ctx, -2, "xte");
        duk_push_boolean(ctx, pJavaScript_pi->Plugin_Active_Leg_Info.arrival);
            duk_put_prop_literal(ctx, -2, "arrived");
    return 1;  // returns one arg
}
 */

static duk_ret_t NMEApush(duk_context *ctx) {  // pushes NMEA sentence on stack out through OpenCPN
    // props to Dirk Smits for the checksum calculation lifted from his NMEAConverter plugin
    duk_idx_t nargs;  // number of args in call
    duk_ret_t result = 0;
    wxString sentence;  // the NMEA sentence
    auto ComputeChecksum{       // Using Lambda function here to keep it private to this function
        [](wxString sentence) {
             unsigned char calculated_checksum = 0;
             for(wxString::const_iterator i = sentence.begin()+1; i != sentence.end() && *i != '*'; ++i)
                 calculated_checksum ^= static_cast<unsigned char> (*i);

            return( wxString::Format("%02X", calculated_checksum) );
        }
    };
    
    nargs = duk_get_top(ctx);
    if ((nargs == 1) &&  (duk_get_type(ctx, 0) == DUK_TYPE_STRING)){
        // we have a single string - good to go
        sentence = wxString(duk_to_string(ctx,0));
        sentence.Trim();
        // we will drop any existing checksum
        int starPos = sentence.Find("*");
        if (starPos != wxNOT_FOUND){ // yes there is one
            sentence = sentence.SubString(0, starPos-1); // truncate at * onwards
            }
        wxString Checksum = ComputeChecksum(sentence);
        sentence = sentence.Append("*");
        sentence = sentence.Append(Checksum);
        sentence = sentence.Append("\r\n");
        PushNMEABuffer(sentence);
        return(result);
    }
    else {
        throwErrorByCtx(ctx, "OCPNpushNMEA called without single string argument");
        }
    return(result);
}

static duk_ret_t sendMessage(duk_context *ctx) {  // sends message to OpenCPN
    duk_idx_t nargs;  // number of args in call
    duk_ret_t result = 0;
    wxString message_body = wxEmptyString;
    
    nargs = duk_get_top(ctx);
    if (nargs < 1){
        throwErrorByCtx(ctx, "OCPNsendMessage error: called without at least one argument");
        }
    duk_require_string(ctx, 0);
    if (nargs > 1){
        // we have a message body
        duk_require_string(ctx, 1);
        message_body = wxString(duk_to_string(ctx,1));
        duk_pop(ctx);
        }
    wxString message_id = wxString(duk_to_string(ctx,0));
    duk_pop(ctx);
    message_id.Trim();
//    TRACE(4, "Sending message " + message_id);
//    TRACE (5, findConsoleByCtx(ctx)->dukDump());
    SendPluginMessage(message_id, message_body);
    return(result);
}

const unsigned int GPXbufferLength {1500};
char GPXbuffer[GPXbufferLength];
static duk_ret_t getARPgpx(duk_context *ctx) {  // get Active Route Point as GPX
    bool result = GetActiveRoutepointGPX( GPXbuffer, GPXbufferLength);
    duk_push_string(ctx, result ? GPXbuffer : "");
    return 1;  // returns one arg
    }

static duk_ret_t OCPNrefreshCanvas(duk_context *ctx) {  // refresh main window
    RequestRefresh(GetOCPNCanvasWindow());
    return(0);
}


static duk_ret_t getNewGUID(duk_context *ctx) {  // get new GUID
    duk_push_string(ctx, GetNewGUID());
    return(1);  // returns 1 string
    }

/* not using this at present
static duk_ret_t getGUID(duk_context *ctx) {  // get GUID as per option
    wxArrayString guidArray;
    duk_idx_t arr_idx;
    wxString result;
    int i;
    size_t count;
    
    int option = duk_to_int(ctx, 0);
    duk_pop(ctx);
    switch (option){
        case NEW: duk_push_string(ctx, GetNewGUID());
            break;
        case WAYPOINTS_ARRAY:
            guidArray = GetWaypointGUIDArray();
            arr_idx = duk_push_array(ctx);
            if (!guidArray.IsEmpty()){
                count = guidArray.GetCount();
                for (i = 0; i < count; i++){
                    duk_push_string(ctx, guidArray[i]);
                    duk_put_prop_index(ctx, arr_idx, i);
                    }
                }
            break;
        case WAYPOINT_SELECTED:
            result = GetSelectedWaypointGUID_Plugin();
            if (result == wxEmptyString) duk_push_string(ctx, result);
            else duk_push_boolean(ctx, false);
            break;
        case ROUTE_SELECTED:
            result = GetSelectedRouteGUID_Plugin();
            if (result == wxEmptyString) duk_push_string(ctx, result);
            else duk_push_boolean(ctx, false);
            break;
        default: throwErrorByCtx(ctx, "OCPNgetGUID error: called with invalid argument");
            }
    return(1);
    }
 */

void clearWaypointsOutofRoute(PlugIn_Route_Ex *p_route){
// For a route structure, clear out the waypoints
// For each waypoint, need to clear out any hyperlink lists
PlugIn_Waypoint_Ex *p_waypoint;

wxPlugin_WaypointExListNode *linknode = p_route->pWaypointList->GetFirst();
for (unsigned int i = 0; linknode; linknode = linknode->GetNext(), i++){
    p_waypoint = linknode->GetData();
    p_waypoint->m_HyperlinkList->DeleteContents(true);
    p_waypoint->m_HyperlinkList->clear();
    }
p_route->pWaypointList->DeleteContents(true);
p_route->pWaypointList->clear();
}

void clearWaypointsOutofTrack(PlugIn_Track *p_track){
// For a track structure, clear out the waypoints
// PlugIn_Waypoint *p_waypoint;  // Not used, see below
/*
wxPlugin_WaypointListNode *linknode = p_track->pWaypointList->GetFirst();
    for (unsigned int i = 0; linknode; linknode = linknode->GetNext(), i++){
    p_waypoint = linknode->GetData();
    p_waypoint->m_HyperlinkList->DeleteContents(true);
    p_waypoint->m_HyperlinkList->clear();
    }
 */
p_track->pWaypointList->DeleteContents(true);
p_track->pWaypointList->clear();
}

static duk_ret_t getWaypointGUIDs(duk_context *ctx){ // get waypoing GUID array
    wxArrayString guidArray;
    duk_idx_t arr_idx;
    int i;
    size_t count;
    
    guidArray = GetWaypointGUIDArray();
    arr_idx = duk_push_array(ctx);
    if (!guidArray.IsEmpty()){
        count = guidArray.GetCount();
        for (i = 0; i < count; i++){
            duk_push_string(ctx, guidArray[i]);
            duk_put_prop_index(ctx, arr_idx, i);
            }
        }
    return(1);
    }
    
static duk_ret_t getActiveWaypointGUID(duk_context *ctx){ // get GUID of active waypoint
    duk_push_string(ctx, GetActiveWaypointGUID());
    return(1);
    }

static duk_ret_t getSingleWaypoint(duk_context *ctx) {

    PlugIn_Waypoint_Ex *p_waypoint = new PlugIn_Waypoint_Ex();
    bool result;
    wxString GUID;
    duk_require_string(ctx, 0);
    GUID = duk_get_string(ctx, 0);
    duk_pop(ctx);
    result = GetSingleWaypointEx(GUID, p_waypoint);
    if (!result){  // waypoint does not exist
        delete p_waypoint;
        throwErrorByCtx(ctx, "OCPNGetSingleWaypoint called with non-existant GUID " + GUID);
        return(1);
        }
    ocpn_waypoint_to_js_duk(ctx, p_waypoint);   // construct the waypoint
    delete p_waypoint;
    return(1);
    }

static duk_ret_t addSingleWaypoint(duk_context *ctx) {
    PlugIn_Waypoint_Ex *p_waypoint;
    bool permanent = true;  // permanent waypoint by default
    bool result;
    wxString GUID;
    duk_idx_t nargs;
    duk_require_object(ctx,0);
    nargs = duk_get_top(ctx);   // number of arguments in call
    if (nargs == 2) {
        duk_require_boolean(ctx,1);
        permanent = duk_get_boolean(ctx, 1);   // decide on permanency
        duk_pop(ctx);
        }
    p_waypoint = js_duk_to_opcn_waypoint(ctx);  // construct the opcn waypoint
    if (p_waypoint->m_GUID == "undefined") p_waypoint->m_GUID = GetNewGUID();
    result = AddSingleWaypointEx(p_waypoint);
    if (!result){ // waypoint already exists?
            throwErrorByCtx(ctx, "OCPNaddSingleWaypoint called with existing GUID " + p_waypoint->m_GUID);
            }
    duk_push_string(ctx, p_waypoint->m_GUID);  // else return the GUID
    // now waypoint safely stored in OpenCPN, clean up - list data not otherwise released
    p_waypoint->m_HyperlinkList->DeleteContents(true);
    p_waypoint->m_HyperlinkList->clear();
    return(1);  // returns result
}

static duk_ret_t updateSingleWaypoint(duk_context *ctx) {
    PlugIn_Waypoint_Ex *p_waypoint;
    bool result;
    duk_require_object(ctx,0);
    p_waypoint = js_duk_to_opcn_waypoint(ctx);  // construct the ocpn waypoint
    result = UpdateSingleWaypointEx(p_waypoint);
    if (!result){ // waypoint does not exists?
        throwErrorByCtx(ctx, "OCPNupdateSingleWaypoint error. Non-existant GUID? " + p_waypoint->m_GUID);
        }
    // now waypoint safely stored in OpenCPN, clean up - list data not otherwise released
    p_waypoint->m_HyperlinkList->DeleteContents(true);
    p_waypoint->m_HyperlinkList->clear();
    duk_push_boolean(ctx, true);    // for compatibility with v0.2 return true
    return(1);
}

static duk_ret_t deleteSingleWaypoint(duk_context *ctx) {  // given a GUID, deletes waypoint
    wxString GUID;
    bool result;
    duk_require_string(ctx,0);
    GUID = wxString(duk_to_string(ctx,0));
    result = DeleteSingleWaypoint(GUID);
    if (!result){  // waypoint does not exist
        throwErrorByCtx(ctx, "OCPNDeleteSingleWaypoint called with non-existant GUID " + GUID);
        }
    duk_pop(ctx);
    duk_push_boolean(ctx, true);    // for compatibility with v0.2 return true
    return(1);
    }
    
static duk_ret_t getRouteGUIDs(duk_context *ctx){ // get routes GUID array
    wxArrayString guidArray;
    duk_idx_t arr_idx;
    int i;
    size_t count;
    
    guidArray = GetRouteGUIDArray();
    arr_idx = duk_push_array(ctx);
    if (!guidArray.IsEmpty()){
        count = guidArray.GetCount();
        for (i = 0; i < count; i++){
            duk_push_string(ctx, guidArray[i]);
            duk_put_prop_index(ctx, arr_idx, i);
            }
        }
    return(1);
    }
    
static duk_ret_t getActiveRouteGUID(duk_context *ctx){ // get GUID of active route
    duk_push_string(ctx, GetActiveRouteGUID());
    return(1);
    }

static duk_ret_t getRouteByGUID(duk_context *ctx) {

    wxString GUID;
    std::unique_ptr<PlugIn_Route_Ex> p_route;
    PlugIn_Waypoint_Ex *p_waypoint = new PlugIn_Waypoint_Ex();

    duk_require_string(ctx,0);
    GUID = duk_get_string(ctx, 0);
    duk_pop(ctx);
    p_route = GetRouteEx_Plugin(GUID);
    if (p_route == nullptr){ // no such route
        throwErrorByCtx(ctx, "OCPNgetRoute called with non-existant GUID " + GUID);
        }
    // extra indentation here shows stack depth - do not reformat!
    duk_push_object(ctx); // construct the route object
    duk_push_string(ctx, p_route->m_NameString);
        duk_put_prop_literal(ctx, -2, "name");
    duk_push_string(ctx, p_route->m_StartString);
        duk_put_prop_literal(ctx, -2, "from");
    duk_push_string(ctx, p_route->m_EndString);
        duk_put_prop_literal(ctx, -2, "to");
    duk_push_string(ctx, p_route->m_GUID);
        duk_put_prop_literal(ctx, -2, "GUID");
    duk_push_boolean(ctx, p_route->m_isActive);
	    duk_put_prop_literal(ctx, -2, "isActive");
    duk_idx_t arr_idx = duk_push_array(ctx); // the waypoint array
    if (p_route->pWaypointList ){  // only attempt this if waypoint list of not empty
        wxPlugin_WaypointExListNode *linknode = p_route->pWaypointList->GetFirst();
        for (duk_idx_t i = 0; linknode; linknode = linknode->GetNext(), i++){
            p_waypoint = linknode->GetData();
            ocpn_waypoint_to_js_duk(ctx, p_waypoint);   // construct this waypoint
            duk_put_prop_index(ctx, arr_idx, i);
            }
        }
    duk_put_prop_literal(ctx, -2, "waypoints");
    return(1);
    }

static duk_ret_t addRoute(duk_context *ctx) { // add the route to OpenCPN
    PlugIn_Route_Ex *p_route;

    bool permanent = true;  // permanent by default
    bool result;
    duk_idx_t nargs;
    duk_require_object(ctx,0);
    nargs = duk_get_top(ctx);   // number of arguments in call
    if (nargs == 2) {
        duk_require_boolean(ctx,1);
        permanent = duk_get_boolean(ctx, 1);   // decide on permanency
        duk_pop(ctx);
        }
    p_route = js_duk_to_opcn_route(ctx, true);    // construct the opcn route, providing a GUID if not supplied
    result = AddPlugInRouteEx(p_route);
    if (!result){
        throwErrorByCtx(ctx, "OCPNaddRoute called with existant GUID " + p_route->m_GUID);
        }
    duk_push_string(ctx, p_route->m_GUID);  // return the GUID
    // now route safely stored in OpenCPN, clean up - lists data not otherwise released
    clearWaypointsOutofRoute(p_route);
    return(1);
    }

static duk_ret_t updateRoute(duk_context *ctx) { // update the route in OpenCPN
    PlugIn_Route_Ex *p_route;
    duk_require_object(ctx,0);
    p_route = js_duk_to_opcn_route(ctx, false);    // construct the opcn route - must have given GUID
    if(!UpdatePlugInRouteEx(p_route)) throwErrorByCtx(ctx, "OCPNupdateRoute called with non-existant GUID " + p_route->m_GUID);
    clearWaypointsOutofRoute(p_route);
    duk_push_boolean(ctx, true);    // for compatibility with v0.2 return true
    return(1);
    }

static duk_ret_t deleteRoute(duk_context *ctx) {  // given a GUID, deletes route
    wxString GUID;
    duk_require_string(ctx,0);
    GUID = wxString(duk_to_string(ctx,0));
    duk_pop(ctx);
    if (!DeletePlugInRoute(GUID)) {
        throwErrorByCtx(ctx, "OCPNdeleteRoute called with non-existant GUID " + GUID);
        }
    duk_push_boolean(ctx, true);    // for compatibility with v0.2 return true
    return(1);
}

#define TRACKS
#ifdef IN_HARNESS
#undef TRACKS   // no support in the harness for tracks at this time, so don't compile
#endif
#ifdef TRACKS

static duk_ret_t getTrackGUIDs(duk_context *ctx){ // get tracks GUID array
    wxArrayString guidArray;
    duk_idx_t arr_idx;
    int i;
    size_t count;
    
    guidArray = GetTrackGUIDArray();
    arr_idx = duk_push_array(ctx);
    if (!guidArray.IsEmpty()){
        count = guidArray.GetCount();
        for (i = 0; i < count; i++){
            duk_push_string(ctx, guidArray[i]);
            duk_put_prop_index(ctx, arr_idx, i);
            }
        }
    return(1);
    }
    
static duk_ret_t getTrackByGUID(duk_context *ctx) {
//    bool result;  // WXOSX Unused
    wxString GUID;
    std::unique_ptr<PlugIn_Track> p_track;
//    Plugin_Hyperlink *p_hyperlink = new Plugin_Hyperlink();  // Not used here
    PlugIn_Waypoint *p_waypoint = new PlugIn_Waypoint();

    GUID = duk_get_string(ctx, 0);
    duk_pop(ctx);
     //No harness equivalent of this, so omit for now
    p_track = GetTrack_Plugin(GUID);
    if (p_track == nullptr){ // no such track
        throwErrorByCtx(ctx, "OCPNgetTrack called with non-existant GUID " + GUID);
        }
    // extra indentation here shows stack depth - do not reformat!
    duk_push_object(ctx); // construct the track object
    duk_push_string(ctx, p_track->m_NameString);
        duk_put_prop_literal(ctx, -2, "name");
    duk_push_string(ctx, p_track->m_StartString);
        duk_put_prop_literal(ctx, -2, "from");
    duk_push_string(ctx, p_track->m_EndString);
        duk_put_prop_literal(ctx, -2, "to");
    duk_push_string(ctx, p_track->m_GUID);
        duk_put_prop_literal(ctx, -2, "GUID");
    duk_idx_t arr_idx = duk_push_array(ctx); // the waypoint array
    if (p_track->pWaypointList ){  // only attempt this if waypoint list is not empty
        wxPlugin_WaypointListNode *linknode = p_track->pWaypointList ->GetFirst();
        for (duk_idx_t i = 0; linknode; linknode = linknode->GetNext(), i++){
            p_waypoint = linknode->GetData();
            ocpn_trackpoint_to_js_duk(ctx, p_waypoint);   // construct this waypoint
            duk_put_prop_index(ctx, arr_idx, i);
            }
        }
    duk_put_prop_literal(ctx, -2, "waypoints");
    return(1);
    }

static duk_ret_t addTrack(duk_context *ctx) { // add the track to OpenCPN
    PlugIn_Track *p_track;

    bool permanent = true;  // permanent by default
    bool result;
    duk_idx_t nargs;
    nargs = duk_get_top(ctx);   // number of arguments in call
    if (nargs == 2) {
        permanent = duk_get_boolean(ctx, -1);   // decide on permanency
        duk_pop(ctx);
        }
    p_track = (PlugIn_Track *) js_duk_to_opcn_track(ctx, true); 
    result = AddPlugInTrack(p_track, permanent);
    if (!result) duk_push_boolean(ctx, false);  //  failed
    else duk_push_string(ctx, p_track->m_GUID);  // else return the GUID
    // now track safely stored in OpenCPN, clean up - lists data not otherwise released
    clearWaypointsOutofTrack(p_track);
    return(1);
}

static duk_ret_t updateTrack(duk_context *ctx) { // update the track in OpenCPN
    PlugIn_Track *p_track;
//    bool result;  // WXOSX Unused
//    duk_idx_t nargs;  // WXOSX Unused

    p_track = js_duk_to_opcn_track(ctx, false);
    duk_push_boolean(ctx, UpdatePlugInTrack(p_track));  // result will be returned
    clearWaypointsOutofTrack(p_track);
    return(1);
}

static duk_ret_t deleteTrack(duk_context *ctx) {  // given a GUID, deletes track
    wxString GUID;
#ifdef __WXOSX__
    bool outcome = NULL;
#else
    bool outcome;
#endif
    GUID = wxString(duk_to_string(ctx,0));
    if (!DeletePlugInTrack(GUID)) {
        throwErrorByCtx(ctx, "OCPNdeleteTrack called with non-existant GUID " + GUID);
        }
    duk_push_boolean(ctx, outcome);
    return(1);  // returns boolean result
}
#endif

#ifdef OMIT
static duk_ret_t jump_to_position(duk_context *ctx) { // jump to position
    duk_double_t lat, lon, scale;
    MAYBE_DUK_DUMP
    lat = duk_get_number(ctx, -3);
    lon = duk_get_number(ctx, -2);
    scale = duk_get_number(ctx, -1);
    JumpToPosition(lat, lon, scale);
    duk_pop_3(ctx);
    return(0);  // no arguments returned
}
#endif

static duk_ret_t getPluginConfig(duk_context *ctx) {  // gets plugin configuration object
    // ****  Indenting here shows stack depth - do not re-indent this section ****
    duk_push_object(ctx);
        duk_push_int(ctx, PLUGIN_VERSION_MAJOR);
            duk_put_prop_literal(ctx, -2, "versionMajor");
        duk_push_int(ctx, PLUGIN_VERSION_MINOR);
            duk_put_prop_literal(ctx, -2, "versionMinor");
        duk_push_string(ctx, PLUGIN_VERSION_COMMENT);
            duk_put_prop_literal(ctx, -2, "comment");
        duk_push_int(ctx, PLUGIN_VERSION_PATCH);
            duk_put_prop_literal(ctx, -2, "patch");
#ifdef IN_HARNESS
        duk_push_boolean(ctx, true);
#else
        duk_push_boolean(ctx, false);
#endif
            duk_put_prop_literal(ctx, -2, "inHarness");
    return 1;  // returns one arg
}

static duk_ret_t getOCPNconfig(duk_context *ctx) {  // gets OCPN configuration as JSON
    // json = OCPNgetOCPNconfig()
    extern JavaScript_pi* pJavaScript_pi;
    if (pJavaScript_pi->openCPNConfig == wxEmptyString) throwErrorByCtx(ctx, "OCPNgetOCPNconfig  no config available");
    duk_push_string(ctx, pJavaScript_pi->openCPNConfig);
    return 1;
    }

static duk_ret_t getAISTargetsArray(duk_context *ctx) {
    ArrayOfPlugIn_AIS_Targets *AIStargets;
    ArrayOfPlugIn_AIS_Targets::iterator it;
    PlugIn_AIS_Target *t;
    duk_idx_t arr_idx;
    int i;
    wxString temp;
    
    arr_idx = duk_push_array(ctx);  // we are going to create an array of objects
    AIStargets = GetAISTargetArray();
    for( it = (*AIStargets).begin(), i= 0; it != (*AIStargets).end(); ++it, ++i ) {
        t = *it;
        // ****  Indenting here shows stack depth - do not re-indent this section ****
        duk_push_object(ctx);
            duk_push_int(ctx, t->MMSI);
                duk_put_prop_literal(ctx, -2, "MMSI");
            duk_push_int(ctx, t->Class);
                duk_put_prop_literal(ctx, -2, "class");
            duk_push_int(ctx, t->NavStatus);
                duk_put_prop_literal(ctx, -2, "navStatus");
            duk_push_number(ctx, t->SOG);
                duk_put_prop_literal(ctx, -2, "SOG");
            duk_push_number(ctx, t->COG);
                duk_put_prop_literal(ctx, -2, "COG");
            duk_push_number(ctx, t->HDG);
                duk_put_prop_literal(ctx, -2, "HDG");
            duk_push_object(ctx);
                duk_push_number(ctx, t->Lon);
                    duk_put_prop_literal(ctx, -2, "longitude");
                duk_push_number(ctx, t->Lat);
                    duk_put_prop_literal(ctx, -2, "latitude");
                duk_put_prop_literal(ctx, -2, "position");
            duk_push_number(ctx, t->ShipType);
                duk_put_prop_literal(ctx, -2, "shipType");
            duk_push_int(ctx, t->ROTAIS);
                duk_put_prop_literal(ctx, -2, "ROTAIS");
            // callSign and shipName padded with '@' so need to remove.  t->ShipName.BeforeFirst() does not compile, so do in stages
            temp = t->CallSign;
            duk_push_string(ctx, temp.BeforeFirst('@'));
                duk_put_prop_literal(ctx, -2, "callSign");
            temp = t->ShipName;
            duk_push_string(ctx, temp.BeforeFirst('@'));
                duk_put_prop_literal(ctx, -2, "shipName");
            duk_push_int(ctx, t->IMO);
                duk_put_prop_literal(ctx, -2, "IMO");
            duk_push_number(ctx, t->Range_NM);
                duk_put_prop_literal(ctx, -2, "range");
            duk_push_number(ctx, t->Brg);
                duk_put_prop_literal(ctx, -2, "bearing");
            duk_push_boolean(ctx, t->bCPA_Valid);
                duk_put_prop_literal(ctx, -2, "CPAvalid");
            duk_push_number(ctx, t->TCPA);
                duk_put_prop_literal(ctx, -2, "CPAminutes");
            duk_push_number(ctx, t->CPA);
                duk_put_prop_literal(ctx, -2, "CPAnm");
            duk_push_int(ctx, t->alarm_state);
                duk_put_prop_literal(ctx, -2, "alarmState");
            duk_put_prop_index(ctx, arr_idx, i);
        }
    return(1);
    }

static duk_ret_t getVectorPP(duk_context *ctx) {
        // get vector between two positions
        // API seems to work in opposite sense - corrected here
        double fromLat, fromLon, toLat, toLon, bearing, distance;
        duk_idx_t obj_idx;
        duk_idx_t from_idx {0};
        duk_idx_t to_idx {1};
        duk_get_prop_literal(ctx, from_idx, "latitude");
        if (duk_is_undefined(ctx, -1)  || !duk_is_number(ctx, -1)) throwErrorByCtx(ctx, "OPNgetVectorPP from latitude is missing or invalid");
        fromLat = duk_get_number(ctx, -1);
        duk_pop(ctx);
        duk_get_prop_literal(ctx, from_idx, "longitude");
        if (duk_is_undefined(ctx, -1)  || !duk_is_number(ctx, -1)) throwErrorByCtx(ctx, "OPNgetVectorPP from longitude is missing or invalid");
        fromLon = duk_get_number(ctx, -1);
        duk_pop(ctx);
        duk_get_prop_literal(ctx, to_idx, "latitude");
        if (duk_is_undefined(ctx, -1)  || !duk_is_number(ctx, -1)) throwErrorByCtx(ctx, "OPNgetVectorPP to latitude is missing or invalid");
        toLat = duk_get_number(ctx, -1);
        duk_pop(ctx);
        duk_get_prop_literal(ctx, to_idx, "longitude");
        if (duk_is_undefined(ctx, -1)  || !duk_is_number(ctx, -1)) throwErrorByCtx(ctx, "OPNgetVectorPP to longitude is missing or invalid");
        toLon = duk_get_number(ctx, -1);
        duk_pop(ctx);
        duk_pop_2(ctx); // pop off both args
        DistanceBearingMercator_Plugin(toLat, toLon, fromLat, fromLon, &bearing, &distance);
        obj_idx = duk_push_object(ctx);
        duk_push_number(ctx, bearing);
        duk_put_prop_literal(ctx, obj_idx, "bearing");
        duk_push_number(ctx, distance);
        duk_put_prop_literal(ctx, obj_idx, "distance");
        return 1;
        }

static duk_ret_t getPositionPV(duk_context *ctx) {
        // get position after position moved by vector
        double fromLat, fromLon, toLat, toLon, bearing, distance;
//        duk_idx_t obj_idx;  // Not used here
        duk_idx_t pos_idx {0};
        duk_idx_t vector_idx {1};

        duk_get_prop_literal(ctx, pos_idx, "latitude");
        if (duk_is_undefined(ctx, -1)  || !duk_is_number(ctx, -1)) throwErrorByCtx(ctx, "OCPNgetPositionPV latitude is missing or invalid");
        fromLat = duk_get_number(ctx, -1);
        duk_pop(ctx);
        duk_get_prop_literal(ctx, pos_idx, "longitude");
        if (duk_is_undefined(ctx, -1)  || !duk_is_number(ctx, -1)) throwErrorByCtx(ctx, "OCPNgetPositionPV  longitude is missing or invalid");
        fromLon = duk_get_number(ctx, -1);
        duk_pop(ctx);
        duk_get_prop_literal(ctx, vector_idx, "bearing");
        if (duk_is_undefined(ctx, -1)  || !duk_is_number(ctx, -1)) throwErrorByCtx(ctx, "OCPNgetPositionPV bearing is missing or invalid");
        bearing = duk_get_number(ctx, -1);
        duk_pop(ctx);
        duk_get_prop_literal(ctx, vector_idx, "distance");
        if (duk_is_undefined(ctx, -1)  || !duk_is_number(ctx, -1)) throwErrorByCtx(ctx, "OCPNgetPositionPV distance is missing or invalid");
        distance = duk_get_number(ctx, -1);
        duk_pop(ctx);
        duk_pop_2(ctx); // pop off both arguments
        duk_push_object(ctx);   // our new position object to return
        PositionBearingDistanceMercator_Plugin(fromLat, fromLon, bearing, distance, &toLat, &toLon);
        duk_push_number(ctx, toLat);
        duk_put_prop_literal(ctx, -2, "latitude");
        duk_push_number(ctx, toLon);
        duk_put_prop_literal(ctx, -2, "longitude");
        return 1;
        }

static duk_ret_t getGCdistance(duk_context *ctx) {
        // retuns Great Circle distance between two positions
        double firstLat, firstLon, secondLat, secondLon, distance;
        duk_idx_t from_idx {0};
        duk_idx_t to_idx {1};
        duk_get_prop_literal(ctx, from_idx, "latitude");
        if (duk_is_undefined(ctx, -1)  || !duk_is_number(ctx, -1)) throwErrorByCtx(ctx, "OPNgetGCdistance first latitude is missing or invalid");
        firstLat = duk_get_number(ctx, -1);
        duk_pop(ctx);
        duk_get_prop_literal(ctx, from_idx, "longitude");
        if (duk_is_undefined(ctx, -1)  || !duk_is_number(ctx, -1)) throwErrorByCtx(ctx, "OPNgetGCdistance first longitude is missing or invalid");
        firstLon = duk_get_number(ctx, -1);
        duk_pop(ctx);
        duk_get_prop_literal(ctx, to_idx, "latitude");
        if (duk_is_undefined(ctx, -1)  || !duk_is_number(ctx, -1)) throwErrorByCtx(ctx, "OPNgetGCdistance second latitude is missing or invalid");
        secondLat = duk_get_number(ctx, -1);
        duk_pop(ctx);
        duk_get_prop_literal(ctx, to_idx, "longitude");
        if (duk_is_undefined(ctx, -1)  || !duk_is_number(ctx, -1)) throwErrorByCtx(ctx, "OPNgetGCdistance second longitude is missing or invalid");
        secondLon = duk_get_number(ctx, -1);
        duk_pop(ctx);
        duk_pop_2(ctx); // pop off both args
        distance = DistGreatCircle_Plugin(firstLat, firstLon, secondLat, secondLon);
        duk_push_number(ctx, distance);
        return 1;
        }
    
// ÑÑÑÑÑÑ API registrations follow

void ocpn_apis_init(duk_context *ctx) { // register the OpenCPN APIs
    duk_idx_t duk_push_c_function(duk_context *ctx, duk_c_function func, duk_idx_t nargs);
     
    duk_push_global_object(ctx);
    
    /* create the getNames objects */
/*    for (int i = 0; i < END_OF_OPCN_GETGUID_OPTIONS; i++){
        duk_push_string(ctx, getNames[i]);
        duk_push_int(ctx,i);
        duk_def_prop(ctx, -3, DUK_DEFPROP_SET_ENUMERABLE);
    }
*/
    /* add the fuctions */
    
    duk_push_string(ctx, "OCPNpushNMEA");
    duk_push_c_function(ctx, NMEApush, 1 /*number of arguments*/);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    
    duk_push_string(ctx, "OCPNsendMessage");
    duk_push_c_function(ctx, sendMessage, DUK_VARARGS /*variable number of arguments*/);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    
    duk_push_string(ctx, "OCPNgetMessageNames");
    duk_push_c_function(ctx, getMessageNames, 0 /* no args */);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);

    duk_push_string(ctx, "OCPNonMessageName");
    duk_push_c_function(ctx, onMessageName, DUK_VARARGS /* args */);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);

    duk_push_string(ctx, "OCPNonNMEAsentence");
    duk_push_c_function(ctx, onNMEAsentence, DUK_VARARGS /* args */);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);

    duk_push_string(ctx, "OCPNonActiveLeg");
    duk_push_c_function(ctx, onActiveLeg, DUK_VARARGS /* args */);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
        
    duk_push_string(ctx, "OCPNgetARPgpx");
    duk_push_c_function(ctx, getARPgpx, 0 /* no args */);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);

    duk_push_string(ctx, "OCPNgetNavigation");
    duk_push_c_function(ctx, getNavigation, 0 /* no args */);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    
    duk_push_string(ctx, "OCPNgetNavigationK");
    duk_push_c_function(ctx, getNavigationK, 0 /* no args */);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
            
    duk_push_string(ctx, "OCPNgetNewGUID");
    duk_push_c_function(ctx, getNewGUID, 0 /* 1 arg */);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    
    duk_push_string(ctx, "OCPNgetWaypointGUIDs");
    duk_push_c_function(ctx, getWaypointGUIDs, 0 /* 0 args */);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    
    duk_push_string(ctx, "OCPNgetActiveWaypointGUID");
    duk_push_c_function(ctx, getActiveWaypointGUID, 0 /* 0 args */);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    
    duk_push_string(ctx, "OCPNgetSingleWaypoint");
    duk_push_c_function(ctx, getSingleWaypoint, 1 /* arg is GUID */);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);

    duk_push_string(ctx, "OCPNaddSingleWaypoint");
    duk_push_c_function(ctx, addSingleWaypoint, DUK_VARARGS);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    
    duk_push_string(ctx, "OCPNupdateSingleWaypoint");
    duk_push_c_function(ctx, updateSingleWaypoint, 1);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);

    duk_push_string(ctx, "OCPNgetVectorPP");
    duk_push_c_function(ctx, getVectorPP, 2);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    
    duk_push_string(ctx, "OCPNgetGCdistance");
    duk_push_c_function(ctx, getGCdistance, 2);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    
    duk_push_string(ctx, "OCPNgetPositionPV");
    duk_push_c_function(ctx, getPositionPV, 2);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);

    duk_push_string(ctx, "OCPNdeleteSingleWaypoint");
    duk_push_c_function(ctx, deleteSingleWaypoint, 1 /* 1 arg */);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    
    duk_push_string(ctx, "OCPNgetRouteGUIDs");
    duk_push_c_function(ctx, getRouteGUIDs, 0 /* 0 args */);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);

    duk_push_string(ctx, "OCPNgetActiveRouteGUID");
    duk_push_c_function(ctx, getActiveRouteGUID, 0 /* 0 args */);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);

    duk_push_string(ctx, "OCPNgetRoute");
    duk_push_c_function(ctx, getRouteByGUID, 1 /* arg is GUID */);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    
    duk_push_string(ctx, "OCPNaddRoute");
    duk_push_c_function(ctx, addRoute, 1 /* arg is GUID */);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    
    duk_push_string(ctx, "OCPNupdateRoute");
    duk_push_c_function(ctx, updateRoute, 1);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    
    duk_push_string(ctx, "OCPNdeleteRoute");
    duk_push_c_function(ctx, deleteRoute, 1);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    
    duk_push_string(ctx, "OCPNrefreshCanvas");
    duk_push_c_function(ctx, OCPNrefreshCanvas, 0);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    
#ifdef TRACKS

    duk_push_string(ctx, "OCPNgetTrackGUIDs");
    duk_push_c_function(ctx, getTrackGUIDs, 0 /* 0 args */);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    
    duk_push_string(ctx, "OCPNgetTrack");
    duk_push_c_function(ctx, getTrackByGUID, 1 /* arg is GUID */);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    
    duk_push_string(ctx, "OCPNaddTrack");
    duk_push_c_function(ctx, addTrack, 1 /* arg is GUID */);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    
    duk_push_string(ctx, "OCPNupdateTrack");
    duk_push_c_function(ctx, updateTrack, 1);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    
    duk_push_string(ctx, "OCPNdeleteTrack");
    duk_push_c_function(ctx, deleteTrack, 1);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
#endif

#ifdef OMIT
    duk_push_string(ctx, "OCPNjumpToPosition");
    duk_push_c_function(ctx, jump_to_position, 3);  // 3 arguments
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
#endif
    
    duk_push_string(ctx, "OCPNgetPluginConfig");
    duk_push_c_function(ctx, getPluginConfig, 1);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    
    duk_push_string(ctx, "OCPNgetOCPNconfig");
    duk_push_c_function(ctx, getOCPNconfig, 0);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);

    
    duk_push_string(ctx, "OCPNgetAISTargets");
    duk_push_c_function(ctx, getAISTargetsArray, 0);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    
    duk_pop(ctx);
}
