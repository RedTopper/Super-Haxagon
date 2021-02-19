#include "Core/Metadata.hpp"

#include <fstream>
#include <sstream>

namespace SuperHaxagon {
	Metadata::Metadata(const std::string& path) {
		std::ifstream metadata(path);
		if (!metadata) return;
		
		std::string line;
		while (std::getline(metadata, line)) {
			std::stringstream values(line);
			std::string label;
			float time;

			// time gets repeated by Audacity for the label's
			// duration. We can ignore it.
			values >> time;
			values.ignore(1, '\t');
			values.ignore(256, '\t');
			values >> label;

			_timestamps[label].push_back(time);
		}
	}

	Metadata::~Metadata() = default;

	bool Metadata::getMetadata(const float time, const std::string& label) {
		if (_timestamps.find(label) == _timestamps.end()) return false; // no data

		// If more than 10 seconds behind, reset
		if (time < _time - 10) _indices.clear();
		_time = time;

		auto event = false;
		// While not at end and the current timestamp is less than the requested one
		// Note: C++ crates _indices[label] when it is accessed for the first time
		while (_indices[label] != _timestamps[label].size() && _timestamps[label][_indices[label]] < time) {
			_indices[label]++;
			event = true;
		}
		
		return event;
	}
}
