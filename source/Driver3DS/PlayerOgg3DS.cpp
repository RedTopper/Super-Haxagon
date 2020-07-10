#include <3ds.h>
#include <memory.h>

#include <stb_vorbis.c>

#include "Driver3DS/PlayerOgg3DS.hpp"

namespace SuperHaxagon {
	const int BUFFER_MS = 200;
	const int AVERAGE_MS = 40;

	LightEvent PlayerOgg3DS::_event{};

	PlayerOgg3DS::PlayerOgg3DS(const std::string& path) {
		_loaded = false;

		auto error = 0;
		_oggFile = stb_vorbis_open_filename(path.c_str(), &error, nullptr);
		if(error || !(_oggFile->channels == 1 || _oggFile->channels == 2)) return;

		const auto bufferSize = getWaveBuffSize(_oggFile->sample_rate, _oggFile->channels) * _waveBuffs.size();
		_audioBuffer = static_cast<int16_t*>(linearAlloc(bufferSize));
		if(!_audioBuffer) {
			stb_vorbis_close(_oggFile);
			return;
		}

		memset(&_waveBuffs, 0, _waveBuffs.size());
		int16_t* buffer = _audioBuffer;

		for(auto& _waveBuff : _waveBuffs) {
			_waveBuff.data_vaddr = buffer;
			_waveBuff.status = NDSP_WBUF_DONE;
			buffer += getWaveBuffSize(_oggFile->sample_rate, _oggFile->channels) / sizeof(buffer[0]);
		}

		_loaded = true;
	}

	PlayerOgg3DS::~PlayerOgg3DS() {
		if (!_loaded) return;
		if (_thread) {
			_quit = true;
			LightEvent_Signal(&_event);
			threadJoin(_thread, UINT64_MAX);
			threadFree(_thread);
		}

		ndspChnReset(_channel);
		linearFree(_audioBuffer);
		stb_vorbis_close(_oggFile);
	}

	void PlayerOgg3DS::setChannel(int channel) {
		_channel = channel;
	}

	void PlayerOgg3DS::setLoop(bool loop) {
		_loop = loop;
	}

	void PlayerOgg3DS::play() {
		if (!_loaded) return;

		ndspChnReset(_channel);
		ndspChnSetInterp(_channel, NDSP_INTERP_POLYPHASE);
		ndspChnSetRate(_channel, static_cast<float>(_oggFile->sample_rate));
		ndspChnSetFormat(_channel, _oggFile->channels == 1 ? NDSP_FORMAT_MONO_PCM16 : NDSP_FORMAT_STEREO_PCM16);

		int32_t priority = 0x30;
		svcGetThreadPriority(&priority, CUR_THREAD_HANDLE);

		priority -= 1; // Set higher priority and clamp
		priority = priority < 0x18 ? 0x18 : priority;
		priority = priority > 0x3F ? 0x3F : priority;

		// Start the thread, passing our opusFile as an argument.
		_thread = threadCreate(audioThread, this, THREAD_STACK_SZ, priority, THREAD_AFFINITY, false);
	}

	bool PlayerOgg3DS::isDone() {
		return !_quit;
	}

	double PlayerOgg3DS::getVelocity() {
		if (!_loaded || !_currentBuffer) return 0;

		auto offset = svcGetSystemTick() - _tick;
		auto* buffer = _currentBuffer;
		auto ms = static_cast<int>(offset / CPU_TICKS_PER_MSEC);
		ms = ms > BUFFER_MS - AVERAGE_MS ? BUFFER_MS - AVERAGE_MS : (ms < 0 ? 0 : ms);
		auto samplesToAverage = _oggFile->sample_rate * AVERAGE_MS / 1000;
		auto samplesStart = _oggFile->sample_rate * ms / 1000;
		auto total = 0.0;
		for (unsigned int i = 0; i < samplesToAverage; i+= _oggFile->channels) {
			total += pow(static_cast<double>(buffer[samplesStart + i]), 2);
		}

		auto val = sqrt(total / samplesToAverage) / 32767;
		return val > 1 ? 1 : val;
	}

	bool PlayerOgg3DS::audioDecode(stb_vorbis* file, ndspWaveBuf* buff, int channel, bool loop) {
		long totalSamples = 0;

		while (totalSamples < static_cast<long>(getSamplesPerBuff(file->sample_rate))) {
			int16_t* buffer = buff->data_pcm16 + (totalSamples * file->channels);
			const size_t bufferSize = (getSamplesPerBuff(file->sample_rate) - totalSamples) * file->channels;


			const int samples = stb_vorbis_get_samples_short_interleaved(file, file->channels, buffer, bufferSize);
			if (samples <= 0) break;

			totalSamples += samples;
		}

		if (totalSamples == 0) {
			if (loop) {
				stb_vorbis_seek_start(file);
			} else {
				return false;
			}
		}

		buff->nsamples = totalSamples;
		ndspChnWaveBufAdd(channel, buff);
		DSP_FlushDataCache(buff->data_pcm16,totalSamples * file->channels * sizeof(int16_t));
		return true;
	}

	void PlayerOgg3DS::audioCallback(void*) {
		LightEvent_Signal(&_event);
	}

	void PlayerOgg3DS::audioThread(void* const self) {
		auto* pointer = static_cast<PlayerOgg3DS*>(self);
		if (!pointer) return;

		while(!pointer->_quit) {
			for(auto& _waveBuff : pointer->_waveBuffs) {
				if(_waveBuff.status != NDSP_WBUF_DONE) continue;
				if(!audioDecode(pointer->_oggFile, &_waveBuff, pointer->_channel, pointer->_loop)) return;
				pointer->_currentBuffer = _waveBuff.data_pcm16;
				pointer->_tick = svcGetSystemTick();
			}

			LightEvent_Wait(&_event);
		}
	}

	unsigned int PlayerOgg3DS::getSamplesPerBuff(unsigned int sampleRate) {
		return sampleRate * BUFFER_MS / 1000;
	}

	unsigned int PlayerOgg3DS::getWaveBuffSize(unsigned int sampleRate, int channels) {
		return getSamplesPerBuff(sampleRate) * channels * sizeof(int16_t);
	}
}