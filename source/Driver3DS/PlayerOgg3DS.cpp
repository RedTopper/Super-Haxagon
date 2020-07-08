#include <3ds.h>

#include "Driver3DS/PlayerOgg3DS.hpp"

namespace SuperHaxagon {
	PlayerOgg3DS::PlayerOgg3DS() {
		ndspSetCallback(audioCallback, nullptr);

		int32_t priority = 0x30;
		svcGetThreadPriority(&priority, CUR_THREAD_HANDLE);

		priority -= 1; // Set higher priority and clamp
		priority = priority < 0x18 ? 0x18 : priority;
		priority = priority > 0x3F ? 0x3F : priority;

		// Start the thread, passing our opusFile as an argument.
		_thread = threadCreate(audioThread, this, OP_THREAD_STACK_SZ, priority, OP_THREAD_AFFINITY, false);
	}

	PlayerOgg3DS::~PlayerOgg3DS() {
		// TODO: Stop audio thread
	}

	void PlayerOgg3DS::setChannel(int channel) {

	}

	void PlayerOgg3DS::setLoop(bool loop) {

	}

	void PlayerOgg3DS::play() {

	}

	void PlayerOgg3DS::stop() {

	}

	bool PlayerOgg3DS::isDone() {
		return false;
	}

	void PlayerOgg3DS::audioCallback(void*) {
		LightEvent_Signal(&_event);
	}

	void PlayerOgg3DS::audioThread(void* const self) {
		const auto* pointer = static_cast<PlayerOgg3DS*>(self);
		if (!pointer) return;
		const auto& player = *pointer;
	}
}