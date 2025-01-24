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

class Chunk : public Entity
{
public:
	ChunkID id;

	bool should_render = false;

	static const int CHUNK_SIZE_X = 16;
	static const int CHUNK_SIZE_Y = 256;
	static const int CHUNK_SIZE_Z = 16;

	BlockInfo blocks[CHUNK_SIZE_X][CHUNK_SIZE_Y][CHUNK_SIZE_Z];

	MapGenerator* map_generator;

	std::map<ChunkDirection::AdjacentChunk, Chunk*> adjacent_chunks;

	Chunk(ChunkID id, glm::vec3 p_position, MapGenerator* map_generator);

	~Chunk();

	void ChunkTest();

	BlockType GetBlockType(int x, int y, int z);

	void GenerateBlocks();
	void GenerateMesh();

};