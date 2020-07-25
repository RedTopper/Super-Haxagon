#include "Driver/3DS/PlayerWav3DS.hpp"

namespace SuperHaxagon {
	PlayerWav3DS::PlayerWav3DS(u8* data, const u32 sampleRate, const u32 dataSize, const u16 channels, const u16 bitsPerSample, const u16 ndspFormat) :
			_data(data),
			_sampleRate(sampleRate),
			_dataSize(dataSize),
			_channels(channels),
			_bitsPerSample(bitsPerSample),
			_ndspFormat(ndspFormat)
	{}

	PlayerWav3DS::~PlayerWav3DS() {
		ndspChnWaveBufClear(_channel);
	}

	void PlayerWav3DS::play() {
		float mix[12]{};
		mix[0] = 1.0;
		mix[1] = 1.0;
		ndspChnReset(_channel);
		ndspChnWaveBufClear(_channel);
		ndspChnSetInterp(_channel, NDSP_INTERP_LINEAR);
		ndspChnSetRate(_channel, _sampleRate);
		ndspChnSetFormat(_channel, _ndspFormat);
		ndspChnSetMix(_channel, mix);
		_buffer.data_vaddr = &_data[0];
		_buffer.nsamples = _dataSize / _bitsPerSample * 8;
		_buffer.looping = _loop;
		_buffer.offset = 0;
		DSP_FlushDataCache(_data, _dataSize);
		ndspChnWaveBufAdd(_channel, &_buffer);
	}

	bool PlayerWav3DS::isDone() const {
		return _buffer.status == NDSP_WBUF_DONE;
	}

	double PlayerWav3DS::getTime() const {
		// Don't care about the time of WAV files
		return 0;
	}
}
