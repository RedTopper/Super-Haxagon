#include <memory>
#include <sstream>
#include <iomanip>

#include "Driver/Platform.hpp"

#include "Game.hpp"
#include "Menu.hpp"

namespace SuperHaxagon {
	Game::Game(Platform& platform) : platform(platform) {
		//audio loading
		sfxBegin = platform.loadAudio(platform.getPathRom("/sound/begin.wav"));
		sfxHexagon = platform.loadAudio(platform.getPathRom("/sound/hexagon.wav"));
		sfxOver = platform.loadAudio(platform.getPathRom("/sound/over.wav"));
		sfxSelect = platform.loadAudio(platform.getPathRom("/sound/select.wav"));
		sfxLevelUp = platform.loadAudio(platform.getPathRom("/sound/level.wav"));
		bgmMenu = platform.loadAudio(platform.getPathRom("/bgm/pamgaea.wav"));
		twister = platform.getTwister();
	}

	int Game::run() {

		//pattern loading
		LoadedState loaded = NOT_LOADED;
		GlobalData data = EMPTY_GLOBAL_DATA;
		FILE *fileData;
		const char *scorePath;
		data.loaded = 0;

		int showGetBGM = 1; //Used to hide the get BGM info after a button press.
		int showLoadLevels = 0; //Used to show option to load levels if external levels exist.
		if (access(NAME_SDMC_PROJECT, 0) != -1) showLoadLevels = 1; //Set true if accessible at startup.
		//Note: will still panic if file is ever removed during runtime when it needs it (by design?)

		state = std::make_unique<Menu>(*this, showLoadLevels);

		//level selection and game over
		int nlevel = 0;
		int nLastLevel = -1;
		Level level = EMPTY_LEVEL;
		LiveLevel gameOver = EMPTY_LIVE_LEVEL;

		//Controller
		GameState state = SWITCH_LOAD_LOCATION;
		while (state != PROGRAM_QUIT) {
			switch (state) {
				case SWITCH_LOAD_LOCATION:
					freeData(data);
					data.loaded = 0;
					switch (loaded) {
						default:
						case NOT_LOADED:
						case SDMC:;
							fileData = fopen(NAME_ROMFS_PROJECT, "rb");
							if (!fileData)
								panic("NO INTERNAL FILE!", "There was no internal file to load. "
								                           "The game was likely compiled incorrectly.", DEF_DEBUG,
								      (int) fileData);
							scorePath = NAME_ROMFS_SCORE;
							loaded = ROMFS;
							break;
						case ROMFS:;
							fileData = fopen(NAME_SDMC_PROJECT, "rb");
							if (!fileData)
								panic("NO EXTERNAL FILE TO LOAD!", "There was no external file to load. "
								                                   "You need to put external levels in the location defined in the README",
								      DEF_DEBUG, (int) fileData);
							scorePath = NAME_SDMC_SCORE;
							loaded = SDMC;
							break;
					}
					data = getData(fileData);
					getScores(scorePath, data);
					fclose(fileData);
					state = MAIN_MENU;
					nlevel = 0;
					nLastLevel = -1;
					break;
				case MAIN_MENU:
					audioStop(&bgm);
					audioPlay(&hexagon, ONCE);
					audioPlay(&mainMenu, LOOP);
					state = doMainMenu(data, loaded, rotate, &nlevel, showGetBGM, showLoadLevels);
					level = data.levels[nlevel];

					//load audio
					audioStop(&mainMenu);
					if (state == PLAYING) {
						if (nlevel != nLastLevel) {
							audioFree(&bgm);
							audioLoad(level.music.str, &bgm, channelBGM);
							nLastLevel = nlevel;
						}
						audioPlay(&bgm, LOOP);
					}

					//never show again.
					showGetBGM = 0;
					break;
				case PLAYING:
					audioPlay(&begin, ONCE);
					state = doPlayGame(level, &gameOver, levelUp);
					break;
				case GAME_OVER:
					audioPlay(&over, ONCE);
					state = doGameOver(level, gameOver);

					//replace and save high score if needed.
					if (gameOver.score > level.highScore) {
						level.highScore = gameOver.score;
						data.levels[nlevel].highScore = gameOver.score;
						putScores(scorePath, data);
					}
					break;
				case PROGRAM_QUIT:;
			}
		}

		//level free
		freeData(data);

		//close GFX

		return 0;
	}

