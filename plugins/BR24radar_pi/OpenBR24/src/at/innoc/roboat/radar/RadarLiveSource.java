package at.innoc.roboat.radar;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.InetAddress;
import java.net.MulticastSocket;
import java.net.SocketException;
import java.net.UnknownHostException;
import at.innoc.roboat.radar.control.LiveControl;

/**
 * 
 * Implements the RadarSource for a actual present device.
 * 
 * Copyright 2011 by Adrian Dabrowski, Sebastian Busch
 * Licensed under GPL 2.0
 * 
 * @author atrox
 */
public class RadarLiveSource implements RadarSource, Runnable {

	private MulticastSocket socket;
	private DatagramPacket datagram;
	private static Thread keepAliveThread=null;
	private static boolean keepAliveStop;
	private static LiveControl control;
	
	
	public RadarLiveSource(LiveControl controlchannel) throws UnknownHostException, IOException {
		control=controlchannel;
		
		socket=new MulticastSocket(6678);
		socket.joinGroup(InetAddress.getByName("236.6.7.8"));
		
		datagram=new DatagramPacket(new byte[80000], 80000);
		keepAliveStop=false;
		if (keepAliveThread==null) {
			keepAliveThread=new Thread(this);
			keepAliveThread.setDaemon(true);
			keepAliveThread.start();
		}
		
	}
	
	/**
	 * gibt einen empfangenen Buffer zurück
	 */
	@Override
	public RadarDataFrame getNextDataFrame() {
		byte[] ret;
		try {
			socket.receive(datagram);
			long time = System.currentTimeMillis();
			ret=java.util.Arrays.copyOfRange(datagram.getData(), datagram.getOffset(), datagram.getLength());
			return new RadarDataFrame(ret, time);
		} catch (SocketException se) {
			System.out.println(se+"");
			return new RadarDataFrame();
		} catch (IOException e) {
			e.printStackTrace();
			return new RadarDataFrame();
		}		
	}

	@Override
	public void close() {
		keepAliveStop=true;
		try {
			socket.leaveGroup(InetAddress.getByName("236.6.7.8"));
		} catch (UnknownHostException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		} 
		socket.close();
		
	}

	
	/**
	 * Sends keep alive packages
	 */
	@Override
	public void run() {
		while (!keepAliveStop) {
			try {
				Thread.sleep(1000);
				control.sendKeepAlive();
			} catch ( Exception e) {}
		}
	}

}
