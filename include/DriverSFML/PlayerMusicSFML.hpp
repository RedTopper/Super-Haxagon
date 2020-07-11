#ifndef SUPER_HAXAGON_PLAYER_MUSIC_SFML_HPP
#define SUPER_HAXAGON_PLAYER_MUSIC_SFML_HPP

#include <memory>
#include <SFML/Audio/Music.hpp>

#include "Driver/Player.hpp"

namespace SuperHaxagon {
	class PlayerMusicSFML : public Player {
	public:
		explicit PlayerMusicSFML(std::unique_ptr<sf::Music> music);
		~PlayerMusicSFML() override;

	private:
		void setChannel(int) override {};
		void setLoop(bool loop) override;

		void play() override;
		bool isDone() override;
		double getVelocity() override;

		std::unique_ptr<sf::Music> _music;
	};
}

#endif //SUPER_HAXAGON_PLAYER_MUSIC_SFML_HPP
