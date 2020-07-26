#ifndef SUPER_HAXAGON_METADATA_HPP
#define SUPER_HAXAGON_METADATA_HPP

#include <map>
#include <string>
#include <vector>

namespace SuperHaxagon {
	class Metadata {
	public:
		explicit Metadata(const std::string& path);
		~Metadata();
		Metadata& operator=(const Metadata&) = delete;
		
		bool getMetadata(double time, const std::string& label);

	private:
		double _time;
		std::map<std::string, std::vector<double>> _timestamps;
		std::map<std::string, size_t> _indices;
	};
}

#endif //SUPER_HAXAGON_METADATA_HPP