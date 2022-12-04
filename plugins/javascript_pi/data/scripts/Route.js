function Route(){
	var i;	// local scope only
	if (typeof Waypoint == "undefined") Waypoint = require("Waypoint");
	this.name = "";
	this.from = "";
	this.to = "";
	this.GUID = "";
	this.waypoints = [];
	this.get = function(GUID){
		if (typeof GUID == "undefined") GUID = this.GUID;
		if (typeof GUID == "undefined") throw("Attempting to get waypoint without GUID");
		gotRoute = OCPNgetRoute(GUID);
		if (!gotRoute) return false;
		// the gotRoute is a bare objct without methods.  Will copy into this object
		this.GUID = gotRoute.GUID;
		this.name = gotRoute.name;
		this.from = gotRoute.from;
		this.to = gotRoute.to;
		// need to construct the waypoint objects so they have their methods
		for (i = 0; i <gotRoute.waypoints.length; i++){
			waypoint = new Waypoint();
			waypoint.GUID = gotRoute.waypoints[i].GUID;
			waypoint.markName = gotRoute.waypoints[i].markName;
			waypoint.description = gotRoute.waypoints[i].description;
			waypoint.position.latitude = gotRoute.waypoints[i].position.latitude;
			waypoint.position.longitude = gotRoute.waypoints[i].position.longitude;
			waypoint.creationDateTime = gotRoute.waypoints[i].creationDateTime;
			waypoint.isVisible = gotRoute.waypoints[i].isVisible;
			waypoint.hyperlinkList = gotRoute.waypoints[i].hyperlinkList;
			this.waypoints.push(waypoint);
			delete waypoint;
			}
		delete gotRoute;
		return true;
		}
	this.update = function(){ // update route to OpenCPN
		if (typeof this.GUID == "undefined") throw("Updating route without GUID");
		return (OCPNupdateRoute(this));
		}
	this.add = function(GUID){ // add route to OpenCPN
		if (typeof GUID == "undefined") GUID = this.GUID;
		GUID = OCPNaddRoute(this);
		return (GUID);
		}
	this.purgeWaypoints = function(){
		for (i = 0; i <this.waypoints.length; i++){
			delete this.waypoints[i];
			}
		this.waypoints.length = 0;	// clears the array
		}
	this.delete = function(GUID){ // delete route in OpenCPN
		if (typeof GUID == "undefined") GUID = this.GUID;
		if (typeof GUID == "undefined") throw("Attempting to delete route without GUID");
		return(OCPNdeleteRoute(GUID));
		}
	}