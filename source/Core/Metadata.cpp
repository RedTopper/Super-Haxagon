// SPDX-FileCopyrightText: 2020 AJ Walter
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Metadata.hpp"

#include "Core/Structs.hpp"

#include <sstream>

namespace SuperHaxagon {
	Metadata::Metadata(const std::unique_ptr<std::istream> stream) {
		if (!stream || !*stream) return;
		
		std::string line;
		while (std::getline(*stream, line)) {
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

	bool Metadata::getMetadata(float time, const std::string& label) {
		if (_timestamps.find(label) == _timestamps.end()) return false; // no data

		// Shift the time into the future a bit for pulses (Beat Small and Beat Large)
		// since there's a bit of a "lead up" animation to the actual beat.
		if (label == "BS" || label == "BL") {
			time += (FRAMES_PER_PULSE_LEAD_UP/2.0f) / 60.0f;
		}

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

	float Metadata::getMaxTime() {
		auto max = 0.0f;
		for (const auto& timestamp : _timestamps) {
			for (auto time : timestamp.second) {
				if (time > max) {
					max = time;
				}
			}
		}

		return max;
	}
}
