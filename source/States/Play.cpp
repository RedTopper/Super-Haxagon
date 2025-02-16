#include "States/Play.hpp"

#include "Core/Game.hpp"
#include "Core/Metadata.hpp"
#include "Driver/Font.hpp"
#include "Driver/Music.hpp"
#include "Factories/LevelFactory.hpp"
#include "Objects/Level.hpp"
#include "States/Over.hpp"
#include "States/Quit.hpp"
#include "States/Transition.hpp"
#include "States/Win.hpp"

namespace SuperHaxagon {

	Play::Play(Game& game, LevelFactory& factory, LevelFactory& selected, const float startScore) :
		_game(game),
		_platform(game.getPlatform()),
		_factory(factory),
		_selected(selected),
		_level(factory.instantiate(game.getTwister(), BASE_DISTANCE)),
		_score(startScore)
	{}

	Play::~Play() = default;

	void Play::enter() {
		auto* bgm = _game.getMusic();
		if (bgm && !_factory.isCreditsLevel()) bgm->play();
		_game.playEffect(SoundEffect::BEGIN);
		_game.setNextCamera(
				{0, -std::sin(SKEW_MIN) * 4.0f, std::cos(SKEW_MIN) * 4.0f},
				{0.0f, 0.0f, 0.0f},
				60.0f
		);
	}

	void Play::exit() {
		auto* bgm = _game.getMusic();
		if (bgm) bgm->pause();
	}

	std::unique_ptr<State> Play::update(const float dilation) {
		// It's technically possible that the BGM metadata was not set
		if (_game.getBGMMetadata()) {

			// Get effect data
			auto& metadata = *_game.getBGMMetadata();
			const auto* bgm = _game.getMusic();
			const auto time = bgm ? bgm->getTime() : 0.0f;

			// Apply effects. More can be added here if needed.
			if (metadata.getMetadata(time, "S")) _level->spin();
			if (metadata.getMetadata(time, "I")) _level->invertBG();
			if (metadata.getMetadata(time, "BL")) _level->pulse(1.0f);
			if (metadata.getMetadata(time, "BS")) _level->pulse(0.66f);
		}

		// Camera
		if (!_game.isCameraMoving()) {
			_skewing = !_skewing;
			auto skewFrameMax = static_cast<float>(_level->getLevelFactory().getSpeedPulse()) * 2.5f;
			skewFrameMax = skewFrameMax < SKEW_MIN_FRAMES ? SKEW_MIN_FRAMES : skewFrameMax;
			_game.setNextCamera(
					{0, -std::sin(_skewing?SKEW_MAX:SKEW_MIN) * 4.0f, std::cos(_skewing?SKEW_MAX:SKEW_MIN) * 4.0f},
					{0, 0, 0},
					skewFrameMax
			);
		}

		// Update level
		const auto previousFrame = _level->getFrame();
		_level->update(_game.getTwister(), dilation);

		// Button presses
		const auto pressed = _platform.getPressed();

		// Check collision
		const auto cursorDistance = HEX_LENGTH + PLAYER_PADDING_BETWEEN_HEX + PLAYER_TRI_HEIGHT;
		const auto hit = _level->collision(cursorDistance, dilation);

		// Keys
		if(pressed.back || hit == Movement::DEAD) {
			if (&_factory != &_selected &&
			    _factory.getCreator() == "REDHAT" && 
			    (_factory.getName() == "VOID" || _factory.getName() == "NULL") &&
			    _factory.getDifficulty() == "???" &&
			    (_factory.getMode() == "???" || _factory.getMode() == "ORIGINAL")) {
				// Play the super special win animation if you are on the last level without selecting it
				// Congrats, you just won the game!
				return std::make_unique<Win>(_game, std::move(_level), _selected, _score, "WONDERFUL");
			}

			if (_factory.isCreditsLevel()) {
				// Cheater
				return std::make_unique<Win>(_game, std::move(_level), _selected, 0.0f, "CHEATER");
			}

			return std::make_unique<Over>(_game, std::move(_level), _selected, _score, "GAME OVER");
		}

		if (pressed.quit) {
			return std::make_unique<Quit>(_game);
		}

		// Go to the next level
		const auto next = _factory.getNextIndex();
		if (next >= 0 && 
		    static_cast<size_t>(next) < _game.getLevels().size() && 
		    _level->getFrame() > 60.0f * _level->getLevelFactory().getNextTime()) {
			return std::make_unique<Transition>(_game, std::move(_level), _selected, _score);
		}

		// Process movement
		if (pressed.left && hit != Movement::CANNOT_MOVE_LEFT) {
			_level->left(dilation);
		} else if (pressed.right && hit != Movement::CANNOT_MOVE_RIGHT) {
			_level->right(dilation);
		}

		// Make sure the cursor doesn't extend too far
		_level->clamp();

		// Update score
		_score += dilation;

		const auto* lastScoreText = getScoreText(static_cast<int>(previousFrame), false);
		if (lastScoreText != getScoreText(static_cast<int>(_level->getFrame()), false)) {
			_level->increaseMultiplier();
			_game.playEffect(SoundEffect::LEVEL_UP);
		}

		return nullptr;
	}

