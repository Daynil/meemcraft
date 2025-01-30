#include "rendering_manager.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "util.h"

void RenderingManager::Init(Camera* p_camera)
{
	camera = p_camera;
}

void RenderingManager::Render()
{
	// All entities share these steps each frame
	renderer->prepare();

	for (auto& block_group : block_groups) {
		auto type = block_group.first;
		auto& blocks = block_group.second;

		// Render opaque only in first pass
		if (type == BlockType::WATER || type == BlockType::SELECTED) {
			continue;
		}

		for (auto it = blocks.begin(); it != blocks.end(); ++it) {
			auto block = *it;
			if (it == blocks.begin()) {
				renderer->prepare_entity(*block);
			}

			// Per-entity functions
			renderer->render_entity(*block);

			// Functions shared by whole group
			if (std::next(it) == blocks.end()) {
				renderer->cleanup_entity(*block);
			}
		}
	}

	for (auto it = chunks.begin(); it != chunks.end(); ++it) {
		auto chunk = &(*it)->opaque_entity;
		if (it == chunks.begin()) {
			renderer->prepare_chunk(*chunk);
		}

		// Per-entity functions
		renderer->render_chunk(*chunk);

		// Functions shared by whole group
		if (std::next(it) == chunks.end()) {
			renderer->cleanup_entity(*chunk);
		}
	}

	auto chunks_with_transparency = filter_vector(chunks, [](Chunk* chunk) {
		return chunk->has_transparent_blocks;
	});

	for (
		auto it = chunks_with_transparency.begin();
		it != chunks_with_transparency.end();
		++it
		) {
		auto chunk = &(*it)->transparent_entity;
		if (it == chunks_with_transparency.begin()) {
			renderer->prepare_chunk(*chunk);
		}

		// Per-entity functions
		renderer->render_chunk(*chunk);

		// Functions shared by whole group
		if (std::next(it) == chunks_with_transparency.end()) {
			renderer->cleanup_entity(*chunk);
		}
	}

	for (auto& block_group : block_groups) {
		auto type = block_group.first;
		auto& blocks = block_group.second;

		// Render transparent only in second pass
		if (type != BlockType::WATER && type != BlockType::SELECTED) {
			continue;
		}

		for (auto it = blocks.begin(); it != blocks.end(); ++it) {
			auto block = *it;
			if (it == blocks.begin()) {
				renderer->prepare_entity(*block);
			}

			// Per-entity functions
			renderer->render_entity(*block);

			// Functions shared by whole group
			if (std::next(it) == blocks.end()) {
				renderer->cleanup_entity(*block);
			}
		}
	}

	for (const auto& entity : entities) {
		renderer->prepare_entity(*entity);
		renderer->render_entity(*entity);
		renderer->cleanup_entity(*entity);
	}

	block_groups.clear();
	chunks.clear();
	entities.clear();
}

void RenderingManager::ProcessBlock(Block* block)
{
	auto block_group = block_groups.find(block->type);
	if (block_group == block_groups.end()) {
		auto result = block_groups.emplace(block->type, std::vector<Block*>());
		block_group = result.first;
	}
	block_group->second.push_back(block);
}

void RenderingManager::ProcessChunk(Chunk* chunk)
{
	chunks.push_back(chunk);
}

void RenderingManager::ProcessEntity(Entity* entity)
{
	entities.push_back(entity);
}

void RenderingManager::ShouldBlockRender(Block* block)
{

}

