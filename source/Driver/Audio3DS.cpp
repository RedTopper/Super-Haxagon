#include <string>
#include <fstream>

#include "Driver/Audio3DS.hpp"
#include "Driver/Player3DS.hpp"

namespace SuperHaxagon {
	Audio3DS::Audio3DS(const std::string& path) {
		loaded = false;

		//open file
		std::ifstream file(path, std::ios::in | std::ios::binary);
		if(!file) return;

		//check signature
		char sig[5] = {};
		file.read(sig, 4);
		if(std::string("RIFF") != sig) return;

		//file sizes and metadata
		file.seekg(40);
		file >> dataSize;
		file.seekg(22);
		file >> channels;
		file.seekg(24);
		file >> sampleRate;
		file.seekg(34);
		file >> bitsPerSample;

		//check header
		if(dataSize == 0 || !(channels == 1 || channels == 2) || !(bitsPerSample == 8 || bitsPerSample == 16)) return;

		data = static_cast<u8*>(linearAlloc(dataSize));

		//check buffer
		if(!data) return;

		//read file
		file.seekg(44);
		file.read(reinterpret_cast<char*>(data), dataSize);

		//set sound format
		if(bitsPerSample == 8) {
			ndspFormat = (channels == 1) ? NDSP_FORMAT_MONO_PCM8  : NDSP_FORMAT_STEREO_PCM8 ;
		} else {
			ndspFormat = (channels == 1) ? NDSP_FORMAT_MONO_PCM16 : NDSP_FORMAT_STEREO_PCM16;
		}

		//finish load
		loaded = true;
	}

	Audio3DS::~Audio3DS() {
		if(!loaded) return;
		linearFree(data);
	}

	std::unique_ptr<Player> Audio3DS::instantiate() {
		if (!loaded) return nullptr;
		return std::make_unique<Player3DS>(data, sampleRate, dataSize, channels, bitsPerSample, ndspFormat);
	}
}

