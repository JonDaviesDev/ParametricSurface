#include "core.h"
#include "CamSetup.h"

//////////////////////////////////////////////////////////////////////////
//------README----------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////

/*

ON APPLICATION RUN - 

	A control window inside the regular display window will appear.
	This control window can be scrolled through to access more user available functionality.

	- It will allow you to move control point positions by using the x, y, z sliders for each point.

	- It will also allow you to activate features like triangulation, wireframes resolution changes and texture application.

	CAMERA CONTROLS

	- The user can move the camera by HOLDING THE RIGHT MOUSE BUTTON whilst moving the mouse around
		(There is a bug which occurs when the RMB is let go and then re-used - the camera tries to look at the last thing
		it was looking at when the RMB was released)

	- MOVEMENT - use WASD for normal movement
			   - use Q and E for height panning


	To properly see the texture, make sure that a resolution of 100 is active (this coupled with an active triangulation works best
*/



#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#pragma region Globals

const int NumObjects = 3;
int objID[NumObjects];

unsigned int myVBO[NumObjects];

unsigned int myVAO[NumObjects];

// Buffer Locations
const unsigned int curvePositionLocation = 0;  
const unsigned int curveColourLocation = 1; 
const unsigned int curveNormalLocation = 2;
const unsigned int curveTexCoordsLocation = 3;
const unsigned int controlPositionLocation = 4;
const unsigned int controlColourLocation = 5;

// Quick reference colours
Vec3 red = {1.0f, 0.0f, 0.0f};
Vec3 green = {0.0f, 1.0f, 0.0f};
Vec3 blue = {0.0f, 0.0f, 1.0f};

// Screen dimensions
unsigned int initWidth = 2000;
unsigned int initHeight = 1500;

// Enum States
Algorithm algorithm = Algorithm::BSPLINE;
ActiveResolution resolution = ActiveResolution::TEN;
Primitive controlPoints = Primitive::POINTS;
Primitive curvePoints = Primitive::POINTS;

// Quick reference resolution sizes
const int res10 = 10;
const int res100 = 100;
const int res1000 = 1000;

const int controlPointCount = 16;

// Manage all shaders created in the scene
ShaderManager shaderList;

// ImGUI Window setup
bool show_demo_window = true;
bool show_another_window = false;
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

// User options - used by ImGUI
bool activateWireFrame = false;
bool triangulation = false;
bool addTexture = false;

// VAO/VBO initialisation
unsigned int vao;
unsigned int curveVBO; 
unsigned int lightVBO;
unsigned int controlPointsVBO;
unsigned int controlPointsEBO;
unsigned int curvePointsEBO;

// stb_image initialisation data
unsigned int texture;
int width, height, channels;
unsigned char* data = nullptr;

#pragma endregion

#pragma region Point Setup

Point pointList[res100];			// Startup number of points (10x10)
std::vector<Point> pointVector;		// List to hold all generated points throughout the scene
std::vector<Point> tempPosition;	// Used by the BSpline algorithm to store U values

std::vector<std::vector<Point>> pointIndices;	// Stores the indices of the surface in a 2D grid
std::vector<Face> faceList;						// Stores each FACE in the surface

Point light = { Vec3(0.0f, 0.0f, 1.0f), Vec3(1.0f, 1.0f, 1.0f) };	// Start up light position and colour

#pragma region BSpline Setup

// Bezier point setup in case the user wants to switch to a different curve type

Point BezierPoint1(-0.9f, 0.2f, 0.0f, red);
Point BezierPoint2(-0.2f, -0.6f, 0.0f, red);
Point BezierPoint3(0.4f, 0.8f, 0.0f, red);
Point BezierPoint4(0.9f, -0.2f, 0.0f, red);

Point BSplineList[4] = {BezierPoint1, BezierPoint2, BezierPoint3, BezierPoint4};

#pragma endregion

#pragma region Patch Setup

// Control Points for the surface patch

Point controlPoint1 = {-0.3f, 0.3f, 0.0f, red};	//Top Row
Point controlPoint2 = {0.0f, 0.3f, 0.0f, red};	//Top Row
Point controlPoint3 = {0.3f, 0.3f, 0.0f, red};	//Top Row
Point controlPoint4 = {0.6f, 0.3f, 0.0f, red};	//Top Row

