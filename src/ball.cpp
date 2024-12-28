#include "ball.h"

#include <cmath>
#include <iostream>

void Ball::Reset()
{
	stuck = true;
	position = player->position + glm::vec3((player->scale.x / 2.0f) - (scale.x / 2.0f), -scale.x, 0.0f);
	position.x = player->position.x + (player->scale.x / 2.0f) - (scale.x / 2.0f);
	position.y = player->position.y - scale.y;
	velocity = glm::vec3(initial_velocity_x, -initial_velocity_y, 0);
}

bool Ball::Move(float dt, int level_width, int level_height, bool should_release, float player_movement)
{
	if (stuck && should_release) {
		stuck = false;
		// If player is moving, take that into account on release
		if (std::abs(player_movement) > 0)
			velocity.x = player_movement * initial_velocity_x * 3.0f;
		else
			velocity.x = initial_velocity_x;
		velocity.y = -initial_velocity_y;
	}

	if (stuck) {
		position.x = player->position.x + (player->scale.x / 2.0f) - (scale.x / 2.0f);
		return false;
	}

	position += velocity * dt;

	if (position.x <= 0.0f) {
		velocity.x = -velocity.x;
		position.x = 0.0f;
	}
	else if ((position.x + scale.x) >= level_width) {
		velocity.x = -velocity.x;
		position.x = level_width - scale.x;
	}

	if (position.y <= 0.0f) {
		velocity.y = -velocity.y;
		position.y = 0.0f;
	}

	if (position.y >= level_height) {
		Reset();
		return true;
	}

	return false;
}

bool Ball::Update(float dt)
{
	if (is_fireball) {
		fireball_time_left -= dt;
		if (fireball_time_left <= 0) {
			is_fireball = false;
			color = glm::vec4(0);
			return true;
		}
	}
	return false;
}

void Ball::ApplyPowerup(PowerupType type)
{
	is_fireball = true;
	fireball_time_left = 10.0f;
	color = glm::vec4(0.8f, 0, 0, 0.3f);
}
