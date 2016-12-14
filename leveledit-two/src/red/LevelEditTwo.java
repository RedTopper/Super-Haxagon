package red;

import java.io.File;

import javax.swing.UIManager;
import javax.swing.UnsupportedLookAndFeelException;
import javax.swing.plaf.nimbus.NimbusLookAndFeel;

import data.Project;

public class LevelEditTwo {
	public static String DIR_NAME = "hexlevels";
	
	public static void main(String[] args) {
		
		//Startup message
		System.out.println("Now Starting Level Editor 2.0");
		try { UIManager.setLookAndFeel(new NimbusLookAndFeel());} catch (UnsupportedLookAndFeelException e) {}
		
		//Open project folder
		File projectDir = Util.getDir(new File(new File("."), DIR_NAME));
		
		//Open the project
		new Project(projectDir).edit();
	}
}