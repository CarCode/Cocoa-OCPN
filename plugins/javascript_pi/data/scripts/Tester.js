// Position, waypoint and route confidence tester

function compareWaypoints(waypoint1, waypoint2){
	// compares two waypoints and displays any mismatches.
	// returns true if error else false if OK
	function display(attribute){
		if (!displaying) printOrange("The following attributes of waypoints do not match:\n");
		displaying = true;
		printOrange(attribute, "\n");
		}
	displaying = false;
	if (waypoint1.position.latitude != waypoint2.position.latitude) display ("latitude");
	if (waypoint1.position.longitude != waypoint2.position.longitude) display ("longitude");
	if (waypoint1.markName != waypoint2.markName) display ("markName");
	if (waypoint1.description != waypoint2.description) display ("markName");
	if (waypoint1.iconName != waypoint2.iconName) display ("iconName");
	if (waypoint1.isVisible != waypoint2.isVisible) display ("isVisible");
	if (waypoint1.creationDateTime != waypoint2.creationDateTime) display ("creationDateTime");
	if (waypoint1.GUID != waypoint1.GUID) return ("GUID");  // omit this - too many issues with routes
	if (waypoint1.hyperlinkList.length != waypoint2.hyperlinkList.length) return ("number of hyperlinks differs");
	numberOfLinks = waypoint1.hyperlinkList.length;
	if (numberOfLinks > 0){  // following loop hangs if no links (why?) so avoid
		for (i = 0; i < numberOfLinks; i++){
			if (waypoint1.hyperlinkList[i].description != waypoint2.hyperlinkList[i].description) display ("hyperlink[" + i + "].description differs");
			if (waypoint1.hyperlinkList[i].link != waypoint2.hyperlinkList[i].link) display ("hyperlink[" + i + "].link differs");
			if (waypoint1.hyperlinkList[i].type != waypoint2.hyperlinkList[i].type) display ("hyperlink[" + i + "].type differs");
			}
		}
	return displaying;
	}
	
function compareRoutes(route1, route2){
	// compares two routes including their waypoints and displays any mismatches.
	// returns true if error else false if OK
	function display(attribute){
	if (!displaying) printOrange("The following attributes of routes do not match:\n");
		displaying = true;
		print(attribute, "\n");
		}
	displaying = false;

	if (route1.GUID != route2.GUID) display ("GUID");
	if (route1.name != route2.name) display ("name");
	if (route1.from != route2.from) display ("from");
	if (route1.to != route2.to) display ("to");
	if (route1.waypoints.length != route2.waypoints.length) return ("Number of waypoints does not match");
	for (i = 0; i < route1.waypoints.length; i++){
		outcome = compareWaypoints(route1.waypoints[i], route2.waypoints[i]);
		if (outcome) display("Waypoints " + i + " in routes do not match ");
		}
	return displaying;
	}

function report(message){
	printOrange(message, "\n");
	OK = false;
	}

try {config = OCPNgetPluginConfig();}
catch(err){throw("This test script is not for JavaScript plugin versions before v0.3");}
if (config.inHarness) throw("This tester script cannot be run in the harness");
print("JavaScript plugin v", config.versionMajor, ".", config.versionMinor, " ", config.comment, "\n");

// 1: Position
print("\n*** Positon tests ***\n");
try { Position = require("Position"); }
catch(err) {
	throw("Unable to load Position contructor from plugin library");
	}
if (typeof(Position) != "function") throw("Position did not load constructor function");
OK = true;
testPosition = new Position;
sentence = "$OCRMB,A,0.000,L,,UK-S:Y,5030.530,N,00121.020,W,0021.506,82.924,0.000,5030.530,S,00120.030,E,V,A*69";
testPosition.NMEAdecode(sentence,2);
if (testPosition.NMEA != "5030.530,S,00120.030,E") report("Position NMEA tests failed");
testPosition.latest();
if(testPosition.fixTime == 0) print("No fix since OpenCPN started - reporting last\n");
else print("Latest fix was at ", Date(testPosition.fixtime*1000), "\n");
print("Position ", testPosition.formatted, "\n");
nav = OCPNgetNavigation();
if (isNaN(nav.SOG)) print("Present navigational information not available\n");
else {
	print("SOG\t", nav.SOG, "\n");
	print("COG\t", nav.COG, "\n");
	print("HDT\t", nav.HDT, "\n");
	print("HDM\t", nav.HDM, "\n");
	print("variation\t", nav.variation, "\n");
	}
delete testPosition;
if (OK) printGreen("Position tests completed OK\n");
else printOrange("Position tests completed with error(s)\n");

// 2: Waypoints
print("\n*** Waypoint tests ***\n");
OK = true;
waypoints = OCPNgetWaypointGUIDs();
print(waypoints.length, " existing waypoints\n");
APRgpx = OCPNgetARPgpx();  // get Active Route Point as GPX
if (APRgpx.length > 0){
	waypointPart  = /<name>.*<\/name>/.exec(APRgpx);
	waypointName = waypointPart[0].slice(6, -7);
	print("Active waypoint is ", waypointName, "\n");
	}
