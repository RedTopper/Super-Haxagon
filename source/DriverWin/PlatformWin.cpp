#include <string>
#include <algorithm>

#include "DriverWin/AudioWin.hpp"
#include "DriverWin/FontWin.hpp"
#include "DriverWin/PlatformWin.hpp"

namespace SuperHaxagon {
	PlatformWin::PlatformWin() :
		window(
			sf::VideoMode(
				(int)(sf::VideoMode::getDesktopMode().width * 0.75),
				(int)(sf::VideoMode::getDesktopMode().height * 0.75)
			),
			"Super Haxagon",
			sf::Style::Default
		) {

		window.setVerticalSyncEnabled(true);
		loaded = true;
	}

	PlatformWin::~PlatformWin() = default;

	bool PlatformWin::loop() {
		sf::Event event{};
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) window.close();
		}
		return loaded && window.isOpen();
	}

	bool PlatformWin::canUpdate() {
		return false;
	}

	std::string PlatformWin::getPath(const std::string& partial) {
		auto path = std::string("./data/custom") + partial;
		std::replace(path.begin(), path.end(), '/', '\\');
		return path;
	}

	std::string PlatformWin::getPathRom(const std::string& partial) {
		auto path = std::string("./data/rom") + partial;
		std::replace(path.begin(), path.end(), '/', '\\');
		return path;
	}

	std::unique_ptr<Audio> PlatformWin::loadAudio(const std::string& path) {
		return std::make_unique<AudioWin>(path);
	}

	std::unique_ptr<Font> PlatformWin::loadFont(const std::string& path) {
		return std::make_unique<FontWin>(path);
	}

	void PlatformWin::playSFX(Audio& audio) {

	}

	void PlatformWin::playBGM(Audio& audio) {

	}

	void PlatformWin::stopBGM() {

	}

	void PlatformWin::pollButtons() {

	}

	Buttons PlatformWin::getDown() {
		return {};
	}

	Buttons PlatformWin::getPressed() {
		return {};
	}

	Point PlatformWin::getScreenDim() const {
		Point point{};
		point.x = window.getSize().x;
		point.y = window.getSize().y;
		return point;
	}

	void PlatformWin::screenBegin() {

	}

	void PlatformWin::screenSwap() {

	}

	void PlatformWin::screenFinalize() {

	}

	void PlatformWin::drawRect(const Color& color, const Point& point, const Point& size) const {

	}

	void PlatformWin::drawTriangle(const Color& color, const std::array<Point, 3>& points) const {

	}

	std::unique_ptr<Twist> PlatformWin::getTwister() {
		std::random_device source;
		std::mt19937::result_type data[std::mt19937::state_size];
		generate(std::begin(data), std::end(data), ref(source));
		return std::make_unique<Twist>(
				std::make_unique<std::seed_seq>(std::begin(data), std::end(data))
		);
	}
}
