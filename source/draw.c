#include "draw.h"

void drawTriangle(Color color, Point points[3]) {
	long paint = RGBA8(color.r,color.b,color.g,0xFF);
	
	//draws a triangle on the correct axis
	sf2d_draw_triangle(
		points[0].x, SCREEN_HEIGHT - 1 - points[0].y,
		points[1].x, SCREEN_HEIGHT - 1 - points[1].y,
		points[2].x, SCREEN_HEIGHT - 1 - points[2].y,
		paint);
}

void drawTrap(Color color, Point points[4]) {
	Point triangle[3];
	triangle[0] = points[0];
	triangle[1] = points[1];
	triangle[2] = points[2];
	drawTriangle(color, triangle);
	triangle[0] = points[0];
	triangle[1] = points[2];
	triangle[2] = points[3];
	drawTriangle(color, triangle);
}

void drawRect(Color color, Point position, Point size) {
	long paint = RGBA8(color.r,color.b,color.g,0xFF);
	sf2d_draw_rectangle(position.x, positiont.y, size.x, size.y, paint);
}

RenderState drawMovingWall(LiveLevel live, LivePattern pattern, LiveWall wall) {
	double distance = wall.distance;
	double height = wall.height;
	
	if(distance + height < FULL_LEN) return TOO_CLOSE;
	if(distance > TOP_SCREEN_DIAG_CENTER) return TOO_FAR; //Might be expensive to calc?
	
	if(distance < FULL_LEN - 2 ) {//so the distance is never negative as it enters.
		height -= FULL_LEN - 2 - distance;
		distance = FULL_LEN - 2; //Should never be 0!!!
	}
	
	Point edges[4] = {0};
	edges[0] = calcPoint(live, OVERFLOW_OFFSET, distance, wall.side + 1, pattern.sides);
	edges[1] = calcPoint(live, OVERFLOW_OFFSET, distance + height, wall.side + 1, pattern.sides);
	edges[2] = calcPoint(live, -OVERFLOW_OFFSET, distance + height, wall.side, pattern.sides);
	edges[3] = calcPoint(live, -OVERFLOW_OFFSET, distance, wall.side, pattern.sides);
	drawTrap(interpolateColor(live.currentFG, live.nextFG, live.tweenPercent), edges);
	return RENDERED;
}

RenderState drawMovingPatterns(LiveLevel live, int manualOffset) {
	
	//furthest distance of closest pattern
	double nearFurthestDistance = 0;
	RenderState nearFurthestRender = RENDERED;
	
	//for all patterns
	for(int iPattern = 0; iPattern < TOTAL_PATTERNS_AT_ONE_TIME; iPattern++) {
		LivePattern pattern = live.patterns[pattern];
		
		//draw all walls
		for(int iWall = 0; iWall < pattern.numWalls; iWall++) {
			MovingWall wall = pattern.walls[iWall];
			RenderState render = drawMovingWall(live, pattern, wall);
			
			//update information about the closest pattern
			if(wall = 0 && wall.distance + wall.height > nearFurthestDistance) {
				nearFurthestDistance = wall.distance + wall.height;
				nearFurthestRender = render;
			}
		}
	}
	return nearFurthestRender;
}

void drawMainHexagon(Color color1, Color color2, Point focus, double rotation, int sides) {
	
	//This draws the center shape.
	Point triangle[3];
	triangle[0] = focus;
	
	for(int concentricHexes = 0; concentricHexes < 2; concentricHexes++) {
		double height;
		Color color;
		if(concentricHexes == 0) {
			//outer color painted first
			color = color1;
			height = FULL_LEN;
		}
		if(concentricHexes == 1) {
			//inner color painted over it all.
			color =  color2;
			height = FULL_LEN - BORDER_LEN;
		}
		
		//draw hexagon (might not actually be hexagon!) in accordance to the closest pattern
		for(int i = 0; i < sides + 1; i++) {
			triangle[(i % 2) + 1].x = (int)(height * cos(rotation + (double)i * TAU/(double)sides) + (double)(focus.x));
			triangle[(i % 2) + 1].y = (int)(height * sin(rotation + (double)i * TAU/(double)sides) + (double)(focus.y));
			if(i != 0) {
				drawTriangle(color, triangle);
			}
		}
	}
}

