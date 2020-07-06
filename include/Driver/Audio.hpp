#ifndef SUPER_HAXAGON_AUDIO_HPP
#define SUPER_HAXAGON_AUDIO_HPP

#include <memory>

namespace SuperHaxagon {
	enum class Stream {
		NONE,
		DIRECT,
		INDIRECT
	};
	
	class Player;
	
	class Audio {
	public:
		Audio() = default;
		Audio(Audio&) = delete;
		virtual ~Audio() = default;

		virtual std::unique_ptr<Player> instantiate() = 0;
	};
}

#endif //SUPER_HAXAGON_AUDIO_HPP
