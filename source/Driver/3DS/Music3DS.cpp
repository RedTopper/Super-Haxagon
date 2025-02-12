#include "Driver/Music.hpp"

#include "Wav3DS.hpp"

#include <stb_vorbis.c>

#include <array>

namespace SuperHaxagon {
	struct Music::MusicData {
		MusicData(const std::string& path) : path(path) {}

		std::string path;
		Thread thread = nullptr;
		int16_t* audioBuffer = nullptr;
		stb_vorbis* oggFile = nullptr;
		std::array<ndspWaveBuf, 3> waveBuffs{};
		volatile bool loaded = false;
		volatile bool loop = false;
		volatile bool threadRunning = true;
		volatile bool decodeDone = false;
		volatile int channel = 0;
		volatile uint64_t start = 0;
		volatile uint64_t diff = 0;
	};

	const int BUFFER_MS = 200;
	static constexpr int THREAD_AFFINITY = -1;
	static constexpr int THREAD_STACK_SZ = 32 * 1024;
	LightEvent _event{};

	unsigned int getSamplesPerBuff(const unsigned int sampleRate) {
		return sampleRate * BUFFER_MS / 1000;
	}

	unsigned int getWaveBuffSize(const unsigned int sampleRate, const int channels) {
		return getSamplesPerBuff(sampleRate) * channels * sizeof(int16_t);
	}

	void audioCallback(void*) {
		LightEvent_Signal(&_event);
	}

	bool audioDecode(stb_vorbis* file, ndspWaveBuf* ndspBuffer, const int channel) {
		long totalSamples = 0;

		while (totalSamples < static_cast<long>(getSamplesPerBuff(file->sample_rate))) {
			auto* const buffer = ndspBuffer->data_pcm16 + (totalSamples * file->channels);
			const size_t bufferSize = (getSamplesPerBuff(file->sample_rate) - totalSamples) * file->channels;

			const auto samples = stb_vorbis_get_samples_short_interleaved(file, file->channels, buffer, bufferSize);
			if (samples <= 0) break;

			totalSamples += samples;
		}

		if (totalSamples == 0) return false;
		ndspBuffer->nsamples = totalSamples;
		ndspChnWaveBufAdd(channel, ndspBuffer);
		DSP_FlushDataCache(ndspBuffer->data_pcm16, totalSamples * file->channels * sizeof(int16_t));
		return true;
	}

	void audioThread(void* const self) {
		auto* data = static_cast<Music::MusicData*>(self);
		if (!data) return;

		while(data->threadRunning) {
			size_t buffsDone = 0;
			for (auto& waveBuff : data->waveBuffs) {
				if (waveBuff.status != NDSP_WBUF_DONE) continue;

				buffsDone += 1;

				if (!audioDecode(data->oggFile, &waveBuff, data->channel)) {
					data->decodeDone = true;
				}
			}

			if (data->decodeDone && buffsDone == data->waveBuffs.size()) {
				if (!data->loop) {
					data->threadRunning = false;
					return;
				}

				// We are looping, so go back to the beginning
				stb_vorbis_seek_start(data->oggFile);
				data->start = svcGetSystemTick();
				data->decodeDone = false;
			}

			if (!data->threadRunning) return;
			LightEvent_Wait(&_event);
		}
	}

	std::unique_ptr<Music> createMusic(const std::string& path) {
		auto data = std::make_unique<Music::MusicData>(path + ".ogg");
		return std::make_unique<Music>(std::move(data));
	}

	Music::Music(std::unique_ptr<MusicData> data) {
		data->loaded = false;

		auto error = 0;
		data->oggFile = stb_vorbis_open_filename(data->path.c_str(), &error, nullptr);
		if(error || !(data->oggFile->channels == 1 || data->oggFile->channels == 2)) return;

		const auto bufferSize = getWaveBuffSize(data->oggFile->sample_rate, data->oggFile->channels) * data->waveBuffs.size();
		data->audioBuffer = static_cast<int16_t*>(linearAlloc(bufferSize));
		if(!data->audioBuffer) {
			stb_vorbis_close(data->oggFile);
			return;
		}

		memset(&data->waveBuffs, 0, data->waveBuffs.size());
		auto* buffer = data->audioBuffer;

		for(auto& waveBuff : data->waveBuffs) {
			waveBuff.data_vaddr = buffer;
			waveBuff.status = NDSP_WBUF_DONE;
			buffer += getWaveBuffSize(data->oggFile->sample_rate, data->oggFile->channels) / sizeof(buffer[0]);
		}

		data->loaded = true;
		_data = std::move(data);
	}

	Music::~Music() {
		if (!_data->loaded) return;

		_data->threadRunning = false;

		// Shutdown thread
		if (_data->thread) {
			LightEvent_Signal(&_event);
			threadJoin(_data->thread, 100000000);
			threadFree(_data->thread);
		}

		ndspChnWaveBufClear(_data->channel);
		ndspChnReset(_data->channel);
		linearFree(_data->audioBuffer);
		stb_vorbis_close(_data->oggFile);
	}

	// 3DS: The thread knows when the song is done and will
	// loop itself appropriately.
	void Music::update() const {}

	void Music::setLoop(const bool loop) const {
		_data->loop = loop;
	}

	void Music::play() const {
		if (!_data->loaded) return;

		if (_data->thread) {
			if (_data->diff) _data->start += svcGetSystemTick() - _data->diff;
			ndspChnSetPaused(_data->channel, false);
			LightEvent_Signal(&_event);
			_data->diff = 0;
			return;
		}

		ndspChnReset(_data->channel);
		ndspChnSetInterp(_data->channel, NDSP_INTERP_POLYPHASE);
		ndspChnSetRate(_data->channel, static_cast<float>(_data->oggFile->sample_rate));
		ndspChnSetFormat(_data->channel, _data->oggFile->channels == 1 ? NDSP_FORMAT_MONO_PCM16 : NDSP_FORMAT_STEREO_PCM16);

		int32_t priority = 0x30;
		svcGetThreadPriority(&priority, CUR_THREAD_HANDLE);

		priority += 1; // Set LOWER priority and clamp
		priority = priority < 0x18 ? 0x18 : priority;
		priority = priority > 0x3F ? 0x3F : priority;

		// Start the thread, passing the player as an argument.
		_data->thread = threadCreate(audioThread, _data.get(), THREAD_STACK_SZ, priority, THREAD_AFFINITY, false);
		_data->start = svcGetSystemTick();
	}

	void Music::pause() const {
		ndspChnSetPaused(0, true);
		_data->diff = svcGetSystemTick();
	}

	bool Music::isDone() const {
		return !_data->threadRunning;
	}

	float Music::getTime() const {
		if (!_data->loaded) return 0;

		// If we set diff (paused), we are frozen in time. Otherwise, the current timestamp is
		// the system minus the start of the song.
		const auto ticks = _data->diff ? _data->diff - _data->start : svcGetSystemTick() - _data->start;
		const auto timeMs = static_cast<float>(ticks) / static_cast<float>(CPU_TICKS_PER_MSEC);
		return timeMs / 1000.0f;
	}
}