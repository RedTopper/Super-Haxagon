#ifndef SUPER_HAXAGON_PLATFORM_LINUX_HPP
#define SUPER_HAXAGON_PLATFORM_LINUX_HPP

#include "Driver/SFML/PlatformSFML.hpp"

namespace SuperHaxagon {
	class PlatformLinux : public PlatformSFML {
	public:
		explicit PlatformLinux(Dbg dbg);
		PlatformLinux(PlatformLinux&) = delete;
		~PlatformLinux() override = default;

		std::string getPath(const std::string& partial, Location location) override;

		void shutdown() override {};
		void message(Dbg dbg, const std::string& where, const std::string& message) override;

		std::unique_ptr<Twist> getTwister() override;

		std::string _sdmc;
		std::string _romfs;
	};
}

#endif //SUPER_HAXAGON_PLATFORM_LINUX_HPP
