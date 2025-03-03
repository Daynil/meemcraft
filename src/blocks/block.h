#pragma once

#include <glm/glm.hpp>

#include "entity.h"
#include "rendering/shader.h"
#include "block_data.h"

class Block : public Entity
{
public:
	BlockType type;
	BlockData& data;

	Block(BlockType type, glm::vec3 position);
};