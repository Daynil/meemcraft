#pragma once

#include "block_data.h"
#include "block.h"

class Chunk
{
public:
	static const int CHUNK_SIZE_X = 16;
	static const int CHUNK_SIZE_Y = 256;
	static const int CHUNK_SIZE_Z = 16;

	Chunk() {};

	BlockType blocks[CHUNK_SIZE_X][CHUNK_SIZE_Y][CHUNK_SIZE_Z];

	void GenerateMesh()
	{
		std::vector<float> vertex_positions;
		std::vector<float> vertex_texture_coords;
		std::vector<unsigned int> vertex_indices;

		for (int x = 0; x < CHUNK_SIZE_X; x++) {
			for (int y = 0; y < CHUNK_SIZE_Y; y++) {
				for (int z = 0; z < CHUNK_SIZE_Z; z++) {
					BlockType block = blocks[x][y][z];

					// Skip all rendering of air blocks
					if (block == BlockType::AIR) {
						continue;
					}

					bool render_left, render_right, render_front, render_back, render_top, render_bottom = false;

					if (x > 0) {
						BlockType block_left = blocks[x - 1][y][z];

						if (block_left == BlockType::AIR) {
							render_left = true;
						}
					}
					if (x < CHUNK_SIZE_X - 1) {
						BlockType block_right = blocks[x + 1][y][z];

						if (block_right == BlockType::AIR) {
							render_right = true;
						}
					}

					if (y > 0) {
						BlockType block_bottom = blocks[x][y - 1][z];

						if (block_bottom == BlockType::AIR) {
							render_bottom = true;
						}
					}
					if (y < CHUNK_SIZE_Y - 1) {
						BlockType block_top = blocks[x][y + 1][z];

						if (block_top == BlockType::AIR) {
							render_top = true;
						}
					}

					if (z > 0) {
						BlockType block_back = blocks[x][y][z - 1];

						if (block_back == BlockType::AIR) {
							render_back = true;
						}
					}
					if (z < CHUNK_SIZE_Z - 1) {
						BlockType block_front = blocks[x][y][z + 1];

						if (block_front == BlockType::AIR) {
							render_front = true;
						}
					}


				}
			}
		}
	}