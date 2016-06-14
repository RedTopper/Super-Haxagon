#include "sound.h"

void audioLoad(const char *path, Track *sound, int channel) {
	FILE *file = fopen(path, "rb");
	if(file == NULL){
	    sound->loaded = false; //bad file
		return;
	}
	
	char sig[4];
	fread(sig, 1, 4, file);
	
	if(sig[0] != 'R' && sig[1] != 'I' && sig[2] != 'F' && sig[3] != 'F'){
		sound->loaded = false; //bad signature
		fclose(file);
		return;
	}
	
	fseek(file, 40, SEEK_SET);
	fread(&(sound->dataSize), 4, 1, file);
	fseek(file, 22, SEEK_SET);
	fread(&(sound->channels), 2, 1, file);
	fseek(file, 24, SEEK_SET);
	fread(&(sound->sampleRate), 4, 1, file);
	fseek(file, 34, SEEK_SET);
	fread(&(sound->bitsPerSample), 2, 1, file);
	
	if(sound->dataSize == 0 || (sound->channels != 1 && sound->channels != 2) || (sound->bitsPerSample != 8 && sound->bitsPerSample != 16)) {
		sound->loaded = false; //bad header
		fclose(file);
		return;
	}
	
	sound->data = linearAlloc(sound->dataSize);
	
	if(!(sound->data)) {
		linearFree(sound->data); //could this cause problems?
		sound->loaded = false; //bad buffer
		fclose(file);
		return;
	}
	
	fseek(file, 44, SEEK_SET);
	fread(sound->data, 1, sound->dataSize, file);
	fclose(file);
	
	if(sound->bitsPerSample == 8) {
		sound->ndspFormat = (sound->channels == 1) ? NDSP_FORMAT_MONO_PCM8  : NDSP_FORMAT_STEREO_PCM8 ;
	} else {
		sound->ndspFormat = (sound->channels == 1) ? NDSP_FORMAT_MONO_PCM16 : NDSP_FORMAT_STEREO_PCM16;
	}
	
	sound->loaded = true;
	sound->ndspChannel = channel;
}

void audioFree(Track *sound) {
	if(sound->loaded) {
		memset(sound->data, 0, sound->dataSize);
		linearFree(sound->data);
	}
}

// Thanks Rinnegatamante
void createDspBlock(ndspWaveBuf* waveBuf, u16 bps, u32 size, bool loop, u32* data){
	waveBuf->data_vaddr = (void*)data;
	waveBuf->nsamples = size / bps;
	waveBuf->looping = loop;
	waveBuf->offset = 0;	
	DSP_FlushDataCache(data, size);
}

bool audioPlay(Track *sound, bool loop) {
	if (sound->loaded) {
		ndspChnReset(sound->ndspChannel);
		ndspChnWaveBufClear(sound->ndspChannel);
		ndspChnSetInterp(sound->ndspChannel, NDSP_INTERP_LINEAR);
		ndspChnSetRate(sound->ndspChannel, sound->sampleRate);
		ndspChnSetFormat(sound->ndspChannel, sound->ndspFormat);
		
		createDspBlock(&waveBuffs[sound->ndspChannel], sound->bitsPerSample >> 3, sound->dataSize, loop, (u32*)sound->data);
		
		DSP_FlushDataCache(sound->data, sound->dataSize);
		
		ndspChnWaveBufAdd(sound->ndspChannel, &waveBuffs[sound->ndspChannel]);
		return true;
	} else {
		return false;
	}
}

void audioUnload() {
	ndspChnWaveBufClear(1);
	ndspChnWaveBufClear(2);
	ndspChnWaveBufClear(3);
	ndspChnWaveBufClear(4);
	ndspChnWaveBufClear(5);

	audioFree(&g_hexagon);
	audioFree(&g_select);
	audioFree(&g_begin);
	audioFree(&g_over);
	audioFree(&g_bgm);
}

void initSounds() {
	audioLoad("romfs:/sound/hexagon.bin", &g_hexagon, 1);
	audioLoad("romfs:/sound/select.bin", &g_select, 2);
	audioLoad("romfs:/sound/begin.bin", &g_begin, 3);
	audioLoad("romfs:/sound/over.bin", &g_over, 4);
	g_bgm.level = -1;
}

void playLevelBGM(int level) {
	ndspChnReset(5);
	if(g_bgm.level != level) {
		audioFree(&g_bgm);
		switch(level) {
			case 0:
				audioLoad("romfs:/music/drFinkelfracken.bin", &g_bgm, 5); break;
			case 1:
				audioLoad("romfs:/music/jackRussel.bin", &g_bgm, 5); break;
			case 2:
				audioLoad("romfs:/music/callMeKatla.bin", &g_bgm, 5); break;
			case 3:
				audioLoad("romfs:/music/captainCool.bin", &g_bgm, 5); break;
			case 4:
				audioLoad("romfs:/music/commandoSteve.bin", &g_bgm, 5); break;
			case 5:
				audioLoad("romfs:/music/mazeOfMayonnaise.bin", &g_bgm, 5); break;
		}
		g_bgm.level = level;
	}
	audioPlay(&g_bgm, true);
}