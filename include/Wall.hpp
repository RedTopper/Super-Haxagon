#ifndef SUPER_HAXAGON_WALL_HPP
#define SUPER_HAXAGON_WALL_HPP

namespace SuperHaxagon {
	class Pattern;
	class WallActive {
	public:
		WallActive(double distance, double height, int side);

		void advance(double speed) {distance -= speed;}

		double getDistance() const {return distance;}
		double getHeight() const {return height;}
		int getSide() const {return side;}

	private:
		double distance;
		double height;
		int side;
	};

	class Wall {
	public:
		Wall(short distance, short height, short side);

		WallActive instantiate(const Pattern& pattern, double distance, int offset) const;
		short getDistance() const {return distance;}
		short getHeight() const {return height;}
		short getSide() const {return side;}

	private:
		short distance;
		short height;
		short side;
	};
}

#endif //SUPER_HAXAGON_WALL_HPP
