package data;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.GridLayout;
import java.io.File;
import java.io.IOException;
import java.nio.BufferUnderflowException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;

import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.UIManager;
import javax.swing.UnsupportedLookAndFeelException;
import javax.swing.plaf.nimbus.NimbusLookAndFeel;

import parts.Pattern;
import red.Util;

public class Level {
	public static final String HEADER = "LEVEL2.0";
	public static final String FOOTER = "ENDLEVEL";
	public static final String EXTENSION = ".head";
	
	//If you want to add a variable here, you'll need to add it in these places too:
	//Level(File projectPath) (to ask it from the user)
	//Level(File levelFile, ArrayList<Pattern> patterns) (to parse  it from a file)
	//writeFile() (to write it to a file)
	//toString() (to convert this object to a string)
	//and getByteLength()! (to write this file)
	private File file;
	private String name;
	private String difficulty;
	private String mode;
	private String creator;
	private ArrayList<Color> bg1 = new ArrayList<>();
	private ArrayList<Color> bg2 = new ArrayList<>();
	private ArrayList<Color> fg = new ArrayList<>();
	private float wallSpeed;
	private float rotationSpeed;
	private float humanSpeed;
	private float pulseSpeed;
	private ArrayList<Pattern> patterns = new ArrayList<>();
	private ArrayList<Pattern> availablePatterns;
	
	public Level(File projectPath, String name, ArrayList<Pattern> availablePatterns) {
		this.file = new File(projectPath, name.replaceAll("\\s", "").toLowerCase() + EXTENSION);
		this.name = name.toUpperCase();
		this.difficulty = "UNKNOWN";
		this.mode = "NORMAL";
		this.creator = "ANONYMOUS";
		this.bg1.add(Color.BLACK);
		this.bg2.add(Color.GRAY);
		this.fg.add(Color.WHITE);
		this.wallSpeed = 2.0f;
		this.rotationSpeed = (float)(Math.PI  * 2.0) / 120.0f;
		this.humanSpeed = (float)(Math.PI  * 2.0) / 60.0f;
		this.pulseSpeed = 10.0f;
		this.availablePatterns = availablePatterns;
	}
	
	public void edit() {
		JFrame level = new JFrame("Level Editor for level '" + name + "'");
		try { UIManager.setLookAndFeel(new NimbusLookAndFeel());} catch (UnsupportedLookAndFeelException e) {}
		level.setMinimumSize(new Dimension(530,562));
		level.setLayout(new GridLayout(1,0));
		
		//Left Side Textual Configuration
		JPanel textConfiguration = new JPanel();
		textConfiguration.setLayout(new GridLayout(0,1));
		textConfiguration.setBackground(Util.BACKGROUND);
		Util.addTitledFieldToPanel(textConfiguration, null, "[String] Name", name);
		Util.addTitledFieldToPanel(textConfiguration, null, "[String] Difficulty", difficulty);
		Util.addTitledFieldToPanel(textConfiguration, null, "[String] Mode", mode);
		Util.addTitledFieldToPanel(textConfiguration, null, "[String] Creator", creator);
		Util.addTitledFieldToPanel(textConfiguration, null, "[float] Wall Speed", wallSpeed + "");
		Util.addTitledFieldToPanel(textConfiguration, null, "[TAU/float] Rotation Speed", rotationSpeed + "");
		Util.addTitledFieldToPanel(textConfiguration, null, "[TAU/float] Human Speed", humanSpeed + "");
		Util.addTitledFieldToPanel(textConfiguration, null, "[float] Pulse Speed", pulseSpeed + "");
		Util.addButtonToPanel(textConfiguration, null, "Save Configuration");
		
		//Right Side pattern chooser.
		JPanel patternConfiguration = new JPanel();
		patternConfiguration.setLayout(new GridLayout(0,1));
		
		//The top
		JPanel top = new JPanel();
		top.setLayout(new BorderLayout());
		top.setBackground(Util.BACKGROUND);
		Util.addButtonToPanel(top, BorderLayout.NORTH, "<html><center>Create New Pattern<br/>(Opens Pattern Editor)</center></html>");
		Util.addTitledListToPanel(top, BorderLayout.CENTER, "Available Patterns", availablePatterns);
		Util.addButtonToPanel(top, BorderLayout.SOUTH, "Edit selected pattern");

		//The bottom
		JPanel bot = new JPanel();
		bot.setLayout(new BorderLayout());
		bot.setBackground(Util.BACKGROUND);
		Util.addButtonToPanel(bot, BorderLayout.NORTH, "Add selected pattern to this level");
		Util.addTitledListToPanel(bot, BorderLayout.CENTER, "Level Patterns", patterns);
		Util.addButtonToPanel(bot, BorderLayout.SOUTH,"Remove selected pattern from this level");;
		
		//Assemble everything into the level editor
		patternConfiguration.add(top);
		patternConfiguration.add(bot);
		level.add(textConfiguration);
		level.add(patternConfiguration);
		level.getContentPane().setBackground(Color.BLACK);
		level.pack();
		level.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		level.setVisible(true);
		level.toFront();
		level.requestFocus();
	}

