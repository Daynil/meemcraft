#include "map_generator.h"

#include <iostream>
#include <vector>
#include <cmath>

#include <stb_image/stb_image.h>
#include <glm/glm.hpp>

#include "resource_manager.h"

#include "entity.h"
#include "block_data.h"


MapGenerator::MapGenerator(RenderingManager* rendering_manager) : rendering_manager(rendering_manager)
{
	// Just a front-facing quad to render the noise map on
	raw_model = new RawModel(BlockVertices::vertices_front, BlockVertices::texture_coords_symmetrical_face, BlockVertices::indices_face);
	shader = new Shader(RESOURCES_PATH "shaders/noise_map.shader");
}

MapGenerator::~MapGenerator()
{
	delete raw_model;
	delete shader;
	delete texture;
}

// https://www.redblobgames.com/maps/terrain-from-noise/
std::vector<double> MapGenerator::GenerateMap(int map_size, unsigned int seed)
{
	std::vector<double> noise_data(map_size * map_size);

	const siv::PerlinNoise perlin{ seed };

	for (int y = 0; y < map_size; ++y) {
		for (int x = 0; x < map_size; ++x) {
			const double nx = x / static_cast<double>(map_size) - 0.5;
			const double ny = y / static_cast<double>(map_size) - 0.5;

			int noise_index = (y * map_size + x);

			double e = (1.0f * (perlin.noise2D(1 * nx, 1 * ny) / 2 + 0.5) +
				0.5f * (perlin.noise2D(2 * nx, 2 * ny) / 2 + 0.5) +
				0.25f * (perlin.noise2D(4 * nx, 4 * ny) / 2 + 0.5) +
				0.13f * (perlin.noise2D(8 * nx, 8 * ny) / 2 + 0.5) +
				0.06f * (perlin.noise2D(16 * nx, 16 * ny) / 2 + 0.5) +
				0.03f * (perlin.noise2D(32 * nx, 32 * ny) / 2 + 0.5));
			e = e / (1.0f + 0.5f + 0.25f + 0.13f + 0.06f + 0.03f);
			e = std::pow(e, 15.0f);

			noise_data[noise_index] = e;
			//noise_data[noise_index] = perlin.octave2D_01((x * nx), (y * ny), octaves);
		}
	}

	// Adjust output value to 0-1 range
	double max_noise = 0.0;
	double min_noise = 99.0;

	for (int y = 0; y < map_size; ++y) {
		for (int x = 0; x < map_size; ++x) {
			int noise_index = (y * map_size + x);
			const double noise_value = noise_data[noise_index];
			if (noise_value > max_noise)
				max_noise = noise_value;
			if (noise_value < min_noise)
				min_noise = noise_value;
		}
	}

	for (int y = 0; y < map_size; ++y) {
		for (int x = 0; x < map_size; ++x) {
			int noise_index = (y * map_size + x);
			const double noise_value = noise_data[noise_index];

			double adj_noise = noise_value - min_noise;
			adj_noise = adj_noise / max_noise;

			noise_data[noise_index] = adj_noise;
		}
	}

	return noise_data;
}

void MapGenerator::CreateNoisemapTexture(std::vector<double> noisemap)
{
	auto map_dim_size = std::sqrt(noisemap.size());

	// RGB = 3 channels
	// We use unsigned char because each char is 8 bits, aka 1 byte
	// In an image, each pixel is 3 bytes, with each byte corresponding to 
	// an rgb color value from 0 to 255.
	// Bytes can store a value up to exactly 255, which is 11111111 in binary.
	// Thus, unsigned chars are a natural choice for image data.
	std::vector<unsigned char> texture_noise_data(map_dim_size * map_dim_size * 3);

	for (int y = 0; y < map_dim_size; y++) {
		for (int x = 0; x < map_dim_size; x++) {
			// RGB = 3 channels
			int texture_index = (y * map_dim_size + x) * 3;
			const double noise_value = noisemap[y * map_dim_size + x];

			// Noise is 0 - 1, convert to rgb up to 255
			unsigned char noise_c = static_cast<unsigned char>(noise_value * 255);

			texture_noise_data[texture_index] = noise_c;
			texture_noise_data[texture_index + 1] = noise_c;
			texture_noise_data[texture_index + 2] = noise_c;
		}
	}

	texture = new Texture(texture_noise_data.data(), map_dim_size, map_dim_size);
}

void MapGenerator::DrawNoisemap()
{
	noisemap = Entity(
		raw_model,
		texture,
		shader,
		glm::vec3(0, 0, -16),
		glm::vec3(0),
		glm::vec3(10));

	rendering_manager->ProcessEntity(&noisemap);
}
