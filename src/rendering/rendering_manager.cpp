#include "rendering_manager.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

		for (auto it = blocks.begin(); it != blocks.end(); ++it) {
			auto block = *it;
			if (it == blocks.begin()) {
				renderer->prepare_entity(*block);
			}

			// Per-entity functions
			renderer->render(*block);

			// Functions shared by whole group
			if (std::next(it) == blocks.end()) {
				renderer->cleanup_entity(*block);
			}
		}
	}

	for (const auto& entity : entities) {
		renderer->prepare_entity(*entity);
		renderer->render(*entity);
		renderer->cleanup_entity(*entity);
	}

	block_groups.clear();
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

void RenderingManager::ProcessEntity(Entity* entity)
{
	entities.push_back(entity);
}

void RenderingManager::ShouldBlockRender(Block* block)
{

}

