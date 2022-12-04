// This script sends any active route out as WPT and RTE sentences so that iNavX shows the up-to-date active route.
// If the route or its waypoints are updated, iNavX will update to reflect this

Position = require("Position); // load position constructor

//	here we define stuff we need outside the functions
const repeatInterval = 20;		// repeat after this number of seconds
var activeWaypointName = “”;		// to hold the active waypoint name, else “”
var activeRouteName = "";	// the name of the active route
var lastRoutePoint;			// these two are needed to fix up  RMB sentences and synthesise a BOD sentence…
var nextRoutePoint;			// … which is needed so iNavX can work out which leg of the route we are on
var bearingToDest;			// bearing to destination WP from where WP is activated
var fromHere;				// position we are navigating from
var variation;				// magnetic variation
var workingPosition = new Position();	// creates a working position object

OCPNonNMEAsentence(processNMEA);  	// start processing NMEA sentences
listenOut();				// start listening

function listenOut(){
	// print(“Listening out\n”);
	// this is where we start the action
	APRgpx = OCPNgetARPgpx();  // get Active Route Point as GPX
	if (APRgpx.length > 0){
		// we have an Active Route Waypoint
		// print(JSON.stringify(APRgpx), “\n”);
		// need to extract the waypoint name and save for later
		waypointPart  = /<name>.*<\/name>/.exec(APRgpx);
		waypointName = waypointPart[0].slice(6, -7);
		if (waypointName != activeWaypointName){
			// active waypoint has changed
			bearingToDest = "";	// invalidate any previous info
			lastRoutePoint = "";  nextRoutePoint = "";
			activeWaypointName = waypointName;
			// we need to remember our position for navigation from here to next Waypoint
			here = OCPNgetNavigation();
			// print(“JS-control:”, JSON.stringify(here), “\n”);
			fromHere = here.position.value;	 // saves the lat and long
			variation = here.variation.value; //save magnetic variation
			}
		// now get the route leg
		OCPNonMessageName(handleAR, “OCPN_ACTIVE_ROUTELEG_RESPONSE”);
		OCPNsendMessage(“OCPN_ACTIVE_ROUTELEG_REQUEST”);
		}
	else {
	// print(“No active route waypoint in GPX\n”);
	activeWaypointName = “”;
	}
	OCPNonSeconds(listenOut, repeatInterval);	 // Do it again
}

function handleAR(activeRouteJS){
	// we have received the active route, if there is one
	activeRoute = JSON.parse(activeRouteJS);
	// NB the JSON returned creates an array
	if (!activeRoute[0].error ){
		// we have an active route
		routeGUID = activeRoute[0].active_route_GUID;
		// print(“GUID length:”,(routeGUID+ “”).length,	 “ GUID:”, routeGUID, “\n”);// make sure this seen as string
		// NB there is a bug in OpenCPN v5.0 that returns the route name instead of the GUID, such that this would not work
		// It was fixed	 by v5.1.605.
		// we need to check which version we are dealing with
		if (routeGUID.length != 36){ // this is the length of a GUID and, hopefully, not a waypoint name
			// we have hit the bug and got the route name!
			// we will have to retrieve the list of routes instead
			OCPNonMessageName(handleRL, “OCPN_ROUTELIST_RESPONSE”);
			OCPNsendMessage(“OCPN_ROUTELIST_REQUEST”, JSON.stringify({“mode”: “Not track”}));
			}
		else { // we have the active route GUID - get the route
			OCPNonMessageName(handleRT, “OCPN_ROUTE_RESPONSE”);
			OCPNsendMessage(“OCPN_ROUTE_REQUEST”, JSON.stringify({“GUID”:routeGUID}));
			}
		}
	else print(“No active route\n”);	
	}


function handleRL(routeListJS){	 // handle route list response
	routeList = JSON.parse(routeListJS);
	count = routeList.length;
	// there is a null entry at index - don’t know why - so we will count from 1 instad of 0
	// work through the routes looking for the active one
	for (i = 1; i < count; i++){
		if (routeList[i].active){
			// have found the active route - request its details
			routeGUID = routeList[i].GUID;
			requestJS = JSON.stringify({“GUID”:routeGUID});
			OCPNonMessageName(handleRT, “OCPN_ROUTE_RESPONSE”);
			OCPNsendMessage(“OCPN_ROUTE_REQUEST”, requestJS);
			break;
			}	
		}
	}

function handleRT(routeJS){
	activeWPL = "";	// we use this to repeat the active waypoint after the BOD sentence
	route = JSON.parse(routeJS);
	if (route.name != activeRouteName){
		// this is either the first time or the active oute has changed
		activeRouteName = route.name;
		lastRoutePoint = ""; nextRoutePoint = "";  // clear out previous route info
		}
//	wpCount = route.waypoints.length;
	// we work through the route points, sending out WPL sentences and noting which is the next and last
	for (i  in route.waypoints){	// push out the WPT sentences
		workingPosition.value.latitude = route.waypoints[i].lat;
		workingPosition.value.longitude = route.waypoints[i].lon;
		sentence = “$GPWPL” + “,” + workingPosition.NMEA() + route.waypoints[i].Name;
		// print(sentence, “\n”);
		OCPNpushNMEA(sentence); 
		if (route.waypoints[i].Name == activeWaypointName){
			activeWPL = sentence;	// remember for later
			if (i > 0) {
				// not the first
				lastRoutePoint = route.waypoints[i-1].Name;
				nextRoutePoint = route.waypoints[i].Name;
				}
			else {
				// Still to reach first point
				lastRoutePoint = “”;  // no last point
				nextRoutePoint = route.waypoints[i].Name;
				}
			}
		}	
	// next we build an array of lists of waypoints to go in each RTE sentence as space permits
	available = 79 - 12 - route.Name.length - 3;  // space available in RTE for waypoint names
	spaceLeft = available;
	var wpLists = [“”]; // create our array of waypoint groups to go in an RTE sentence
	listIndex = 0;
	for (i in route.waypoints){
		wpName = route.waypoints[i].Name;
		wpNameLength = wpName.length;
		if (spaceLeft >= wpNameLength){
			wpLists[listIndex] += (wpName + “,”);
			spaceLeft -= (wpNameLength+1);	//allow for comma
			continue;
			}
		else{
			// no more space in this one
			wpLists[listIndex] = wpLists[listIndex].slice(0,-1);  // drop trailing comma
			wpLists.push(“”);  // new array member starts empty
			listIndex += 1; spaceLeft = available;
			i -= 1; // don’t forget this last waypoint still to be fitted in next time
			}
		}
	// we may have a trailing comma after last one
	lastOne = wpLists[listIndex];
	lastChar = lastOne.charAt(lastOne.length - 1);
	if (lastChar == “,”) lastOne = lastOne.slice(0,-1); // drop it
	wpLists[listIndex] = lastOne;
	arrayCount = wpLists.length;

	for (i in wpLists) { // send out the RTE sentences
		sentence = “$GPRTE,” + arrayCount + “,” + (i+1) + “,c,” + route.Name + “,” + wpLists[i];
		// print(sentence, “\n”);
		OCPNpushNMEA(sentence);
		}	
	// Now to send a BOD sentence
	if (bearingToDest != ""){ 	// can only do this if we have acquired a bearing - else wait until we have it
		bearingToDest = bearingToDest*1;  // very odd - have to force this to be number for next bit
		bearingToDestM = bearingToDest + variation;
		sentence = “$GPBOD” + “,” + bearingToDest.toFixed(2) + “,” + “T” + “,” + bearingToDestM.toFixed(2) + “,” + “M” + “,” + nextRoutePoint + “,” + lastRoutePoint;
		OCPNpushNMEA(sentence);
		if (activeWPL != "") OCPNpushNMEA(activeWPL);  // repeat the active WPL fer th BOD, as per MacENC
		}
	}

function processNMEA(input){	// we need to un-abbreviate the waypoint name in APB sentences
	if(input.OK && (activeRouteName != "")){ // only bother if have active waypoint
		switch (input.value.slice(0,6)) {
		case “$OCRMB”:
			{
			if (nextRoutePoint == "") break;	// we cannot act until we have this
			splut = input.value.split(“,”, 20);
			// print(“Array length:”, splut.length, “ “, JSON.stringify(splut), “\n”);
			shortWPname = splut[5];
			if (activeWaypointName.startsWith(shortWPname)){  // we check this really is the right one
				splut[0] = “$GPRMB”;		// give it our branding
				splut[4] = lastRoutePoint; // and add the origin WP name
				splut[5] = nextRoutePoint;	// the full destination WP Name
				bearingToDest = splut[11];	// remember the bearing
				result = splut.join(“,”);  // put it back together
				OCPNpushNMEA(result);
				}
			break;
			}
		case “$OCAPB”:
			{
			// print("APB received\n");
			splut = input.value.split(“,”, 20);
			splut[0] = "$GPAPB";		// give it our branding
			splut[10] = nextRoutePoint;  // the full destination WP Name
			result = splut.join(“,”);  // put it back together
			OCPNpushNMEA(result);
			break;
			}
		default:
			break;
			}
		}
	OCPNonNMEAsentence(processNMEA); // Listen out for another NMEA sentence
	}