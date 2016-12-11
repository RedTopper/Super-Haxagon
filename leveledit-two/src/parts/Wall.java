package parts;

public class Wall {
	public static final int BYTE_LENGTH = 3 * 2; //3 chars consisting of 2 bytes each
	private char distance;
	private char height;
	private char side;
	
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
}
