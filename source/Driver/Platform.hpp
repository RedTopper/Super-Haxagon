#ifndef SUPER_HAXAGON_PLATFORM_HPP
#define SUPER_HAXAGON_PLATFORM_HPP

#include "Core/Vector.hpp"

#include <memory>
#include <string>
#include <vector>
#include <fstream>

namespace SuperHaxagon {
	struct Color;
	class Twist;
	class Font;
	class Music;
	class Sound;
	class Screen;

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
		struct PlatformImpl;

		Platform();
		Platform(Platform&) = delete;
		~Platform();

		bool loop();
		float getDilation() const;

		std::string getPath(const std::string& partial, Location location) const;
		std::unique_ptr<std::istream> openFile(const std::string& partial, Location location) const;

		std::unique_ptr<Font> loadFont(int size) const;
		std::unique_ptr<Sound> loadSound(const std::string& base) const;
		std::unique_ptr<Music> loadMusic(const std::string& base, Location location) const;
		Screen& getScreen();

		std::vector<std::pair<Location, std::string>> loadUserLevels();

		static std::string getButtonName(const Buttons& button);
		Buttons getPressed() const;

		std::unique_ptr<Twist> getTwister();

		void shutdown();
		void message(Dbg level, const std::string& where, const std::string& message) const;
		static Supports supports();

	private:
		std::unique_ptr<PlatformImpl> _impl{};
	};
}

#endif //SUPER_HAXAGON_PLATFORM_HPP
