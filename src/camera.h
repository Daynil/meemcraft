#pragma once

#include <glm/glm.hpp>

class Camera
{
public:
	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;

	float FOV = 45.0f;
	float NEAR_PLANE = 0.1f;
	float FAR_PLANE = 100.0f;

	float speed = 10.0f;
	float dead_zone = 0.1f;

	Camera();

	void Pivot(float yaw, float pitch, float roll);
	void Move(float dt, glm::vec2 direction);
};