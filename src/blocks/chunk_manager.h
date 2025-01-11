#pragma once

#include <map>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>

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

	std::queue<std::string> chunk_queue;
	//std::queue<Chunk> chunk_queue;
	std::mutex queue_mutex;

	ChunkManager() {};

	// Load a chunk - determine block types by noise_map, which faces
	// to render, and load geometry.
	std::string LoadChunk(glm::vec2 world_coord);
	//Chunk LoadChunk(glm::vec2 world_coord);

	// Process a chunk in a thread and push result to queue.
	void ChunkWorker(glm::vec2 world_coord);
	// Put a chunk into queue to be processed in a thread.
	void QueueChunk(glm::vec2 world_coord);
	// Thread coordinating loop - loads data in from the workers via queue
	// when available.
	void ProcessChunks();

	void LoadChunks();

	std::map<ChunkDirection::AdjacentChunk, Chunk*> GetAdjacentChunks(glm::vec2 world_coord);
};