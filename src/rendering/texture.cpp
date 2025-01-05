#include <iostream>

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <stb_image/stb_image.h>

#include "texture.h"

Texture::Texture(std::string texturePath)
{
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	// Set texture wrapping/filtering options
	// on currently bound texture object.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Load and generate the texture
	int width, height, nChannels;
	unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &nChannels, STBI_rgb_alpha);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
}

// Appends individual image bytes (src_data) to a group of image bytes (dest)
// in row-major order (Row 0 bytes of all images, then Row 1 bytes of all images, etc).
void copy_sub_image(unsigned char* src_data, int src_width, int src_height, std::vector<unsigned char>& dest, int dest_x, int dest_y, int channels, int total_dest_images)
{
	// Copy each row of src into dest
	for (int y = 0; y < src_height; y++) {
		// Where to copy into the merged buffer
		int dest_offset = ((dest_y + y) * src_width * total_dest_images + dest_x) * channels;
		// Where to read from in src
		int src_offset = (y * src_width) * channels;

		// Copy row of channels
		memcpy(&dest[dest_offset], &src_data[src_offset], src_width * channels);
	}
}

Texture::Texture(std::vector<std::string> texturePaths, bool stack_horizontal)
{
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	// Set texture wrapping/filtering options
	// on currently bound texture object.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Each image may have 3 or 4 channels. We're forcing 4 for all here.
	const int channels_desired = STBI_rgb_alpha;

	int width_top, height_top, channels_top;
	unsigned char* data_top = stbi_load(texturePaths[0].c_str(), &width_top, &height_top, &channels_top, channels_desired);

	int width_side, height_side, channels_side;
	unsigned char* data_side = stbi_load(texturePaths[1].c_str(), &width_side, &height_side, &channels_side, channels_desired);

	int width_bottom, height_bottom, channels_bottom;
	unsigned char* data_bottom = stbi_load(texturePaths[2].c_str(), &width_bottom, &height_bottom, &channels_bottom, channels_desired);

	if (!data_top || !data_side || !data_bottom) {
		std::cout << "Failed to load multi-image texture" << std::endl;
	}

	// ---- Create a merged image buffer (horizontal layout) ----
	// For a horizontal layout:
	//   totalWidth = widthTop + widthSide + widthBottom
	//   mergedHeight = max of the three heights (assuming they match or you want padding)
	// Note, simplified for minecraft blocks (same widths/heights)
	int total_width = width_top * 3;
	int merged_height = height_top;

	// RGBA = 4 bytes per pixel (RGB would be 3 per pixel)
	std::vector<unsigned char> merged_pixels(width_top * 3 * height_top * channels_desired, 0);


	copy_sub_image(data_top, width_top, height_top, merged_pixels, 0, 0, channels_desired, 3);
	copy_sub_image(data_side, width_side, height_side, merged_pixels, width_top, 0, channels_desired, 3);
	copy_sub_image(data_bottom, width_bottom, height_bottom, merged_pixels, width_top + width_side, 0, channels_desired, 3);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, total_width, merged_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, merged_pixels.data());
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data_top);
	stbi_image_free(data_side);
	stbi_image_free(data_bottom);
}

Texture::Texture(unsigned char* raw_data, int width_px, int height_px)
{
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	// Set texture wrapping/filtering options
	// on currently bound texture object.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_px, height_px, 0, GL_RGB, GL_UNSIGNED_BYTE, raw_data);
	glGenerateMipmap(GL_TEXTURE_2D);
}

Texture::Texture(std::vector<BlockData> blocks_data, int num_textures)
{
	// Each image may have 3 or 4 channels. We're forcing 4 for all here.
	const int channels_desired = STBI_rgb_alpha;

	int block_dimensions = 128;
	std::vector<unsigned char> merged_pixels(block_dimensions * block_dimensions * num_textures * channels_desired, 0);

	int textures_loaded = 0;
	for (auto& data : blocks_data) {
		int width, height, channels;
		unsigned char* img_data = stbi_load(data.top_texture_path.c_str(), &width, &height, &channels, channels_desired);

		if (!img_data) {
			std::cout << "Failed to load image texture for: " << data.top_texture_path << std::endl;
		}

		copy_sub_image(img_data, width, height, merged_pixels, textures_loaded * block_dimensions, 0, channels_desired, num_textures);

		if (!data.symmetrical) {
			img_data = stbi_load(data.sides_texture_path.c_str(), &width, &height, &channels, channels_desired);

			if (!img_data) {
				std::cout << "Failed to load image texture for: " << data.sides_texture_path << std::endl;
			}

			copy_sub_image(img_data, width, height, merged_pixels, (textures_loaded + 1) * block_dimensions, 0, channels_desired, num_textures);

			img_data = stbi_load(data.bottom_texture_path.c_str(), &width, &height, &channels, channels_desired);

			if (!img_data) {
				std::cout << "Failed to load image texture for: " << data.bottom_texture_path << std::endl;
			}

			copy_sub_image(img_data, width, height, merged_pixels, (textures_loaded + 2) * block_dimensions, 0, channels_desired, num_textures);

			textures_loaded += 3;
		}
		else {
			textures_loaded++;
		}
		stbi_image_free(img_data);
	}

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	// Set texture wrapping/filtering options
	// on currently bound texture object.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, block_dimensions * num_textures, block_dimensions, 0, GL_RGBA, GL_UNSIGNED_BYTE, merged_pixels.data());
	glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture::Delete()
{
	glDeleteTextures(1, &textureID);
}
