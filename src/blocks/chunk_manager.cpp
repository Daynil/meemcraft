#include "chunk_manager.h"

#include <iostream>


std::string ChunkManager::LoadChunk(glm::vec2 world_coord)
{
	// Simulate CPU-intensive work
	std::this_thread::sleep_for(std::chrono::seconds((int)world_coord.x));
	return "Chunk_" + std::to_string(world_coord.x);
}

void ChunkManager::ChunkWorker(glm::vec2 world_coord)
{
	// Immediately calls LoadChunk, and work starts right here.
	auto result = LoadChunk(world_coord);
	{
		// We want to push to the queue from a thread, so we lock it.
		std::lock_guard<std::mutex> lock(queue_mutex);
		// Push the completed chunk to our queue for ProcessChunks to handle.
		chunk_queue.push(result);
	}
}

void ChunkManager::QueueChunk(glm::vec2 world_coord)
{
	std::thread(&ChunkManager::ChunkWorker, this, world_coord).detach();
}


void ChunkManager::ProcessChunks()
{
	// Briefly lock the queue to check if we have any new chunks completed.
	std::unique_lock<std::mutex> lock(queue_mutex);

	std::vector<std::string> completed_chunks;
	while (!chunk_queue.empty())
	{
		// If we have one, grab it.
		// Use move to avoid copying chunks, just taking ownership of them.
		completed_chunks.push_back(std::move(chunk_queue.front()));
		// Since we've handled it, we can remove this chunk from the queue.
		chunk_queue.pop();
	}

	// Now that we have all our completed chunks, unlock the queue to avoid 
	// holding it up while we use the chunks.
	lock.unlock();

	for (auto& chunk : completed_chunks) {
		std::cout << "Processed " << chunk << std::endl;
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

	for (int cx = 0; cx < chunks_per_side; cx++) {
		for (int cz = 0; cz < chunks_per_side; cz++) {
			glm::vec2 world_coord = glm::vec2(cx, cz);
			if (chunks.contains(world_coord)) {
				// TODO: eventually load chunk vertices into GPU here
				// AKA chunk.load()
				continue;
			}

			for (int z = 0; z < Chunk::CHUNK_SIZE_Z; z++) {
				int row_start = (cz * Chunk::CHUNK_SIZE_Z + z) * map_size.x + (cx * Chunk::CHUNK_SIZE_X);

				chunk_map_data.insert(
					chunk_map_data.end(),
					noise_map.begin() + row_start,
					noise_map.begin() + row_start + Chunk::CHUNK_SIZE_X
				);
			}

			chunks.emplace(
				world_coord,
				new Chunk(
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
	for (int cx = 0; cx < chunks_per_side; cx++) {
		for (int cz = 0; cz < chunks_per_side; cz++) {
			glm::vec2 world_coord = glm::vec2(cx, cz);
			//if (chunks.contains(world_coord)) {
			//	// TODO: eventually load chunk vertices into GPU here
			//	// AKA chunk.load()
			//	continue;
			//}

			auto chunk = chunks.at(world_coord);
			chunk->adjacent_chunks = GetAdjacentChunks(world_coord);

			chunk->GenerateMesh();
		}
	}
}

std::map<ChunkDirection::AdjacentChunk, Chunk*> ChunkManager::GetAdjacentChunks(glm::vec2 world_coord)
{
	std::map<ChunkDirection::AdjacentChunk, Chunk*> adjacent_chunks;

	auto get_chunk_or_null = [this](const glm::vec2& coord) -> Chunk* {
		auto it = chunks.find(coord);
		if (it == chunks.end())
			return nullptr;
		else
			return it->second;
	};

	adjacent_chunks.emplace(
		ChunkDirection::AdjacentChunk::FRONT,
		get_chunk_or_null(glm::vec2(world_coord.x, world_coord.y + 1))
	);
	adjacent_chunks.emplace(
		ChunkDirection::AdjacentChunk::BACK,
		get_chunk_or_null(glm::vec2(world_coord.x, world_coord.y - 1))
	);
	adjacent_chunks.emplace(
		ChunkDirection::AdjacentChunk::LEFT,
		get_chunk_or_null(glm::vec2(world_coord.x - 1, world_coord.y))
	);
	adjacent_chunks.emplace(
		ChunkDirection::AdjacentChunk::RIGHT,
		get_chunk_or_null(glm::vec2(world_coord.x + 1, world_coord.y))
	);

	return adjacent_chunks;
}
