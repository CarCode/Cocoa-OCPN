package at.innoc.roboat.radar.control;

/**
 * Stellt einen Zoomlevel dar. 
 * Beinhaltet Namen und interne Daten zum Zoom level; wird von LiveControl verwaltet.
 * @author atrox
 *
 */
public class ZoomLevel {
	public String name;
	public int index;
	
	protected byte[] data;
	
	/**
	 * Interner Konstruktor
	 * @param index
	 * @param name
	 * @param data
	 */
	protected ZoomLevel(int index,String name,byte[] data) {
		this.name=name;
		this.index=index;
		this.data=data;
	}
	
	/**
	 * Name des Zoom Levels
	 * @return
	 */
	public String getName() {
		return name;
	}
	
	/**
	 * Zur Feststellung einer Reihenfolge 
	 * @return
	 */
	public int getIndex() {
		return index;
	}

	/**
	 * Interne Daten - Command code für Radar
	 */
	protected byte[] getData() {
		return data;
	}

	public String toString() {
		return name;
	}
	
}


