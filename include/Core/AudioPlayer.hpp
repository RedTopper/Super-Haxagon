#ifndef SUPER_HAXAGON_AUDIO_PLAYER_HPP
#define SUPER_HAXAGON_AUDIO_PLAYER_HPP

namespace SuperHaxagon {
	class AudioPlayer {
	public:
		AudioPlayer() = default;
		AudioPlayer(AudioPlayer&) = delete;
		virtual ~AudioPlayer() = default;

		virtual void setChannel(int channel) = 0;
		virtual void setLoop(bool loop) = 0;
		virtual void play() = 0;
		virtual void pause() = 0;
		virtual bool isDone() const = 0;
		virtual float getTime() const = 0;
	};
}

#endif //SUPER_HAXAGON_AUDIO_PLAYER_HPP
