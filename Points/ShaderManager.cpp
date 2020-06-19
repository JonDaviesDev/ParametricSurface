#include "ShaderManager.h"
#include "Shader.h"


#pragma region Constructors

ShaderManager::ShaderManager() {}

ShaderManager::~ShaderManager() {}

#pragma endregion

#pragma region Properties

void ShaderManager::SetShaderCount(int value) { shaderCount = value; }

std::vector<Shader> ShaderManager::GetShaderList() { return shaderList; }

int ShaderManager::GetShaderCount() { return shaderCount; }

Shader ShaderManager::GetShader(int ID) 
{
	if(ID > shaderCount)
	{
		std::cout << "ERROR::SHADERMANAGER::GETSHADER::REQUESTED_ID_IS_OUT_OF_VECTOR_RANGE" << std::endl;

		return Shader(0, 0, 0);
	}
	else
	{
		return shaderList[ID];
	}
}

#pragma endregion

#pragma region Methods

void ShaderManager::CreateShader(const char* vertexFileName, const char* fragmentFileName)
{
	shaderList.push_back(Shader(vertexFileName, fragmentFileName));

	shaderCount++;
}

void ShaderManager::Clean()
{
	for(int i = 0; i < shaderCount; i++)
	{
		shaderList[i].~Shader();
	}
}

#pragma endregion
