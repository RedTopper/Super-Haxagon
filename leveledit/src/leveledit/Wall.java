package leveledit;

import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class Wall {
	short distanceFromCenter, side, length;
	public Wall(short side, short distanceFromCenter, short length) {
		this.side = side;
		this.distanceFromCenter = distanceFromCenter;
		this.length = length;
	}
	
	public Wall(byte[] read) throws UnsupportedOperationException {
		if(read.length != 3 * 2) {
			throw new UnsupportedOperationException("Byte array is not 3 \"2 byte\" shorts");
		}
		ByteBuffer bb = ByteBuffer.wrap(read);
		bb.order(ByteOrder.LITTLE_ENDIAN);
		side = bb.getShort();
		distanceFromCenter = bb.getShort();
		length = bb.getShort();
	}
	
	public void writeObject(FileOutputStream file) {
		ByteBuffer bb = ByteBuffer.allocate(3 * 2);
		bb.order(ByteOrder.LITTLE_ENDIAN);
		bb.putShort(side);
		bb.putShort(distanceFromCenter);
		bb.putShort(length);
		try {
			file.write(bb.array());
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
}
