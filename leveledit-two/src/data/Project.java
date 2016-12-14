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

@SuppressWarnings("serial")
public class Project extends JFrame{
	public static final String HEADER = "HAXAGON1.0";
	public static final String EXTENSION = ".haxagon";
	public final File dir;
	
	private ArrayList<Level> levels = null;
	private ArrayList<Pattern> patterns = null;
	private ListData list = null;
	
	public Project(File projectDir) {
		super("Open Level");
		this.dir = projectDir;
		loadPatterns();
		loadLevels();
		setMinimumSize(new Dimension(245, 350));
		setLayout(new GridLayout(0,1));
	}
	
	public void edit() {
		JPanel contents = new JPanel();
		contents.setLayout(new BorderLayout());
		contents.setBackground(Util.BACKGROUND);
		list = Util.addTitledListToPanel(contents, BorderLayout.CENTER, "Available Levels", levels);
		Util.addButtonToPanel(contents, BorderLayout.NORTH, "Create new level", new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				String name = JOptionPane.showInputDialog(Project.this, 
					"What do you want to call this level?", 
					"Level Name", JOptionPane.QUESTION_MESSAGE);
				if(name == null || name.length() == 0) return;
				Project.this.setVisible(false);
				new Level(Project.this, name.trim()).edit();
			}
		});
		Util.addButtonToPanel(contents, BorderLayout.SOUTH, "Edit selected level", new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				int index = list.list.getSelectedIndex();
				if(index < 0) return;
				Project.this.setVisible(false);
				levels.get(index).edit();
			}
		});
		add(contents);
		pack();
		setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
		setLocationRelativeTo(null);
		setVisible(true);
	}

	public void loadLevels() {
		System.out.println("Refreshing level list...");
		levels = new ArrayList<>();
		ArrayList<File> levelFiles = Util.getDirContents(Util.FileType.FILE, dir, "FOUND LEVELS", "NO LEVELS FOUND");
		for(File levelFile : levelFiles) {
			try {
				levels.add(new Level(this, levelFile));
			} catch (Exception e) {
				System.out.println("Something went wrong when loading a level!");
				e.printStackTrace();
			}
		}
		if(list == null) return;
		Util.updateList(list, levels);
	}

	public void loadPatterns() {
		System.out.println("Refreshing patterns...");
		patterns = new ArrayList<>();
		File patternFolder = Util.getDir(new File(dir, Pattern.DIR_NAME));
		ArrayList<File> patternFiles = Util.getDirContents(Util.FileType.FILE, patternFolder, "FOUND PATTERNS", "NO PATTERNS FOUND");
		for(File patternFile : patternFiles) {
			try {
				patterns.add(new Pattern(Project.this, patternFile));
			} catch (Exception e) {
				System.out.println("Something went wrong when loading a pattern!");
				e.printStackTrace();
			}
		}
	}
	
	public ArrayList<Pattern> getPatterns() {
		return patterns;
	}
}