	public Level(File levelFile, ArrayList<Pattern> availablePatterns) throws IOException {
		file = levelFile;
		ByteBuffer levelRawData = Util.readBinaryFile(levelFile);
		try {
			Util.checkString(levelRawData, HEADER);
			this.name = Util.readString(levelRawData);
			this.difficulty = Util.readString(levelRawData);
			this.mode = Util.readString(levelRawData);
			this.creator = Util.readString(levelRawData);
			this.bg1 = Util.readColors(levelRawData);
			this.bg2 = Util.readColors(levelRawData);
			this.fg = Util.readColors(levelRawData);
			this.wallSpeed = levelRawData.getFloat();
			this.rotationSpeed = levelRawData.getFloat();
			this.humanSpeed = levelRawData.getFloat();
			this.pulseSpeed = levelRawData.getFloat();
			loadPatterns(levelRawData, levelRawData.getInt(), availablePatterns);
			this.availablePatterns = availablePatterns;
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
		b.putInt(patterns.size());
		for(Pattern p : patterns) Util.putString(b, p.toString());
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
		for(int i = 0; i < patterns.size(); i++) {
			r.append(delemeter);
			r.append(patterns.get(i).toString());
			delemeter = ", ";
		}
		r.append("]");
		return r.toString();
	}

	/**
	private void pickColors(int bg1size, int bg2size, int fgsize) {
		for(int i = 0; i < bg1size; i++) {
			bg1.add(JColorChooser.showDialog(null, "BACKGROUND COLOR ONE: COLOR " + (i + 1), Color.BLACK));
		}
		for(int i = 0; i < bg2size; i++) {
			bg2.add(JColorChooser.showDialog(null, "BACKGROUND COLOR TWO: COLOR " + (i + 1), Color.GRAY));
		}
		for(int i = 0; i < fgsize; i++) {
			fg.add(JColorChooser.showDialog(null, "FOREGROUND COLOR: COLOR " + (i + 1), Color.WHITE));
		}
	}**/

	private int getByteLength() {
		int patternsLength = 0;
		for(Pattern p : patterns) {
			patternsLength += p.toString().length() + 1;
		}
		return
		HEADER.length() + 							//Size of the header
		name.length() + 1 + 						//Name size, plus the null terminator
		difficulty.length()  + 1 + 					//Difficulty size, plus the null terminator
		mode.length() + 1 + 						//Mode size, plus the null terminator
		creator.length() + 1 + 						//Creator size, plus the null terminator
		bg1.size() * Util.COLOR_BYTE_LENGTH + 1 + 	//Colors plus the extra length byte
		bg2.size() * Util.COLOR_BYTE_LENGTH + 1 +	//For the second background color
		fg.size() * Util.COLOR_BYTE_LENGTH + 1 +	//And the foreground colors
		4 * 4 + 									//wallSpeed, rotationSpeed, humanSpeed, and pulseSpeed
		4 +											//Integer to store the amount of pattern names
		patternsLength +							//Computed size of all strings
		FOOTER.length();							//The footer
	}

	private void loadPatterns(ByteBuffer levelRawData, int numberOfPatterns, ArrayList<Pattern> availablePatterns) throws IOException {
		
		//load patterns from shared container
		String[] patternNames = new String[numberOfPatterns];
		for(int i = 0; i < numberOfPatterns; i++) {
			patternNames[i] = Util.readString(levelRawData);
			
			//find matching pattern in patterns directory
			boolean patternFound = false;
			for(int j = 0; j < availablePatterns.size(); j++) {
				if(patternNames[i].equals(availablePatterns.get(j).toString())) {
					patternFound = true;
					this.patterns.add(availablePatterns.get(j));
					break;
				}
			}
			
			//if pattern is not found, level cannot be created
			if(!patternFound) throw new IOException("Pattern " + patternNames[i] + " not found in the patterns folder!");
		}
	}
}