Point controlPoint5 = {-0.3f, 0.0f, 0.0f, red};	//Upper Middle
Point controlPoint6 = {0.0f, 0.0f, 0.0f, red};	//Upper Middle
Point controlPoint7 = {0.3f, 0.0f, 0.0f, red};	//Upper Middle
Point controlPoint8 = {0.6f, 0.0f, 0.0f, red};	//Upper Middle

Point controlPoint9 = {-0.3f, -0.3f, 0.0f, red};	//Lower Middle
Point controlPoint10 = {0.0f, -0.3f, 0.0f, red};	//Lower Middle
Point controlPoint11 = {0.3f, -0.3f, 0.0f, red};	//Lower Middle
Point controlPoint12 = {0.6f, -0.3f, 0.0f, red};	//Lower Middle

Point controlPoint13 = {-0.3f, -0.6f, 0.0f, red};	//Bottom Row
Point controlPoint14 = {0.0f, -0.6f, 0.0f, red};	//Bottom Row
Point controlPoint15 = {0.3f, -0.6f, 0.0f, red};	//Bottom Row
Point controlPoint16 = {0.6f, -0.6f, 0.0f, red};	//Bottom Row



// These points have been ordered into a 2D array

Point controlList[4][4] =
{
	{controlPoint1, controlPoint2, controlPoint3, controlPoint4},
	{controlPoint5, controlPoint6, controlPoint7, controlPoint8},
	{controlPoint9, controlPoint10, controlPoint11, controlPoint12},
	{controlPoint13, controlPoint14, controlPoint15, controlPoint16}
};


// Indices array for the control points
unsigned int controlPointIndices[] = {

	// First Face
	0, 1, 4,
	1, 5, 4,

	// Second Face
	1, 2, 5,
	2, 6, 5, 

	// Third Face
	2, 3, 6,
	3, 7, 6, 

	// Fourth Face
	4, 5, 8, 
	5, 9, 8, 

	// Fifth Face
	5, 6, 9, 
	6, 10, 9,

	// Sixth Face
	6, 7, 10,
	7, 11, 10,

	// Seventh Face
	8, 9, 12, 
	9, 13, 12,

	// Eighth Face
	9, 10, 13,
	10, 13, 14,

	// Ninth Face
	10, 11, 14,
	11, 15, 14

};

// Indices list for the curve points
std::vector<unsigned int> curvePointIndices;

#pragma endregion

#pragma endregion

#pragma region Function Prototypes

void Hermite(Point p1, Point p2, Point tang1, Point tang2);

void CalculateHermite(float t, int element, Point p1, Point p2, Point tang1, Point tang2);

void CalculateBezier(float t, int element, Point p1, Point p2, Point p3, Point p4);

void Bezier(Point p1, Point p2, Point p3, Point p4, int resolution);

void CalculateBSpline(float t, int element, Point p1, Point p2, Point p3, Point p4);

void BSpline(Point p1, Point p2, Point p3, Point p4, int resolution);

Point U(float t, int row);
Point V(float t, const std::vector<Point>& points);
Point Calc(float u, float v);
void Loop(int resolution);

int SetResolutionStatus(ActiveResolution resolution);

int SetPrimitiveType(Primitive newPrimitive);

float B0(float t);
float B1(float t);
float B2(float t);
float B3(float t);


#pragma region Setup

void VectorToVector2D();

void Triangulate();

void PullPointsOutOfFaceList(const std::vector<Face>& faceList);

void FindNormals();

void init_SceneData();

void mySetupGeometries();

void resize(GLFWwindow* window, int width, int height);

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods);

void printDebugInfo();

void renderScene();

#pragma endregion

#pragma endregion

void renderScene()
{	

#pragma region Render Starting Tasks

	// Clear the rendering window
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ImGui_ImplGlfwGL3_NewFrame();

	faceList.clear();

#pragma endregion

#pragma region Shader Setup

	glm::vec3 lightpositionGLM(light.position.x, light.position.y, light.position.z);

	// Light Shader
	shaderList.GetShader(2).Use();
	shaderList.GetShader(2).DispatchVec3f(glm::vec3(1.0f, 0.0f, 0.0f), "lightColour");
	shaderList.GetShader(2).DispatchVec3f(camera.position, "viewPosition");
	glm::mat4 model = glm::mat4(1.0f);
	shaderList.GetShader(2).DispatchVec3f(lightpositionGLM, "lightPosition");
	shaderList.GetShader(2).Dispatch4x4Matrix(model, "modelMatrix");

	// Point Shader
	shaderList.GetShader(0).Use();
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)initWidth / (float)initHeight, 0.1f, 200.0f);
	shaderList.GetShader(0).Dispatch4x4Matrix(projection, "projection");
	glm::mat4 view = camera.GetViewMatrix();
	shaderList.GetShader(0).Dispatch4x4Matrix(view, "view");
	model = glm::mat4(1.0f);
	shaderList.GetShader(0).Dispatch4x4Matrix(model, "model");

