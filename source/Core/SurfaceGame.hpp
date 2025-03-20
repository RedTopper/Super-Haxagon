#ifndef SUPER_HAXAGON_SURFACE_GAME_HPP
#define SUPER_HAXAGON_SURFACE_GAME_HPP

// SPDX-FileCopyrightText: 2025 AJ Walter
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Core/Camera.hpp"
#include "Core/Structs.hpp"
#include "Core/Matrix.hpp"

#include <deque>
#include <vector>

namespace SuperHaxagon {
	class Screen;
	class Pattern;
	class Wall;
	class SurfaceGame {
	public:
		SurfaceGame(Screen& screen, Camera& camera);
		~SurfaceGame() = default;

		/**
		 * Projects the game from game coordinates to camera coordinates.
		 *
		 * This can (should?) be done as a vertex shader, but some platforms
		 * do not support said shaders, so do it in software.
		 */
		void project(const Color& color, std::vector<Vec2f>& points);

		/**
		 * Draws points to the screen on a range of [-1, 1] for x and y.
		 */
		void drawPolyGame(const Color& color, std::vector<Vec2f>& points);

		/**
		 * Draws the background of the screen (the radiating colors part)
		 */
		void drawBackground(const Color& color1, const Color& color2, float sides);

		/**
		 * Draws a regular polygon at some point focus. Useful for generating
		 * the regular polygon in the center of the screen.
		 */
		void drawRegular(const Color& color, float radius, float sides);

		/**
		 * Draws the little cursor in the center of the screen controlled by a human.
		 */
		void drawCursor(const Color& color, float radius, float cursor);

		/**
		 * Completely draws all patterns in a live level. Can also be used to create
		 * an "Explosion" effect if you use "offset". (for game overs)
		 */
		void drawPatterns(const Color& color, const std::deque<Pattern>& patterns, float sides);

		/**
		 * Draws a single moving wall based on a live wall, a color, some rotational value, and the total
		 * amount of sides that appears.
		 */
		void drawWalls(const Color& color, const Wall& wall, float sides);

		/**
		 * Draws 4 triangles in the corner of the expected render area. Useful for debugging.
		 */
		void drawDebugTriangles();

		/**
		 * Converts points to the absolute positions within the screen area.
		 * @param points
		 */
		void toScreenSpace(std::vector<Vec2f>& points);

		void calculateMatrix(float rotation);

		void copyMatrix(const SurfaceGame& other);

		void setWallOffset(float offset);

		void setDepth(float depth);

	private:
		Screen& _screen;
		Camera& _camera;
		float _offset = 0.0f;
		float _depth = 0.0;
		Matrix4x4f _matrix{};
	};
}

#endif //SUPER_HAXAGON_SURFACE_GAME_HPP
