#include "resource_manager.h"

#include <glad/glad.h>

std::map<std::string, RawModel> ResourceManager::RawModels;
std::map<std::string, Shader> ResourceManager::Shaders;
std::map<std::string, Texture> ResourceManager::Textures;
std::map<std::string, Sound> ResourceManager::Sounds;
BlockData ResourceManager::BlockDataMap[(int)BlockType::BLOCK_COUNT];

float ResourceManager::texture_atlas_x_unit = 0.0f;

Shader& ResourceManager::LoadShader(std::string name, Shader shader)
{
	Shaders.emplace(name, shader);
	return Shaders.at(name);
}

Shader& ResourceManager::GetShader(std::string name)
{
	return Shaders.at(name);
}

Texture& ResourceManager::LoadTexture(std::string name, Texture texture)
{
	Textures.emplace(name, texture);
	return Textures.at(name);
}

Texture& ResourceManager::GetTexture(std::string name)
{
	return Textures.at(name);
}

RawModel& ResourceManager::LoadRawModel(std::string name, RawModel model)
{
	RawModels.emplace(name, model);
	return RawModels.at(name);
}

RawModel& ResourceManager::GetRawModel(std::string name)
{
	return RawModels.at(name);
}

Sound& ResourceManager::LoadRSound(std::string name, std::string sound_path)
{
	Sounds.emplace(name, LoadSound(sound_path.c_str()));
	return Sounds.at(name);
}

Sound& ResourceManager::GetSound(std::string name)
{
	return Sounds.at(name);
}

BlockData& ResourceManager::LoadBlockData(BlockType type, BlockData data)
{
	BlockDataMap[type] = data;
	return BlockDataMap[type];
	//BlockDataMap.emplace(type, data);
	//return BlockDataMap.at(type);
}

BlockData& ResourceManager::GetBlockData(BlockType type)
{
	return BlockDataMap[type];
}

void ResourceManager::Clear()
{
	for (auto& iter : Shaders)
		iter.second.deleteShader();
	for (auto& iter : RawModels)
		iter.second.Delete();
	for (auto& iter : Textures)
		iter.second.Delete();
}
