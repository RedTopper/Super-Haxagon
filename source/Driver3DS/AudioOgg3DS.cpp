#include <string>
#include <opus/opusfile.h>

#include "Driver3DS/AudioOgg3DS.hpp"
#include "Driver3DS/PlayerOgg3DS.hpp"

namespace SuperHaxagon {
	AudioOgg3DS::AudioOgg3DS(const std::string& path) {
		_loaded = false;

		auto error = 0;
		OggOpusFile *opusFile = op_open_file(path.c_str(), &error);
		if(error) return;

		const auto bufferSize = WAVEBUF_SIZE * OP_N_BUFFS;
		_audioBuffer = static_cast<int16_t*>(linearAlloc(bufferSize));
		if(!_audioBuffer) {
			op_free(opusFile);
			return;
		}

		for(auto & oggBuff : _waveBuffs) {
			oggBuff.data_vaddr = _audioBuffer;
			oggBuff.status = NDSP_WBUF_DONE;
			_audioBuffer += WAVEBUF_SIZE / sizeof(_audioBuffer);
		}

		_loaded = true;
	}

	AudioOgg3DS::~AudioOgg3DS() {
		if(!_loaded) return;
		linearFree(_audioBuffer);
	}

	std::unique_ptr<Player> AudioOgg3DS::instantiate() {
		if (!_loaded) return nullptr;
		return std::make_unique<PlayerOgg3DS>();
	}
}
