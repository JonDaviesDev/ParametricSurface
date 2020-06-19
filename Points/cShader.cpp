#include "core.h"

#include <windows.h>
#include <direct.h>


#define SHADER_COUNT	1

const char *vShader[] = {	"shaders\\points.vert",	//
};

const char *fShader[] = {	"shaders\\points.frag", //
};


//////////////////////////////////////////////////////////////////////////////////////////
// cShaderInfo() - constructor
//////////////////////////////////////////////////////////////////////////////////////////
cShaderInfo::cShaderInfo()
{
	m_num_shaders = SHADER_COUNT;
	m_pList = NULL;
	m_flags = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
// clean() - delete shader object
//////////////////////////////////////////////////////////////////////////////////////////
void cShader::clean(const char *pVertexShader_path, const char *pFragmentShader_Path)
{
	if( m_v )
		glDeleteShader(m_v); 
	if( m_f )
		glDeleteShader(m_f); 
	if( m_program )
		glDeleteProgram(m_program);
}

//////////////////////////////////////////////////////////////////////////////////////////
// create() - creates a new shader object
//////////////////////////////////////////////////////////////////////////////////////////
void cShader::create(const char *pVertexShader_path, const char *pFragmentShader_Path)
{
	char *vs = NULL, *fs = NULL;

	m_v  = glCreateShader(GL_VERTEX_SHADER);
	m_f  = glCreateShader(GL_FRAGMENT_SHADER);

	_getcwd(m_cwd, _MAX_PATH);
	
	// store vertex path..
	strcpy_s( m_vpath, m_cwd );
	strcat_s( m_vpath, "\\" );
	strcat_s( m_vpath, pVertexShader_path );

	// store shader path..
	strcpy_s( m_fpath, m_cwd );
	strcat_s( m_fpath, "\\" );
	strcat_s( m_fpath, pFragmentShader_Path );

	vs = textFileRead(m_vpath);
	fs = textFileRead(m_fpath);

	if( !vs )
	{
		printf("\n\nERROR: file: %s not found -  ", m_vpath);
		return;
	}
	if( !fs )
	{
		printf("\n\nERROR: file: %s not found -  ", m_fpath);
		return;
	}


	const char *vv = vs;
	const char *ff = fs;

	glShaderSource(m_v, 1, &vv, NULL );
	glShaderSource(m_f, 1, &ff, NULL );

	free(vs);
	free(fs);

	glCompileShader(m_v);
	glCompileShader(m_f);

	m_program = glCreateProgram();
	glAttachShader(m_program, m_v);
	glAttachShader(m_program, m_f); 

	glLinkProgram(m_program);

}


//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////
// create() - allocate block of memory to hold ALL shaders
//////////////////////////////////////////////////////////////////////////////////////////
void cShaderInfo::create()
{
	m_num_shaders = SHADER_COUNT;

	m_pList = new cShader[SHADER_COUNT];	

	for( int i=0; i<SHADER_COUNT; i++ )
	{
		list(i)->create( vShader[i], fShader[i] );	
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// clean() - delete an existing shader object
//////////////////////////////////////////////////////////////////////////////////////////
void cShaderInfo::clean()
{
	m_num_shaders = SHADER_COUNT;

	if( m_pList )
	{
		for( int i=0; i<SHADER_COUNT; i++ )
		{
			list(i)->clean( vShader[i], fShader[i] );	
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// list() - returns requested element from shader list
//////////////////////////////////////////////////////////////////////////////////////////
cShader *cShaderInfo::list(int id)
{
	if( id >= m_num_shaders )
	{
		// if you get here, then you are trying to access a shader element that exceeds the shader array size.
		// try increasing the number_of_shaders created in main() - ShaderInfo.create(number_of_shaders);
		printf("ERROR: ");
		printf("array limits exceeded\n");
	}

	return &m_pList[id];
}

#pragma region Float

void cShader::Dispatch1f(unsigned int shaderID, float value, const std::string& variableName)
{
	glUniform1f(glGetUniformLocation(shaderID, variableName.c_str()), value);
}

void cShader::DispatchVec2f(unsigned int shaderID, glm::vec2 vec2f, const std::string& variableName)
{
	glUniform2f(glGetUniformLocation(shaderID, variableName.c_str()), vec2f.x, vec2f.y);
}

void cShader::DispatchVec3f(unsigned int shaderID, glm::vec3 vec3f, const std::string& variableName)
{
	glUniform3f(glGetUniformLocation(shaderID, variableName.c_str()), vec3f.x, vec3f.y, vec3f.z);
}

void cShader::DispatchVec4f(unsigned int shaderID, glm::vec4 vec4f, const std::string& variableName)
{
	glUniform4f(glGetUniformLocation(shaderID, variableName.c_str()), vec4f.x, vec4f.y, vec4f.z, vec4f.w);
}

#pragma endregion

#pragma region Int

void cShader::DispatchInt(unsigned int shaderID, int value, const std::string& variableName)
{
	glUniform1i(glGetUniformLocation(shaderID, variableName.c_str()), value);
}

void cShader::DispatchInt2(unsigned int shaderID, glm::vec<2, int> value, const std::string& variableName)
{
	glUniform2i(glGetUniformLocation(shaderID, variableName.c_str()), value.x, value.y);
}

void cShader::DispatchInt3(unsigned int shaderID, glm::vec<3, int> value, const std::string& variableName)
{
	glUniform3i(glGetUniformLocation(shaderID, variableName.c_str()), value.x, value.y, value.z);
}

void cShader::DispatchInt4(unsigned int shaderID, glm::vec<4, int> value, const std::string& variableName)
{
	glUniform4i(glGetUniformLocation(shaderID, variableName.c_str()), value.x, value.y, value.z, value.w);
}

#pragma endregion

#pragma region Matrices

void cShader::Dispatch4x4Matrix(unsigned int shaderID, glm::mat4x4 matrix, const std::string& variableName)
{
	glUniformMatrix4fv(glGetUniformLocation(shaderID, variableName.c_str()), 1, GL_FALSE, &matrix[0][0]);
}

#pragma endregion

