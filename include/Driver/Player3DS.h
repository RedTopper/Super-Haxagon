#ifndef SUPER_HAXAGON_PLAYER_3DS_H
#define SUPER_HAXAGON_PLAYER_3DS_H

#include <3ds.h>

#include "Player.h"

namespace SuperHaxagon {
	class Player3DS : public Player {
	public:
		Player3DS(u8* data, u32 sampleRate, u32 dataSize, u16 channels, u16 bitsPerSample, u16 ndspFormat);
		~Player3DS();

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

		ndspWaveBuf controller;

		int channel = 0;
		bool loop = false;
	};
}

#endif //SUPER_HAXAGON_PLAYER_3DS_H
