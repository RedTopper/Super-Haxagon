#ifndef SUPER_HAXAGON_AUDIO_H
#define SUPER_HAXAGON_AUDIO_H

#include "Player.h"

namespace SuperHaxagon {
	class Audio {
	public:
		virtual std::unique_ptr<Player> instantiate(bool loop) = 0;
	};
}

#endif //SUPER_HAXAGON_AUDIO_H
