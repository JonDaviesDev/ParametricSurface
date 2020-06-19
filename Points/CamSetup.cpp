#include "CamSetup.h"


Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), YAW, PITCH);



#pragma region Screen Information

unsigned int screenWidth = 2000;

unsigned int screenHeight = 1500;

#pragma endregion

#pragma region Mouse Variables

bool firstMouse = true;

float lastX = 0.0f;
float lastY = 0.0f;

float xOffset = 0.0f;
float yOffset = 0.0f;

float cameraSpeed = 0.0f;

float currentFrame = 0.0f;

float deltaTime = 0.01f;
float lastFrame = 0.0f;

#pragma endregion

#pragma region Screen Callbacks

void FrameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{
	if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
	{
		if (firstMouse)
		{
			lastX = (float)xPos;
			lastY = (float)yPos;

			firstMouse = false;
		}

		xOffset = (float)xPos - lastX;
		yOffset = lastY - (float)yPos;

		lastX = (float)xPos;
		lastY = (float)yPos;

		camera.MouseMovement(xOffset, yOffset, true);
	}
}

void ProcessInput(GLFWwindow* window)
{
	cameraSpeed = 2.5f * deltaTime;

	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}

	if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}

	if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(LEFT, deltaTime);
	}

	if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}

	if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(UP, deltaTime);
	}

	if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(DOWN, deltaTime);
	}
}

#pragma endregion