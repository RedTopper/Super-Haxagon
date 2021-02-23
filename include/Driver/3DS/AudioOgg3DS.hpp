#ifndef SUPER_HAXAGON_AUDIO_OGG_3DS_HPP
#define SUPER_HAXAGON_AUDIO_OGG_3DS_HPP

#include "Core/Audio.hpp"

#include <memory>
#include <string>

namespace SuperHaxagon {
	class AudioOgg3DS : public Audio {
	public:
		explicit AudioOgg3DS(const std::string& path);
		~AudioOgg3DS() override;

		std::unique_ptr<Player> instantiate() override;

	private:
		const std::string _path;
	};
}

#endif //SUPER_HAXAGON_AUDIO_OGG_3DS_HPP
