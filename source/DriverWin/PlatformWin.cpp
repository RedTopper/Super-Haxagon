#include <string>
#include <algorithm>
#include <SDL2/SDL.h>

#include "DriverWin/AudioWin.hpp"
#include "DriverWin/FontWin.hpp"
#include "DriverWin/PlatformWin.hpp"

namespace SuperHaxagon {
	PlatformWin::PlatformWin() {
		window = SDL_CreateWindow("Super Haxagon", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_OPENGL);

		if (window == nullptr) {
			SDL_Log("Could not create a window: %s", SDL_GetError());
			return;
		}

		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
		if (renderer == nullptr) {
			SDL_DestroyWindow(window);
			SDL_Log("Could not create a renderer: %s", SDL_GetError());
			return;
		}

		loaded = true;
	}

	PlatformWin::~PlatformWin() {
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
	}

	bool PlatformWin::loop() {
		if (SDL_PollEvent(&event) && event.type == SDL_QUIT) {
			loaded = false;
		}

		return loaded;
	}

	bool PlatformWin::hasScreen(Screen test) {
		return test == Screen::TOP;
	}

	std::string PlatformWin::getPath(const std::string& partial) {
		auto path = std::string("./data/custom") + partial;
		std::replace(path.begin(), path.end(), '/', '\\');
		return path;
	}

	std::string PlatformWin::getPathRom(const std::string& partial) {
		auto path = std::string("./data/rom") + partial;
		std::replace(path.begin(), path.end(), '/', '\\');
		return path;
	}

	std::unique_ptr<Audio> PlatformWin::loadAudio(const std::string& path) {
		return std::make_unique<AudioWin>(path);
	}

	std::unique_ptr<Font> PlatformWin::loadFont(const std::string& path) {
		return std::make_unique<FontWin>(path);
	}

	void PlatformWin::playSFX(Audio& audio) {

	}

	void PlatformWin::playBGM(Audio& audio) {

	}

	void PlatformWin::stopBGM() {

	}

	void PlatformWin::pollButtons() {

	}

	Buttons PlatformWin::getDown() {
		return {};
	}

	Buttons PlatformWin::getPressed() {
		return {};
	}

	Point PlatformWin::getScreenDim() const {
		return {};
	}

	void PlatformWin::drawRect(const Color& color, const Point& point, const Point& size) const {

	}

	void PlatformWin::drawTriangle(const Color& color, const std::array<Point, 3>& points) const {

	}

	std::unique_ptr<Twist> PlatformWin::getTwister() {
		std::random_device source;
		std::mt19937::result_type data[std::mt19937::state_size];
		generate(std::begin(data), std::end(data), ref(source));
		return std::make_unique<Twist>(
				std::make_unique<std::seed_seq>(std::begin(data), std::end(data))
		);
	}
}
