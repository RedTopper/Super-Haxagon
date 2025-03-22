// SPDX-FileCopyrightText: 2025 AJ Walter, see driver/SDL2/README.md for MIT usage.
// SPDX-License-Identifier: GPL-3.0-or-later OR MIT

#include "CreateSDL2.hpp"

namespace SuperHaxagon {
	void setKey(Buttons& buttons, int key, bool pressing) {
		switch (key) {
			case SDLK_ESCAPE:
				buttons.back = pressing; return;
			case SDLK_DELETE:
				buttons.quit = pressing; return;
			case SDLK_RETURN:
				buttons.select = pressing; return;
			case SDLK_LEFT:
			case SDLK_a:
				buttons.left = pressing; return;
			case SDLK_RIGHT:
			case SDLK_d:
				buttons.right = pressing; return;
			default:
				return;
		}
	}

	void setController(Buttons& buttons, int key, bool pressing, ControllerSettings settings) {
		if (settings == ControllerSettings::SWAPPED_FACE_BUTTONS || settings == ControllerSettings::XBOX_B_SELECT) {
			switch (key) {
				case SDL_CONTROLLER_BUTTON_B:
					buttons.select = pressing; return;
				case SDL_CONTROLLER_BUTTON_A:
					buttons.back = pressing; return;
				case SDL_CONTROLLER_BUTTON_Y:
					buttons.right = pressing; return;
				case SDL_CONTROLLER_BUTTON_X:
					buttons.left = pressing; return;
				default:
					break;
			}
		}

		if (settings == ControllerSettings::XBOX) {
			switch (key) {
				case SDL_CONTROLLER_BUTTON_Y:
					buttons.right = pressing; return;
				case SDL_CONTROLLER_BUTTON_X:
					buttons.left = pressing; return;
				default:
					break;
			}
		}

		switch (key) {
			case SDL_CONTROLLER_BUTTON_A:
				buttons.select = pressing; return;
			case SDL_CONTROLLER_BUTTON_B:
				buttons.back = pressing; return;
			case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
			case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
			case SDL_CONTROLLER_BUTTON_Y:
				buttons.left = pressing; return;
			case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
			case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
			case SDL_CONTROLLER_BUTTON_X:
				buttons.right = pressing; return;
			case SDL_CONTROLLER_BUTTON_START:
				buttons.quit = pressing; return;
			default: return;
		}
	}

	std::string getButtonName(ButtonName buttonName, bool showKbdControls, ControllerSettings settings) {
		if (showKbdControls) {
			switch (buttonName) {
				case ButtonName::BACK: return "ESC";
				case ButtonName::SELECT: return "ENTER";
				case ButtonName::LEFT: return "LEFT | A";
				case ButtonName::RIGHT: return "RIGHT | D";
				case ButtonName::QUIT: return "DEL";
				default: return "?";
			}
		}

		if (settings == ControllerSettings::XBOX || settings == ControllerSettings::XBOX_B_SELECT) {
			switch (buttonName) {
				case ButtonName::LEFT: return "X | L | PAD L";
				case ButtonName::RIGHT: return "Y | R | PAD R";
				default: break;
			}
		}

		if (settings == ControllerSettings::XBOX_B_SELECT) {
			switch (buttonName) {
				case ButtonName::BACK: return "A";
				case ButtonName::SELECT: return "B";
				default: break;
			}
		}

		switch (buttonName) {
			case ButtonName::BACK: return "B";
			case ButtonName::SELECT: return "A";
			case ButtonName::LEFT: return "Y | L | PAD L";
			case ButtonName::RIGHT: return "X | R | PAD R";
			case ButtonName::QUIT: return "START";
			default: return "?";
		}
	}
}