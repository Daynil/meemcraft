#pragma once

#include <glm/glm.hpp>

#include "entity.h"
#include "shader_s.h"

class Brick : public Entity
{
public:
	glm::vec4 color;
	int life;
	bool solid;

	Brick(RawModel* model, Texture* texture, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec4 color = glm::vec4(0), int life = 1, bool solid = false) : Entity(model, texture, position, rotation, scale), color(color), life(life), solid(solid) {};

	void ProvideRenderData(Shader& shader) override;
};