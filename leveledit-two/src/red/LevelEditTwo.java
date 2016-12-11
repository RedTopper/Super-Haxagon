package red;

import java.io.File;
import java.io.IOException;
import java.util.Scanner;

import data.Project;

public class LevelEditTwo {
	public static String FOLDER_NAME = "hexlevels";
	
	private static Scanner reader = new Scanner(System.in);
	
	public static void main(String[] args) {
		
		//Startup message
		System.out.println("Now Starting Level Editor 2.0");
		
		//Open project folder
		File projectDir = Util.getFolder(new File(new File("."), FOLDER_NAME));
		
		//List projects
		Util.getFolderContents(Util.FileType.FOLDER, projectDir, "PROJECTS", "NO PROJECTS FOUND");
		
		//Open project folder
		String project = prompt("Type the name of the project you would like to open\nor create one by typing in a new name:");
		try {
			Project conf = new Project(new File(projectDir, project));
			conf.toString();
		} catch (IOException e) {
			System.out.println("Something went wrong when opening the project!");
			e.printStackTrace();
		}
		
		//Close resources
		reader.close();
	}
	
	public static String prompt(String prompt) {
		System.out.println("\n" + prompt);
		System.out.print("> ");
		return reader.nextLine();
	}
}