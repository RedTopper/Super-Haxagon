#pragma once

/**
 * Draws the main menu of the program based on loaded data and main menu variables.
 */
void drawMainMenu(GlobalData data, MainMenu menu);
void drawMainMenuBot(LoadedState loaded, double fps, int showGetBGM, int showLoadLevels);

/**
 * Base render for the game. Draws based on a single active level.
 */
void drawPlayGame(Level level, LiveLevel liveLevel, double offset, double sides);
void drawPlayGameBot(Level level, LiveLevel liveLevel, double fps);

/**
 * Renders game over text.
 */
void drawGameOverBot(int score, int highScore, double fps, int frame, int showText);

/**
 * Draws the crash screen.
 * :(
 */
void drawPanic(const char* message, const char* file, const char* function, int line, int error);
void drawPanicBot(void);

/**
 * Draws a non-destructive warning screen for the user to read.
 * :/
 */
void drawWarning(const char* message, const char* file, const char* function, int line);

/**
 * Draws a completely black screen on the bottom of the 3DS.
 */
void drawBlackBot(void);
