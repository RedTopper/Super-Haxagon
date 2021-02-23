#ifndef SUPER_HAXAGON_PLAYER_NSPIRE_HPP
#define SUPER_HAXAGON_PLAYER_NSPIRE_HPP

#include "Driver/Player.hpp"

namespace SuperHaxagon {
	class PlayerNSpire : public Player {
	public:
		explicit PlayerNSpire(float maxTime);
		~PlayerNSpire() override = default;

		void setChannel(int) override {}
		void setLoop(bool) override {}

		void play() override;
		void pause() override;
		bool isDone() const override;
		float getTime() const override;
		void addTime(float time);

	private:
		float _time{};
		bool _playing = false;
		bool _isDone = true;
		float _maxTime{};
	};
}


#endif //SUPER_HAXAGON_PLAYER_NSPIRE_HPP
