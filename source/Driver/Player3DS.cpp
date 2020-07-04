#ifdef _3DS
#include "Driver/Player3DS.hpp"

namespace SuperHaxagon {
	Player3DS::Player3DS(u8* data, u32 sampleRate, u32 dataSize, u16 channels, u16 bitsPerSample, u16 ndspFormat) :
			data(data),
			sampleRate(sampleRate),
			dataSize(dataSize),
			channels(channels),
			bitsPerSample(bitsPerSample),
			ndspFormat(ndspFormat),
			buffer()
	{}

	Player3DS::~Player3DS() {
		Player3DS::stop();
		ndspChnWaveBufClear(channel);
	}

	void Player3DS::play() {
		stop();
		ndspChnReset(channel);
		ndspChnWaveBufClear(channel);
		ndspChnSetInterp(channel, NDSP_INTERP_LINEAR);
		ndspChnSetRate(channel, sampleRate);
		ndspChnSetFormat(channel, ndspFormat);
		buffer.data_vaddr = (void*)data;
		buffer.nsamples = dataSize / bitsPerSample;
		buffer.looping = loop;
		buffer.offset = 0;
		DSP_FlushDataCache(data, dataSize);
		ndspChnWaveBufAdd(channel, &buffer);
	}

	void Player3DS::stop() {
		ndspChnReset(channel);
	}

	bool Player3DS::isDone() {
		return buffer.status == NDSP_WBUF_DONE;
	}
}

#endif