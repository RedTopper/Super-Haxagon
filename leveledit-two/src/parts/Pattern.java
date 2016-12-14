package parts;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.FocusEvent;
import java.awt.event.FocusListener;
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
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;

import data.Level;
import data.Project;
import gfx.JPattern;
import red.Dynamic;
import red.ListData;
import red.Util;

@SuppressWarnings("serial")
public class Pattern extends JFrame{
	
	//Globals
	public static final String DIR_NAME = "patterns";
	public static final String HEADER = "PATTERN1.0";
	public static final String FOOTER = "ENDPATTERN";
	public static final String EXTENSION = ".ptn";
	
	//Locals
	public final Project project;
	public final File patternFile;

	//Variables to write to file
	private int sidesRequired;
	private ArrayList<Wall> walls = new ArrayList<>();
	
	public Pattern(Project project, String name) {
		super("New pattern '" + name + "'");
		this.project = project;
		this.patternFile = new File(project.dir, DIR_NAME + File.separator + name + EXTENSION);
		this.sidesRequired = 6;
	}
	
	public Pattern(Project project, File patternFile) throws IOException {
		super("Pattern '" + patternFile.getName() + "'");
		this.project = project;
		this.patternFile = patternFile;
		ByteBuffer patternRawData = Util.readBinaryFile(patternFile);
		try {
			Util.checkString(patternRawData, HEADER);
			this.sidesRequired = patternRawData.get() & 0xFF;
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
		d.putByte(sidesRequired);
		d.putWalls(walls);
		d.putRawString(FOOTER);
		d.write(patternFile);
	}
	
	public void edit(Level level) {
		setLayout(new BorderLayout());
		addWindowListener(new WindowListener() {
			public void windowOpened(WindowEvent e) {}
			public void windowClosed(WindowEvent e) {}
			public void windowIconified(WindowEvent e) {}
			public void windowDeiconified(WindowEvent e) {}
			public void windowActivated(WindowEvent e) {}
			public void windowDeactivated(WindowEvent e) {};
			public void windowClosing(WindowEvent e) {
				level.refreshPatterns();
				level.setVisible(true);
			}
		});
		JPattern patternCanvis = new JPattern(level, this);
		
		JPanel leftConfig = Util.startFrame(new GridLayout(0,1));
		
		JPanel wallList = Util.startFrame(new BorderLayout());
		ListData selected = Util.addTitledListToPanel(wallList, BorderLayout.CENTER, "Walls", walls);
		
		JPanel editBoxes = Util.startFrame(new GridLayout(0,1));
		JTextField jdist = Util.addTitledFieldToPanel(editBoxes, null, "[int] Wall Distance", null);
		addFocus(selected, patternCanvis, jdist, Wall.Set.DISTANCE);
		JTextField jheig = Util.addTitledFieldToPanel(editBoxes, null, "[int] Wall Height", null);
		addFocus(selected, patternCanvis, jheig, Wall.Set.HEIGHT);
		JTextField jside = Util.addTitledFieldToPanel(editBoxes, null, "[int] Wall Side", null);
		addFocus(selected, patternCanvis, jside, Wall.Set.SIDE);
		JTextField jlvsd = Util.addTitledFieldToPanel(editBoxes, null, "[int] Pattern Shape", sidesRequired + "");
		Util.addButtonToPanel(editBoxes, null, "Save Pattern", new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				try {
					sidesRequired 	= Integer.parseInt(jlvsd.getText());
					int result = JOptionPane.showOptionDialog(Pattern.this, 
						"Do you really want to save this information?\nThe current file will be backed up.", 
						"Really save?", JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE, null, 
						null, JOptionPane.YES_OPTION);
					if(result != JOptionPane.YES_OPTION) return;
					writeFile();
					JOptionPane.showMessageDialog(Pattern.this, 
						"Wrote the file to your hard drive successfully!", 
						"Success!", JOptionPane.INFORMATION_MESSAGE);
				} catch (Exception ex) {
					JOptionPane.showMessageDialog(Pattern.this, 
						"Error updating the level!\nCheck console for details.", 
						"Error!", JOptionPane.ERROR_MESSAGE);
					ex.printStackTrace();
				}
			}
		});
		
		Util.addButtonToPanel(wallList, BorderLayout.NORTH, "Remove Wall", new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				int index = selected.list.getSelectedIndex();
				if(index < 0) return;
				walls.remove(index);
				Util.updateList(selected, walls);
				patternCanvis.repaint();
			}
		});
		selected.list.addListSelectionListener(new ListSelectionListener() {
			public void valueChanged(ListSelectionEvent e) {
				int index = selected.list.getSelectedIndex();
				if(index < 0) return; //Should not ever happen.
				Wall wall = walls.get(index);
				jdist.setText((int)(wall.getDistance()) + "");
				jheig.setText((int)(wall.getHeight()) + "");
				jside.setText((int)(wall.getSide()) + "");
				patternCanvis.selectIndex(index);
				patternCanvis.repaint();
			}
		});
		Util.addButtonToPanel(wallList, BorderLayout.SOUTH, "Add Wall", new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				walls.add(new Wall());
				Util.updateList(selected, walls);
				patternCanvis.repaint();
			}
		});
		
		leftConfig.setPreferredSize(new Dimension(200,0));
		leftConfig.add(wallList);
		leftConfig.add(editBoxes);
		add(leftConfig, BorderLayout.WEST);
		add(patternCanvis, BorderLayout.CENTER);
		pack();
		setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
		setLocationRelativeTo(null);
		setMinimumSize(getPreferredSize());
		setVisible(true);
	}

	public String toString() {
		return patternFile.getName();
	}
	
	public Wall[] getWalls() { 
		return walls.toArray(new Wall[0]);
	}

	private void loadWalls(ByteBuffer patternRawData) {
		int numberOfWalls = patternRawData.get() & 0xFF;
		for(int i = 0; i < numberOfWalls; i++) {
			walls.add(new Wall(patternRawData.getChar(), patternRawData.getChar(), patternRawData.getChar()));
		}
	}
	
	//TODO: Fix the race case when selecting the select thing.
	private void addFocus(ListData selected, JPattern patternCanvis, JTextField field, Wall.Set side) {
		field.addFocusListener(new FocusListener() {
			public void focusGained(FocusEvent e) {}
			public void focusLost(FocusEvent e) {
				int index = selected.list.getSelectedIndex();
				if(index < 0) return;
				int val = 1;
				try {val = Integer.parseInt(field.getText());} catch (Exception ex) {return;};	
				switch(side) {
				case DISTANCE:
					walls.get(index).setDistance(val); break;
				case HEIGHT:
					walls.get(index).setHeight(val); break;
				case SIDE:
					walls.get(index).setSide(val); break;
				}
				Util.updateList(selected, walls);
				patternCanvis.repaint();
			}
		});
	}
}
