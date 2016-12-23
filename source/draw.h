#pragma once

/**
 * Draws a triangle
 */
void drawTriangle(Color color, Point points[3]);

/**
 * Draws a trapizoid
 */
void drawTrap(Color color, Point points[4]);

/**
 * Draws a rectangle using the super fast 2d library and
 * the color/point datatype.
 */
void drawRect(Color color, Point position, Point size);

/**
 * Draws a single moving wall based on a live wall, a color, some rotational value, and the total
 * amount of sides that appears.
 */
void drawMovingWall(Color color, Point focus, LiveWall wall, double rotation, int sides);

/**
 * Completely draws all walls in a live level. 
 */
void drawMovingPatterns(Color color, Point focus, LiveLevel live, double manualOffset);

/**
 * Draws the main hexagon in the game (Might not actually be a hexagon!)
 */
void drawMainHexagon(Color color1, Color color2, Point focus, double rotation, int sides);

/**
 * Draws the background of the game.
 */
void drawBackground(Color color1, Color color2, Point focus, double height, double rotation, int sides);

/**
 * Draws the little cursor in the center of the screen controlled by a human.
 */
void drawHumanCursor(Color color, Point focus, double cursor, double rotation);

/**
 * Draws the main menu of the program based on loaded data and main menu variables.
 */
void drawMainMenu(GlobalData data, MainMenu menu);
void drawMainMenuBot(double fps);

/**
 * Base render for the game. Draws based on a single active level. 
 */
void drawPlayGame(Level level, LiveLevel liveLevel, double offset);
void drawPlayGameBot(FileString name, int score, double fps);

/** 
 * Renders game over text.
 */
void drawGameOverBot(LiveLevel level, int score, double fps, int frame);