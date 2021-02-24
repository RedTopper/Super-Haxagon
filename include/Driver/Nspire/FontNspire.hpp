#ifndef SUPER_HAXAGON_FONT_NSPIRE_HPP
#define SUPER_HAXAGON_FONT_NSPIRE_HPP

#include "Core/Font.hpp"

#include <memory>
#include <ngc.h>

namespace SuperHaxagon {
	class Platform;
	class FontNspire : public Font {
	public:
		FontNspire(Gc& gc, int size);
		~FontNspire() override;

		void setScale(float) override;
		float getHeight() const override;
		float getWidth(const std::string& text) const override;
		void draw(const Color& color, const Point& position, Alignment alignment, const std::string& text) override;
		std::unique_ptr<char[]> utf16(const std::string& text) const;

	private:
		gui_gc_Font _font{};
		Gc& _gc;
	};
}

#endif //SUPER_HAXAGON_FONT_NSPIRE_HPP
