#ifndef SUPER_HAXAGON_STATE_HPP
#define SUPER_HAXAGON_STATE_HPP

#include <memory>

namespace SuperHaxagon {
	class State {
		virtual std::unique_ptr<State> update() = 0;
		virtual void draw() = 0;
		virtual void enter() {};
		virtual void exit() {};
	};
}

#endif //SUPER_HAXAGON_STATE_HPP
