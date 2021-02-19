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
		
		bool getMetadata(float time, const std::string& label);

	private:
		float _time = 0;
		std::map<std::string, std::vector<float>> _timestamps;
		std::map<std::string, size_t> _indices;
	};
}

#endif //SUPER_HAXAGON_METADATA_HPP