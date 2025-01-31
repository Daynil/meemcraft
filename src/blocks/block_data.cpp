#include "block_data.h"

#include <filesystem>
#include <unordered_set>

#include "resource_manager.h"

void BlockLoader::LoadBlocks()
{
	ResourceManager::LoadRawModel("block_symmetrical", RawModel(BlockVertices::vertices, BlockVertices::texture_coords_symmetrical, BlockVertices::indices));
	ResourceManager::LoadRawModel("block_3_part", RawModel(BlockVertices::vertices, BlockVertices::texture_coords_3_part, BlockVertices::indices));

	std::vector<BlockData> blocks_data;
	int textures_loaded = 0;

	for (int i = 0; i < BlockType::BLOCK_COUNT; i++)
	{
		BlockType block_type = (BlockType)i;
		BlockData data;

		data.type = block_type;

		// Skip all model stuff for air
		if (block_type == BlockType::AIR) {
			ResourceManager::LoadBlockData(
				block_type,
				data
			);
			continue;
		}

		std::string type = BlockTypeString[i];

		bool is_symmetrical = !std::filesystem::exists(RESOURCES_PATH "assets/blocks/" + type + "_top.png") && !std::filesystem::exists(RESOURCES_PATH "assets/blocks" + type + "_bottom.png");

		data.symmetrical = is_symmetrical;

		if (is_symmetrical) {
			data.top_texture_path = RESOURCES_PATH "assets/blocks/" + type + ".png";
			ResourceManager::LoadTexture(type, Texture(data.top_texture_path));
			data.top_texture_num = textures_loaded;
			textures_loaded++;
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

			data.top_texture_path = block.at(0);
			data.sides_texture_path = block.at(1);
			data.bottom_texture_path = block.at(2);

			data.top_texture_num = textures_loaded;
			data.sides_texture_num = textures_loaded + 1;
			data.bottom_texture_num = textures_loaded + 2;
			textures_loaded += 3;
		}

		blocks_data.push_back(data);

		ResourceManager::LoadBlockData(
			block_type,
			data
		);
	}

	images_in_block_texture_atlas = textures_loaded;
	texture_atlas_x_unit = 1.0f / textures_loaded;
	ResourceManager::LoadTexture("block_atlas", Texture(blocks_data, textures_loaded));
}