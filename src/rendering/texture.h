#pragma once

#include <string>
#include <vector>

class Texture
{
public:
	unsigned int textureID;

	// Loads an image as a single texture
	Texture(std::string texturePath);
	// Loads multiple images as a single texture,
	// stacked horizontally by default.
	// These are stacked left to right the order of the vector.
	Texture(std::vector<std::string> texturePaths, bool stack_horizontal = true);
	// Loads raw byte data as a single texture
	Texture(unsigned char* raw_data, int width_px, int height_px);

	void Delete();
};