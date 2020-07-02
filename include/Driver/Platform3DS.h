#ifndef SUPER_HAXAGON_PLATFORM_3DS_H
#define SUPER_HAXAGON_PLATFORM_3DS_H

#include "Platform.h"
#include "Audio.h"

static const int MAX_TRACKS = 4;

namespace SuperHaxagon {
	class Platform3DS : public Platform {
	public:
		Platform3DS();
		~Platform3DS();

		std::string getPath(const std::string& partial) override;
		std::string getPathRom(const std::string& partial) override;

		std::unique_ptr<Audio> loadAudio(const std::string& path) override;
		void playSFX(Audio* audio) override;
		void playBGM(Audio* audio) override;
		void stopBGM() override;

	private:
		std::unique_ptr<Audio> sfx[MAX_TRACKS];
		std::unique_ptr<Audio> bgm;
	};
}

#endif //SUPER_HAXAGON_PLATFORM_3DS_H
