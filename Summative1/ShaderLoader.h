/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) [2024] Media Design School
File Name : Assignmen2
Description : GD1P04 - Assignment2
Author : Bach Thanh Duong
Mail : bach.duong@mds.ac.nz
**************************************************************************/
#pragma once

#include <glew.h>
#include <glfw3.h>
#include <iostream>

class ShaderLoader
{

public:
	static GLuint CreateProgram(const char* VertexShaderFilename, const char* FragmentShaderFilename);

private:
	ShaderLoader(void);
	~ShaderLoader(void);
	static GLuint CreateShader(GLenum shaderType, const char* shaderName);
	static std::string ReadShaderFile(const char* filename);
	static void PrintErrorDetails(bool isShader, GLuint id, const char* name);
};
