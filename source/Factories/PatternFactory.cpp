#include "Factories/PatternFactory.hpp"

#include "Core/Twist.hpp"
#include "Core/Platform.hpp"

namespace SuperHaxagon {
	const char* PatternFactory::PATTERN_HEADER = "PTN1.1";
	const char* PatternFactory::PATTERN_FOOTER = "ENDPTN";

	PatternFactory::PatternFactory(std::istream& stream, Platform& platform) {
		_name = readString(stream, platform, "pattern name");

		if (!readCompare(stream, PATTERN_HEADER)) {
			platform.message(Dbg::WARN, "pattern", _name + " pattern header invalid!");
			return;
		}

		// This might be able to be increased later
		_sides = read32(stream, 0, 256, platform, _name + " pattern sides");
		if(_sides < MIN_PATTERN_SIDES) _sides = MIN_PATTERN_SIDES;

		const auto numWalls = read32(stream, 1, 1000, platform, _name + " pattern walls");
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
