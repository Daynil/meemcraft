#pragma once
#include <string>
#include <vector>

#include "rendering/texture.h"

// This is the raw model with just the vertex data
class RawModel
{
public:
	unsigned int VAO_ID;
	unsigned int vertex_count;

	RawModel() {};
	RawModel(const std::vector<float>& vertex_positions, const std::vector<float>& vertex_texture_uvs, const std::vector<unsigned int >& vertex_indices);

	void Delete();

private:
	unsigned int VBOs[2];
	unsigned int EBO;

	std::vector<float> vertex_positions;
	std::vector<float> vertex_texture_uvs;
	std::vector<unsigned int> vertex_indices;
};