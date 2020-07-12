#include "Core/Twist.hpp"
#include "Factories/Pattern.hpp"
#include "Factories/Wall.hpp"

namespace SuperHaxagon {
	const char* PatternFactory::PATTERN_HEADER = "PTN1.1";
	const char* PatternFactory::PATTERN_FOOTER = "ENDPTN";

	Pattern::Pattern(std::vector<std::unique_ptr<Wall>> walls, const int sides) : _walls(std::move(walls)), _sides(sides) {}

	double Pattern::getFurthestWallDistance() const {
		double maxDistance = 0;
		for(const auto& wall : _walls) {
			const auto distance = wall->getDistance() + wall->getHeight();
			if(distance > maxDistance) maxDistance = distance;
		}

		return maxDistance;
	}

	void Pattern::advance(const double speed) {
		for(auto& wall : _walls) {
			wall->advance(speed);
		}
	}

	PatternFactory::PatternFactory(std::ifstream& file) {
		_name = readString(file, "pattern name");

		if (!readCompare(file, PATTERN_HEADER)) {
			warn("pattern", _name + " pattern header invalid!");
			return;
		}

		// This might be able to be increased later
		_sides = read32(file, 0, 256, _name + " pattern sides");
		if(_sides < MIN_PATTERN_SIDES) _sides = MIN_PATTERN_SIDES;

		const int numWalls = read32(file, 1, 1000, _name + " pattern walls");
		for(auto i = 0; i < numWalls; i++) _walls.emplace_back(std::make_unique<WallFactory>(file, _sides));

		if (!readCompare(file, PATTERN_FOOTER)) {
			warn("pattern", _name + " pattern footer invalid!");
			return;
		}

		_loaded = true;
	}

	PatternFactory::~PatternFactory() = default;

	std::unique_ptr<Pattern> PatternFactory::instantiate(Twist& rng, const double distance) const {
		const auto offset = rng.rand(_sides - 1);
		std::vector<std::unique_ptr<Wall>> active;
		for(const auto& wall : _walls) {
			active.emplace_back(wall->instantiate(distance, offset, _sides));
		}

		return std::make_unique<Pattern>(std::move(active), _sides);
	}
}
