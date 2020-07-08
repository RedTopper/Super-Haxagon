#ifndef SUPER_HAXAGON_FONT_WIN_HPP
#define SUPER_HAXAGON_FONT_WIN_HPP

#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>

#include "Driver/Font.hpp"

namespace SuperHaxagon {
	class PlatformWin;

	class FontWin : public Font {
	public:
		FontWin(PlatformWin& platform, const std::string& path, double size);
		~FontWin() override;

		void setScale(double scale) override;
		double getHeight() const override;
		double getWidth(const std::string& text) const override;
		void draw(const Color& color, const Point& position, Alignment alignment, const std::string& text) const override;

	private:
		PlatformWin& _platform;
		sf::Font _font;
		bool _loaded = false;
		double _scale;
		double _size;
	};
}

#endif //SUPER_HAXAGON_FONT_WIN_HPP
