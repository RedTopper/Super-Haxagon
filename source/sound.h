#pragma once

#include <3ds.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    u8* sndbuffer;
    u32 sndsize;
    int channel;
    int duration;
	bool loaded;
} Track;

Track g_hexagon;
Track g_select;
Track g_begin;
Track g_over;
Track g_bgm;

void audioUnload();
void initSounds();