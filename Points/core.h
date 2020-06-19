#pragma once



// These libraries are needed to link the program (Visual Studio specific)
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")
#pragma comment(lib,"glfw3.lib")
#pragma comment(lib,"glew32s.lib")
#pragma comment(lib,"glew32.lib")

//#define GLEW_STATIC
#include "GL/glew.h" 
#include "GLFW/glfw3.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

#include "GLM/glm.hpp"
#include "GLM/gtc/matrix_transform.hpp"
#include "GLM/gtc/type_ptr.hpp"
#include "GLM/gtx/dual_quaternion.hpp"

#include <vector>
#include <array>

#include "ShaderManager.h"
#include "Shader.h"


#include <Windows.h>
#include <stdio.h>



#include "textfile.h"
#include "cShader.h"

#define u32 unsigned int

extern bool check_for_opengl_errors();
extern void error_callback(int error, const char* description);

#pragma region Structs

extern struct Vec2
{
	float x, y;

	Vec2() : x(0), y(0) {}

	Vec2(float x, float y) : x(x), y(y) {}

	Vec2 operator+(Vec2 b)
	{
		return Vec2((this->x + b.x), (this->y + b.y));
	}

	Vec2 operator-(Vec2 b)
	{
		return Vec2((this->x - b.x), (this->y - b.y));
	}

	Vec2 operator*(float s)
	{
		return Vec2(s * this->x, s * this->y);
	}
};

extern struct Vec3
{
	float x, y, z;

	Vec3() : x(0), y(0), z(0){}

	Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

	Vec3 operator-(Vec3 b)
	{
		return Vec3((this->x - b.x), (this->y - b.y), (this->z - b.z));
	}

	Vec3 Cross(Vec3 b)
	{
		return Vec3(float((this->y * b.z) - (this->z * b.y)), -float((this->x * b.z) - (this->z * b.x)), float((this->x * b.y) - (this->y * b.x)));
	}

	double Magnitude()
	{
		return glm::sqrt((this->x * this->x) + (this->y * this->y) + (this->z * this->z));
	}

	Vec3 Normalise()
	{
		return Vec3((this->x / this->Magnitude()), (this->y / this->Magnitude()), (this->z / this->Magnitude()));
	}
};

extern struct Vec4
{
	float r, g, b, a;

	Vec4() {}

	Vec4(float red, float green, float blue, float alpha)
	{
		r = red;
		g = green;
		b = blue;
		a = alpha;
	}
};

extern struct Point
{
	unsigned int id;
	Vec3 position;
	Vec3 normal;
	Vec3 colour;
	Vec2 texCoord;

	Point() {}

	Point(float x, float y)
	{
		position.x = x;
		position.y = y;

		colour.x = 1.0f;
		colour.y = 1.0f;
		colour.z = 1.0f;
	}

	Point(float x, float y, float z)
	{
		position.x = x;
		position.y = y;
		position.z = z;

		colour.x = 1.0f;
		colour.y = 1.0f;
		colour.z = 1.0f;
	}

	Point(float x, float y, float z, Vec3 rgb)
	{
		position.x = x;
		position.y = y;
		position.z = z;

		colour.x = rgb.x;
		colour.y = rgb.y;
		colour.z = rgb.z;
	}

	Point(Vec3 coordinates, Vec3 rgb)
	{
		position.x = coordinates.x;
		position.y = coordinates.y;
		position.z = coordinates.z;

		colour.x = rgb.x;
		colour.y = rgb.y;
		colour.z = rgb.z;
	}

	Point(Vec3 coordinates, Vec3 normal, Vec3 rgb)
	{
		position.x = coordinates.x;
		position.y = coordinates.y;
		position.z = coordinates.z;

		this->normal.x = normal.x;
		this->normal.y = normal.y;
		this->normal.z = normal.z;

		colour.x = rgb.x;
		colour.y = rgb.y;
		colour.z = rgb.z;
	}

	bool operator!=(Vec3 external)
	{
		bool x = false;
		bool y = false;
		bool z = false;

		if (this->position.x != external.x)
			x = true;
		if (this->position.y != external.y)
			y = true;
		if (this->position.z != external.z)
			z = true;

		if (x && y && z)
			return true;
		else return false;
	}
};

extern struct Face
{
	Point p1, p2, p3;

	Face(Point p1, Point p2, Point p3)
	{
		this->p1 = p1;
		this->p2 = p2;
		this->p3 = p3;
	}
};

#pragma endregion





extern enum class ActiveResolution
{
	TEN,
	HUNDRED,
	THOUSAND
};

extern enum class Primitive
{
	POINTS,
	LINES,
	LINESTRIP,
	LINEADJ,
	LINELOOP
};

extern enum class Algorithm
{
	BEZIER,
	BSPLINE,
	PATCH
};