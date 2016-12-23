#pragma once

/**
 * Checks if the cursor has collided with a wall.
 */
MovementState collisionLiveWall(LiveWall wall, double cursorPos, double cursorStep, int sides);

/**
 * Checks if the cursor has collided with the running level level.
 */
MovementState collisionLiveLevel(LiveLevel live, double cursorStep);

/**
 * Logic for working the main menu, along  with a selection sound.
 * Modifys the address of level to the currently selected level upon return.
 */
GameState doMainMenu(GlobalData data, Track select, int* level);

/**
 * Plays a level.
 */
GameState doPlayGame(GlobalData data, int level);