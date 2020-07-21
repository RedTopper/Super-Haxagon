#ifndef SUPER_HAXAGON_PLAYER_SOUND_SFML_HPP
#define SUPER_HAXAGON_PLAYER_SOUND_SFML_HPP

#include <memory>
#include <SFML/Audio/Sound.hpp>

#include "Driver/Player.hpp"

namespace SuperHaxagon {
	class PlayerSoundSFML : public Player {
	public:
		explicit PlayerSoundSFML(std::unique_ptr<sf::Sound> sound);
		~PlayerSoundSFML() override;

	private:
		void setChannel(int) override {};
		void setLoop(bool loop) override;

		void play() override;
		void pause() override {};
		bool isDone() override;
		double getVelocity() override;

		std::unique_ptr<sf::Sound> _sound;
	};
}

#endif //SUPER_HAXAGON_PLAYER_SOUND_SFML_HPP
