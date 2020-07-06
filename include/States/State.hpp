#ifndef SUPER_HAXAGON_STATE_HPP
#define SUPER_HAXAGON_STATE_HPP

#include <memory>

namespace SuperHaxagon {
	class State {
	public:
		virtual ~State() = default;

		virtual std::unique_ptr<State> update(double dilation) = 0;
		virtual void drawTop(double scale) = 0;
		virtual void drawBot(double scale) = 0;
		virtual void enter() {};
		virtual void exit() {};
	};
}

#endif //SUPER_HAXAGON_STATE_HPP
