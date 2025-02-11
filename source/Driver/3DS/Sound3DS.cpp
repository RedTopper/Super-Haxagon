#include "Driver/Sound.hpp"

#include "Wav3DS.hpp"

namespace SuperHaxagon {
	struct Sound::SoundData {
		SoundData(Wav& wav) : wav(wav) {}
		const Wav& wav;
		int channel = 0;
		ndspWaveBuf buffer{};
	};

	std::unique_ptr<Sound> createSound(Wav& wav) {
		return std::make_unique<Sound>(std::make_unique<Sound::SoundData>(wav));
	}

	Sound::Sound(std::unique_ptr<SoundData> data) : _data(std::move(data)) {}

	Sound::~Sound() {
		ndspChnWaveBufClear(_data->channel);
	}

	void Sound::setChannel(int channel) {
		_data->channel = channel;
	}

	void Sound::play() const {
		const Wav& wav = _data->wav;
		if (!wav.loaded) return;

		const auto channel = _data->channel;
		ndspWaveBuf& buffer = _data->buffer;
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
	}

	bool Sound::isDone() const {
		return _data->buffer.status == NDSP_WBUF_DONE;
	}
}
