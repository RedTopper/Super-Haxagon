#ifndef SUPER_HAXAGON_AUDIO_3DS_H
#define SUPER_HAXAGON_AUDIO_3DS_H

#include <3ds.h>
#include <bits/unique_ptr.h>

#include "Audio.h"

namespace SuperHaxagon {
	class Audio3DS : public Audio {
	public:
		Audio3DS(const std::string& path);
		~Audio3DS();

		std::unique_ptr<Player> instantiate(bool loop) override;

	private:
		u8* data;
		u32 sampleRate;
		u32 dataSize;
		u16 channels;
		u16 bitsPerSample;
		u16 ndspFormat;
		int level;
		bool loaded;
	};
}

#endif //SUPER_HAXAGON_AUDIO_3DS_H
