package red;

import java.awt.Color;
import java.io.File;
import java.io.IOException;
import java.nio.BufferUnderflowException;
import java.nio.ByteBuffer;
import java.util.ArrayList;

public final class Util {
	private Util() throws InstantiationException {
		throw new InstantiationException();
	}
	
	public enum FileType {
		FILE,
		DIRECTORY
	}
	
	public static File getFolder(File folder) {
		if(!folder.exists()) {
			folder.mkdir();
			System.out.println("Created directory: '" + folder.getAbsolutePath() + "'");
		} else if(!folder.isDirectory()) {
			System.out.println("Folder '" + folder.getAbsolutePath() + "' is not a directory!");
			System.exit(0);
		} else if(!folder.canWrite()) {
			System.out.println("Cannot write to '" + folder.getAbsolutePath() + "'!");
			System.exit(0);
		} else {
			System.out.println("Opened directory: '" + folder.getAbsolutePath() + "'");
		}
		return folder;
	}
	
	/**
	 * Gets a list of files based on the FileType is a DIRECTORY or a FILE  
	 * @param type Search for either FILES or DIRECTORIES in the passed folder.
	 * @param dir The folder to look in.
	 * @param foundMessage The message to print if at least one FILE/DIRECTORY is found
	 * @param notFoundMessage The message to print if nothing is found.
	 * @return
	 */
	public static ArrayList<File> getFolderContents(FileType type, File dir, String foundMessage, String notFoundMessage) {
		boolean foundPath = false;
		ArrayList<File> paths = new ArrayList<>();
		for(String pathName : dir.list()) {
			File path = new File(dir, pathName);
			if((type == FileType.DIRECTORY ? path.isDirectory() : path.isFile())) {
				if(!foundPath) {
					foundPath = true;
					System.out.println("\n" + foundMessage + ":");
				}
				paths.add(path);
				System.out.println(pathName);
			}
		}
		if(!foundPath) System.out.println("\n" + notFoundMessage + "!");
		System.out.println("");
		return paths;
	}
	
	/**
	 * Checks to make sure that a string in the file appears
	 * (Useful for checking the header of the file)
	 * @param data Raw data to check
	 * @param s The string to compare (will read as many bytes as passed string)
	 * @throws IOException
	 * @throws BufferUnderflowException
	 */
	public static void checkString(ByteBuffer data, String s) throws IOException, BufferUnderflowException {
		for(char header : s.toCharArray()) {
			byte file = data.get();
			if(header != file) {
				throw new IOException("Invalid string!");
			}
		}
	}
	
	/**
	 * Reads a null terminated string from the file.
	 * @param data Raw data
	 * @return A string.
	 * @throws IOException
	 * @throws BufferUnderflowException
	 */
	public static String readString(ByteBuffer data) throws IOException, BufferUnderflowException {
		StringBuilder string = new StringBuilder();
		byte part;
		while((part = data.get()) != '\0') {
			string.append((char)part);
		}
		return string.toString();
	}
	
	/**
	 * Gets a color from the file.
	 * @param data Raw data.
	 * @return A color representation (3 bytes required)
	 * @throws IOException
	 * @throws BufferUnderflowException
	 */
	public static Color readColor(ByteBuffer data) throws IOException, BufferUnderflowException {
		return new Color(data.get() & 0xFF, data.get() & 0xFF, data.get() & 0xFF);
	}
}
