#include "entity.h"

#include <glm/glm.hpp>

#include <optional>


Entity::Entity(RawModel* pModel, Texture* pTexture, Shader* pShader, glm::vec3 pPosition, glm::vec3 pRotation, glm::vec3 pScale)
{
	model = pModel;
	texture = pTexture;
	shader = pShader;
	position = pPosition;
	rotation = pRotation;
	scale = pScale;
}
