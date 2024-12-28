#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "entity.h"
#include "player.h"

class Laser : public Entity
{
public:
	float velocity = 200.0f;

	glm::vec4 color = glm::vec4(1.0f, 0, 0, 0.9f);

	Laser(RawModel* model, Texture* texture, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) : Entity(model, texture, position, rotation, scale) {};

	void ProvideRenderData(Shader& shader) override
	{
		shader.setVec4("u_color", color);
	}
};

class LaserGun : public Entity
{
public:
	Player* player;
	float laser_time_left = 10.0f;

	glm::vec4 color = glm::vec4(0.58f, 0, 0, 0.9f);

	std::vector<Laser> lasers;

	LaserGun(RawModel* model, Texture* texture, glm::vec3 p_position, glm::vec3 rotation, glm::vec3 scale, Player* player);

	void Move(float dt);
	bool Update(float dt);

	void Shoot();

	void ProvideRenderData(Shader& shader) override
	{
		shader.setVec4("u_color", color);
	}
};