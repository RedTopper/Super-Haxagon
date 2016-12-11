package data;

import java.awt.Color;
import java.io.File;
import java.io.IOException;
import java.nio.BufferUnderflowException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;

import parts.Pattern;
import red.Util;

public class Level {
	public static final String HEADER = "LEVEL2.0";
	public static final String FOOTER = "ENDLEVEL";
	public static final String EXTENSION = ".head";
	
	private File file;
	private String name;
	private String difficulty;
	private String mode;
	private String creator;
	private Color[] bg1 = new Color[2];
	private Color[] bg2 = new Color[2];
	private Color[] fg = new Color[2];
	private float wallSpeed;
	private float rotationSpeed;
	private float humanSpeed;
	private float pulseSpeed;
	private int numberOfPatterns;
	private ArrayList<Pattern> patterns = new ArrayList<>();
	
	public Level(File levelFile, ArrayList<Pattern> patterns) throws IOException {
		file = levelFile;
		ByteBuffer levelRawData = Util.loadBinaryFile(levelFile);
		try {
			Util.checkString(levelRawData, HEADER);
			name = Util.readString(levelRawData);
			difficulty = Util.readString(levelRawData);
			mode = Util.readString(levelRawData);
			creator = Util.readString(levelRawData);
			creator = "REDHAT";
			bg1[0] = Util.readColor(levelRawData);
			bg1[1] = Util.readColor(levelRawData);
			bg2[0] = Util.readColor(levelRawData);
			bg2[1] = Util.readColor(levelRawData);
			fg[0] = Util.readColor(levelRawData);
			fg[1] = Util.readColor(levelRawData);
			wallSpeed = levelRawData.getFloat();
			rotationSpeed = levelRawData.getFloat();
			humanSpeed = levelRawData.getFloat();
			pulseSpeed = levelRawData.getFloat();
			numberOfPatterns = levelRawData.getInt();
			loadPatterns(levelRawData, patterns);
			Util.checkString(levelRawData, FOOTER);
		}  catch(BufferUnderflowException e) {
			System.out.println("The file does not have all of the properties needed to create a level!");
		}
		System.out.println(this);
	}

	public void writeFile() throws IOException {
		ByteBuffer b = ByteBuffer.allocate(getByteLength());
		b.order(ByteOrder.LITTLE_ENDIAN);
		b.put(HEADER.getBytes());
		b.put((byte) name.length()); 		b.put(name.getBytes());
		b.put((byte) difficulty.length()); 	b.put(difficulty.getBytes());
		b.put((byte) mode.length()); 		b.put(mode.getBytes());
		b.put((byte) creator.length()); 	b.put(creator.getBytes());
		b.put(Util.writeColors(bg1));
		b.put(Util.writeColors(bg2));
		b.put(Util.writeColors(fg));
		b.putFloat(wallSpeed);
		b.putFloat(rotationSpeed);
		b.putFloat(humanSpeed);
		b.putFloat(pulseSpeed);
		b.putInt(numberOfPatterns);
		for(Pattern p : patterns) {
			b.put((byte) p.getName().length()); b.put(p.getName().getBytes());
		}
		b.put(FOOTER.getBytes());
		Util.writeBinaryFile(file, b);
	}

	public String toString() {
		StringBuilder r = new StringBuilder();
		r.append("\nLEVEL DATA:");
		r.append("\nLevel Name: " + name);
		r.append("\nDifficulty: " + difficulty);
		r.append("\nMode:       " + mode);
		r.append("\nCreator:    " + creator);
		r.append("\nBackground colors one: " + bg1[0].toString().substring(14) + ", " + bg1[1].toString().substring(14));
		r.append("\nBackground colors two: " + bg2[0].toString().substring(14) + ", " + bg2[1].toString().substring(14));
		r.append("\nForeground colors:     " + fg[0].toString().substring(14) + ", " + fg[1].toString().substring(14));
		r.append("\nWall speed:     " + wallSpeed);
		r.append("\nRotation speed: " + rotationSpeed);
		r.append("\nHuman speed:    " + humanSpeed);
		r.append("\nPulse speed:    " + pulseSpeed);
		r.append("\nAttached Patterns:");
		r.append("\n[");
		String delemeter = "";
		for(int i = 0; i < numberOfPatterns; i++) {
			r.append(delemeter);
			r.append(patterns.get(i).getName());
			delemeter = ", ";
		}
		r.append("]");
		return r.toString();
	}

	private int getByteLength() {
		int patternsLength = 0;
		for(Pattern p : patterns) {
			patternsLength += p.getName().length() + 1;
		}
		return
		HEADER.length() + 				//Size of the header
		name.length() + 1 + 			//Name size, plus the null terminator
		difficulty.length()  + 1 + 		//Difficulty size, plus the null terminator
		mode.length() + 1 + 			//Mode size, plus the null terminator
		creator.length() + 1 + 			//Creator size, plus the null terminator
		Util.COLOR_BYTE_LENGTH * 6 + 	//Size of the 6 colors 
		4 * 4 + 						//wallSpeed, rotationSpeed, humanSpeed, and pulseSpeed
		4 +								//Integer to store the amount of pattern names
		patternsLength +				//Computed size of all strings
		FOOTER.length();				//The footer
	}

	private void loadPatterns(ByteBuffer levelRawData, ArrayList<Pattern> patterns) throws BufferUnderflowException, IOException {
		//load patterns from shared container
		String[] patternNames = new String[numberOfPatterns];
		for(int i = 0; i < numberOfPatterns; i++) {
			patternNames[i] = Util.readString(levelRawData);
			
			//find matching pattern in patterns directory
			boolean patternFound = false;
			for(int j = 0; j < patterns.size(); j++) {
				if(patternNames[i].equals(patterns.get(j).getName())) {
					patternFound = true;
					this.patterns.add(patterns.get(j));
					break;
				}
			}
			
			//if pattern is not found, level cannot be created
			if(!patternFound) throw new IOException("Pattern " + patternNames[i] + " not found in the patterns folder!");
		}
	}
}
