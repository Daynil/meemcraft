#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.h"
#include "game.h"

class Controls
{
public:
	Controls(GLFWwindow* window, Camera* camera, Game* game);

	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

	static void joystick_callback(int jid, int event);
	void poll_gamepad(GLFWwindow* window);

private:
	static GLFWwindow* window;
	Camera* camera;
	Game* game;

	double lastX = 0;
	double lastY = 0;
	bool firstMouse = true;
	int joystick_id = -1;

	float yaw = 0;
	float pitch = 0;
	float roll = 0;

	float speed = 2.5f;

	float mouse_sensitivity = 0.1f;
	float joystick_sensitivity = 1.0f;
};