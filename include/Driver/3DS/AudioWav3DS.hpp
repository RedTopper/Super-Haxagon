#ifndef SUPER_HAXAGON_AUDIO_WAV_3DS_HPP
#define SUPER_HAXAGON_AUDIO_WAV_3DS_HPP

#include "Core/Audio.hpp"

#include <3ds.h>

#include <memory>
#include <string>

namespace SuperHaxagon {
	class AudioWav3DS : public Audio {
	public:
		explicit AudioWav3DS(const std::string& path);
		~AudioWav3DS() override;

		std::unique_ptr<Player> instantiate() override;

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

#endif //SUPER_HAXAGON_AUDIO_WAV_3DS_HPP
