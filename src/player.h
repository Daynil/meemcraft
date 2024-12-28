#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "entity.h"
#include "powerup.h"

struct ActivePowerup {
	PowerupType type;
	float seconds_left;
};

class Player : public Entity
{
public:
	glm::vec4 color;
	float start_velocity = 500.0f;
	float velocity = 500.0f;

	glm::vec3 start_size;

	std::vector<ActivePowerup> active_powerups;

	Player(RawModel* model, Texture* texture, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec4 color = glm::vec4(0))
		: Entity(model, texture, position, rotation, scale), start_size(scale), color(color) {
	};

	void Reset();
	void CollectPowerup(PowerupType type);

	void Move(float dt, int level_width, float move);
	void Update(float dt);

	void ProvideRenderData(Shader& shader) override
	{
		shader.setVec4("u_color", color);
	};
};