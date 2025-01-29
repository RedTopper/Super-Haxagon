#ifndef SUPER_HAXAGON_AUDIO_PLAYER_SOUND_SFML_HPP
#define SUPER_HAXAGON_AUDIO_PLAYER_SOUND_SFML_HPP

#include "Core/AudioPlayer.hpp"

#include <SFML/Audio/Sound.hpp>

#include <memory>

namespace SuperHaxagon {
	class AudioPlayerSoundSFML : public AudioPlayer {
	public:
		explicit AudioPlayerSoundSFML(std::unique_ptr<sf::Sound> sound);
		~AudioPlayerSoundSFML() override;

		void setChannel(int) override {};
		void setLoop(bool loop) override;

		void play() override;
		void pause() override {}
		bool isDone() const override;
		float getTime() const override;

	private:
		std::unique_ptr<sf::Sound> _sound;
	};
}

#endif //SUPER_HAXAGON_AUDIO_PLAYER_SOUND_SFML_HPP
