/* ***********************************************************************
* Project:  OpenCPN
* Purpose:  JavaScript Plugin
* Author:   Tony Voss 16/05/2020
*
* Copyright Ⓒ 2020 by Tony Voss
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License, under which
* you must including a copy of these terms in all copies
* https://www.gnu.org/licenses/gpl-3.0.en.html
***************************************************************************/

#include "stdio.h"
#include "duktape.h"
#include "JavaScript_pi.h"
#include "JavaScriptgui.h"
#include "JavaScriptgui_impl.h"
#include <wx/listimpl.cpp>

#include <stdarg.h>

extern JS_control_class JS_control;
void JS_dk_error(duk_context *ctx, wxString message);
WX_DEFINE_LIST(Plugin_HyperlinkList);
WX_DEFINE_LIST(Plugin_WaypointList);

/* here define parameters for the OPCNgetGUID options */
/* Not usung this at present
 enum {
    NEW,
    WAYPOINTS_ARRAY,
    WAYPOINT_SELECTED,
    ROUTE_SELECTED,
    END_OF_OPCN_GETGUID_OPTIONS
    
};
wxString getNames[] = {
    _("NEW"),
    _("WAYPOINTS_ARRAY"),
    _("WAYPOINT_SELECTED"),
    _("ROUTE_SELECTED"),
};
 */
/* end of OPCNgetGUID options */

PlugIn_Waypoint * js_duk_to_opcn_waypoint(duk_context *ctx){
    // returns an opcn waypoint constructed from js waypoint on top of duk stack
    duk_size_t listLength, i;
    PlugIn_Waypoint *p_waypoint = new PlugIn_Waypoint();
    // indenting here represents stack hight - do not reformat
    duk_get_prop_string(ctx, -1, "position");
        if (!duk_get_prop_string(ctx, -1, "latitude")) JS_dk_error(ctx, "addSingleWaypoint error: no latitude");
            p_waypoint->m_lat = duk_to_number(ctx, -1);
            duk_pop(ctx);
        if (!duk_get_prop_string(ctx, -1, "longitude")) JS_dk_error(ctx, "addSingleWaypoint error: no longitude");
            p_waypoint->m_lon = duk_to_number(ctx, -1);
            duk_pop(ctx);
        duk_pop(ctx);   // done with position
    duk_get_prop_string(ctx, -1, "markName");
        p_waypoint->m_MarkName = duk_to_string(ctx, -1);
        duk_pop(ctx);
    duk_get_prop_string(ctx, -1, "GUID");
        p_waypoint->m_GUID = duk_to_string(ctx, -1);
        duk_pop(ctx);
    if (wxIsEmpty(p_waypoint->m_GUID)) p_waypoint->m_GUID = GetNewGUID();  // if no GUID, provide one
    duk_get_prop_string(ctx, -1, "description");
        p_waypoint->m_MarkDescription = duk_to_string(ctx, -1);
        duk_pop(ctx);
    duk_get_prop_string(ctx, -1, "iconName");
        p_waypoint->m_IconName = duk_to_string(ctx, -1);
        duk_pop(ctx);
    duk_get_prop_string(ctx, -1, "isVisible");
        p_waypoint->m_IsVisible = duk_to_boolean(ctx, -1);
        duk_pop(ctx);
    duk_get_prop_string(ctx, -1, "creationDateTime");
        p_waypoint->m_CreateTime = wxDateTime(duk_to_number(ctx, -1));
        duk_pop(ctx);
    if (duk_get_prop_string(ctx, -1, "hyperlinkList")){
        // we have a hyperlinkList - this is hard work!
        p_waypoint->m_HyperlinkList = new Plugin_HyperlinkList; // need to initialise to empty list
        if (duk_is_array(ctx, -1)){
            // and it is an array
            duk_to_object(ctx, -1);
                listLength = duk_get_length(ctx, -1);
                for (i = 0; i < listLength; i++){
                    Plugin_Hyperlink *p_hyperlink{new Plugin_Hyperlink};
                    duk_get_prop_index(ctx, -1, i);
                        duk_get_prop_string(ctx, -1, "description");
                        p_hyperlink->DescrText = duk_get_string(ctx, -1);
                        duk_pop(ctx);
                    duk_get_prop_string(ctx, -1, "link");
                        p_hyperlink->Link = duk_get_string(ctx, -1);
                        duk_pop(ctx);
                    duk_get_prop_string(ctx, -1, "type");
                        p_hyperlink->Type = duk_get_string(ctx, -1);
                        duk_pop(ctx);
                    p_waypoint->m_HyperlinkList->Append(p_hyperlink);
                    duk_pop(ctx);
                }
            }
        }
    duk_pop(ctx);
    return(p_waypoint);
    }

