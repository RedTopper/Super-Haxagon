#ifndef SUPER_HAXAGON_PLATFORM_NSPIRE_HPP
#define SUPER_HAXAGON_PLATFORM_NSPIRE_HPP

#include "Driver/Platform.hpp"

#include "gl.h"

namespace SuperHaxagon {
	class PlatformNSpire : public Platform {
	public:
		explicit PlatformNSpire(Dbg dbg);
		PlatformNSpire(PlatformNSpire&) = delete;
		~PlatformNSpire() override = default;
		
		bool loop() override;
		float getDilation() override {return 1.0;}
		
		std::string getPath(const std::string& partial) override;
		std::string getPathRom(const std::string& partial) override;
		std::unique_ptr<Audio> loadAudio(const std::string& path, SuperHaxagon::Stream stream) override;
		std::unique_ptr<Font> loadFont(const std::string& path, int size) override;
		
		void playSFX(Audio&) override {};
		void playBGM(Audio&) override {};
		
		std::string getButtonName(const Buttons& button) override;
		Buttons getPressed() override;
		Point getScreenDim() const override;
		
		void screenBegin() override;
		void screenSwap() override {}
		void screenFinalize() override;
		void drawPoly(const Color& color, const std::vector<Point>& points) override;

		void shutdown() override;
		void message(Dbg dbg, const std::string& where, const std::string& message) override;

		std::unique_ptr<Twist> getTwister() override;

	private:
		std::unique_ptr<COLOR[]> _framebuffer = nullptr;
		int _z = 0;
	};
}

#endif //SUPER_HAXAGON_PLATFORM_NSPIRE_HPP