#pragma endregion

#pragma region Render - Surface Points

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, curveVBO);
	glPointSize(5);
	Loop(SetResolutionStatus(resolution));
	glBufferData(GL_ARRAY_BUFFER, pointVector.size() * sizeof(Point), pointVector.data(), GL_DYNAMIC_DRAW);
	glDrawArrays(SetPrimitiveType(curvePoints), 0, pointVector.size());

#pragma endregion

#pragma region IF - Triangulated

	if (triangulation)
	{
		VectorToVector2D();
		Triangulate();
		PullPointsOutOfFaceList(faceList);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, curvePointsEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, curvePointIndices.size() * sizeof(float), curvePointIndices.data(), GL_DYNAMIC_DRAW);

		glLineWidth(2.0f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawElements(GL_TRIANGLES, curvePointIndices.size(), GL_UNSIGNED_INT, 0);
	}
	else
	{
		faceList.clear();
	}


#pragma endregion

#pragma region IF - Textured

	if (addTexture)
	{
		shaderList.GetShader(0).Use();
		shaderList.GetShader(0).DispatchBool(true, "textureCheck");

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, curvePointsEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, curvePointIndices.size() * sizeof(float), curvePointIndices.data(), GL_DYNAMIC_DRAW);

		glBindTexture(GL_TEXTURE_2D, texture);
		glDrawElements(GL_TRIANGLES, curvePointIndices.size(), GL_UNSIGNED_INT, 0);
	}
	else
	{
		faceList.clear();
		shaderList.GetShader(0).Use();
		shaderList.GetShader(0).DispatchBool(false, "textureCheck");
	}

#pragma endregion

#pragma region Find Normals

	FindNormals();

	glBindBuffer(GL_VERTEX_ARRAY, curveVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, pointVector.size() * sizeof(float), pointVector.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(curveNormalLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, Point::normal));
	glEnableVertexAttribArray(curveNormalLocation);

#pragma endregion

#pragma region ImGui

	// Control Point Shader
	shaderList.GetShader(1).Use();
	shaderList.GetShader(1).Dispatch4x4Matrix(projection, "projection");
	shaderList.GetShader(1).Dispatch4x4Matrix(view, "view");
	shaderList.GetShader(1).Dispatch4x4Matrix(model, "model");

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, controlPointsVBO);
	
	ImGui::Begin("Menu System");
	ImGui::SetWindowFontScale(2.0f);	// use for 4K display
	ImGui::Text("Point Controller");                 

