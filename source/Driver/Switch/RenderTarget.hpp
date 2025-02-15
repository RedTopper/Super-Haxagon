#ifndef SUPER_HAXAGON_RENDER_TARGET_HPP
#define SUPER_HAXAGON_RENDER_TARGET_HPP

#include "Core/Structs.hpp"

#include <glad/glad.h>

#include <vector>

namespace SuperHaxagon {
	constexpr int BUFFER_RESIZE_STEP = 500;
	constexpr float Z_STEP = 0.00001f;

	struct Vertex {
		Vec2f p;
		Color c;
		float z;
	};

	struct VertexUV {
		Vec2f p;
		Vec2f uv;
		Color c;
		float z;
	};

	class Screen;

	template<class T>
	class RenderTarget {
	public:
		RenderTarget(const Platform& platform, bool transparent, const std::string& shaderVertex, const std::string& shaderFragment, std::string label);
		~RenderTarget();

		void bind() const;
		void insert(const T& vertex);
		void reference(unsigned int index);
		void advance(unsigned int indices);
		void draw(const Platform& platform, int width, int height);
		bool isTransparent() const {return _transparent;}

	private:
		void init(const Platform& platform, const char* shaderVertex, const char* shaderFragment);
		static GLuint compile(const Platform& platform, GLenum type, const char* source);

		const std::string _label;

		bool _transparent;
		unsigned int _iboLastIndex = 0;
		unsigned int _vboBufferSize = 0;
		unsigned int _iboBufferSize = 0;

		std::vector<T> _vertices;
		std::vector<unsigned int> _indices;

		GLuint _program = 0;
		GLuint _tex = 0;
		GLuint _vao = 0;
		GLuint _vbo = 0;
		GLuint _ibo = 0;
	};

	template<class T>
	void resize(const Platform& platform, GLuint type, unsigned int& size, std::vector<T>& vector, const std::string& label);
}

#endif //SUPER_HAXAGON_RENDER_TARGET_HPP
