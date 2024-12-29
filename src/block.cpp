#include "block.h"

#include "resource_manager.h"

Block::Block(BlockType type, glm::vec3 p_position, bool symmetrical) : type(type)
{
	position = p_position;
	rotation = glm::vec3(0);
	scale = glm::vec3(1);

	if (symmetrical)
		model = &ResourceManager::GetRawModel("block_symmetrical");
	else
		model = &ResourceManager::GetRawModel("block_3_part");
	texture = &ResourceManager::GetTexture(BlockTypeString[type]);
}

Block::Block(const Block& orig)
{
	position = orig.position;
	rotation = orig.rotation;
	scale = orig.scale;
	symmetrical = orig.symmetrical;
	model = orig.model;
	texture = orig.texture;
}
