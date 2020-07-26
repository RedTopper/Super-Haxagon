#ifndef SUPER_HAXAGON_FONT_SFML_HPP
#define SUPER_HAXAGON_FONT_SFML_HPP

#include "Driver/Font.hpp"

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>

namespace SuperHaxagon {
	class PlatformSFML;

	class FontSFML : public Font {
	public:
		FontSFML(PlatformSFML& platform, const std::string& path, double size);
		~FontSFML() override;

		void setScale(double scale) override;
		double getHeight() const override;
		double getWidth(const std::string& text) const override;
		void draw(const Color& color, const Point& position, Alignment alignment, const std::string& text) override;

	private:
		PlatformSFML& _platform;
		sf::Font _font;
		bool _loaded = false;
		double _scale;
		double _size;
	};
}

#endif //SUPER_HAXAGON_FONT_SFML_HPP
