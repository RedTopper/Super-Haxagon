package data;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;
import java.io.File;
import java.io.IOException;
import java.nio.BufferUnderflowException;
import java.nio.ByteBuffer;
import java.util.ArrayList;

import javax.swing.JFrame;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JTextField;

import parts.Pattern;
import red.Dynamic;
import red.JListData;
import red.Util;

public class Level {
	public static final String HEADER = "LEVEL2.0";
	public static final String FOOTER = "ENDLEVEL";
	public static final String EXTENSION = ".head";
	
	private File levelFile;
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
	private ArrayList<Pattern> availablePatterns;
	private ArrayList<Pattern> patterns = new ArrayList<>();
	
	/**
	 * Default level parameters.
	 * @param projectPath The folder of the project
	 * @param name Project name (also file name)
	 * @param availablePatterns A list of all of the loaded patterns
	 */
	public Level(File projectPath, ArrayList<Pattern> availablePatterns, String name) {
		this.levelFile = new File(projectPath, name.replaceAll("\\s", "") + EXTENSION);
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
		ByteBuffer levelRawData = Util.readBinaryFile(levelFile);
		this.levelFile = levelFile;
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
			this.availablePatterns = availablePatterns;
			loadPatterns(levelRawData, availablePatterns);
			Util.checkString(levelRawData, FOOTER);
		}  catch(BufferUnderflowException e) {
			System.out.println("The file does not have all of the properties needed to create a level!");
			throw e;
		}
	}

	/**
	 * Writes this object to a file.
	 * @throws IOException
	 */
	public void writeFile() throws IOException {
		Dynamic d = new Dynamic();
		d.putRawString(HEADER);
		d.putString(name);
		d.putString(difficulty);
		d.putString(mode);
		d.putString(creator);
		d.putString(music);
		d.putColors(bg1);
		d.putColors(bg2);
		d.putColors(fg);
		d.putFloat(wallSpeed);
		d.putFloat(rotationSpeed);
		d.putFloat(humanSpeed);
		d.putFloat(pulseSpeed);
		d.putInt(patterns.size());
		for(Pattern p : patterns) d.putString(p.toString());
		d.putRawString(FOOTER);
		d.write(levelFile);
	}

	/**
	 * Launches the editor.
	 */
	public void edit(JFrame frame, Project project) {
		JFrame level = new JFrame("Level Editor for level '" + name + "'");
		level.setMinimumSize(new Dimension(685,620));
		level.setLayout(new GridLayout(1,0));
		level.addWindowListener(new WindowListener() {
			public void windowOpened(WindowEvent e) {}
			public void windowClosed(WindowEvent e) {}
			public void windowIconified(WindowEvent e) {}
			public void windowDeiconified(WindowEvent e) {}
			public void windowActivated(WindowEvent e) {}
			public void windowDeactivated(WindowEvent e) {};
			public void windowClosing(WindowEvent e) {
				frame.setVisible(true);
				project.loadLevels();
			}
		});
		
		//BEGIN Left Side Textual Configuration
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
					name = Util.upperText(jname);
					difficulty = Util.upperText(jdiff);
					mode = Util.upperText(jmode);
					creator = Util.upperText(jcrea);
					music = jmuse.getText();
					wallSpeed = Float.parseFloat(jwall.getText());
					rotationSpeed = Float.parseFloat(jrota.getText());
					humanSpeed = Float.parseFloat(jhuma.getText());
					pulseSpeed = Float.parseFloat(jpuls.getText());
					int result = JOptionPane.showOptionDialog(level, 
						"Do you really want to save this information?\nThe current file will be backed up.", 
						"Really save?", JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE, null, 
						null, JOptionPane.YES_OPTION);
					if(result != JOptionPane.YES_OPTION) return;
					writeFile();
					JOptionPane.showMessageDialog(level, 
						"Wrote the file to your hard drive successfully!", 
						"Success!", JOptionPane.INFORMATION_MESSAGE);
				} catch (Exception ex) {
					JOptionPane.showMessageDialog(level, 
						"Error updating the level!\nCheck console for details.", 
						"Error!", JOptionPane.ERROR_MESSAGE);
					ex.printStackTrace();
				}
			}
		});
		level.add(textConfiguration);
		//END
		
		//BEGIN Color Panel
		JPanel colors = new JPanel();
		colors.setLayout(new GridLayout(0,1));
		colors.setBackground(Util.BACKGROUND);
		Util.createColorPicker(colors, bg1, "Background Primary");
		Util.createColorPicker(colors, bg2, "Background Secondary");
		Util.createColorPicker(colors, fg, "Foreground");
		level.add(colors);
		//END
		
		//BEGIN Right Side pattern chooser
		JPanel patternConfiguration = new JPanel();
		patternConfiguration.setLayout(new GridLayout(0,1));
		
		//BEGIN Top Pattern Selector
		JPanel top = new JPanel();
		top.setLayout(new BorderLayout());
		top.setBackground(Util.BACKGROUND);
		
		//BEGIN Buttons at the top of the available patterns
		JPanel topButtons = new JPanel();
		topButtons.setLayout(new BorderLayout());
		topButtons.setBackground(Util.BACKGROUND);
		Util.addButtonToPanel(topButtons, BorderLayout.NORTH, "Create new pattern", new  ActionListener() {
			public void actionPerformed(ActionEvent e) {
				
			}
		});
		Util.addButtonToPanel(topButtons, BorderLayout.SOUTH, "Edit selected available pattern", new  ActionListener() {
			public void actionPerformed(ActionEvent e) {
				
			}
		});
		top.add(topButtons, BorderLayout.NORTH);
		//END
		
		JListData javai = Util.addTitledListToPanel(top, BorderLayout.CENTER, "Available Patterns", availablePatterns);
		patternConfiguration.add(top);
		//END
	
		//BEGIN Bottom Level Patterns
		JPanel bot = new JPanel();
		bot.setLayout(new BorderLayout());
		bot.setBackground(Util.BACKGROUND);
		JListData jleve = Util.addTitledListToPanel(bot, BorderLayout.CENTER, "Linked Patterns", patterns);
		Util.addButtonToPanel(bot, BorderLayout.NORTH, "Link available pattern to this level", new  ActionListener() {
			public void actionPerformed(ActionEvent e) {
				int selection = javai.list.getSelectedIndex();
				if(selection < 0) return;
				patterns.add(availablePatterns.get(selection));
				Util.updateList(jleve, patterns);
			}
		});
		Util.addButtonToPanel(bot, BorderLayout.SOUTH,"Unlink pattern from this level", new  ActionListener() {
			public void actionPerformed(ActionEvent e) {
				int selection = jleve.list.getSelectedIndex();
				if(selection < 0) return;
				patterns.remove(selection);
				Util.updateList(jleve, patterns);
			}
		});
		patternConfiguration.add(bot);
		//END
		
		level.add(patternConfiguration);
		//END

		//bring window to life!
		level.pack();
		level.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
		level.setLocationRelativeTo(null);
		level.setVisible(true);
	}
	
	public String toString() {
		return levelFile.getName();
	}

	/**
	 * Loads the patterns into this level (the ones needed)
	 * @param levelRawData The raw data  of the file.
	 * @param numberOfPatterns The number of patterns read.
	 * @param availablePatterns All available patterns.
	 * @throws IOException
	 */
	private void loadPatterns(ByteBuffer levelRawData, ArrayList<Pattern> availablePatterns) throws IOException {
		
		//get number of patterns
		int numberOfPatterns = levelRawData.getInt();
		
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
