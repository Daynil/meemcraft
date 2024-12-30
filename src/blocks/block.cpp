#include "block.h"

#include "resource_manager.h"

Block::Block(BlockType type, glm::vec3 p_position) : type(type), data(ResourceManager::GetBlockData(type))
{
	position = p_position;
	rotation = glm::vec3(0);
	scale = glm::vec3(1);

	if (data.symmetrical)
		model = &ResourceManager::GetRawModel("block_symmetrical");
	else
		model = &ResourceManager::GetRawModel("block_3_part");
	texture = &ResourceManager::GetTexture(BlockTypeString[type]);
}
