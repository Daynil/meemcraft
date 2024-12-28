#pragma once

#include <string>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

class Display
{
public:
	float displayWidth;
	float displayHeight;

	GLFWwindow* window;

	Display(float displayWidth, float displayHeight, std::string title);
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
};