#pragma once

#include <string>
#ifdef __ANDROID__
#include <GLES3/gl3.h>
#else
#include <GL/glew.h>
#endif

namespace GameEngine {
	class GLSLProgram
	{
	public:
		GLSLProgram();
		~GLSLProgram();

		void CompileShaders(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath);
		void CompileShadersFromSource(const char* vertexSource, const char* fragmentSource);
		void LinkShaders();
		void AddAttribute(const std::string& attributeName);
		GLint GetUniformLocation(const std::string& uniformName);
		void Use();
		void UnUse();
		void Dispose();

		static std::string PreprocessShaderSource(const std::string& source, bool targetGLES);

	private:
		void CompileShader(const char* source, const std::string& name, GLuint id);

		int m_numAttributes = 0;

		GLuint m_programId = 0;
		GLuint m_vertexShaderId = 0;
		GLuint m_fragmentShaderId = 0;
	};
}

