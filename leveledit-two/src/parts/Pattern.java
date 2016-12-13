package parts;

import java.io.File;
import java.io.IOException;
import java.nio.BufferUnderflowException;
import java.nio.ByteBuffer;
import java.util.ArrayList;

import red.Dynamic;
import red.Util;

public class Pattern {
	public static final String HEADER = "PATTERN1.0";
	public static final String FOOTER = "ENDPATTERN";
	public static final String EXTENSION = ".ptn";
	
	private File patternFile;
	private int sidesRequired;
	private ArrayList<Wall> walls = new ArrayList<>();
	
	public Pattern(File patternFile, int sidesRequired) {
		this.patternFile = patternFile;
		this.sidesRequired = 6;
	}
	
	public Pattern(File patternFile) throws IOException {
		this.patternFile = patternFile;
		ByteBuffer patternRawData = Util.readBinaryFile(patternFile);
		try {
			Util.checkString(patternRawData, HEADER);
			//this.sidesRequired = patternRawData.getInt();
			loadWalls(patternRawData);
			Util.checkString(patternRawData, FOOTER);
		} catch(BufferUnderflowException e) {
			System.out.println("The file does not have all of the properties needed to create a level!");
			throw e;
		}
	}
	
	public void writeFile() throws IOException {
		Dynamic d = new Dynamic();
		d.putRawString(HEADER);
		d.putInt(sidesRequired);
		d.putWalls(walls);
		d.putRawString(FOOTER);
		d.write(patternFile);
	}

	public String toString() {
		return patternFile.getName();
	}

	private void loadWalls(ByteBuffer patternRawData) {
		int numberOfWalls = patternRawData.getInt();
		for(int i = 0; i < numberOfWalls; i++) {
			walls.add(new Wall(patternRawData.getChar(), patternRawData.getChar(), patternRawData.getChar()));
		}
	}
}
