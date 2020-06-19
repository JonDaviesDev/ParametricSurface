#pragma once
#include "core.h"

enum CameraMovement
{
	FORWARD,
	FORWARDSLOW,
	BACKWARD,
	BACKWARDSLOW,
	LEFT,
	LEFTSLOW,
	RIGHT,
	RIGHTSLOW,
	UP,
	UPSLOW,
	DOWN,
	DOWNSLOW
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 5.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 90.0f;

class Camera
{
#pragma region Attributes

public:
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;

	// Euler angles
	float yaw = 0.0f;
	float pitch = 0.0f;

	// Options
	float movementSpeed;
	float mouseSensitivity;
	float zoom;

#pragma endregion

#pragma region Constructors

public:
	Camera(glm::vec3 cameraPosition, glm::vec3 upVector, float cameraYaw, float cameraPitch);

#pragma endregion

#pragma region Properties

public:
	glm::mat4 GetViewMatrix();

#pragma endregion

#pragma region Methods

public:
	void ProcessKeyboard(CameraMovement direction, float deltaTime);
	void MouseMovement(float xDelta, float yDelta, bool constrainPitch);
	void ProcesssMouseScroll(float yDelta);

private:
	void UpdateCameraVectors();

#pragma endregion
};