#pragma region Control Points

	ImGui::SliderFloat3("CP 1 - (X, Y, Z)",  &controlPoint1.position.x,  -1.0f, 1.0f);
	ImGui::SliderFloat3("CP 2 - (X, Y, Z)",  &controlPoint2.position.x,  -1.0f, 1.0f);
	ImGui::SliderFloat3("CP 3 - (X, Y, Z)",  &controlPoint3.position.x,  -1.0f, 1.0f);
	ImGui::SliderFloat3("CP 4 - (X, Y, Z)",  &controlPoint4.position.x,  -1.0f, 1.0f);
	ImGui::SliderFloat3("CP 5 - (X, Y, Z)",  &controlPoint5.position.x,  -1.0f, 1.0f);
	ImGui::SliderFloat3("CP 6 - (X, Y, Z)",  &controlPoint6.position.x,  -1.0f, 1.0f);
	ImGui::SliderFloat3("CP 7 - (X, Y, Z)",  &controlPoint7.position.x,  -1.0f, 1.0f);
	ImGui::SliderFloat3("CP 8 - (X, Y, Z)",  &controlPoint8.position.x,  -1.0f, 1.0f);
	ImGui::SliderFloat3("CP 9 - (X, Y, Z)",  &controlPoint9.position.x,  -1.0f, 1.0f);
	ImGui::SliderFloat3("CP 10 - (X, Y, Z)", &controlPoint10.position.x, -1.0f, 1.0f);
	ImGui::SliderFloat3("CP 11 - (X, Y, Z)", &controlPoint11.position.x, -1.0f, 1.0f);
	ImGui::SliderFloat3("CP 12 - (X, Y, Z)", &controlPoint12.position.x, -1.0f, 1.0f);
	ImGui::SliderFloat3("CP 13 - (X, Y, Z)", &controlPoint13.position.x, -1.0f, 1.0f);
	ImGui::SliderFloat3("CP 14 - (X, Y, Z)", &controlPoint14.position.x, -1.0f, 1.0f);
	ImGui::SliderFloat3("CP 15 - (X, Y, Z)", &controlPoint15.position.x, -1.0f, 1.0f);
	ImGui::SliderFloat3("CP 16 - (X, Y, Z)", &controlPoint16.position.x, -1.0f, 1.0f);

	glBufferData(GL_ARRAY_BUFFER, sizeof(controlList), controlList, GL_DYNAMIC_DRAW);

	controlList[0][0] = controlPoint1;
	controlList[0][1] = controlPoint2;
	controlList[0][2] = controlPoint3;
	controlList[0][3] = controlPoint4;
	controlList[1][0] = controlPoint5;
	controlList[1][1] = controlPoint6;
	controlList[1][2] = controlPoint7;
	controlList[1][3] = controlPoint8;
	controlList[2][0] = controlPoint9;
	controlList[2][1] = controlPoint10;
	controlList[2][2] = controlPoint11;
	controlList[2][3] = controlPoint12;
	controlList[3][0] = controlPoint13;
	controlList[3][1] = controlPoint14;
	controlList[3][2] = controlPoint15;
	controlList[3][3] = controlPoint16;

	glPointSize(10);
	int vertexCount = sizeof(controlList) / sizeof(Point);
	glBindVertexArray(controlPointsVBO);
	glDrawArrays(SetPrimitiveType(controlPoints), 0, vertexCount);

	if (activateWireFrame)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, controlPointsEBO);

		glLineWidth(2.0f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawElements(GL_TRIANGLES, sizeof(controlPointIndices), GL_UNSIGNED_INT, 0);
	}



#pragma endregion

#pragma region Set Primitive - ImGui

	ImGui::Text("Control: Primitive Type - ");
	ImGui::SameLine();
	if(ImGui::Button("Points##"))
	{
		controlPoints = Primitive::POINTS;
	}
	ImGui::SameLine();
	if(ImGui::Button("Lines##"))
	{
		controlPoints = Primitive::LINES;
	}
	ImGui::SameLine();
	if(ImGui::Button("LineStrip##"))
	{
		controlPoints = Primitive::LINESTRIP;
	}
	ImGui::SameLine();
	if(ImGui::Button("LineAdj##"))
	{
		controlPoints = Primitive::LINEADJ;
	}
	ImGui::SameLine();
	if(ImGui::Button("LineLoop##"))
	{
		controlPoints = Primitive::LINELOOP;
	}

	ImGui::Text("Points: Primitive Type - ");
	ImGui::SameLine();
	if(ImGui::Button("Points"))
	{
		curvePoints = Primitive::POINTS;
	}
	ImGui::SameLine();
	if(ImGui::Button("Lines"))
	{
		curvePoints = Primitive::LINES;
	}
	ImGui::SameLine();
	if(ImGui::Button("LineStrip"))
	{
		curvePoints = Primitive::LINESTRIP;
	}
	ImGui::SameLine();
	if(ImGui::Button("LineAdj"))
	{
		curvePoints = Primitive::LINEADJ;
	}
	ImGui::SameLine();
	if(ImGui::Button("LineLoop"))
	{
		curvePoints = Primitive::LINELOOP;
	}

#pragma endregion

#pragma region Set Resolution - ImGui

	if(ImGui::Button("Resolution - 10"))
	{
		resolution = ActiveResolution::TEN;
	}
	ImGui::SameLine();
	if(ImGui::Button("Resolution - 100"))
	{
		resolution = ActiveResolution::HUNDRED;
	}
	ImGui::SameLine();

	if (ImGui::Button("Toggle Wire Frame"))
	{
		if (activateWireFrame == true)
		{
			activateWireFrame = false;
		}
		else
		{
			activateWireFrame = true;
		}
	}

	if (ImGui::Button("Triangulate"))
	{
		if (triangulation == true)
		{
			triangulation = false;
		}
		else
		{
			triangulation = true;
		}
	}

	if (ImGui::Button("Add Texture"))
	{
		if (addTexture == true)
		{
			addTexture = false;
		}
		else
		{
			addTexture = true;
		}
	}

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	ImGui::End();

#pragma endregion

