package parts;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;
import java.io.File;
import java.io.IOException;
import java.nio.BufferUnderflowException;
import java.nio.ByteBuffer;
import java.util.ArrayList;

import javax.swing.JFrame;
import javax.swing.JPanel;

import data.Level;
import gfx.JPattern;
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
	
	public void edit(JFrame frame, Level level) {
		JFrame pattern = new JFrame("Pattern Editor for pattern '" + patternFile.getName() + "'");
		pattern.setLayout(new BorderLayout());
		pattern.addWindowListener(new WindowListener() {
			public void windowOpened(WindowEvent e) {}
			public void windowClosed(WindowEvent e) {}
			public void windowIconified(WindowEvent e) {}
			public void windowDeiconified(WindowEvent e) {}
			public void windowActivated(WindowEvent e) {}
			public void windowDeactivated(WindowEvent e) {};
			public void windowClosing(WindowEvent e) {
				frame.setVisible(true);
			}
		});
		
		JPanel leftConfig = Util.startFrame(new GridLayout(0,1));
		leftConfig.setPreferredSize(new Dimension(120,0));
		
		JPanel wallList = Util.startFrame(new BorderLayout());
		Util.addTitledListToPanel(wallList, BorderLayout.CENTER, "Walls", walls);
		Util.addButtonToPanel(wallList, BorderLayout.SOUTH, "Edit Pattern", null);
		leftConfig.add(wallList);
		
		JPanel editBoxes = Util.startFrame(new GridLayout(0,1));
		Util.addTitledFieldToPanel(editBoxes, null, "[int] Distance", null);
		Util.addTitledFieldToPanel(editBoxes, null, "[int] Height", null);
		Util.addTitledFieldToPanel(editBoxes, null, "[int] Side", null);
		leftConfig.add(editBoxes);
		
		pattern.add(leftConfig, BorderLayout.WEST);
		pattern.add(new JPattern(level, this), BorderLayout.CENTER);
		
		pattern.pack();
		pattern.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
		pattern.setLocationRelativeTo(null);
		pattern.setMinimumSize(pattern.getPreferredSize());
		pattern.setVisible(true);
	}

	public String toString() {
		return patternFile.getName();
	}
	
	public Wall[] getWalls() { 
		return (Wall[]) walls.toArray();
	}

	private void loadWalls(ByteBuffer patternRawData) {
		int numberOfWalls = patternRawData.getInt();
		for(int i = 0; i < numberOfWalls; i++) {
			walls.add(new Wall(patternRawData.getChar(), patternRawData.getChar(), patternRawData.getChar()));
		}
	}
}
