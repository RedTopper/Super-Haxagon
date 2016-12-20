#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>

#include "sound.h"

void audioLoad(const char* path, Track* sound, int channel) {
	sound->loaded = false;
	
	//open file
	FILE *file = fopen(path, "rb");
	if(!file) return;
	
	//check signature
	char sig[4];
	fread(sig, 1, 4, file);
	if(!(sig[0] == 'R' && sig[1] == 'I' && sig[2] == 'F' && sig[3] == 'F')){
		fclose(file);
		return;
	}
	
	//file sizes and metadata
	fseek(file, 40, SEEK_SET);
	fread(&(sound->dataSize), 4, 1, file);
	fseek(file, 22, SEEK_SET);
	fread(&(sound->channels), 2, 1, file);
	fseek(file, 24, SEEK_SET);
	fread(&(sound->sampleRate), 4, 1, file);
	fseek(file, 34, SEEK_SET);
	fread(&(sound->bitsPerSample), 2, 1, file);
	
	//check header
	if(sound->dataSize == 0 || !(sound->channels == 1 || sound->channels == 2) || !(sound->bitsPerSample == 8 || sound->bitsPerSample == 16)) {
		fclose(file);
		return;
	}
	
	sound->data = linearAlloc(sound->dataSize);
	
	//check buffer
	if(!(sound->data)) {
		fclose(file);
		return;
	}
	
	//read file
	fseek(file, 44, SEEK_SET);
	fread(sound->data, 1, sound->dataSize, file);
	fclose(file);
	
	//set sound format
	if(sound->bitsPerSample == 8) {
		sound->ndspFormat = (sound->channels == 1) ? NDSP_FORMAT_MONO_PCM8  : NDSP_FORMAT_STEREO_PCM8 ;
	} else {
		sound->ndspFormat = (sound->channels == 1) ? NDSP_FORMAT_MONO_PCM16 : NDSP_FORMAT_STEREO_PCM16;
	}
	
	//finish load
	sound->loaded = true;
	sound->ndspChannel = channel;
}

void audioFree(Track* sound) {
	if(!(sound->loaded)) return;
	ndspChnWaveBufClear(sound->ndspChannel);
	memset(sound->data, 0, sound->dataSize);
	linearFree(sound->data);
}

void audioPlay(Track* sound, bool loop) {
	if (!(sound->loaded)) return;
	ndspChnReset(sound->ndspChannel);
	ndspChnWaveBufClear(sound->ndspChannel);
	ndspChnSetInterp(sound->ndspChannel, NDSP_INTERP_LINEAR);
	ndspChnSetRate(sound->ndspChannel, sound->sampleRate);
	ndspChnSetFormat(sound->ndspChannel, sound->ndspFormat);
	createDspBlock(&waveBuffs[sound->ndspChannel], sound->bitsPerSample >> 3, sound->dataSize, loop, (u32*)sound->data);
	DSP_FlushDataCache(sound->data, sound->dataSize);
	ndspChnWaveBufAdd(sound->ndspChannel, &waveBuffs[sound->ndspChannel]);
}

void audioStop(Track* sound) {
	if (!(sound->loaded)) return;
	ndspChnReset(sound->ndspChannel);
}

// Thanks Rinnegatamante
void createDspBlock(ndspWaveBuf* waveBuf, u16 bps, u32 size, bool loop, u32* data){
	waveBuf->data_vaddr = (void*)data;
	waveBuf->nsamples = size / bps;
	waveBuf->looping = loop;
	waveBuf->offset = 0;	
	DSP_FlushDataCache(data, size);
}