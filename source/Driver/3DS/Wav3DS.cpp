#include "Wav3DS.hpp"

#include <fstream>

namespace SuperHaxagon {
	Wav::Wav(std::string path) {
		// Open file`
		std::ifstream file(path + ".wav", std::ios::in | std::ios::binary);
		if(!file) return;

		// Check signature
		char sig[5] = {};
		file.read(sig, 4);
		if(std::string("RIFF") != sig) return;

		// File sizes and metadata
		file.seekg(40);
		file.read(reinterpret_cast<char*>(&dataSize), sizeof(dataSize));
		file.seekg(22);
		file.read(reinterpret_cast<char*>(&channels), sizeof(channels));
		file.seekg(24);
		file.read(reinterpret_cast<char*>(&sampleRate), sizeof(sampleRate));
		file.seekg(34);
		file.read(reinterpret_cast<char*>(&bitsPerSample), sizeof(bitsPerSample));

		// Check header
		if(dataSize == 0 || !(channels == 1 || channels == 2) || !(bitsPerSample == 8 || bitsPerSample == 16)) return;

		data = static_cast<u8*>(linearAlloc(dataSize));

		// Check buffer
		if(!data) return;

		// Read file
		file.seekg(44);
		file.read(reinterpret_cast<char*>(data), dataSize);

		// Set sound format
		if(bitsPerSample == 8) {
			ndspFormat = (channels == 1) ? NDSP_FORMAT_MONO_PCM8  : NDSP_FORMAT_STEREO_PCM8 ;
		} else {
			ndspFormat = (channels == 1) ? NDSP_FORMAT_MONO_PCM16 : NDSP_FORMAT_STEREO_PCM16;
		}

		// Finish load
		loaded = true;
	}

	Wav::~Wav() {
		linearFree(data);
	}
} // SuperHaxagon