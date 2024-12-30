#include "map_generator.h"

#include <iostream>
#include <vector>
#include <cmath>

#include <stb_image/stb_image.h>
#include <glm/glm.hpp>

#include "resource_manager.h"

#include "entity.h"


MapGenerator::MapGenerator(Renderer* renderer) : renderer(renderer)
{
	// Just a front-facing quad to render the noise map on
	raw_model = new RawModel(NoisemapVertices::vertices, NoisemapVertices::texture_coords, NoisemapVertices::indices);
	shader = new Shader(RESOURCES_PATH "shaders/noise_map.shader");
}

MapGenerator::~MapGenerator()
{
	delete raw_model;
	delete shader;
	delete texture;
}

std::vector<double> MapGenerator::GenerateMap(int chunk_size, double frequency, unsigned int octaves, unsigned int seed)
{
	frequency = std::clamp(frequency, 0.1, 64.0);
	octaves = std::clamp(octaves, 1u, 16u);

	std::vector<double> noise_data(chunk_size * chunk_size);

	const siv::PerlinNoise perlin{ seed };
	const double fx = (frequency / chunk_size); // aka width
	const double fy = (frequency / chunk_size); // aka height

	for (int y = 0; y < chunk_size; ++y) {
		for (int x = 0; x < chunk_size; ++x) {
			// RGB = 3 channels
			int noise_index = (y * chunk_size + x);
			// Noise is 0 - 1, convert to rgb up to 255
			noise_data[noise_index] = perlin.octave2D_01((x * fx), (y * fy), octaves);
		}
	}

	return noise_data;
}

void MapGenerator::CreateNoisemapTexture(std::vector<double> noisemap)
{
	auto chunk_size = std::sqrt(noisemap.size());

	// RGB = 3 channels
	// We use unsigned char because each char is 8 bits, aka 1 byte
	// In an image, each pixel is 3 bytes, with each byte corresponding to 
	// an rgb color value from 0 to 255.
	// Bytes can store a value up to exactly 255, which is 11111111 in binary.
	// Thus, unsigned chars are a natural choice for image data.
	std::vector<unsigned char> texture_noise_data(chunk_size * chunk_size * 3);

	for (int y = 0; y < chunk_size; ++y) {
		for (int x = 0; x < chunk_size; ++x) {
			// RGB = 3 channels
			int texture_index = (y * chunk_size + x) * 3;
			const double noise_value = noisemap[y * chunk_size + x];
			// Noise is 0 - 1, convert to rgb up to 255
			unsigned char noise_c = static_cast<unsigned char>(noise_value * 255);

			texture_noise_data[texture_index] = noise_c;
			texture_noise_data[texture_index + 1] = noise_c;
			texture_noise_data[texture_index + 2] = noise_c;
		}
	}

	texture = new Texture(texture_noise_data.data(), chunk_size, chunk_size);
}

void MapGenerator::DrawNoisemap()
{
	Entity noisemap(
		raw_model,
		texture,
		glm::vec3(1, 1, -2),
		glm::vec3(0),
		glm::vec3(10));
	renderer->render(noisemap, *shader);
}
