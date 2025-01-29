#ifndef SUPER_HAXAGON_TWIST_HPP
#define SUPER_HAXAGON_TWIST_HPP

#include "Structs.hpp"

#include <algorithm>
#include <memory>
#include <random>

namespace SuperHaxagon {

	/**
	 * Note: This class was taken from
	 * https://github.com/RedTopper/Adventure-Commander
	 */
	class Twist {
	public:
		explicit Twist(const std::unique_ptr<std::seed_seq> seeds) {
			_mt = std::make_unique<std::mt19937>(*seeds);
		}

		/**
		 * Generates a float between [0.0, 1.0)
		 * @return a random float
		 */
		float rand() const {
			return rand(1.0f);
		}

		/**
		 * Generates an int between [0, max]
		 * @param max The maximum integer (Closed)
		 * @return a random int
		 */
		int rand(const int max) const {
			return rand(0, max);
		}

		/**
		 * Generates a float between [0, max)
		 * @param max The maximum float (Open)
		 * @return a random float
		 */
		float rand(const float max) const {
			return rand(0.0, max);
		}

		/**
		 * Generates a random point within the specified
		 * dimension from ( [0, x], [0, y] )
		 * @param max The maximum point (Closed)
		 * @return a random point
		 */
		Point rand(const Point& max) const {
			return rand(Point(), max);
		}

		/**
		 * Generates an int between [min, max]
		 * @param min The minimum integer (Closed)
		 * @param max The maximum integer (Closed)
		 * @return a random int
		 */
		int rand(const int min, const int max) const {
			return std::uniform_int_distribution<>(min, max)(*_mt);
		}

		/**
		 * Generates a float between [min, max)
		 * @param min The minimum float (Closed)
		 * @param max The maximum float (Open)
		 * @return a random float
		 */
		float rand(const float min, const float max) const {
			return static_cast<float>(std::uniform_real_distribution<>(min, max)(*_mt));
		}

		/**
		 * Generates a random point within the specified
		 * dimension from ( [min.x, max.x], [min.y, max.y] )
		 * @param min The minimum point (Closed)
		 * @param max The maximum point (Closed)
		 * @return a random point
		 */
		Point rand(const Point& min, const Point& max) const {
			return Point{rand(min.x, max.x), rand(min.y, max.y)};
		}

		/**
		 * Generates a random number based on geometric
		 * probability. The higher `probability`, the
		 * higher chance to generate lower numbers.
		 * @param probability A geometric probability
		 * @return a random int
		 */
		int geom(const float probability) const {
			return std::geometric_distribution<>(probability)(*_mt);
		}

		/**
		 * Clamps a geometric sequence between min and max
		 * @param probability A geometric probability
		 * @param min Lower bound (Closed)
		 * @param max Upper bound (Closed)
		 * @return a random int
		 */
		int geom(const float probability, const int min, const int max) const {
			return std::min(geom(probability) + min, max);
		}

		/**
		 * Seeds the internal engine
		 * @param str A string, like how Minecraft seeds it's worlds.
		 */
		void seed(const std::string& str) {
			std::seed_seq seed(str.begin(), str.end());
			_mt = std::make_unique<std::mt19937>(seed);
		}

	private:
		std::unique_ptr<std::mt19937> _mt;
	};
}

#endif //SUPER_HAXAGON_TWIST_HPP
