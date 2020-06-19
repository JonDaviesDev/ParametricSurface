#include "Shader.h"

#pragma region Constructors

Shader::Shader()
{
	id = -1;
}

Shader::Shader(const char* vertexFilePath, const char* fragmentFilePath, const char* geometryFilePath)
{
	GLuint vertexShader = 0;		// Initialise vertexShader location to 0
	GLuint fragmentShader = 0;		// Initialise fragmentShader location to 0
	GLuint geometryShader = 0;		// Initialise geometryShader location to 0
	
	vertexShader = LoadShader(GL_VERTEX_SHADER, vertexFilePath);			// Load vertexShader
	fragmentShader = LoadShader(GL_FRAGMENT_SHADER, fragmentFilePath);		// Load fragmentShader
	
	if(geometryFilePath != "")
	{
		geometryShader = LoadShader(GL_GEOMETRY_SHADER, geometryFilePath);		// Load geometryShader
	}
	

	LinkProgram(vertexShader, geometryShader, fragmentShader);		// Link to object

	glDeleteShader(vertexShader);		
	glDeleteShader(fragmentShader);		// Once linked these can be safely deleted
	glDeleteShader(geometryShader);		
}

Shader::~Shader()
{
	glDeleteShader(id);
}

#pragma endregion

#pragma region Properties

GLint Shader::GetID() { return id; }

#pragma endregion

#pragma region Methods

std::string Shader::LoadShaderFile(const char* filePath)
{
	std::string temp;
	std::string source;

	std::ifstream inputFile;

	inputFile.open(filePath);

	if(inputFile)
	{
		while(std::getline(inputFile, temp))
		{
			source += temp + "\n";
		}
	}
	else
	{
		std::cout << "ERROR::SHADER::LOADSHADERFILE::CANNOT_OPEN_FILE: " << filePath << std::endl;
	}

	inputFile.close();

	return source;
}

GLuint Shader::LoadShader(GLenum shaderType, const char* filePath)
{
	GLint success;
	char infoLog[512];

	// Create a shader from the type that is passed into the function (vertex, frag, tess, geo, etc)
	GLuint shader = glCreateShader(shaderType);

	std::string stringSource = LoadShaderFile(filePath);

	// Load the contents of the shaders source file into a char buffer
	const GLchar* source = stringSource.c_str();

	// Bind the newly created shader to the source code
	glShaderSource(shader, 1, &source, NULL);

	// Compile the new shader
	glCompileShader(shader);

	// returns true if the last compile operation on shader was successful, and false otherwise.
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	// if the shader did NOT compile successfully
	if(!success)
	{
		// Get the infolog
		glGetShaderInfoLog(shader, 512, NULL, infoLog);

		// Display shader file name that did not load
		std::cout << "ERROR::LOADSHADER::COULD_NOT_COMPILE_SHADER: " << filePath << std::endl;
		
		// display error
		std::cout << infoLog << std::endl;
	}

	return shader;
}

void Shader::LinkProgram(GLuint vertexShader, GLuint geometryShader, GLuint fragmentShader)
{
	GLint success;
	char infoLog[1024];

	// Create the shader and store location
	this->id = glCreateProgram();

	check_for_opengl_errors();

	// Attach the vertex shader
	glAttachShader(this->id, vertexShader);

	check_for_opengl_errors();

	// If a geometry shader has been passed in, attach it
	if(geometryShader)
	{
		// Attach geometry shader
		glAttachShader(this->id, geometryShader);
	}

	// Attach fragment shader
	glAttachShader(this->id, fragmentShader);

	check_for_opengl_errors();

	// Link the program after attaching all the necessary shaders
	glLinkProgram(this->id);

	check_for_opengl_errors();

	// returns true if the last link operation on shader was successful, and false otherwise.
	glGetShaderiv(vertexShader, GL_LINK_STATUS, &success);
	glGetShaderiv(fragmentShader, GL_LINK_STATUS, &success);

	check_for_opengl_errors();

	//if the shader did NOT link successfully
	if(!success)
	{
		// Get the infolog
		glGetShaderInfoLog(this->id, 512, NULL, infoLog);

		// Display shader file name that did not load
		std::cout << "ERROR::LINKSHADER::COULD_NOT_LINK_SHADER: " << std::endl;

		// display error
		std::cout << infoLog << std::endl;
	}

	// Stop using program
	glUseProgram(0);

	check_for_opengl_errors();

}

void Shader::Use()
{
	glUseProgram(id);
}

#pragma endregion

#pragma region Uniform Handling

#pragma region Float

void Shader::Dispatch1f(float value, const std::string& variableName)
{
	glUniform1f(glGetUniformLocation(id, variableName.c_str()), value);
}

void Shader::DispatchVec2f(glm::vec2 vec2f, const std::string& variableName)
{
	glUniform2f(glGetUniformLocation(id, variableName.c_str()), vec2f.x, vec2f.y);
}

void Shader::DispatchVec3f(glm::vec3 vec3f, const std::string& variableName)
{
	glUniform3f(glGetUniformLocation(id, variableName.c_str()), vec3f.x, vec3f.y, vec3f.z);
}

void Shader::DispatchVec4f(glm::vec4 vec4f, const std::string& variableName)
{
	glUniform4f(glGetUniformLocation(id, variableName.c_str()), vec4f.x, vec4f.y, vec4f.z, vec4f.w);
}

#pragma endregion

#pragma region Int

void Shader::DispatchInt(int value, const std::string& variableName)
{
	glUniform1i(glGetUniformLocation(id, variableName.c_str()), value);
}

void Shader::DispatchInt2(glm::vec<2, int> value, const std::string& variableName)
{
	glUniform2i(glGetUniformLocation(id, variableName.c_str()), value.x, value.y);
}

void Shader::DispatchInt3(glm::vec<3, int> value, const std::string& variableName)
{
	glUniform3i(glGetUniformLocation(id, variableName.c_str()), value.x, value.y, value.z);
}

void Shader::DispatchInt4(glm::vec<4, int> value, const std::string& variableName)
{
	glUniform4i(glGetUniformLocation(id, variableName.c_str()), value.x, value.y, value.z, value.w);
}

#pragma endregion

#pragma region Bool

void Shader::DispatchBool(bool value, const std::string& variableName)
{
	glUniform1i(glGetUniformLocation(id, variableName.c_str()), value);
}

#pragma endregion

#pragma region Matrices

void Shader::Dispatch4x4Matrix(glm::mat4x4 matrix, const std::string& variableName)
{
	glUniformMatrix4fv(glGetUniformLocation(id, variableName.c_str()), 1, GL_FALSE, &matrix[0][0]);
}

#pragma endregion

#pragma endregion