	void Game::drawBackground(const Color& color1, const Color& color2, const Point& focus, double height, double rotation, double sides) {
		int exactSides = std::ceil(sides);

		//solid background.
		Point position = {0,0};
		Point size = platform.getScreenDim();
		platform.drawRect(color1, position, size);

		//This draws the main background.
		std::vector<Point> edges;
		edges.reserve(exactSides);

		for(int i = 0; i < exactSides; i++) {
			edges[i].x = lround(height * cos(rotation + (double)i * TAU/sides) + (double)(focus.x));
			edges[i].y = lround(height * sin(rotation + (double)i * TAU/sides) + (double)(focus.y));
		}

		std::array<Point, 3> triangle{};
		triangle[0] = focus;

		//if the sides is odd we need to "make up a color" to put in the gap between the last and first color
		if(exactSides % 2) {
			triangle[1] = edges[exactSides - 1];
			triangle[2] = edges[0];
			platform.drawTriangle(interpolateColor(color1, color2, 0.5f), triangle);
		}

		//Draw the rest of the triangles
		for(int i = 0; i < exactSides - 1; i = i + 2) {
			triangle[1] = edges[i];
			triangle[2] = edges[i + 1];
			platform.drawTriangle(color2, triangle);
		}
	}

	void Game::drawRegular(const Color& color, const Point& focus, int height, double rotation, double sides) {
		int exactSides = std::ceil(sides);

		std::vector<Point> edges;
		edges.reserve(exactSides);

		//calculate the triangle backwards so it overlaps correctly.
		for(int i = 0; i < exactSides; i++) {
			edges[i].x = lround(height * cos(rotation + (double)i * TAU/sides) + (double)(focus.x));
			edges[i].y = lround(height * sin(rotation + (double)i * TAU/sides) + (double)(focus.y));
		}

		std::array<Point, 3> triangle{};
		triangle[0] = focus;

		//connect last triangle edge to first
		triangle[1] = edges[exactSides - 1];
		triangle[2] = edges[0];
		platform.drawTriangle(color, triangle);

		//draw rest of regular polygon
		for(int i = 0; i < exactSides - 1; i++) {
			triangle[1] = edges[i];
			triangle[2] = edges[i + 1];
			platform.drawTriangle(color, triangle);
		}
	}

	void Game::drawHumanCursor(const Color& color, const Point& focus, double cursor, double rotation) {
		std::array<Point, 3> triangle{};
		triangle[0] = calcPoint(focus, cursor + rotation, 0.0, getHexLength() + getHumanPadding() + getHumanHeight());
		triangle[1] = calcPoint(focus, cursor + rotation, getHumanWidth()/2, getHexLength()  + getHumanPadding());
		triangle[2] = calcPoint(focus, cursor + rotation, -getHumanWidth()/2, getHexLength()  + getHumanPadding());
		platform.drawTriangle(color, triangle);
	}

	Color Game::interpolateColor(const Color& one, const Color& two, double percent) {
		Color result{};
		result.r = (int)linear((double)one.r, (double)two.r, percent);
		result.g = (int)linear((double)one.g, (double)two.g, percent);
		result.b = (int)linear((double)one.b, (double)two.b, percent);
		result.a = (int)linear((double)one.a, (double)two.a, percent);
		return result;
	}

	double Game::linear(double start, double end, double percent) {
		return (end - start) * percent + start;
	}

	Point Game::calcPoint(const Point& focus, double rotation, double offset, double distance)  {
		Point point = {0,0};
		point.x = lround(distance * cos(rotation + offset) + focus.x);
		point.y = lround(distance * sin(rotation + offset) + focus.y);
		return point;
	}

	std::string Game::getBestTime(int score) {
		std::stringstream buffer;
		int scoreInt = (int)((double)score/60.0);
		int decimalPart = (int)(((double)score/60.0 - (double)scoreInt) * 100.0);
		buffer << std::fixed << std::setprecision(3) << scoreInt << std::setprecision(2) << decimalPart;
		return buffer.str();
	}

	const char* Game::getScoreText(int score) {
		if(score < 10 * 60) return "SPACE";
		if(score < 20 * 60) return "POINT";
		if(score < 30 * 60) return "LINE";
		if(score < 40 * 60) return "TRIANGLE";
		if(score < 50 * 60) return "SQUARE";
		if(score < 60 * 60) return "PENTAGON";
		return "HEXAGON";
	}
}

