#ifndef SUPER_HAXAGON_METADATA_HPP
#define SUPER_HAXAGON_METADATA_HPP

// SPDX-FileCopyrightText: 2020 AJ Walter
// SPDX-License-Identifier: GPL-3.0-or-later

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace SuperHaxagon {
	class Metadata {
	public:
		explicit Metadata(std::unique_ptr<std::istream> stream);
		~Metadata();
		Metadata& operator=(const Metadata&) = delete;
		
		bool getMetadata(float time, const std::string& label);

		float getMaxTime();

	private:
		float _time = 0;
		std::map<std::string, std::vector<float>> _timestamps;
		std::map<std::string, size_t> _indices;
	};
}

#endif //SUPER_HAXAGON_METADATA_HPP
