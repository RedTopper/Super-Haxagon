package data;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import java.io.IOException;
import java.nio.BufferUnderflowException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;

import javax.swing.JFrame;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.UIManager;
import javax.swing.UnsupportedLookAndFeelException;
import javax.swing.plaf.nimbus.NimbusLookAndFeel;

import parts.Pattern;
import red.JListData;
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
	private String music;
	private ArrayList<Color> bg1 = new ArrayList<>();
	private ArrayList<Color> bg2 = new ArrayList<>();
	private ArrayList<Color> fg = new ArrayList<>();
	private float wallSpeed;
	private float rotationSpeed;
	private float humanSpeed;
	private float pulseSpeed;
	private ArrayList<Pattern> patterns = new ArrayList<>();
	private ArrayList<Pattern> availablePatterns;
	
	/**
	 * Default level parameters.
	 * @param projectPath The folder of the project
	 * @param name Project name (also file name)
	 * @param availablePatterns A list of all of the loaded patterns
	 */
	public Level(File projectPath, String name, ArrayList<Pattern> availablePatterns) {
		this.file = new File(projectPath, name.replaceAll("\\s", "").toLowerCase() + EXTENSION);
		this.name = name.toUpperCase();
		this.difficulty = "UNKNOWN";
		this.mode = "NORMAL";
		this.creator = "ANONYMOUS";
		this.music = "NONE";
		this.bg1.add(Color.BLACK);
		this.bg2.add(Color.GRAY);
		this.fg.add(Color.WHITE);
		this.wallSpeed = 2.0f;
		this.rotationSpeed = (float)(Math.PI  * 2.0) / 120.0f;
		this.humanSpeed = (float)(Math.PI  * 2.0) / 60.0f;
		this.pulseSpeed = 10.0f;
		this.availablePatterns = availablePatterns;
	}
	
	/**
	 * Loads a level from a file.
	 * @param levelFile The file to load.
	 * @param availablePatterns A list of all loaded patterns
	 * @throws IOException
	 */
	public Level(File levelFile, ArrayList<Pattern> availablePatterns) throws IOException {
		file = levelFile;
		ByteBuffer levelRawData = Util.readBinaryFile(levelFile);
		try {
			Util.checkString(levelRawData, HEADER);
			this.name = Util.readString(levelRawData);
			this.difficulty = Util.readString(levelRawData);
			this.mode = Util.readString(levelRawData);
			this.creator = Util.readString(levelRawData);
			this.music = Util.readString(levelRawData);
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
	}

	/**
	 * Writes this object to a file.
	 * @throws IOException
	 */
	public void writeFile() throws IOException {
		ByteBuffer b = ByteBuffer.allocate(getByteLength());
		b.order(ByteOrder.LITTLE_ENDIAN);
		b.put(HEADER.getBytes());
		Util.putString(b, name);
		Util.putString(b, difficulty);
		Util.putString(b, mode);
		Util.putString(b, creator);
		Util.putString(b, music);
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
		music.length() + 1 +					//Name of the music file to load
		bg1.size() * Util.COLOR_BYTE_LENGTH + 1 + 	//Colors plus the extra length byte
		bg2.size() * Util.COLOR_BYTE_LENGTH + 1 +	//For the second background color
		fg.size() * Util.COLOR_BYTE_LENGTH + 1 +	//And the foreground colors
		4 * 4 + 									//wallSpeed, rotationSpeed, humanSpeed, and pulseSpeed
		4 +											//Integer to store the amount of pattern names
		patternsLength +							//Computed size of all strings
		FOOTER.length();							//The footer
	}

	/**
	 * Launches the editor.
	 */
	public void edit() {
		//JColorChooser.showDialog(null, "FOREGROUND COLOR: COLOR " + (i + 1), Color.WHITE);
		try { UIManager.setLookAndFeel(new NimbusLookAndFeel());} catch (UnsupportedLookAndFeelException e) {}
		JFrame level = new JFrame("Level Editor for level '" + name + "'");
		level.setMinimumSize(new Dimension(530,620));
		level.setLayout(new GridLayout(1,0));
		
		//Left Side Textual Configuration
		JPanel textConfiguration = new JPanel();
		textConfiguration.setLayout(new GridLayout(0,1));
		textConfiguration.setBackground(Util.BACKGROUND);
		JTextField jname = Util.addTitledFieldToPanel(textConfiguration, null, "[String] Name", name);
		JTextField jdiff = Util.addTitledFieldToPanel(textConfiguration, null, "[String] Difficulty", difficulty);
		JTextField jmode = Util.addTitledFieldToPanel(textConfiguration, null, "[String] Mode", mode);
		JTextField jcrea = Util.addTitledFieldToPanel(textConfiguration, null, "[String] Creator", creator);
		JTextField jmuse = Util.addTitledFieldToPanel(textConfiguration, null, "[String] Music File + extension", music);
		JTextField jwall = Util.addTitledFieldToPanel(textConfiguration, null, "[float] Wall Speed", wallSpeed + "");
		JTextField jrota = Util.addTitledFieldToPanel(textConfiguration, null, "[TAU/float] Rotation Step", rotationSpeed + "");
		JTextField jhuma = Util.addTitledFieldToPanel(textConfiguration, null, "[TAU/float] Human Step", humanSpeed + "");
		JTextField jpuls = Util.addTitledFieldToPanel(textConfiguration, null, "[float] Pulse Speed", pulseSpeed + "");
		Util.addButtonToPanel(textConfiguration, null, "Save Configuration", new  ActionListener() {
			public void actionPerformed(ActionEvent e) {
				try {
					name = Util.updateText(jname);
					difficulty = Util.updateText(jdiff);
					mode = Util.updateText(jmode);
					creator = Util.updateText(jcrea);
					music = jmuse.getText();
					wallSpeed = Float.parseFloat(jwall.getText());
					rotationSpeed = Float.parseFloat(jrota.getText());
					humanSpeed= Float.parseFloat(jhuma.getText());
					pulseSpeed = Float.parseFloat(jpuls.getText());
					int result = JOptionPane.showOptionDialog(level, "Do you really want to save this information?\nThe current file will be backed up.", 
								 "Really save?", JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE, null, null, JOptionPane.YES_OPTION);
					if(result == JOptionPane.YES_OPTION) {
						writeFile();
					}
				} catch (Exception ex) {
					JOptionPane.showMessageDialog(level, "Error updating the level!\nCheck console for details.", "Error!", JOptionPane.ERROR_MESSAGE);
					ex.printStackTrace();
				}
			}
		});
		
		//Right Side pattern chooser.
		JPanel patternConfiguration = new JPanel();
		patternConfiguration.setLayout(new GridLayout(0,1));
		
		//The top
		JPanel top = new JPanel();
		top.setLayout(new BorderLayout());
		top.setBackground(Util.BACKGROUND);
		Util.addButtonToPanel(top, BorderLayout.NORTH, "<html><center>Create new pattern<br/>(Opens Pattern Editor)</center></html>", new  ActionListener() {
			public void actionPerformed(ActionEvent e) {
				
			}
		});
		JListData javai = Util.addTitledListToPanel(top, BorderLayout.CENTER, "Available Patterns", availablePatterns);
		Util.addButtonToPanel(top, BorderLayout.SOUTH, "<html><center>Edit available pattern<br/>(Opens Pattern Editor)</center></html>", new  ActionListener() {
			public void actionPerformed(ActionEvent e) {
				
			}
		});
	
		//The bottom
		JPanel bot = new JPanel();
		bot.setLayout(new BorderLayout());
		bot.setBackground(Util.BACKGROUND);
		JListData jleve = Util.addTitledListToPanel(bot, BorderLayout.CENTER, "Level Patterns", patterns);
		Util.addButtonToPanel(bot, BorderLayout.NORTH, "Add available pattern", new  ActionListener() {
			public void actionPerformed(ActionEvent e) {
				
				//check if the user actually selected something
				String selection = javai.list.getSelectedValue();
				if(selection == null) return;
				
				//check if the pattern is real
				Pattern selectedPattern = null;
				for(Pattern p : availablePatterns) {
					if(p.toString().equals(selection)) 
						selectedPattern = p;
				}
				if(selectedPattern == null) return;
				
				//add it
				patterns.add(selectedPattern);
				jleve.model.removeAllElements();
				for(Pattern p : patterns) {
					jleve.model.addElement(p.toString());
				}
			}
		});
		Util.addButtonToPanel(bot, BorderLayout.SOUTH,"Remove level pattern", new  ActionListener() {
			public void actionPerformed(ActionEvent e) {
				
				//check if the user actually selected something
				int selection = jleve.list.getSelectedIndex();
				if(selection < 0) return;
				
				//remove it
				for(int i = 0; i < patterns.size(); i++) {
					if(patterns.get(i).toString().equals(jleve.model.getElementAt(selection))) {
						patterns.remove(i);
						break;
					}
				}
				jleve.model.remove(selection);
			}
		});
		
		//Assemble everything into the level editor
		patternConfiguration.add(top);
		patternConfiguration.add(bot);
		level.add(textConfiguration);
		level.add(patternConfiguration);
		level.getContentPane().setBackground(Color.BLACK);
		level.pack();
		level.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
		level.setVisible(true);
		level.toFront();
		level.requestFocus();
	}

	/**
	 * Loads the patterns into this level (the ones needed)
	 * @param levelRawData The raw data  of the file.
	 * @param numberOfPatterns The number of patterns read.
	 * @param availablePatterns All available patterns.
	 * @throws IOException
	 */
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
