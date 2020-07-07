#include "Core/Game.hpp"
#include "Driver/Font.hpp"
#include "Driver/Audio.hpp"
#include "Factories/Level.hpp"
#include "States/Play.hpp"
#include "States/Quit.hpp"
#include "States/Over.hpp"

namespace SuperHaxagon {

	Play::Play(Game& game, LevelFactory& factory) :
		game(game),
		platform(game.getPlatform()),
		factory(factory),
		level(factory.instantiate(game.getTwister(), SCALE_BASE_DISTANCE))
	{}

	Play::~Play() = default;

	void Play::enter() {
		std::string path;
		if (factory.getLocation() == Location::INTERNAL) {
			path = platform.getPathRom("/bgm" + factory.getMusic());
		} else if (factory.getLocation() == Location::EXTERNAL) {
			path = platform.getPath("/bgm" + factory.getMusic());
		}

		bgm = platform.loadAudio(path, Stream::INDIRECT);
		platform.playSFX(game.getSfxBegin());
		platform.playBGM(*bgm);
	}

	void Play::exit() {
		platform.stopBGM();
	}

	std::unique_ptr<State> Play::update(double dilation) {
		double maxRenderDistance = SCALE_BASE_DISTANCE * (game.getScreenDimMax() / 240);
		level->update(game.getTwister(), SCALE_HEX_LENGTH, maxRenderDistance, dilation);

		// Button presses
		auto pressed = platform.getPressed();

		// Check collision
		double cursorDistance = SCALE_HEX_LENGTH + SCALE_HUMAN_PADDING + SCALE_HUMAN_HEIGHT;
		auto hit = level->collision(cursorDistance, dilation);
		if(pressed.back || hit == Movement::DEAD) {
			return std::make_unique<Over>(game, factory, std::move(level), score);
		}

		if (pressed.quit) {
			return std::make_unique<Quit>();
		}

		if (pressed.left && hit != Movement::CANNOT_MOVE_LEFT) {
			level->left(dilation);
		} else if (pressed.right && hit != Movement::CANNOT_MOVE_RIGHT) {
			level->right(dilation);
		}

		// Make sure the cursor doesn't extend too far
		level->clamp();

		// Update score
		auto lastScoreText = getScoreText((int)score);
		score += dilation;

		if(lastScoreText != getScoreText((int)score)) {
			level->increaseMultiplier();
			platform.playSFX(game.getSfxLevelUp());
		}

		return nullptr;
	}

	void Play::drawTop(double scale) {
		level->draw(game, scale, 0);
	}

	void Play::drawBot(double scale) {
		auto& small = game.getFontSmall();

		// Makes it so the score text doesn't freak out
		// if getWidth returns slightly different values
		// each frame
		if (scalePrev != scale) {
			scalePrev = scale;
			small.setScale(scale);
			scoreWidth = small.getWidth("TIME: " + getTime(0));
		}

		double width = platform.getScreenDim().x;
		double PAD = 3 * scale;

		small.setScale(scale);

		// Draw the top left POINT/LINE thing
		auto levelUp = getScoreText((int)score);
		Point levelUpPosText = {PAD, PAD};
		Point levelUpBkgPos = {0, 0};
		Point LevelUpBkgSize = {
			levelUpPosText.x + small.getWidth(levelUp) + PAD,
			levelUpPosText.y + small.getHeight() + PAD
		};

		std::array<Point, 3> levelUpBkgTri = {
			Point{LevelUpBkgSize.x, platform.getScreenDim().y - LevelUpBkgSize.y},
			Point{LevelUpBkgSize.x, platform.getScreenDim().y},
			Point{LevelUpBkgSize.x + LevelUpBkgSize.y/2, platform.getScreenDim().y}
		};

		platform.drawRect(COLOR_TRANSPARENT, levelUpBkgPos, LevelUpBkgSize);
		platform.drawTriangle(COLOR_TRANSPARENT, levelUpBkgTri);
		small.draw(COLOR_WHITE, levelUpPosText, Alignment::LEFT, levelUp);

		// Draw the current score
		auto textScore = "TIME: " + getTime((int)score);
		Point scorePosText = { width - PAD - scoreWidth, PAD};
		Point scoreBkgPos = {scorePosText.x - PAD, 0};
		Point scoreBkgSize = {
			width - scoreBkgPos.x,
			scorePosText.y + small.getHeight() + PAD
		};

		// Before we draw the score, compute the best time graph.
		bool drawBar = false;
		bool drawHigh = false;
		double originalY = scoreBkgSize.y;
		double HEIGHT_BAR = 2 * scale;
		if (factory.getHighScore() > 0) {
			if (score < factory.getHighScore()) {
				scoreBkgSize.y += HEIGHT_BAR + PAD;
				drawBar = true;
			} else {
				scoreBkgSize.y += small.getHeight() + PAD;
				drawHigh = true;
			}
		}

		std::array<Point, 3> scoreBkgTri = {
			Point{scoreBkgPos.x, platform.getScreenDim().y - scoreBkgSize.y},
			Point{scoreBkgPos.x, platform.getScreenDim().y},
			Point{scoreBkgPos.x - scoreBkgSize.y/2, platform.getScreenDim().y}
		};

		platform.drawTriangle(COLOR_TRANSPARENT, scoreBkgTri);
		platform.drawRect(COLOR_TRANSPARENT, scoreBkgPos, scoreBkgSize);
		small.draw(COLOR_WHITE, scorePosText, Alignment::LEFT, textScore);

		if (drawBar) {
			Point barPos = {scorePosText.x, originalY};
			Point barWidth = {scoreWidth, HEIGHT_BAR};
			Point barWidthScore = {scoreWidth * (score / factory.getHighScore()), HEIGHT_BAR};
			platform.drawRect(COLOR_BLACK, barPos, barWidth);
			platform.drawRect(COLOR_WHITE, barPos, barWidthScore);
		}

		if (drawHigh) {
			Color textColor = COLOR_WHITE;
			Point posBest = {width - PAD, originalY};
			if (score - factory.getHighScore() <= PULSE_TIMES * PULSE_TIME) {
				double percent = getPulse(score, PULSE_TIME, factory.getHighScore());
				textColor = interpolateColor(PULSE_LOW, PULSE_HIGH, percent);
			}

			small.draw(textColor, posBest, Alignment::RIGHT,  "NEW RECORD!");
		}
	}
}
