#ifndef SUPER_HAXAGON_AUDIO_3DS_HPP
#define SUPER_HAXAGON_AUDIO_3DS_HPP

#include <3ds.h>
#include <memory>

#include "Driver/Audio.hpp"

namespace SuperHaxagon {
	class AudioOgg3DS : public Audio {
	public:
		// See https://github.com/devkitPro/3ds-examples/blob/master/audio/opus-decoding/source/main.c
		static constexpr int OP_N_BUFFS = 3;
		static constexpr int OP_SAMPLE_RATE = 48000;
		static constexpr int OP_SAMPLES_PER_BUF = OP_SAMPLE_RATE * 120 / 1000;
		static constexpr int OP_CHANNELS_PER_SAMPLE = 2;
		static constexpr size_t WAVEBUF_SIZE = OP_SAMPLES_PER_BUF * OP_CHANNELS_PER_SAMPLE * sizeof(int16_t);

		explicit AudioOgg3DS(const std::string& path);
		~AudioOgg3DS() override;

		std::unique_ptr<Player> instantiate() override;

	private:
		int16_t* _audioBuffer = nullptr;
		ndspWaveBuf _waveBuffs[OP_N_BUFFS]{};
		bool _loaded;
	};
}

#endif //SUPER_HAXAGON_AUDIO_3DS_HPP
