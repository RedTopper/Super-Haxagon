package red;

import java.io.File;
import java.util.Scanner;

import config.Project;

public class LevelEditTwo {
	public static String MASTER_FOLDER_NAME = "hexlevels";
	
	private static Scanner reader = new Scanner(System.in);
	
	public static void main(String[] args) {
		boolean open = false;
		System.out.println("Now Starting Level Editor 2.0");
		
		//Open project folder
		File projectDir = Util.getFolder(new File(new File("."), MASTER_FOLDER_NAME));
		
		//List projects
		Util.getFolderContents(Util.FileType.DIRECTORY, projectDir, "PROJECTS", "NO PROJECTS FOUND");
		
		//Open project folder
		Project conf = null;
		while(!open) {
			String project = prompt("Type the name of the project you would like to open\nor create one by typing in a new name:");
			try {
				conf = new Project(new File(projectDir, project));
				open = conf.success();
			} catch (Exception e) {
				System.out.println("\nSomething went wrong when loading that directory!");
				System.out.println(e.getMessage());
			}
		}
		
		//Close resources
		//conf.write();
		reader.close();
	}
	
	public static String prompt(String prompt) {
		System.out.println(prompt);
		System.out.print("> ");
		return reader.nextLine();
	}
}