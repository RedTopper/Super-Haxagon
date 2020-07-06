#ifndef SUPER_HAXAGON_PLAYER_WIN_HPP
#define SUPER_HAXAGON_PLAYER_WIN_HPP
#define SFML_STATIC

#include <memory>
#include <SFML/Audio/SoundSource.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/Music.hpp>

#include "Driver/Player.hpp"

namespace SuperHaxagon {
	class PlayerWin : public Player {
	public:
		explicit PlayerWin(std::unique_ptr<sf::SoundSource> source);
		~PlayerWin() override;

		void setChannel(int) override {};
		void setLoop(bool loop) override;

		void play() override;
		void stop() override;
		bool isDone() override;

	private:
		std::unique_ptr<sf::SoundSource> source;
		bool loaded = false;
	};
}

#endif //SUPER_HAXAGON_PLAYER_WIN_HPP
