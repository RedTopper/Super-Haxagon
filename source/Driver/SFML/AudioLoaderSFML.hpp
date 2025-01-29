#ifndef SUPER_HAXAGON_AUDIO_LOADER_SFML_HPP
#define SUPER_HAXAGON_AUDIO_LOADER_SFML_HPP

#include "Core/AudioLoader.hpp"

#include <SFML/Audio/Music.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

namespace SuperHaxagon {
	class AudioLoaderSFML : public AudioLoader {
	public:
		AudioLoaderSFML(const std::string& path, Stream stream);
		~AudioLoaderSFML() override;

		std::unique_ptr<AudioPlayer> instantiate() override;

	private:
		Stream _stream = Stream::NONE;

		std::unique_ptr<sf::SoundBuffer> _buffer;
		std::unique_ptr<sf::Music> _music;
	};
}

#endif //SUPER_HAXAGON_AUDIO_LOADER_SFML_HPP
