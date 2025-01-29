#ifndef SUPER_HAXAGON_FONT_SWITCH_HPP
#define SUPER_HAXAGON_FONT_SWITCH_HPP

#include "Core/Font.hpp"

#include "Core/Structs.hpp"
#include "RenderTarget.hpp"

#include <memory>

namespace SuperHaxagon {
	static constexpr int GLYPH_START = 32;
	static constexpr int GLYPH_END = 128;

	struct CharacterInfo {
		Point pxAdvance;
		Point pxOffset;
		Point pxDim;
		Point uv;
		float x;
	};

	class PlatformSwitch;

	class FontSwitch : public Font {
	public:
		FontSwitch(SuperHaxagon::PlatformSwitch& platform, const std::string& path, float size);
		~FontSwitch() override;

		void setScale(float) override {};
		float getHeight() const override;
		float getWidth(const std::string& text) const override;
		void draw(const Color& color, const Point& position, Alignment alignment, const std::string& text) override;

	private:
		PlatformSwitch& _platform;

		bool _loaded = false;
		unsigned int _texWidth;
		unsigned int _texHeight;
		unsigned int _top; // from baseline to top

		std::shared_ptr<RenderTarget<VertexUV>> _surface;

		CharacterInfo _chars[GLYPH_END]{};
	};
}

#endif //SUPER_HAXAGON_FONT_SWITCH_HPP
