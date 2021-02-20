#include "Factories/Level.hpp"

#include "Core/Game.hpp"
#include "Core/Twist.hpp"
#include "Driver/Platform.hpp"

namespace SuperHaxagon {
	const char* LevelFactory::LEVEL_HEADER = "LEV3.0";
	const char* LevelFactory::LEVEL_FOOTER = "ENDLEV";

	Level::Level(const LevelFactory& factory, Twist& rng, const float patternDistCreate) : _factory(&factory) {
		for (auto i = COLOR_LOCATION_FIRST; i != COLOR_LOCATION_LAST; i++) {
			const auto location = static_cast<LocColor>(i);
			const auto& colors = factory.getColors().at(location);
			_color[location] = colors[0];
			_colorNextIndex[location] = colors.size() > 1 ? 1 : 0;
			_colorNext[location] = colors[_colorNextIndex[location]];
		}

		//fetch a starting pattern
		_patterns.emplace_back(getRandomPattern(rng).instantiate(rng, patternDistCreate));

		//set up the amount of sides the level should have.
		_sidesLast = _patterns.front().getSides();
		_sidesCurrent = _patterns.front().getSides();
		_cursorPos = TAU/4.0f + (factory.getSpeedCursor() / 2.0f);
	}

	Level::~Level() = default;

	void Level::update(Twist& rng, const float patternDistDelete, const float patternDistCreate, const float dilation) {
		// Update frame
		_frame += dilation;
		
		// Update color frame and clamp
		_tweenFrame += dilation;
		if(_tweenFrame >= static_cast<float>(_factory->getSpeedPulse())) {
			_tweenFrame = 0;
			for (auto i = COLOR_LOCATION_FIRST; i != COLOR_LOCATION_LAST; i++) {
				const auto location = static_cast<LocColor>(i);
				const auto& availableColors = _factory->getColors().at(location);

				_color[location] = _colorNext[location];
				_colorNextIndex[location] = _colorNextIndex[location] + 1 < availableColors.size() ? _colorNextIndex[location] + 1 : 0;
				_colorNext[location] = availableColors[_colorNextIndex[location]];
				if (_frame > 60.0f * 60.0f) {
					_colorNext[location] = rotateColor(_colorNext[location], 180);
				} 
			}
		}

		// Bring walls forward if we are not delaying
		// Otherwise tween from one shape to another.
		if (_delayFrame <= 0) {
			_sidesTween = static_cast<float>(_sidesCurrent);
			for (auto& pattern : _patterns) {
				pattern.advance(_factory->getSpeedWall() * dilation * _multiplierWalls);
			}
		} else {
			const auto percent = _delayFrame / _delayMax;
			_sidesTween = linear(static_cast<float>(_sidesCurrent), static_cast<float>(_sidesLast), percent);
			_delayFrame -= dilation;
		}

		// Move the walls (either closer to the player or away from the hexagon)
		if (_multiplierWalls > 0) {
			advanceWalls(rng, patternDistDelete, patternDistCreate);
		} else {
			reverseWalls(rng, patternDistDelete, patternDistCreate);
		}

		// Rotate level
		if (_rotateToZero) {
			// Trying to snap back to zero
			_rotation += ROTATE_ZERO_SPEED * (_rotation < PI ? -1.0f : 1.0f);
			if (_rotation <= 0 || _rotation >= TAU) {
				_rotation = 0;
				_rotateToZero = false;
			}
		} else {
			// We are rotating normally
			_rotation += (_factory->getSpeedRotation() + _spin) * _multiplierRot * dilation;
			if (_rotation >= TAU) _rotation -= TAU;
			if (_rotation < 0) _rotation += TAU;
		}

		// Update effect timings
		_flipFrame -= dilation;
		_spin -= SPIN_SPEED / FRAMES_PER_SPIN * dilation;
		_pulse -= PULSE_DISTANCE / FRAMES_PER_PULSE * dilation;
		if (_spin < 0) _spin = 0;
		if (_pulse < 0) _pulse = 0;

		// Flip level if needed
		// Cannot flip while spin effect is happening
		if(static_cast<int>(_spin) == 0 && _flipFrame <= 0) {
			_multiplierRot *= -1.0f;
			_flipFrame = rng.rand(FLIP_FRAMES_MIN, FLIP_FRAMES_MAX);
		}
	}

