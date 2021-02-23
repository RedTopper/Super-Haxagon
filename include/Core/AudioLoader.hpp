#ifndef SUPER_HAXAGON_AUDIO_LOADER_HPP
#define SUPER_HAXAGON_AUDIO_LOADER_HPP

#include <memory>

namespace SuperHaxagon {
	enum class Stream {
		NONE,
		DIRECT,
		INDIRECT
	};
	
	class AudioPlayer;
	
	class AudioLoader {
	public:
		AudioLoader() = default;
		AudioLoader(AudioLoader&) = delete;
		virtual ~AudioLoader() = default;

		virtual std::unique_ptr<AudioPlayer> instantiate() = 0;
	};
}

#endif //SUPER_HAXAGON_AUDIO_LOADER_HPP
