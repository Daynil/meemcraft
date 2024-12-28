#pragma once

#include <glm/glm.hpp>

#include "entity.h"
#include "shader_s.h"

enum BlockType {
	// Environmental
	GRASS,
	DIRT,
	SAND,
	STONE,

	// Ore
	COPPER,
	TIN,
	IRON,
	SILVER,
	GOLD,

	// Growth
	WOOD,
	LEAVES,

	// Represents the amount of block types
	BLOCK_COUNT
};

constexpr std::array<const char*, BlockType::BLOCK_COUNT> BlockTypeString = {
	// Environmental
	"GRASS",
	"DIRT",
	"SAND",
	"STONE",

	// Ore
	"COPPER",
	"TIN",
	"IRON",
	"SILVER",
	"GOLD",

	// Growth
	"WOOD",
	"LEAVES",
};

class Block : public Entity
{
public:
	BlockType type;

	Block(BlockType type, glm::vec3 position);
};