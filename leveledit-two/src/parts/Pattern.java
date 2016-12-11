package parts;

import java.io.File;
import java.io.IOException;
import java.nio.BufferUnderflowException;
import java.nio.ByteBuffer;
import java.util.ArrayList;

import red.Util;

public class Pattern {
	public static final String HEADER = "PATTERN1.0";
	public static final String FOOTER = "ENDPATTERN";
	public static final String EXTENSION = ".ptn";
	
	private File file;
	private ArrayList<Wall> walls = new ArrayList<>();
	
	public Pattern(File patternFile) throws IOException {
		file = patternFile;
		createPattern(Util.loadBinaryFile(patternFile));
	}
	
	private void createPattern(ByteBuffer patternRawData) throws BufferUnderflowException, IOException {
		Util.checkString(patternRawData, HEADER);
		int numberOfWalls = patternRawData.getInt();
		for(int i = 0; i < numberOfWalls; i++) {
			walls.add(new Wall(patternRawData.getChar(), patternRawData.getChar(), patternRawData.getChar()));
		}
		Util.checkString(patternRawData, FOOTER);
	}

	public String getName() {
		return file.getName();
	}
	
	public void writeFile() {
		
	}
	
	public int getByteLength() {
		return
		HEADER.length() + 	//Size of the header
		4 + 				//Integer to represent how many walls there are
		Wall.BYTE_LENGTH +  //size of a wall
		FOOTER.length(); 	//Size of the footer
	}
}
