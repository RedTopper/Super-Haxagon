#ifndef SUPER_HAXAGON_FONT_3DS_HPP
#define SUPER_HAXAGON_FONT_3DS_HPP

#include <citro2d.h>

#include "Driver/Font.hpp"

namespace SuperHaxagon {
	class Font3DS : public Font {
	public:
		Font3DS(const std::string& path, int size, C2D_TextBuf& buff);
		~Font3DS() override;

		void setScale(double) override {};
		double getHeight() const override;
		double getWidth(const std::string& text) const override;
		void draw(const Color& color, const Point& position, Alignment alignment, const std::string& str) const override;

	private:
		int size;

		C2D_Font font;
		C2D_TextBuf& buff;
	};
}

#endif //SUPER_HAXAGON_FONT_3DS_HPP