#pragma endregion	

}

#pragma region Main

int main()
{
	// initialise glfw
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(initWidth, initHeight, "Textured Quad Example", NULL, NULL);
	if(window == NULL)
	{
		fprintf(stdout, "Failed to create GLFW window!\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// initialise glew
	glewInit();

	// print version info 
	printDebugInfo();

	// Set callback function for resizing the window
	glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);

	// Set callback for keyboard events
	//glfwSetKeyCallback(window, keyboard);

	glfwSetCursorPosCallback(window, MouseCallback);

	//resize(window, initWidth, initHeight);

	// initialise geometry and shaders
	init_SceneData();

	ImGui::CreateContext();
	ImGui_ImplGlfwGL3_Init(window, true);
	ImGui::StyleColorsDark();



	// Loop while program is not terminated.
	while(!glfwWindowShouldClose(window))
	{
		ProcessInput(window);

		renderScene();						// Render into the current buffer
		
		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);			// Displays what was just rendered (using double buffering).

		glfwPollEvents();				// Use this version when animating as fast as possible
	}

	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}

#pragma endregion

#pragma region Functions

#pragma region Curve Algorithms

void CalculateHermite(float t, int element, Point p1, Point p2, Point tang1, Point tang2)
{
	float h0, h1, h2, h3;

	h0 = 2.0f * t * t * t - 3.0f * t * t + 1.0f;
	h1 = -2.0f * t * t * t + 3.0f * t * t;
	h2 = t * t * t - 2.0f * t * t + t;
	h3 = t * t * t - t * t;

	float x = p1.position.x * h0 + p2.position.x * h1 + (tang1.position.x) * h2 + (tang2.position.x) * h3;
	float y = p1.position.y * h0 + p2.position.y * h1 + (tang1.position.y) * h2 + (tang2.position.y) * h3;
	float z = p1.position.z * h0 + p2.position.z * h1 + (tang1.position.z) * h2 + (tang2.position.z) * h3;

	pointList[element] = {Point(x, y, z)};
}

void Hermite(Point p1, Point p2, Point tang1, Point tang2, int numberOfPoints)
{
	for(int i = 0; i < numberOfPoints; i++)
	{
		CalculateHermite((float)i / (float)numberOfPoints, i, p1, p2, tang1, tang2);
	}
}



void CalculateBezier(float t, int element, Point p1, Point p2, Point p3, Point p4)
{
	float x = B0(t) * p1.position.x + B1(t) * p2.position.x + B2(t) * p3.position.x + B3(t) * p4.position.x;
	float y = B0(t) * p1.position.y + B1(t) * p2.position.y + B2(t) * p3.position.y + B3(t) * p4.position.y;
	float z = B0(t) * p1.position.z + B1(t) * p2.position.z + B2(t) * p3.position.z + B3(t) * p4.position.z;

	pointList[element] = {Point(x, y, z)};
}

void Bezier(Point p1, Point p2, Point p3, Point p4, int resolution)
{
	for(int i = 0; i < resolution; i++)
	{
		CalculateBezier((float)i / (float)resolution, i, p1, p2, p3, p4);
	}
}



void CalculateBSpline(float t, int element, Point p1, Point p2, Point p3, Point p4)
{
	float B0, B1, B2, B3;

	B0 = ((1 - t) * (1 - t) * (1 - t));
	B1 = (3 * t * t * t - 6 * t * t + 4);
	B2 = (-3 * t * t * t + 3 * t * t + 1);
	B3 = (t * t * t);

	float x = 1.0f / 6.0f * (B0 * p1.position.x + B1 * p2.position.x + B2 * p3.position.x + B3 * p4.position.x);
	float y = 1.0f / 6.0f * (B0 * p1.position.y + B1 * p2.position.y + B2 * p3.position.y + B3 * p4.position.y);
	float z = 1.0f / 6.0f * (B0 * p1.position.z + B1 * p2.position.z + B2 * p3.position.z + B3 * p4.position.z);

	pointList[element] = {Point(x, y, z)};
}

void BSpline(Point p1, Point p2, Point p3, Point p4, int resolution)
{
	for(int i = 0; i < resolution; i++)
	{
		CalculateBSpline((float)i / (float)resolution, i, p1, p2, p3, p4);
	}
}










////////////////////////////////////////////////////////////////////
// Surface Calculation Functions								  //
////////////////////////////////////////////////////////////////////

