#include "Level.hpp"
#include "Game.hpp"

namespace SuperHaxagon {
	Level::Level() {
		nextIndexBG1 = (level.getColorsBG1().size() > 1 ? 1 : 0);
		nextIndexBG2 = (level.getColorsBG2().size() > 1 ? 1 : 0);
		nextIndexFG = (level.getColorsFG().size() > 1 ? 1 : 0);

		//fetch some random starting patterns
		auto distance = (double)game.getRenderDistance();
		for(int i = 0;  i < TOTAL_PATTERNS_AT_ONE_TIME;  i++) {
			int pCount = level.getPatterns().size();
			int pSelected = game.getTwister().rand(pCount);
			auto& pattern = level.getPatterns()[pSelected];
			patterns.emplace_back(pattern.instantiate(game.getTwister(), distance));
			distance = patterns.back().getFurthestWallDistance();
		}

		//set up the amount of sides the level should have.
		lastSides = patterns.front().getSides();
		currentSides = patterns.front().getSides();
		cursorPos = TAU/4.0 + (level.getSpeedCursor() / 2.0);
	}
}