#pragma once

#include <3ds.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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

bool audioPlay(Track *sound, bool loop);
void audioUnload();
void initSounds();
void playLevelBGM(int level);