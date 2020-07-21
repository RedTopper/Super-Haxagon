#include "Core/Twist.hpp"
#include "Core/Game.hpp"
#include "Factories/Level.hpp"
#include "Factories/Pattern.hpp"
#include "Factories/Wall.hpp"

namespace SuperHaxagon {
	const char* LevelFactory::LEVEL_HEADER = "LEV2.1";
	const char* LevelFactory::LEVEL_FOOTER = "ENDLEV";

	Level::Level(const LevelFactory& factory, Twist& rng, const double patternDistCreate) : _factory(factory) {
		_nextIndexBg1 = (factory.getColorsBG1().size() > 1 ? 1 : 0);
		_nextIndexBg2 = (factory.getColorsBG2().size() > 1 ? 1 : 0);
		_nextIndexFg = (factory.getColorsFG().size() > 1 ? 1 : 0);

		//fetch some random starting patterns
		auto distance = patternDistCreate;
		do {
			const auto pCount = static_cast<int>(factory.getPatterns().size());
			const auto pSelected = rng.rand(pCount - 1);
			const auto& pattern = factory.getPatterns()[pSelected];
			_patterns.emplace_back(pattern->instantiate(rng, distance));
			distance = _patterns.back()->getFurthestWallDistance();
		} while (_patterns.back()->getFurthestWallDistance() < patternDistCreate);

		//set up the amount of sides the level should have.
		_lastSides = _patterns.front()->getSides();
		_currentSides = _patterns.front()->getSides();
		_cursorPos = TAU/4.0 + (factory.getSpeedCursor() / 2.0);
	}

	Level::~Level() = default;

	void Level::update(Twist& rng, const double patternDistDelete, const double patternDistCreate, const double dilation) {
		// Update color frame and clamp
		_tweenFrame += dilation;
		if(_tweenFrame >= _factory.getSpeedPulse()) {
			_tweenFrame = 0;
			_indexBg1 = _nextIndexBg1;
			_indexBg2 = _nextIndexBg2;
			_indexFg = _nextIndexFg;
			_nextIndexBg1 = _indexBg1 + 1 < static_cast<int>(_factory.getColorsBG1().size()) ? _indexBg1 + 1 : 0;
			_nextIndexBg2 = _indexBg2 + 1 < static_cast<int>(_factory.getColorsBG2().size()) ? _indexBg2 + 1 : 0;
			_nextIndexFg = _indexFg + 1 < static_cast<int>(_factory.getColorsFG().size()) ? _indexFg + 1 : 0;
		}

		// Bring walls forward if we are not delaying
		// Otherwise tween from one shape to another.
		if(_delayFrame <= 0) {
			_sidesTween = _currentSides;
			for(auto& pattern : _patterns) {
				pattern->advance(_factory.getSpeedWall() * dilation * _multiplierWalls);
			}
		} else {
			const auto percent = _delayFrame / _delayMax;
			_sidesTween = linear(_currentSides, _lastSides, percent);
			_delayFrame -= dilation;
		}

		// Shift patterns forward
		if(_patterns.front()->getFurthestWallDistance() < patternDistDelete) {
			_lastSides = _patterns.front()->getSides();
			_patterns.pop_front();
			_currentSides = _patterns.front()->getSides();

			// Delay the level if the shifted pattern does  not have the same sides as the last.
			if(_lastSides != _currentSides) {
				_delayMax = FRAMES_PER_CHANGE_SIDE / _factory.getSpeedWall() * static_cast<double>(std::abs(_currentSides - _lastSides));
				_delayFrame = _delayMax;
			}
		}

		// Create new pattern if needed
		if (_patterns.size() < 2 || _patterns.back()->getFurthestWallDistance() < patternDistCreate) {
			const auto pCount = static_cast<int>(_factory.getPatterns().size());
			const auto pSelected = rng.rand(pCount - 1);
			const auto& pattern = _factory.getPatterns()[pSelected];
			_patterns.emplace_back(pattern->instantiate(rng, _patterns.back()->getFurthestWallDistance()));
		}

		// Rotate level
		_rotation += _factory.getSpeedRotation() * _multiplierRot * dilation;
		if(_rotation >= TAU) _rotation -= TAU;
		if(_rotation < 0) _rotation  += TAU;

		// Flip level if needed
		_flipFrame -= dilation;
		if(_flipFrame <= 0) {
			_multiplierRot *= -1.0;
			_flipFrame = rng.rand(FLIP_FRAMES_MIN, FLIP_FRAMES_MAX);
		}
	}

