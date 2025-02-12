#include "Driver/Sound.hpp"

#include "Wav3DS.hpp"

namespace SuperHaxagon {
	// 3ds has 24 tracks
	// This includes the channel reserved for BGM!
	constexpr int MAX_TRACKS = 16;

	// Global store for what current channel we are using
	int channel = 1;
	ndspWaveBuf buffers[MAX_TRACKS]{};

	struct Sound::SoundData {
		SoundData(const std::string path) : wav(path) {}
		const Wav wav;
	};

	std::unique_ptr<Sound> createSound(const std::string& path) {
		return std::make_unique<Sound>(std::make_unique<Sound::SoundData>(path));
	}

	Sound::Sound(std::unique_ptr<SoundData> data) : _data(std::move(data)) {}

	Sound::~Sound() {
		ndspChnWaveBufClear(channel);
	}

	void Sound::play() const {
		const Wav& wav = _data->wav;
		if (!wav.loaded) return;

		ndspWaveBuf& buffer = buffers[channel];
		float mix[12]{};
		mix[0] = 1.0;
		mix[1] = 1.0;
		ndspChnReset(channel);
		ndspChnWaveBufClear(channel);
		ndspChnSetInterp(channel, NDSP_INTERP_LINEAR);
		ndspChnSetRate(channel, wav.sampleRate);
		ndspChnSetFormat(channel, wav.ndspFormat);
		ndspChnSetMix(channel, mix);
		buffer.data_vaddr = &wav.data[0];
		buffer.nsamples = wav.dataSize / wav.bitsPerSample * 8;
		buffer.looping = false;
		buffer.offset = 0;
		DSP_FlushDataCache(wav.data, wav.dataSize);
		ndspChnWaveBufAdd(channel, &buffer);

		// Next play will use next channel.
		channel++;

		// Channel 0 is reserved for BGM.
		// Only one BGM is ever expected to be played at a time.
		if (channel > MAX_TRACKS) channel = 1;
	}
}
