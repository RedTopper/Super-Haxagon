#ifndef DRIVER_SOUND_HPP
#define DRIVER_SOUND_HPP

// SPDX-FileCopyrightText: 2025 AJ Walter
// SPDX-License-Identifier: GPL-3.0-or-later OR MIT

#include <memory>

namespace SuperHaxagon {
	class Sound {
	public:
		struct SoundImpl;
		explicit Sound(std::unique_ptr<SoundImpl> impl);
		Sound(Sound&) = delete;
		~Sound();

		void play() const;

	private:
		std::unique_ptr<SoundImpl> _impl;
	};
}

#endif //DRIVER_SOUND_HPP
