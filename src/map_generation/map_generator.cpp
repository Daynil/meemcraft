#include "map_generator.h"

#include <iostream>
#include <vector>
#include <cmath>

#include <stb_image/stb_image.h>
#include <glm/glm.hpp>

#include "resource_manager.h"

#include "entity.h"
#include "block_data.h"


MapGenerator::MapGenerator(RenderingManager* rendering_manager, unsigned int seed) : rendering_manager(rendering_manager), perlin(seed)
{
	// Just a front-facing quad to render the noise map on
	raw_model = new RawModel(BlockVertices::vertices_front, BlockVertices::texture_coords_symmetrical_face, BlockVertices::indices_face);
	shader = new Shader(RESOURCES_PATH "shaders/noise_map.shader");
}

MapGenerator::~MapGenerator()
{
	delete raw_model;
	delete shader;
	if (texture)
		delete texture;
}

// https://www.youtube.com/watch?v=CSa5O6knuwI
// https://dawnosaur.substack.com/p/how-minecraft-generates-worlds-you
// Note:
// We have to pre-generate a noisemap each time the world offset changes
// for a given view radius. Sampling perlin at per-point instead of pregenerating
// is conceptually simpler, but orders of magnitude slower (presumably due to
// algorithmic optimizations in the implementation).
void MapGenerator::GenerateMap(int size_x, int size_z, int offset_x, int offset_z)
{
	w_offset_x = offset_x;
	w_offset_z = offset_z;

	std::vector<std::vector<double>> noise_data(size_x, std::vector<double>(size_z));
	std::vector<std::vector<double>> noise_patches_data(size_x, std::vector<double>(size_z));

	// 0.1 - 64
	// Continentalness
	double frequency_cont = 0.01;
	// 1 - 16
	int octaves_cont = 4;

	double frequency_patches = 0.1;
	int octaves_patches = 4;

	for (int x = 0; x < size_x; ++x) {
		for (int z = 0; z < size_z; ++z) {
			noise_data[x][z] = perlin.octave2D_11(
				(x + offset_x) * frequency_cont, (z + offset_z) * frequency_cont, octaves_cont
			);
			noise_patches_data[x][z] = perlin.octave2D_11(
				(x + offset_x) * frequency_patches, (z + offset_z) * frequency_patches, octaves_patches
			);
		}
	}

	// https://www.redblobgames.com/maps/terrain-from-noise/

	//// Adjust output value to 0-1 range
	//double max_noise = 0.0;
	//double min_noise = 99.0;

	//for (int x = 0; x < size_x; ++x) {
	//	for (int z = 0; z < size_z; ++z) {
	//		const double nx = (x + offset_x) / static_cast<double>(size_x) - 0.5;
	//		const double ny = (z + offset_z) / static_cast<double>(size_z) - 0.5;

	//		double e = (1.0f * (perlin.noise2D(1 * nx, 1 * ny) / 2 + 0.5) +
	//			0.5f * (perlin.noise2D(2 * nx, 2 * ny) / 2 + 0.5) +
	//			0.25f * (perlin.noise2D(4 * nx, 4 * ny) / 2 + 0.5) +
	//			0.13f * (perlin.noise2D(8 * nx, 8 * ny) / 2 + 0.5) +
	//			0.06f * (perlin.noise2D(16 * nx, 16 * ny) / 2 + 0.5) +
	//			0.03f * (perlin.noise2D(32 * nx, 32 * ny) / 2 + 0.5));
	//		e = e / (1.0f + 0.5f + 0.25f + 0.13f + 0.06f + 0.03f);
	//		e = std::pow(e, 15.0f);

	//		noise_data[x][z] = e;

	//		if (e > max_noise)
	//			max_noise = e;
	//		if (e < min_noise)
	//			min_noise = e;
	//	}
	//}

	//for (int x = 0; x < size_x; ++x) {
	//	for (int z = 0; z < size_z; ++z) {
	//		const double noise_value = noise_data[x][z];

	//		double adj_noise = noise_value - min_noise;
	//		adj_noise = adj_noise / max_noise;

	//		noise_data[x][z] = adj_noise;
	//	}
	//}

	noisemap_cont_data = noise_data;
	noisemap_patches_data = noise_patches_data;
}

void MapGenerator::FollowCamera(glm::vec3 camera_pos)
{
	noisemap.position = glm::vec3(camera_pos.x, camera_pos.y, camera_pos.z - 16);
}

void MapGenerator::Reseed(unsigned int seed)
{
	perlin.reseed(seed);
}

// The concept of offsets is that our noise_data vector always starts at 0,0.
// If we track the world offset, we can sample from the 0,0 indexed array more easily
// using world coordinates by just subtracting the world offset.
NoiseData MapGenerator::SampleNoise(int wx, int wz) const
{
	int ox = wx - w_offset_x;
	int oz = wz - w_offset_z;

	NoiseData data{};
	data.continentalness = noisemap_cont_data[ox][oz];
	data.patches = noisemap_patches_data[ox][oz];

	return data;
}

void MapGenerator::CreateNoisemapTexture()
{
	auto map_dim_size = noisemap_cont_data[0].size();

	// RGB = 3 channels
	// We use unsigned char because each char is 8 bits, aka 1 byte
	// In an image, each pixel is 3 bytes, with each byte corresponding to 
	// an rgb color value from 0 to 255.
	// Bytes can store a value up to exactly 255, which is 11111111 in binary.
	// Thus, unsigned chars are a natural choice for image data.
	std::vector<unsigned char> texture_noise_data(map_dim_size * map_dim_size * 3);

	for (int x = 0; x < map_dim_size; x++) {
		for (int z = 0; z < map_dim_size; z++) {
			// RGB = 3 channels
			int texture_index = (z * map_dim_size + x) * 3;
			// Compress -1 to 1 -> 0 to 1
			const double noise_value = (noisemap_cont_data[x][z] + 1) / 2;

			// Noise is 0 - 1, convert to rgb up to 255
			unsigned char noise_c = static_cast<unsigned char>(noise_value * 255);

			texture_noise_data[texture_index] = noise_c;
			texture_noise_data[texture_index + 1] = noise_c;
			texture_noise_data[texture_index + 2] = noise_c;
		}
	}

	// If we already had a texture, we must delete
	// the old pointer to avoid memory leaks.
	if (texture) {
		delete texture;
		texture = nullptr;
	}
	texture = new Texture(texture_noise_data.data(), map_dim_size, map_dim_size);
}

void MapGenerator::DrawNoisemap(glm::vec3 camera_pos)
{
	noisemap = Entity(
		raw_model,
		texture,
		shader,
		glm::vec3(camera_pos.x, camera_pos.y, camera_pos.z - 30),
		glm::vec3(0),
		glm::vec3(10));

	rendering_manager->ProcessEntity(&noisemap);
}
