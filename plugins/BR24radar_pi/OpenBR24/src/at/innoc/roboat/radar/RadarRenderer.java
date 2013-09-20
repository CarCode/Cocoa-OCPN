package at.innoc.roboat.radar;

import java.awt.Graphics;
import java.awt.image.BufferedImage;
import java.awt.image.WritableRaster;
import java.lang.reflect.InvocationTargetException;

/**
 * 
 * Renders scan lines into a bitmap
 * 
 * Copyright 2011 by Adrian Dabrowski, Sebastian Busch
 * Licensed under GPL 2.0
 * 
 */
public class RadarRenderer implements RadarLineListener {

	RadarRendererListener listener;
	RadarSource source;
	RadarFrameDecoder decoder;
	BufferedImage img;
	WritableRaster raster;
	
	int imgw=1024;
	int imgh=1024;
	int midx,midy;
	float scale=1;
	
	int rgbtype=BufferedImage.TYPE_INT_RGB;
	
	
	/**
	 * Creates an Image Renderer for an Image, by selecting source and decoder-class
	 * Sets an renderer listener.
	 * @param whichSource source
	 * @param whichDecoder class of radar decoder
	 * @param listener null if unused
	 */
	public RadarRenderer (RadarSource whichSource, Class<RadarFrameDecoder> whichDecoder, RadarRendererListener listener) {
		
		//TODO: make image size changeable
		//TODO make color selectable
		img=new BufferedImage(imgw,imgh,rgbtype);
		raster=img.getRaster();
		midx=imgw/2;
		midy=imgh/2;
		
		this.listener=listener;
		source=whichSource;
		try {
			decoder= whichDecoder.getConstructor(RadarSource.class,RadarLineListener.class).newInstance(whichSource,this);
		} catch (IllegalArgumentException e) {
			e.printStackTrace();
		} catch (SecurityException e) {
			e.printStackTrace();
		} catch (InstantiationException e) {
			e.printStackTrace();
		} catch (IllegalAccessException e) {
			e.printStackTrace();
		} catch (InvocationTargetException e) {
			e.printStackTrace();
		} catch (NoSuchMethodException e) {
			e.printStackTrace();
		}
		
		//decoder=new whichDecoder(source,this);
	}
	
	public RadarRenderer (RadarSource whichSource, RadarRendererListener listener) {
		this(whichSource,RadarFrameDecoder.class,listener);
	}

	
	public BufferedImage getImage() {
		return img;
	}
	
	private int min(int a, int b, int c, int d) {
		int r=a;
		if (b<r) r=b;
		if (c<r) r=c;
		if (d<r) r=d;
		return r;
	}
	
	private int max(int a, int b, int c, int d) {
		int r=a;
		if (b>r) r=b;
		if (c>r) r=c;
		if (d>r) r=d;
		return r;
	}
	
	private int min(int a, int b, int c) {
		int r=a;
		if (b<r) r=b;
		if (c<r) r=c;
		return r;
	}
	
	private int max(int a, int b, int c) {
		int r=a;
		if (b>r) r=b;
		if (c>r) r=c;
		return r;
	}
	
	private int min(int a, int b) {
		int r=a;
		if (b<r) r=b;
		return r;
	}
	
	private int max(int a, int b) {
		int r=a;
		if (b>r) r=b;
		return r;
	}
	
	
	int framecount=0;
	long lastredraw=0;
	int lastdrawnangle=0;
	/**
	 * We delay some of the redraws, so we do not cause excessive CPU usage by updating the screen to often.
	 */
	@Override
	public void endOfFrame() {
		long t=System.currentTimeMillis();
		if (framecount++>5) {
			//if (lastredraw<t-100) {
				int x1=(int) (midx+Math.cos(Math.toRadians(lastdrawnangle))*512);
				int y1=(int) (midy+Math.sin(Math.toRadians(lastdrawnangle))*512);
				int x2=(int) (midx+Math.cos(Math.toRadians(oldangle+1))*512);
				int y2=(int) (midy+Math.sin(Math.toRadians(oldangle+1))*512);
				
				int minx=max(min(x1,x2,midx)-1,0);
				int miny=max(min(y1,y2,midy)-1,0);
				int maxx=min(max(x1,x2,midx)+1,imgw);
				int maxy=min(max(y1,y2,midy)+1,imgh);
				
				listener.onImageUpdate(lastdrawnangle, oldangle+1, minx, miny, maxx-minx, maxy-miny);
				lastdrawnangle=oldangle;
				lastredraw=t;
				framecount=0;
			//}
		}
	}

	
	
	int oldzoom=0;
	int oldangle=0;
	int[] pixel=new int[3];
	/**
	 * Actually draw all the pixels
	 */
	public void newRadarLine(float angle, int zoom, byte[] linedata) {
		
		if (zoom!=oldzoom) handleZoomChange(oldzoom,zoom);
		oldzoom=zoom;
		oldangle=(int)Math.floor(angle);
		
		float rad=(float) Math.toRadians(angle);
		double radcos=Math.cos(rad);
		double radsin=Math.sin(rad);
		
		for (int i=0;i<512;i++) {
			pixel[1]=linedata[i]&0xff;
			
			float r=i;//*scale;
			
			float x=(float) (midx+radcos*r);
			float y=(float) (midy+radsin*r);
			
			raster.setPixel((int)x, (int)y, pixel);
		}		
	}
	
	/**
	 * When a zoom level change (change of radar range) happens, we try to preserve the allready scanned data by resizing the image.
	 * @param oldzoom
	 * @param newzoom
	 */
	private void handleZoomChange(int oldzoom, int newzoom) {
		float factor=(float)oldzoom/(float)newzoom;
		BufferedImage image2=new BufferedImage(imgw,imgh,rgbtype);
		Graphics g2=image2.getGraphics();
		// resize image
		g2.drawImage(img, (int)(midx-midx*factor), (int)(midy-midy*factor), (int)(midx+midx*factor), (int)(midy+midy*factor), 
				0, 0, imgw, imgh, null);
		raster=image2.getRaster();
		img=image2;
		
		
		listener.onZoomChange(newzoom);
		listener.onImageUpdate(0, 360, 0, 0, imgw, imgh); // redraw all
	}
	
	
	public void onClose() {
		listener.onClose();
	}
	
	
}
