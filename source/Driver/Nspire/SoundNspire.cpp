#include "Driver/Sound.hpp"

// Simple file so the linker has something to link to
namespace SuperHaxagon {
	struct Sound::SoundImpl {};
	Sound::~Sound() = default;
	void Sound::play() const {}
}