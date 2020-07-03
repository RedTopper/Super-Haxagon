#ifndef SUPER_HAXAGON_AUDIO_H
#define SUPER_HAXAGON_AUDIO_H

namespace SuperHaxagon {
	class Player;
	class Audio {
	public:
		virtual std::unique_ptr<Player> instantiate() = 0;
	};
}

#endif //SUPER_HAXAGON_AUDIO_H
