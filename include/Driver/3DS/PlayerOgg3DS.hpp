#ifndef SUPER_HAXAGON_PLAYER_OGG_3DS_HPP
#define SUPER_HAXAGON_PLAYER_OGG_3DS_HPP

#include <string>
#include <array>
#include <3ds.h>
#include <3ds/synchronization.h>

#include "Driver/Player.hpp"

struct stb_vorbis;

namespace SuperHaxagon {
	class PlayerOgg3DS : public Player {
	public:
		static constexpr int THREAD_AFFINITY = -1;
		static constexpr int THREAD_STACK_SZ = 32 * 1024;

		explicit PlayerOgg3DS(const std::string& path);
		~PlayerOgg3DS() override;

		static void audioCallback(void*);
		static LightEvent _event;

	private:
		void setChannel(int channel) override;
		void setLoop(bool loop) override;

		void play() override;
		void pause() override;
		bool isDone() override;
		double getTime() override;

		static bool audioDecode(stb_vorbis* file, ndspWaveBuf* buff, int channel);
		static void audioThread(void*);

		// 120 ms per buffer
		static unsigned int getSamplesPerBuff(unsigned int sampleRate);
		static unsigned int getWaveBuffSize(unsigned int sampleRate, int channels);

		Thread _thread = nullptr;
		int16_t* _audioBuffer = nullptr;
		stb_vorbis* _oggFile = nullptr;
		std::array<ndspWaveBuf, 3> _waveBuffs{};
		volatile bool _loaded = false;   // if data is loaded
		volatile bool _loop = false;     // if audio should loop
		volatile bool _quit = false;     // if thread should be shut down
		volatile int _channel = 0;
		volatile uint64_t _start = 0;
		volatile uint64_t _diff = 0;
	};
}

#endif //SUPER_HAXAGON_PLAYER_OGG_3DS_HPP
