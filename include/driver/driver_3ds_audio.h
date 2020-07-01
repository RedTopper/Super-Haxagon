#ifndef SUPER_HAXAGON_DRIVER_3DS_AUDIO_H
#define SUPER_HAXAGON_DRIVER_3DS_AUDIO_H

#include "driver_3ds.h"

namespace SuperHaxagon {
	class Driver3DSAudio {
	public:
		explicit Driver3DSAudio(std::string path);
		~Driver3DSAudio();

		bool finished();
		static const char* opusStrError(int error)
	};
}

#endif //SUPER_HAXAGON_DRIVER_3DS_AUDIO_H
