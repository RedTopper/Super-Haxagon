#ifndef SUPER_HAXAGON_WALL_HPP
#define SUPER_HAXAGON_WALL_HPP

#include "Core/Structs.hpp"

#include <vector>

namespace SuperHaxagon {
	class Wall {
	public:

		//Overflow so you don't get glitched lines between hexagons.
		//This is really just some arbitrary number so yeah...
		static constexpr float WALL_OVERFLOW = TAU/1200.0f;

		Wall(float distance, float height, int side);

		void advance(float speed);
		Movement collision(float cursorHeight, float cursorPos, float cursorStep, int sides) const;
		std::vector<Vec2f> calcPoints(const Vec2f& focus, float rotation, float sides, float offset, float scale) const;
		static Vec2f calcPoint(const Vec2f& focus, float rotation, float overflow, float distance, float sides, int side);

		float getDistance() const {return _distance;}
		float getHeight() const {return _height;}
		int getSide() const {return _side;}

	private:
		float _distance;
		float _height;
		int _side;
	};
}

#endif //SUPER_HAXAGON_WALL_HPP
