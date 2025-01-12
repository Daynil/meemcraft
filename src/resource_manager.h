#pragma once

#include <map>
#include <string>
#include <raudio.h>

#include "rendering/shader.h"
#include "raw_model.h"
#include "rendering/texture.h"
#include "blocks/block.h"
#include "blocks/block_data.h"

class ResourceManager
{
public:
	static std::map<std::string, Shader> Shaders;
	static std::map<std::string, RawModel> RawModels;
	static std::map<std::string, Texture> Textures;
	static std::map<std::string, Sound> Sounds;
	static BlockData BlockDataMap[(int)BlockType::BLOCK_COUNT];

	static float texture_atlas_x_unit;

	static Shader& LoadShader(std::string name, Shader shader);
	static Shader& GetShader(std::string name);

	static Texture& LoadTexture(std::string name, Texture texture);
	static Texture& GetTexture(std::string name);

	static RawModel& LoadRawModel(std::string name, RawModel model);
	static RawModel& GetRawModel(std::string name);

	static Sound& LoadRSound(std::string name, std::string sound_path);
	static Sound& GetSound(std::string name);

	static BlockData& LoadBlockData(BlockType type, BlockData data);
	static BlockData& GetBlockData(BlockType type);

	// Properly deallocate all loaded resources
	static void Clear();
private:
	ResourceManager() {};
};