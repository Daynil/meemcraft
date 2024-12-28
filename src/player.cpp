#include "player.h"

#include <iostream>

#include <glm/glm.hpp>

void Player::CollectPowerup(PowerupType type)
{
	std::cout << "Collected: " + type << std::endl;

	// If we already have the powerup, just refresh its timer
	for (auto& powerup : active_powerups) {
		if (powerup.type == type) {
			powerup.seconds_left = 10.0f;
			return;
		}
	}

	active_powerups.push_back({ type, 10.0f });
	if (type == SPEED) {
		velocity *= 2.0f;
		color = glm::vec4(0.9f, 0, 0, 0.3f);
	}
	else if (type == WIDTH) {
		scale.x += 100.0f;
	}
}

void Player::Reset()
{
	active_powerups.clear();
}

void Player::Move(float dt, int level_width, float move)
{
	if (position.x >= 0 && position.x <= (level_width - scale.x))
		position.x += velocity * dt * move;

	position.x = glm::clamp(position.x, 0.0f, level_width - scale.x);
}

void Player::Update(float dt)
{
	for (auto it = active_powerups.begin(); it != active_powerups.end();) {
		it->seconds_left -= dt;
		if (it->seconds_left <= 0) {
			if (it->type == SPEED) {
				velocity = start_velocity;
			}
			else if (it->type == WIDTH) {
				scale.x = start_size.x;
			}
			std::cout << "Expired: " + it->type << std::endl;
			it = active_powerups.erase(it);
			color = glm::vec4(0);
		}
		else {
			++it;
		}
	}
}
