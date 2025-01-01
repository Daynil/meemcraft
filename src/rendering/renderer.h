#pragma once

#include <glad/glad.h>
#include <stb_image/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "entity.h"
#include "camera.h"
#include "display.h"
#include "shader.h"

class Renderer
{
public:
	Display* display;
	Camera* camera;

	Renderer(Display* display) : display(display) {};

	void init(Camera* camera);
	void prepare_entity(Entity& entity);
	void cleanup_entity(Entity& entity);
	void render(Entity& entity);
	void prepare();
};