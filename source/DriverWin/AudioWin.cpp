#include "DriverWin/AudioWin.hpp"
#include "DriverWin/PlayerWin.hpp"

namespace SuperHaxagon {
	AudioWin::AudioWin(const std::string& path) {};
	AudioWin::~AudioWin() = default;

	std::unique_ptr<Player> AudioWin::instantiate() {
		return std::make_unique<PlayerWin>();
	}
}
