#ifndef DATA_SFML_HPP
#define DATA_SFML_HPP

// SPDX-FileCopyrightText: 2025 AJ Walter, see driver/SFML/README.md for MIT usage.
// SPDX-License-Identifier: GPL-3.0-or-later OR MIT

#include <SFML/Window.hpp>

#include <string>

namespace SuperHaxagon {
	void initializePlatform(std::string& sdmc, std::string& romfs, bool& platformBackslash);
	void osSpecificWindowSetup(sf::Window& window);
}

#endif //DATA_SFML_HPP
