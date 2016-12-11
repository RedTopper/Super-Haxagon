package config;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;

import red.Util;

public class Project {
	public static String PATTERN_FOLDER_NAME = "patterns";
	
	private  boolean loaded = false;
	ArrayList<Level> levels = new ArrayList<>();
	
	public Project(File path) throws IOException {
		File project = Util.getFolder(path);
		File patterns = Util.getFolder(new File(path, PATTERN_FOLDER_NAME));
		ArrayList<File> levels = Util.getFolderContents(Util.FileType.FILE, project, "FOUND LEVELS", "NO LEVELS FOUND");
		for(File levelFile : levels) {
			try {
				Level level = new Level(levelFile);
				this.levels.add(level);
			} catch (Exception e) {
				System.out.println(e.getMessage());
			}
		}
		loaded = true;
	}

	public boolean success() {
		return loaded;
	}
}
