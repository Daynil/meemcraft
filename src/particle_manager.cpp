#include "particle_manager.h"

#include <iostream>

#include "util.h"
#include "resource_manager.h"

void ParticleManager::Init(unsigned int p_num_particles)
{
	num_particles = p_num_particles;
	for (unsigned int i = 0; i < num_particles; i++)
	{
		float random_x = random_float(-5, 5);
		float random_y = random_float(-5, 5);
		particles.push_back(Particle(
			model,
			texture,
			&ResourceManager::GetShader("particle"),
			ref_entity->position + glm::vec3(random_x, random_y, 0.0f) + glm::vec3(ref_entity->scale.x / 2.0f, ref_entity->scale.x / 2.0f, 0.0f),
			glm::vec3(0),
			glm::vec3(ref_entity->scale / 8.0f),
			color,
			random_float(0, 1)
		));
	}
}

void ParticleManager::Render(Renderer& renderer)
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	for (auto& particle : particles)
	{
		if (particle.color.a > 0)
			renderer.render_entity(particle);
	}
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleManager::Update(float dt)
{
	//std::cout << dt;
	for (auto& particle : particles)
	{
		float alpha_decay = random_float(0.8f, 1.0f);
		particle.life -= dt;
		particle.color.a -= alpha_decay * dt;
		if (particle.life <= 0) {
			float random_x = random_float(-5, 5);
			float random_y = random_float(-5, 5);

			particle.life = random_float(0, 1);
			particle.position = ref_entity->position + glm::vec3(random_x, random_y, 0.0f) + glm::vec3(ref_entity->scale.x / 2.0f, ref_entity->scale.x / 2.0f, 0.0f);
			particle.color = color;
		}
	}
}
