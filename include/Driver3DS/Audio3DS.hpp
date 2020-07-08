#ifndef SUPER_HAXAGON_AUDIO_3DS_HPP
#define SUPER_HAXAGON_AUDIO_3DS_HPP

#include <3ds.h>
#include <memory>

#include "Driver/Audio.hpp"

namespace SuperHaxagon {
	class Audio3DS : public Audio {
	public:
		explicit Audio3DS(const std::string& path);
		~Audio3DS() override;

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

#endif //SUPER_HAXAGON_AUDIO_3DS_HPP
