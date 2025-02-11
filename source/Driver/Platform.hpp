#ifndef SUPER_HAXAGON_PLATFORM_HPP
#define SUPER_HAXAGON_PLATFORM_HPP

#include "Font.hpp"
#include "Music.hpp"

#include <memory>
#include <string>
#include <vector>
#include <fstream>

namespace SuperHaxagon {
	struct Point;
	struct Color;
	class Twist;

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

	enum class SoundEffect {
		BEGIN,
		HEXAGON,
		OVER,
		SELECT,
		LEVEL_UP,
		WONDERFUL,
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
		struct PlatformData;

		Platform();
		Platform(Platform&) = delete;
		~Platform();

		bool loop();
		float getDilation() const;

		std::string getPath(const std::string& partial, Location location) const;
		std::unique_ptr<std::istream> openFile(const std::string& partial, Location location) const;

		void loadSFX(SoundEffect effect, const std::string& name) const;
		void loadFont(int size);

		std::vector<std::pair<Location, std::string>> loadUserLevels();

		void playSFX(SoundEffect effect) const;
		void playBGM(const std::string& base, Location location);
		void stopBGM() {_bgm = nullptr;}

		Music* getBGM() const {return _bgm.get();}
		Font& getFont(const int size) {
			for (const auto& font : _fonts) {
				if (font.first == size) {
					return *font.second;
				}
			}

			// Fallback to first loaded font
			return *_fonts.front().second;
		};

		static std::string getButtonName(const Buttons& button);
		Buttons getPressed() const;
		Point getScreenDim() const;

		void screenBegin() const;
		void screenSwap();
		void screenFinalize() const;
		void drawPoly(const Color& color, const std::vector<Point>& points) const;

		std::unique_ptr<Twist> getTwister();

		void shutdown();
		void message(Dbg level, const std::string& where, const std::string& message) const;
		static Supports supports();

	private:
		std::unique_ptr<PlatformData> _plat{};

		float _delta = 0.0f;

		std::vector<std::pair<int, std::unique_ptr<Font>>> _fonts{};
		std::unique_ptr<Music> _bgm{};
	};
}

#endif //SUPER_HAXAGON_PLATFORM_HPP
