#include "Core/Twist.hpp"
#include "Factories/Pattern.hpp"
#include "Factories/Wall.hpp"

namespace SuperHaxagon {
	const char* PatternFactory::PATTERN_HEADER = "PTN1.1";
	const char* PatternFactory::PATTERN_FOOTER = "ENDPTN";

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

	PatternFactory::PatternFactory(std::ifstream& file) {
		name = readString(file, "pattern name");

		if (!readCompare(file, PATTERN_HEADER))
			throw malformed("pattern", name + " pattern header invalid!");

		// This might be able to be increased later
		sides = read32(file, 0, 256, name + " pattern sides");
		if(sides < MIN_PATTERN_SIDES) sides = MIN_PATTERN_SIDES;

		int numWalls = read32(file, 1, 1000, name + " pattern walls");
		for(int i = 0; i < numWalls; i++) walls.emplace_back(std::make_unique<WallFactory>(file, sides));

		if (!readCompare(file, PATTERN_FOOTER))
			throw malformed("pattern", name + " pattern footer invalid!");
	}

	PatternFactory::~PatternFactory() = default;

	std::unique_ptr<Pattern> PatternFactory::instantiate(Twist& rng, double distance) const {
		int offset = rng.rand(sides);
		std::vector<std::unique_ptr<Wall>> active;
		for(const auto& wall : walls) {
			active.emplace_back(wall->instantiate(distance, offset, sides));
		}

		return std::make_unique<Pattern>(std::move(active), sides);
	}
}
