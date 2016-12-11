package config;

import java.awt.Color;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.nio.BufferUnderflowException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.channels.FileChannel;
import java.util.ArrayList;

import parts.Pattern;
import red.Util;

public class Level {
	public static final String LEVEL_HEADER = "LEVEL2.0";
	
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
	
	String[] patternNames;
	Pattern[] patterns;
	
	public Level(File levelFile) throws IOException {
		FileInputStream levelInputStream = new FileInputStream(levelFile);
		FileChannel levelChannel = levelInputStream.getChannel();
		ByteBuffer levelRawData = ByteBuffer.allocate((int)levelChannel.size());
		levelChannel.read(levelRawData);
		levelRawData.rewind();
		levelRawData.order(ByteOrder.LITTLE_ENDIAN);
		createLevel(levelRawData);
		System.out.println(this);
		levelInputStream.close();
	}

	private void createLevel(ByteBuffer levelRawData) throws IOException {
		try {
			Util.checkString(levelRawData, LEVEL_HEADER);
			name = Util.readString(levelRawData);
			difficulty = Util.readString(levelRawData);
			mode = Util.readString(levelRawData);
			creator = Util.readString(levelRawData);
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
			patternNames = new String[numberOfPatterns];
			patterns = new Pattern[numberOfPatterns];
			for(int i = 0; i < numberOfPatterns; i++) {
				
			}
		}  catch(BufferUnderflowException e) {
			System.out.println("The file does not have all of the properties needed to create a level!");
		}
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
		r.append("\nPusle speed:    " + pulseSpeed);
		return r.toString();
	}
}
