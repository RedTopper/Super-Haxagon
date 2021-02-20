#ifndef SUPER_HAXAGON_PLATFORM_HPP
#define SUPER_HAXAGON_PLATFORM_HPP

#include "Audio.hpp"
#include "Player.hpp"

#include <memory>
#include <string>
#include <vector>

namespace SuperHaxagon {
	struct Point;
	struct Color;
	class Twist;
	class Font;

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

	enum class Supports {
		NOTHING = 0,
		SHADOWS = 1,
		FILESYSTEM = 1 << 1
	};

	inline Supports operator |(Supports lhs, Supports rhs) {
		using T = std::underlying_type_t<Supports>;
		return static_cast<Supports>(static_cast<T>(lhs) | static_cast<T>(rhs));
	}

	inline Supports operator &(Supports lhs, Supports rhs) {
		using T = std::underlying_type_t<Supports>;
		return static_cast<Supports>(static_cast<T>(lhs) & static_cast<T>(rhs));
	}


	class Platform {
	public:
		explicit Platform(const Dbg dbg) : _dbg(dbg) {}
		Platform(Platform&) = delete;
		virtual ~Platform() = default;

		virtual bool loop() = 0;
		virtual float getDilation() = 0;

		virtual std::string getPath(const std::string& partial) = 0;
		virtual std::string getPathRom(const std::string& partial) = 0;
		virtual std::unique_ptr<Audio> loadAudio(const std::string& path, Stream stream) = 0;
		virtual std::unique_ptr<Font> loadFont(const std::string& path, int size) = 0;

		virtual void playSFX(Audio& audio) = 0;
		virtual void playBGM(Audio& audio) = 0;
		virtual void stopBGM() {_bgm = nullptr;}
		virtual Player* getBGM() {return _bgm.get();}

		virtual std::string getButtonName(const Buttons& button) = 0;
		virtual Buttons getPressed() = 0;
		virtual Point getScreenDim() const = 0;

		virtual void screenBegin() = 0;
		virtual void screenSwap() = 0;
		virtual void screenFinalize() = 0;
		virtual void drawPoly(const Color& color, const std::vector<Point>& points) = 0;

		virtual std::unique_ptr<Twist> getTwister() = 0;

		virtual void shutdown() = 0;
		virtual void message(Dbg level, const std::string& where, const std::string& message) = 0;
		virtual Supports supports() = 0;

	protected:
		Dbg _dbg;
		std::unique_ptr<Player> _bgm{};
	};
}

#endif //SUPER_HAXAGON_PLATFORM_HPP
