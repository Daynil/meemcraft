#include "chunk.h"

#include <iostream>

#include "util.h"

//Chunk::Chunk(ChunkID p_id, glm::vec3 p_position, std::vector<std::vector<double>>* chunk_map_data)
//{
//	id = p_id;
//	position = p_position;
//	GenerateBlocks(chunk_map_data);
//}

void Chunk::ChunkTest()
{
	std::vector<float> vertex_positions;
	std::vector<float> vertex_texture_coords;
	std::vector<unsigned int> vertex_indices;

	float texture_atlas_x_unit = ResourceManager::texture_atlas_x_unit;

	BlockType block = BlockType::GRASS_BLOCK;
	BlockData block_data = ResourceManager::GetBlockData(block);

	std::vector<float> texture_coords = BlockVertices::texture_coords_face(texture_atlas_x_unit, block_data.sides_texture_num);


	for (int block_num = 0; block_num < 4; block_num++)
	{
		// 1) Determine how many vertices we’ve already placed
		unsigned int baseIndex = vertex_positions.size() / 3; // each vertex has 3 floats

		// 2) Copy the 4 'left-face' vertices, adding (x, y, z)
		for (int i = 0; i < 4; i++) {
			float vx = BlockVertices::vertices_left[i * 3 + 0] + 0;
			float vy = BlockVertices::vertices_left[i * 3 + 1] + 0;
			float vz = BlockVertices::vertices_left[i * 3 + 2] + block_num;

			vertex_positions.push_back(vx);
			vertex_positions.push_back(vy);
			vertex_positions.push_back(vz);
		}

		vertex_texture_coords.insert(
			vertex_texture_coords.end(),
			texture_coords.begin(),
			texture_coords.end()
		);

		vertex_indices.push_back(baseIndex + 0);
		vertex_indices.push_back(baseIndex + 1);
		vertex_indices.push_back(baseIndex + 3);
		vertex_indices.push_back(baseIndex + 3);
		vertex_indices.push_back(baseIndex + 1);
		vertex_indices.push_back(baseIndex + 2);
	}

	model = new RawModel(vertex_positions, vertex_texture_coords, vertex_indices);
	rotation = glm::vec3(0);
	scale = glm::vec3(1);
	texture = &ResourceManager::GetTexture("block_atlas");
	shader = &ResourceManager::GetShader("entity");
}

BlockType Chunk::GetBlockType(double noise_value, int y)
{
	float sea_level_start = 0.5f;
	float sea_level_end = 0.55f;
	int sea_height = CHUNK_SIZE_Y / 2;
	float mountains = 0.7f;

	BlockType block = BlockType::AIR;

	int area_height = sea_height + (sea_height * (noise_value - sea_level_start));

	if (y <= area_height) {
		block = BlockType::GRASS_BLOCK;
	}

	// Above ground
	//if (noise_value >= sea_level_start) {
	//	int area_height = sea_height + (sea_height * (noise_value - sea_level_start));

	//	if (noise_value < mountains) {
	//		if (noise_value <= sea_level_end) {
	//			if (y <= area_height) {
	//				block = BlockType::SAND;
	//			}
	//		}
	//		else {
	//			if (y <= area_height) {
	//				block = BlockType::GRASS_BLOCK;
	//			}
	//		}
	//	}
	//	else {
	//		if (y <= area_height) {
	//			block = BlockType::STONE;
	//		}
	//	}
	//}
	//// Underground
	//else {
	//	if (y < sea_height) {
	//		block = BlockType::STONE;
	//	}
	//}

	return block;
}

void Chunk::GenerateBlocks(std::vector<std::vector<double>>* chunk_map)
{
	for (int x = 0; x < CHUNK_SIZE_X; x++) {
		for (int y = 0; y < CHUNK_SIZE_Y; y++) {
			for (int z = 0; z < CHUNK_SIZE_Z; z++) {
				const double noise_value = (*chunk_map)[x][z];
				BlockInfo info;
				info.type = GetBlockType(noise_value, y);
				info.health = 10;
				blocks[x][y][z] = info;
			}
		}
	}
}

