#ifndef SUPER_HAXAGON_PLATFORM_WIN_H
#define SUPER_HAXAGON_PLATFORM_WIN_H

#include "Platform.h"

namespace SuperHaxagon {
	class PlatformWin : public Platform {
	public:
		std::string getPath(const std::string &partial) override;
		std::string getPathRom(const std::string &partial) override;

		std::unique_ptr<Audio> loadAudio(const std::string &path) override;
		void playSFX(Audio& audio) override;
		void playBGM(Audio& audio) override;
		void stopBGM() override;
	};
}

#endif //SUPER_HAXAGON_PLATFORM_WIN_H
