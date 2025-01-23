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
#include "camera.h"

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

	bool operator==(const CoordMap& other) const {
		return world_coord == other.world_coord;
	}

	bool operator<(const CoordMap& other) const {
		if (world_coord.x != other.world_coord.x)
			return world_coord.x < other.world_coord.x;
		if (world_coord.y != other.world_coord.y)
			return world_coord.y < other.world_coord.y;
		return world_coord.x < other.world_coord.x;
	}
};

class ChunkManager
{
public:
	static const int VIEW_DIST_CHUNKS = 10;

	// Not thread safe, accessible to main thread only
	std::map<ChunkID, Chunk*, Vec2Comparator> chunks;

	ThreadPool* thread_pool;
	// Thread safe variables
	std::mutex queue_mutex;
	bool batch_processing = false;
	int chunks_in_batch = 0;
	int chunks_in_batch_complete = 0;
	std::queue<std::tuple<glm::vec2, std::vector<CoordMap>>> chunks_initial_data_queue;
	std::queue<Chunk*> chunks_cpu_queue;
	std::queue<Chunk*> chunks_gpu_queue;
	bool noisemap_data_generated = false;

	MapGenerator* map_generator;
	Camera* camera;

	ChunkManager(ThreadPool* thread_pool, MapGenerator* map_generator, Camera* camera) : thread_pool(thread_pool), map_generator(map_generator), camera(camera) {};

	// 1. Queue up needed chunks and delete those out of view dist
	void RefreshChunksCenteredAt(glm::vec2 position);

	// 2.
	// Thread coordinating loop which, in batches, queues up the full pipeline
	// (allowing only a single batch to process at a time to avoid race conditions)
	// and handles needed main-thread actions:
	// 
	// Freshly queued chunks first get their initial data processed.
	//	- Creates noisemap for the set of blocks
	//	- Gets the initial block data and collects adjacent chunks
	// 
	// Chunks ready to process on CPU get throttled here to avoid swamping the CPU.
	//	- CPU loads mesh data.
	// 
	// Chunks ready for GPU likewise get throttled.
	//	- Uploads final vertex data to GPU on main thread 
	//	(OpenGL requires everything on the main thread).
	void ProcessChunks();

	// 3.
	// For a given set of chunks to load, create their data first.
	// This is the minimal work we need to do before we start calculating meshes because
	// meshes need all adjacent chunks to have this data.
	// This occurs on a thread pool to avoid work on main thread.
	void CreateInitialChunkData(std::tuple<glm::vec2, std::vector<CoordMap>> point_coord_map);

	// 4.
	// Thread coordinating loop calls this to start loading mesh data in thread pool.
	// When complete here, push to GPU queue for thread coordinating loop to finish
	// upload.
	void QueueChunk(Chunk* chunk);

	// For debugging (regenerate map)
	void ClearChunks();

	void ClearChunksOutOfView();

	ChunkID GetAdjacentChunkID(ChunkID chunk_id, ChunkDirection::AdjacentChunk direction);

	std::map<ChunkDirection::AdjacentChunk, Chunk*> GetAdjacentChunks(ChunkID chunk_id, std::map<ChunkID, Chunk*, Vec2Comparator> chunks_to_check_new,
		std::map<ChunkID, Chunk*, Vec2Comparator> chunks_to_check_existing);

	std::map<ChunkDirection::AdjacentChunk, Chunk*> GetAdjacentExistingChunks(ChunkID chunk);

	glm::vec2 GetChunkDistance(ChunkID chunk_id, ChunkID other_id);

private:
	// Load a chunk - determine block types by noise_map, which faces
	// to render, and load geometry.
	Chunk* LoadChunk(Chunk* chunk);

	Chunk* GetChunkOrNull(ChunkID chunk_id);
};