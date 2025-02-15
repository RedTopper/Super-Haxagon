#ifndef SUPER_HAXAGON_SCREEN_HPP
#define SUPER_HAXAGON_SCREEN_HPP

#include "Core/Structs.hpp"

#include <vector>
#include <memory>

namespace SuperHaxagon {
	class Screen {
	public:
		struct ScreenData;
		explicit Screen(std::unique_ptr<ScreenData> data);
		Screen(Screen&) = delete;
		~Screen();

		// Draws a polygon to the screen with absolute coordinates.
		void drawPoly(const Color& color, const std::vector<Vec2f>& points) const;

		Vec2f getScreenDim() const;
		void screenBegin() const;
		void screenFinalize() const;
		void screenSwitch() const;
		void clear(const Color& color) const;

	private:
		std::unique_ptr<ScreenData> _data;
	};
}

#endif //SUPER_HAXAGON_SCREEN_HPP
