#ifndef SUPER_HAXAGON_AUDIO_NSPIRE_HPP
#define SUPER_HAXAGON_AUDIO_NSPIRE_HPP

#include "Driver/Audio.hpp"
#include "Driver/NSpire/PlayerNSpire.hpp"

#include <string>

namespace SuperHaxagon {
	class Platform;
	class AudioNSpire : public Audio {
	public:
		AudioNSpire() {}
		~AudioNSpire() override {}

		std::unique_ptr<Player> instantiate() override {return std::make_unique<PlayerNSpire>();}
	};
}

#endif //SUPER_HAXAGON_AUDIO_NSPIRE_HPP
