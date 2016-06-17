package leveledit;

import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;

public class Pattern {
	ArrayList<Wall> walls = new ArrayList<>();
	
	public void addWall(Wall wall) {
		walls.add(wall);
	}
	
	public Pattern(byte[] read) throws UnsupportedOperationException {
		ByteBuffer bb = ByteBuffer.wrap(read);
		bb.order(ByteOrder.LITTLE_ENDIAN);
		int length = bb.getInt();
		for(int i = 0; i < length; i++) {
			byte wall[] = new byte[3 * 2];
			for(int b = 0; b < 3 * 2; b++) {
				wall[b] = read[b + i * (3 * 2)];
			}
			walls.add(new Wall(wall));
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
}
