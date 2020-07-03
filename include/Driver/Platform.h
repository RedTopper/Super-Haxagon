#ifndef SUPER_HAXAGON_PLATFORM_H
#define SUPER_HAXAGON_PLATFORM_H
#define _3DS

#include <memory>
#include <string>

#include "Audio.h"
#include "Structs.h"

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

	class Platform {
	public:
		virtual std::string getPath(const std::string& partial) = 0;
		virtual std::string getPathRom(const std::string& partial) = 0;

		virtual std::unique_ptr<Audio> loadAudio(const std::string& path) = 0;

		virtual void playSFX(Audio& audio) = 0;
		virtual void playBGM(Audio& audio) = 0;
		virtual void stopBGM() = 0;

		virtual void pollButtons() = 0;
		virtual Buttons getDown() = 0;
		virtual Buttons getPressed() = 0;

		/* Some platforms may ignore this */
		void setScreen(Screen select) { screen = select; }
		virtual Point getScreenDim() const = 0;
		virtual Point getShadowOffset() const = 0;
		virtual int getRenderDistance() const = 0;

		virtual void drawRect(const Color& color, const Point& point, const Point& size) const = 0;
		virtual void drawTriangle(const Color& color, const std::array<Point, 3>& points) const = 0;

	protected:
		Screen screen;
	};
}

#ifdef _3DS
#include "Platform3DS.h"
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
#endif //SUPER_HAXAGON_PLATFORM_H
