package red;

import javax.swing.DefaultListModel;
import javax.swing.JList;

public class JListData {
	public final DefaultListModel<String> model;
	public final JList<String> list;
	public  JListData(DefaultListModel<String> model, JList<String> list) {
		this.model = model;
		this.list = list;
	}
}
