#include "Driver/3DS/AudioWav3DS.hpp"

#include "Driver/3DS/PlayerWav3DS.hpp"

#include <fstream>

namespace SuperHaxagon {
	AudioWav3DS::AudioWav3DS(const std::string& path) {
		_loaded = false;

		// Open file`
		std::ifstream file(path + ".wav", std::ios::in | std::ios::binary);
		if(!file) return;

		// Check signature
		char sig[5] = {};
		file.read(sig, 4);
		if(std::string("RIFF") != sig) return;

		// File sizes and metadata
		file.seekg(40);
		file.read(reinterpret_cast<char*>(&_dataSize), sizeof(_dataSize));
		file.seekg(22);
		file.read(reinterpret_cast<char*>(&_channels), sizeof(_channels));
		file.seekg(24);
		file.read(reinterpret_cast<char*>(&_sampleRate), sizeof(_sampleRate));
		file.seekg(34);
		file.read(reinterpret_cast<char*>(&_bitsPerSample), sizeof(_bitsPerSample));

		// Check header
		if(_dataSize == 0 || !(_channels == 1 || _channels == 2) || !(_bitsPerSample == 8 || _bitsPerSample == 16)) return;

		_data = static_cast<u8*>(linearAlloc(_dataSize));

		// Check buffer
		if(!_data) return;

		// Read file
		file.seekg(44);
		file.read(reinterpret_cast<char*>(_data), _dataSize);

		// Set sound format
		if(_bitsPerSample == 8) {
			_ndspFormat = (_channels == 1) ? NDSP_FORMAT_MONO_PCM8  : NDSP_FORMAT_STEREO_PCM8 ;
		} else {
			_ndspFormat = (_channels == 1) ? NDSP_FORMAT_MONO_PCM16 : NDSP_FORMAT_STEREO_PCM16;
		}

		// Finish load
		_loaded = true;
	}

	AudioWav3DS::~AudioWav3DS() {
		if(!_loaded) return;
		linearFree(_data);
	}

	std::unique_ptr<Player> AudioWav3DS::instantiate() {
		if (!_loaded) return nullptr;
		return std::make_unique<PlayerWav3DS>(_data, _sampleRate, _dataSize, _channels, _bitsPerSample, _ndspFormat);
	}
}
