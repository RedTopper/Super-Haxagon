// SPDX-FileCopyrightText: 2025 AJ Walter
// SPDX-License-Identifier: GPL-3.0-or-later OR MIT

#include "Driver/Platform.hpp"

#include "Core/Twist.hpp"

#include <iostream>

namespace SuperHaxagon {
	Twist Platform::getTwister() {
		std::random_device source;
		std::mt19937::result_type data[std::mt19937::state_size];
		generate(std::begin(data), std::end(data), ref(source));
		return Twist(
			std::make_unique<std::seed_seq>(std::begin(data), std::end(data))
		);
	}
}
