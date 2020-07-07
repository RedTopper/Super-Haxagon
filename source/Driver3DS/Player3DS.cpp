#include "Driver3DS/Player3DS.hpp"

namespace SuperHaxagon {
	Player3DS::Player3DS(u8* data, u32 sampleRate, u32 dataSize, u16 channels, u16 bitsPerSample, u16 ndspFormat) :
			data(data),
			sampleRate(sampleRate),
			dataSize(dataSize),
			channels(channels),
			bitsPerSample(bitsPerSample),
			ndspFormat(ndspFormat)
	{}

	Player3DS::~Player3DS() {
		Player3DS::stop();
		ndspChnWaveBufClear(channel);
	}

	void Player3DS::play() {
		stop();
		float mix[12]{};
		mix[0] = 1.0;
		mix[1] = 1.0;
		ndspChnReset(channel);
		ndspChnSetInterp(channel, NDSP_INTERP_LINEAR);
		ndspChnSetRate(channel, sampleRate);
		ndspChnSetFormat(channel, ndspFormat);
		ndspChnSetMix(channel, mix);
		buffer.data_vaddr = &data[0];
		buffer.nsamples = dataSize / bitsPerSample * 8;
		buffer.looping = loop;
		buffer.offset = 0;
		DSP_FlushDataCache(data, dataSize);
		ndspChnWaveBufAdd(channel, &buffer);
	}

	void Player3DS::stop() {
		ndspChnWaveBufClear(channel);
	}

	bool Player3DS::isDone() {
		return buffer.status == NDSP_WBUF_DONE;
	}
}
