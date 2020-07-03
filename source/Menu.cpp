#include "Game.h"
#include "Structs.h"
#include "Level.h"
#include "Menu.h"

namespace SuperHaxagon {
	Menu::Menu(Platform& driver, Game& game, Audio& bgmMenu, Audio& sfxHexagon, Audio& sfxSelect, bool showLoadLevels) :
		platform(driver),
		game(game),
		bgmMenu(bgmMenu),
		sfxHexagon(sfxHexagon),
		sfxSelect(sfxSelect),
		showLoadLevels(showLoadLevels)
	{}

	void Menu::enter() {
		platform.stopBGM();
		platform.playBGM(bgmMenu);
		platform.playSFX(sfxHexagon);
	}

	void Menu::exit() {
		platform.stopBGM();
	}

	std::unique_ptr<State> Menu::update() {
		platform.pollButtons();
		Buttons press = platform.getPressed();

		if (press.quit) return; //TODO;

		if(!transitionDirection) {
			if (press.back && showLoadLevels) return; //TODO
			if (press.select) return; //TODO
			if (press.right) {
				transitionDirection = 1;
				lastLevel = level;
				level++;
				platform.playSFX(sfxSelect);
			}
			if (press.left) {
				transitionDirection = -1;
				lastLevel = level;
				level--;
				platform.playSFX(sfxSelect);
			}
		}

		if(level >=  game.getLevels().size()) level = 0;
		if(level < 0) level = game.getLevels().size()) - 1;
		if(transitionDirection) transitionFrame++;
		if(transitionFrame >= FRAMES_PER_TRANSITION) {
			transitionFrame = 0;
			transitionDirection = 0;
		}

