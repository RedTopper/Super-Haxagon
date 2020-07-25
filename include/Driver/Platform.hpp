#ifndef SUPER_HAXAGON_PLATFORM_HPP
#define SUPER_HAXAGON_PLATFORM_HPP

#include <memory>
#include <string>

#include "Driver/Audio.hpp"
#include "Driver/Player.hpp"
#include "Driver/Font.hpp"
#include "Core/Twist.hpp"

namespace SuperHaxagon {
	enum class Dbg {
		INFO,
		WARN,
		FATAL
	};

	struct Buttons {
		bool select : 1;
		bool back : 1;
		bool quit : 1;
		bool left : 1;
		bool right : 1;
	};

	struct Point;
	struct Color;

	class Platform {
	public:
		explicit Platform(Dbg dbg) : _dbg(dbg) {}
		Platform(Platform&) = delete;
		virtual ~Platform() = default;

		virtual bool loop() = 0;
		virtual double getDilation() = 0;

		virtual std::string getPath(const std::string& partial) = 0;
		virtual std::string getPathRom(const std::string& partial) = 0;
		virtual std::unique_ptr<Audio> loadAudio(const std::string& path, Stream stream) = 0;
		virtual std::unique_ptr<Font> loadFont(const std::string& path, int size) = 0;

		virtual void playSFX(Audio& audio) = 0;
		virtual void playBGM(Audio& audio) = 0;
		virtual double getBGMTime() = 0;

		virtual std::string getButtonName(const Buttons& button) = 0;
		virtual Buttons getPressed() = 0;
		virtual Point getScreenDim() const = 0;

		virtual void screenBegin() = 0;
		virtual void screenSwap() = 0;
		virtual void screenFinalize() = 0;
		virtual void drawRect(const Color& color, const Point& point, const Point& size) = 0;
		virtual void drawTriangle(const Color& color, const std::array<Point, 3>& points) = 0;

		virtual std::unique_ptr<Twist> getTwister() = 0;

		virtual void shutdown() = 0;
		virtual void message(Dbg level, const std::string& where, const std::string& message) = 0;

		void stopBGM() {
			_bgm = nullptr;
		}

		void pauseBGM() {
			if (!_bgm) return;
			_bgm->pause();
		}

		void resumeBGM() {
			if (!_bgm) return;
			_bgm->play();
		}

	protected:
		Dbg _dbg;
		std::unique_ptr<Player> _bgm = nullptr;
	};
}

#endif //SUPER_HAXAGON_PLATFORM_HPP