/* The 'LOOP' function is the one that is called in the renderloop.  It cycles through every iteration of the resolution
size and calls the 'CALC' function everytime which finds its position, and after that it initialises the other parts of each POINT.  

CALC - this function uses the s and t values found from the LOOP and uses the s values first to find the U value of EVERY ROW.
	    These values are stored in the 'tempPosition' vector.
		
		Once all rows have been completed, the entire vector along with the t value is passed into the V function to find the v
		value.
*/

inline float SplineB0(float t)
{
	return ((1 - t) * (1 - t) * (1 - t)) / 6.0f;
}

inline float SplineB1(float t)
{
	return (3 * t * t * t - 6 * t * t + 4) / 6.0f;
}

inline float SplineB2(float t)
{
	return (-3 * t * t * t + 3 * t * t + 3 * t + 1) / 6.0f;
}

inline float SplineB3(float t)
{
	return (t * t * t) / 6.0f;
}


Point U(float t, int rowNumber)
{
	Point p;

	// Total each point by multiplying it with its blending function
	p.position.x =				   (SplineB0(t) * controlList[rowNumber][0].position.x +
									SplineB1(t) * controlList[rowNumber][1].position.x +
									SplineB2(t) * controlList[rowNumber][2].position.x +
									SplineB3(t) * controlList[rowNumber][3].position.x);

	p.position.y =				   (SplineB0(t) * controlList[rowNumber][0].position.y +
									SplineB1(t) * controlList[rowNumber][1].position.y +
									SplineB2(t) * controlList[rowNumber][2].position.y +
									SplineB3(t) * controlList[rowNumber][3].position.y);

	p.position.z =				   (SplineB0(t) * controlList[rowNumber][0].position.z +
									SplineB1(t) * controlList[rowNumber][1].position.z +
									SplineB2(t) * controlList[rowNumber][2].position.z +
									SplineB3(t) * controlList[rowNumber][3].position.z);

	return p;
}

Point V(float t, const std::vector<Point>& points)
{
	Point p;

	// Total each point by multiplying it with its blending function
	p.position.x =				 (SplineB0(t) * points[0].position.x +
								  SplineB1(t) * points[1].position.x +
								  SplineB2(t) * points[2].position.x +
								  SplineB3(t) * points[3].position.x);

	p.position.y =				 (SplineB0(t) * points[0].position.y +
								  SplineB1(t) * points[1].position.y +
								  SplineB2(t) * points[2].position.y +
								  SplineB3(t) * points[3].position.y);

	p.position.z =				 (SplineB0(t) * points[0].position.z +
								  SplineB1(t) * points[1].position.z +
								  SplineB2(t) * points[2].position.z +
								  SplineB3(t) * points[3].position.z);

	return p;
}

Point Calc(float s, float t)
{
	tempPosition.clear();

	// Create an array of 4 points
	tempPosition.resize(4);

	// Each point will be the returning value of the U LERP
	tempPosition[0] = U(s, 0);
	tempPosition[1] = U(s, 1);
	tempPosition[2] = U(s, 2);
	tempPosition[3] = U(s, 3);

	return V(t, tempPosition);
}

void Loop(int resolution)
{
	pointVector.clear();
	pointVector.resize(((size_t)resolution * (size_t)resolution));
	
	int k = 0;

	for (int i = 0; i < resolution; i++)
	{
		for (int j = 0; j < resolution; j++)
		{
			// calculate the point on the surface
			pointVector[k] = Calc((float)i / (float)(resolution - 1), (float)j / (float)(resolution - 1));
			pointVector[k].colour = Vec3(1.0f, 1.0f, 1.0f);
			pointVector[k].id = k;
			pointVector[k].texCoord = Vec2((float)i / (float)(resolution - 1), (float)j / (float)(resolution - 1));
			
			k++;
		}
	}
}

////////////////////////////////////////////////////////////////////
//																  //
////////////////////////////////////////////////////////////////////


void VectorToVector2D()
{
	// This is just a utility function to move the contents of the 2D vector into a 1D vector

	curvePointIndices.clear();

	int k = 0;

	if (pointVector.size() == res100)
	{
		pointIndices.resize(pointVector.size() * 0.1);

		for (int i = 0; i < pointVector.size() * 0.1; i++)
		{
			pointIndices[i].resize(pointVector.size() * 0.1);

			for (int j = 0; j < pointVector.size() * 0.1; j++)
			{
				pointIndices[i][j] = pointVector[k];

				k++;
			}
		}
	}
	else if (pointVector.size() == 10000)
	{
		pointIndices.resize(pointVector.size() * 0.01);

		for (int i = 0; i < pointVector.size() * 0.01; i++)
		{
			pointIndices[i].resize(pointVector.size() * 0.01);

			for (int j = 0; j < pointVector.size() * 0.01; j++)
			{
				pointIndices[i][j] = pointVector[k];

				k++;
			}
		}
	}

	
}

