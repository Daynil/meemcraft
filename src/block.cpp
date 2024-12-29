#include "block.h"

#include "resource_manager.h"

Block::Block(std::string type, glm::vec3 p_position, bool symmetrical) : type(type)
{
	position = p_position;
	rotation = glm::vec3(0);
	scale = glm::vec3(1);

	if (symmetrical)
		model = &ResourceManager::GetRawModel("block_symmetrical");
	else
		model = &ResourceManager::GetRawModel("block_3_part");
	texture = &ResourceManager::GetTexture(type);
}