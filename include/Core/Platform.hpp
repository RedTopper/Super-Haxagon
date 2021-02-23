#ifndef SUPER_HAXAGON_PLATFORM_HPP
#define SUPER_HAXAGON_PLATFORM_HPP

#include "AudioLoader.hpp"
#include "AudioPlayer.hpp"

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

	enum class Location {
		ROM,
		USER
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

		virtual std::string getPath(const std::string& partial, Location location) = 0;
		virtual std::unique_ptr<std::istream> openFile(const std::string& partial, Location location);
		virtual std::unique_ptr<AudioLoader> loadAudio(const std::string& partial, Stream stream, Location location) = 0;
		virtual std::unique_ptr<Font> loadFont(const std::string& partial, int size) = 0;

		virtual void playSFX(AudioLoader& audio) = 0;
		virtual void playBGM(AudioLoader& audio) = 0;
		virtual void stopBGM();
		virtual AudioPlayer* getBGM();

		virtual std::string getButtonName(const Buttons& button) = 0;
		virtual Buttons getPressed() = 0;
		virtual Point getScreenDim() const = 0;

		virtual void screenBegin() = 0;
		virtual void screenSwap();
		virtual void screenFinalize() = 0;
		virtual void drawPoly(const Color& color, const std::vector<Point>& points) = 0;

		virtual std::unique_ptr<Twist> getTwister() = 0;

		virtual void shutdown() = 0;
		virtual void message(Dbg level, const std::string& where, const std::string& message) = 0;
		virtual Supports supports();

	protected:
		Dbg _dbg;
		std::unique_ptr<AudioPlayer> _bgm{};
	};
}

#endif //SUPER_HAXAGON_PLATFORM_HPP
