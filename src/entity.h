#pragma once
#include <glm/glm.hpp>

#include <vector>
#include <optional>

#include "texture.h"
#include "raw_model.h"
#include "shader_s.h"

class Entity
{
public:
	RawModel* model;
	Texture* texture;

	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	Entity() {};
	Entity(RawModel* pModel, Texture* texture, glm::vec3 pPosition, glm::vec3 pRotation, glm::vec3 pScale);

	// Implemented by Entities if needed for non-standard
	// shader (e.g., add u_color)
	virtual void ProvideRenderData(Shader& shader) {};

	virtual ~Entity() {};

private:
	std::vector<float> vertex_positions;
	std::vector<float> vertex_texture_uvs;
	std::vector<unsigned int> vertex_indices;
};