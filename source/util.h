#pragma once

/**
 * Creates a number between start and end based on a percent. (from 0.0 to 1.00)
 */
double linear(double start, double end, double percent);

/**
 * Checks a result and, if it's bad, will hang the program in a loop and
 * display an error to the user with a file offset.
 */
int check(int result, const char* message, const char* file, const char* function, int line, int error);

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
ButtonState getButton();

/**
 * Gets the current level that the score represents (for example, point)
 * if the user is less than 10 seconds into the game)
 */
 char* getScoreText(int score);