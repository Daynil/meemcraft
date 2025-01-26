#pragma once

#include <glm/glm.hpp>

#include "block_data.h"
#include "block.h"
#include "resource_manager.h"
#include "shared.h"

// Forward declaration to avoid circular imports
class MapGenerator;

// A chunk's ID is its world coordinate.
// World coordinates are 2d since we take vertical slices, so this is (x, z),
// though it is accessed as (x, y).
using ChunkID = glm::vec2;

class Chunk
{
public:
	ChunkID id;

	glm::vec3 position;
	bool has_transparent_blocks = false;

	Entity opaque_entity;
	Entity transparent_entity;

	bool should_render = false;

	BlockInfo blocks[ChunkHelpers::CHUNK_SIZE_X][ChunkHelpers::CHUNK_SIZE_Y][ChunkHelpers::CHUNK_SIZE_Z];

	MapGenerator* map_generator;

	std::map<ChunkHelpers::AdjacentChunk, Chunk*> adjacent_chunks;

	Chunk(ChunkID id, glm::vec3 p_position, MapGenerator* map_generator);

	~Chunk();

	void ChunkTest();

	BlockType GetBlockType(int x, int y, int z);

	bool ShouldRenderFace(BlockType block, BlockType adjacent_block);
	bool IsTransparentBlock(BlockType block);

	void GenerateBlocks();
	void GenerateMesh();
	void LoadToGPU();

};