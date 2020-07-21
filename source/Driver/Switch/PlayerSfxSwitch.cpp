#include "Driver/Switch/PlayerSfxSwitch.hpp"

namespace SuperHaxagon {
	PlayerSfxSwitch::PlayerSfxSwitch(Mix_Chunk* sfx) : _sfx(sfx) {}
	
	PlayerSfxSwitch::~PlayerSfxSwitch() = default;

	void PlayerSfxSwitch::play() {
		Mix_PlayChannel(-1, _sfx, 0);
	}
}
