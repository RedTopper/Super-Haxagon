#include <Driver/Audio.hpp>
#include "Core/Game.hpp"
#include "Driver/Font.hpp"
#include "Factories/Level.hpp"
#include "States/Quit.hpp"
#include "States/Play.hpp"
#include "States/Menu.hpp"

namespace SuperHaxagon {
	Menu::Menu(Game& game) :
		game(game),
		platform(game.getPlatform())
	{}

	Menu::~Menu() = default;

	void Menu::enter() {
		bgm = platform.loadAudio(platform.getPathRom("/bgm/pamgaea"), SuperHaxagon::Stream::INDIRECT);
		platform.playSFX(game.getSfxHexagon());
		platform.playBGM(*bgm);
	}

	void Menu::exit() {
		platform.stopBGM();
	}

	std::unique_ptr<State> Menu::update(double dilation) {
		Buttons press = platform.getPressed();

		if (press.quit) return std::make_unique<Quit>();

		if(!transitionDirection) {
			if (press.select) return std::make_unique<Play>(game, *game.getLevels()[level]);
			if (press.right) {
				transitionDirection = 1;
				lastLevel = level;
				level++;
				platform.playSFX(game.getSfxSelect());
			}
			if (press.left) {
				transitionDirection = -1;
				lastLevel = level;
				level--;
				platform.playSFX(game.getSfxSelect());
			}
		}

		if(level >=  (int)game.getLevels().size()) level = 0;
		if(level < 0) level = (int)game.getLevels().size() - 1;
		if(transitionDirection) transitionFrame += dilation;
		if(transitionFrame >= FRAMES_PER_TRANSITION) {
			transitionFrame = 0;
			transitionDirection = 0;
		}

		return nullptr;
	}

	void Menu::drawTop(double scale) {
		double percentRotated = (double)(transitionFrame) / (double)FRAMES_PER_TRANSITION;
		double rotation = percentRotated * TAU/6.0;

		// If the user is going to the left, flip the radians so the animation plays backwards.
		if(transitionDirection == -1) {
			rotation *= -1.0;
		}

		// Colors
		Color FG{};
		Color BG1{};
		Color BG2{};
		Color BG3{};

		auto& levelPrev = *game.getLevels()[lastLevel];
		auto& levelCur = *game.getLevels()[level];

		if(transitionDirection) {
			FG = interpolateColor(levelPrev.getColorsFG()[0], levelCur.getColorsFG()[0], percentRotated);
			BG1 = interpolateColor(levelPrev.getColorsBG1()[0], levelCur.getColorsBG2()[0], percentRotated);
			BG2 = interpolateColor(levelPrev.getColorsBG2()[0], levelCur.getColorsBG1()[0], percentRotated);
			BG3 = interpolateColor(levelPrev.getColorsBG2()[0], levelCur.getColorsBG2()[0], percentRotated); //Real BG2 transition
		} else {
			FG = levelCur.getColorsFG()[0];
			BG1 = levelCur.getColorsBG1()[0];
			BG2 = levelCur.getColorsBG2()[0];
			BG3 = levelCur.getColorsBG2()[0]; //same as BG2
		}

		Point screen = platform.getScreenDim();
		Point shadow = game.getShadowOffset();

		Point focus = {screen.x/2, screen.y/4};
		Point offsetFocus = {focus.x + shadow.x, focus.y + shadow.y};

		// Home screen always has 6 sides.
		// Use a multiplier of 2 because the view is shifted down
		game.drawBackground(BG1, BG2, focus, 2, rotation, 6.0);

		// Shadows
		game.drawRegular(COLOR_SHADOW, offsetFocus, SCALE_HEX_LENGTH * scale, rotation, 6.0);
		game.drawCursor(COLOR_SHADOW, offsetFocus, TAU / 4.0, 0, scale);

		// Geometry
		game.drawRegular(FG, focus,SCALE_HEX_LENGTH * scale, rotation, 6.0);
		game.drawRegular(BG3, focus, SCALE_HEX_LENGTH * scale - SCALE_HEX_BORDER * scale, rotation, 6.0);
		game.drawCursor(FG, focus, TAU / 4.0, 0, scale); //Draw cursor fixed quarter circle, no movement.

		auto& large = game.getFontLarge();
		auto& small = game.getFontSmall();

		// Padding for text
		double PAD = 4;

		// Actual text
		auto scoreTime = "BEST: " + getTime(levelCur.getHighScore());
		auto diff = "DIFF: " + levelCur.getDifficulty();
		auto mode = "MODE: " + levelCur.getMode();
		auto auth = "AUTH: " + levelCur.getCreator();
		auto renderCreator = levelCur.getLocation() == Location::EXTERNAL;
		large.setScale(scale);
		small.setScale(scale);

		// Text positions
		Point posTitle = {PAD, PAD};
		Point posDifficulty = {PAD, posTitle.y + large.getHeight() + PAD};
		Point posMode = {PAD, posDifficulty.y + small.getHeight() + PAD};
		Point posCreator = {PAD, posMode.y + (renderCreator ? small.getHeight() + PAD : 0)};
		Point posTime = {PAD, screen.y - small.getHeight() - PAD};

		// Triangle positions
		Point infoPos = {0, 0};
		Point infoSize = {std::max({
			large.getWidth(levelCur.getName()),
			small.getWidth(diff),
			small.getWidth(mode),
			small.getWidth(auth),
		}) + PAD, posCreator.y + small.getHeight() + PAD};

		// It looks like the original game had triangles flipped....
		std::array<Point, 3> infoTriangle = {
			Point{infoSize.x, platform.getScreenDim().y - infoSize.y},
			Point{infoSize.x, platform.getScreenDim().y},
			Point{infoSize.x + infoSize.y/2,  platform.getScreenDim().y}
		};

		platform.drawRect(COLOR_TRANSPARENT, infoPos, infoSize);
		platform.drawTriangle(COLOR_TRANSPARENT, infoTriangle);

		// Score block with triangle
		Point timePos = {0, posTime.y - PAD};
		Point timeSize = {small.getWidth(scoreTime) + PAD * 2, small.getHeight() + PAD * 2};
		std::array<Point, 3> timeTriangle = {
			Point{timeSize.x, timeSize.y},
			Point{timeSize.x, 0},
			Point{timeSize.x + timeSize.y/2, 0}
		};

		platform.drawRect(COLOR_TRANSPARENT, timePos, timeSize);
		platform.drawTriangle(COLOR_TRANSPARENT, timeTriangle);

		large.draw(COLOR_WHITE, posTitle, Alignment::LEFT, levelCur.getName());
		small.draw(COLOR_GREY, posDifficulty, Alignment::LEFT, diff);
		small.draw(COLOR_GREY, posMode, Alignment::LEFT, mode);

		if (renderCreator)
			small.draw(COLOR_GREY, posCreator, Alignment::LEFT, auth);

		small.draw(COLOR_WHITE, posTime, Alignment::LEFT, scoreTime);
	}

	void Menu::drawBot(double) {}
}
