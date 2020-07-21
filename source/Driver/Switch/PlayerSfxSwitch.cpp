#include "Driver/Switch/PlayerSfxSwitch.hpp"

namespace SuperHaxagon {
	PlayerSfxSwitch::PlayerSfxSwitch(Mix_Chunk* sfx) : _sfx(sfx) {}

	void PlayerSfxSwitch::play() {
		Mix_PlayChannel(-1, _sfx, 0);
	}
}
