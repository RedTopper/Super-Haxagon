package leveledit;

import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;

public class Pattern {
	ArrayList<Wall> walls = new ArrayList<>();
	
	public Pattern() {
	}
	
	public Pattern(FileInputStream file) throws UnsupportedOperationException {
		try {
			byte[] len = new byte[4];
			file.read(len, 0, 4);
			ByteBuffer bb = ByteBuffer.wrap(len);
			bb.order(ByteOrder.LITTLE_ENDIAN);
			int length = bb.getInt();
			for(int i = 0; i < length; i++) {
				walls.add(new Wall(file));
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	public void writeObject(FileOutputStream file) {
		ByteBuffer bb = ByteBuffer.allocate(4);
		bb.order(ByteOrder.LITTLE_ENDIAN);
		bb.putInt(walls.size());
		try {
			file.write(bb.array());
		} catch (IOException e) {
			e.printStackTrace();
		}
		for(Wall wall : walls) {
			wall.writeObject(file);
		}
	}
	
	public void addWall(Wall wall) {
		walls.add(wall);
	}
	
	public int size() {
		return walls.size();
	}
}
