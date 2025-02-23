#ifndef DATA_SDL2_HPP
#define DATA_SDL2_HPP

#include "Driver/Platform.hpp"

#include <SDL.h>

#include <string>

namespace SuperHaxagon {
	// Required functions to implement a SDL2 based platform
	SDL_Window* getWindow(const Platform& platform);
	void setKey(Buttons& buttons, int key, bool pressing);
	void setController(Buttons& buttons, int key, bool pressing);
	std::string getButtonName(ButtonName buttonName, bool showKbdControls);
	void platformSpecificHints();
}

#endif //DATA_SDL2_HPP
