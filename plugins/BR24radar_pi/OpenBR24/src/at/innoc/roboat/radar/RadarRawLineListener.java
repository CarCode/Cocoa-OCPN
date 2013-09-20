package at.innoc.roboat.radar;

/**
 * 
 * A "raw line listener" does not only get the decoded scan lines, but also the raw data
 * 
 * Copyright 2011 by Adrian Dabrowski, Sebastian Busch
 * Licensed under GPL 2.0
 * 
 */
public interface RadarRawLineListener extends RadarLineListener {
	
	public void newRawRadarLine(byte[] headerdata, byte[] linedata);
		
}
