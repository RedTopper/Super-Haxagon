package gfx;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Polygon;

import javax.swing.JPanel;

import data.Level;
import parts.Pattern;
import parts.Wall;

@SuppressWarnings("serial")
public class JPattern extends JPanel {
	
	//in case I need level colors?
	//private Level level;
	private Pattern pattern;
	private int selectedIndex = -1;
	
	public JPattern(Level level, Pattern pattern) {
		super();
        setPreferredSize(new Dimension(640,640));
        this.pattern = pattern;
    }
	
	public void paintWall(Graphics g, Wall wall) {
		Polygon p = new Polygon();
		p.addPoint(1,1);
		if(selectedIndex > 0) return;
	}

    public void paintComponent(Graphics g) {
        super.paintComponent(g);
        g.setColor(Color.WHITE);
        g.fillRect(0, 0, getPreferredSize().width, getPreferredSize().height);
        for(Wall wall : pattern.getWalls()) paintWall(g, wall);
    }

	public void selectIndex(int index) {
		selectedIndex = index;
	}
}
