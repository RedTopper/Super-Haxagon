#ifndef DRIVER_FONT_HPP
#define DRIVER_FONT_HPP

// SPDX-FileCopyrightText: 2025 AJ Walter
// SPDX-License-Identifier: GPL-3.0-or-later OR MIT

#include "Driver/Tools/Color.hpp"
#include "Driver/Tools/Vector.hpp"

#include <memory>
#include <string>

namespace SuperHaxagon {
	enum class Alignment {
		LEFT,
		CENTER,
		RIGHT
	};

	class Platform;

	class Font {
	public:
		struct FontImpl;

		explicit Font(std::unique_ptr<FontImpl> impl);
		Font(Font&) = delete;
		~Font();

		void setScale(float scale);
		float getHeight() const;
		float getWidth(const std::string& str) const;
		void draw(const Color& color, const Vec2f& position, Alignment alignment, const std::string& text) const;

	private:
		std::unique_ptr<FontImpl> _impl;
	};
}

#endif //DRIVER_FONT_HPP
