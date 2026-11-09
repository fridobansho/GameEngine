#pragma once

#include <string>
#include <GL/glew.h>

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

	private:
		void CompileShader(const char* source, const std::string& name, GLuint id);

		int m_numAttributes = 0;

		GLuint m_programId = 0;
		GLuint m_vertexShaderId = 0;
		GLuint m_fragmentShaderId = 0;
	};
}

