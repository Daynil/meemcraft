#include "laser.h"
#include "laser.h"
#include "resource_manager.h"

LaserGun::LaserGun(RawModel* model, Texture* texture, glm::vec3 p_position, glm::vec3 rotation, glm::vec3 scale, Player* player) : Entity(model, texture, p_position, rotation, scale), player(player)
{
	position = glm::vec3(player->position + glm::vec3(
		// X position gets set automatically in move
		0,
		-scale.y,
		0));
}

void LaserGun::Move(float dt)
{
	position.x = player->position.x + (player->scale.x / 2.0f) - (scale.x / 1.8f);

	for (auto it = lasers.begin(); it != lasers.end();) {
		it->position.y -= (it->velocity * dt);
		if (it->position.y <= 0.0f) {
			it = lasers.erase(it);
		}
		else {
			++it;
		}
	}
}

bool LaserGun::Update(float dt)
{
	laser_time_left -= dt;
	if (laser_time_left <= 0)
		return true;
	return false;
}

void LaserGun::Shoot()
{
	lasers.push_back(Laser(
		&ResourceManager::GetRawModel("quad"),
		&ResourceManager::GetTexture("laser"),
		position + glm::vec3(scale.x / 2.8f, 0, 0),
		glm::vec3(0),
		glm::vec3(40.0f, 75.0f, 0)
	));
}
