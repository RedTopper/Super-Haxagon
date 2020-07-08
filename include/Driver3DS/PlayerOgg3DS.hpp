#ifndef SUPER_HAXAGON_PLAYER_OGG_3DS_HPP
#define SUPER_HAXAGON_PLAYER_OGG_3DS_HPP

#include <3ds.h>
#include <3ds/synchronization.h>

#include "Driver/Player.hpp"

namespace SuperHaxagon {
	class PlayerOgg3DS : public Player {
	public:
		static constexpr int OP_THREAD_AFFINITY = -1;
		static constexpr int OP_THREAD_STACK_SZ = 32 * 1024;

		PlayerOgg3DS();
		~PlayerOgg3DS() override;

		void setChannel(int channel) override;
		void setLoop(bool loop) override;

		void play() override;
		void stop() override;
		bool isDone() override;

	private:
		static void audioCallback(void*);
		static void audioThread(void*);
		static LightEvent _event;

		Thread _thread{};
	};
}

#endif //SUPER_HAXAGON_PLAYER_OGG_3DS_HPP
