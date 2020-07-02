#ifndef SUPER_HAXAGON_AUDIO_3DS_H
#define SUPER_HAXAGON_AUDIO_3DS_H

#include <3ds.h>
#include <bits/unique_ptr.h>

#include "Audio.h"

namespace SuperHaxagon {
	class Audio3DS : public Audio {
	public:
		explicit Audio3DS(const std::string& path);
		~Audio3DS();

		std::unique_ptr<Player> instantiate() override;

	private:
		u8* data = nullptr;
		u32 sampleRate = 0;
		u32 dataSize = 0;
		u16 channels = 0;
		u16 bitsPerSample = 0;
		u16 ndspFormat = 0;

		bool loaded;
	};
}

#endif //SUPER_HAXAGON_AUDIO_3DS_H