	void Level::draw(Game& game, const float scale, const float offsetWall) const {

		// Calculate colors
		const auto percentTween = _tweenFrame / static_cast<float>(_factory->getSpeedPulse());
		const auto fg = interpolateColor(_color.at(LocColor::FG), _colorNext.at(LocColor::FG), percentTween);
		const auto bg1 = interpolateColor(_color.at(LocColor::BG1), _colorNext.at(LocColor::BG1), percentTween);
		const auto bg2 = interpolateColor(_color.at(LocColor::BG2), _colorNext.at(LocColor::BG2), percentTween);

		// Fix for triangle levels
		const auto diagonal = _sidesTween >= 3.0f && _sidesTween < 4.0f ?  2.0f : 1.0f;

		const auto center = game.getScreenCenter();
		const auto shadow = game.getShadowOffset();

		game.drawBackground(_bgInverted ? bg2 : bg1, _bgInverted ? bg1 : bg2, center, diagonal, _rotation, _sidesTween);

		const auto cursorDistance = SCALE_HEX_LENGTH + SCALE_HUMAN_PADDING;

		// Draw shadows, if supported
		if (static_cast<int>(game.getPlatform().supports() & Supports::SHADOWS)) {
			const Point offsetFocus = { center.x + shadow.x, center.y + shadow.y };
			game.drawPatterns(COLOR_SHADOW, offsetFocus, _patterns, _rotation, _sidesTween, offsetWall + _pulse, scale);
			game.drawRegular(COLOR_SHADOW, offsetFocus, (SCALE_HEX_LENGTH + _pulse) * scale, _rotation, _sidesTween);
			if (_showCursor) game.drawCursor(COLOR_SHADOW, offsetFocus, _cursorPos, _rotation, _pulse + cursorDistance, scale);
		}

		// Draw real thing
		game.drawPatterns(fg, center, _patterns, _rotation, _sidesTween, offsetWall + _pulse, scale);
		game.drawRegular(fg, center, (SCALE_HEX_LENGTH + _pulse) * scale, _rotation, _sidesTween);
		game.drawRegular(bg2, center, (SCALE_HEX_LENGTH - SCALE_HEX_BORDER + _pulse) * scale, _rotation, _sidesTween);
		if (_showCursor) game.drawCursor(fg, center, _cursorPos, _rotation, _pulse + cursorDistance, scale);
	}

	Movement Level::collision(const float cursorDistance, const float dilation) const {
		auto collision = Movement::CAN_MOVE;

		// For all patterns (technically only need to check front two)
		for(const auto& pattern : _patterns) {

			// For all walls
			for(const auto& wall : pattern.getWalls()) {
				const auto check = wall.collision(cursorDistance, _cursorPos, _factory->getSpeedCursor() * dilation, pattern.getSides());

				// Update collision
				if(collision == Movement::CAN_MOVE) collision = check; //If we can move, try and replace it with something else
				if(check == Movement::DEAD)  { //If we are ever dead, return it.
					return Movement::DEAD;
				}
			}
		}

		return collision;
	}

	void Level::increaseMultiplier() {
		const auto dir = (_multiplierRot > 0 ? 1 : -1);
		_multiplierRot += static_cast<float>(dir) * DIFFICULTY_SCALAR_ROT;
		_multiplierWalls += DIFFICULTY_SCALAR_WALLS;
	}

	void Level::clearPatterns() {
		_patterns.clear();
	}

	void Level::rotate(const float distance, const float dilation) {
		_rotation += distance * dilation;
	}

	void Level::left(const float dilation) {
		_cursorPos += _factory->getSpeedCursor() * dilation;
	}

	void Level::right(const float dilation) {
		_cursorPos -= _factory->getSpeedCursor() * dilation;
	}

	void Level::clamp() {
		if(_cursorPos >= TAU) _cursorPos -= TAU;
		if(_cursorPos < 0) _cursorPos  += TAU;
	}

	void Level::spin() {
		_spin = SPIN_SPEED;
	}
	
	void Level::invertBG() {
		_bgInverted = !_bgInverted;
	}
	
	void Level::pulse(const float scale) {
		_pulse = PULSE_DISTANCE * scale;
	}

	void Level::setWinFactory(const LevelFactory* factory) {
		_factory = factory;
	}

	void Level::setWinSides(const int sides) {
		_sidesLast = _sidesCurrent;
		_sidesCurrent = sides;

		if (_sidesLast != _sidesCurrent) {
			_delayMax = FRAMES_PER_CHANGE_SIDE;
			_delayFrame = _delayMax;
		}
	}

	void Level::resetColors() {
		for (auto& e : _colorNextIndex) {
			e.second = 0;
		}
	}

	void Level::advanceWalls(Twist& rng, const float patternDistDelete, const float patternDistCreate) {
		// Shift patterns forward
		if (_patterns.front().getFurthestWallDistance() < patternDistDelete) {
			_sidesLast = _patterns.front().getSides();
			_patterns.pop_front();
			_sidesCurrent = _patterns.front().getSides();

			// Delay the level if the shifted pattern does  not have the same sides as the last.
			if (_sidesLast != _sidesCurrent) {
				_delayMax = FRAMES_PER_CHANGE_SIDE / _factory->getSpeedWall() * static_cast<float>(std::abs(_sidesCurrent - _sidesLast));
				_delayFrame = _delayMax;
			}
		}

		// Create new pattern if needed
		if (_patterns.size() < 2 || _patterns.back().getFurthestWallDistance() < patternDistCreate) {
			_patterns.emplace_back(getRandomPattern(rng).instantiate(rng, _patterns.back().getFurthestWallDistance()));
		}
	}

