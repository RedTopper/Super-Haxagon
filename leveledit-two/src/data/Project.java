package data;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;

import parts.Pattern;
import red.Util;

public class Project {
	public static String FOLDER_NAME = "patterns";
	public static final String HEADER = "HAXAGON1.0";
	public static final String EXTENSION = ".haxagon";
	
	private ArrayList<Level> levels = new ArrayList<>();
	
	public Project(File path) throws IOException {
		
		//Open directories
		File projectFolder = Util.getFolder(path);
		ArrayList<File> levelFiles = Util.getFolderContents(Util.FileType.FILE, projectFolder, "FOUND LEVELS", "NO LEVELS FOUND");
		File patternFolder = Util.getFolder(new File(path, FOLDER_NAME));
		ArrayList<File> patternFiles = Util.getFolderContents(Util.FileType.FILE, patternFolder, "FOUND PATTERNS", "NO PATTERNS FOUND");
		ArrayList<Pattern> patterns = new ArrayList<>();
		for(File patternFile : patternFiles) {
			patterns.add(new Pattern(patternFile));
		}
		
		//Load level files
		for(File levelFile : levelFiles) {
			try {
				Level level = new Level(levelFile, patterns);
				this.levels.add(level);
				level.writeFile();
			} catch (Exception e) {
				System.out.println("Something went wrong when loading a level!");
				e.printStackTrace();
			}
		}
		new Level(projectFolder);
	}
}
