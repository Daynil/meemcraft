#pragma once

#include <map>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>

#include <glm/glm.hpp>
#include "thread_pool.hpp"

#include "block_data.h"
#include "block.h"
#include "chunk.h"
#include "shared.h"

struct Vec2Comparator {
	bool operator()(const ChunkID& a, const ChunkID& b) const {
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
	std::map<ChunkID, Chunk*, Vec2Comparator> chunks;

	std::queue<Chunk*> chunk_queue;
	std::mutex queue_mutex;

	ThreadPool pool;

	ChunkManager();

	// Thread coordinating loop - loads data in from the workers via queue
	// when available.
	void ProcessChunks();

	// Put a chunk into queue to be processed in a thread.
	void QueueChunk(Chunk* chunk);

	void LoadChunks();

	std::map<ChunkDirection::AdjacentChunk, Chunk*> GetAdjacentChunks(ChunkID chunk_id, std::map<ChunkID, Chunk*, Vec2Comparator> chunks_to_check);

private:
	// Load a chunk - determine block types by noise_map, which faces
	// to render, and load geometry.
	Chunk* LoadChunk(Chunk* chunk);
};