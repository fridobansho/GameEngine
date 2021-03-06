#pragma once

#include <string>
#include <GL/glew.h>

namespace GameEngine {
	class GLSLProgram
	{
	public:
		GLSLProgram();
		~GLSLProgram();

		void CompileShaders(const std::string&, const std::string&);
		void LinkShaders();
		void AddAttribute(const std::string&);
		GLint GetUniformLocation(const std::string&);
		void Use();
		void UnUse();

	private:
		void CompileShader(const std::string&, GLuint);

		int m_numAttributes = 0;

		GLuint m_programId = 0;
		GLuint m_vertexShaderId = 0;
		GLuint m_fragmentShaderId = 0;
	};
}

