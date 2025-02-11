#ifndef MEMORY_FS_HPP
#define MEMORY_FS_HPP

#include <string>
#include <istream>
#include <memory>

namespace SuperHaxagon {
	class MemoryFS {
	public:
		static std::unique_ptr<std::istream> openFile(const std::string& partial);
	};
}

#endif //MEMORY_FS_HPP
