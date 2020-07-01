#ifndef SUPER_HAXAGON_DRIVER_3DS_H
#define SUPER_HAXAGON_DRIVER_3DS_H

#include "driver.h"

static const int MAX_TRACKS = 4;

namespace SuperHaxagon {
	class Driver3DSAudio;
	class Driver3DS : public Driver {
	public:
		Driver3DS();
		~Driver3DS();

		std::string getPath(std::string partial) override;
		std::string getPathRom(std::string partial) override;

		void playAudio(std::string path) override;

	private:
		std::unique_ptr<Driver3DSAudio> tracks[MAX_TRACKS];
	};
}

#endif //SUPER_HAXAGON_DRIVER_3DS_H