void drawMainHexagonLive(LiveLevel live) {
	drawMainHexagon(
		tweenColor(live.currentFG, live.nextFG, live.tweenPercent), 
		tweenColor(live.currentBG2, live.nextBG2, live.tweenPercent),
		CENTER, live.rotation, live.patterns[0].sides);
}

void drawBackground(Color color1, Color color2, Point focus, double height, double rotation, int sides) {
	
	//solid background.
	Point position = {0,0};
	Point side = {TOP_WIDTH, SCREEN_HEIGHT};
	drawRect(color1, position, size);
	
	//This draws the main background.
	Point* edges = malloc(sizeof(point) * sides);
	check(edges, "Error drawing background!", 0x0);
	
	for(int i = 0; i < sides; i++) {
		edges[i].x = (int)(height * cos(rotation + (double)i * TAU/6.0) + focus.x);
		edges[i].y = (int)(height * sin(rotation + (double)i * TAU/6.0) + focus.y);
	}
	
	Point triangle[3];
	triangle[0] = focus;
	for(int i = 0; i < sides - 1; i = i + 2) {
		triangle[1] = edges[i];
		triangle[2] = edges[i + 1];
		drawTriangle(color2, triangle);
	}
	
	//if the sides is odd we need to "make up a color" to put in the gap.
	if(sides % 2) {
		triangle[1] = edges[sides - 2];
		triangle[2] = edges[sides - 1];
		drawTriangle(tweenColor(color1, color2, 0.5f), triangle);
	}
	
	free(edges);
}

void drawBackgroundLive(LiveLevel live) {
	drawBackground(
		tweenColor(live.currentBG1, live.nextBG1, live.tweenPercent), 
		tweenColor(live.currentBG2, live.nextBG2, live.tweenPercent),
		CENTER, TOP_SCREEN_DIAG_CENTER, live.rotation, live.patterns[0].sides);
}

void drawHumanCursor(Color color, Point focus, double cursor, double rotation) {
	Point humanTriangle[3];
	for(int i = 0; i < 3; i++) {
		double height = 0.0;
		double position = 0.0;
		if(i == 0) {
			height = FULL_LEN + HUMAN_PADDING + HUMAN_HEIGHT;
			position = cursor + rotation;
		} else {
			height = FULL_LEN + HUMAN_PADDING;
			if(i == 1) {
				position = cursor + HUMAN_WIDTH/2 + rotation;
			} else {
				position = cursor - HUMAN_WIDTH/2 + rotation;
			}
		}
		humanTriangle[i].x = (int)(height * cos(position) + focus.x);
		humanTriangle[i].y = (int)(height * sin(position) + focus.y);
	}
	drawTriangle(humanTriangle);
}

void drawMainMenu(GlobalData data, MainMenu menu) {
	double percentRotated = (double)(menu.rotationFrame) / (double)FRAMES_PER_ONE_SIDE_ROTATION);
	double rotation = percentRotated * TAU/6.0;
	if(menu.rotationDirection == -1) { //if the user is going to the left, flip the radians so the animation plays backwards.
		rotation *= -1.0;
	}
	if(menu.lastLevel % 2 == 1) { //We need to offset the hexagon if the selection is odd. Otherwise the bg colors flip!
		rotation += TAU/6.0;
	}
	
	//Colors
	Color FG;
	Color BG1;
	Color BG2;
	Level lastLevel = data.levels[menu.lastLevel];
	Level level = data.levels[menu.level];
	if(menu.transitioning) {
		FG = interpolateColor(lastLevel.colorsFG[0], level.colorsFG[0], percentRotated);
		BG1 = interpolateColor(lastLevel.colorsBG1[0], level.colorsBG1[0], percentRotated);
		BG2 = interpolateColor(lastLevel.colorsBG2[0], level.colorsBG2[0], percentRotated);
	} else {
		FG = level.colorsFG[0];
		BG1 = level.colorsBG1[0];
		BG2 = level.colorsBG2[0];
	} 
	
	Point focus = {TOP_WIDTH/2, SCREEN_HEIGHT/2 - 40};
	
	//Use 1.4 so the background height reaches the edge of the screen but does not go too far.
	//home screen always has 6 sides.
	drawBackground(BG1, BG2, focus, TOP_WIDTH / 1.4, rotation, 6); 
	drawMainHexagon(FG, BG2, focus, rotation, 6);
	drawHumanCursor(FG, focus, TAU/4.0, 0); //Draw cursor fixed quarter circle, no movement.

	Color white = {0xFF, 0xFF, 0xFF};
	Point title = {4, 4};
	Point difficulty = {4, 40};
	Point mode = {4, 56};
	Point creator = {4, 72};
	Point time = {4, SCREEN_HEIGHT - 18};
	
	Color black = {0,0,0};
	Point infoPos = {0, 0};
	Point infoSize = {TOP_WIDTH, creator.y + 16 + 2};
	drawRect(black, infoPos, infoSize);
	
	Point timePos = {0, time.y - 4};
	Point timeSize = {11/*chars*/ * 16, 16 + 8};
	drawRect(black, timePos, timeSize);
	
	writeFont(white, title, level.name.str, true);
	writeFont(white, difficulty, level.difficulty.str, false);
	writeFont(white, mode, level.mode.str, false);
	writeFont(white, creator, level.creator.str, false);
	writeFont(white, time, "SCORE: ??????", false);
}

