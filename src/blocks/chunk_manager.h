#pragma once

#include <map>
#include <vector>

#include <glm/glm.hpp>

#include "block_data.h"
#include "block.h"
#include "chunk.h"
#include "shared.h"

struct Vec2Comparator {
	bool operator()(const glm::vec2& a, const glm::vec2& b) const {
		if (a.x != b.x) {
			return a.x < b.x;
		}
		return a.y < b.y;
	}
};


class ChunkManager
{
public:

	std::vector<double> noise_map;
	std::map<glm::vec2, Chunk*, Vec2Comparator> chunks;

	ChunkManager() {};

	void LoadChunks();

	std::map<ChunkDirection::AdjacentChunk, Chunk*> GetAdjacentChunks(glm::vec2 world_coord);
};