#ifndef SUPER_HAXAGON_QUIT_HPP
#define SUPER_HAXAGON_QUIT_HPP

#include "States/State.hpp"

namespace SuperHaxagon {
	class Quit : public State {
	public:
		Quit() = default;
		Quit(Quit&) = delete;
		~Quit() override = default;

		std::unique_ptr<State> update(double) override {return nullptr;}
		void drawTop(double) override {}
		void drawBot(double) override {}
	};
}

#endif //SUPER_HAXAGON_QUIT_HPP
