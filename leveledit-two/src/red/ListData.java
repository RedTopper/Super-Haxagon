package red;

import javax.swing.DefaultListModel;
import javax.swing.JList;

public class ListData {
	public final DefaultListModel<String> model;
	public final JList<String> list;
	public  ListData(DefaultListModel<String> model, JList<String> list) {
		this.model = model;
		this.list = list;
	}
}
