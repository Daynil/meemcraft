#include "block.h"

#include "resource_manager.h"

Block::Block(BlockType type, glm::vec3 p_position) : type(type)
{
	position = p_position;
	rotation = glm::vec3(0);
	scale = glm::vec3(1);

	std::string type_str = BlockTypeString[type];

	model = &ResourceManager::GetRawModel("block");
	texture = &ResourceManager::GetTexture(type_str);
}