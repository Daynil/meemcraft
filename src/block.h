#pragma once

#include <glm/glm.hpp>

#include "entity.h"
#include "shader_s.h"
#include "block_data.h"

class Block : public Entity
{
public:
	BlockType type;
	bool symmetrical;

	Block(BlockType type, glm::vec3 position, bool symmetrical);
	Block(const Block& orig);
};