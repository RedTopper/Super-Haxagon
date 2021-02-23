#ifndef SUPER_HAXAGON_AUDIO_PLAYER_NSPIRE_HPP
#define SUPER_HAXAGON_AUDIO_PLAYER_NSPIRE_HPP

#include "Core/AudioPlayer.hpp"

namespace SuperHaxagon {
	class AudioPlayerNspire : public AudioPlayer {
	public:
		explicit AudioPlayerNspire(float maxTime);
		~AudioPlayerNspire() override = default;

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


#endif //SUPER_HAXAGON_AUDIO_PLAYER_NSPIRE_HPP
