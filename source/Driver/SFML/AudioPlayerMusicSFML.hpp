#ifndef SUPER_HAXAGON_AUDIO_PLAYER_MUSIC_SFML_HPP
#define SUPER_HAXAGON_AUDIO_PLAYER_MUSIC_SFML_HPP

#include "Core/AudioPlayer.hpp"

#include <SFML/Audio/Music.hpp>

#include <memory>

namespace SuperHaxagon {
	class AudioPlayerMusicSFML : public AudioPlayer {
	public:
		explicit AudioPlayerMusicSFML(std::unique_ptr<sf::Music> music);
		~AudioPlayerMusicSFML() override;
		
		void setChannel(int) override {};
		void setLoop(bool loop) override;

		void play() override;
		void pause() override;
		bool isDone() const override;
		float getTime() const override;

	private:
		std::unique_ptr<sf::Music> _music;
	};
}

#endif //SUPER_HAXAGON_AUDIO_PLAYER_MUSIC_SFML_HPP
