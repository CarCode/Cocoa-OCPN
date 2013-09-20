package at.innoc.roboat.radar;
/**
 * Interface for generic Radar Imaging Data Streams
 * GPL 2.0
 * @author atrox
 *
 */

public interface RadarSource {

	public class RadarDataFrame {
		byte[] data;
		long timeStamp;
		public RadarDataFrame(byte[] data, long timeStamp) {
			this.data = data;
			this.timeStamp = timeStamp;
		}
		public RadarDataFrame() {
			data = null;
			timeStamp = 0;
		}
		public byte[] getData() {
			return data;
		}
		public long getTimeStamp() {
			return timeStamp;
		}
	}
	/**
	 * Returnes the next data frame - whetever it is from a recording or a live feed
	 * @return data or null if no data to be returned. (ie. socket closed)
	 */
	public RadarDataFrame getNextDataFrame();
	
	// TODO: timeout oder so...
	
	/**
	 * Closes this Source
	 */
	public void close();
	
}
