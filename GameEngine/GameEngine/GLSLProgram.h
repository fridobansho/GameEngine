#pragma once

#include <string>
#include <GL/glew.h>

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

	int _numAttributes;

	GLuint _programId;
	GLuint _vertexShaderId;
	GLuint _fragmentShaderId;
};

