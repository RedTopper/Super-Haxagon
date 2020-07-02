#ifndef SUPER_HAXAGON_STATE_H
#define SUPER_HAXAGON_STATE_H

#include <memory>

namespace SuperHaxagon {
	class State {
		virtual std::unique_ptr<State> next() = 0;
		virtual void update() = 0;
		virtual void draw() = 0;
	};
}

#endif //SUPER_HAXAGON_STATE_H