GameState drawPlayGame() {
	double radians = g_levelData[g_level].radians;
		
	Point fg = g_levelColor[g_level][FG];
	Point bg1 = g_levelColor[g_level][BG1];
	Point bg2 = g_levelColor[g_level][BG2];
	
	sf2d_set_clear_color(pulseColor(bg1, radians).color);
	
	////RENDER TOP SCREEN
	sf2d_start_frame(GFX_TOP, GFX_LEFT);
	
	Point center;
	center.x = TOP_WIDTH/2;
	center.y = SCREEN_HEIGHT/2;
	
	drawBackground(
		pulseColor(center, radians),
		pulseColor(bg2, radians),
		TOP_WIDTH / 1.5,
		radians);
	MovementState collision = drawWalls(
		pulseColor(center, radians),
		pulseColor(fg, radians),
		radians, 0);
	drawMainHexagon(
		pulseColor(center, radians),
		pulseColor(fg, radians),
		pulseColor(bg1, radians),
		radians);
	drawHumanCursor(
		pulseColor(center, radians),
		pulseColor(fg, radians),
		g_levelData[g_level].cursor,
		radians);
	
	if(collision == DEAD) return GAME_OVER;
	
	////ROTATE
	radians = (radians + g_levelData[g_level].rotStep);
	if(radians >= TAU) radians -= TAU;
	if(radians < 0) radians += TAU;
	g_levelData[g_level].radians = radians;
	sf2d_end_frame();
	g_score++;
	
	//KEYS
	u32 kDown = hidKeysDown();
	u32 kHold = hidKeysHeld();
	if(kDown & KEY_B) {
		return GAME_OVER; //Theoretically can be switched to MAIN_MENU.
	}
	if((kHold & (KEY_R | KEY_ZR | KEY_CSTICK_RIGHT | KEY_CPAD_RIGHT | KEY_DRIGHT | KEY_X)) && collision != CANNOT_MOVE_RIGHT)
		g_levelData[g_level].cursor = (g_levelData[g_level].cursor - g_levelData[g_level].rotStepHuman);
	if((kHold & (KEY_L | KEY_ZL | KEY_CSTICK_LEFT  | KEY_CPAD_LEFT  | KEY_DLEFT  | KEY_Y)) && collision != CANNOT_MOVE_LEFT)
		g_levelData[g_level].cursor = (g_levelData[g_level].cursor + g_levelData[g_level].rotStepHuman);
	
	if(g_levelData[g_level].cursor >= TAU) g_levelData[g_level].cursor-=TAU;
	if(g_levelData[g_level].cursor < 0) g_levelData[g_level].cursor+=TAU;
	return PLAYING;
}

