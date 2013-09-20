package at.innoc.roboat.radar;

/**
 * 
 * Interface for a render listener. Gets updates when the rendered radar image changed.
 * 
 * Copyright 2011 by Adrian Dabrowski, Sebastian Busch
 * Licensed under GPL 2.0
 * 
 */
public interface RadarRendererListener {

	/**
	 * A piece of the Image has been updated and is ready to be redrawn
	 * @param fromAngle
	 * @param toAngle
	 * @param x
	 * @param y
	 * @param w
	 * @param h
	 */
	public void onImageUpdate(int fromAngle, int toAngle,int x, int y, int w, int h);
	
	/**
	 * Zoom Factor changed.
	 * @param meter
	 */
	public void onZoomChange(int meter);
	
	/**
	 * Source seized to exist
	 */
	public void onClose();
	
	
}
