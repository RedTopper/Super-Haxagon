package red;

import java.util.ArrayList;

public class ArbitraryCode {	
	/**
	 * You can write your own code here to generate patterns programmatically. 
	 * Just return an array of patterns and the runtime will allow you to add 
	 * it to the contents of the level patterns by typing r! */
	public static ArrayList<Pattern> runCode() {
		ArrayList<Pattern> patterns = new ArrayList<>();
		Pattern p = null;
		
		//Write code here. Examples:
		//Creates a spiral for the user to swing around.
		p = new Pattern();
		for(int i = 0; i < 12; i++) {
			p.addWall(new Wall(i%6,40 + (i > 4 ? i*14 : 0),20  + (i <= 4 ? i*14 : 0)));
		}
		patterns.add(p);
		
		//Creates a wall on one side with a single hole, then another on the other side
		p = new Pattern();
		for(int i = 0; i < 12; i++) {
			if(i != 0 && i != 9) {
				p.addWall(new Wall(i%6, (i > 5 ? 40 : 140), 16));
			}
		}
		patterns.add(p);
		
		return patterns;
	}
}
