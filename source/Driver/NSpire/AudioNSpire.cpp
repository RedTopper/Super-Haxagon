#include "Driver/NSpire/AudioNSpire.hpp"

#include "Driver/Platform.hpp"

#include <istream>

namespace SuperHaxagon {
	AudioNSpire::AudioNSpire(std::unique_ptr<std::istream> file) {
		Metadata metadata(std::move(file));
		_time = metadata.getMaxTime();
	}

	std::unique_ptr<Player> AudioNSpire::instantiate() {
		return std::make_unique<PlayerNSpire>(_time);
	}
}
