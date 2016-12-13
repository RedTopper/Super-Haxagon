package gfx;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Polygon;

import javax.swing.JPanel;

import data.Level;
import parts.Pattern;

@SuppressWarnings("serial")
public class JPattern extends JPanel {
	private Pattern pattern;
	private Level level;
	
	public JPattern(Level level, Pattern pattern) {
		super();
        setPreferredSize(new Dimension(640,480));
        this.pattern = pattern;
    }

    public void paintComponent(Graphics g) {
        super.paintComponent(g);
        Polygon p = new Polygon();
        p.addPoint(20, 20);
        p.addPoint(30, 50);
        p.addPoint(40, 50);
        p.addPoint(50, 20);
        g.setColor(Color.RED);
        g.fillRect(0, 0, getPreferredSize().width, getPreferredSize().height);
        g.setColor(Color.BLUE);
        g.fillPolygon(p);
    }
}
