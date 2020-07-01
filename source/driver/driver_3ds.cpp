#include <3ds.h>
#include <string>
#include <memory>

#include "driver/driver_3ds.h"
#include "driver/driver_3ds_audio.h"

static const int SAMPLE_RATE = 48000;

namespace SuperHaxagon {
	Driver3DS::Driver3DS() {
		romfsInit();
		ndspInit();
		gfxInitDefault();

		// Setup NDSP
		ndspChnReset(0);
		ndspSetOutputMode(NDSP_OUTPUT_STEREO);
		ndspChnSetInterp(0, NDSP_INTERP_POLYPHASE);
		ndspChnSetRate(0, SAMPLE_RATE);
		ndspChnSetFormat(0, NDSP_FORMAT_STEREO_PCM16);
	}

	Driver3DS::~Driver3DS() {
		gfxExit();
		ndspExit();
		romfsExit();
	}

	std::string Driver3DS::getPathRom(std::string partial) {
		return std::string("romfs:") + partial;
	}

	std::string Driver3DS::getPath(std::string partial) {
		return std::string("sdmc:/3ds/data/haxagon") + partial;
	}

	void Driver3DS::playAudio(std::string path) {
		for (auto& track : tracks) {
			if (track->finished()) {
				track = std::make_unique<Driver3DSAudio>(path);
			}
		}
	}
}
