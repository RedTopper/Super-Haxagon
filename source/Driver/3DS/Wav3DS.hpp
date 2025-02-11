#ifndef WAV_3DS_HPP
#define WAV_3DS_HPP

#include <3ds.h>

#include <string>

namespace SuperHaxagon {
	class Wav {
	public:
		Wav(std::string path);
		~Wav();

		u8 *data = nullptr;
		u32 sampleRate;
		u32 dataSize;
		u16 channels;
		u16 bitsPerSample;
		u16 ndspFormat;
		bool loaded = false;
	};
}

#endif //WAV_3DS_HPP
