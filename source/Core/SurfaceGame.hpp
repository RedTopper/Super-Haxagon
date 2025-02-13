#ifndef SUPER_HAXAGON_SURFACE_GAME_HPP
#define SUPER_HAXAGON_SURFACE_GAME_HPP

#include "Structs.hpp"

#include <deque>
#include <vector>

namespace SuperHaxagon {
	class Screen;
	class Pattern;
	class Wall;
	class SurfaceGame {
	public:
		SurfaceGame(Screen& screen);
		virtual ~SurfaceGame() = default;

		/**
		 * Applies a software based vertex shader to the passed coordinates.
		 * Useful for platforms that don't have programmable shaders.
		 * Subclasses may choose to override this and simply pass the points to drawPolyGame.
		 */
		virtual void vertexShader(const Color& color, std::vector<Vec2f>& points);

		/**
		 * Draws points to the screen on a range of [-1, 1] for x and y.
		 */
		virtual void drawPolyGame(const Color& color, std::vector<Vec2f>& points);

		/**
		 * Draws a rectangle on the screen. Position is bottom left.
		 */
		void drawRect(Color color, Vec2f position, Vec2f size);

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
		 * Converts points to the absolute positions within the screen area.
		 * @param points
		 */
		void toScreenSpace(std::vector<Vec2f>& points);

		void copySettings(const SurfaceGame& surface);
		void setRotation(float rotation);
		void setZoom(float zoom);
		void setRoll(float roll);
		void setPitch(float pitch);
		void setWallOffset(float offset);
		void setDepth(float depth);
		void setTranslate(const Vec2f& translate);
		void reset();

	protected:
		Screen& _screen;

	private:
		Matrix4x4f getMatrix();

		bool _recalculate = true;
		float _rotation = 0.0f;
		float _zoom = 1.0f;
		float _roll = 0.0f;
		float _pitch = 0.0f;
		float _offset = 0.0f;
		float _depth = 0.0;
		Vec2f _translate{};
		Matrix4x4f _project{};
		Matrix4x4f _camera{};
		Matrix4x4f _matrix{};
	};
}

#endif //SUPER_HAXAGON_SURFACE_GAME_HPP
