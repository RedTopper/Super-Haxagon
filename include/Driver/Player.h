#ifndef SUPER_HAXAGON_PLAYER_H
#define SUPER_HAXAGON_PLAYER_H

namespace SuperHaxagon {
	class Player {
	private:
		virtual void setChannel(int channel) = 0;
		virtual void setLoop(bool loop) = 0;
		virtual void play() = 0;
		virtual void stop() = 0;
		virtual bool isDone() = 0;

		// These controls shouldn't be accessed outside of the platform
		friend class Platform3DS;
		friend class PlatformWin;
	};
}

#endif //SUPER_HAXAGON_PLAYER_H
