package at.innoc.roboat.radar.control;
/**
 * Allows Control of Radar Functions 
 * (Zoom levels, Power, Keep alive, etc..)
 * @author atrox - Adrian Dabrowski
 */

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.Inet4Address;
import java.net.InetAddress;
import java.net.MulticastSocket;
import java.net.UnknownHostException;

public class LiveControl {
	private java.net.DatagramSocket socket=null;
	private InetAddress addr;
	private int port=6680;
	private static ZoomLevel[] zoomlevels=new ZoomLevel[]{
			new ZoomLevel(0, "50m", new byte[]{0x03,(byte)0xc1,(byte)0xf4,0x01,0,0}), // 1f4=500dm
			new ZoomLevel(1, "75m", new byte[]{0x03,(byte)0xc1,(byte)0xee,0x02,0,0}), // 2ee=750dm
			new ZoomLevel(2, "100m", new byte[]{0x03,(byte)0xc1,(byte)0xee,0x03,0,0}),
			new ZoomLevel(3, "250m", new byte[]{0x03,(byte)0xc1,(byte)0xc4,(byte)0x09,0,0}),
			new ZoomLevel(4, "500m", new byte[]{0x03,(byte)0xc1,(byte)0x88,(byte)0x13,0,0}),
			new ZoomLevel(5, "750m", new byte[]{0x03,(byte)0xc1,(byte)0x4c,(byte)0x1d,0,0}),
			new ZoomLevel(6, "1km", new byte[]{0x03,(byte)0xc1,(byte)0x10,(byte)0x27,0,0}),
			new ZoomLevel(7, "1.5km", new byte[]{0x03,(byte)0xc1,(byte)0x98,(byte)0x3a,0,0}),
			new ZoomLevel(8, "2km", new byte[]{0x03,(byte)0xc1,(byte)0x20,(byte)0x4e,0,0}),
			new ZoomLevel(9, "3km", new byte[]{0x03,(byte)0xc1,(byte)0x30,(byte)0x75,0,0}),
			new ZoomLevel(10, "4km", new byte[]{0x03,(byte)0xc1,(byte)0x40,(byte)0x9c,0,0}),
			new ZoomLevel(11, "6km", new byte[]{0x03,(byte)0xc1,(byte)0x60,(byte)0xea,0,0}),
			new ZoomLevel(12, "8km", new byte[]{0x03,(byte)0xc1,(byte)0x80,(byte)0x38,1,0}),
			new ZoomLevel(13, "12km", new byte[]{0x03,(byte)0xc1,(byte)0xc0,(byte)0xd4,1,0}),
			new ZoomLevel(14, "16km", new byte[]{0x03,(byte)0xc1,(byte)0x00,(byte)0x71,2,0}),
			new ZoomLevel(15, "24km", new byte[]{0x03,(byte)0xc1,(byte)0x80,(byte)0xa9,3,0}) // 03a980=240000dm
			//TODO: new ZoomLevel(16, "36km", new byte[]{0x03,(byte)0xc1,(byte)0x80,(byte)0xa9,3,0})	
		};
	
	public void LiveControl() {	
		try {
			addr=InetAddress.getByAddress(new byte[]{(byte)236,6,7,10});
		} catch (UnknownHostException e) {
			e.printStackTrace();
		}
	}
	
	/**
	 * Opens a control channel to the SimRad device 
	 * @throws IOException
	 */
	public void open() throws IOException {
		socket=new DatagramSocket();
		socket.connect(InetAddress.getByAddress(new byte[]{(byte)236,6,7,10}), port);
	}
	
	/**
	 * Closes the control channel
	 */
	public void close() {
		socket.close();
		socket=null;
	}
	
	/**
	 * Sends keep alive Info to Radar
	 * otherwise the Radar will stop after ~ 10 sec
	 */
	public void sendKeepAlive() throws IOException {
		DatagramPacket dp=new DatagramPacket(new byte[]{(byte)0xA0,(byte)0xC1},2,addr,port);
		socket.send(dp);
/*		dp=new DatagramPacket(new byte[]{0x03,(byte)0xC2},2,addr,port);
		socket.send(dp);
		dp=new DatagramPacket(new byte[]{0x04,(byte)0xC2},2,addr,port);
		socket.send(dp);
		dp=new DatagramPacket(new byte[]{0x05,(byte)0xC2},2,addr,port);
		socket.send(dp);*/
	}
	
	/**
	 * Turns the Radar on and off
	 * @param onoff, true to turn it on
	 * @throws IOException
	 */
	public void Power(boolean onoff) throws IOException {
		if (onoff) {
			DatagramPacket dp=new DatagramPacket(new byte[]{0x00,(byte)0xc1,0x01},3,addr,port);
			socket.send(dp);
			dp=new DatagramPacket(new byte[]{0x01,(byte)0xc1,0x01},3,addr,port);
			socket.send(dp);
		} else {
			DatagramPacket dp=new DatagramPacket(new byte[]{0x00,(byte)0xc1,0x00},3,addr,port);
			socket.send(dp);
			dp=new DatagramPacket(new byte[]{0x01,(byte)0xc1,0x00},3,addr,port);
			socket.send(dp);
			
		}
	}
	
	/**
	 * Return a list of all possible Zoom levels
	 * @return
	 */
	public static ZoomLevel[] getAvailableZoomSettings () {
		return zoomlevels;
	}
	
	/** 
	 * Set a specific Zoom Level
	 */
	public void setZoom(int i) throws IOException {
		byte[] data=zoomlevels[i].getData();
		DatagramPacket dp=new DatagramPacket(data,data.length,addr,port);
		socket.send(dp);
	}
	
}
