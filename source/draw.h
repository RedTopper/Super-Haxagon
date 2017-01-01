#pragma once

/**
 * Draws the main menu of the program based on loaded data and main menu variables.
 */
void drawMainMenu(GlobalData data, MainMenu menu);
void drawMainMenuBot(LoadedState loaded, double fps);

/**
 * Base render for the game. Draws based on a single active level. 
 */
void drawPlayGame(Level level, LiveLevel liveLevel, double offset, double sides);
void drawPlayGameBot(FileString name, int score, double fps);

/** 
 * Renders game over text.
 */
void drawGameOverBot(int score, double fps, int frame);

/** 
 * Draws the crash screen.
 * :(
 */
void drawPanic(const char* message, const char* file, const char* function, int line, int error);
void drawPanicBot(void);
