#ifndef SUPER_HAXAGON_FONT_WIN_HPP
#define SUPER_HAXAGON_FONT_WIN_HPP

#include "Driver/Font.hpp"

namespace SuperHaxagon {
	class FontWin : public Font {
	public:
		explicit FontWin(const std::string& path);

		int getHeight() const override;
		void draw(const Color& color, const Point& position, Alignment alignment, std::string text) const override;
	};
}

#endif //SUPER_HAXAGON_FONT_WIN_HPP
