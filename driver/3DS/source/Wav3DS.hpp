#ifndef WAV_3DS_HPP
#define WAV_3DS_HPP

// SPDX-FileCopyrightText: 2025 AJ Walter
// SPDX-License-Identifier: GPL-3.0-or-later OR MIT

#include <3ds.h>

#include <string>

namespace SuperHaxagon {
	class Wav {
	public:
		Wav(std::string path);
		~Wav();

		u8 *data = nullptr;
		u32 sampleRate;
		u32 dataSize;
		u16 channels;
		u16 bitsPerSample;
		u16 ndspFormat;
		u16 blockAlign;
		bool loaded = false;
	};
}

#endif //WAV_3DS_HPP
