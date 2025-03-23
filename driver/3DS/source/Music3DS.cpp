// SPDX-FileCopyrightText: 2025 AJ Walter
// SPDX-License-Identifier: GPL-3.0-or-later OR MIT

#include "Driver/Music.hpp"

#include "Wav3DS.hpp"

#include <stb_vorbis.c>

#include <array>

namespace SuperHaxagon {
	LightEvent _event{};
	void audioThread(void* const self);

	const int BUFFER_MS = 200;
	static constexpr int THREAD_AFFINITY = -1;
	static constexpr int THREAD_STACK_SZ = 32 * 1024;

	unsigned int getSamplesPerBuff(const unsigned int sampleRate) {
		return sampleRate * BUFFER_MS / 1000;
	}

	unsigned int getWaveBuffSize(const unsigned int sampleRate, const int channels) {
		return getSamplesPerBuff(sampleRate) * channels * sizeof(int16_t);
	}

	struct Music::MusicImpl {
		MusicImpl(const std::string& path) {
			loaded = false;

			auto error = 0;
			oggFile = stb_vorbis_open_filename(path.c_str(), &error, nullptr);
			if(error || !(oggFile->channels == 1 || oggFile->channels == 2)) return;

			const auto bufferSize = getWaveBuffSize(oggFile->sample_rate, oggFile->channels) * waveBuffs.size();
			audioBuffer = static_cast<int16_t*>(linearAlloc(bufferSize));
			if(!audioBuffer) {
				stb_vorbis_close(oggFile);
				return;
			}

			memset(&waveBuffs, 0, waveBuffs.size());
			auto* buffer = audioBuffer;

			for(auto& waveBuff : waveBuffs) {
				waveBuff.data_vaddr = buffer;
				waveBuff.status = NDSP_WBUF_DONE;
				buffer += getWaveBuffSize(oggFile->sample_rate, oggFile->channels) / sizeof(buffer[0]);
			}

			loaded = true;
		}

		~MusicImpl() {
			if (!loaded) return;

			threadRunning = false;

			// Shutdown thread
			if (thread) {
				LightEvent_Signal(&_event);
				threadJoin(thread, 100000000);
				threadFree(thread);
			}

			ndspChnWaveBufClear(channel);
			ndspChnReset(channel);
			linearFree(audioBuffer);
			stb_vorbis_close(oggFile);
		}

		void play() {
			if (!loaded) return;

			if (thread) {
				if (diff) start += svcGetSystemTick() - diff;
				ndspChnSetPaused(channel, false);
				LightEvent_Signal(&_event);
				diff = 0;
				return;
			}

			ndspChnReset(channel);
			ndspChnSetInterp(channel, NDSP_INTERP_POLYPHASE);
			ndspChnSetRate(channel, static_cast<float>(oggFile->sample_rate));
			ndspChnSetFormat(channel, oggFile->channels == 1 ? NDSP_FORMAT_MONO_PCM16 : NDSP_FORMAT_STEREO_PCM16);

			int32_t priority = 0x30;
			svcGetThreadPriority(&priority, CUR_THREAD_HANDLE);

			priority += 1; // Set LOWER priority and clamp
			priority = priority < 0x18 ? 0x18 : priority;
			priority = priority > 0x3F ? 0x3F : priority;

			// Start the thread, passing the player as an argument.
			thread = threadCreate(audioThread, this, THREAD_STACK_SZ, priority, THREAD_AFFINITY, false);
			start = svcGetSystemTick();
		}

		float getTime() const {
			if (!loaded) return 0;

			// If we set diff (paused), we are frozen in time. Otherwise, the current timestamp is
			// the system minus the start of the song.
			const auto ticks = diff ? diff - start : svcGetSystemTick() - start;
			const auto timeMs = static_cast<float>(ticks) / static_cast<float>(CPU_TICKS_PER_MSEC);
			return timeMs / 1000.0f;
		}

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
		auto* data = static_cast<Music::MusicImpl*>(self);
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

	Music::Music(std::unique_ptr<MusicImpl> impl) : _impl(std::move(impl)) {}

	Music::~Music() = default;

	// 3DS: The thread knows when the song is done and will
	// loop itself appropriately.
	void Music::update() const {}

	void Music::setLoop(const bool loop) const {
		_impl->loop = loop;
	}

	void Music::play() const {
		_impl->play();
	}

	void Music::pause() const {
		ndspChnSetPaused(0, true);
		_impl->diff = svcGetSystemTick();
	}

	bool Music::isDone() const {
		return !_impl->threadRunning;
	}

	float Music::getTime() const {
		return _impl->getTime();
	}

	std::unique_ptr<Music> createMusic(const std::string& path) {
		auto data = std::make_unique<Music::MusicImpl>(path + ".ogg");
		if (!data->loaded) return nullptr;
		return std::make_unique<Music>(std::move(data));
	}
}