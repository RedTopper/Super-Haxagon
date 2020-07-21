#ifndef SUPER_HAXAGON_AUDIO_SWITCH_HPP
#define SUPER_HAXAGON_AUDIO_SWITCH_HPP

#include <string>
#include <SDL2/SDL_mixer.h>

#include "Driver/Audio.hpp"

namespace SuperHaxagon {
	class Platform;
	class AudioSwitch : public Audio {
	public:
		AudioSwitch(Platform& platform, const std::string& path, Stream stream);
		~AudioSwitch() override;

		std::unique_ptr<Player> instantiate() override;

	private:
		Mix_Music* _music = nullptr;
		Mix_Chunk* _sfx = nullptr;
	};
}

#endif //SUPER_HAXAGON_AUDIO_SWITCH_HPP