	auto Level::reverseWalls(Twist& rng, const float patternDistDelete, const float patternDistCreate) -> void {
		if (_patterns.back().getClosestWallDistance() > patternDistDelete && _patterns.size() > 1) {
			_patterns.pop_back();
		}

		// Create a new pattern at the front.
		// We need to advance it so the last wall is where we create the patterns
		if (_patterns.front().getClosestWallDistance() > patternDistCreate + _frontGap && _autoPatternCreate) {
			auto pattern = getRandomPattern(rng).instantiate(rng, patternDistCreate);
			_frontGap = pattern.getClosestWallDistance() * 1.5f; // Too small of a gap otherwise
			pattern.advance(pattern.getFurthestWallDistance());
			_patterns.emplace_front(pattern);
			if (pattern.getSides() != _sidesCurrent) setWinSides(pattern.getSides());
		}
	}

	const PatternFactory& Level::getRandomPattern(Twist& rng) {
		const auto& patterns = _factory->getPatterns();
		if (_sameCount <= 0) {
			const auto& pattern = *patterns[rng.rand(static_cast<int>(patterns.size()) - 1)];
			if (pattern.getSides() != _sameSides) {
				_sameSides = pattern.getSides();
				_sameCount = rng.rand(MIN_SAME_SIDES, MAX_SAME_SIDES);
			}

			return pattern;
		}

		_sameCount--;
		std::vector<std::shared_ptr<PatternFactory>> selectable;
		for (const auto& pattern : patterns) {
			if (pattern->getSides() == _sameSides) {
				selectable.push_back(pattern);
			}
		}

		// While this never should be hit, it's possible to change the factory
		// during runtime so a new factory might not have levels with the same
		// amount of sides as the last one
		if (selectable.empty()) {
			_sameCount = 0;
			_sameSides = 0;
			return *patterns[rng.rand(static_cast<int>(patterns.size()) - 1)];
		} 

		return *selectable[rng.rand(static_cast<int>(selectable.size()) - 1)];
	}

	LevelFactory::LevelFactory(std::ifstream& file, std::vector<std::shared_ptr<PatternFactory>>& shared, const LocLevel location, Platform& platform, const size_t levelIndexOffset) {
		_location = location;

		if (!readCompare(file, LEVEL_HEADER)) {
			platform.message(Dbg::WARN, "level", "level header invalid!");
			return;
		}

		_name = readString(file, platform, "level name");
		_difficulty = readString(file, platform, _name + " level difficulty");
		_mode = readString(file, platform, _name + " level mode");
		_creator = readString(file, platform, _name + " level creator");
		_music = "/" + readString(file, platform, _name + " level music");

		const auto numColorsBG1 = read32(file, 1, 512, platform, "level background 1");
		_colors[LocColor::BG1].reserve(numColorsBG1);
		for (auto i = 0; i < numColorsBG1; i++) _colors[LocColor::BG1].emplace_back(readColor(file));

		const auto numColorsBG2 = read32(file, 1, 512, platform, "level background 2");
		_colors[LocColor::BG2].reserve(numColorsBG2);
		for (auto i = 0; i < numColorsBG2; i++) _colors[LocColor::BG2].emplace_back(readColor(file));

		const auto numColorsFG = read32(file, 1, 512, platform, "level foreground");
		_colors[LocColor::FG].reserve(numColorsFG);
		for (auto i = 0; i < numColorsFG; i++) _colors[LocColor::FG].emplace_back(readColor(file));

		_speedWall = readFloat(file);
		_speedRotation = readFloat(file);
		_speedCursor = readFloat(file);
		_speedPulse = read32(file, 4, 8192, platform, "level pulse");
		_nextIndex = read32(file, -1, 8192, platform, "next index");
		_nextTime = readFloat(file);

		// Negative numbers should remain invalid. -1 usually means load no other level.
		if (_nextIndex >= 0) _nextIndex += static_cast<int>(levelIndexOffset);

		const auto numPatterns = read32(file, 1, 512, platform, "level pattern count");
		for (auto i = 0; i < numPatterns; i++) {
			auto found = false;
			auto search = readString(file, platform, "level pattern name match");
			for (const auto& pattern : shared) {
				if (pattern->getName() == search) {
					_patterns.push_back(pattern);
					found = true;
					break;
				}
			}

			if (!found) {
				platform.message(Dbg::WARN, "level", "could not find pattern " + search + " for " + _name);
				return;
			}
		}

		if (!readCompare(file, LEVEL_FOOTER)) {
			platform.message(Dbg::WARN, "level", "level footer invalid!");
			return;
		}

		_loaded = true;
	}

	std::unique_ptr<Level> LevelFactory::instantiate(Twist& rng, float renderDistance) const {
		return std::make_unique<Level>(*this, rng, renderDistance);
	}

	bool LevelFactory::setHighScore(const int score) {
		if(score > _highScore) {
			_highScore = score;
			return true;
		}

		return false;
	}
}