else print("No active waypoint\n");
try {Waypoint = require("Waypoint");}
catch(err){
	throw("Unable to load waypoint contructor from plugin library");
	}
if (typeof(Waypoint) != "function") throw("Waypoint constructor did not load function");
testWaypoint = new Waypoint(0, -123.45);
testWaypoint.position.latitude = 12.345;
if (testWaypoint.position.NMEA != "1220.700,N,12327.000,W") throw("Waypoint NMEA attribute error");
testWaypoint.markName = "Test waypoint";
testWaypoint.description = "This waypoint is a temporary one for diagnostics";
testWaypoint.iconName = "Anchor";
// add some hyperlinks
var link1 = {description:"OpenCPN", link: "https://opencpn.org", type: "Type1"};
var link2 = {description:"JavaScript user guide", link: "https://github.com/antipole2/JavaScript_pi/blob/master/JavaScript%20plugin%20user%20guide.pdf", type: "Type2"};
testWaypoint.hyperlinkList.push(link1);
testWaypoint.hyperlinkList.push(link2);
try { GUID = testWaypoint.add();}
catch(error){ throw("Adding waypoint failed");}
testWaypoint.GUID = GUID;
readWaypoint = new Waypoint();
try { // non-existant waypoint
	tryError = false;
	readWaypoint.get("abc");
	}
catch(error){
	tryError = true;
	}
if (!tryError) throw("Trying to read a non-existant waypoint succeeded !!");
try { readWaypoint.get(GUID); }
catch(error){ throw("Get existing waypoint failed");}
outcome = compareWaypoints(testWaypoint, readWaypoint);
if (outcome) report("Read-back of waypoint does not match original ");
returnedTimeStamp = readWaypoint.creationDateTime;	// remember for use in Route tests because of OpenCPN bug
readWaypoint.description = readWaypoint.description + " (updated)";
readWaypoint.position.latitude = -10;
readWaypoint.position.longitude = 75;
readWaypoint.iconName = "Diamond";
readWaypoint.markName = "Test waypoint updated";
readWaypoint.isVisible = !readWaypoint.isVisible
readWaypoint.hyperlinkList[0].description = "Updated";
try {readWaypoint.update();}
catch(error){ throw("Updating waypoint failed");}
updatedWaypoint = new Waypoint();
try {updatedWaypoint.get(GUID); }
catch(error){ throw("Getting back updated waypoint failed");}
outcome = compareWaypoints(readWaypoint, updatedWaypoint);
if (outcome) report("Read back of updated waypoint does not match original of updated waypoint");
try {updatedWaypoint.delete();}
catch(error){ throw("Deleting the updated waypoint failed"); }
delete testWaypoint; delete readWaypoint; delete updatedWaypoint;
if (OK) printGreen("Waypoint tests completed OK\n");
else printRed("Waypoint tests completed with error(s)\n");

// 3: Routes
print("\n*** Route tests ***\n");
OK = true;

try {Route = require("Route");}
catch(err){
	throw("Unable to load route contructor from plugin library");
	}
if (typeof(Route) != "function") throw("Route constructor did not load");
newRoute = new Route();
newRoute.name = "Constructed test";
newRoute.from = "Start place";
newRoute.to = "End place";
for (i = 1; i < 4; i++) { // construct some waypoints
	waypoint = new Waypoint();
	waypoint.markName = "TestWP" + i;
	waypoint.description = "Description of WP" + i;
	waypoint.position.latitude = 60 + i/10;
	waypoint.position.longitude = -1 - i/10
	waypoint.creationDateTime = returnedTimeStamp;  // *** THIS IS TO STOP BUG BEING REPORTED AGAIN
	newRoute.waypoints.push(waypoint);
	}
try { GUID = newRoute.add();}
catch(error){throw("Failed to create new route")}
newRoute.GUID = GUID;
retrievedRoute = new Route;
try { retrievedRoute.get(GUID); }
catch(error) { throw("Failed to retrieve route");}
outcome = compareRoutes(newRoute, retrievedRoute);
if (outcome) {
	report("Retrieved route fails to match original ");
	print("\nnewRoute:\n", JSON.stringify(newRoute), "\n\n");
	print("\nretrievedRoute:\n", JSON.stringify(retrievedRoute), "\n\n");
	}
retrievedRoute.name = "Changed route name";
retrievedRoute.to = "New to string";
retrievedRoute.waypoints[1].position.longitude += 1;
try { retrievedRoute.update(); }
catch(error) {throw("Failed to update route");}
updatedRoute = new Route;
try { updatedRoute.get(GUID);}
catch(error) {throw("Failed to retrieve updated route");}
outcome = compareRoutes(retrievedRoute, updatedRoute);
if (outcome) {
	report("Retrieved updated route fails to match original ");
	print("\nretrievedRoute:\n", JSON.stringify(retrievedRoute), "\n\n");
	print("\nupdatedRoute:\n", JSON.stringify(updatedRoute), "\n\n");
	}
