#ifndef SUPER_HAXAGON_PLATFORM_HPP
#define SUPER_HAXAGON_PLATFORM_HPP
#define _3DS

#include <memory>
#include <string>

#include "Twist.hpp"

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
		virtual bool hasScreen(Screen test) = 0;

		virtual std::string getPath(const std::string& partial) = 0;
		virtual std::string getPathRom(const std::string& partial) = 0;
		virtual std::unique_ptr<Audio> loadAudio(const std::string& path) = 0;

		virtual void playSFX(Audio& audio) = 0;
		virtual void playBGM(Audio& audio) = 0;
		virtual void stopBGM() = 0;

		virtual void pollButtons() = 0;
		virtual Buttons getDown() = 0;
		virtual Buttons getPressed() = 0;

		virtual Point getScreenDim() const = 0;
		virtual Point getShadowOffset() const = 0;

		virtual void drawRect(const Color& color, const Point& point, const Point& size) const = 0;
		virtual void drawTriangle(const Color& color, const std::array<Point, 3>& points) const = 0;
		virtual void drawFont(const Font& font, const Point& point, const std::string& text) const = 0;

		/**
		 * This is usually a good way to initialize the mt19937 but some platforms have poor access to
		 * std::random_device, so the driver will leave it up to sub implementations if they want to change it.
		 */
		virtual std::unique_ptr<Twist> getTwister() {
			std::random_device source;
			std::mt19937::result_type data[std::mt19937::state_size];
			generate(std::begin(data), std::end(data), ref(source));
			return std::make_unique<Twist>(
				std::make_unique<std::seed_seq>(std::begin(data), std::end(data))
			);
		};

		/**
		 * Some platforms may ignore this
		 */
		void setScreen(Screen select) {screen = select;}

	protected:
		Screen screen;
	};
}

#ifdef _3DS
#include "Platform3DS.hpp"
#elif __SWITCH__
#include "PlatformSwitch.h"
#else
#include "PlatformWin.h"
#endif

namespace SuperHaxagon {
	static std::unique_ptr<Platform> getPlatform() {
#ifdef _3DS
		return std::make_unique<Platform3DS>();
#elif __SWITCH__
		return std::make_unique<PlatformSwitch>();
#else
		return std::make_unique<PlatformWin>();
#endif
	}
}
#endif //SUPER_HAXAGON_PLATFORM_HPP
