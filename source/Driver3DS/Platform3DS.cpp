#include <3ds.h>
#include <string>
#include <memory>

#include "Driver3DS/Player3DS.hpp"
#include "Driver3DS/Audio3DS.hpp"
#include "Driver3DS/Font3DS.hpp"
#include "Driver3DS/Platform3DS.hpp"

static const int SAMPLE_RATE = 48000;

namespace SuperHaxagon {
	Platform3DS::Platform3DS() {
		romfsInit();
		ndspInit();
		gfxInitDefault();

		// Setup NDSP
		ndspSetOutputMode(NDSP_OUTPUT_STEREO);
	}

	Platform3DS::~Platform3DS() {
		gfxExit();
		ndspExit();
		romfsExit();
	}

	bool Platform3DS::loop() {
		return aptMainLoop();
	}

	double Platform3DS::getDilation() {
		return 0;
	}

	std::string Platform3DS::getPath(const std::string& partial) {
		return std::string("sdmc:/3ds/data/haxagon") + partial;
	}

	std::string Platform3DS::getPathRom(const std::string& partial) {
		return std::string("romfs:") + partial;
	}

	std::unique_ptr<Audio> Platform3DS::loadAudio(const std::string& path, Stream stream) {
		return std::make_unique<Audio3DS>(path);
	}

	std::unique_ptr<Font> Platform3DS::loadFont(const std::string& path, int size) {
		return std::make_unique<Font3DS>(path);
	}

	// Note: If there are no available channels the audio is silently discarded
	void Platform3DS::playSFX(Audio& audio) {
		int channel = 1;
		for (auto& player : sfx) {
			if (!player || player->isDone()) {
				player = audio.instantiate();
				player->setChannel(channel);
				player->setLoop(false);
				player->play();
			}

			channel++;
		}
	}

	void Platform3DS::playBGM(Audio& audio) {
		bgm = audio.instantiate();
		bgm->setChannel(0);
		bgm->setLoop(true);
		bgm->play();
	}

	void Platform3DS::stopBGM() {
		if (bgm) bgm->stop();
		bgm = nullptr;
	}

	Buttons Platform3DS::getPressed() {
		auto input = hidKeysDown();
		Buttons buttons{};
		buttons.select = (bool)(input & KEY_A);
		buttons.back = (bool)(input & KEY_B);
		buttons.quit = (bool)(input & KEY_START);
		buttons.left = (bool)(input & (KEY_L | KEY_ZL | KEY_CSTICK_LEFT | KEY_CPAD_LEFT | KEY_DLEFT | KEY_Y));
		buttons.right = (bool)(input & (KEY_R | KEY_ZR | KEY_CSTICK_RIGHT | KEY_CPAD_RIGHT | KEY_DRIGHT | KEY_X));
		return buttons;
	}

	Point Platform3DS::getScreenDim() const {
		return {400, 240}; //TODO Update
	}

	void Platform3DS::screenBegin() {

	}

	void Platform3DS::screenSwap() {

	}

	void Platform3DS::screenFinalize() {

	}

	void Platform3DS::drawRect(const Color& color, const Point& point, const Point& size) {

	}

	void Platform3DS::drawTriangle(const Color& color, const std::array<Point, 3>& points) {

	}

	std::unique_ptr<Twist> Platform3DS::getTwister() {
		// Kind of a shitty way to do this but it's the best I got.
		auto a = new std::seed_seq{svcGetSystemTick()};
		return std::make_unique<Twist>(
				std::unique_ptr<std::seed_seq>(a)
		);
	}
}
