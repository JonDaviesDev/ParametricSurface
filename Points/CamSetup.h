#pragma once
#include "Camera.h"

extern Camera camera;

#pragma region Mouse Variables

extern bool firstMouse;

extern float lastX;
extern float lastY;

extern float xOffset;
extern float yOffset;

extern float cameraSpeed;

#pragma endregion

#pragma region Screen Information

extern unsigned int screenWidth;

extern unsigned int screenHeight;

#pragma endregion

#pragma region ScreenCallbacks

extern void FrameBufferSizeCallback(GLFWwindow* window, int width, int height);

extern void MouseCallback(GLFWwindow* window, double x, double y);

#pragma endregion

#pragma region Keyboard

extern void ProcessInput(GLFWwindow* window);

#pragma endregion
