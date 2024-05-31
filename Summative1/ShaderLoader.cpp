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
#include "ShaderLoader.h" 
#include<iostream>
#include<fstream>
#include<vector>

ShaderLoader::ShaderLoader(void) {}
ShaderLoader::~ShaderLoader(void) {}

GLuint ShaderLoader::CreateProgram(const char* vertexShaderFilename, const char* fragmentShaderFilename)
{
	GLuint vertexShader = CreateShader(GL_VERTEX_SHADER, vertexShaderFilename);
	GLuint fragmentShader = CreateShader(GL_FRAGMENT_SHADER, fragmentShaderFilename);

	GLuint program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	// Check for link errors
	int link_result = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &link_result);
	if (link_result == GL_FALSE)
	{
		std::string programName = std::string(vertexShaderFilename) + " " + std::string(fragmentShaderFilename);
		PrintErrorDetails(false, program, programName.c_str());
		return 0;
	}

	return program;
}

GLuint ShaderLoader::CreateShader(GLenum shaderType, const char* shaderName)
{
	std::string shaderCode = ReadShaderFile(shaderName);
	const char* shaderCodeCStr = shaderCode.c_str();

	GLuint shaderID = glCreateShader(shaderType);
	glShaderSource(shaderID, 1, &shaderCodeCStr, NULL);
	glCompileShader(shaderID);

	// Check for compile errors
	int compile_result = 0;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compile_result);
	if (compile_result == GL_FALSE)
	{
		PrintErrorDetails(true, shaderID, shaderName);
		return 0;
	}

	return shaderID;
}

std::string ShaderLoader::ReadShaderFile(const char* filename)
{
	// Open the file for reading
	std::ifstream file(filename, std::ios::in);
	std::string shaderCode;

	// Ensure the file is open and readable
	if (!file.good()) {
		std::cout << "Cannot read file:  " << filename << std::endl;
		return "";
	}

	// Determine the size of of the file in characters and resize the string variable to accomodate
	file.seekg(0, std::ios::end);
	shaderCode.resize((unsigned int)file.tellg());

	// Set the position of the next character to be read back to the beginning
	file.seekg(0, std::ios::beg);
	// Extract the contents of the file and store in the string variable
	file.read(&shaderCode[0], shaderCode.size());
	file.close();
	return shaderCode;
}

void ShaderLoader::PrintErrorDetails(bool isShader, GLuint id, const char* name)
{
	int infoLogLength = 0;
	// Retrieve the length of characters needed to contain the info log
	(isShader == true) ? glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLogLength) : glGetProgramiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);
	std::vector<char> log(infoLogLength);

	// Retrieve the log info and populate log variable
	(isShader == true) ? glGetShaderInfoLog(id, infoLogLength, NULL, &log[0]) : glGetProgramInfoLog(id, infoLogLength, NULL, &log[0]);
	std::cout << "Error compiling " << ((isShader == true) ? "shader" : "program") << ": " << name << std::endl;
	std::cout << &log[0] << std::endl;
}