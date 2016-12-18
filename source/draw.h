#pragma once

#include "header.h"

typedef struct {
  int x, y;
} Point;

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
 * Draws a single moving wall based on live settings, the playing pattern, and a wall.
 */
RenderState drawMovingWall(LiveLevel live, LivePattern pattern, LiveWall wall);

/**
 * Completely draws all walls in a live level. Returns the render value
 * of the furthest wall of the closest pattern (as to trigger a shift).
 */
RenderState drawMovingPatterns(LiveLevel live, int manualOffset);

/**
 * Draws the main hexagon in the game (Might not actually be a hexagon!)
 */
void drawMainHexagon(Color color1, Color color2, Point focus, double rotation, int sides);

/**
 * Helper function to drawMainHexagon, but with the LiveLevel datatype.
 */
void drawMainHexagonLive(LiveLevel live);

/**
 * Draws the background of the game.
 */
void drawBackground(Color color1, Color color2, Point focus, double height, double rotation, int sides);

/**
 * Similar to drawMainHexagonLive, this helper function calls drawBackground based on a LiveLevel datatype.
 */
void drawBackgroundLive(LiveLevel live);

/**
 * Draws the little cursor in the center of the screen controlled by a human.
 */
void drawHumanCursor(Color color, Point focus, double cursor, double rotation)

/**
 * Draws the main menu of the program.
 */
void drawMainMenu(GlobalData data, MainMenu menu)