#pragma once

#include <glm/glm.hpp>

#include "block_data.h"
#include "block.h"
#include "resource_manager.h"
#include "shared.h"

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

	std::map<ChunkDirection::AdjacentChunk, Chunk*> adjacent_chunks;

	Chunk(ChunkID id, glm::vec3 p_position, std::vector<std::vector<double>>* chunk_map_data);

	~Chunk() {
		delete model;
		adjacent_chunks.clear();
	};

	void ChunkTest();

	BlockType GetBlockType(double noise_value, int y);

	void GenerateBlocks(std::vector<std::vector<double>>* chunk_map);
	void GenerateMesh();

};