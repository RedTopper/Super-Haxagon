#ifndef SUPER_HAXAGON_FONT_3DS_HPP
#define SUPER_HAXAGON_FONT_3DS_HPP

#include "Driver/Font.hpp"

#include <citro2d.h>

namespace SuperHaxagon {
	class Font3DS : public Font {
	public:
		Font3DS(const std::string& path, int size, C2D_TextBuf& buff);
		~Font3DS() override;

		void setScale(float) override {};
		float getHeight() const override;
		float getWidth(const std::string& str) const override;
		void draw(const Color& color, const Point& position, Alignment alignment, const std::string& str) override;

	private:
		float _size;

		C2D_Font _font;
		C2D_TextBuf& _buff;
	};
}

#endif //SUPER_HAXAGON_FONT_3DS_HPP
