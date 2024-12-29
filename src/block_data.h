#pragma once

namespace BlockData
{
	//const std::vector<float> vertices_top = {
	//	-0.5f,  0.5f,  0.5f,
	//	-0.5f,  0.5f, -0.5f,
	//	 0.5f,  0.5f, -0.5f,
	//	 0.5f,  0.5f,  0.5f,
	//};

	//const std::vector<float> texture_coords_top = {
	//	0,0,
	//	0,1,
	//	1,1,
	//	1,0
	//};

	//const std::vector<unsigned int> indices_top = {
	//	0,1,3,
	//	3,1,2
	//};

	const std::vector<float> vertices = {
		// Front face
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,

		 // Back face
		 -0.5f,  0.5f,  0.5f,
		 -0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,

		 // Right face
		 0.5f,  0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,

		 // Left face
		 -0.5f,  0.5f, -0.5f,
		 -0.5f, -0.5f, -0.5f,
		 -0.5f, -0.5f,  0.5f,
		 -0.5f,  0.5f,  0.5f,

		 // Top face
		 -0.5f,  0.5f,  0.5f,
		 -0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f,  0.5f,

		 // Bottom face
		 -0.5f, -0.5f,  0.5f,
		 -0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f
	};

	const unsigned int blocks_in_texture = 3;
	const float x_coord_unit = 1.0f / blocks_in_texture;


	const std::vector<float> texture_coords = {
		// Front face (Grass + Dirt)
		x_coord_unit, 0,                    // Bottom-left of Grass+Dirt
		x_coord_unit, 1,           // Top-left of Grass+Dirt
		x_coord_unit * 2, 1,       // Top-right of Grass+Dirt
		x_coord_unit * 2, 0,   // Bottom-right of Grass+Dirt

		// Back face (Grass + Dirt)
		x_coord_unit, 0,                    // Bottom-left of Grass+Dirt
		x_coord_unit, 1,           // Top-left of Grass+Dirt
		x_coord_unit * 2, 1,       // Top-right of Grass+Dirt
		x_coord_unit * 2, 0,   // Bottom-right of Grass+Dirt

		// Right face (Grass + Dirt)
		x_coord_unit, 0,                    // Bottom-left of Grass+Dirt
		x_coord_unit, 1,           // Top-left of Grass+Dirt
		x_coord_unit * 2, 1,       // Top-right of Grass+Dirt
		x_coord_unit * 2, 0,   // Bottom-right of Grass+Dirt

		// Left face (Grass + Dirt)
		x_coord_unit, 0,                    // Bottom-left of Grass+Dirt
		x_coord_unit, 1,           // Top-left of Grass+Dirt
		x_coord_unit * 2, 1,       // Top-right of Grass+Dirt
		x_coord_unit * 2, 0,   // Bottom-right of Grass+Dirt

		// Top face (Grass)
		0, 0,                             // Bottom-left of Grass
		0, 1,                    // Top-left of Grass
		x_coord_unit, 1,           // Top-right of Grass
		x_coord_unit, 0,                    // Bottom-right of Grass

		// Bottom face (Dirt)
		x_coord_unit * 2, 0,                // Bottom-left of Dirt
		x_coord_unit * 2, 1,       // Top-left of Dirt
		x_coord_unit * 3, 1,       // Top-right of Dirt
		x_coord_unit * 3, 0                 // Bottom-right of Dirt

	};

	const std::vector<unsigned int> indices = {
		0,1,3,
		3,1,2,
		4,5,7,
		7,5,6,
		8,9,11,
		11,9,10,
		12,13,15,
		15,13,14,
		16,17,19,
		19,17,18,
		20,21,23,
		23,21,22
	};
}