void Triangulate()
{
	// This function stesps through all indices in the 2D grid, and for each point, finds the point right and below-right,
	// and pushes it on the faceList

	for (int i = 0; i < pointIndices.size() - 1; ++i)
	{
		for (int j = 0; j < pointIndices.size() - 1; ++j)
		{
			faceList.push_back(Face(pointIndices[j][i], pointIndices[j + 1][i], pointIndices[j + 1][i + 1]));
		}
	}
}

void PullPointsOutOfFaceList(const std::vector<Face>& faceList)
{
	// This function accesses the 3 points stored in each FACE and pushes them in order onto a vector of unsigned ints

	for (int i = 0; i < faceList.size(); i++)
	{
		curvePointIndices.push_back(faceList[i].p1.id);
		curvePointIndices.push_back(faceList[i].p2.id);
		curvePointIndices.push_back(faceList[i].p3.id);
	}
}

void FindNormals()
{
	for (int i = 0; i < faceList.size(); i++)
	{
		Vec3 p1 = faceList[i].p1.position;
		Vec3 p2 = faceList[i].p2.position;
		Vec3 p3 = faceList[i].p3.position;

		Vec3 vec1 = p2 - p1;
		Vec3 vec2 = p3 - p1;

		Vec3 crossProduct = vec1.Cross(vec2);


		// Finds the normal of each iterative faceList point
		pointVector[faceList[i].p1.id].normal = crossProduct.Normalise();
		pointVector[faceList[i].p2.id].normal = crossProduct.Normalise();
		pointVector[faceList[i].p3.id].normal = crossProduct.Normalise();
	}
}

#pragma endregion

#pragma region Helper Functions

float B0(float t)
{
	return -t * -t * -t + 3 * t * t - 3 * t + 1;
}

float B1(float t)
{
	return 3 * t * t * t - 6 * t * t + 3 * t;
}

float B2(float t)
{
	return -3 * t * t * t + 3 * t * t;
}

float B3(float t)
{
	return t * t * t;
}

void CalculateResolution(std::vector<float>& resolution10, int numberOfPoints)
{
	float j = 0.0f;

	for(int i = 0; i < numberOfPoints; i++)
	{
		resolution10.push_back(j);

		j += (1 / numberOfPoints);
	}
}

Vec3 Linear(Vec3 p1, Vec3 p2, float t)
{
	float x = p1.x + t * (p2.x - p1.x);
	float y = p1.y + t * (p2.y - p1.y);

	return Vec3(x, y, 0);
}


#pragma endregion

#pragma region Enum-related Functions

int SetResolutionStatus(ActiveResolution newStatus)
{
	// Use memset to clear the pointList array each time the resolution is changed because 
	// otherwise there are still points in the pointsList which are displayed

	if(newStatus == ActiveResolution::TEN && resolution == newStatus)
	{
		memset(pointList, 0, sizeof(pointList));
		pointVector.clear();
		pointVector.resize(0);
		return res10;
	}
	else if(newStatus == ActiveResolution::HUNDRED && resolution == newStatus)
	{
		memset(pointList, 0, sizeof(pointList));
		pointVector.clear();
		pointVector.resize(0);
		return res100;
	}
	else if(newStatus == ActiveResolution::THOUSAND && resolution == newStatus)
	{
		memset(pointList, 0, sizeof(pointList));
		pointVector.clear();
		pointVector.resize(0);
		return res1000;
	}
	else
	{
		return res10;
	}
}

int SetPrimitiveType(Primitive newPrimitive)
{
	if(newPrimitive == Primitive::POINTS)
	{
		return 0x0000;
	}
	else if(newPrimitive == Primitive::LINES)
	{
		return 0x0001;
	}
	else if(newPrimitive == Primitive::LINESTRIP)
	{
		return 0x0003;
	}
	else if(newPrimitive == Primitive::LINEADJ)
	{
		return 0x000A;
	}
	else if(newPrimitive == Primitive::LINELOOP)
	{
		return 0x0002;
	}
	else
	{
		return 0x0000;
	}
}

#pragma endregion

#pragma region Setup

