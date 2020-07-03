#ifndef SUPER_HAXAGON_PLATFORM_3DS_H
#define SUPER_HAXAGON_PLATFORM_3DS_H

#include "Structs.h"
#include "Platform.h"
#include "Audio.h"
#include "Player.h"

static const int MAX_TRACKS = 4;

namespace SuperHaxagon {
	class Platform3DS : public Platform {
	public:
		Platform3DS();
		~Platform3DS();

		std::string getPath(const std::string& partial) override;
		std::string getPathRom(const std::string& partial) override;

		std::unique_ptr<Audio> loadAudio(const std::string& path) override;
		void playSFX(Audio& audio) override;
		void playBGM(Audio& audio) override;
		void stopBGM() override;

		void pollButtons() override;
		Buttons getDown() override;
		Buttons getPressed() override;

		Point getScreenDim()  override const;
		Point getShadowOffset() override const;
		int getRenderDistance() override const;

	private:
		static Buttons toButtons(u32);
		std::unique_ptr<Player> sfx[MAX_TRACKS];
		std::unique_ptr<Player> bgm;
	};
}

#endif //SUPER_HAXAGON_PLATFORM_3DS_H
