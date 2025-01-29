#ifndef SUPER_HAXAGON_PLATFORM_MACOS_HPP
#define SUPER_HAXAGON_PLATFORM_MACOS_HPP

#include "Driver/SFML/PlatformSFML.hpp"

namespace SuperHaxagon {
	class PlatformMacOS : public PlatformSFML {
	public:
		explicit PlatformMacOS(Dbg dbg);
		PlatformMacOS(PlatformMacOS&) = delete;
		~PlatformMacOS() override = default;

		std::string getPath(const std::string& partial, Location location) override;

		void shutdown() override {};
		void message(Dbg dbg, const std::string& where, const std::string& message) override;

		std::unique_ptr<Twist> getTwister() override;
	};
}

#endif //SUPER_HAXAGON_PLATFORM_MACOS_HPP
