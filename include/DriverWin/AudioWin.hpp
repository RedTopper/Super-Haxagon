#ifndef SUPER_HAXAGON_AUDIO_WIN_HPP
#define SUPER_HAXAGON_AUDIO_WIN_HPP
#define SFML_STATIC

#include <SFML/Audio/SoundBuffer.hpp>

#include "Driver/Audio.hpp"

namespace SuperHaxagon {
	class AudioWin : public Audio {
	public:
		explicit AudioWin(const std::string& path);
		~AudioWin();

		std::unique_ptr<Player> instantiate() override;

	private:
		sf::SoundBuffer buffer;
		bool loaded = false;
	};
}

#endif //SUPER_HAXAGON_AUDIO_WIN_HPP
