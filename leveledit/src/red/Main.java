package red;

import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.InputMismatchException;
import java.util.Scanner;

public class Main {
	public static void main(String[] args) {
		Scanner reader = new Scanner(System.in);
		String path = "";
		FileInputStream file = null;
		System.out.println("Welcome to the level editor 2000");
		do {
			System.out.println("Please type a file you would like to open or create:");
			System.out.print("> ");
		} while ((file = openFile((path = reader.next()))) == null);
		
		ArrayList<Pattern> contents = readContents(file);
		
		mainmenu(contents, reader, path);
		
		try {
			file.close();
			reader.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	private static ArrayList<Pattern> readContents(FileInputStream file) {
		ArrayList<Pattern> patterns = new ArrayList<>();
		try {
			
			byte[] len = new byte[4];
			if(file.read(len, 0, 4) == -1) return patterns;
			for(byte b : len) {
				System.out.print((int)b + ",");
			}
			ByteBuffer bb = ByteBuffer.wrap(len);
			bb.order(ByteOrder.LITTLE_ENDIAN);
			int length = bb.getInt();
			for(int i = 0; i < length; i++) {
				patterns.add(new Pattern(file));
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
		return patterns;
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
	
	static FileInputStream openFile(String path) {
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
				byte[] array = new byte[4];
				file.read(array, 0, 4);
			    if(!('L' == array[0] && 'E' == array[1] && 'V' == array[2] && 'E' == array[3])) {
			    	System.out.println("That isn't the right file type!");
			    	return null;
			    }
			} catch (IOException e1) {
				System.out.println("That file is protected!");
			}
		}
	    return file;
	}

	private static void mainmenu(ArrayList<Pattern> contents, Scanner reader, String path) {
		System.out.println("c: create pattern");
		System.out.println("d: destroy pattern");
		System.out.println("e: edit pattern");
		System.out.println("r: run code");
		System.out.println("v: view pattern");
		System.out.println("q: Save and quit");
		boolean running = true;
		while(running) {
			System.out.println("----------------MAIN MENU----------------");
			System.out.println("There are " + contents.size() + " patterns in this file.");
			System.out.print("> ");
			String in = reader.next().trim().toLowerCase();
			switch(in) {
			case "c":
				System.out.println("Type anything else but a number to exit.");
				Pattern p = new Pattern();
				boolean run = true;
				while(run) {
					try {
						System.out.println(p);
						System.out.print("Side    >");
						short side = reader.nextShort();
						System.out.print("Distance>");
						short distanceFromCenter = reader.nextShort();
						System.out.print("Length  >");
						short length = reader.nextShort();
						p.addWall(new Wall(side, distanceFromCenter, length));
					} catch (InputMismatchException e) {
						reader.next(); //consume
						if(p.size() > 0) {
							System.out.println("Pattern saved.");
							contents.add(p);
						} else {
							System.out.println("Pattern discarded because there were no full entries.");
						}
						run = false;
					}
				}
				break;
			case "d":
			case "v":
				System.out.println("Witch one?:");
				System.out.print(">");
				try {
					int index = reader.nextInt();
					if(in.equals("d")) {
						System.out.println(contents.remove(index));
					} else {
						System.out.println(contents.get(index));
					}
				} catch (InputMismatchException e) {
					reader.next(); //consume
					System.out.println("Psych, that's the wrong number!");
				} catch (IndexOutOfBoundsException e) {
					System.out.println("Psych, that's the wrong number! (Patterns are 0 indexed)");
				}
				break;
			case "r":
				ArrayList<Pattern> patternsToAdd =ArbitraryCode.runCode();
				for(Pattern pattern : patternsToAdd) {
					contents.add(pattern);
					System.out.println(pattern);
				}
				System.out.println("All " + patternsToAdd.size() +  " patterns coded were added to the file.");
				break;
			case "q":
				running = false;
				break;
			}
			System.out.println("Autosaving work...");
			try {
				FileOutputStream out = new FileOutputStream(path);
				byte[] data = "LEVE".getBytes();
				out.write(data);
				
				ByteBuffer bb = ByteBuffer.allocate(4);
				bb.order(ByteOrder.LITTLE_ENDIAN);
				bb.putInt(contents.size());
				out.write(bb.array());
				
				for(Pattern p : contents) {
					p.writeObject(out);
				}
				out.close();
			} catch (IOException e1) {
				e1.printStackTrace();
			}
		}
	}
}
