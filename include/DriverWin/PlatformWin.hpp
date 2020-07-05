#ifndef SUPER_HAXAGON_PLATFORM_WIN_HPP
#define SUPER_HAXAGON_PLATFORM_WIN_HPP

#include <SDL2/SDL.h>

#include "Driver/Platform.hpp"

namespace SuperHaxagon {

	class PlatformWin : public Platform {
	public:
		PlatformWin();
		~PlatformWin();

		bool loop() override;
		bool hasScreen(Screen test) override;

		std::string getPath(const std::string& partial) override;
		std::string getPathRom(const std::string& partial) override;
		std::unique_ptr<Audio> loadAudio(const std::string& path) override;

		std::unique_ptr<Font> loadFont(const std::string& path) override;

		void playSFX(Audio& audio) override;
		void playBGM(Audio& audio) override;
		void stopBGM() override;

		void pollButtons() override;
		Buttons getDown() override;
		Buttons getPressed() override;

		Point getScreenDim() const override;

		void drawRect(const Color& color, const Point& point, const Point& size) const override;
		void drawTriangle(const Color& color, const std::array<Point, 3>& points) const override;

		std::unique_ptr<Twist> getTwister() override;

	private:
		SDL_Window* window;
		SDL_Renderer* renderer;
		SDL_Event event{};

		bool loaded = false;
	};
}

#endif //SUPER_HAXAGON_PLATFORM_WIN_HPP
