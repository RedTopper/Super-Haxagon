#ifndef SUPER_HAXAGON_MEMORYFS_HPP
#define SUPER_HAXAGON_MEMORYFS_HPP

#include <string>
#include <memory>
#include <iostream>

namespace SuperHaxagon {
	class MemoryFS {
	public:
		static std::unique_ptr<std::istream> openFile(const std::string& partial);
	};
}

#endif