	void Level::draw(Game& game, const double scale, const double offset, const double beat) const {

		// Calculate colors
		const auto percentTween = _tweenFrame / _factory.getSpeedPulse();
		const auto fg = interpolateColor(_factory.getColorsFG()[_indexFg], _factory.getColorsFG()[_nextIndexFg], percentTween);
		const auto bg1 = interpolateColor(_factory.getColorsBG1()[_indexBg1], _factory.getColorsBG1()[_nextIndexBg1], percentTween);
		const auto bg2 = interpolateColor(_factory.getColorsBG2()[_indexBg2], _factory.getColorsBG2()[_nextIndexBg2], percentTween);

		// Fix for triangle levels
		const double diagonal = _sidesTween >= 3 && _sidesTween < 4 ?  2 : 1;

		const auto center = game.getScreenCenter();
		const auto shadow = game.getShadowOffset();

		game.drawBackground(bg1, bg2, center, diagonal, _rotation, _sidesTween);

		// Draw shadows
		const auto cursorDistance = SCALE_HEX_LENGTH + SCALE_HUMAN_PADDING;
		const Point offsetFocus = {center.x + shadow.x, center.y + shadow.y};
		game.drawPatterns(COLOR_SHADOW, offsetFocus, _patterns, _rotation, _sidesTween, offset + beat, scale);
		game.drawRegular(COLOR_SHADOW, offsetFocus, (SCALE_HEX_LENGTH + beat) * scale, _rotation, _sidesTween);
		game.drawCursor(COLOR_SHADOW, offsetFocus, _cursorPos, _rotation, beat + cursorDistance, scale);

		// Draw real thing
		game.drawPatterns(fg, center, _patterns, _rotation, _sidesTween, offset+ beat, scale);
		game.drawRegular(fg, center, (SCALE_HEX_LENGTH + beat) * scale, _rotation, _sidesTween);
		game.drawRegular(bg2, center, (SCALE_HEX_LENGTH - SCALE_HEX_BORDER + beat) * scale, _rotation, _sidesTween);
		game.drawCursor(fg, center, _cursorPos, _rotation, beat + cursorDistance, scale);
	}

	Movement Level::collision(const double cursorDistance, const double dilation) const {
		auto collision = Movement::CAN_MOVE;

		// For all patterns (technically only need to check front two)
		for(const auto& pattern : _patterns) {

			// For all walls
			for(const auto& wall : pattern->getWalls()) {
				const auto check = wall->collision(cursorDistance, _cursorPos, _factory.getSpeedCursor() * dilation, pattern->getSides());

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
		_multiplierRot += dir * DIFFICULTY_SCALAR_ROT;
		_multiplierWalls += DIFFICULTY_SCALAR_WALLS;
	}

	void Level::clearPatterns() {
		_patterns.clear();
	}

	void Level::rotate(const double distance, const double dilation) {
		_rotation += distance * dilation;
	}

	void Level::left(const double dilation) {
		_cursorPos += _factory.getSpeedCursor() * dilation;
	}

	void Level::right(const double dilation) {
		_cursorPos -= _factory.getSpeedCursor() * dilation;
	}

	void Level::clamp() {
		if(_cursorPos >= TAU) _cursorPos -= TAU;
		if(_cursorPos < 0) _cursorPos  += TAU;
	}

	LevelFactory::LevelFactory(std::ifstream& file, std::vector<std::shared_ptr<PatternFactory>>& shared, const Location location, Platform& platform) {
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

		const int numColorsBG1 = read32(file, 1, 512, platform, "level background 1");
		_colorsBg1.reserve(numColorsBG1);
		for (auto i = 0; i < numColorsBG1; i++) _colorsBg1.emplace_back(readColor(file));

		const int numColorsBG2 = read32(file, 1, 512, platform, "level background 2");
		_colorsBg2.reserve(numColorsBG2);
		for (auto i = 0; i < numColorsBG2; i++) _colorsBg2.emplace_back(readColor(file));

		const int numColorsFG = read32(file, 1, 512, platform, "level foreground");
		_colorsFg.reserve(numColorsFG);
		for (auto i = 0; i < numColorsFG; i++) _colorsFg.emplace_back(readColor(file));

		_speedWall = readFloat(file);
		_speedRotation = readFloat(file);
		_speedCursor = readFloat(file);
		_speedPulse = read32(file, 4, 8192, platform, "level pulse");

		const int numPatterns = read32(file, 1, 512, platform, "level pattern count");
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

	std::unique_ptr<Level> LevelFactory::instantiate(Twist& rng, double renderDistance) const {
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
