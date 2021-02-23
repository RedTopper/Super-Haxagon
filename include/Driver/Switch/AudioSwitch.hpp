#ifndef SUPER_HAXAGON_AUDIO_SWITCH_HPP
#define SUPER_HAXAGON_AUDIO_SWITCH_HPP

#include "Core/Audio.hpp"

#include <SDL2/SDL_mixer.h>

#include <string>

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
