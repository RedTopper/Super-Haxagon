#ifndef SUPER_HAXAGON_QUIT_HPP
#define SUPER_HAXAGON_QUIT_HPP

#include "State.hpp"

namespace SuperHaxagon {
	class Quit : public State {
	public:
		Quit() = default;
		Quit(Quit&) = delete;
		std::unique_ptr<State> update() override {return nullptr;}
		void draw() override {}
	};
}

#endif //SUPER_HAXAGON_QUIT_HPP
