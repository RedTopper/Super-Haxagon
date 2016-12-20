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

ndspWaveBuf waveBuffs[23];

/**
 * Loads a music file based on a path into memory
 */
void audioLoad(const char* path, Track* sound, int channel);

/**
 * Plays a Track. loop: True if you want the audio to loop.
 */
void audioPlay(Track *sound, bool loop);

/**
 * Safely stops a Track.
 */
void audioStop(Track *sound);

/**
 * Frees a track and all of it's resources from memory.
 */
 void audioFree(Track* sound);