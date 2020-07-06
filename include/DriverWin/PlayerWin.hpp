#ifndef SUPER_HAXAGON_PLAYER_WIN_HPP
#define SUPER_HAXAGON_PLAYER_WIN_HPP
#define SFML_STATIC

#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Sound.hpp>

#include "Driver/Player.hpp"

namespace SuperHaxagon {
	class PlayerWin : public Player {
	public:
		PlayerWin(sf::SoundBuffer& buffer, bool loaded);
		~PlayerWin();

		void setChannel(int) override {};
		void setLoop(bool loop) override;

		void play() override;
		void stop() override;
		bool isDone() override;

	private:
		sf::Sound sound;
		bool loaded = false;
		bool loop = false;
	};
}

#endif //SUPER_HAXAGON_PLAYER_WIN_HPP
