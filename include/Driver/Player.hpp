#ifndef SUPER_HAXAGON_PLAYER_HPP
#define SUPER_HAXAGON_PLAYER_HPP

namespace SuperHaxagon {
	class Player {
	public:
		Player() = default;
		Player(Player&) = delete;
		virtual ~Player() = default;

	private:
		virtual void setChannel(int channel) = 0;
		virtual void setLoop(bool loop) = 0;
		virtual void play() = 0;
		virtual bool isDone() = 0;

		// These controls shouldn't be accessed outside of the platform
		friend class Platform3DS;
		friend class PlatformWin;
	};
}

#endif //SUPER_HAXAGON_PLAYER_HPP
