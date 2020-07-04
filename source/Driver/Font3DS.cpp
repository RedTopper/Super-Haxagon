#ifdef _3DS
#include "Driver/Font3DS.hpp"

namespace SuperHaxagon {
	Font3DS::Font3DS(const std::string& path) {

	}

	int Font3DS::getHeight() const {
		return 0;
	}

	void Font3DS::draw(const Color& color, const Point& position, Alignment alignment, std::string text) const {

	}
}

#endif
