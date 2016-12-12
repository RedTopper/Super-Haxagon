package red;

import java.awt.Color;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.BufferUnderflowException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.channels.FileChannel;
import java.util.ArrayList;

import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JList;
import javax.swing.JPanel;
import javax.swing.JTextField;

public final class Util {
	private Util() throws InstantiationException {
		throw new InstantiationException();
	}
	
	public enum FileType {
		FILE,
		FOLDER
	}
	
	public static final int COLOR_BYTE_LENGTH = 3; // Each color takes up 1 byte;
	public static final String OLD = File.separator + "backup";
	public static final Color BACKGROUND = new Color(245, 245, 245);
	
	/**
	 * Gets a folder based on a file object.
	 * Creates one if it does not exist. Will quit the program on error.
	 * @param folder The folder to create.
	 * @return
	 */
	public static File getFolder(File folder) {
		System.out.println();
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
			if((type == FileType.FOLDER ? path.isDirectory() : path.isFile())) {
				if(!foundPath) {
					foundPath = true;
					System.out.println(foundMessage + ":");
				}
				paths.add(path);
				System.out.println(pathName);
			}
		}
		if(!foundPath) System.out.println(notFoundMessage + "!");
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
		byte length = data.get();
		StringBuilder string = new StringBuilder();
		for(int i = 0; i < length; i++) {
			string.append((char)data.get());
		}
		return string.toString();
	}
	
	public static void putString(ByteBuffer buffer, String string) {
		if(string.length() >= 256) string = string.substring(0, 256);
		buffer.put((byte) string.length()); 		
		buffer.put(string.getBytes());
	}

	/**
	 * Gets a color from the file.
	 * @param data Raw data.
	 * @return A color representation (3 bytes required)
	 * @throws IOException
	 * @throws BufferUnderflowException
	 */
	public static ArrayList<Color> readColors(ByteBuffer data) throws IOException, BufferUnderflowException {
		int length = data.get() & 0xFF;
		ArrayList<Color> colors = new ArrayList<>();
		for(int i = 0; i < length; i++) {
			colors.add(new Color(data.get() & 0xFF, data.get() & 0xFF, data.get() & 0xFF));
		}
		return colors;
	}
	
	public static byte[] putColors(ArrayList<Color> c) {
		byte[] colors = new byte[c.size() * 3 + 1];
		colors[0] = (byte)(c.size());
		for(int i = 0; i < c.size(); i++) {
			colors[i * 3 + 1] = (byte) c.get(i).getRed();
			colors[i * 3 + 2] = (byte) c.get(i).getBlue();
			colors[i * 3 + 3] = (byte) c.get(i).getGreen();
		}
		return colors;
	}
	
	public static String getColorAsString(ArrayList<Color> colors) {
		StringBuilder s = new StringBuilder();
		s.append("[");
		String delemeter = "";
		for(Color c : colors) {
			s.append(delemeter);
			s.append(c.toString().substring(14));
			delemeter = ", ";
		}
		s.append("]");
		return s.toString();
	}

	public static ByteBuffer readBinaryFile(File file) throws IOException {
		FileInputStream inputStream = new FileInputStream(file);
		FileChannel channel = inputStream.getChannel();
		ByteBuffer rawData = ByteBuffer.allocate((int)channel.size());
		channel.read(rawData);
		rawData.rewind();
		rawData.order(ByteOrder.LITTLE_ENDIAN);
		inputStream.close();
		return rawData;
	}
	
	public static void writeBinaryFile(File file, ByteBuffer buffer) throws IOException {
		
		//Delete really old file and move old file into it's place
		File oldDir = Util.getFolder(new File(file.getParent() + OLD));
		File backupFile = new File(oldDir, file.getName());
		backupFile.delete();
		file.renameTo(backupFile);
		
		//write new file
		FileOutputStream outputStream = new FileOutputStream(file, false);
		FileChannel channel = outputStream.getChannel();
		buffer.rewind();
		channel.write(buffer);
		channel.close();
		outputStream.close();
		System.out.println("Backed up and wrote file: '" + file.getAbsolutePath() + "'");
	}
	
	public static void addTitledFieldToPanel(JPanel panel, Object constraints, String title, String defaultText) {
		JTextField text = new JTextField(defaultText);
		text.setBorder(BorderFactory.createTitledBorder(title));
		text.setBackground(BACKGROUND);
		panel.add(text, constraints);
	}
	
	public static void addTitledListToPanel(JPanel panel, Object constraints, String title, ArrayList<?> list) {
		JList<Object> internal = new JList<>(list.toArray());
		internal.setBorder(BorderFactory.createTitledBorder(title));
		internal.setBackground(BACKGROUND);
		panel.add(internal, constraints);
	}
	
	public static void addButtonToPanel(JPanel panel, Object constraints, String text) {
		JButton button = new JButton(text);
		button.setBackground(BACKGROUND);
		panel.add(button, constraints);
	}
}
