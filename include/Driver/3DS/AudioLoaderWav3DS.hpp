#ifndef SUPER_HAXAGON_AUDIO_LOADER_WAV_3DS_HPP
#define SUPER_HAXAGON_AUDIO_LOADER_WAV_3DS_HPP

#include "Core/AudioLoader.hpp"

#include <3ds.h>

#include <memory>
#include <string>

namespace SuperHaxagon {
	class AudioLoaderWav3DS : public AudioLoader {
	public:
		explicit AudioLoaderWav3DS(const std::string& path);
		~AudioLoaderWav3DS() override;

		std::unique_ptr<AudioPlayer> instantiate() override;

	private:
		u8* _data = nullptr;
		u32 _sampleRate = 0;
		u32 _dataSize = 0;
		u16 _channels = 0;
		u16 _bitsPerSample = 0;
		u16 _ndspFormat = 0;

		bool _loaded;
	};
}

#endif //SUPER_HAXAGON_AUDIO_LOADER_WAV_3DS_HPP
