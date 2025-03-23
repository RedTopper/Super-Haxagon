#ifndef DRIVER_PLATFORM_HPP
#define DRIVER_PLATFORM_HPP

// SPDX-FileCopyrightText: 2025 AJ Walter
// SPDX-License-Identifier: GPL-3.0-or-later OR MIT

#include "Driver/Tools/Color.hpp"
#include "Driver/Tools/Vector.hpp"

#include <memory>
#include <string>
#include <vector>
#include <fstream>

namespace SuperHaxagon {
	class Random;
	class Font;
	class Music;
	class Sound;
	class Screen;

	enum class Dbg {
		INFO,
		WARN,
		FATAL
	};

	enum class ButtonName {
		SELECT,
		BACK,
		QUIT,
		LEFT,
		RIGHT
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

		std::string getButtonName(ButtonName buttonName);
		Buttons getPressed() const;

		Random getRandom();

		void shutdown();
		void message(Dbg level, const std::string& where, const std::string& message) const;
		static Supports supports();

	private:
		// Hello voyager! I came across this interesting bug while compiling for the
		// Miyoo Mini, which uses GCC 8.2.1. Thought I would share it here:
		// https://stackoverflow.com/questions/58375922/compile-error-with-gcc-when-in-class-initializing-unique-ptr-of-incomplete-type
		// Previous code was:
		// std::unique_ptr<PlatformImpl> _impl{};
		std::unique_ptr<PlatformImpl> _impl;
	};
}

#endif //DRIVER_PLATFORM_HPP
