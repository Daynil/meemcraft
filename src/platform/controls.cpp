#include "controls.h"

#include <string>
#include <iostream>
#include <cmath>

GLFWwindow* Controls::window = nullptr;

Controls::Controls(GLFWwindow* pWindow, Camera* pCamera, Game* pGame)
{
	bool MOUSE_LOOK_ENABLED = true;

	Controls::window = pWindow;
	camera = pCamera;
	game = pGame;

	glfwSetWindowUserPointer(window, this);

	if (MOUSE_LOOK_ENABLED)
	{
		// Disable cursor for best FPS mode, removed for testing
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetCursorPosCallback(window, mouse_callback);
	}

	glfwSetScrollCallback(window, scroll_callback);
	glfwSetJoystickCallback(joystick_callback);

	glfwSetKeyCallback(window, key_callback);

	// Look for controllers that were already connected before the game started
	for (int i = GLFW_JOYSTICK_1; i < GLFW_JOYSTICK_16; i++)
	{
		if (glfwJoystickPresent(i))
			joystick_callback(i, GLFW_CONNECTED);
	}
}


void Controls::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	// Retrieve the controls instance associated with this window
	Controls* controls = static_cast<Controls*>(glfwGetWindowUserPointer(window));

	if (controls->firstMouse)
	{
		controls->lastX = xpos;
		controls->lastY = ypos;
		controls->firstMouse = false;
	}

	float xoffset = xpos - controls->lastX;
	float yoffset = controls->lastY - ypos;
	controls->lastX = xpos;
	controls->lastY = ypos;

	controls->yaw += (xoffset * controls->mouse_sensitivity);
	controls->pitch += (yoffset * controls->mouse_sensitivity);

	if (controls->pitch > 89.0f)
		controls->pitch = 89.0f;
	if (controls->pitch < -89.0f)
		controls->pitch = -89.0f;

	controls->camera->Pivot(controls->yaw, controls->pitch, controls->roll);
}

void Controls::scroll_callback(GLFWwindow* window, double xoffest, double yoffset)
{
	// Retrieve the controls instance associated with this window
	Controls* controls = static_cast<Controls*>(glfwGetWindowUserPointer(window));

	controls->camera->FOV -= (float)yoffset;
	if (controls->camera->FOV < 1.0f)
		controls->camera->FOV = 1.0f;
	if (controls->camera->FOV > 45.0f)
		controls->camera->FOV = 45.0f;
}

void Controls::key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// Retrieve the controls instance associated with this window
	Controls* controls = static_cast<Controls*>(glfwGetWindowUserPointer(window));

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			controls->game->keyboard_keys[key] = true;
		else if (action == GLFW_RELEASE) {
			controls->game->keyboard_keys[key] = false;
			controls->game->keyboard_keys_processed[key] = false;
		}
	}
}

void Controls::joystick_callback(int jid, int event)
{
	// Retrieve the controls instance associated with this window
	Controls* controls = static_cast<Controls*>(glfwGetWindowUserPointer(Controls::window));

	if (event == GLFW_CONNECTED)
	{
		if (glfwJoystickIsGamepad(jid))
		{
			std::cout << "Gamepad connected: " + (std::string)glfwGetGamepadName(jid) + " (id: " << jid << ")" << std::endl;
			GLFWgamepadstate state;
			if (glfwGetGamepadState(jid, &state))
			{
				controls->joystick_id = jid;
			}
			else
			{
				std::cout << "Gamepad has no valid mapping: " + (std::string)glfwGetGamepadName(jid) << std::endl;
			}
		}

	}
	else if (event == GLFW_DISCONNECTED)
	{
		//std::cout << "Gamepad disconnected: " + (std::string)glfwGetGamepadName(jid) << std::endl;
		std::cout << "Gamepad id " << jid << " disconnected!" << std::endl;
		controls->joystick_id = -1;
	}
}

void Controls::poll_gamepad(GLFWwindow* window)
{
	// Handle window closure buttons here
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (joystick_id >= 0)
	{
		GLFWgamepadstate state;
		if (glfwGetGamepadState(joystick_id, &state))
		{
			float leftStickX = state.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
			float leftStickY = state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];

			// Avoid dead zone
			if (std::abs(leftStickX) > 0.1)
				game->left_stick_x = leftStickX;
			else
				game->left_stick_x = 0;

			if (std::abs(leftStickY) > 0.1)
				game->left_stick_y = leftStickY;
			else
				game->left_stick_y = 0;

			float rightStickX = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X];
			float rightStickY = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y];

			// Avoid dead zone
			if (std::abs(rightStickX) > 0.1)
				yaw += (rightStickX * joystick_sensitivity);
			if (std::abs(rightStickY) > 0.1)
				// By default right stick uses airplane style controls (up is down) - invert
				pitch += (-rightStickY * joystick_sensitivity);

			if (pitch > 89.0f)
				pitch = 89.0f;
			if (pitch < -89.0f)
				pitch = -89.0f;

			glm::vec3 direction;
			direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
			direction.y = sin(glm::radians(pitch));
			direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
			camera->cameraFront = glm::normalize(direction);


			for (int i = 0; i < GLFW_GAMEPAD_BUTTON_LAST; ++i) {
				if (state.buttons[i] == GLFW_PRESS)
					game->gamepad_keys[i] = true;
				else if (state.buttons[i] == GLFW_RELEASE) {
					game->gamepad_keys[i] = false;
					game->gamepad_keys_processed[i] = false;
				}
			}
		}
	}
}