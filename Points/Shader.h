#pragma once

#pragma region Preprocessor Directives

#pragma region System Files

#include <iostream>
#include <fstream>
#include <string>

#pragma endregion

#pragma region Project Files

#include "core.h"

#pragma endregion

#pragma endregion

class Shader
{
#pragma region Attributes

private:
	GLuint id;

#pragma endregion

#pragma region Constructors

public:
	Shader();

	Shader(const char* vertexFilePath, const char* fragmentFilePath, const char* geometryFilePath = "");

	~Shader();

#pragma endregion

#pragma region Properties

public:
	GLint GetID();

#pragma endregion

#pragma region Methods

public:
	void Use();

private:
	std::string LoadShaderFile(const char* filePath);

	GLuint LoadShader(GLenum shaderType, const char* filePath);

	void LinkProgram(GLuint vertexShader, GLuint geometryShader, GLuint fragmentShader);

#pragma endregion

#pragma region Uniform Handling

public:

#pragma region Float

	void Dispatch1f(float value, const std::string& variableName);

	void DispatchVec2f(glm::vec2 vec2f, const std::string& variableName);

	void DispatchVec3f(glm::vec3 vec3f, const std::string& variableName);

	void DispatchVec4f(glm::vec4 vec4f, const std::string& variableName);

#pragma endregion
	
#pragma region Bool

	void DispatchBool(bool value, const std::string& variableName);

#pragma region Int

	void DispatchInt(int value, const std::string& variableName);

	void DispatchInt2(glm::vec<2, int> value, const std::string& variableName);

	void DispatchInt3(glm::vec<3, int> value, const std::string& variableName);

	void DispatchInt4(glm::vec<4, int> value, const std::string& variableName);

#pragma endregion

#pragma region Matrices

	void Dispatch4x4Matrix(glm::mat4x4 matrix, const std::string& variableName);

	void Dispatch4x4Matrix(glm::mat4x4 matrix, glm::mat4x4 transform, const std::string& variableName);

#pragma endregion


#pragma endregion

};

