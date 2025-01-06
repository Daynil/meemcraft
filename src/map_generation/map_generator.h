#pragma once

#include <vector>

#include "perlin_noise.hpp"

#include "rendering/rendering_manager.h"
#include "model.h"
#include "rendering/texture.h"
#include "rendering/shader.h"


class MapGenerator
{
public:
	RenderingManager* rendering_manager;

	RawModel* raw_model;
	Shader* shader;
	Texture* texture;

	Entity noisemap;

	MapGenerator() = default;
	MapGenerator(RenderingManager* rendering_manager);
	~MapGenerator();

	std::vector<double> GenerateMap(int map_size, unsigned int seed);

	void CreateNoisemapTexture(std::vector<double> noisemap);
	void DrawNoisemap();
};