#ifdef _WIN32
#include "Driver/FontWin.hpp"

namespace SuperHaxagon {
	FontWin::FontWin(const std::string& path) {

	}

	int FontWin::getHeight() const {
		return 0;
	}

	void FontWin::draw(const Color& color, const Point& position, Alignment alignment, std::string text) const {

	}
}

#endif