#include "Driver/NSpire/FontNSpire.hpp"

#include "Core/Structs.hpp"

namespace SuperHaxagon {
	FontNSpire::FontNSpire(int) {

	}

	FontNSpire::~FontNSpire() {

	}

	float FontNSpire::getWidth(const std::string&) const {
		return 1;
	}

	void FontNSpire::draw(const Color&, const Point&, const Alignment, const std::string&) {

	}

	void FontNSpire::setScale(float) {}

	float FontNSpire::getHeight() const {return 4;}
}
