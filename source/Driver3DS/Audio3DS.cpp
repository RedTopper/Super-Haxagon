#include <string>
#include <fstream>

#include "Driver3DS/Audio3DS.hpp"
#include "Driver3DS/Player3DS.hpp"

namespace SuperHaxagon {
	Audio3DS::Audio3DS(const std::string& path) {
		_loaded = false;

		//open file
		std::ifstream file(path + ".wav", std::ios::in | std::ios::binary);
		if(!file) return;

		//check signature
		char sig[5] = {};
		file.read(sig, 4);
		if(std::string("RIFF") != sig) return;

		//file sizes and metadata
		file.seekg(40);
		file.read(reinterpret_cast<char*>(&_dataSize), sizeof(_dataSize));
		file.seekg(22);
		file.read(reinterpret_cast<char*>(&_channels), sizeof(_channels));
		file.seekg(24);
		file.read(reinterpret_cast<char*>(&_sampleRate), sizeof(_sampleRate));
		file.seekg(34);
		file.read(reinterpret_cast<char*>(&_bitsPerSample), sizeof(_bitsPerSample));

		//check header
		if(_dataSize == 0 || !(_channels == 1 || _channels == 2) || !(_bitsPerSample == 8 || _bitsPerSample == 16)) return;

		_data = static_cast<u8*>(linearAlloc(_dataSize));

		//check buffer
		if(!_data) return;

		//read file
		file.seekg(44);
		file.read(reinterpret_cast<char*>(_data), _dataSize);

		//set sound format
		if(_bitsPerSample == 8) {
			_ndspFormat = (_channels == 1) ? NDSP_FORMAT_MONO_PCM8  : NDSP_FORMAT_STEREO_PCM8 ;
		} else {
			_ndspFormat = (_channels == 1) ? NDSP_FORMAT_MONO_PCM16 : NDSP_FORMAT_STEREO_PCM16;
		}

		//finish load
		_loaded = true;
	}

	Audio3DS::~Audio3DS() {
		if(!_loaded) return;
		linearFree(_data);
	}

	std::unique_ptr<Player> Audio3DS::instantiate() {
		if (!_loaded) return nullptr;
		return std::make_unique<Player3DS>(_data, _sampleRate, _dataSize, _channels, _bitsPerSample, _ndspFormat);
	}
}
