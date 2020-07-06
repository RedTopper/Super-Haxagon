#ifndef SUPER_HAXAGON_PLAYER_3DS_HPP
#define SUPER_HAXAGON_PLAYER_3DS_HPP

#include <3ds.h>

#include "Driver/Player.hpp"

namespace SuperHaxagon {
	class Player3DS : public Player {
	public:
		Player3DS(u8* data, u32 sampleRate, u32 dataSize, u16 channels, u16 bitsPerSample, u16 ndspFormat);
		~Player3DS() override;

		void setChannel(int _channel) override { channel = _channel; }
		void setLoop(bool _loop) override { loop = _loop; }
		
		void play() override;
		void stop() override;
		bool isDone() override;

	private:
		u8* data;
		u32 sampleRate;
		u32 dataSize;
		u16 channels;
		u16 bitsPerSample;
		u16 ndspFormat;

		ndspWaveBuf buffer;

		int channel = 0;
		bool loop = false;
	};
}

#endif //SUPER_HAXAGON_PLAYER_3DS_HPP
