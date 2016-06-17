package leveledit;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Scanner;

public class Main {
	private static void readContents(FileInputStream file) {
		
	}
	
	static void createFile(String path) {
		try {
			FileOutputStream out = new FileOutputStream(path);
			byte[] data = "LEVE".getBytes();
			out.write(data);
			out.close();
		} catch (IOException e1) {
			e1.printStackTrace();
		}
	}
	
	static FileInputStream createOrOpenFile(String path) {
		FileInputStream file = null;
	    try {
			file = new FileInputStream(path);
			byte[] array = new byte[4];
			file.read(array, 0, 4);
		    if(!('L' == array[0] && 'E' == array[1] && 'V' == array[2] && 'E' == array[3])) {
		    	System.out.println("That isn't the right file type!");
		    	return null;
		    }
		} catch (IOException e) {
			createFile(path);
			try {
				file = new FileInputStream(path);
			} catch (FileNotFoundException e1) {
				System.out.println("That file is protected. Check the error!");
			}
		}
	    return file;
	}
	
	public static void main(String[] args) {
		Scanner reader = new Scanner(System.in);
		FileInputStream file = null;
		System.out.println("Welcome to the level editor 2000");
		String openmessage = "Please type a file you would like to open or create:";
		System.out.println(openmessage);
		while ((file = createOrOpenFile(reader.next())) == null) {
			System.out.println(openmessage);
		}
		readContents(file);
		
		try {
			file.close();
			reader.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
}
