#ifndef SUPER_HAXAGON_AUDIO_LOADER_OGG_3DS_HPP
#define SUPER_HAXAGON_AUDIO_LOADER_OGG_3DS_HPP

#include "Core/AudioLoader.hpp"

#include <memory>
#include <string>

namespace SuperHaxagon {
	class AudioLoaderOgg3DS : public AudioLoader {
	public:
		explicit AudioLoaderOgg3DS(const std::string& path);
		~AudioLoaderOgg3DS() override;

		std::unique_ptr<AudioPlayer> instantiate() override;

	private:
		const std::string _path;
	};
}

#endif //SUPER_HAXAGON_AUDIO_LOADER_OGG_3DS_HPP

