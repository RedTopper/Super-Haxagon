#ifndef SUPER_HAXAGON_AUDIO_LOADER_NSPIRE_HPP
#define SUPER_HAXAGON_AUDIO_LOADER_NSPIRE_HPP

#include "Core/Metadata.hpp"
#include "Core/AudioLoader.hpp"
#include "Driver/NSpire/AudioPlayerNspire.hpp"

namespace SuperHaxagon {
	enum class Location;
	class Platform;
	class AudioLoaderNspire : public AudioLoader {
	public:
		explicit AudioLoaderNspire(std::unique_ptr<std::istream> file);
		~AudioLoaderNspire() override = default;

		std::unique_ptr<AudioPlayer> instantiate() override;

	private:
		float _time;
	};
}

#endif //SUPER_HAXAGON_AUDIO_LOADER_NSPIRE_HPP