void Chunk::GenerateMesh()
{
	std::vector<float> vertex_positions;
	std::vector<float> vertex_texture_coords;
	std::vector<unsigned int> vertex_indices;

	vertex_positions.reserve(CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z * 6 * 4 * 3);
	vertex_texture_coords.reserve(CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z * 6 * 4 * 2);
	vertex_indices.reserve(CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z * 6 * 6);

	float texture_atlas_x_unit = ResourceManager::texture_atlas_x_unit;

	int total_faces_rendered = 0;

	Chunk* left_chunk = adjacent_chunks.at(ChunkDirection::AdjacentChunk::LEFT);
	Chunk* right_chunk = adjacent_chunks.at(ChunkDirection::AdjacentChunk::RIGHT);
	Chunk* front_chunk = adjacent_chunks.at(ChunkDirection::AdjacentChunk::FRONT);
	Chunk* back_chunk = adjacent_chunks.at(ChunkDirection::AdjacentChunk::BACK);

	//Timer timer("Chunk mesh");

	for (int x = 0; x < CHUNK_SIZE_X; x++) {
		for (int y = 0; y < CHUNK_SIZE_Y; y++) {
			for (int z = 0; z < CHUNK_SIZE_Z; z++) {
				BlockInfo block_info = blocks[x][y][z];
				BlockType block = block_info.type;
				BlockData& block_data = ResourceManager::GetBlockData(block);

				// Skip all rendering of air blocks
				if (block == BlockType::AIR || block_info.health <= 0) {
					continue;
				}

				bool top_block = y == CHUNK_SIZE_Y - 1;
				if (y < CHUNK_SIZE_Y - 1) {
					BlockType block_top = blocks[x][y + 1][z].type;

					if (block_top == BlockType::AIR) {
						top_block = true;
					}
				}

				for (int i = 0; i < BlockVertices::BlockFace::FACES_COUNT; i++)
				{
					BlockVertices::BlockFace face = (BlockVertices::BlockFace)i;
					bool render_face = false;

					if (face == BlockVertices::BlockFace::LEFT) {
						if (x > 0) {
							BlockType block_left = blocks[x - 1][y][z].type;

							if (block_left == BlockType::AIR) {
								render_face = true;
							}
						}
						// Note: edge of the map will never be visible
						// so we never render map-edge faces
						else {
							if (left_chunk) {
								auto left_chunk_adjacent_block = left_chunk->blocks[CHUNK_SIZE_X - 1][y][z].type;

								if (left_chunk_adjacent_block == BlockType::AIR) {
									render_face = true;
								}
							}
						}
					}

					if (face == BlockVertices::BlockFace::RIGHT) {
						if (x < CHUNK_SIZE_X - 1) {
							BlockType block_right = blocks[x + 1][y][z].type;

							if (block_right == BlockType::AIR) {
								render_face = true;
							}
						}
						else {
							if (right_chunk) {
								auto right_chunk_adjacent_block = right_chunk->blocks[0][y][z].type;

								if (right_chunk_adjacent_block == BlockType::AIR) {
									render_face = true;
								}
							}
						}
					}

					if (face == BlockVertices::BlockFace::FRONT) {
						// Logic for the front faces of blocks within a chunk
						if (z < CHUNK_SIZE_Z - 1) {
							BlockType block_front = blocks[x][y][z + 1].type;

							if (block_front == BlockType::AIR) {
								render_face = true;
							}
						}
						// Logic for front faces of blocks at front-most edge of chunk
						else {
							if (front_chunk) {
								auto front_chunk_adjacent_block = front_chunk->blocks[x][y][0].type;

								if (front_chunk_adjacent_block == BlockType::AIR) {
									render_face = true;
								}
							}
						}
					}

					if (face == BlockVertices::BlockFace::BACK) {
						if (z > 0) {
							BlockType block_back = blocks[x][y][z - 1].type;

							if (block_back == BlockType::AIR) {
								render_face = true;
							}
						}
						else {
							if (back_chunk) {
								auto back_chunk_adjacent_block = back_chunk->blocks[x][y][CHUNK_SIZE_Z - 1].type;

								if (back_chunk_adjacent_block == BlockType::AIR) {
									render_face = true;
								}
							}
						}
					}


					if (face == BlockVertices::BlockFace::BOTTOM) {
						// Note: Never render bottom face of bottom of chunk, it's never visible
						if (y > 0) {
							BlockType block_bottom = blocks[x][y - 1][z].type;

							if (block_bottom == BlockType::AIR) {
								render_face = true;
							}
						}
					}

					if (face == BlockVertices::BlockFace::TOP) {
						if (y < CHUNK_SIZE_Y - 1) {
							BlockType block_top = blocks[x][y + 1][z].type;

							if (block_top == BlockType::AIR) {
								render_face = true;
							}
						}
						else {
							render_face = true;
						}
					}

					if (render_face) {
						std::vector<float> texture_coords;
						if (block_data.symmetrical || face == BlockVertices::BlockFace::TOP) {
							texture_coords = BlockVertices::texture_coords_face(texture_atlas_x_unit, block_data.top_texture_num);
						}
						else if (face == BlockVertices::BlockFace::BOTTOM) {
							texture_coords = BlockVertices::texture_coords_face(texture_atlas_x_unit, block_data.bottom_texture_num);
						}
						else {
							if (top_block) {
								texture_coords = BlockVertices::texture_coords_face(texture_atlas_x_unit, block_data.sides_texture_num);
							}
							else {
								texture_coords = BlockVertices::texture_coords_face(texture_atlas_x_unit, block_data.bottom_texture_num);
							}
						}

						// 1) Determine how many vertices we’ve already placed
						unsigned int baseIndex = vertex_positions.size() / 3; // each vertex has 3 floats

						// 2) Copy the 4 face vertices, adding (x, y, z)
						for (int i = 0; i < 4; i++) {
							float vx = BlockVertices::vertices_face.at(face)[i * 3 + 0] + x;
							float vy = BlockVertices::vertices_face.at(face)[i * 3 + 1] + y;
							float vz = BlockVertices::vertices_face.at(face)[i * 3 + 2] + z;

							vertex_positions.push_back(vx);
							vertex_positions.push_back(vy);
							vertex_positions.push_back(vz);
						}

						vertex_texture_coords.insert(
							vertex_texture_coords.end(),
							texture_coords.begin(),
							texture_coords.end()
						);

						vertex_indices.push_back(baseIndex + 0);
						vertex_indices.push_back(baseIndex + 1);
						vertex_indices.push_back(baseIndex + 3);
						vertex_indices.push_back(baseIndex + 3);
						vertex_indices.push_back(baseIndex + 1);
						vertex_indices.push_back(baseIndex + 2);
					}
				}
			}
		}
	}
	//timer.Stop();

	model = new RawModel(vertex_positions, vertex_texture_coords, vertex_indices, true);
	rotation = glm::vec3(0);
	scale = glm::vec3(1);
	texture = &ResourceManager::GetTexture("block_atlas");
	shader = &ResourceManager::GetShader("entity");
};