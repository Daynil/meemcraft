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
#include "util.h"
#include "map_generation/map_generator.h"

struct Vec2Comparator {
	bool operator()(const ChunkID& a, const ChunkID& b) const {
		if (a.x != b.x) {
			return a.x < b.x;
		}
		return a.y < b.y;
	}
};

struct CoordMap {
	// Coord of chunk relative to chunks being generated
	glm::vec2 relative_coord;
	// Coord of chunk in world
	ChunkID world_coord;
};

class ChunkManager
{
public:
	static const int VIEW_DIST_CHUNKS = 10;

	std::vector<std::vector<double>> noise_map;
	// Not thread safe, accessible to main thread only
	std::map<ChunkID, Chunk*, Vec2Comparator> chunks;

	ThreadPool* thread_pool;
	// Thread safe variables
	std::mutex queue_mutex;
	std::map<ChunkID, Chunk*, Vec2Comparator> chunks_to_queue;
	bool chunks_to_queue_ready = false;
	std::vector<Chunk*> chunks_cpu_queue;
	std::vector<Chunk*> chunks_gpu_queue;
	std::queue<Chunk*> chunk_queue;

	MapGenerator* map_generator;

	ChunkManager(ThreadPool* thread_pool, MapGenerator* map_generator) : thread_pool(thread_pool), map_generator(map_generator) {};

	void GenerateChunksCenteredAt(glm::vec2 position);

	// 1.
	// When we're ready to create a new set of chunks, this sets off a set of coordinated queues
	// which use a thread pool to offload all expensive tasks off the main thread.
	void QueueChunks();

	// 2.
	// For a given set of chunks to load, create their data first.
	// This is the minimal work we need to do before we start calculating meshes because
	// meshes need all adjacent chunks to have this data.
	void CreateInitialChunkData(std::vector<CoordMap> chunks_to_gen);

	// 3.
	// Thread coordinating loop:
	// Checks for completion of initial chunk data, then queues to CPU. 
	// Once the CPU queue completes tasks, pushes them to the GPU queue.
	void ProcessChunks();

	// 4.
	// Chunks that are ready to process on CPU get throttled here to avoid swamping the CPU
	void LoadChunks();

	// 5.
	// Put a chunk into a thread pool to get its mesh data processed on the CPU.
	void QueueChunk(Chunk* chunk);

	// 6.
	// Chunks that are ready to process on GPU get throttled here to avoid swamping the GPU.
	// Uploads final vertex data to GPU on the main thread (OpenGL requires everything on main thread).
	void UploadCompletedChunks();

	// For debugging (regenerate map)
	void ClearChunks();

	std::map<ChunkDirection::AdjacentChunk, Chunk*> GetAdjacentChunks(ChunkID chunk_id, std::map<ChunkID, Chunk*, Vec2Comparator> chunks_to_check_new,
		std::map<ChunkID, Chunk*, Vec2Comparator> chunks_to_check_existing);

private:
	// Load a chunk - determine block types by noise_map, which faces
	// to render, and load geometry.
	Chunk* LoadChunk(Chunk* chunk);

	Chunk* GetChunkOrNull(ChunkID chunk_id);
};