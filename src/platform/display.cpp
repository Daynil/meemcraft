#include "display.h"

#include <iostream>

#include <glad/glad.h>
#include "openglDebug.h"

Display::Display(float pDisplayWidth, float pDisplayHeight, std::string title)
{
	displayWidth = pDisplayWidth;
	displayHeight = pDisplayHeight;

	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //you might want to do this when testing the game for shipping

	// Reports OpenGL errors to std out
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

	window = glfwCreateWindow(displayWidth, displayHeight, title.c_str(), NULL, NULL);
	if (!window)
		return;

	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);

	// Reports OpenGL errors to std out
	int flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		std::cout << "Using OpenGL Version:" << std::endl;
		std::cout << glGetString(GL_VERSION) << std::endl;
		std::cout << "OpenGL Error Logging Enabled" << std::endl;
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(glDebugOutput, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}

	// Associate this Display instance with the GLFW window
	glfwSetWindowUserPointer(window, this);

	// Behind the scenes, this is used to transform 2d coordinates to coordinates on screen
	// E.g., (-0.5,0.5) would (as its final transformation) be mapped to (200,450) in screen coords
	glViewport(0, 0, displayWidth, displayHeight);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Listen to window resize events and update viewport
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

}

void Display::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// Retrieve the instance associated with this window
	Display* display = static_cast<Display*>(glfwGetWindowUserPointer(window));
	glViewport(0, 0, width, height);
	display->displayWidth = width;
	display->displayHeight = height;
}
