//////////////////////////////////////////////////////////////////////////////////////////
// cShader.h
//////////////////////////////////////////////////////////////////////////////////////////

#pragma once


class cShader
{
public:

	cShader(){}
	~cShader(){}

	inline void			cwd(char *path){ strcpy_s( m_cwd, path );}
	inline char			*cwd(){return m_cwd;}
	
	inline unsigned int	program(){return m_program;}
	inline GLint		get_colTextureLoc(){return m_colTextureLoc;}
	inline GLint		intensity(){return m_intensity;}
	inline GLint		get_effectID() { return m_effectID; }


	void clean(const char *pVertexShader_path, const char *pFragmentShader_Path);
	void create(const char *pVertexShader_path, const char *pFragmentShader_Path);

private:
	char m_vpath[256];
	char m_fpath[256];
	char m_cwd[256];

	GLuint m_v;
	GLuint m_f;

	unsigned int m_program;

	GLint	m_colTextureLoc;
	GLint	m_intensity;
	GLint	m_effectID;

public:
#pragma region Float

	void Dispatch1f(unsigned int shaderID, float value, const std::string& variableName);

	void DispatchVec2f(unsigned int shaderID, glm::vec2 vec2f, const std::string& variableName);

	void DispatchVec3f(unsigned int shaderID, glm::vec3 vec3f, const std::string& variableName);

	void DispatchVec4f(unsigned int shaderID, glm::vec4 vec4f, const std::string& variableName);

#pragma endregion

#pragma region Int

	void DispatchInt(unsigned int shaderID, int value, const std::string& variableName);

	void DispatchInt2(unsigned int shaderID, glm::vec<2, int> value, const std::string& variableName);

	void DispatchInt3(unsigned int shaderID, glm::vec<3, int> value, const std::string& variableName);

	void DispatchInt4(unsigned int shaderID, glm::vec<4, int> value, const std::string& variableName);

#pragma endregion

#pragma region Matrices

	void Dispatch4x4Matrix(unsigned int shaderID, glm::mat4x4 matrix, const std::string& variableName);

	void Dispatch4x4Matrix(unsigned int shaderID, glm::mat4x4 matrix, glm::mat4x4 transform, const std::string& variableName);

#pragma endregion
};


class cShaderInfo
{
public:

	cShaderInfo();
	~cShaderInfo()
	{
	//	delete m_pList;
	}
	void create();//int count);
	void clean();

	inline cShader	*getList(){	return m_pList;}

	cShader *list(int id);

	void set_flags(unsigned int value)
	{
		m_flags = value;
	}

	unsigned int get_flags()
	{
		return m_flags;
	}

	inline void shaderCount(int count){ m_num_shaders = count;}
	inline int  shaderCount(){return m_num_shaders;}
	

private:
	cShader		*m_pList;

	int				m_num_shaders;
	unsigned int	m_flags;		// used to render individual maps
};
