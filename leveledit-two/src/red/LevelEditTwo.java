package red;

import java.io.File;
import java.util.Scanner;

import javax.swing.UIManager;
import javax.swing.UnsupportedLookAndFeelException;
import javax.swing.plaf.nimbus.NimbusLookAndFeel;

import data.Project;

public class LevelEditTwo {
	public static String FOLDER_NAME = "hexlevels";
	
	private static Scanner reader = new Scanner(System.in);
	
	public static void main(String[] args) {
		
		//Startup message
		System.out.println("Now Starting Level Editor 2.0");
		try { UIManager.setLookAndFeel(new NimbusLookAndFeel());} catch (UnsupportedLookAndFeelException e) {}
		
		//Open project folder
		File projectDir = Util.getFolder(new File(new File("."), FOLDER_NAME));
		
		//Open the project
		Project conf = new Project(projectDir);
		conf.edit();
		
		//Close resources
		reader.close();
	}
	
	public static String prompt(String prompt) {
		System.out.println("\n" + prompt + ":");
		System.out.print("> ");
		return reader.nextLine();
	}
}