		return nullptr;
	}

	void Menu::draw() {
		platform.setScreen(Screen::TOP);

		double percentRotated = (double)(transitionFrame) / (double)FRAMES_PER_TRANSITION;
		double rotation = percentRotated * Game::TAU/6.0;
		if(transitionDirection == -1) { //if the user is going to the left, flip the radians so the animation plays backwards.
			rotation *= -1.0;
		}

		//Colors
		Color FG{};
		Color BG1{};
		Color BG2{};
		Color BG3{};

		auto& levelPrev = game.getLevels()[lastLevel];
		auto& levelCur = game.getLevels()[level];
		if(transitionDirection) {
			FG = Game::interpolateColor(levelPrev->getColorsFG()[0], levelCur->getColorsFG()[0], percentRotated);
			BG1 = Game::interpolateColor(levelPrev->getColorsBG1()[0], levelCur->getColorsBG2()[0], percentRotated);
			BG2 = Game::interpolateColor(levelPrev->getColorsBG2()[0], levelCur->getColorsBG1()[0], percentRotated);
			BG3 = Game::interpolateColor(levelPrev->getColorsBG2()[0], levelCur->getColorsBG2()[0], percentRotated); //Real BG2 transition
		} else {
			FG = levelCur->getColorsFG()[0];
			BG1 = levelCur->getColorsBG1()[0];
			BG2 = levelCur->getColorsBG2()[0];
			BG3 = levelCur->getColorsBG2()[0]; //same as BG2
		}

		Point screen = platform.getScreenDim();
		Point shadow = platform.getShadowOffset();

		Point focus = {screen.x/2, screen.y/4};
		Point offsetFocus = {focus.x + shadow.x, focus.y + shadow.y};

		//home screen always has 6 sides.
		Game::drawBackground(platform, BG1, BG2, focus, platform.getRenderDistance(), rotation, 6.0);

		//shadows
		drawRegular(Game::COLOR_SHADOW, offsetFocus, DEF_HEX_FULL_LEN, rotation, 6.0);
		drawHumanCursor(Game::COLOR_SHADOW, offsetFocus, TAU/4.0, 0);

		//geometry
		drawRegular(FG, focus, DEF_HEX_FULL_LEN, rotation, 6.0);
		drawRegular(BG3, focus, DEF_HEX_FULL_LEN - DEF_HEX_BORDER_LEN, rotation, 6.0);
		drawHumanCursor(FG, focus, TAU/4.0, 0); //Draw cursor fixed quarter circle, no movement.

		//top rectangle and triangle
		int TRIANGLE_WIDTH = 70;
		int GAP_FROM_RIGHT_SIDE = 20;

		//text positions
		Point posTitle = {4, 2};
		Point posDifficulty = {4, posTitle.y + 32 + 1};
		Point posMode = {4, posDifficulty.y + 16 + 1};
		Point posCreator = {4, posMode.y + 16 + 1};
		Point posTime = {4, SCREEN_HEIGHT - 18};

		Point infoPos = {0, 0};
		Point infoSize = {TOP_WIDTH - TRIANGLE_WIDTH - GAP_FROM_RIGHT_SIDE, posCreator.y + 16 + 3};
		Point infoTriangle[3] = {
				{infoSize.x, SCREEN_HEIGHT - 1 - infoSize.y},
				{infoSize.x, SCREEN_HEIGHT - 1},
				{infoSize.x + TRIANGLE_WIDTH, SCREEN_HEIGHT - 1}};
		drawRect(TRANSP, infoPos, infoSize);
		drawTriangle(TRANSP, infoTriangle);

		//score block with triangle
		Point timePos = {0, posTime.y - 3};
		Point timeSize = {10/*chars?*/ * 16 + 4, 16 + 7};
		Point timeTriangle[3] = {
				{timeSize.x, timeSize.y - 3},
				{timeSize.x, - 1}, //why does this have to be -1?
				{timeSize.x + 18, -1}}; //I mean, it works...
		drawRect(TRANSP, timePos, timeSize);
		drawTriangle(TRANSP, timeTriangle);

		//actual text
		char* scoreTime  = getBestTime(level.highScore);
		writeFont(WHITE, posTitle, level.name.str, FONT32, ALIGN_LEFT_C);
		writeFont(GREY, posDifficulty, level.difficulty.str, FONT16, ALIGN_LEFT_C);
		writeFont(GREY, posMode, level.mode.str, FONT16, ALIGN_LEFT_C);
		writeFont(GREY, posCreator, level.creator.str, FONT16, ALIGN_LEFT_C);
		writeFont(WHITE, posTime, scoreTime, FONT16, ALIGN_LEFT_C);
		free(scoreTime);



		Point posButton = {4, 4};
		Point posLocation = {210, 4};
		Point posLevels = {4, posLocation.y + 16 + 1};
		Point posDownload = {4, posLevels.y + 32 + 1};
		Point posDownload2 = {4, posDownload.y + 16 + 1};
		Point posDownload3 = {4, posDownload2.y + 16 + 1};

		Point posDownloadHigh = {4, posButton.y};
		Point posDownloadHigh2 = {4, posDownloadHigh.y + 16 + 1};
		Point posDownloadHigh3 = {4, posDownloadHigh2.y + 16 + 1};

		drawBlackBot();
		if(showLoadLevels){
			writeFont(WHITE, posButton, "PRESS B TO LOAD", FONT16, ALIGN_LEFT_C);
			if(loaded == ROMFS) writeFont(WHITE, posLocation, "SDMC", FONT16, ALIGN_LEFT_C);
			if(loaded == SDMC) writeFont(WHITE, posLocation, "ROMFS", FONT16, ALIGN_LEFT_C);
			writeFont(WHITE, posLevels, "LEVELS", FONT16, ALIGN_LEFT_C);
		}
		if(showGetBGM) {
			if(showLoadLevels) {
				writeFont(WHITE, posDownload, "GET BGM FROM GITHUB: ", FONT16, ALIGN_LEFT_C);
				writeFont(WHITE, posDownload2, "REDINQUISITIVE/", FONT16, ALIGN_LEFT_C);
				writeFont(WHITE, posDownload3, "SUPER-HAXAGON!", FONT16, ALIGN_LEFT_C);
			} else {
				writeFont(WHITE, posDownloadHigh, "GET BGM FROM GITHUB: ", FONT16, ALIGN_LEFT_C);
				writeFont(WHITE, posDownloadHigh2, "REDINQUISITIVE/", FONT16, ALIGN_LEFT_C);
				writeFont(WHITE, posDownloadHigh3, "SUPER-HAXAGON!", FONT16, ALIGN_LEFT_C);
			}
		}

		drawFramerate(fps);
	}
}