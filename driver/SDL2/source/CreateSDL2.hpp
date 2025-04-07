#ifndef DATA_SDL2_HPP
#define DATA_SDL2_HPP

// SPDX-FileCopyrightText: 2025 AJ Walter
// SPDX-License-Identifier: GPL-3.0-or-later OR MIT

#include "Driver/Platform.hpp"

#include <SDL.h>

#include <string>

namespace SuperHaxagon {
	enum class ControllerSettings {
		// Traditional "Nintendo" layout, mapping to the standard buttons that the 3DS/Switch would use.
		// Assumptions:
		// "X" is north and maps to "Rotate Clockwise"
		// "A" is east and maps to "Select"
		NINTENDO,

		// This mode makes the most sense for xbox controller users.
		// Assumptions:
		// "Y" is north and maps to "Rotate Clockwise"
		// "A" is south and maps to "Select" AND the user actually wants to use "A" as select
		XBOX,

		// This mode swaps A and B for an xbox controller
		// Assumptions:
		// "Y" is north and maps to "Rotate Clockwise"
		// "B" is east and maps to "Select" (nintendo style) AND controller prompts will use the text "B" for selecting.
		XBOX_B_SELECT,

		// This mode is primarily intended for when the physical face buttons
		// are swapped in hardware compared to what the controller is reporting.
		// Assumptions:
		// "X" is north, but reported as "Y"
		// "A" is east, but is reported as "B"
		SWAPPED_FACE_BUTTONS,
	};

	// Required functions to implement a SDL2 based platform
	SDL_Window* getWindow(const Platform& platform);
	void initializePlatform(std::string& sdmc, std::string& romfs, bool& backslash, ControllerSettings& controllerSettings);

	// Functions used for keybinds (default implementations in KeybindsSDL2.cpp)
	void setKey(Buttons& buttons, int key, bool pressing);
	void setController(Buttons& buttons, int key, bool pressing, ControllerSettings settings);
	std::string getButtonName(ButtonName buttonName, bool showKbdControls, ControllerSettings settings);
}

#endif //DATA_SDL2_HPP
