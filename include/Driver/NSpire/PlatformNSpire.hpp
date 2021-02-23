#ifndef SUPER_HAXAGON_PLATFORM_NSPIRE_HPP
#define SUPER_HAXAGON_PLATFORM_NSPIRE_HPP

#include "Core/Platform.hpp"

namespace SuperHaxagon {
	typedef struct {
		int x;
		int y;
	} Point2D;
	
	class PlatformNSpire : public Platform {
	public:
		explicit PlatformNSpire(Dbg dbg);
		PlatformNSpire(PlatformNSpire&) = delete;
		~PlatformNSpire() override = default;
		
		bool loop() override;
		float getDilation() override;

		std::string getPath(const std::string& partial, Location location) override;
		std::unique_ptr<std::istream> openFile(const std::string& partial, Location location) override;
		std::unique_ptr<Audio> loadAudio(const std::string& partial, Stream stream, Location location) override;
		std::unique_ptr<Font> loadFont(const std::string& partial, int size) override;
		
		void playSFX(Audio&) override {}
		void playBGM(Audio& audio) override;
		
		std::string getButtonName(const Buttons& button) override;
		Buttons getPressed() override;
		Point getScreenDim() const override;
		
		void screenBegin() override;
		void screenFinalize() override;
		void drawPoly(const Color& color, const std::vector<Point>& points) override;

		void shutdown() override;
		void message(Dbg dbg, const std::string& where, const std::string& message) override;
		Supports supports() override;

		std::unique_ptr<Twist> getTwister() override;

	private:
		float _dilation = 1.0;
	};
}

#endif //SUPER_HAXAGON_PLATFORM_NSPIRE_HPP
