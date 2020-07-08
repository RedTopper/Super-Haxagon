#include <3ds.h>
#include <string>
#include <memory>
#include <array>
#include <sys/stat.h>

#include "Driver3DS/Player3DS.hpp"
#include "Driver3DS/Audio3DS.hpp"
#include "Driver3DS/Font3DS.hpp"
#include "Driver3DS/Platform3DS.hpp"

static const int SAMPLE_RATE = 48000;

namespace SuperHaxagon {
	Platform3DS::Platform3DS() {
		romfsInit();
		gfxInitDefault();
		C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
		C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
		C2D_Prepare();

		_buff = C2D_TextBufNew(4096);
		_top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
		_bot = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

		// Setup NDSP
		ndspInit();
		ndspSetOutputMode(NDSP_OUTPUT_STEREO);

		mkdir("sdmc:/3ds", 0777);
		mkdir("sdmc:/3ds/data", 0777);
		mkdir("sdmc:/3ds/data/haxagon", 0777);
	}

	Platform3DS::~Platform3DS() {
		C2D_Fini();
		C3D_Fini();
		gfxExit();
		ndspExit();
		romfsExit();
	}

	bool Platform3DS::loop() {
		return aptMainLoop();
	}

	double Platform3DS::getDilation() {
		return 1.0;
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
		return std::make_unique<Font3DS>(path, size, _buff);
	}

	// Note: If there are no available channels the audio is silently discarded
	void Platform3DS::playSFX(Audio& audio) {
		int channel = 1;
		for (auto& player : _sfx) {
			if (!player || player->isDone()) {
				player = audio.instantiate();
				if (!player) return;
				player->setChannel(channel);
				player->setLoop(false);
				player->play();
				break;
			}

			channel++;
		}
	}

	void Platform3DS::playBGM(Audio& audio) {
		_bgm = audio.instantiate();
		if (!_bgm) return;
		_bgm->setChannel(0);
		_bgm->setLoop(true);
		_bgm->play();
	}

	void Platform3DS::stopBGM() {
		if (_bgm) _bgm->stop();
		_bgm = nullptr;
	}

	Buttons Platform3DS::getPressed() {
		hidScanInput();
		const auto input = hidKeysHeld();
		Buttons buttons{};
		buttons.select = (input & KEY_A) > 0;
		buttons.back = (input & KEY_B) > 0;
		buttons.quit = (input & KEY_START) > 0;
		buttons.left = (input & (KEY_L | KEY_ZL | KEY_CSTICK_LEFT | KEY_CPAD_LEFT | KEY_DLEFT | KEY_Y)) > 0;
		buttons.right = (input & (KEY_R | KEY_ZR | KEY_CSTICK_RIGHT | KEY_CPAD_RIGHT | KEY_DRIGHT | KEY_X)) > 0;
		return buttons;
	}

	Point Platform3DS::getScreenDim() const {
		return {static_cast<double>(_drawingOnTop ? 400 : 320), 240};
	}

	void Platform3DS::screenBegin() {
		_drawingOnTop = true;
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(_top, C2D_Color32(0x00, 0x00, 0xFF, 0xff));
		C2D_SceneBegin(_top);
	}

	void Platform3DS::screenSwap() {
		_drawingOnTop = false;
		C2D_TargetClear(_bot, C2D_Color32(0x00, 0x00, 0x00, 0xff));
		C2D_SceneBegin(_bot);
	}

	void Platform3DS::screenFinalize() {
		C3D_FrameEnd(0);
	}

	void Platform3DS::drawRect(const Color& color, const Point& point, const Point& size) {
		const auto c = C2D_Color32(color.r, color.g, color.b, color.a);
		C2D_DrawRectangle(
			static_cast<float>(point.x), 
			static_cast<float>(point.y), 0, 
			static_cast<float>(size.x), 
			static_cast<float>(size.y), 
			c, c, c, c
		);
	}

	void Platform3DS::drawTriangle(const Color& color, const std::array<Point, 3>& points) {
		const auto c = C2D_Color32(color.r, color.g, color.b, color.a);
		const auto height = getScreenDim().y;
		C2D_DrawTriangle(
			static_cast<float>(points[0].x), static_cast<float>(height - points[0].y), c,
			static_cast<float>(points[1].x), static_cast<float>(height - points[1].y), c,
			static_cast<float>(points[2].x), static_cast<float>(height - points[2].y), c,
			0
		);
	}

	std::unique_ptr<Twist> Platform3DS::getTwister() {
		// Kind of a shitty way to do this but it's the best I got.
		const auto a = new std::seed_seq{svcGetSystemTick()};
		return std::make_unique<Twist>(
			std::unique_ptr<std::seed_seq>(a)
		);
	}
}
