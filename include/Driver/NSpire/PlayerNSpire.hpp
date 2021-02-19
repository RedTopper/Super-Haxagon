#ifndef SUPER_HAXAGON_PLAYER_NSPIRE_HPP
#define SUPER_HAXAGON_PLAYER_NSPIRE_HPP

#include "Driver/Player.hpp"

namespace SuperHaxagon {
	class PlayerNSpire : public Player {
	public:
		explicit PlayerNSpire() {}
		~PlayerNSpire() override {}

		void setChannel(int) override {}
		void setLoop(bool) override {}

		void play() override {}
		void pause() override {}
		bool isDone() const override {return false;}
		double getTime() const override {return 0;}
		double getNow() const {return 0;}
	};
}


#endif //SUPER_HAXAGON_PLAYER_NSPIRE_HPP
