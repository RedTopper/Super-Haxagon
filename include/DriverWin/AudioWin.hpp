#ifndef SUPER_HAXAGON_AUDIO_WIN_HPP
#define SUPER_HAXAGON_AUDIO_WIN_HPP

#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Music.hpp>

#include "Driver/Audio.hpp"

namespace SuperHaxagon {
	class AudioWin : public Audio {
	public:
		AudioWin(const std::string& path, Stream stream);
		~AudioWin() override;

		std::unique_ptr<Player> instantiate() override;

	private:
		Stream _stream = Stream::NONE;

		std::unique_ptr<sf::SoundBuffer> _buffer;
		std::unique_ptr<sf::Music> _music;
	};
}

#endif //SUPER_HAXAGON_AUDIO_WIN_HPP