PlugIn_Route * js_duk_to_opcn_route(duk_context *ctx){
    // returns an opcn route constructed from js route on top of duk stack
    // also works for tracks
    duk_size_t listLength, i;
    PlugIn_Route *p_route = new PlugIn_Route();
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
    duk_get_prop_string(ctx, -1, "GUID");
    p_route->m_GUID = duk_to_string(ctx, -1);
    if (wxIsEmpty(p_route->m_GUID)) p_route->m_GUID = GetNewGUID();  // if no GUID, provide one
    duk_pop(ctx);
    if (duk_get_prop_string(ctx, -1, "waypoints")){
    p_route->pWaypointList = new Plugin_WaypointList; // need to initialise to empty list
    if (duk_is_array(ctx, -1)){
        // and it is an array
        duk_to_object(ctx, -1);
            listLength = duk_get_length(ctx, -1);
            for (i = 0; i < listLength; i++){   // do waypoints array
                duk_get_prop_index(ctx, -1, i);
                PlugIn_Waypoint *p_waypoint = js_duk_to_opcn_waypoint(ctx);
                    p_route->pWaypointList->Append(p_waypoint);
                    duk_pop(ctx);
            }
        }
    }
    duk_pop(ctx);
    return(p_route);
    }

void ocpn_waypoint_to_js_duk(duk_context *ctx, PlugIn_Waypoint *p_waypoint){
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
        duk_push_string(ctx, p_waypoint->m_IconName);
            duk_put_prop_literal(ctx, -2, "iconName");
        duk_push_boolean(ctx, p_waypoint->m_IsVisible);
            duk_put_prop_literal(ctx, -2, "isVisible");
        duk_push_number(ctx, p_waypoint->m_CreateTime.GetTicks());
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
    
static duk_ret_t getMessageNames(duk_context *ctx) {  // get message names seen
    // JS_control_class JS_control;
    
    duk_push_string(ctx, JS_control.getMessagesString());
    return 1;  // returns one arg
    }

static duk_ret_t onMessageName(duk_context *ctx) {  // to wait for message - save function to call
    int index = JS_control.messageIndex(wxString(duk_to_string(ctx, 1)));
    JS_control.m_messages[index].functionName = JS_control.getFunctionName();
    JS_control.m_JSactive = true;
    return 0;  // returns no arg
}

static duk_ret_t onNMEAsentence(duk_context *ctx) {  // to wait for NMEA message - save function to call
    if (JS_control.m_NMEAmessageFunction != wxEmptyString){
        // request already outstanding
        cout << "JavaScript onNMEAsentence called with call outstanding - this ignored\n";
        }
    else{
        JS_control.m_NMEAmessageFunction = JS_control.getFunctionName();
        JS_control.m_JSactive = true;
        }
    return 0;  // returns no arg
    }

static duk_ret_t getNavigation(duk_context *ctx) {  // gets latest navigation data and constructs navigation object
//    time_t fixTime = JS_control.m_positionFix.FixTime;
    // ****  Indenting here shows stack depth - do not re-indent this section ****
    duk_push_object(ctx);
        duk_push_int(ctx, JS_control.m_positionFix.FixTime);
            duk_put_prop_literal(ctx, -2, "fixTime");
        duk_push_object(ctx);                                  // start of position
            duk_push_number(ctx, JS_control.m_positionFix.Lat);
                duk_put_prop_literal(ctx, -2, "latitude");
            duk_push_number(ctx, JS_control.m_positionFix.Lon);
                duk_put_prop_literal(ctx, -2, "longitude");
            duk_put_prop_literal(ctx, -2, "position");             // end of position
        duk_push_number(ctx, JS_control.m_positionFix.Sog);
            duk_put_prop_literal(ctx, -2, "SOG");
        duk_push_number(ctx, JS_control.m_positionFix.Cog);
            duk_put_prop_literal(ctx, -2, "COG");
        duk_push_number(ctx, JS_control.m_positionFix.Var);
            duk_put_prop_literal(ctx, -2, "variation");
        duk_push_number(ctx, JS_control.m_positionFix.Hdm);
            duk_put_prop_literal(ctx, -2, "HDM");
        duk_push_number(ctx, JS_control.m_positionFix.Hdt);
            duk_put_prop_literal(ctx, -2, "HDT");
        duk_push_int(ctx, JS_control.m_positionFix.nSats);
            duk_put_prop_literal(ctx, -2, "nSats");
    return 1;  // returns one arg
}

static duk_ret_t getNavigationK(duk_context *ctx) {  // gets latest navigation data and constructs navigation object Signak K formatted
    wxString thisTime = wxDateTime(JS_control.m_positionFix.FixTime).FormatISOCombined().c_str();
    // ****  Indenting here shows stack depth - do not re-indent this section ****
    duk_push_object(ctx);
        duk_push_object(ctx);                               // start of SOG
            duk_push_number(ctx, JS_control.m_positionFix.Sog);
            duk_put_prop_literal(ctx, -2, "value");
            duk_push_string(ctx, "OCPN");
            duk_put_prop_literal(ctx, -2, "sentence");
            duk_push_string(ctx, thisTime);
            duk_put_prop_literal(ctx, -2, "timestamp");
            duk_put_prop_literal(ctx, -2, "speedOverGround"); // end of SOG
        duk_push_object(ctx);                               // start of COG
            duk_push_number(ctx, JS_control.m_positionFix.Cog);
            duk_put_prop_literal(ctx, -2, "value");
            duk_push_string(ctx, "OCPN");
            duk_put_prop_literal(ctx, -2, "sentence");
            duk_push_string(ctx, thisTime);
            duk_put_prop_literal(ctx, -2, "timestamp");
            duk_put_prop_literal(ctx, -2, "courseOverGround"); // end of COG
        duk_push_object(ctx);                                // start of position
            duk_push_object(ctx);                              // start of value
                duk_push_number(ctx, JS_control.m_positionFix.Lat);
                duk_put_prop_literal(ctx, -2, "latitude");
                duk_push_number(ctx, JS_control.m_positionFix.Lon);
                duk_put_prop_literal(ctx, -2, "longitude");
            duk_put_prop_literal(ctx, -2, "value");             // end of value
            duk_push_string(ctx, "OCPN");                       // start of nSats
            duk_put_prop_literal(ctx, -2, "sentence");
            duk_push_int(ctx, JS_control.m_positionFix.nSats);
            duk_put_prop_literal(ctx, -2, "numberSatellites");  // end of nSats
            duk_push_string(ctx, thisTime);
            duk_put_prop_literal(ctx, -2, "timestamp");
        duk_put_prop_literal(ctx, -2, "position");              // end of position
    duk_push_object(ctx);                                   // start of heading
        duk_push_object(ctx);                                // start of heading true
            duk_push_number(ctx, JS_control.m_positionFix.Hdt);
            duk_put_prop_literal(ctx, -2, "value");
            duk_push_string(ctx, "OCPN");
            duk_put_prop_literal(ctx, -2, "sentence");
            duk_push_string(ctx, thisTime);
            duk_put_prop_literal(ctx, -2, "timestamp");
            duk_put_prop_literal(ctx, -2, "headingTrue");
        duk_push_object(ctx);                                   // start of heading magnetic
            duk_push_number(ctx, JS_control.m_positionFix.Hdm);
            duk_put_prop_literal(ctx, -2, "value");
            duk_push_string(ctx, "OCPN");
            duk_put_prop_literal(ctx, -2, "sentence");
            duk_push_string(ctx, thisTime);
            duk_put_prop_literal(ctx, -2, "timestamp");
            duk_put_prop_literal(ctx, -2, "headingMagnetic");
        duk_put_prop_literal(ctx, -2, "heading");              // end of heading
    duk_push_object(ctx);                                   // start of variation
        duk_push_number(ctx, JS_control.m_positionFix.Var);
        duk_put_prop_literal(ctx, -2, "value");
        duk_push_string(ctx, "OCPN");
        duk_put_prop_literal(ctx, -2, "sentence");
        duk_push_string(ctx, thisTime);
        duk_put_prop_literal(ctx, -2, "timestamp");
        duk_put_prop_literal(ctx, -2, "variation");
    return 1;  // returns one arg
}

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

            return( wxString::Format(_("%02X"), calculated_checksum) );
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
        sentence = sentence.Append(wxT("*"));
        sentence = sentence.Append(Checksum);
        sentence = sentence.Append(wxT("\r\n"));
        PushNMEABuffer(sentence);
        return(result);
    }
    else {
        JS_dk_error(ctx, "OCPNpushNMEA error: called without single string argument");
    }
    return(result);
}

