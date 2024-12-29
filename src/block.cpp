#include "block.h"

#include "resource_manager.h"

Block::Block(std::string type, glm::vec3 p_position) : type(type)
{
	position = p_position;
	rotation = glm::vec3(0);
	scale = glm::vec3(1);

	model = &ResourceManager::GetRawModel("block");
	texture = &ResourceManager::GetTexture(type);
}