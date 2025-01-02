#include "block_data.h"

#include <filesystem>

#include "resource_manager.h"

void BlockLoader::LoadBlocks()
{
	ResourceManager::LoadRawModel("block_symmetrical", RawModel(BlockVertices::vertices, BlockVertices::texture_coords_symmetrical, BlockVertices::indices));
	ResourceManager::LoadRawModel("block_3_part", RawModel(BlockVertices::vertices, BlockVertices::texture_coords_3_part, BlockVertices::indices));

	for (int i = 0; i < BlockType::BLOCK_COUNT; i++)
	{
		BlockType block_type = (BlockType)i;

		// Skip all model stuff for air
		if (block_type == BlockType::AIR) {
			continue;
		}

		std::string type = BlockTypeString[i];

		bool is_symmetrical = !std::filesystem::exists(RESOURCES_PATH "assets/blocks/" + type + "_top.png");

		if (is_symmetrical) {
			ResourceManager::LoadTexture(type, Texture(RESOURCES_PATH "assets/blocks/" + type + ".png"));
		}
		else {
			std::vector<std::string> block;

			block = {
				RESOURCES_PATH "assets/blocks/" + type + "_top.png",
				RESOURCES_PATH "assets/blocks/" + type + "_side.png",
				RESOURCES_PATH "assets/blocks/" + type + "_bottom.png",
			};

			if (block_type == BlockType::GRASS_BLOCK) {
				block.at(2) = RESOURCES_PATH "assets/blocks/dirt.png";
			}
			else {
				bool top_exists = std::filesystem::exists(RESOURCES_PATH "assets/blocks/" + type + "_top.png");
				bool side_exists = std::filesystem::exists(RESOURCES_PATH "assets/blocks/" + type + "_side.png");
				bool bottom_exists = std::filesystem::exists(RESOURCES_PATH "assets/blocks/" + type + "_bottom.png");

				// E.g. oak_log (has top no bottom)
				if (top_exists && !bottom_exists) {
					block.at(2) = block.at(0);
				}
				// E.g. oak_log has a top and side, but side has no _side
				if (!side_exists) {
					block.at(1) = RESOURCES_PATH "assets/blocks/" + type + ".png";
				}
				// TODO: address other exceptions as they come up
			}

			ResourceManager::LoadTexture(type, Texture(block));
		}
		BlockData data;
		data.symmetrical = is_symmetrical;
		ResourceManager::LoadBlockData(
			block_type,
			data
		);
	}
}