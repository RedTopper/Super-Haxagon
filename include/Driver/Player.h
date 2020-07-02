#ifndef SUPER_HAXAGON_PLAYER_H
#define SUPER_HAXAGON_PLAYER_H

namespace SuperHaxagon {
	class Player {
		virtual void setChannel(int channel) = 0;
		virtual void play() = 0;
		virtual void stop() = 0;
		virtual bool isPlaying() = 0;
	};
}

#endif //SUPER_HAXAGON_PLAYER_H