static duk_ret_t sendMessage(duk_context *ctx) {  // sends message to OpenCPN
    duk_idx_t nargs;  // number of args in call
    duk_ret_t result = 0;
    wxString message_body = wxEmptyString;
    
    nargs = duk_get_top(ctx);
    if (nargs < 1){
        JS_dk_error(ctx, "OCPNsendMessage error: called without at least one argument");
        }
    if ((nargs >= 1) &&  (duk_get_type(ctx, 1) == DUK_TYPE_STRING)){
        // we have a message body
        message_body = wxString(duk_to_string(ctx,1));
        }
    wxString message_id = wxString(duk_to_string(ctx,0));
    message_id.Trim();
    SendPluginMessage(message_id, message_body);
    return(result);
}

static duk_ret_t OCPNonSeconds(duk_context *ctx) {  // call function after milliseconds elapsed
    duk_ret_t result = 0;
    duk_idx_t nargs = duk_get_top(ctx);  // number of args in call
    TimeActions ourTimeAction;
    wxString argument = wxEmptyString;
    
    if ((int)JS_control.m_times.GetCount() > 10){
        JS_dk_error(ctx, "OCPNsendMessage error: already have 10 calls outstanding");
        }
    if ((nargs < 2) || (nargs > 3)){
        JS_dk_error(ctx, "OCPNsendMessage error: requires two or three arguments");

        }
    wxTimeSpan delay(0, 0, duk_to_number(ctx,1), 0);
    ourTimeAction.timeToCall = wxDateTime::Now() + delay;
    ourTimeAction.functionName = JS_control.getFunctionName();
    if (nargs > 2) argument = wxString(duk_to_string(ctx,2));  //if user included 3rd argument, use it
    ourTimeAction.argument = argument;
    JS_control.m_times.Add(ourTimeAction);  // add this action to array
    return(result);
    }

