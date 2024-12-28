#pragma once

#include <optional>
#include <glm/glm.hpp>

#include "entity.h"
#include "shader_s.h"

class Particle : public Entity
{
public:
	glm::vec4 color;
	float life;

	Particle(RawModel* model, Texture* texture, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec4 color = glm::vec4(0), float life = 1.0f) : Entity(model, texture, position, rotation, scale), color(color), life(life) {};

	void ProvideRenderData(Shader& shader) override;
};
