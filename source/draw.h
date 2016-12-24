#pragma once

/**
 * Draws the main menu of the program based on loaded data and main menu variables.
 */
void drawMainMenu(GlobalData data, MainMenu menu);
void drawMainMenuBot(double fps);

/**
 * Base render for the game. Draws based on a single active level. 
 */
void drawPlayGame(Level level, LiveLevel liveLevel, double offset, double sides);
void drawPlayGameBot(FileString name, int score, double fps);

/** 
 * Renders game over text.
 */
void drawGameOverBot(int score, double fps, int frame);