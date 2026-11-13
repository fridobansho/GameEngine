#include <cstring>
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
		m_vertexShaderId = 0;
		m_fragmentShaderId = 0;
	}

	void GLSLProgram::AddAttribute(const std::string & attributeName)
	{
		glBindAttribLocation(m_programId, m_numAttributes++, attributeName.c_str());
	}

	GLint GLSLProgram::GetUniformLocation(const std::string & uniformName)
	{
		GLint location = glGetUniformLocation(m_programId, uniformName.c_str());
		if (location == -1)
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
		if (m_programId != 0)
		{
#ifndef __ANDROID__
			if (glDeleteProgram != nullptr)
#endif
			{
				glDeleteProgram(m_programId);
			}
			m_programId = 0;
		}
		m_vertexShaderId = 0;
		m_fragmentShaderId = 0;
		m_numAttributes = 0;
	}

	std::string GLSLProgram::PreprocessShaderSource(const std::string& source, bool targetGLES)
	{
		size_t versionPos = source.find("#version");
		if (targetGLES)
		{
			if (versionPos == std::string::npos)
			{
				if (source.find("precision ") == std::string::npos)
				{
					return "#version 300 es\nprecision mediump float;\n" + source;
				}
				return "#version 300 es\n" + source;
			}

			size_t lineEnd = source.find('\n', versionPos);
			std::string versionLine = (lineEnd == std::string::npos)
				? source.substr(versionPos)
				: source.substr(versionPos, lineEnd - versionPos);
			std::string prefix = source.substr(0, versionPos);
			std::string rest = (lineEnd == std::string::npos) ? "" : source.substr(lineEnd + 1);

			bool alreadyES300 = (versionLine.find("300") != std::string::npos &&
								 versionLine.find("es") != std::string::npos);
			std::string header = alreadyES300 ? "#version 300 es\n" : "#version 300 es\n";
			if (rest.find("precision ") == std::string::npos)
			{
				header += "precision mediump float;\n";
			}
			return prefix + header + rest;
		}
		else
		{
			if (versionPos != std::string::npos)
			{
				size_t lineEnd = source.find('\n', versionPos);
				std::string versionLine = (lineEnd == std::string::npos)
					? source.substr(versionPos)
					: source.substr(versionPos, lineEnd - versionPos);
				if (versionLine.find("300") != std::string::npos &&
					versionLine.find("es") != std::string::npos)
				{
					std::string prefix = source.substr(0, versionPos);
					std::string rest = (lineEnd == std::string::npos) ? "" : source.substr(lineEnd + 1);
					return prefix + "#version 130\n" + rest;
				}
			}
			return source;
		}
	}

	void GLSLProgram::CompileShader(const char* source, const std::string& name, GLuint id)
	{
		bool isGLES = false;
#ifdef __ANDROID__
		isGLES = true;
#else
		if (glGetString != nullptr)
		{
			const GLubyte* versionStr = glGetString(GL_VERSION);
			if (versionStr != nullptr && std::strstr(reinterpret_cast<const char*>(versionStr), "OpenGL ES") != nullptr)
			{
				isGLES = true;
			}
		}
#endif
		std::string processed = PreprocessShaderSource(source != nullptr ? source : "", isGLES);
		const char* processedPtr = processed.c_str();
		glShaderSource(id, 1, &processedPtr, nullptr);

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