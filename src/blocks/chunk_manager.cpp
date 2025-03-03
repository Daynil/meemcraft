#include "chunk_manager.h"''

#include <iostream>

using namespace ChunkHelpers;

Chunk* ChunkManager::LoadChunk(Chunk* chunk)
{
	chunk->GenerateMesh();
	chunk->should_render = true;
	return chunk;
}

void ChunkManager::RefreshChunksCenteredAt(glm::vec2 position)
{
	int pos_x = position.x;
	int pos_z = position.y;

	int cx = pos_x / CHUNK_SIZE_X;
	int cz = pos_z / CHUNK_SIZE_Z;

	std::vector<CoordMap> chunks_to_gen;

	// Skip chunks already being displayed
	// Coords relative to those we're generating
	int rx = 0;
	int rz = 0;
	// World coords
	for (int wx = cx - VIEW_DIST_CHUNKS; wx < cx + VIEW_DIST_CHUNKS; wx++) {
		for (int wz = cz - VIEW_DIST_CHUNKS; wz < cz + VIEW_DIST_CHUNKS; wz++) {
			ChunkID chunk_id = glm::vec2(wx, wz);
			if (!GetChunkOrNull(chunk_id)) {
				chunks_to_gen.push_back(CoordMap{ glm::vec2(rx, rz), chunk_id });
			}
			rz++;
		}
		rx++;
		rz = 0;
	}

	if (chunks_to_gen.size() == 0)
		return;

	// We need to remesh chunks adjacent to newly inserted ones
	// since they may have culled faces where they shouldn't or vice versa
	std::vector<CoordMap> adjacent_chunks_to_remesh;
	for (auto& chunk_coord_map : chunks_to_gen) {
		auto adjacent_chunks = GetAdjacentExistingChunks(chunk_coord_map.world_coord);
		for (auto& adjacent_chunk_p : adjacent_chunks) {
			auto* adjacent_chunk = adjacent_chunk_p.second;
			if (adjacent_chunk) {
				auto dist = GetChunkDistance(chunk_coord_map.world_coord, adjacent_chunk->id);
				adjacent_chunks_to_remesh.push_back(
					CoordMap{ glm::vec2(chunk_coord_map.relative_coord.x + dist.x, chunk_coord_map.relative_coord.y + dist.y),
					adjacent_chunk->id }
				);
			}
		}
	}

	chunks_to_gen.insert(chunks_to_gen.end(), adjacent_chunks_to_remesh.begin(), adjacent_chunks_to_remesh.end());

	// Multiple chunks can have the same adjacent chunk, so remove duplicates first
	std::sort(chunks_to_gen.begin(), chunks_to_gen.end());
	auto it = std::unique(chunks_to_gen.begin(), chunks_to_gen.end());
	chunks_to_gen.resize(std::distance(chunks_to_gen.begin(), it));

	{
		std::unique_lock<std::mutex> lock(queue_mutex);
		chunks_initial_data_queue.push({ glm::vec2(cx, cz), chunks_to_gen });
		print("pushed to queue");
	}
}

void ChunkManager::CreateInitialChunkData(std::tuple<glm::vec2, std::vector<CoordMap>> point_coord_map)
{
	auto& center_point = std::get<0>(point_coord_map);
	auto& chunks_to_gen = std::get<1>(point_coord_map);

	// This becomes an issue if we have flown well outside of the currently loaded 
	// area and lots of chunks are queued up.
	// Make noisemap size bigger than we need - quick to gen, and if we have 
	// moved much further than the loaded area, still works.
	// Shouldn't be an issue in the real game with slower movement.
	auto map_size = CHUNK_SIZE_X * VIEW_DIST_CHUNKS * 2 + (CHUNK_SIZE_X * 4);

	int offset_x = (center_point.x * CHUNK_SIZE_X) - (map_size / 2);
	int offset_z = (center_point.y * CHUNK_SIZE_Z) - (map_size / 2);

	map_generator->GenerateMap(
		map_size, map_size, offset_x, offset_z
	);

	{
		std::unique_lock<std::mutex> lock(queue_mutex);
		noisemap_data_generated = true;
	}

	int view_distance = 10;
	int chunks_per_side = view_distance * 2 + 1;

	std::map<ChunkID, Chunk*, Vec2Comparator> local_chunks_to_queue;

	for (auto& chunk_coord_map : chunks_to_gen) {
		local_chunks_to_queue.emplace(
			chunk_coord_map.world_coord,
			new Chunk(
				chunk_coord_map.world_coord,
				glm::vec3(
					(chunk_coord_map.world_coord.x * CHUNK_SIZE_X),
					// Shift sea level to y = 0
					-(CHUNK_SIZE_Y / 2),
					(chunk_coord_map.world_coord.y * CHUNK_SIZE_Z)
				),
				map_generator
			)
		);
	}

	for (auto& p_chunk : local_chunks_to_queue) {
		auto chunk = p_chunk.second;
		chunk->adjacent_chunks = GetAdjacentChunks(chunk->id, local_chunks_to_queue, chunks);
	}

	{
		std::unique_lock<std::mutex> lock(queue_mutex);
		for (auto& p_chunk : local_chunks_to_queue) {
			auto chunk = p_chunk.second;
			chunks_cpu_queue.push(chunk);
		}
	}
}

