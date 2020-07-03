#ifndef SUPER_HAXAGON_AUDIO_HPP
#define SUPER_HAXAGON_AUDIO_HPP

namespace SuperHaxagon {
	class Player;
	class Audio {
	public:
		virtual std::unique_ptr<Player> instantiate() = 0;
	};
}

#endif //SUPER_HAXAGON_AUDIO_HPP
