#pragma once

/**
 * Creates a number between start and end based on a percent. (from 0.0 to 1.00)
 */
double linear(double start, double end, double percent);

/**
 * Panics the game. This version appends to a file and immediately quits the game.
 * This method should hang the program until the user quits via home or homebrew exit.
 */
void panic(const char* title, const char* message, const char* file, const char* function, int line, int error);

/**
 * Full linear interpolation of a color.
 */
Color interpolateColor(Color one, Color two, double percent);

/**
 * Calculates a point (either for a wall or a general point) based on the running level's rotation, some offset
 * so it renders correctly, some distance the point should be from the center, the  side the point
 * should be rendered on, based on a total number of sides the shape has.
 */
Point calcPointWall(Point focus, double rotation, double offset, double distance, int side, double sides);
Point calcPoint(Point focus, double rotation, double offset, double distance);

/**
 * Gets a button state from user input.
 */
ButtonState getButton(void);

/**
 * Gets the current level that the score represents (for example, point)
 * if the user is less than 10 seconds into the game)
 */
char* getScoreText(int score);

 /**
  * Kind of like getScoreText(int score), this method converts a score
  * into a string. The resulting string MUST be freed! The format of  this
  * method looks like such:
  *
  * TTT:%% where  TTT is the current time in seconds, and %% is  the percent
  * through the current second.
  */
char* getScoreTime(int score);
