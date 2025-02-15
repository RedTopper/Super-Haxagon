#include "Driver/Platform.hpp"

#include "Core/Twist.hpp"
#include "Driver/Sound.hpp"
#include "Driver/SFML/DataSFML.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <direct.h>
#include <iostream>
#include <windows.h>

namespace SuperHaxagon {
	Platform::Platform() {
		_plat = createPlatform("./sdmc", "./romfs", false);

		SetForegroundWindow(_plat->window->getSystemHandle());

		_mkdir(".\\sdmc");
	}

	Platform::~Platform() = default;

	void Platform::message(const Dbg dbg, const std::string& where, const std::string& message) const {
		if (dbg == Dbg::INFO) {
			std::cout << "[win:info] " + where + ": " + message << std::endl;
		} else if (dbg == Dbg::WARN) {
			std::cout << "[win:warn] " + where + ": " + message << std::endl;
		} else if (dbg == Dbg::FATAL) {
			std::cerr << "[win:fatal] " + where + ": " + message << std::endl;
			MessageBox(nullptr, (where + ": " + message).c_str(), "A fatal error occurred!", MB_OK);
		}
	}

	std::unique_ptr<Twist> Platform::getTwister() {
		try {
			std::random_device source;
			if (source.entropy() == 0) {
				message(Dbg::INFO, "rng", "entropy too low, using time");
				throw std::runtime_error("lacking entropy");
			}

			std::mt19937::result_type data[std::mt19937::state_size];
			generate(std::begin(data), std::end(data), ref(source));
			return std::make_unique<Twist>(
					std::make_unique<std::seed_seq>(std::begin(data), std::end(data))
			);
		} catch (std::runtime_error&) {
			auto* a = new std::seed_seq{time(nullptr)};
			return std::make_unique<Twist>(
					std::unique_ptr<std::seed_seq>(a)
			);
		}
	}
}