// SPDX-FileCopyrightText: 2025 AJ Walter, see driver/3DS/README.md for MIT usage.
// SPDX-License-Identifier: GPL-3.0-or-later OR MIT

#include "Driver/Sound.hpp"

#include "Wav3DS.hpp"

namespace SuperHaxagon {
	// 3ds has 24 tracks
	// This includes the channel reserved for BGM!
	constexpr int MAX_TRACKS = 16;

	// Global store for what current channel we are using
	int channel = 1;
	ndspWaveBuf buffers[MAX_TRACKS]{};

	struct Sound::SoundImpl {
		SoundImpl(const std::string path) : wav(path) {}

		void play() const {
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
			buffer.nsamples = (wav.dataSize / (wav.bitsPerSample * wav.channels)) * 8;
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

		const Wav wav;
	};

	Sound::Sound(std::unique_ptr<SoundImpl> impl) : _impl(std::move(impl)) {}

	Sound::~Sound() = default;

	void Sound::play() const {
		_impl->play();
	}

	std::unique_ptr<Sound> createSound(const std::string& path) {
		return std::make_unique<Sound>(std::make_unique<Sound::SoundImpl>(path));
	}
}
