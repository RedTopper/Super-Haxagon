// SPDX-FileCopyrightText: 2025 AJ Walter
// SPDX-License-Identifier: GPL-3.0-or-later OR MIT

#include "Driver/Platform.hpp"

#include "Driver/Tools/Random.hpp"

#include <iostream>

namespace SuperHaxagon {
	Random Platform::getRandom() {
		std::random_device source;
		std::mt19937::result_type data[std::mt19937::state_size];
		generate(std::begin(data), std::end(data), ref(source));
		return Random(
			std::make_unique<std::seed_seq>(std::begin(data), std::end(data))
		);
	}
}
