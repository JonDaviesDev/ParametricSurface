#pragma once
#include "core.h"

class Shader;

class ShaderManager
{
#pragma region Attributes

private:
	std::vector<Shader> shaderList;

	 int shaderCount;

#pragma endregion

#pragma region Constructors

public:
	ShaderManager();

	~ShaderManager();

#pragma endregion

#pragma region Properties

public:

#pragma region Setters

	void SetShaderCount(int value);

#pragma endregion

#pragma region Getters

	std::vector<Shader> GetShaderList();

	int GetShaderCount();

	Shader GetShader(int ID);

#pragma endregion

#pragma endregion

#pragma region Methods

public:
	void CreateShader(const char* vertexFileName, const char* fragmentFileName);

	void Clean();

#pragma endregion
};

