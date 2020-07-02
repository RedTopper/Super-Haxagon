#ifndef SUPER_HAXAGON_PLATFORM_H
#define SUPER_HAXAGON_PLATFORM_H
#define _3DS

#include <memory>
#include <string>

#include "Audio.h"

namespace SuperHaxagon {
	class Platform {
	public:
		virtual std::string getPath(const std::string& partial) = 0;
		virtual std::string getPathRom(const std::string& partial) = 0;

		virtual std::unique_ptr<Audio> loadAudio(const std::string& path) = 0;
		virtual void playSFX(Audio* audio) = 0;
		virtual void playBGM(Audio* audio) = 0;
		virtual void stopBGM() = 0;
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
