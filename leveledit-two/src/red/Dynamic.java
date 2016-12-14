package red;

import java.awt.Color;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.channels.FileChannel;
import java.util.ArrayList;

import parts.Wall;

public class Dynamic {
	ArrayList<ByteBuffer> buffers = new ArrayList<>();
	
	public void putInt(int i) {
		ByteBuffer buf = ByteBuffer.allocate(4);
		buf.order(ByteOrder.LITTLE_ENDIAN);
		buf.putInt(i);
		buffers.add(buf);
	}
	
	public void putFloat(float f) {
		ByteBuffer buf = ByteBuffer.allocate(4);
		buf.order(ByteOrder.LITTLE_ENDIAN);
		buf.putFloat(f);
		buffers.add(buf);
	}
	
	public void putColors(ArrayList<Color> c) {
		ByteBuffer buf = ByteBuffer.allocate(c.size() * 3 + 1);
		buf.order(ByteOrder.LITTLE_ENDIAN);
		buf.put((byte) c.size());
		for(int i = 0; i < c.size(); i++) {
			buf.put((byte) c.get(i).getRed());
			buf.put((byte) c.get(i).getGreen());
			buf.put((byte) c.get(i).getBlue());
		}
		buffers.add(buf);
	}
	
	public void putString(String string) {
		if(string.length() >= 256) string = string.substring(0, 256);
		ByteBuffer buf = ByteBuffer.allocate(string.length() + 1);
		buf.order(ByteOrder.LITTLE_ENDIAN);
		buf.put((byte) string.length()); 		
		buf.put(string.getBytes());
		buffers.add(buf);
	}
	
	public void putWalls(ArrayList<Wall> walls) {
		ByteBuffer buf = ByteBuffer.allocate(walls.size() * Wall.BYTE_LENGTH + 1);
		buf.order(ByteOrder.LITTLE_ENDIAN);
		buf.put((byte) walls.size());
		for(Wall wall : walls) {
			buf.putChar(wall.getDistance());
			buf.putChar(wall.getHeight());
			buf.putChar(wall.getSide());
		}
		buffers.add(buf);
	}

	public void putRawString(String string) {
		ByteBuffer buf = ByteBuffer.allocate(string.length());
		buf.order(ByteOrder.LITTLE_ENDIAN);
		buf.put(string.getBytes());
		buffers.add(buf);
	}
	
	public void write(File file) throws IOException {
		
		//Delete really old file and move old file into it's place
		File oldDir = Util.getDir(new File(file.getParent() + Util.OLD));
		File backupFile = new File(oldDir, file.getName());
		backupFile.delete();
		file.renameTo(backupFile);
		
		//write new file
		FileOutputStream outputStream = new FileOutputStream(file, false);
		FileChannel channel = outputStream.getChannel();
		for(ByteBuffer buf : buffers) {
			buf.rewind();
			channel.write(buf);
		}
		channel.close();
		outputStream.close();
		System.out.println("Backed up and wrote file: '" + file.getAbsolutePath() + "'");
	}

	public void putByte(int i) {
		ByteBuffer buf = ByteBuffer.allocate(1);
		buf.order(ByteOrder.LITTLE_ENDIAN);
		buf.put((byte)i);
		buffers.add(buf);
	}
	
}
