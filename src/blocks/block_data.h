#pragma once

#include <array>
#include <vector>

enum BlockType {
	// AKA no block
	AIR,

	// Environmental
	GRASS_BLOCK,
	DIRT,
	SAND,
	STONE,

	// Ore
	COAL_ORE,
	COPPER_ORE,
	IRON_ORE,
	GOLD_ORE,

	// Growth
	OAK_LOG,
	OAK_LEAVES,

	// Represents the amount of block types
	BLOCK_COUNT
};

constexpr std::array<const char*, BlockType::BLOCK_COUNT> BlockTypeString = {
	"AIR",

	// Environmental
	"GRASS_BLOCK",
	"DIRT",
	"SAND",
	"STONE",

	// Ore
	"COAL_ORE",
	"COPPER_ORE",
	"IRON_ORE",
	"GOLD_ORE",

	// Growth
	"OAK_LOG",
	"OAK_LEAVES",
};

// Compile-time data we extract based on block texture files
struct BlockData {
	// Whether all 6 sides use the same texture
	bool symmetrical;
};

namespace BlockVertices
{
	const unsigned int blocks_in_texture = 3;
	const float x_coord_unit = 1.0f / blocks_in_texture;

	// Per-face vertices
	const std::vector<float> vertices_front = {
		-0.5f,   0.5f,  0.5f,                 // Top-left        
		-0.5f,  -0.5f,  0.5f,                 // Bottom-left
		 0.5f,  -0.5f,  0.5f,                 // Bottom-right
		 0.5f,   0.5f,  0.5f,                 // Top-right
	};

	const std::vector<float> vertices_back = {
		  0.5f,  0.5f, -0.5f,
		  0.5f, -0.5f, -0.5f,
		 -0.5f, -0.5f, -0.5f,
		 -0.5f,  0.5f, -0.5f,
	};

	const std::vector<float> vertices_right = {
		 0.5f,   0.5f,  0.5f,
		 0.5f,  -0.5f,  0.5f,
		 0.5f,  -0.5f, -0.5f,
		 0.5f,   0.5f, -0.5f,
	};

	const std::vector<float> vertices_left = {
		 -0.5f,  0.5f, -0.5f,
		 -0.5f, -0.5f, -0.5f,
		 -0.5f, -0.5f,  0.5f,
		 -0.5f,  0.5f,  0.5f,
	};

	const std::vector<float> vertices_top = {
		 -0.5f,  0.5f, -0.5f,
		 -0.5f,  0.5f,  0.5f,
		  0.5f,  0.5f,  0.5f,
		  0.5f,  0.5f, -0.5f,
	};

	const std::vector<float> vertices_bottom = {
		 -0.5f, -0.5f, -0.5f,
		  0.5f, -0.5f, -0.5f,
		  0.5f, -0.5f,  0.5f,
		 -0.5f, -0.5f,  0.5f
	};

	// Side faces (front/back/left/right) are all the same texture
	// on the multi-texture blocks.
	// E.g. on a grass block, grass+dirt
	const std::vector<float> texture_coords_3_part_sides = {
		x_coord_unit, 0,                  // Bottom-left of Grass+Dirt
		x_coord_unit, 1,                  // Top-left of Grass+Dirt
		x_coord_unit * 2, 1,              // Top-right of Grass+Dirt
		x_coord_unit * 2, 0,              // Bottom-right of Grass+Dirt
	};

	// Grass only on grass block
	const std::vector<float> texture_coords_3_part_top = {
		0, 0,                             // Bottom-left of Grass
		0, 1,                             // Top-left of Grass
		x_coord_unit, 1,                  // Top-right of Grass
		x_coord_unit, 0,                  // Bottom-right of Grass
	};

	// Dirt only on grass block
	const std::vector<float> texture_coords_3_part_bottom = {
		// Bottom face (Dirt)
		x_coord_unit * 2, 0,              // Bottom-left of Dirt
		x_coord_unit * 2, 1,              // Top-left of Dirt
		x_coord_unit * 3, 1,              // Top-right of Dirt
		x_coord_unit * 3, 0               // Bottom-right of Dirt
	};

	// Same for all faces
	const std::vector<float> texture_coords_symmetrical_face = {
		0, 0,                            // Bottom-left
		0, 1,                            // Top-left
		1, 1,                            // Top-right
		1, 0,                            // Bottom-right
	};

	const std::vector<unsigned int> indices_face = {
		0,1,3,
		3,1,2,
	};



