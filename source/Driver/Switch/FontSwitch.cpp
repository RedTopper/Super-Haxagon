#include "Driver/Switch/FontSwitch.hpp"

namespace SuperHaxagon {
	FontSwitch::FontSwitch(Platform& platform, const std::string& path, const double size) :
		_platform(platform),
		_scale(1),
		_size(size) {
		_loaded = false;
	}

	FontSwitch::~FontSwitch() = default;

	void FontSwitch::setScale(const double scale) {
		// scale up at half rate so it looks nice I guess?
		_scale = (scale - 1) / 2 + 1;
	}

	double FontSwitch::getHeight() const {
		return _size * _scale;
	}

	double FontSwitch::getWidth(const std::string& text) const {
		return text.length() * _size * _scale;
	}

	void FontSwitch::draw(const Color& color, const Point& position, Alignment alignment, const std::string& text) const {

	}
}