GameState drawGameOver() {
	double radians = g_levelData[g_level].radians;
	
	if(g_transition && g_transitionFrame < FRAMES_PER_GAME_OVER) {
		g_gameOverDistance = -pow(GAME_OVER_ACCEL_RATE, g_transitionFrame);
		g_transitionFrame++;
	} else {
		g_transitionFrame = 0;
		g_transition = false;
	}
		
	Point fg = g_levelColor[g_level][FG];
	Point bg1 = g_levelColor[g_level][BG1];
	Point bg2 = g_levelColor[g_level][BG2];
		
	sf2d_set_clear_color(bg1.color);
	
	////RENDER TOP SCREEN
	sf2d_start_frame(GFX_TOP, GFX_LEFT);
	
	Point center;
	center.x = TOP_WIDTH/2;
	center.y = SCREEN_HEIGHT/2;
	
	drawBackground(center, bg2, TOP_WIDTH / 1.5, radians);
	drawWalls(center, fg, radians, g_gameOverDistance);
	drawMainHexagon(center, fg, bg1, radians);
	drawHumanCursor(center, fg, g_levelData[g_level].cursor, radians); //Draw cursor fixed quarter circle, no movement.
	
	////ROTATE
	radians = (radians + GAME_OVER_ROT_SPEED);
	if(radians >= TAU) radians -= TAU;
	if(radians < 0) radians += TAU;
	g_levelData[g_level].radians = radians;
	sf2d_end_frame();
	
	//KEYS
	u32 kDown = hidKeysDown();
	if(!g_transition) {
		if(kDown & KEY_B) return MAIN_MENU;
		if(kDown & KEY_A) return PLAYING;
	} 
	return GAME_OVER;
}

void drawLagometer(int level) {
	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
	sf2d_draw_line(0, SCREEN_HEIGHT - 1, (int)((double)BOT_WIDTH * ((double)g_fps/60.0)), SCREEN_HEIGHT - 1, 1, RGBA8(0xFF,0,0,0xFF));
	if(level >= 0) {
		sf2d_draw_rectangle(0,0,BOT_WIDTH, 32 + 4, RGBA8(0, 0, 0, 0xFF));
		Point p;
		p.x = 14;
		p.y = 4;
		p.color = RGBA8(0xFF,0xFF,0xFF,0xFF);
		writeFont(p,"LOADING BGM", true);
	}
	if(g_gameState == PLAYING) {
		sf2d_draw_rectangle(0,23,BOT_WIDTH, 4, RGBA8(0, 0xFF, 0, 0xFF));
		sf2d_draw_rectangle(0,0,BOT_WIDTH, 22, RGBA8(0, 0, 0, 0xFF));
		Point p;
		p.color = RGBA8(0xFF,0xFF,0xFF,0xFF);
		p.x = 4;
		p.y = 4;
		writeFont(p,"POINT", false);
		p.x = 230;
		p.y = 4;
		char buffer[6+1];
		int scoreInt = (int)((double)g_score/60.0);
		int decimalPart = (int)(((double)g_score/60.0 - (double)scoreInt) * 100.0);
		snprintf(buffer, 6+1, "%03d:%02d", scoreInt, decimalPart); //Emergency stack overflow prevention
		writeFont(p,buffer, false);
	}
	if(g_gameState == GAME_OVER) {
		sf2d_draw_rectangle(0,0,BOT_WIDTH, 112, RGBA8(0, 0, 0, 0xFF));
		Point p;
		p.color = RGBA8(0xFF,0xFF,0xFF,0xFF);
		p.x = 44;
		p.y = 4;
		writeFont(p,"GAME OVER", true);
		p.x = 90;
		p.y = 40;
		char buffer[12+1];
		int scoreInt = (int)((double)g_score/60.0);
		int decimalPart = (int)(((double)g_score/60.0 - (double)scoreInt) * 100.0);
		snprintf(buffer, 12+1, "TIME: %03d:%02d", scoreInt, decimalPart); //Emergency stack overflow prevention
		writeFont(p,buffer, false);
		if(!g_transition) {
			p.x = 70;
			p.y = 70;
			writeFont(p,"PRESS A TO PLAY", false);
			p.x = 70;
			p.y = 86;
			writeFont(p,"PRESS B TO QUIT", false);
		}
	}
	sf2d_end_frame();
}