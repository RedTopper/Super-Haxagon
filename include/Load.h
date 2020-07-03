#ifndef SUPER_HAXAGON_LOAD_H
#define SUPER_HAXAGON_LOAD_H

namespace SuperHaxagon {
	class State;
	class Game;
	class Platform;
	class Load : public State {
	public:
		explicit Load(Game& game);
		Load(Load&) = delete;

		std::unique_ptr<State> update() override;
		void draw() override;

	private:
		Game& game;
		Platform& platform;
	};
}

#endif //SUPER_HAXAGON_LOAD_H
