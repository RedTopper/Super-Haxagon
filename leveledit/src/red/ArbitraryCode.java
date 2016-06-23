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
		for(int i = 0; i < 16; i++) {
			p.getWalls().add(new Wall(i%6, 40 + (i > 4 ? i*14 : 0), 20  + (i <= 4 ? i*14 : 0)));
		}
		p.setLevelFlags((byte)0b111111); //Every level (0-5) is enabled (set to 1)
		patterns.add(p);
		
		//Creates a wall on one side with a single hole, then another on the other side
		p = new Pattern();
		for(int i = 0; i < 12; i++) {
			if(i != 0 && i != 9) {
				p.getWalls().add(new Wall(i%6, (i <= 5 ? 40 : 140), 16));
			}
		}
		p.setLevelFlags((byte)0b111111);
		patterns.add(p);
		
		//Creates a double tri thing.
		p = new Pattern();
		for(int i = 0; i < 3; i++) {
			p.getWalls().add(new Wall(i * 2, 40, 16));
			p.getWalls().add(new Wall(i * 2, 40 + 16 * 2, 16));
		}
		p.setLevelFlags((byte)0b111111);
		patterns.add(p);
		
		//Creates a TRIPPLE tri thing (more spread out).
		p = new Pattern();
		for(int i = 0; i < 3; i++) {
			for(int side = 0; side< 3; side++) {
				p.getWalls().add(new Wall(side * 2, 40 + 16 * i * 4, 16));
			}
		}
		p.setLevelFlags((byte)0b110110);
		patterns.add(p);
		
		//Creates a QUADRA tri thing (more spread out and they overlap!).
		p = new Pattern();
		for(int i = 0; i < 4; i++) {
			for(int side = 0; side< 3; side++) {
				p.getWalls().add(new Wall((side * 2 + (i % 2 == 0 ? 0 : 1)) % 6, 40 + 16 * i * 4, 16));
			}
		}
		p.setLevelFlags((byte)0b110110);
		patterns.add(p);
		
		//Creates a ladder
		p = new Pattern();
		p.getWalls().add(new Wall(0, 40, 16 * 5 * 3 - 32));
		p.getWalls().add(new Wall(3, 40, 16 * 5 * 3 - 32));
		for(int i = 0; i < 5; i++) {
			p.getWalls().add(new Wall(( i % 2 == 0 ? 1 : 2), 40 + i * 3 * 16, 16));
			p.getWalls().add(new Wall(( i % 2 == 0 ? 4 : 5), 40 + i * 3 * 16, 16));
		}
		p.setLevelFlags((byte)0b101001); //This means 0th level, 3rd level, and 5th level (Note it's backwards!)
		patterns.add(p);
		
		//Creates an inverted ladder. Spin 2 right and 2 left many times.
		p = new Pattern();
		p.getWalls().add(new Wall(0, 40, 16 * 5 * 3 - 32));
		for(int i = 0; i < 5; i++) {
			for(int side = 1; side < 6; side++) {
				if(side != (i % 2  == 0 ? 4 : 2) && side != (i % 2  == 0 ? 5 : 1))
				p.getWalls().add(new Wall(side, 40 + i * 3 * 16, 16));
			}
		}
		p.setLevelFlags((byte)0b011010);
		patterns.add(p);
		
		//Creates a one way double right rotational thing of many deaths.
		p = new Pattern();
		for(int i = 0; i < 6; i++) {
			for(int side = 0; side < 6; side++) {
				if(side != 0 && side != 3) {
					p.getWalls().add(new Wall((side + i) % 6, 40 + i * 3 * 16, 16));
				}
			}
		}
		p.setLevelFlags((byte)0b110011);
		patterns.add(p);
		
		return patterns;
	}
}
