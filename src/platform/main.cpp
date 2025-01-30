#include <iostream>
#include <cmath>

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stb_image/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Our stuff
#include "display.h"
#include "rendering/renderer.h"
#include "controls.h"
#include "game.h"
#include "resource_manager.h"
#include "tests/tests.h"


#define USE_GPU_ENGINE 0
extern "C"
{
	__declspec(dllexport) unsigned long NvOptimusEnablement = USE_GPU_ENGINE;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = USE_GPU_ENGINE;
}

int main(void)
{
	RunTests();
	// If we just want a test run without spinning up the UI, we can uncomment this
	//return 0;

	if (!glfwInit())
		return -1;

	Display display(1920.0f, 1080.0f, "MeemCraft");
	if (!display.window)
	{
		std::cout << "***ERROR initializing glfw window" << std::endl;
		glfwTerminate();
		return -1;
	}

	Renderer renderer(&display);

	Game game(display.displayWidth, display.displayHeight, &renderer);
	game.Init();
	Controls controls(display.window, game.camera, &game);

	float lastFrame = 0.0f;

	while (!glfwWindowShouldClose(display.window))
	{
		float currentFrame = glfwGetTime();
		// Delta time = seconds per frame (s/f)
		float deltaTime = currentFrame - lastFrame;
		float fps = 1 / deltaTime;
		lastFrame = currentFrame;

		controls.poll_gamepad(display.window);
		game.ProcessInput(deltaTime);

		game.Update(deltaTime);

		game.Render();

		//std::cout << gameState.fps << " " << gameState.deltaTime << std::endl;

		glfwSwapBuffers(display.window);
		glfwPollEvents();
	}

	ResourceManager::Clear();

	//there is no need to call the clear function for the libraries since the os will do that for us.
	//by calling this functions we are just wasting time.
	//glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
