#include "Pattern.hpp"
#include "Twist.hpp"

namespace SuperHaxagon {
	PatternActive::PatternActive(std::vector<WallActive> walls, int sides) : walls(std::move(walls)), sides(sides) {}

	double PatternActive::getFurthestWallDistance() const {
		double maxDistance = 0;
		for(const auto& wall : walls) {
			double distance = wall.getDistance() + wall.getHeight();
			if(distance > maxDistance) maxDistance = distance;
		}
		return maxDistance;
	}

	void PatternActive::advance(double speed) {
		for(auto& wall : walls) {
			wall.advance(speed);
		}
	}

	PatternActive Pattern::instantiate(Twist& rng, double distance) const {
		int offset = rng.rand(sides);
		std::vector<WallActive> active;
		for(const Wall& wall : walls) {
			active.emplace_back(wall.instantiate(*this, distance, offset));
		}

		return {active, sides};
	}


}
