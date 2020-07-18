#ifndef SUPER_HAXAGON_FONT_SWITCH_HPP
#define SUPER_HAXAGON_FONT_SWITCH_HPP

#include "Driver/Font.hpp"

namespace SuperHaxagon {
	class Platform;

	class FontSwitch : public Font {
	public:
		FontSwitch(Platform& platform, const std::string& path, double size);
		~FontSwitch() override;

		void setScale(double scale) override;
		double getHeight() const override;
		double getWidth(const std::string& text) const override;
		void draw(const Color& color, const Point& position, Alignment alignment, const std::string& text) const override;

	private:
		Platform& _platform;

		bool _loaded = false;
		double _scale;
		double _size;
	};
}

#endif //SUPER_HAXAGON_FONT_SWITCH_HPP