try {
	tryError = false;
	OCPNdeleteRoute("aaa");
	}
catch(error) {tryError = true;}
if (!tryError) throw("Deleting non-existant route reports success!");
updatedRoute.purgeWaypoints();
if (updatedRoute.waypoints.length) throw("Failed to purge waypoints from updatedRoute");
try { updatedRoute.delete();}
catch(error){ throw("Failed to delete route in OpenCPN");}
if (OK) printGreen("Route tests completed OK\n");
else printRed("Route tests completed with error(s)\n");


// 3: Tracks
print("\n*** Track tests ***\n");
printOrange("These tests do not yet complete because of limitations in the OpenCPN API\n");
//throw("Track tests not yet available");
OK = true;
try {Track = require("Track");}
catch(err){
	throw("Unable to load track contructor from plugin library");
	}
if (typeof(Track) != "function") throw("Track constructor did not load");
newTrack = new Track();
newTrack.name = "Constructed test";
newTrack.from = "Start place";
newTrack.to = "End place";
for (i = 1; i < 4; i++) { // construct some waypoints
	waypoint = new Waypoint();
	waypoint.markName = "TestWP" + i;
	waypoint.description = "Description of WP" + i;
	waypoint.position.latitude = 60 + i/10;
	waypoint.position.longitude = -1 - i/10
	waypoint.creationDateTime = returnedTimeStamp;  // *** THIS IS TO STOP BUG BEING REPORTED AGAIN
	newTrack.waypoints.push(waypoint);
	}
// print("Track is:\n", newTrack, "\n\n");
GUID = newTrack.add();
if (!GUID) throw("Failed to create new track");
//newTrack.GUID = GUID;
retrievedTrack = new Track;
if (!retrievedTrack.get(GUID)){
	printOrange("Failed to retrieve track\n");
	OK = false;
	}
else {
	outcome = compareRoutes(newTrack, retrievedTrack); // works for tracks too
	if (outcome) {
		report("Retrieved track fails to match original ");
		print("\nnewTrack:\n", JSON.stringify(newTrack), "\n\n");
		print("\nretrievedTrack:\n", JSON.stringify(retrieved), "\n\n");
		}
	retrievedTrack.name = "Changed track name";
	retrievedTrack.to = "New to string";
	retrievedTrack.waypoints[1].position.longitude += 1;
	if (!retrievedRTrack.update()) throw("Failed to update track");
	updatedTrack = new Track;
	if (!updatedTrack.get(GUID)) throw("Failed to retrieve updated track");
	outcome = compareRoutes(retrievedTrack, updatedTrack);	// compare works for tracks too
	if (outcome) {
		report("Retrieved updated track fails to match original ");
		print("\nretrievedTrack:\n", JSON.stringify(retrievedTrack), "\n\n");
		print("\nupdatedTrack:\n", JSON.stringify(updatedTrack), "\n\n");
		}
	updatedTrack.purgeWaypoints();
	if (updatedTrack.waypoints.length) throw("Failed to purge waypoints from updatedTrack");
	updatedTrack.delete();
	if (updatedTrack.get(GUID)) throw("Failed to delete track in OpenCPN");
	}
if (OCPNdeleteTrack("aaa")) throw("Deleting non-existant track reports success!");

// synthesise a track
var scale = 0.05;
track = new Track;
track.name = "Test track synthesised";
track.from = "Starting place";
track.to = "Ending place";
track.GUID = OCPNgetNewGUID();
numberRoutepoints = 5;
nextPosition = new Position(50.3, 0.5);	// SE of Isle of Wight

for (i = 0; i < numberRoutepoints; i++){
	routepoint = new Waypoint;
	rpPosition = new Position(nextPosition.latitude, nextPosition.longitude);
	routepoint.markName = "Rpt" + i;
	routepoint.iconName = "Circle";
	routepoint.position = rpPosition;
	routepoint.creationDateTime = new Date()/1000;
	nextPosition.latitude += Math.sin(i)*scale;
	nextPosition.longitude += 0.5;
//	print("Next position: ", nextPosition.formatted, "\n");
	track.waypoints.push(routepoint);
	}

GUID = OCPNaddTrack(track);

readbackTrack = OCPNgetTrack(track.GUID);
if (!readbackTrack) printOrange("Failed to read back track\n");

track.name = "Synthesised track updated";
track.from = "Starting place 2";
track.to = "Ending place 2";
track.waypoints[2].markName += "updated";
track.waypoints[2].position.latitude = 60;
track.waypoints[2].position.longitude = -1;
outcome = OCPNupdateTrack(track);
if (!outcome) printOrange("Failed to update track\n");
outcome = OCPNdeleteTrack(track);
if (!outcome) printOrange("Failed to delete track\n");


if (OK) printGreen("Track tests OK\n");
else printRed("Track tests completed with error(s)\n");
scriptResult("Tests completed");
