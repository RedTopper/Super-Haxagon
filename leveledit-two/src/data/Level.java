package data;

import java.awt.Color;
import java.io.File;
import java.io.IOException;
import java.nio.BufferUnderflowException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;

import javax.swing.JColorChooser;

import parts.Pattern;
import red.LevelEditTwo;
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
	
	public Level(File projectPath) {
		try {
			name = LevelEditTwo.prompt("[String] Level Name").toUpperCase();
			difficulty = LevelEditTwo.prompt("[String] Difficulty").toUpperCase();
			mode = LevelEditTwo.prompt("[String] Apparent Mode").toUpperCase();
			creator = LevelEditTwo.prompt("[String] Your Name/Username").toUpperCase();
			bg1 = new Color[Integer.parseInt(LevelEditTwo.prompt("[int] Amount of background one colors"))];
			bg2 = new Color[Integer.parseInt(LevelEditTwo.prompt("[int] Amount of background two ccolors"))];
			fg = new Color[Integer.parseInt(LevelEditTwo.prompt("[int] Amount of foreground colors"))];
			System.out.println("OK! Creating color chooser dialogue box. ");
			System.out.println("If you do not see it, try moving some windows on your desktop!");
			pickColors();
			wallSpeed = Float.parseFloat(LevelEditTwo.prompt("[float] Wall advancement speed"));
			rotationSpeed = Float.parseFloat(LevelEditTwo.prompt("[float] Level rotation speed\nWill be TAU/this! Enter 0.0 for no rotation"));
			if(rotationSpeed != 0.0) rotationSpeed = (float) ((Math.PI * 2.0) / (double)rotationSpeed);
			humanSpeed = Float.parseFloat(LevelEditTwo.prompt("[float] Human rotation speed\nWill be TAU/this! Enter 0.0 for TAU/240.0"));
			if(humanSpeed != 0.0) humanSpeed = (float) (Math.PI * 2.0 / (double)humanSpeed); else humanSpeed = (float) (Math.PI * 2.0 / 240.0);
			pulseSpeed = Float.parseFloat(LevelEditTwo.prompt("[float] Pulse rate"));
			numberOfPatterns = 0;
			file = new File(projectPath, name.toLowerCase() + EXTENSION);
			System.out.println(this);
			if(LevelEditTwo.prompt("[Y/N] Write this configuration?").toUpperCase().equals("Y")) {
				writeFile();
			}
		} catch (Exception e) {
			
		}
	}

	private void pickColors() {
		for(int i = 0; i < bg1.length; i++) {
			bg1[i] = JColorChooser.showDialog(null, "BACKGROUND COLOR ONE: COLOR " + (i + 1), Color.BLACK);
		}
		for(int i = 0; i < bg2.length; i++) {
			bg2[i] = JColorChooser.showDialog(null, "BACKGROUND COLOR TWO: COLOR " + (i + 1), Color.GRAY);
		}
		for(int i = 0; i < fg.length; i++) {
			fg[i] = JColorChooser.showDialog(null, "FOREGROUND COLOR: COLOR " + (i + 1), Color.WHITE);
		}
	}

	public Level(File levelFile, ArrayList<Pattern> patterns) throws IOException {
		file = levelFile;
		ByteBuffer levelRawData = Util.readBinaryFile(levelFile);
		try {
			Util.checkString(levelRawData, HEADER);
			name = Util.readString(levelRawData);
			difficulty = Util.readString(levelRawData);
			mode = Util.readString(levelRawData);
			creator = Util.readString(levelRawData);
			bg1 = Util.readColors(levelRawData);
			bg2 = Util.readColors(levelRawData);
			fg = Util.readColors(levelRawData);
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
		Util.putString(b, name);
		Util.putString(b, difficulty);
		Util.putString(b, mode);
		Util.putString(b, creator);
		b.put(Util.putColors(bg1));
		b.put(Util.putColors(bg2));
		b.put(Util.putColors(fg));
		b.putFloat(wallSpeed);
		b.putFloat(rotationSpeed);
		b.putFloat(humanSpeed);
		b.putFloat(pulseSpeed);
		b.putInt(numberOfPatterns);
		for(Pattern p : patterns) {
			Util.putString(b, p.getName());
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
		r.append("\nBackground colors one: " + Util.getColorAsString(bg1));
		r.append("\nBackground colors two: " + Util.getColorAsString(bg2));
		r.append("\nForeground colors:     " + Util.getColorAsString(fg));
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
		bg1.length * Util.COLOR_BYTE_LENGTH + 1 +
		bg2.length * Util.COLOR_BYTE_LENGTH + 1 +
		fg.length * Util.COLOR_BYTE_LENGTH + 1 +
		4 * 4 + 						//wallSpeed, rotationSpeed, humanSpeed, and pulseSpeed
		4 +								//Integer to store the amount of pattern names
		patternsLength +				//Computed size of all strings
		FOOTER.length();				//The footer
	}

	private void loadPatterns(ByteBuffer levelRawData, ArrayList<Pattern> patterns) throws IOException {
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
