#pragma once

typedef struct {
	u8* data;
	u32 sampleRate;
	u32 dataSize;
	u16 channels;
	u16 bitsPerSample;
	u16 ndspFormat;
	int ndspChannel;
	int level;
	bool loaded;
} Track;

Track g_hexagon;
Track g_select;
Track g_begin;
Track g_over;
Track g_bgm;

ndspWaveBuf waveBuffs[23];

bool audioPlay(Track *sound, bool loop);
void audioStop(Track *sound);
void audioUnload();
void initSounds();
void playLevelBGM(int level);