#ifndef SUPER_HAXAGON_FONT_3DS_HPP
#define SUPER_HAXAGON_FONT_3DS_HPP

#include "Driver/Font.hpp"

namespace SuperHaxagon {
	class Font3DS : public Font {
	public:
		explicit Font3DS(const std::string& path);

		int getHeight() const override;
		void draw(const Color& color, const Point& position, Alignment alignment, std::string text) const override;
	};
}

#endif //SUPER_HAXAGON_FONT_3DS_HPP
