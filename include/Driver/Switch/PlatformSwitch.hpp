#ifndef SUPER_HAXAGON_PLATFORM_SWITCH_HPP
#define SUPER_HAXAGON_PLATFORM_SWITCH_HPP

#include <fstream>

#include "Driver/SFML/PlatformSFML.hpp"

namespace SuperHaxagon {
	class PlatformSwitch : public PlatformSFML {
	public:
		explicit PlatformSwitch(const Dbg dbg);
		PlatformSwitch(PlatformSwitch&) = delete;
		~PlatformSwitch() = default;

		std::string getPath(const std::string& partial) override;
		std::string getPathRom(const std::string& partial) override;
		bool loop() override;
		std::unique_ptr<Twist> getTwister() override;
		void show() override;
		void message(Dbg dbg, const std::string& where, const std::string& message) override;

	private:
		std::ofstream _console;
		std::deque<std::pair<Dbg, std::string>> _messages{};
	};
}

#endif //SUPER_HAXAGON_PLATFORM_SWITCH_HPP
