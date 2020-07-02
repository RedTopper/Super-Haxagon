#include <3ds.h>
#include <string>
#include <memory>

#include <Driver/Audio3DS.h>
#include "Driver/Platform3DS.h"

static const int SAMPLE_RATE = 48000;

namespace SuperHaxagon {
	Platform3DS::Platform3DS() {
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

	Platform3DS::~Platform3DS() {
		gfxExit();
		ndspExit();
		romfsExit();
	}

	std::string Platform3DS::getPath(const std::string& partial) {
		return std::string("sdmc:/3ds/data/haxagon") + partial;
	}

	std::string Platform3DS::getPathRom(const std::string& partial) {
		return std::string("romfs:") + partial;
	}

	std::unique_ptr<Audio> Platform3DS::loadAudio(const std::string& path) {
		return std::make_unique<Audio3DS>(path);
	}

	void Platform3DS::playSFX(Audio* audio) {

	}

	void Platform3DS::playBGM(Audio* audio) {

	}

	void Platform3DS::stopBGM() {

	}
}
