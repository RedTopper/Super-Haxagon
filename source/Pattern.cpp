#include "Pattern.hpp"
#include "Twist.hpp"

namespace SuperHaxagon {
	Pattern::Pattern(std::vector<Wall> walls, int sides) : walls(std::move(walls)), sides(sides) {}

	double Pattern::getFurthestWallDistance() const {
		double maxDistance = 0;
		for(const auto& wall : walls) {
			double distance = wall.getDistance() + wall.getHeight();
			if(distance > maxDistance) maxDistance = distance;
		}
		return maxDistance;
	}

	void Pattern::advance(double speed) {
		for(auto& wall : walls) {
			wall.advance(speed);
		}
	}

	Pattern PatternFactory::instantiate(Twist& rng, double distance) const {
		int offset = rng.rand(sides);
		std::vector<Wall> active;
		for(const WallFactory& wall : walls) {
			active.emplace_back(wall.instantiate(*this, distance, offset));
		}

		return {active, sides};
	}


}
