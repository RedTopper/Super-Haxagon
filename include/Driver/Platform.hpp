#ifndef SUPER_HAXAGON_PLATFORM_HPP
#define SUPER_HAXAGON_PLATFORM_HPP

#include <memory>
#include <string>

#include "Core/Twist.hpp"

namespace SuperHaxagon {
	struct Buttons {
		bool select : 1;
		bool back : 1;
		bool quit : 1;
		bool left : 1;
		bool right : 1;
	};

	enum class Screen {
		TOP, BOTTOM
	};

	struct Point;
	struct Color;
	class Audio;
	class Font;

	class Platform {
	public:
		Platform() = default;
		Platform(Platform&) = delete;

		virtual bool loop() = 0;
		virtual bool hasScreen(Screen test) = 0;

		virtual std::string getPath(const std::string& partial) = 0;
		virtual std::string getPathRom(const std::string& partial) = 0;
		virtual std::unique_ptr<Audio> loadAudio(const std::string& path) = 0;
		virtual std::unique_ptr<Font> loadFont(const std::string& path) = 0;

		virtual void playSFX(Audio& audio) = 0;
		virtual void playBGM(Audio& audio) = 0;
		virtual void stopBGM() = 0;

		virtual void pollButtons() = 0;
		virtual Buttons getDown() = 0;
		virtual Buttons getPressed() = 0;

		virtual Point getScreenDim() const = 0;

		virtual void drawRect(const Color& color, const Point& point, const Point& size) const = 0;
		virtual void drawTriangle(const Color& color, const std::array<Point, 3>& points) const = 0;

		virtual std::unique_ptr<Twist> getTwister() = 0;
		void setScreen(Screen select) {screen = select;}

	protected:
		Screen screen = Screen::TOP;
	};
}

#endif //SUPER_HAXAGON_PLATFORM_HPP
