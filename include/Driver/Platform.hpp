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

	struct Point;
	struct Color;
	class Audio;
	class Font;

	class Platform {
	public:
		Platform() = default;
		Platform(Platform&) = delete;

		virtual bool loop() = 0;
		virtual bool canUpdate() = 0;

		virtual std::string getPath(const std::string& partial) = 0;
		virtual std::string getPathRom(const std::string& partial) = 0;
		virtual std::unique_ptr<Audio> loadAudio(const std::string& path) = 0;
		virtual std::unique_ptr<Font> loadFont(const std::string& path, int size) = 0;

		virtual void playSFX(Audio& audio) = 0;
		virtual void playBGM(Audio& audio) = 0;
		virtual void stopBGM() = 0;

		virtual Buttons getPressed() = 0;
		virtual Point getScreenDim() const = 0;

		virtual void screenBegin() = 0;
		virtual void screenSwap() = 0;
		virtual void screenFinalize() = 0;
		virtual void drawRect(const Color& color, const Point& point, const Point& size) = 0;
		virtual void drawTriangle(const Color& color, const std::array<Point, 3>& points) = 0;

		virtual std::unique_ptr<Twist> getTwister() = 0;
	};
}

#endif //SUPER_HAXAGON_PLATFORM_HPP
