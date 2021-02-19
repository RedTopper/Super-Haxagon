#ifndef SUPER_HAXAGON_PLAYER_SOUND_SFML_HPP
#define SUPER_HAXAGON_PLAYER_SOUND_SFML_HPP

#include "Driver/Player.hpp"

#include <SFML/Audio/Sound.hpp>

#include <memory>

namespace SuperHaxagon {
	class PlayerSoundSFML : public Player {
	public:
		explicit PlayerSoundSFML(std::unique_ptr<sf::Sound> sound);
		~PlayerSoundSFML() override;

		void setChannel(int) override {};
		void setLoop(bool loop) override;

		void play() override;
		void pause() override {};
		bool isDone() const override;
		float getTime() const override;

	private:
		std::unique_ptr<sf::Sound> _sound;
	};
}

#endif //SUPER_HAXAGON_PLAYER_SOUND_SFML_HPP
