#include <fstream>
#include <vector>

#include "GLSLProgram.h"
#include "GameEngineErrors.h"
#include "IOManager.h"

namespace GameEngine {
	GLSLProgram::GLSLProgram() : m_programId(0), m_vertexShaderId(0), m_fragmentShaderId(0), m_numAttributes(0)
	{
	}

	GLSLProgram::~GLSLProgram()
	{
	}

	void GLSLProgram::CompileShaders(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath)
	{
		std::string vertSource;
		std::string fragSource;

		IOManager::ReadFileToBuffer(vertexShaderFilePath, vertSource);
		IOManager::ReadFileToBuffer(fragmentShaderFilePath, fragSource);

		CompileShadersFromSource(vertSource.c_str(), fragSource.c_str());
	}

	void GLSLProgram::CompileShadersFromSource(const char* vertexSource, const char* fragmentSource)
	{
		m_programId = glCreateProgram();

		m_vertexShaderId = glCreateShader(GL_VERTEX_SHADER);

		if (m_vertexShaderId == 0)
		{
			FatalError("Vertex shader failed to be created.");
		}

		m_fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

		if (m_fragmentShaderId == 0)
		{
			FatalError("Fragment shader failed to be created.");
		}

		CompileShader(vertexSource, "Vertex Shader", m_vertexShaderId);
		CompileShader(fragmentSource, "Fragment Shader", m_fragmentShaderId);
	}

	void GLSLProgram::LinkShaders()
	{
		glAttachShader(m_programId, m_vertexShaderId);
		glAttachShader(m_programId, m_fragmentShaderId);

		glLinkProgram(m_programId);

		GLint isLinked = 0;
		glGetProgramiv(m_programId, GL_LINK_STATUS, &isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(m_programId, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<char> errorLog(maxLength);
			glGetProgramInfoLog(m_programId, maxLength, &maxLength, &errorLog[0]);

			glDeleteProgram(m_programId);

			glDeleteShader(m_vertexShaderId);
			glDeleteShader(m_fragmentShaderId);

			std::printf("%s\n", &(errorLog[0]));
			FatalError("Shaders failed to compile");
		}

		glDetachShader(m_programId, m_vertexShaderId);
		glDetachShader(m_programId, m_fragmentShaderId);
		glDeleteShader(m_vertexShaderId);
		glDeleteShader(m_fragmentShaderId);
	}

	void GLSLProgram::AddAttribute(const std::string & attributeName)
	{
		glBindAttribLocation(m_programId, m_numAttributes++, attributeName.c_str());
	}

	GLint GLSLProgram::GetUniformLocation(const std::string & uniformName)
	{
		GLint location = glGetUniformLocation(m_programId, uniformName.c_str());
		if (location == GL_INVALID_INDEX)
		{
			FatalError("Uniform " + uniformName + " not found in shader.");
		}
		return location;
	}

	void GLSLProgram::Use()
	{
		glUseProgram(m_programId);

		for (int i = 0; i < m_numAttributes; i++)
		{
			glEnableVertexAttribArray(i);
		}
	}

	void GLSLProgram::UnUse()
	{
		glUseProgram(0);
		for (int i = 0; i < m_numAttributes; i++)
		{
			glDisableVertexAttribArray(i);
		}
	}

	void GLSLProgram::Dispose()
	{
		if (m_programId)
		{
			glDeleteProgram(m_programId);
		}
	}

	void GLSLProgram::CompileShader(const char* source, const std::string& name, GLuint id)
	{
		glShaderSource(id, 1, &source, nullptr);

		glCompileShader(id);

		GLint success = 0;

		glGetShaderiv(id, GL_COMPILE_STATUS, &success);

		if (success == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<char> errorLog(maxLength);
			glGetShaderInfoLog(id, maxLength, &maxLength, &errorLog[0]);

			glDeleteShader(id);

			std::printf("%s\n", &(errorLog[0]));
			FatalError("Shader " + name + " failed to compile");
		}
	}
}