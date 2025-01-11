#include "chunk_manager.h"

#include <iostream>

Chunk* ChunkManager::LoadChunk(Chunk* chunk)
{
	chunk->GenerateMesh();
	chunk->should_render = true;
	return chunk;
}

void ChunkManager::QueueChunk(Chunk* chunk)
{
	thread_pool->enqueue([this, chunk]() {
		// Immediately calls LoadChunk, and work starts right here.
		// But, doesn't block main thread since we 
		auto result = LoadChunk(chunk);
		{
			// We want to push to the queue from a thread, so we lock it.
			std::lock_guard<std::mutex> lock(queue_mutex);
			// Push the completed chunk to our queue for ProcessChunks to handle.
			chunk_queue.push(result);
		}
	});
}

void ChunkManager::ProcessChunks()
{
	// Briefly lock the queue to check if we have any new chunks completed.
	std::unique_lock<std::mutex> lock(queue_mutex);

	std::vector<Chunk*> completed_chunks;
	while (!chunk_queue.empty())
	{
		// If we have one ready, grab it.
		// Use move to avoid copying chunks, just taking ownership of them.
		completed_chunks.push_back(std::move(chunk_queue.front()));
		// Since we've handled it, we can remove this chunk from the queue.
		chunk_queue.pop();
	}

	// Now that we have all our completed chunks, unlock the queue to avoid 
	// holding it up while we use the chunks.
	lock.unlock();

	for (auto chunk : completed_chunks) {
		//std::cout << "Processed " << chunk << std::endl;
		chunk->model->LoadToGPU();
		chunks.emplace(chunk->id, chunk);
	}
}

void ChunkManager::LoadChunks()
{
	// TODO:
		// make 2d vector of chunks by world addresses
		// 	// so we can look up adjacent chunks and not render adjacent chunk faces
		// // and later, so we can add new chunks as user moves
		// // Create a chunk manager that allows chunks to query adjacent chunk faces
		//  chunk interiors still generating when not needed
		//  view frustrum culling

		// Before adjacent chunk logic, 
		// 8 chunks per side took 369mb of memory
	// Down to 227mb after adjacent chunk logic!
	int chunks_per_side = 8;
	glm::vec3 map_size = glm::vec3(
		Chunk::CHUNK_SIZE_X * chunks_per_side,
		Chunk::CHUNK_SIZE_Y,
		Chunk::CHUNK_SIZE_Z * chunks_per_side
	);

	std::vector<double> chunk_map_data;
	chunk_map_data.reserve(Chunk::CHUNK_SIZE_X * Chunk::CHUNK_SIZE_Z);

	std::map<ChunkID, Chunk*, Vec2Comparator> chunks_pending;

	for (int cx = 0; cx < chunks_per_side; cx++) {
		for (int cz = 0; cz < chunks_per_side; cz++) {
			ChunkID chunk_id = glm::vec2(cx, cz);

			//if (chunks.contains(chunk_id)) {
			//	// TODO: eventually load chunk vertices into GPU here
			//	// AKA chunk.load()
			//	continue;
			//}

			for (int z = 0; z < Chunk::CHUNK_SIZE_Z; z++) {
				int row_start = (cz * Chunk::CHUNK_SIZE_Z + z) * map_size.x + (cx * Chunk::CHUNK_SIZE_X);

				chunk_map_data.insert(
					chunk_map_data.end(),
					noise_map.begin() + row_start,
					noise_map.begin() + row_start + Chunk::CHUNK_SIZE_X
				);
			}

			chunks_pending.emplace(
				chunk_id,
				new Chunk(
					chunk_id,
					glm::vec3(
						(cx * Chunk::CHUNK_SIZE_X),
						// Shift sea level to y = 0
						-(Chunk::CHUNK_SIZE_Y / 2),
						(cz * Chunk::CHUNK_SIZE_Z)
					),
					&chunk_map_data
				)
			);

			chunk_map_data.clear();
		}
	}

	// Only load meshes after all chunk block info has been generated for all chunks
	// so we can reference adjacent chunk block types
	for (auto& p_chunk : chunks_pending) {
		auto chunk = p_chunk.second;
		chunk->adjacent_chunks = GetAdjacentChunks(chunk->id, chunks_pending);
		QueueChunk(chunk);
	}
}

std::map<ChunkDirection::AdjacentChunk, Chunk*> ChunkManager::GetAdjacentChunks(ChunkID chunk_id, std::map<ChunkID, Chunk*, Vec2Comparator> chunks_to_check)
{
	std::map<ChunkDirection::AdjacentChunk, Chunk*> adjacent_chunks;

	auto get_chunk_or_null = [chunks_to_check](const glm::vec2& coord) -> Chunk* {
		auto it = chunks_to_check.find(coord);
		if (it == chunks_to_check.end())
			return nullptr;
		else
			return it->second;
	};

	adjacent_chunks.emplace(
		ChunkDirection::AdjacentChunk::FRONT,
		get_chunk_or_null(glm::vec2(chunk_id.x, chunk_id.y + 1))
	);
	adjacent_chunks.emplace(
		ChunkDirection::AdjacentChunk::BACK,
		get_chunk_or_null(glm::vec2(chunk_id.x, chunk_id.y - 1))
	);
	adjacent_chunks.emplace(
		ChunkDirection::AdjacentChunk::LEFT,
		get_chunk_or_null(glm::vec2(chunk_id.x - 1, chunk_id.y))
	);
	adjacent_chunks.emplace(
		ChunkDirection::AdjacentChunk::RIGHT,
		get_chunk_or_null(glm::vec2(chunk_id.x + 1, chunk_id.y))
	);

	return adjacent_chunks;
}
