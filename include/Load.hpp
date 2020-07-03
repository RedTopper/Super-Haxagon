#ifndef SUPER_HAXAGON_LOAD_HPP
#define SUPER_HAXAGON_LOAD_HPP

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

#endif //SUPER_HAXAGON_LOAD_HPP
