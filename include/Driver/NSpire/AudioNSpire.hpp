#ifndef SUPER_HAXAGON_AUDIO_NSPIRE_HPP
#define SUPER_HAXAGON_AUDIO_NSPIRE_HPP

#include "Core/Metadata.hpp"
#include "Driver/Audio.hpp"
#include "Driver/NSpire/PlayerNSpire.hpp"

namespace SuperHaxagon {
	enum class Location;
	class Platform;
	class AudioNSpire : public Audio {
	public:
		explicit AudioNSpire(std::unique_ptr<std::istream> file);
		~AudioNSpire() override = default;

		std::unique_ptr<Player> instantiate() override;

	private:
		float _time;
	};
}

#endif //SUPER_HAXAGON_AUDIO_NSPIRE_HPP
