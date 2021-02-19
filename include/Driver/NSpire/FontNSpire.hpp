#ifndef SUPER_HAXAGON_FONT_NSPIRE_HPP
#define SUPER_HAXAGON_FONT_NSPIRE_HPP

#include "Driver/Font.hpp"

#include "Core/Structs.hpp"

namespace SuperHaxagon {
	class FontNSpire : public Font {
	public:
		FontNSpire();
		~FontNSpire() override;

		void setScale(double) override {};
		double getHeight() const override {return 4;}
		double getWidth(const std::string& text) const override;
		void draw(const Color& color, const Point& position, Alignment alignment, const std::string& text) override;
	};
}

#endif //SUPER_HAXAGON_FONT_NSPIRE_HPP
