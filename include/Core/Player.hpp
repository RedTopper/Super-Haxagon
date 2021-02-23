#ifndef SUPER_HAXAGON_PLAYER_HPP
#define SUPER_HAXAGON_PLAYER_HPP

namespace SuperHaxagon {
	class Player {
	public:
		Player() = default;
		Player(Player&) = delete;
		virtual ~Player() = default;

		virtual void setChannel(int channel) = 0;
		virtual void setLoop(bool loop) = 0;
		virtual void play() = 0;
		virtual void pause() = 0;
		virtual bool isDone() const = 0;
		virtual float getTime() const = 0;
	};
}

#endif //SUPER_HAXAGON_PLAYER_HPP