void ChunkManager::ProcessChunks()
{
	// Handle initial queue
	// If we're not already processing a batch, start the pipeline
	{
		std::unique_lock<std::mutex> lock(queue_mutex);
		if (!batch_processing && !chunks_initial_data_queue.empty()) {
			batch_processing = true;

			std::tuple<glm::vec2, std::vector<CoordMap>> data = std::move(chunks_initial_data_queue.front());
			chunks_initial_data_queue.pop();

			chunks_in_batch = std::get<1>(data).size();

			thread_pool->enqueue([this, data]() {
				CreateInitialChunkData(data);
			});
		}

		if (noisemap_data_generated) {
			// Note: debug only
			// We create our noisemaps in a background thread, but OpenGL needs
			// textures generated on the main thread.
			// We create one noisemap per batch.
			map_generator->CreateNoisemapTexture();
			noisemap_data_generated = false;
		}
	}

	// Handle CPU queue
	const int max_load_per_frame = 3;

	{
		// QueueChunk does minimal work on the main thread (just queues to thread pool), 
		// so we can lock the thread quickly here
		std::unique_lock<std::mutex> lock(queue_mutex);
		int num_to_load = std::min(max_load_per_frame, (int)chunks_cpu_queue.size());
		for (int i = 0; i < num_to_load; i++) {
			QueueChunk(std::move(chunks_cpu_queue.front()));
			chunks_cpu_queue.pop();
		}
	}

	// Handle GPU queue
	const int max_upload_per_frame = 3;

	{
		std::unique_lock<std::mutex> lock(queue_mutex);
		int num_to_upload = std::min(max_upload_per_frame, (int)chunks_gpu_queue.size());
		lock.unlock();

		for (int i = 0; i < num_to_upload; i++) {
			// Lock carefully since LoadToGPU takes a while
			lock.lock();
			Chunk* chunk = std::move(chunks_gpu_queue.front());
			chunks_gpu_queue.pop();
			lock.unlock();

			chunk->LoadToGPU();

			auto it = chunks.find(chunk->id);
			if (it != chunks.end()) {
				// Replacing an existing chunk.
				// Must free the old pointer to avoid memory leaks.
				delete it->second;
				it->second = nullptr;
			}
			chunks.insert_or_assign(chunk->id, chunk);

			lock.lock();
			chunks_in_batch_complete++;
			lock.unlock();
		}
	}

	// Once all queues for a batch are empty, mark the batch complete so a new one
	// can start.
	{
		std::unique_lock<std::mutex> lock(queue_mutex);
		bool batch_complete = chunks_in_batch == chunks_in_batch_complete;
		int batches_left = chunks_initial_data_queue.size();
		if (batch_processing && batch_complete) {
			print("finished batch, batches remaining: " + std::to_string(batches_left));
			chunks_in_batch_complete = 0;
			batch_processing = false;

			// Only clear chunks when it is safe to do so - we have no more batches
			// currently in process. If we clear them while batches are processing, 
			// we get dangling pointers.
			if (batches_left == 0) {
				ClearChunksOutOfView();
			}
		}
	}
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
			chunks_gpu_queue.push(result);
		}
	});
}

