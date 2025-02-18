#include "Objects/Level.hpp"

#include "Core/Game.hpp"
#include "Core/Twist.hpp"
#include "Factories/LevelFactory.hpp"
#include "Factories/PatternFactory.hpp"

#include <algorithm>

namespace SuperHaxagon {
	Level::Level(const LevelFactory& factory, Twist& rng, const float patternDistCreate, const float rotation, const float cursorPos) : _factory(&factory) {
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
		_cursorPos = cursorPos != 0.0f ? cursorPos : -TAU/4.0f + (factory.getSpeedCursor() / 2.0f);
		_rotation = rotation;
	}

	Level::~Level() = default;

	void Level::update(Twist& rng, const float dilation) {
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
					_colorNext[location] = rotateColor(_colorNext[location], 90);
				} 
			}
		}

		// Calculate colors
		const auto percentTween = _tweenFrame / static_cast<float>(_factory->getSpeedPulse());
		_colorCurrent = GameColors{
				interpolateColor(_color.at(LocColor::FG), _colorNext.at(LocColor::FG), percentTween),
				interpolateColor(_color.at(LocColor::BG1), _colorNext.at(LocColor::BG1), percentTween),
				interpolateColor(_color.at(LocColor::BG2), _colorNext.at(LocColor::BG2), percentTween),
		};

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
			advanceWalls(rng, HEX_LENGTH, 2.0f);
		} else {
			reverseWalls(rng, 2.0f, 0.0f);
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
		if (_spin < 0.0f) _spin = 0.0f;

		// Beat/pulse logic
		_pulseFrame += dilation;
		if (_pulseDirection > 0) {
			_currentScaleFactor = linear(
				0,
				_pulseSize * MAX_PULSE_DISTANCE,
				_pulseFrame / FRAMES_PER_PULSE_LEAD_UP
			);

			if (_pulseFrame / FRAMES_PER_PULSE_LEAD_UP >= 1.0f) _pulseDirection = -1;
		} else if (_pulseDirection < 0) {
			_currentScaleFactor = linear(
				_pulseSize * MAX_PULSE_DISTANCE,
				0,
				(_pulseFrame-FRAMES_PER_PULSE_LEAD_UP) / FRAMES_PER_PULSE_LEAD_OUT
			);

			if (_pulseFrame / (FRAMES_PER_PULSE_LEAD_UP + FRAMES_PER_PULSE_LEAD_OUT) >= 1.0f) _pulseDirection = 0;
		}

		_currentScaleFactor = std::clamp(_currentScaleFactor, 0.0f, _pulseSize * MAX_PULSE_DISTANCE);

		// Flip level if needed
		// Cannot flip while spin effect is happening
		if (_spin == 0.0f && _flipFrame <= 0) {
			_multiplierRot *= -1.0f;
			_flipFrame = rng.rand(FLIP_FRAMES_MIN, FLIP_FRAMES_MAX);
		}
	}

	void Level::draw(SurfaceGame& surface, SurfaceGame* shadows, const float offset) const {
		const float realHexLength = (_currentScaleFactor * HEXAGON_PULSE_MULTIPLIER + 1.0f) * HEX_LENGTH;

		surface.calculateMatrix(_rotation, _currentScaleFactor + 1.0f);
		surface.setWallOffset(offset);

		surface.drawBackground(
				_bgInverted ? _colorCurrent.bg2 : _colorCurrent.bg1,
				_bgInverted ? _colorCurrent.bg1 : _colorCurrent.bg2,
				_sidesTween
		);

		if (shadows) {
			shadows->copyMatrix(surface);
			shadows->setWallOffset(offset);
			shadows->setDepth(-0.025f);
			shadows->drawPatterns(COLOR_SHADOW, _patterns, _sidesTween);
			shadows->drawRegular(COLOR_SHADOW, realHexLength, _sidesTween);
			if (_showCursor) shadows->drawCursor(COLOR_SHADOW, HEX_LENGTH + PLAYER_PADDING_BETWEEN_HEX, _cursorPos);
		}

		surface.drawPatterns(_colorCurrent.fg, _patterns, _sidesTween);
		surface.drawRegular(_colorCurrent.fg, realHexLength, _sidesTween);
		surface.drawRegular(_colorCurrent.bg2, (realHexLength - HEX_BORDER), _sidesTween);
		if (_showCursor) surface.drawCursor(_colorCurrent.fg, HEX_LENGTH + PLAYER_PADDING_BETWEEN_HEX, _cursorPos);

		// To help debugging
		// surface.drawDebugTriangles();
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
		_cursorPos -= _factory->getSpeedCursor() * dilation;
	}

	void Level::right(const float dilation) {
		_cursorPos += _factory->getSpeedCursor() * dilation;
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
	
	void Level::pulse(const float pulse) {
		_pulseFrame = 0.0f;
		_pulseDirection = 1;
		_pulseSize = pulse;
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

			// Delay the level if the shifted pattern does not have the same sides as the last.
			if (_sidesLast != _sidesCurrent) {
				_delayMax = FRAMES_PER_CHANGE_SIDE * static_cast<float>(std::abs(_sidesCurrent - _sidesLast));
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
}