void init_SceneData()
{
	mySetupGeometries();

	// load the list of shaders required within this scene
	shaderList.CreateShader("shaders/points.vert", "shaders/points.frag");
	shaderList.CreateShader("shaders/control.vert", "shaders/control.frag");
	shaderList.CreateShader("shaders/light.vert", "shaders/light.frag");
	shaderList.CreateShader("shaders/texture.vert", "shaders/texture.frag");


	check_for_opengl_errors();   // Good idea to check for errors -- esp. good for debugging!
}

void mySetupGeometries()
{
	// objects..
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &curveVBO);
	glGenBuffers(1, &lightVBO);
	glGenBuffers(1, &controlPointsVBO);
	glGenBuffers(1, &controlPointsEBO);
	glGenBuffers(1, &curvePointsEBO);
	glGenTextures(1, &texture);

	glBindVertexArray(vao);

	// Control Points
	glBindBuffer(GL_ARRAY_BUFFER, controlPointsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(controlList), controlList, GL_STATIC_DRAW);
	glVertexAttribPointer(controlPositionLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, Point::position));
	glEnableVertexAttribArray(controlPositionLocation);
	glVertexAttribPointer(controlColourLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, Point::colour));
	glEnableVertexAttribArray(controlColourLocation);



	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, controlPointsEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(controlPointIndices), controlPointIndices, GL_STATIC_DRAW);



	// Points
	glBindBuffer(GL_ARRAY_BUFFER, curveVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pointList), pointList, GL_STATIC_DRAW);
	glVertexAttribPointer(curvePositionLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, Point::position));
	glEnableVertexAttribArray(curvePositionLocation);
	glVertexAttribPointer(curveColourLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, Point::colour));
	glEnableVertexAttribArray(curveColourLocation);
	glVertexAttribPointer(curveTexCoordsLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, Point::texCoord));
	glEnableVertexAttribArray(curveTexCoordsLocation);


	// Texture
	glBindTexture(GL_TEXTURE_2D, texture);

	// Constraints if the image size is incompatible
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	data = stbi_load("beach.jpg", &width, &height, &channels, 0);

	if (data != nullptr)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else std::cout << "Texture cannot be found\n";

	// free the data array once it has been loaded into a texture
	stbi_image_free(data);
}

void resize(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);		// Draw into entire window
}

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// ignore key up (key release) events
	if(action == GLFW_RELEASE)
		return;

	// process keyboard inputs here..
	switch(key)
	{
	case GLFW_KEY_ESCAPE:
	case GLFW_KEY_X:
		glfwSetWindowShouldClose(window, true);
		break;
	case GLFW_KEY_A:
		controlPoints = Primitive::POINTS;
		break;
	case GLFW_KEY_S:
		controlPoints = Primitive::LINES;
		break;
	case GLFW_KEY_D:
		controlPoints = Primitive::LINESTRIP;
		break;
	case GLFW_KEY_F:
		controlPoints = Primitive::LINEADJ;
		break;
	case GLFW_KEY_G:
		controlPoints = Primitive::LINELOOP;
		break;
	case GLFW_KEY_Q:
		curvePoints = Primitive::POINTS;
		break;
	case GLFW_KEY_W:
		curvePoints = Primitive::LINES;
		break;
	case GLFW_KEY_E:
		curvePoints = Primitive::LINESTRIP;
		break;
	case GLFW_KEY_R:
		curvePoints = Primitive::LINEADJ;
		break;
	case GLFW_KEY_T:
		curvePoints = Primitive::LINELOOP;
		break;
	case GLFW_KEY_1:
		resolution = ActiveResolution::TEN;
		break;
	case GLFW_KEY_2:
		resolution = ActiveResolution::HUNDRED;
		break;
	case GLFW_KEY_3:
		resolution = ActiveResolution::THOUSAND;
		break;
		
	case GLFW_KEY_SPACE:
	{
	}
	break;

	default:
	{
	}
	}
}

void printDebugInfo()
{
	// Print info of GPU and supported OpenGL version
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("OpenGL version supported %s\n", glGetString(GL_VERSION));

#ifdef GL_SHADING_LANGUAGE_VERSION
	printf("Supported GLSL version is %s.\n", (char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
#endif

	printf("Using GLEW version %s.\n", glewGetString(GLEW_VERSION));
	printf("------------------------------\n");
	printf("Press ESCAPE or 'X' or 'x' to exit.\n");
}

#pragma endregion

#pragma endregion