void ChunkManager::ClearChunks()
{
	for (auto& p_chunk : chunks) {
		delete p_chunk.second;
	}
	chunks.clear();
}

void ChunkManager::ClearChunksOutOfView()
{
	int cx = camera->cameraPos.x / CHUNK_SIZE_X;
	int cz = camera->cameraPos.z / CHUNK_SIZE_Z;

	for (auto it = chunks.begin(); it != chunks.end(); )
	{
		auto* chunk = it->second;
		// Preserve a couple of chunks outside of view dist
		// since we're more likely to backtrack to existing locations.
		bool chunk_out_of_view_dist =
			chunk->id.x < cx - VIEW_DIST_CHUNKS - 2
			|| chunk->id.x > cx + VIEW_DIST_CHUNKS + 2
			|| chunk->id.y < cz - VIEW_DIST_CHUNKS - 2
			|| chunk->id.y > cz + VIEW_DIST_CHUNKS + 2;

		if (chunk_out_of_view_dist) {
			delete chunk;
			it = chunks.erase(it);
		}
		else {
			++it;
		}
	}
}

std::map<ChunkHelpers::AdjacentChunk, Chunk*> ChunkManager::GetAdjacentChunks(
	ChunkID chunk_id, std::map<ChunkID, Chunk*, Vec2Comparator> chunks_to_check_new,
	std::map<ChunkID, Chunk*, Vec2Comparator> chunks_to_check_existing
)
{
	std::map<ChunkHelpers::AdjacentChunk, Chunk*> adjacent_chunks;

	auto get_chunk_or_null = [chunks_to_check_new, chunks_to_check_existing](
		const glm::vec2& coord) -> Chunk* {
		auto it_new = chunks_to_check_new.find(coord);
		auto it_existing = chunks_to_check_existing.find(coord);
		if (it_new == chunks_to_check_new.end() && it_existing == chunks_to_check_existing.end())
			return nullptr;
		else if (it_new == chunks_to_check_new.end()) {
			return it_existing->second;
		}
		else
			return it_new->second;
	};

	for (int i = 0; i < ChunkHelpers::AdjacentChunk::COUNT; i++)
	{
		ChunkHelpers::AdjacentChunk dir = (ChunkHelpers::AdjacentChunk)i;
		adjacent_chunks.emplace(
			dir,
			get_chunk_or_null(GetAdjacentChunkID(chunk_id, dir))
		);
	}

	return adjacent_chunks;
}

std::map<ChunkHelpers::AdjacentChunk, Chunk*> ChunkManager::GetAdjacentExistingChunks(ChunkID chunk_id)
{
	std::map<ChunkHelpers::AdjacentChunk, Chunk*> adjacent_chunks;

	for (int i = 0; i < ChunkHelpers::AdjacentChunk::COUNT; i++)
	{
		ChunkHelpers::AdjacentChunk dir = (ChunkHelpers::AdjacentChunk)i;
		adjacent_chunks.emplace(
			dir,
			GetChunkOrNull(GetAdjacentChunkID(chunk_id, dir))
		);
	}

	return adjacent_chunks;
}

ChunkID ChunkManager::GetAdjacentChunkID(ChunkID chunk_id, ChunkHelpers::AdjacentChunk direction)
{
	switch (direction)
	{
	case ChunkHelpers::FRONT:
		return glm::vec2(chunk_id.x, chunk_id.y + 1);
	case ChunkHelpers::BACK:
		return glm::vec2(chunk_id.x, chunk_id.y - 1);
	case ChunkHelpers::LEFT:
		return glm::vec2(chunk_id.x - 1, chunk_id.y);
	case ChunkHelpers::RIGHT:
		return glm::vec2(chunk_id.x + 1, chunk_id.y);
	default:
		break;
	}
}

glm::vec2 ChunkManager::GetChunkDistance(ChunkID chunk_id, ChunkID other_id)
{
	return glm::vec2(other_id.x - chunk_id.x, other_id.y - chunk_id.y);
}

Chunk* ChunkManager::GetChunkOrNull(ChunkID chunk_id)
{
	auto it = chunks.find(chunk_id);
	if (it == chunks.end())
		return nullptr;
	else
		return it->second;
}