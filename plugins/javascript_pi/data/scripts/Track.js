function Track(){
	if (typeof Waypoint == "undefined") Waypoint = require("Waypoint");
	this.name = "";
	this.from = "";
	this.to = "";
	this.GUID = "";
	this.waypoints = [];
	this.get = function(GUID){
		if (typeof GUID == "undefined") GUID = this.GUID;
		if (typeof GUID == "undefined") throw("Attempting to get waypoint without GUID");
		gotTrack = OCPNgetTrack(GUID);
		if (!gotTrack) return false;
		// the gotTrack is a bare objct without methods.  Will copy into this object
		this.GUID = gotTrack.GUID;
		this.name = gotTrack.name;
		this.from = gotTrack.from;
		this.to = gotTrack.to;
		// need to construct the waypoint objects so they have their methods
		for (i = 0; i <gotTrack.waypoints.length; i++){
			waypoint = new Waypoint();
			waypoint.GUID = gotTrack.waypoints[i].GUID;
			waypoint.markName = gotTrack.waypoints[i].markName;
			waypoint.description = gotTrack.waypoints[i].description;
			waypoint.position.latitude = gotTrack.waypoints[i].position.latitude;
			waypoint.position.longitude = gotTrack.waypoints[i].position.longitude;
			waypoint.creationDateTime = gotTrack.waypoints[i].creationDateTime;
			waypoint.isVisible = gotTrack.waypoints[i].isVisible;
			waypoint.hyperlinkList = gotTrack.waypoints[i].hyperlinkList;
			this.waypoints.push(waypoint);
			delete waypoint;
			}
		delete gotTrack;
		return true;
		}
	this.update = function(){ // update track to OpenCPN
		if (typeof this.GUID == "undefined") throw("Updating track without GUID");
		return (OCPNupdateTrack(this));
		}
	this.add = function(GUID){ // add track to OpenCPN
		if (typeof GUID == "undefined") GUID = this.GUID;
		GUID = OCPNaddTrack(this);
		return (GUID);
		}
	this.purgeWaypoints = function(){
		for (i = 0; i <this.waypoints.length; i++){
			delete this.waypoints[i].position;
			delete this.waypoints[i];
			}
		this.waypoints.length = 0;	// clears the array
		}
	this.delete = function(GUID){ // delete track in OpenCPN
		if (typeof GUID == "undefined") GUID = this.GUID;
		if (typeof GUID == "undefined") throw("Attempting to delete track without GUID");
		return(OCPNdeleteTrack(GUID));
		}
	}