#include "Driver/3DS/PlayerOgg3DS.hpp"

#include <stb_vorbis.c>

namespace SuperHaxagon {
	const int BUFFER_MS = 200;

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
		auto* buffer = _audioBuffer;

		for(auto& waveBuff : _waveBuffs) {
			waveBuff.data_vaddr = buffer;
			waveBuff.status = NDSP_WBUF_DONE;
			buffer += getWaveBuffSize(_oggFile->sample_rate, _oggFile->channels) / sizeof(buffer[0]);
		}

		_loaded = true;
	}

	PlayerOgg3DS::~PlayerOgg3DS() {
		if (!_loaded) return;

		_quit = true;

		if (_thread) {
			LightEvent_Signal(&_event);
			threadJoin(_thread, 100000000);
			threadFree(_thread);
		}

		ndspChnWaveBufClear(_channel);
		ndspChnReset(_channel);
		linearFree(_audioBuffer);
		stb_vorbis_close(_oggFile);
	}

	void PlayerOgg3DS::setChannel(const int channel) {
		_channel = channel;
	}

	void PlayerOgg3DS::setLoop(const bool loop) {
		_loop = loop;
	}

	void PlayerOgg3DS::play() {
		if (!_loaded) return;

		if (_thread) {
			if (_diff) _start += svcGetSystemTick() - _diff;
			ndspChnSetPaused(_channel, false);
			LightEvent_Signal(&_event);
			_diff = 0;
			return;
		}

		ndspChnReset(_channel);
		ndspChnSetInterp(_channel, NDSP_INTERP_POLYPHASE);
		ndspChnSetRate(_channel, static_cast<float>(_oggFile->sample_rate));
		ndspChnSetFormat(_channel, _oggFile->channels == 1 ? NDSP_FORMAT_MONO_PCM16 : NDSP_FORMAT_STEREO_PCM16);

		int32_t priority = 0x30;
		svcGetThreadPriority(&priority, CUR_THREAD_HANDLE);

		priority += 1; // Set LOWER priority and clamp
		priority = priority < 0x18 ? 0x18 : priority;
		priority = priority > 0x3F ? 0x3F : priority;

		// Start the thread, passing the player as an argument.
		_thread = threadCreate(audioThread, this, THREAD_STACK_SZ, priority, THREAD_AFFINITY, false);
		_start = svcGetSystemTick();
	}

	void PlayerOgg3DS::pause() {
		ndspChnSetPaused(_channel, true);
		_diff = svcGetSystemTick();
	}

	bool PlayerOgg3DS::isDone() const {
		return !_quit;
	}

	float PlayerOgg3DS::getTime() const {
		if (!_loaded) return 0;

		// If we set diff (paused), we are frozen in time. Otherwise, the current timestamp is
		// the system minus the start of the song.
		const auto ticks = _diff ? _diff - _start : svcGetSystemTick() - _start;
		const auto timeMs = static_cast<float>(ticks) / static_cast<float>(CPU_TICKS_PER_MSEC);
		return timeMs / 1000.0f;
	}

	bool PlayerOgg3DS::audioDecode(stb_vorbis* file, ndspWaveBuf* buff, const int channel) {
		long totalSamples = 0;

		while (totalSamples < static_cast<long>(getSamplesPerBuff(file->sample_rate))) {
			auto* const buffer = buff->data_pcm16 + (totalSamples * file->channels);
			const size_t bufferSize = (getSamplesPerBuff(file->sample_rate) - totalSamples) * file->channels;

			const auto samples = stb_vorbis_get_samples_short_interleaved(file, file->channels, buffer, bufferSize);
			if (samples <= 0) break;

			totalSamples += samples;
		}

		if (totalSamples == 0) return false;
		buff->nsamples = totalSamples;
		ndspChnWaveBufAdd(channel, buff);
		DSP_FlushDataCache(buff->data_pcm16, totalSamples * file->channels * sizeof(int16_t));
		return true;
	}

	void PlayerOgg3DS::audioCallback(void*) {
		LightEvent_Signal(&_event);
	}

	void PlayerOgg3DS::audioThread(void* const self) {
		auto* pointer = static_cast<PlayerOgg3DS*>(self);
		if (!pointer) return;

		while(!pointer->_quit) {
			for (auto& waveBuff : pointer->_waveBuffs) {
				if (waveBuff.status != NDSP_WBUF_DONE) continue;
				if (!audioDecode(pointer->_oggFile, &waveBuff, pointer->_channel)) {
					if (!pointer->_loop) {
						pointer->_quit = true;
						return;
					}

					// We are looping, so go back to the beginning
					stb_vorbis_seek_start(pointer->_oggFile);
					pointer->_start = svcGetSystemTick();
				}
			}

			if (pointer->_quit) return;
			LightEvent_Wait(&_event);
		}
	}

	unsigned int PlayerOgg3DS::getSamplesPerBuff(const unsigned int sampleRate) {
		return sampleRate * BUFFER_MS / 1000;
	}

	unsigned int PlayerOgg3DS::getWaveBuffSize(const unsigned int sampleRate, const int channels) {
		return getSamplesPerBuff(sampleRate) * channels * sizeof(int16_t);
	}
}