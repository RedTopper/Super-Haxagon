package data;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import java.util.ArrayList;

import javax.swing.JFrame;
import javax.swing.JOptionPane;
import javax.swing.JPanel;

import parts.Pattern;
import red.ListData;
import red.Util;

public class Project {
	public static String PATTERN_FOLDER_NAME = "patterns";
	public static final String HEADER = "HAXAGON1.0";
	public static final String EXTENSION = ".haxagon";
	
	private File projectDir;
	private ArrayList<Level> levels = null;
	private ArrayList<Pattern> patterns = null;
	private ListData list = null;
	
	public Project(File projectDir) {
		this.projectDir = projectDir;
		loadPatterns();
		loadLevels();
	}
	
	public void edit() {
		JFrame project = new JFrame("Open Level");
		project.setMinimumSize(new Dimension(245, 350));
		project.setLayout(new GridLayout(0,1));
		
		JPanel contents = new JPanel();
		contents.setLayout(new BorderLayout());
		contents.setBackground(Util.BACKGROUND);
		list = Util.addTitledListToPanel(contents, BorderLayout.CENTER, "Available Levels", levels);
		Util.addButtonToPanel(contents, BorderLayout.NORTH, "Create new level", new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				String name = JOptionPane.showInputDialog(project, 
					"What do you want to call this level?", 
					"Level Name", JOptionPane.QUESTION_MESSAGE);
				if(name == null || name.length() == 0) return;
				project.setVisible(false);
				new Level(projectDir, patterns, name.trim()).edit(project, Project.this);
			}
		});
		Util.addButtonToPanel(contents, BorderLayout.SOUTH, "Edit selected level", new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				int index = list.list.getSelectedIndex();
				if(index < 0) return;
				project.setVisible(false);
				levels.get(index).edit(project, Project.this);
			}
		});
		
		project.add(contents);
		project.pack();
		project.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
		project.setLocationRelativeTo(null);
		project.setVisible(true);
	}

	public void loadLevels() {
		System.out.println("Refreshing level list...");
		levels = new ArrayList<>();
		ArrayList<File> levelFiles = Util.getFolderContents(Util.FileType.FILE, projectDir, "FOUND LEVELS", "NO LEVELS FOUND");
		for(File levelFile : levelFiles) {
			try {
				levels.add(new Level(levelFile, patterns));
			} catch (Exception e) {
				System.out.println("Something went wrong when loading a level!");
				e.printStackTrace();
			}
		}
		if(list == null) return;
		list.model.clear();
		for(Level level : levels) list.model.addElement(level.toString());
	}

	private void loadPatterns() {
		System.out.println("Loading patterns...");
		patterns = new ArrayList<>();
		File patternFolder = Util.getFolder(new File(projectDir, PATTERN_FOLDER_NAME));
		ArrayList<File> patternFiles = Util.getFolderContents(Util.FileType.FILE, patternFolder, "FOUND PATTERNS", "NO PATTERNS FOUND");
		for(File patternFile : patternFiles) {
			try {
				patterns.add(new Pattern(patternFile));
			} catch (Exception e) {
				System.out.println("Something went wrong when loading a pattern!");
				e.printStackTrace();
			}
		}
	}
}
