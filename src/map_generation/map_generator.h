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
	Texture* texture = nullptr;

	// Rendered noisemap's current offset vs. world coords
	int w_offset_x = 0;
	int w_offset_z = 0;
	std::vector<std::vector<double>> noisemap_data;
	Entity noisemap;

	siv::PerlinNoise perlin;
	// 0.1 - 64
	double frequency = 0.01;
	// 1 - 16
	int octaves = 4;

	MapGenerator() = default;
	MapGenerator(RenderingManager* rendering_manager, unsigned int seed);
	~MapGenerator();

	void GenerateMap(int size_x, int size_z, int offset_x, int offset_z);

	void FollowCamera(glm::vec3 camera_pos);

	void Reseed(unsigned int seed);
	double SampleNoise(int x, int z) const;

	void CreateNoisemapTexture();
	void DrawNoisemap(glm::vec3 camera_pos);
};