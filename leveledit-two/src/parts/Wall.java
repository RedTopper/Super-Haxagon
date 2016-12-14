package parts;

public class Wall {
	public static final int BYTE_LENGTH = 3 * 2; //3 chars consisting of 2 bytes each
	private char distance;
	private char height;
	private char side;
	
	public enum Set {
		DISTANCE,
		HEIGHT,
		SIDE
	}
	
	public Wall() {
		this.distance = 20;
		this.height = 4;
		this.side = 0;
	}
	
	public Wall(char distance, char height, char side) {
		this.distance = distance;
		this.height = height;
		this.side = side;
	}
	
	public char getDistance() {
		return distance;
	}
	
	public char getHeight() {
		return height;
	}
	
	public char getSide() {
		return side;
	}
	
	public void setDistance(int dist) {
		this.distance = (char)dist;
	}
	
	public void setHeight(int height) {
		this.height = (char)height;
	}
	
	public void setSide(int side) {
		this.side = (char)side;
	}
	
	public String toString() {
		return "[dist=" + (int)distance + ",height=" + (int)height + ",side=" + (int)side + "]";
	}
}
