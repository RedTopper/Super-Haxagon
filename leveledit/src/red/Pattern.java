package red;

import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;

public class Pattern {
	ArrayList<Wall> walls = new ArrayList<>();
	byte levelFlags = 0;
	
	public Pattern() {
	}
	
	public Pattern(FileInputStream file) throws UnsupportedOperationException {
		try {
			byte[] len = new byte[5];
			file.read(len, 0, 5);
			ByteBuffer bb = ByteBuffer.wrap(len);
			bb.order(ByteOrder.LITTLE_ENDIAN);
			int length = bb.getInt();
			levelFlags = bb.get();
			for(int i = 0; i < length; i++) {
				walls.add(new Wall(file));
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	public void writeObject(FileOutputStream file) {
		ByteBuffer bb = ByteBuffer.allocate(5);
		bb.order(ByteOrder.LITTLE_ENDIAN);
		bb.putInt(walls.size());
		bb.put(levelFlags);
		try {
			file.write(bb.array());
		} catch (IOException e) {
			e.printStackTrace();
		}
		for(Wall wall : walls) {
			wall.writeObject(file);
		}
	}
	
	public ArrayList<Wall> getWalls() {
		return walls;
	}
	
	public void setLevelFlags(byte levelFlags) {
		this.levelFlags = levelFlags;
	}
	
	public void setLevelFlag(int level) {
		if(level < 0 || level > 5) {
			System.out.println("I can't do that!"); return;
		}
		levelFlags = (byte) (levelFlags | (1 << level));
	}
	
	public void clearLevelFlags() {
		levelFlags = 0;
	}
	
	public String toString() {
		String levelFlags =  		 "Level:    {";
		String side =  				 "Sides:    {";
		String distanceFromCenter =  "Distance: {";
		String length =				 "Length:   {";
		boolean ran = false;
		for(Wall wall : walls) {
			ran = true;
			side = side + padRight(wall.side,4) + ",";
			distanceFromCenter = distanceFromCenter + padRight(wall.distanceFromCenter,4) + ",";
			length = length + padRight(wall.length,4) + ",";
		}
		for(int i = 0; i < 6; i++) {
			levelFlags += ((this.levelFlags >> i & 1) > 0 ? i + ":y ," : i + ":n ,");
		}
		if(ran) {
			return levelFlags.substring(0, levelFlags.length() - 1) + "}\n"
					+ side.substring(0, side.length() - 1) + "}\n"
					+ distanceFromCenter.substring(0, side.length() - 1) + "}\n"
					+ length.substring(0, side.length() - 1) + "}";
		} else {
			return levelFlags.substring(0, levelFlags.length() - 1) + "}\n" +
					side + "}\n" + distanceFromCenter + "}\n" + length + "}";
		}
	}
	
	public static String padRight(int number, int len) {
	     return String.format("%1$-" + len + "d", number);  
	}
}
