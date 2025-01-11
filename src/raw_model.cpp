#include "raw_model.h"

#include <glad/glad.h>

RawModel::RawModel(const std::vector<float>& vertex_positions, const std::vector<float>& vertex_texture_uvs, const std::vector<unsigned int>& vertex_indices, bool defer_load) : vertex_positions(vertex_positions), vertex_texture_uvs(vertex_texture_uvs), vertex_indices(vertex_indices)
{
	vertex_count = vertex_indices.size();
	if (!defer_load)
		LoadToGPU();
}

void RawModel::LoadToGPU()
{
	// Create VAO to store our data in
// VAO = vertex array objects (stores configuration of the attributes)
// The VAO is the top-level storage container
// It stores references to the other information - buffer objects of our vertex positions, textures, indices, etc.
// Using the VAO ID, we can pull up all the other info (which we do in our renderer).
	glGenVertexArrays(1, &VAO_ID);
	glBindVertexArray(VAO_ID);

	// Create EBO for our indices
	// EBO = element buffer objects 
	// This stores indices that OpenGL uses to decide what vertices to draw and in what order (counterclockwise)
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertex_indices.size() * sizeof(unsigned int), vertex_indices.data(), GL_STATIC_DRAW);

	// Create VBO for positions and textures
	// VBO = vertex buffer objects 
	// This stores the actual object vertices
	glGenBuffers(2, VBOs);

	// Positions
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, vertex_positions.size() * sizeof(float), vertex_positions.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
	glEnableVertexAttribArray(0);

	// Texture coords
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
	glBufferData(GL_ARRAY_BUFFER, vertex_texture_uvs.size() * sizeof(float), vertex_texture_uvs.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
	glEnableVertexAttribArray(1);

	// Unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void RawModel::Delete()
{
	glDeleteVertexArrays(1, &VAO_ID);
	glDeleteBuffers(2, VBOs);
	glDeleteBuffers(1, &EBO);
}
