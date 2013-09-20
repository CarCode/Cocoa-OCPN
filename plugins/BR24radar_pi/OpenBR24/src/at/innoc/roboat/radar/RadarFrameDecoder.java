package at.innoc.roboat.radar;

import at.innoc.roboat.radar.RadarSource.RadarDataFrame;


/**
 * Copyright 2011 by Adrian Dabrowski, Sebastian Busch
 * Licensed under GPL 2.0
 * 
 * @author Adrian Dabrowski, Sebastian Busch
 *
 *
 * Decodes radar frames into single radar scan lines
 *
 */
public class RadarFrameDecoder implements Runnable {
	Thread thread;
	RadarSource source;
	RadarLineListener handler;
	
	static final double SQRT2=Math.sqrt(2);
	
	public RadarFrameDecoder(RadarSource s, RadarLineListener h) {
		source=s;
		handler=h;
		thread=new Thread(this);
		thread.setDaemon(true);
		thread.start();
		
	}
	
	
	/**
	 * Suspend current thread, until RadarFrameDecoder received all frames.
	 */
	public void join() {
		try {
			thread.join();
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
	}
	

	@Override
	public void run() {
		boolean rawhandler=handler instanceof RadarRawLineListener;
		RadarDataFrame dataFrame=null;
		byte[] seqheader=new byte[8];
		byte[] lineheader=new byte[24];
		byte[] linedata=new byte[512];
		int i=0;
		
		while ((dataFrame=source.getNextDataFrame()).getData()!=null) {
			int len=dataFrame.getData().length;
			System.arraycopy(dataFrame.getData(), 0, seqheader, 0, 8);
			i=8;
			while(len-i>=536) { // 536 == linedata(512) + lineheader(24)
				System.arraycopy(dataFrame.getData(), i, lineheader, 0, 24);
				System.arraycopy(dataFrame.getData(), i+24, linedata, 0, 512);
				
				int angleraw=(lineheader[9]&0xff)<<8 | (lineheader[8]&0xff);
				float angle=((float)angleraw*360/4096) -90; //TODO, probably should be done in renderer
				
				int rr=lineheader[23]&0xff; // rr
				
				int sstt=(lineheader[13]&0xff)<<8|(lineheader[12]&0xff);
				
				int meter;				
				meter=(int)((double)sstt*10/SQRT2);
				
				if (rawhandler)
					((RadarRawLineListener)handler).newRawRadarLine(lineheader, linedata);
				
				handler.newRadarLine(angle, meter, linedata);
				
				i+=536;
			}
			handler.endOfFrame();
			
		}
		handler.endOfFrame();
		handler.onClose();
	}

	
	
	
}
