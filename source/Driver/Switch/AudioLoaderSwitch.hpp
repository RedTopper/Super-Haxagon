#ifndef SUPER_HAXAGON_AUDIO_LOADER_SWITCH_HPP
#define SUPER_HAXAGON_AUDIO_LOADER_SWITCH_HPP

#include "Core/AudioLoader.hpp"

#include <SDL2/SDL_mixer.h>

#include <string>

namespace SuperHaxagon {
	class Platform;
	class AudioLoaderSwitch : public AudioLoader {
	public:
		AudioLoaderSwitch(Platform& platform, const std::string& path, Stream stream);
		~AudioLoaderSwitch() override;

		std::unique_ptr<AudioPlayer> instantiate() override;

	private:
		Mix_Music* _music = nullptr;
		Mix_Chunk* _sfx = nullptr;
	};
}

#endif //SUPER_HAXAGON_AUDIO_LOADER_SWITCH_HPP
