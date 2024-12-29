#include "camera.h"

#include <iostream>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera()
{
	cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	cameraFront = glm::normalize(glm::vec3(0.0f, 0.0f, -2.0f));
	cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
}

void Camera::Pivot(float yaw, float pitch, float roll)
{
	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);
}

void Camera::Move(float dt, glm::vec2 direction)
{
	if (std::abs(direction.y) >= dead_zone) {
		// invert y (defaults to down is up)
		cameraPos += -direction.y * speed * dt * cameraFront;
	}
	if (std::abs(direction.x) >= dead_zone) {
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * direction.x * speed * dt;
	}
}
