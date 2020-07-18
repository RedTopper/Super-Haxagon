#ifndef SUPER_HAXAGON_PLATFORM_WIN_HPP
#define SUPER_HAXAGON_PLATFORM_WIN_HPP

#include <Driver/SFML/PlatformSFML.hpp>

namespace SuperHaxagon {
	class PlatformWin : public PlatformSFML {
	public:
		explicit PlatformWin(Dbg dbg);
		PlatformWin(PlatformWin&) = delete;
		~PlatformWin() override = default;

		std::string getPath(const std::string& partial) override;
		std::string getPathRom(const std::string& partial) override;

		void shutdown() override {};
		void message(Dbg dbg, const std::string& where, const std::string& message) override;

		std::unique_ptr<Twist> getTwister() override;
	};
}

#endif //SUPER_HAXAGON_PLATFORM_WIN_HPP
