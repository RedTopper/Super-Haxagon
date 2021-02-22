#include "Factories/Pattern.hpp"

#include "Core/Twist.hpp"
#include "Driver/Platform.hpp"

namespace SuperHaxagon {
	const char* PatternFactory::PATTERN_HEADER = "PTN1.1";
	const char* PatternFactory::PATTERN_FOOTER = "ENDPTN";

	Pattern::Pattern(std::vector<Wall>& walls, const int sides) : _walls(std::move(walls)), _sides(sides) {}

	float Pattern::getFurthestWallDistance() const {
		const auto furthest = std::max_element(_walls.begin(), _walls.end(), [](const auto& a, const auto& b) {
			// true if first is less than second
			return a.getDistance() + a.getHeight() < b.getDistance() + b.getHeight();
		});

		return furthest->getDistance() + furthest->getHeight();
	}

	float Pattern::getClosestWallDistance() const {
		return std::min_element(_walls.begin(), _walls.end(), [](const auto& a, const auto& b) {
			// true if a is less than b
			return a.getDistance() < b.getDistance();
		})->getDistance();
	}

	void Pattern::advance(const float speed) {
		for(auto& wall : _walls) {
			wall.advance(speed);
		}
	}

	PatternFactory::PatternFactory(std::istream& stream, Platform& platform) {
		_name = readString(stream, platform, "pattern name");

		if (!readCompare(stream, PATTERN_HEADER)) {
			platform.message(Dbg::WARN, "pattern", _name + " pattern header invalid!");
			return;
		}

		// This might be able to be increased later
		_sides = read32(stream, 0, 256, platform, _name + " pattern sides");
		if(_sides < MIN_PATTERN_SIDES) _sides = MIN_PATTERN_SIDES;

		const int numWalls = read32(stream, 1, 1000, platform, _name + " pattern walls");
		for (auto i = 0; i < numWalls; i++) _walls.emplace_back(stream, _sides);

		if (!readCompare(stream, PATTERN_FOOTER)) {
			platform.message(Dbg::WARN, "pattern", _name + " pattern footer invalid!");
			return;
		}

		_loaded = true;
	}

	PatternFactory::~PatternFactory() = default;

	Pattern PatternFactory::instantiate(Twist& rng, const float distance) const {
		const auto offset = rng.rand(_sides - 1);
		std::vector<Wall> active;
		for(const auto& wall : _walls) {
			active.emplace_back(wall.instantiate(distance, offset, _sides));
		}

		return {active, _sides};
	}
}
