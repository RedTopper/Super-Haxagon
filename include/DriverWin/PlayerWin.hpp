#ifndef SUPER_HAXAGON_PLAYER_WIN_HPP
#define SUPER_HAXAGON_PLAYER_WIN_HPP

#include <memory>
#include <SFML/Audio/SoundSource.hpp>

#include "Driver/Player.hpp"

namespace SuperHaxagon {
	class PlayerWin : public Player {
	public:
		explicit PlayerWin(std::unique_ptr<sf::SoundSource> source);
		~PlayerWin() override;

	private:
		void setChannel(int) override {};
		void setLoop(bool loop) override;

		void play() override;
		bool isDone() override;
		double getVelocity() override;

		std::unique_ptr<sf::SoundSource> _source;
	};
}

#endif //SUPER_HAXAGON_PLAYER_WIN_HPP