	// Per-cube vertices
	const std::vector<float> vertices = {
		// Front face
		-0.5f,   0.5f,  0.5f,                 // Top-left        
		-0.5f,  -0.5f,  0.5f,                 // Bottom-left
		 0.5f,  -0.5f,  0.5f,                 // Bottom-right
		 0.5f,   0.5f,  0.5f,                 // Top-right

		 // Back face
		  0.5f,  0.5f, -0.5f,
		  0.5f, -0.5f, -0.5f,
		 -0.5f, -0.5f, -0.5f,
		 -0.5f,  0.5f, -0.5f,

		 // Right face
		 0.5f,   0.5f,  0.5f,
		 0.5f,  -0.5f,  0.5f,
		 0.5f,  -0.5f, -0.5f,
		 0.5f,   0.5f, -0.5f,

		 // Left face
		 -0.5f,  0.5f, -0.5f,
		 -0.5f, -0.5f, -0.5f,
		 -0.5f, -0.5f,  0.5f,
		 -0.5f,  0.5f,  0.5f,

		 // Top face
		 -0.5f,  0.5f, -0.5f,
		 -0.5f,  0.5f,  0.5f,
		  0.5f,  0.5f,  0.5f,
		  0.5f,  0.5f, -0.5f,

		  // Bottom face
		 -0.5f, -0.5f, -0.5f,
		  0.5f, -0.5f, -0.5f,
		  0.5f, -0.5f,  0.5f,
		 -0.5f, -0.5f,  0.5f
	};

	// Note top and bottom inverted due to texture coord system
	// (0 is bottom left)
	const std::vector<float> texture_coords_3_part = {
		// Front face (Grass + Dirt)
		x_coord_unit, 0,                  // Bottom-left of Grass+Dirt
		x_coord_unit, 1,                  // Top-left of Grass+Dirt
		x_coord_unit * 2, 1,              // Top-right of Grass+Dirt
		x_coord_unit * 2, 0,              // Bottom-right of Grass+Dirt

		// Back face (Grass + Dirt)
		x_coord_unit, 0,                  // Bottom-left of Grass+Dirt
		x_coord_unit, 1,                  // Top-left of Grass+Dirt
		x_coord_unit * 2, 1,              // Top-right of Grass+Dirt
		x_coord_unit * 2, 0,              // Bottom-right of Grass+Dirt

		// Right face (Grass + Dirt)
		x_coord_unit, 0,                  // Bottom-left of Grass+Dirt
		x_coord_unit, 1,                  // Top-left of Grass+Dirt
		x_coord_unit * 2, 1,              // Top-right of Grass+Dirt
		x_coord_unit * 2, 0,              // Bottom-right of Grass+Dirt

		// Left face (Grass + Dirt)
		x_coord_unit, 0,                  // Bottom-left of Grass+Dirt
		x_coord_unit, 1,                  // Top-left of Grass+Dirt
		x_coord_unit * 2, 1,              // Top-right of Grass+Dirt
		x_coord_unit * 2, 0,              // Bottom-right of Grass+Dirt

		// Top face (Grass)
		0, 0,                             // Bottom-left of Grass
		0, 1,                             // Top-left of Grass
		x_coord_unit, 1,                  // Top-right of Grass
		x_coord_unit, 0,                  // Bottom-right of Grass

		// Bottom face (Dirt)
		x_coord_unit * 2, 0,              // Bottom-left of Dirt
		x_coord_unit * 2, 1,              // Top-left of Dirt
		x_coord_unit * 3, 1,              // Top-right of Dirt
		x_coord_unit * 3, 0               // Bottom-right of Dirt

	};

	const std::vector<float> texture_coords_symmetrical = {
		0, 0,                            // Bottom-left
		0, 1,                            // Top-left
		1, 1,                            // Top-right
		1, 0,                            // Bottom-right

		0, 0,                            // Bottom-left
		0, 1,                            // Top-left
		1, 1,                            // Top-right
		1, 0,                            // Bottom-right

		0, 0,                            // Bottom-left
		0, 1,                            // Top-left
		1, 1,                            // Top-right
		1, 0,                            // Bottom-right

		0, 0,                            // Bottom-left
		0, 1,                            // Top-left
		1, 1,                            // Top-right
		1, 0,                            // Bottom-right

		0, 0,                            // Bottom-left
		0, 1,                            // Top-left
		1, 1,                            // Top-right
		1, 0,                            // Bottom-right

		0, 0,                            // Bottom-left
		0, 1,                            // Top-left
		1, 1,                            // Top-right
		1, 0,                            // Bottom-right
	};

	// The indices indicate to OpenGL which set of 
	// vertices/texture coords to use to use per triangle.
	// Note that order is important - each triangle should be drawn
	// clockwise. This allows for proper back-face culling (not drawing
	// stuff that is not visible).
	const std::vector<unsigned int> indices = {
		// Front face
		0,1,3,
		3,1,2,

		// Back face
		4,5,7,
		7,5,6,

		// Right face
		8,9,11,
		11,9,10,

		// Left face
		12,13,15,
		15,13,14,

		// Top face
		16,17,19,
		19,17,18,

		// Bottom face
		20,21,23,
		23,21,22
	};
}

class BlockLoader
{
public:
	BlockLoader() {};

	void LoadBlocks();
};