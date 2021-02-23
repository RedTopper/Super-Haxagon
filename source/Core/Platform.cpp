#include "Core/Platform.hpp"

#include <fstream>

namespace SuperHaxagon {
	std::unique_ptr<std::istream> Platform::openFile(const std::string& partial, const Location location) {
		return std::make_unique<std::ifstream>(getPath(partial, location), std::ios::in | std::ios::binary);
	}

	void Platform::stopBGM() {
		_bgm = nullptr;
	}
	
	AudioPlayer* Platform::getBGM() {
		return _bgm.get();
	}

	void Platform::screenSwap() {
		// By default do nothing since most platforms don't have two screens.
	}

	Supports Platform::supports() {
		// By default support everything. Individual platforms can turn features off.
		return Supports::FILESYSTEM | Supports::SHADOWS;
	}
}
