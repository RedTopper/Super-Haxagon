#ifndef SUPER_HAXAGON_AUDIO_WIN_HPP
#define SUPER_HAXAGON_AUDIO_WIN_HPP
#define SFML_STATIC

#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/Music.hpp>

#include "Driver/Audio.hpp"

namespace SuperHaxagon {
	class AudioWin : public Audio {
	public:
		AudioWin(const std::string& path, Stream stream);
		~AudioWin() override;

		std::unique_ptr<Player> instantiate() override;

	private:
		Stream stream = Stream::NONE;

		std::unique_ptr<sf::SoundBuffer> buffer;
		std::unique_ptr<sf::Music> music;
	};
}

#endif //SUPER_HAXAGON_AUDIO_WIN_HPP