	void Play::drawGame(SurfaceGame& surface, SurfaceGame* shadows) {
		_level->draw(surface, shadows, 0);
	}

	void Play::drawBotUI(SurfaceUI& surface) {
		const auto scale = surface.getScale();
		if (!_game.getFontSmall()) return;
		auto& small = *_game.getFontSmall();

		// Makes it so the score text doesn't freak out
		// if getWidth returns slightly different values
		// each frame
		if (_scalePrev != scale) {
			_scalePrev = scale;
			small.setScale(scale);
			_scoreWidth = small.getWidth("TIME: " + getTime(0));
		}

		const auto pad = 3 * scale;

		small.setScale(scale);

		// Draw the top left POINT/LINE thing
		// Note, 400 is kind of arbitrary. Perhaps it's needed to update this later.
		const auto* levelUp = getScoreText(static_cast<int>(_level->getFrame()), surface.getScreenDim().x <= 400);
		const Vec2f levelUpPosText = {pad, pad};
		const Vec2f levelUpBkgSize = {
			small.getWidth(levelUp) + pad * 2,
			small.getHeight() + pad * 2
		};

		// Clockwise, from top left
		const std::vector<Vec2f> levelUpBkg = {
			{0, 0},
			{levelUpBkgSize.x + levelUpBkgSize.y / 2, 0},
			{levelUpBkgSize.x, levelUpBkgSize.y},
			{0, levelUpBkgSize.y},
		};

		surface.drawPolyUI(COLOR_TRANSPARENT, levelUpBkg);
		small.draw(COLOR_WHITE, levelUpPosText, Alignment::LEFT, levelUp);

		// Draw the current score
		const auto screenWidth = surface.getScreenDim().x;
		const auto textScore = "TIME: " + getTime(_score);
		const Vec2f scorePosText = {screenWidth - pad - _scoreWidth, pad};
		Vec2f scoreBkgSize = {_scoreWidth + pad * 2, small.getHeight() + pad * 2};

		// Before we draw the score, compute the best time graph.
		auto drawBar = false;
		auto drawHigh = false;
		const auto originalY = scoreBkgSize.y;
		const auto heightBar = 2 * scale;
		const auto highScore = static_cast<float>(_selected.getHighScore());

		// Adjust background size to accommodate for new record or bar
		const auto* const recordText = "NEW RECORD!";
		if (highScore > 0) {
			if (_score < highScore) {
				scoreBkgSize.y += heightBar + pad;
				drawBar = true;
			} else {
				// Makes sure that if the "New Record" text is longer than the
				// score then we increase its width
				const auto recordTextWidth = small.getWidth(recordText);
				scoreBkgSize.y += small.getHeight() + pad;
				scoreBkgSize.x += recordTextWidth > _scoreWidth ? recordTextWidth - _scoreWidth : 0;
				drawHigh = true;
			}
		}

		// Clockwise, from top left
		const std::vector<Vec2f> scoreBkg = {
			{screenWidth - scoreBkgSize.x - scoreBkgSize.y / 2, 0},
			{screenWidth, 0},
			{screenWidth, scoreBkgSize.y},
			{screenWidth - scoreBkgSize.x, scoreBkgSize.y}
		};

		surface.drawPolyUI(COLOR_TRANSPARENT, scoreBkg);
		small.draw(COLOR_WHITE, scorePosText, Alignment::LEFT, textScore);

		if (drawBar) {
			const Vec2f barPos = {scorePosText.x, originalY};
			const Vec2f barWidth = {_scoreWidth, heightBar};
			const Vec2f barWidthScore = {_scoreWidth * (_score / highScore), heightBar};
			surface.drawRectUI(COLOR_BLACK, barPos, barWidth);
			surface.drawRectUI(COLOR_WHITE, barPos, barWidthScore);
		}

		if (drawHigh) {
			auto textColor = COLOR_WHITE;
			const Vec2f posBest = {screenWidth - pad, originalY};
			if (_score - highScore <= PULSE_TIMES * PULSE_TIME) {
				const auto percent = getPulse(_score, PULSE_TIME, highScore);
				textColor = interpolateColor(PULSE_LOW, PULSE_HIGH, percent);
			}

			small.draw(textColor, posBest, Alignment::RIGHT, recordText);
		}
	}
}
