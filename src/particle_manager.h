#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "entity.h"
#include "particle.h"
#include "rendering/shader.h"
#include "raw_model.h"
#include "rendering/texture.h"
#include "rendering/renderer.h"

class ParticleManager
{
public:
	RawModel* model;
	Texture* texture;
	Shader* shader;

	Entity* ref_entity;

	std::vector<Particle> particles;

	glm::vec4 color = glm::vec4(1);

	unsigned int num_particles;

	ParticleManager(RawModel* model, Texture* texture, Shader* shader, Entity* ref_entity)
		:model(model), texture(texture), shader(shader), ref_entity(ref_entity) {
	};

	void Init(unsigned int num_particles);

	void Render(Renderer& renderer);

	void Update(float dt);
};