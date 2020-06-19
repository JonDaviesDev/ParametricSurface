#include "Camera.h"


#pragma region Constructors

Camera::Camera(glm::vec3 cameraPosition, glm::vec3 upVector, float cameraYaw, float cameraPitch)
{
	front = glm::vec3(0.0f, 0.0f, -1.0f);
	movementSpeed = SPEED;
	mouseSensitivity = SENSITIVITY;
	zoom = ZOOM;

	position = cameraPosition;
	worldUp = upVector;
	yaw = cameraYaw;
	pitch = cameraPitch;

	UpdateCameraVectors();
}

#pragma endregion

#pragma region Properties

glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(position, position + front, up);
}

#pragma endregion

#pragma region Methods

void Camera::ProcessKeyboard(CameraMovement direction, float deltaTime)
{
	float velocity = movementSpeed * deltaTime;

	if(direction == FORWARD)
	{
		position += front * velocity;
	}

	if (direction == FORWARDSLOW)
	{
		velocity /= 3;
		
		position += front * velocity;
		int i = 3;
	}

	if(direction == BACKWARD)
	{
		position -= front * velocity;
	}

	if (direction == BACKWARDSLOW)
	{
		position -= front * (velocity / 2);
	}

	if(direction == RIGHT)
	{
		position += right * velocity;
	}

	if (direction == RIGHTSLOW)
	{
		position += right * (velocity / 2);
	}

	if(direction == LEFT)
	{
		position -= right * velocity;
	}

	if (direction == LEFT)
	{
		position -= right * (velocity / 2);
	}

	if(direction == UP)
	{
		position += (glm::cross(right, front)) * velocity;
	}

	if (direction == UPSLOW)
	{
		position += (glm::cross(right, front)) * (velocity / 2);
	}

	if(direction == DOWN)
	{
		position -= (glm::cross(right, front)) * velocity;
	}

	if (direction == DOWNSLOW)
	{
		position -= (glm::cross(right, front)) * (velocity / 2);
	}
}

void Camera::MouseMovement(float xDelta, float yDelta, bool constrainPitch)
{
	xDelta *= mouseSensitivity;
	yDelta *= mouseSensitivity;

	yaw += xDelta;
	pitch += yDelta;

	if(constrainPitch)
	{
		if(pitch > 89.0f)
		{
			pitch = 89.0f;
		}
		if(pitch < -89.0f)
		{
			pitch = -89.0f;
		}
	}

	UpdateCameraVectors();
}

void Camera::ProcesssMouseScroll(float yDelta)
{
	if(zoom >= 1.0f && zoom <= 45.0f)
	{
		zoom -= yDelta;
	}

	if(zoom <= 1.0f)
	{
		zoom = 1.0f;
	}

	if(zoom >= 45.0f)
	{
		zoom = 45.0f;
	}
}

void Camera::UpdateCameraVectors()
{
	glm::vec3 frontVec;

	frontVec.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	frontVec.y = sin(glm::radians(pitch));
	frontVec.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	front = glm::normalize(frontVec);

	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));
}

#pragma endregion