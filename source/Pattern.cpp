#include "Pattern.hpp"
#include "Twist.hpp"
#include "Wall.hpp"

namespace SuperHaxagon {
	Pattern::Pattern(std::vector<std::unique_ptr<Wall>> walls, int sides) : walls(std::move(walls)), sides(sides) {}

	double Pattern::getFurthestWallDistance() const {
		double maxDistance = 0;
		for(const auto& wall : walls) {
			double distance = wall->getDistance() + wall->getHeight();
			if(distance > maxDistance) maxDistance = distance;
		}
		return maxDistance;
	}

	void Pattern::advance(double speed) {
		for(auto& wall : walls) {
			wall->advance(speed);
		}
	}

	std::unique_ptr<Pattern> PatternFactory::instantiate(Twist& rng, double distance) const {
		int offset = rng.rand(sides);
		std::vector<std::unique_ptr<Wall>> active;
		for(const auto& wall : walls) {
			active.emplace_back(wall->instantiate(distance, offset, sides));
		}

		return std::make_unique<Pattern>(active, sides);
	}
}
