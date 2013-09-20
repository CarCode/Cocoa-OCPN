/**
 * Consumer of an Radar Data Line
 */
package at.innoc.roboat.radar;

/**
 * 
 * Interface for a radar scan line consumer, eg a display 
 * 
 * Copyright 2011 by Adrian Dabrowski, Sebastian Busch
 * Licensed under GPL 2.0
 * 
 * @author atrox
 */
public interface RadarLineListener {

	/**
	 * Gets a new Radar Scanline
	 * @param angle
	 * @param scale // in meters 
	 * @param linedata
	 * 
	 */
	public void newRadarLine(float angle, int scale, byte[] linedata);
	
	/**
	 * An frame of multiple scanlines just ended - its now time for some updates - eg. display
	 */
	public void endOfFrame();
	
	/**
	 * The stream has ended
	 */
	public void onClose();
	
}
