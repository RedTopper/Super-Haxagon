#ifndef SUPER_HAXAGON_FONT_NSPIRE_HPP
#define SUPER_HAXAGON_FONT_NSPIRE_HPP

#include "Driver/Font.hpp"

namespace SuperHaxagon {
	class Platform;
	class FontNSpire : public Font {
	public:
		FontNSpire(int size);
		~FontNSpire() override;

		void setScale(double) override;
		double getHeight() const override;
		double getWidth(const std::string& text) const override;
		void draw(const Color& color, const Point& position, Alignment alignment, const std::string& text) override;
	};
}

#endif //SUPER_HAXAGON_FONT_NSPIRE_HPP
