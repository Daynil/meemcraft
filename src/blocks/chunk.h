#pragma once

#include <glm/glm.hpp>

#include "block_data.h"
#include "block.h"
#include "resource_manager.h"
#include "shared.h"

class Chunk : public Entity
{
public:
	static const int CHUNK_SIZE_X = 16;
	static const int CHUNK_SIZE_Y = 256;
	static const int CHUNK_SIZE_Z = 16;

	BlockInfo blocks[CHUNK_SIZE_X][CHUNK_SIZE_Y][CHUNK_SIZE_Z];

	std::map<ChunkDirection::AdjacentChunk, Chunk*> adjacent_chunks;

	Chunk(glm::vec3 p_position, std::vector<double>* chunk_map_data);

	//~Chunk() {
	//	delete model;
	//};

	void ChunkTest();

	BlockType GetBlockType(double noise_value, int y);

	void GenerateBlocks(std::vector<double>* chunk_map);
	void GenerateMesh();


	//void GenerateMeshOld(std::vector<double> chunk_map)
	//{
	//	std::vector<float> vertex_positions;
	//	std::vector<float> vertex_texture_coords;
	//	std::vector<unsigned int> vertex_indices;

	//	vertex_positions.reserve(CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z * 6 * 4 * 3);
	//	vertex_texture_coords.reserve(CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z * 6 * 4 * 2);
	//	vertex_indices.reserve(CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z * 6 * 6);

	//	float texture_atlas_x_unit = ResourceManager::texture_atlas_x_unit;

	//	int total_faces_rendered = 0;

	//	for (int x = 0; x < CHUNK_SIZE_X; x++) {
	//		for (int y = 0; y < CHUNK_SIZE_Y; y++) {
	//			for (int z = 0; z < CHUNK_SIZE_Z; z++) {
	//				const double noise_value = chunk_map[z * CHUNK_SIZE_X + x];
	//				blocks[x][y][z] = GetBlockType(noise_value, y);
	//			}
	//		}
	//	}

	//	for (int x = 0; x < CHUNK_SIZE_X; x++) {
	//		for (int y = 0; y < CHUNK_SIZE_Y; y++) {
	//			for (int z = 0; z < CHUNK_SIZE_Z; z++) {
	//				BlockType block = blocks[x][y][z];
	//				BlockData block_data = ResourceManager::GetBlockData(block);

	//				// Skip all rendering of air blocks
	//				if (block == BlockType::AIR) {
	//					continue;
	//				}

	//				bool top_block = y == CHUNK_SIZE_Y - 1;
	//				if (y < CHUNK_SIZE_Y - 1) {
	//					BlockType block_top = blocks[x][y + 1][z];

	//					if (block_top == BlockType::AIR) {
	//						top_block = true;
	//					}
	//				}

	//				for (int i = 0; i < BlockVertices::BlockFace::FACES_COUNT; i++)
	//				{
	//					BlockVertices::BlockFace face = (BlockVertices::BlockFace)i;
	//					bool render_face = false;

	//					if (face == BlockVertices::BlockFace::LEFT) {
	//						if (x > 0) {
	//							BlockType block_left = blocks[x - 1][y][z];

	//							if (block_left == BlockType::AIR) {
	//								render_face = true;
	//							}
	//						}
	//						else {
	//							render_face = true;
	//						}
	//					}

	//					if (face == BlockVertices::BlockFace::RIGHT) {
	//						if (x < CHUNK_SIZE_X - 1) {
	//							BlockType block_right = blocks[x + 1][y][z];

	//							if (block_right == BlockType::AIR) {
	//								render_face = true;
	//							}
	//						}
	//						else {
	//							render_face = true;
	//						}
	//					}

	//					if (face == BlockVertices::BlockFace::BOTTOM) {
	//						if (y > 0) {
	//							BlockType block_bottom = blocks[x][y - 1][z];

	//							if (block_bottom == BlockType::AIR) {
	//								render_face = true;
	//							}
	//						}
	//						else {
	//							render_face = true;
	//						}
	//					}

	//					if (face == BlockVertices::BlockFace::TOP) {
	//						if (y < CHUNK_SIZE_Y - 1) {
	//							BlockType block_top = blocks[x][y + 1][z];

	//							if (block_top == BlockType::AIR) {
	//								render_face = true;
	//							}
	//						}
	//						else {
	//							render_face = true;
	//						}
	//					}


	//					if (face == BlockVertices::BlockFace::BACK) {
	//						if (z > 0) {
	//							BlockType block_back = blocks[x][y][z - 1];

	//							if (block_back == BlockType::AIR) {
	//								render_face = true;
	//							}
	//						}
	//						else {
	//							render_face = true;
	//						}
	//					}


	//					if (face == BlockVertices::BlockFace::FRONT) {
	//						if (z < CHUNK_SIZE_Z - 1) {
	//							BlockType block_front = blocks[x][y][z + 1];

	//							if (block_front == BlockType::AIR) {
	//								render_face = true;
	//							}
	//						}
	//						else {
	//							render_face = true;
	//						}
	//					}

	//					if (render_face) {
	//						std::vector<float> texture_coords;
	//						if (block_data.symmetrical || face == BlockVertices::BlockFace::TOP) {
	//							texture_coords = BlockVertices::texture_coords_face(texture_atlas_x_unit, block_data.top_texture_num);
	//						}
	//						else if (face == BlockVertices::BlockFace::BOTTOM) {
	//							texture_coords = BlockVertices::texture_coords_face(texture_atlas_x_unit, block_data.bottom_texture_num);
	//						}
	//						else {
	//							if (top_block) {
	//								texture_coords = BlockVertices::texture_coords_face(texture_atlas_x_unit, block_data.sides_texture_num);
	//							}
	//							else {
	//								texture_coords = BlockVertices::texture_coords_face(texture_atlas_x_unit, block_data.bottom_texture_num);
	//							}
	//						}

	//						// 1) Determine how many vertices we’ve already placed
	//						unsigned int baseIndex = vertex_positions.size() / 3; // each vertex has 3 floats

	//						// 2) Copy the 4 face vertices, adding (x, y, z)
	//						for (int i = 0; i < 4; i++) {
	//							float vx = BlockVertices::vertices_face.at(face)[i * 3 + 0] + x;
	//							float vy = BlockVertices::vertices_face.at(face)[i * 3 + 1] + y;
	//							float vz = BlockVertices::vertices_face.at(face)[i * 3 + 2] + z;

	//							vertex_positions.push_back(vx);
	//							vertex_positions.push_back(vy);
	//							vertex_positions.push_back(vz);
	//						}

	//						vertex_texture_coords.insert(
	//							vertex_texture_coords.end(),
	//							texture_coords.begin(),
	//							texture_coords.end()
	//						);

	//						vertex_indices.push_back(baseIndex + 0);
	//						vertex_indices.push_back(baseIndex + 1);
	//						vertex_indices.push_back(baseIndex + 3);
	//						vertex_indices.push_back(baseIndex + 3);
	//						vertex_indices.push_back(baseIndex + 1);
	//						vertex_indices.push_back(baseIndex + 2);
	//					}
	//				}
	//			}
	//		}
	//	}

	//	model = new RawModel(vertex_positions, vertex_texture_coords, vertex_indices);
	//	rotation = glm::vec3(0);
	//	scale = glm::vec3(1);
	//	texture = &ResourceManager::GetTexture("block_atlas");
	//	shader = &ResourceManager::GetShader("entity");
	//};
};