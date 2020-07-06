#ifndef SUPER_HAXAGON_QUIT_HPP
#define SUPER_HAXAGON_QUIT_HPP

#include "States/State.hpp"

namespace SuperHaxagon {
	class Quit : public State {
	public:
		Quit() = default;
		Quit(Quit&) = delete;

		std::unique_ptr<State> update(double dilation) override {return nullptr;}
		void drawTop() override {}
		void drawBot() override {}
	};
}

#endif //SUPER_HAXAGON_QUIT_HPP
