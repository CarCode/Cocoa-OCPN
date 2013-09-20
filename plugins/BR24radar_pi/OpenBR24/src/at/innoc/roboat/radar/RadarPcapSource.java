package at.innoc.roboat.radar;

import org.jnetpcap.Pcap;
import org.jnetpcap.app.IpReassemblyExample;
import org.jnetpcap.app.IpReassemblyExample.IpReassemblyBuffer;
import org.jnetpcap.app.IpReassemblyExample.IpReassemblyBufferHandler;
import org.jnetpcap.nio.JMemory.Type;
import org.jnetpcap.packet.JMemoryPacket;
import org.jnetpcap.packet.JPacket;
import org.jnetpcap.packet.PcapPacket;
import org.jnetpcap.protocol.network.Ip4;
import org.jnetpcap.protocol.tcpip.Udp;


/**
 * 
 * Implements a RadarSource, pulls data from an PCAP file
 * 
 * Copyright 2011 by Adrian Dabrowski, Sebastian Busch
 * Licensed under GPL 2.0
 * 
 */
public class RadarPcapSource implements RadarSource, IpReassemblyBufferHandler {
	private Pcap pcap=null;
	private IpReassemblyExample asm=null;
	private volatile boolean hasReassembledData=false;
	private byte[] data=null;
	private Ip4 ip;
	/*
	 * Time stretch factor
	 * Values equal or less than zero result in replay as fast as possible.
	 */
	private double timeStretch = 1;
	
	/**
	 * Constructor
	 * @param filename
	 * @param rate (ms) - how many ms between each packet
	 */
	public RadarPcapSource(String filename, double timeStretch) {
		StringBuilder errbuf = new StringBuilder();
		
		pcap=Pcap.openOffline(filename, errbuf);
		
		asm=new IpReassemblyExample(10000,this); // 10 sec timeout for orphan fragments
		ip=new Ip4();
		this.timeStretch = timeStretch;
	}
	
	public RadarPcapSource(String string) {
		this(string, 1.0);
	}

	long startTimeStamp=0;
	long currentFrameTimeStamp=0;
	long firstFrameTimeStamp=0;
	/**
	 * from Interface RadarSource
	 */
	@Override
	public RadarDataFrame getNextDataFrame() {
		PcapPacket packet = new PcapPacket(65536);
		//Udp udp=new Udp();
		
		while (!hasReassembledData) {
			if (pcap.nextEx(packet) < 1) { 
				return new RadarDataFrame(); 
			}
			if (packet.hasHeader(ip) /* && packet.hasHeader(udp) */ ) {  // damn! fragments don't have an udp header (!)
				/* if (udp.destination() == 6678 ) { */
					if (ip.destinationToInt() == 0xec060708) { // dest.ip==236.6.7.8 
						asm.nextPacket(packet, null); // user objects seems useless
					}
				/* }  */
			}			
		}
		currentFrameTimeStamp = packet.getCaptureHeader().timestampInMillis();
		//System.out.println(new Date(currentFrameTimeStamp));
		if(timeStretch > 0) {
			if(firstFrameTimeStamp == 0) {
				firstFrameTimeStamp = currentFrameTimeStamp;
			}
			long time = System.currentTimeMillis();
			if(startTimeStamp == 0) {
				startTimeStamp = time;
			}
			long playTimeStamp = Math.round(startTimeStamp + (currentFrameTimeStamp - firstFrameTimeStamp) * timeStretch);
			if (playTimeStamp > time) {
				try {
					Thread.sleep(playTimeStamp - time);
				} catch (InterruptedException e) {
				}
			}
		}	
		
		hasReassembledData = false;
		return new RadarDataFrame(data, currentFrameTimeStamp);
	}

	
	/**
	 * a reassembled datagram arrived from the Reassembler - event handler
	 * from Interface IpReassemblyBufferHandler
	 */
	@Override
	public void nextIpDatagram(IpReassemblyBuffer buffer) {
		if (buffer.isComplete() == false) return;
        JPacket packet = new JMemoryPacket(Type.POINTER);
        packet.peer(buffer);
        packet.getCaptureHeader().wirelen(buffer.size());
        packet.getCaptureHeader().caplen(buffer.size());
        packet.scan(Ip4.ID); // decode the packet
        Ip4 ip = packet.getHeader(new Ip4());
        ip.checksum(ip.calculateChecksum());
        Udp udp= packet.getHeader(new Udp());
		data=udp.getPayload();
		hasReassembledData=true;
	}

	/**
	 * Closes Data Stream
	 * from RadarSource
	 */
	@Override
	public void close() {
		pcap.close();
	}

	/**
	 * Returns Time Stretch
	 * @return
	 */
	public double getTimeStretch() {
		return timeStretch;
	}
	
	/**
	 * sets Time Stretch Factor. 
	 * 2.. two times slower
	 * 1.. real speed
	 * 0.5 .. two times faster
	 * 0 .. as fast as possible
	 * @param ts
	 */
	public void setTimeStretch(double ts) {
		timeStretch=ts;
	}

}
