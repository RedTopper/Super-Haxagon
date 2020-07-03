#ifndef SUPER_HAXAGON_OVER_HPP
#define SUPER_HAXAGON_OVER_HPP

#include "State.hpp"

namespace SuperHaxagon {
	class Over : public State {
		std::unique_ptr<State> update() override;

		void draw() override;
	};
}

#endif //SUPER_HAXAGON_OVER_HPP
