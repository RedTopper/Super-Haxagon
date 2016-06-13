#include "sound.h"

//Used for compiling sdmc access instead of ROMFS. Might go unused.
const bool LOAD_SDMC = false;

void audioLoad(const char *path, Track *sound, int channel) {
	FILE *file = fopen(path, "rb");
	if(file != NULL){
		fseek(file, 0, SEEK_END);
		sound->sndsize = ftell(file);
		fseek(file, 0, SEEK_SET);
		sound->sndsize = sound->sndsize - 0x48; //Skip wav header.
		fseek(file, 0x48, SEEK_SET);
		sound->sndbuffer = linearAlloc(sound->sndsize);
		fread(sound->sndbuffer, 1, sound->sndsize, file);
		fclose(file);
		if (sound->sndbuffer != NULL) {
            sound->loaded = true;
			sound->channel = channel;
		} else {
            sound->loaded = false;
		}
	} else {
        sound->loaded = false;
	}
}

void audioFree(Track *sound) {
	if(sound->loaded) {
		memset(sound->sndbuffer, 0, sound->sndsize);
		GSPGPU_FlushDataCache(sound->sndbuffer, sound->sndsize);
		linearFree(sound->sndbuffer);
	}
}

void audioPlay(Track *sound, bool loop) {
	if (sound->loaded) {
		u32 flags;
		if (loop) {
			flags = SOUND_FORMAT_16BIT | SOUND_REPEAT;
		} else {
			flags = SOUND_FORMAT_16BIT;
		}
		csndPlaySound(sound->channel, flags, 44100, 1, 0, sound->sndbuffer, sound->sndbuffer, sound->sndsize);
	}
}

void audioUnload(){
	csndExecCmds(true);

	CSND_SetPlayState(1, 0);
	CSND_SetPlayState(2, 0);
	CSND_SetPlayState(3, 0);
	CSND_SetPlayState(4, 0);

	audioFree(&g_hexagon);
	audioFree(&g_select);
	audioFree(&g_begin);
	audioFree(&g_over);
	audioFree(&g_bgm);
}

void initSounds() {
	if(LOAD_SDMC) {
		audioLoad("sound/hexagon.bin", &g_hexagon, 1);
		audioLoad("sound/select.bin", &g_select, 2);
		audioLoad("sound/begin.bin", &g_begin, 3);
		audioLoad("sound/over.bin", &g_over, 4);
	} else {
		audioLoad("romfs:/sound/hexagon.bin", &g_hexagon, 1);
		audioLoad("romfs:/sound/select.bin", &g_select, 2);
		audioLoad("romfs:/sound/begin.bin", &g_begin, 3);
		audioLoad("romfs:/sound/over.bin", &g_over, 4);
	}
}