const unsigned int GPXbufferLength {1500};
char GPXbuffer[GPXbufferLength];
static duk_ret_t getARPgpx(duk_context *ctx) {  // get Active Route Point as GPX
    bool result = GetActiveRoutepointGPX( GPXbuffer, GPXbufferLength);
    duk_push_string(ctx, result ? GPXbuffer : "");
    return 1;  // returns one arg
    }

static duk_ret_t cancelAll(duk_context *ctx) {  // cancel all call-backs
    duk_ret_t result = 0;
    JS_control.clear();
    return(result);
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
        default: JS_dk_error(ctx, "OCPNgetGUID error: called with invalid argument");
            }
    return(1);
    }
 */

void clearWaypointsOutofRoute(PlugIn_Route *p_route){
// For a route structure, clear out the waypoints
// For each waypoint, need to clear out any hyperlink lists
PlugIn_Waypoint *p_waypoint;

wxPlugin_WaypointListNode *linknode = p_route->pWaypointList->GetFirst();
for (unsigned int i = 0; linknode; linknode = linknode->GetNext(), i++){
    p_waypoint = linknode->GetData();
    p_waypoint->m_HyperlinkList->DeleteContents(true);
    p_waypoint->m_HyperlinkList->clear();
    }
p_route->pWaypointList->DeleteContents(true);
p_route->pWaypointList->clear();
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

static duk_ret_t getSingleWaypoint(duk_context *ctx) {

    PlugIn_Waypoint *p_waypoint = new PlugIn_Waypoint();;
    bool result;
    wxString GUID;
//    Plugin_Hyperlink *p_hyperlink = new Plugin_Hyperlink();
    
    GUID = duk_get_string(ctx, 0);
    duk_pop(ctx);
    result = GetSingleWaypoint(GUID, p_waypoint);
    if (!result){
        // waypoint does not exist
        duk_push_boolean(ctx, false);
        delete p_waypoint;
        return(1);
        }
    ocpn_waypoint_to_js_duk(ctx, p_waypoint);   // construct the waypoint
    delete p_waypoint;
    return(1);
    }

static duk_ret_t addSingleWaypoint(duk_context *ctx) {
    PlugIn_Waypoint *p_waypoint;
    bool permanent = true;  // permanent waypoint by default
    bool result;
    duk_idx_t nargs;

    nargs = duk_get_top(ctx);   // number of arguments in call
    if (nargs == 2) {
        permanent = duk_get_boolean(ctx, -1);   // decide on permanency
        duk_pop(ctx);
        }
    p_waypoint = js_duk_to_opcn_waypoint(ctx);  // construct the opcn waypoint
    result = AddSingleWaypoint(p_waypoint, permanent);
    if (!result) duk_push_boolean(ctx, false);  //  failed
    else duk_push_string(ctx, p_waypoint->m_GUID);  // else return the GUID
    // now waypoint safely stored in OpenCPN, clean up - list data not otherwise released
    p_waypoint->m_HyperlinkList->DeleteContents(true);
    p_waypoint->m_HyperlinkList->clear();
    return(1);  // returns result
}

static duk_ret_t updateSingleWaypoint(duk_context *ctx) {
    PlugIn_Waypoint *p_waypoint;
    bool result;
 
    p_waypoint = js_duk_to_opcn_waypoint(ctx);  // construct the opcn waypoint
    result = UpdateSingleWaypoint(p_waypoint);
    if (!result) duk_push_boolean(ctx, false);  //  failed
    else duk_push_string(ctx, p_waypoint->m_GUID);  // else return the GUID
    // now waypoint safely stored in OpenCPN, clean up - list data not otherwise released
    p_waypoint->m_HyperlinkList->DeleteContents(true);
    p_waypoint->m_HyperlinkList->clear();
    return(1);  // returns boolean result
}

static duk_ret_t deleteSingleWaypoint(duk_context *ctx) {  // given a GUID, deletes waypoint
    wxString GUID;
    bool outcome;
    
    GUID = wxString(duk_to_string(ctx,0));
    outcome = DeleteSingleWaypoint(GUID);
    duk_push_boolean(ctx, outcome);
    return(1);  // returns boolean result
    }

static duk_ret_t getRouteByGUID(duk_context *ctx) {

    wxString GUID;
    std::unique_ptr<PlugIn_Route> p_route;
    PlugIn_Waypoint *p_waypoint = new PlugIn_Waypoint();

    GUID = duk_get_string(ctx, 0);
    duk_pop(ctx);
    p_route = GetRoute_Plugin(GUID);
    if (p_route == nullptr){ // no such route
        duk_push_boolean(ctx, false);
        return(1);
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
    duk_idx_t arr_idx = duk_push_array(ctx); // the waypoint array
    if (p_route->pWaypointList ){  // only attempt this if waypoint list of not empty
        wxPlugin_WaypointListNode *linknode = p_route->pWaypointList ->GetFirst();
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
    PlugIn_Route *p_route;

    bool permanent = true;  // permanent by default
    bool result;
    duk_idx_t nargs;
    nargs = duk_get_top(ctx);   // number of arguments in call
    if (nargs == 2) {
        permanent = duk_get_boolean(ctx, -1);   // decide on permanency
        duk_pop(ctx);
        }
    p_route = js_duk_to_opcn_route(ctx);    // construct the opcn route
    result = AddPlugInRoute(p_route, permanent);
    if (!result) duk_push_boolean(ctx, false);  //  failed
    else duk_push_string(ctx, p_route->m_GUID);  // else return the GUID
    // now route safely stored in OpenCPN, clean up - lists data not otherwise released
    clearWaypointsOutofRoute(p_route);
    return(1);
}

static duk_ret_t updateRoute(duk_context *ctx) { // update the route in OpenCPN
    PlugIn_Route *p_route;

    p_route = js_duk_to_opcn_route(ctx);    // construct the opcn route
    duk_push_boolean(ctx, UpdatePlugInRoute(p_route));  // result will be returned
    clearWaypointsOutofRoute(p_route);
    return(1);
}

static duk_ret_t deleteRoute(duk_context *ctx) {  // given a GUID, deletes route
    wxString GUID;
    bool outcome;

    GUID = wxString(duk_to_string(ctx,0));
    outcome = DeletePlugInRoute(GUID);
    duk_push_boolean(ctx, outcome);
    return(1);  // returns boolean result
}

#ifdef TRACKS
static duk_ret_t getTrackByGUID(duk_context *ctx) {
    bool result;
    wxString GUID;
    std::unique_ptr<PlugIn_Track> p_track;
    Plugin_Hyperlink *p_hyperlink = new Plugin_Hyperlink();
    PlugIn_Waypoint *p_waypoint = new PlugIn_Waypoint();

    GUID = duk_get_string(ctx, 0);
    duk_pop(ctx);
    p_track = GetTrack_Plugin(GUID);
    if (p_track == nullptr){ // no such track
        duk_push_boolean(ctx, false);
        return(1);
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
    if (p_track->pWaypointList ){  // only attempt this if waypoint list of not empty
        wxPlugin_WaypointListNode *linknode = p_track->pWaypointList ->GetFirst();
        for (duk_idx_t i = 0; linknode; linknode = linknode->GetNext(), i++){
            p_waypoint = linknode->GetData();
            ocpn_waypoint_to_js_duk(ctx, p_waypoint);   // construct this waypoint
            duk_put_prop_index(ctx, arr_idx, i);
            }
        }
    duk_put_prop_literal(ctx, -2, "waypoints");
    return(1);
    }

static duk_ret_t addTrack(duk_context *ctx) { // add the track to OpenCPN
    PlugIn_Route *p_track;

    bool permanent = true;  // permanent by default
    bool result;
    duk_idx_t nargs;
    nargs = duk_get_top(ctx);   // number of arguments in call
    if (nargs == 2) {
        permanent = duk_get_boolean(ctx, -1);   // decide on permanency
        duk_pop(ctx);
        }
    p_track = js_duk_to_opcn_route(ctx);    // construct the opcn track (using the same function as for route
    result = AddPlugInRoute(p_track, permanent);
    if (!result) duk_push_boolean(ctx, false);  //  failed
    else duk_push_string(ctx, p_track->m_GUID);  // else return the GUID
    // now track safely stored in OpenCPN, clean up - lists data not otherwise released
    clearWaypointsOutofRoute(p_track);  // works for tracks too
    return(1);
}

static duk_ret_t updateTrack(duk_context *ctx) { // update the track in OpenCPN
    PlugIn_Route *p_track;
    bool result;
    duk_idx_t nargs;

    p_track = js_duk_to_opcn_route(ctx);    // construct the opcn track using the route constructor
    duk_push_boolean(ctx, UpdatePlugInRoute(p_track));  // result will be returned
    clearWaypointsOutofRoute(p_track);      // works for tracks too
    return(1);
}

static duk_ret_t deleteTrack(duk_context *ctx) {  // given a GUID, deletes track
    wxString GUID;
    bool outcome;

    GUID = wxString(duk_to_string(ctx,0));
    outcome = DeletePlugInTrack(GUID);  // temp fix!!!!!!
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

// —————— API registrations follow

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
    duk_push_c_function(ctx, onMessageName, 2 /* args */);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);

    duk_push_string(ctx, "OCPNonNMEAsentence");
    duk_push_c_function(ctx, onNMEAsentence, 1 /* args */);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    
    duk_push_string(ctx, "OCPNonSeconds");
    duk_push_c_function(ctx, OCPNonSeconds, DUK_VARARGS);
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
    
    duk_push_string(ctx, "OCPNcancelAll");
    duk_push_c_function(ctx, cancelAll, 0 /* no args */);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    
    duk_push_string(ctx, "OCPNgetNewGUID");
    duk_push_c_function(ctx, getNewGUID, 0 /* 1 arg */);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    
    duk_push_string(ctx, "OCPNgetWaypointGUIDs");
    duk_push_c_function(ctx, getWaypointGUIDs, 0 /* 0 args */);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    wxArrayString GetWaypointGUIDArray( void );
    
    duk_push_string(ctx, "OCPNgetSingleWaypoint");
    duk_push_c_function(ctx, getSingleWaypoint, 1 /* arg is GUID */);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);

    duk_push_string(ctx, "OCPNaddSingleWaypoint");
    duk_push_c_function(ctx, addSingleWaypoint, DUK_VARARGS);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    
    duk_push_string(ctx, "OCPNupdateSingleWaypoint");
    duk_push_c_function(ctx, updateSingleWaypoint, 1);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);


    duk_push_string(ctx, "OCPNdeleteSingleWaypoint");
    duk_push_c_function(ctx, deleteSingleWaypoint, 1 /* 1 arg */);
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
    
    duk_pop(ctx);
}
