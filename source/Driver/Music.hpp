#ifndef SUPER_HAXAGON_AUDIO_PLAYER_MUSIC_HPP
#define SUPER_HAXAGON_AUDIO_PLAYER_MUSIC_HPP

#include <memory>

namespace SuperHaxagon {


	class Music {
	public:
		struct MusicData;

		explicit Music(std::unique_ptr<MusicData> data);
		Music(Music&) = delete;
		~Music();

		// Some platforms need to update their state
		// every frame. Call this function to do that.
		void update() const;

		void setLoop(bool loop) const;
		void play() const;
		void pause() const;
		bool isDone() const;
		float getTime() const;

	private:
		std::unique_ptr<MusicData> _data;
	};
}

#endif //SUPER_HAXAGON_AUDIO_PLAYER_MUSIC_HPP
