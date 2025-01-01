#pragma once

#include <vector>

#include "perlin_noise.hpp"

#include "rendering/rendering_manager.h"
#include "model.h"
#include "rendering/texture.h"
#include "rendering/shader.h"

namespace NoisemapVertices
{
	const std::vector<float> vertices = {
		// Front face
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
	};

	const std::vector<float> texture_coords = {
		0, 0,                            // Bottom-left
		0, 1,                            // Top-left
		1, 1,                            // Top-right
		1, 0,                            // Bottom-right
	};

	const std::vector<unsigned int> indices = {
		// Front face
		0,1,3,
		3,1,2,
	};
}

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

	std::vector<double> GenerateMap(int chunk_size, double frequency, unsigned int octaves, unsigned int seed);

	void CreateNoisemapTexture(std::vector<double> noisemap);
	void DrawNoisemap();
};