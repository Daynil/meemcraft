#include "chunk_manager.h"

#include <iostream>


Chunk* ChunkManager::LoadChunk(Chunk* chunk)
{
	chunk->GenerateMesh();
	chunk->should_render = true;
	return chunk;
}

void ChunkManager::UploadCompletedChunks()
{
	const int max_upload_per_frame = 20;

	int num_to_upload = std::min(max_upload_per_frame, (int)chunks_gpu_queue.size());

	for (int i = 0; i < num_to_upload; i++) {
		Chunk* chunk = chunks_gpu_queue[i];
		chunk->model->LoadToGPU();
		chunks.emplace(chunk->id, chunk);
	}

	chunks_gpu_queue.erase(
		chunks_gpu_queue.begin(),
		chunks_gpu_queue.begin() + num_to_upload
	);
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
		chunks_gpu_queue.push_back(chunk);
	}
}

void ChunkManager::QueueChunks()
{
	// TODO:
	// Make this async
	// View frustrum culling
	int view_distance = 10;
	int chunks_per_side = view_distance * 2 + 1;
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
	// so we can reference adjacent chunk block types.
	for (auto& p_chunk : chunks_pending) {
		auto chunk = p_chunk.second;
		// TODO: eventually we should combine all chunks and these pending ones
		// so we can have access to ones already loaded too.
		chunk->adjacent_chunks = GetAdjacentChunks(chunk->id, chunks_pending);
		chunks_cpu_queue.push_back(chunk);
	}
}

void ChunkManager::LoadChunks()
{
	const int max_load_per_frame = 3;

	int num_to_load = std::min(max_load_per_frame, (int)chunks_cpu_queue.size());

	for (int i = 0; i < num_to_load; i++) {
		QueueChunk(chunks_cpu_queue[i]);
	}

	chunks_cpu_queue.erase(
		chunks_cpu_queue.begin(),
		chunks_cpu_queue.begin() + num_to_load
	);
}

void ChunkManager::ClearChunks()
{
	for (auto& p_chunk : chunks) {
		delete p_chunk.second;
	}
	noise_map.clear();
	chunks.clear();
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
