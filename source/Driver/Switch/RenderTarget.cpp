#include "Driver/Switch/RenderTarget.hpp"

#include "Core/Platform.hpp"

#include <algorithm>

namespace SuperHaxagon {
	template<>
	RenderTarget<Vertex>::RenderTarget(Platform& platform, const bool transparent, const std::string& shaderVertex, const std::string& shaderFragment, std::string label) :
		_label(std::move(label)),
		_transparent(transparent) {
		init(platform, shaderVertex.c_str(), shaderFragment.c_str());

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, p)));
		glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, c)));
		glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, z)));
	}

	template<>
	RenderTarget<VertexUV>::RenderTarget(Platform& platform, const bool transparent, const std::string& shaderVertex, const std::string& shaderFragment, std::string label) :
		_label(std::move(label)),
		_transparent(transparent) {
		init(platform, shaderVertex.c_str(), shaderFragment.c_str());

		glActiveTexture(GL_TEXTURE0);

		glGenTextures(1, &_tex);
		glBindTexture(GL_TEXTURE_2D, _tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(VertexUV), reinterpret_cast<void*>(offsetof(VertexUV, p)));
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexUV), reinterpret_cast<void*>(offsetof(VertexUV, uv)));
		glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(VertexUV), reinterpret_cast<void*>(offsetof(VertexUV, c)));
		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(VertexUV), reinterpret_cast<void*>(offsetof(VertexUV, z)));
	}

	template<class T>
	RenderTarget<T>::~RenderTarget<T>() {
		// ignores zeros, so delete away
		glDeleteBuffers(1, &_vbo);
		glDeleteBuffers(1, &_ibo);
		glDeleteTextures(1, &_tex);
		glDeleteProgram(_program);
	}

	template<class T>
	void RenderTarget<T>::bind() const {
		glUseProgram(_program);
		glBindVertexArray(_vao);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);

		if (_tex) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, _tex);
		}
	}

	template<class T>
	void RenderTarget<T>::insert(const T& vertex) {
		_vertices.push_back(vertex);
	}

	template<class T>
	void RenderTarget<T>::reference(const unsigned int index) {
		_indices.push_back(_iboLastIndex + index);
	}

	template<class T>
	void RenderTarget<T>::advance(const unsigned int indices) {
		_iboLastIndex += indices;
	}

	template<class T>
	void RenderTarget<T>::draw(Platform& platform) {
		if (!_transparent) {
			// Opaque objects need to be rendered front to back, the engine
			// adds all polygons back to front.
			std::reverse(_indices.begin(),_indices.end());
		}

		bind();

		// glUniform ignores all invalid glGetUniformLocation results, so if the shader doesn't
		// have these variables then they will be ignored.
		const auto screen = platform.getScreenDim();
		glUniform1f(glGetUniformLocation(_program, "s_width"), static_cast<float>(screen.x));
		glUniform1f(glGetUniformLocation(_program, "s_height"), static_cast<float>(screen.y));
		glUniform1i(glGetUniformLocation(_program, "f_tex"), 0);

		resize(platform, GL_ARRAY_BUFFER, _vboBufferSize, _vertices, _label + " vertices");
		resize(platform, GL_ELEMENT_ARRAY_BUFFER, _iboBufferSize, _indices, _label + " indices");

		if (_transparent) {
			glEnable(GL_BLEND);
			glDepthMask(GL_FALSE);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}

		glDrawElements(GL_TRIANGLES, static_cast<uint32_t>(_indices.size()), GL_UNSIGNED_INT, nullptr);

		if (_transparent) {
			glDepthMask(GL_TRUE);
			glDisable(GL_BLEND);
		}

		_vertices.clear();
		_indices.clear();
		_iboLastIndex = 0;
	}

	template<class T>
	void RenderTarget<T>::init(Platform& platform, const char* shaderVertex, const char* shaderFragment) {
		const auto vs = compile(platform, GL_VERTEX_SHADER, shaderVertex);
		const auto fs = compile(platform, GL_FRAGMENT_SHADER, shaderFragment);

		_program = glCreateProgram();
		glAttachShader(_program, vs);
		glAttachShader(_program, fs);
		glLinkProgram(_program);
		glUseProgram(_program);

		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);

		glGenBuffers(1, &_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);

		glGenBuffers(1, &_ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
	}

	template<class T>
	GLuint RenderTarget<T>::compile(Platform& platform, const GLenum type, const char* source) {
		GLint success;
		GLchar msg[512];

		const auto handle = glCreateShader(type);
		if (!handle) {
			platform.message(Dbg::INFO, "compile",  "failed to create shader");
			return 0;
		}

		glShaderSource(handle, 1, &source, nullptr);
		glCompileShader(handle);
		glGetShaderiv(handle, GL_COMPILE_STATUS, &success);

		if (!success) {
			glGetShaderInfoLog(handle, sizeof(msg), nullptr, msg);
			platform.message(Dbg::INFO, "compile",  msg);
			glDeleteShader(handle);
			return 0;
		}

		return handle;
	}

	template<class T>
	void resize(Platform& platform, const GLuint type, unsigned int& size, std::vector<T>& vector, const std::string& label) {
		if (size < vector.size() || size == 0) {
			if (size == 0) size = BUFFER_RESIZE_STEP;
			while (size < vector.size()) size *= 2;
			platform.message(Dbg::INFO, "platform", "resized " + label + " to " + std::to_string(size));
			glBufferData(type, size * sizeof(T), vector.data(), GL_DYNAMIC_DRAW);
		} else {
			glBufferSubData(type, 0, size * sizeof(T), vector.data());
		}
	}

	// Repeat after me: I will only ever instantiate the classes here
	template class RenderTarget<Vertex>;
	template class RenderTarget<VertexUV>;
}