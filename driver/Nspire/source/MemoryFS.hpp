#ifndef MEMORY_FS_HPP
#define MEMORY_FS_HPP

// SPDX-FileCopyrightText: 2021 AJ Walter, see driver/Nspire/README.md for MIT usage.
// SPDX-License-Identifier: GPL-3.0-or-later OR MIT

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
