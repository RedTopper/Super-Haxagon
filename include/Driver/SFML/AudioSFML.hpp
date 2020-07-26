#ifndef SUPER_HAXAGON_AUDIO_SFML_HPP
#define SUPER_HAXAGON_AUDIO_SFML_HPP

#include "Driver/Audio.hpp"

#include <SFML/Audio/Music.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

namespace SuperHaxagon {
	class AudioSFML : public Audio {
	public:
		AudioSFML(const std::string& path, Stream stream);
		~AudioSFML() override;

		std::unique_ptr<Player> instantiate() override;

	private:
		Stream _stream = Stream::NONE;

		std::unique_ptr<sf::SoundBuffer> _buffer;
		std::unique_ptr<sf::Music> _music;
	};
}

#endif //SUPER_HAXAGON_AUDIO_SFML_HPP
