#include "chunk.h"

#include <iostream>

#include "util.h"
#include "map_generation/map_generator.h"

using namespace ChunkHelpers;

Chunk::Chunk(ChunkID p_id, glm::vec3 p_position, MapGenerator* map_generator) : map_generator(map_generator)
{
	id = p_id;
	position = p_position;

	opaque_entity = Entity();
	opaque_entity.position = p_position;
	opaque_entity.model = nullptr;
	opaque_entity.rotation = glm::vec3(0);
	opaque_entity.scale = glm::vec3(1);
	opaque_entity.texture = &ResourceManager::GetTexture("block_atlas");
	opaque_entity.shader = &ResourceManager::GetShader("entity");

	transparent_entity = Entity();
	transparent_entity.position = p_position;
	transparent_entity.model = nullptr;
	transparent_entity.is_transparent = true;
	transparent_entity.rotation = glm::vec3(0);
	transparent_entity.scale = glm::vec3(1);
	transparent_entity.texture = &ResourceManager::GetTexture("block_atlas");
	transparent_entity.shader = &ResourceManager::GetShader("entity");

	GenerateBlocks();
}

Chunk::~Chunk() {
	delete opaque_entity.model;
	delete transparent_entity.model;
	adjacent_chunks.clear();
};

double Chunk::GetRawHeight(double cont)
{
	float slope = 0;
	float y_intercept = 0;

	float spline_1 = 0.3;
	float spline_2 = 0.6;

	if (cont < spline_1) {
		slope = (100 - 50) / (spline_1 - -1);
		y_intercept = 50 - (slope * -1);
	}
	else if (cont >= spline_1 && cont < spline_2) {
		slope = (150 - 100) / (spline_2 - spline_1);
		y_intercept = 100 - (slope * spline_1);
	}
	else {
		slope = 0;
		y_intercept = 150;
	}

	return slope * cont + y_intercept;
}

BlockType Chunk::GetBlockType(int x, int y, int z)
{
	float sea_level_start = 0.5f;
	float sea_level_end = 0.55f;
	float mountains = 0.9f;

	int sea_height = CHUNK_SIZE_Y * 0.3;
	int mountain_height = CHUNK_SIZE_Y * 0.5;

	int base_soil_depth = 3;

	BlockType block = BlockType::AIR;

	int terrain_height = 0;


	int wx = id.x * CHUNK_SIZE_X;
	int wz = id.y * CHUNK_SIZE_Z;
	NoiseData noise_data = map_generator->SampleNoise(wx + x, wz + z);

	double cont = noise_data.continentalness;
	double patch = noise_data.patches;

	double h = GetRawHeight(cont);
	double hL = GetRawHeight(map_generator->SampleNoise(wx + x - 1, wz + z).continentalness);
	double hR = GetRawHeight(map_generator->SampleNoise(wx + x + 1, wz + z).continentalness);
	double hF = GetRawHeight(map_generator->SampleNoise(wx + x, wz + z + 1).continentalness);
	double hB = GetRawHeight(map_generator->SampleNoise(wx + x - 1, wz + z - 1).continentalness);

	double slope = std::max(std::abs(hL - hR), std::abs(hF - hB));

	terrain_height = std::floor(h);

	if (y < terrain_height) {
		if (y < sea_height)
			block = BlockType::DIRT;
		else
			block = BlockType::GRASS_BLOCK;

		int soil_depth = std::floor(patch * 3) + base_soil_depth;
		if (slope > 3.5) {
			soil_depth = std::max(0, soil_depth - 2);
		}
		//print(std::format("{0}", soil_depth));

		if (y > sea_height - 3 && y < sea_height + 3) {
			if (y > terrain_height - soil_depth)
				block = BlockType::SAND;
		}
		else {
			if (y < terrain_height - soil_depth)
				block = BlockType::STONE;
		}
	}
	else if (y == sea_height) {
		block = BlockType::WATER;
		if (!has_transparent_blocks)
			has_transparent_blocks = true;
	}

	return block;
}

bool Chunk::ShouldRenderFace(BlockType block, BlockType adjacent_block)
{
	// If the block is transparent, only render faces if adjacent to air.
	// This makes transparent blocks appear continuous.
	if (ChunkHelpers::IsTransparent(block)) {
		return adjacent_block == BlockType::AIR;
	}
	// Solid blocks should render if they are near a transparent block to be visible.
	return adjacent_block == BlockType::AIR
		|| ChunkHelpers::IsTransparent(adjacent_block);
}

void Chunk::GenerateBlocks()
{
	for (int x = 0; x < CHUNK_SIZE_X; x++) {
		for (int y = 0; y < CHUNK_SIZE_Y; y++) {
			for (int z = 0; z < CHUNK_SIZE_Z; z++) {
				BlockInfo info;
				info.type = GetBlockType(x, y, z);
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

	std::vector<float> transparent_vertex_positions;
	std::vector<float> transparent_vertex_texture_coords;
	std::vector<unsigned int> transparent_vertex_indices;

	vertex_positions.reserve(CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z * 6 * 4 * 3);
	vertex_texture_coords.reserve(CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z * 6 * 4 * 2);
	vertex_indices.reserve(CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z * 6 * 6);

	if (has_transparent_blocks) {
		transparent_vertex_positions.reserve(CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z * 6 * 4 * 3);
		transparent_vertex_texture_coords.reserve(CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z * 6 * 4 * 2);
		transparent_vertex_indices.reserve(CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z * 6 * 6);
	}

	float texture_atlas_x_unit = ResourceManager::texture_atlas_x_unit;

	int total_faces_rendered = 0;

	Chunk* left_chunk = adjacent_chunks.at(ChunkHelpers::AdjacentChunk::LEFT);
	Chunk* right_chunk = adjacent_chunks.at(ChunkHelpers::AdjacentChunk::RIGHT);
	Chunk* front_chunk = adjacent_chunks.at(ChunkHelpers::AdjacentChunk::FRONT);
	Chunk* back_chunk = adjacent_chunks.at(ChunkHelpers::AdjacentChunk::BACK);

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
					top_block = ShouldRenderFace(block, block_top);
				}

				for (int i = 0; i < BlockVertices::BlockFace::FACES_COUNT; i++)
				{
					BlockVertices::BlockFace face = (BlockVertices::BlockFace)i;
					bool render_face = false;

					if (face == BlockVertices::BlockFace::LEFT) {
						if (x > 0) {
							BlockType block_left = blocks[x - 1][y][z].type;
							render_face = ShouldRenderFace(block, block_left);
						}
						// Note: edge of the map will never be visible
						// so we never render map-edge faces
						else {
							if (left_chunk) {
								auto left_chunk_adjacent_block = left_chunk->blocks[CHUNK_SIZE_X - 1][y][z].type;
								render_face = ShouldRenderFace(block, left_chunk_adjacent_block);
							}
						}
					}
					else if (face == BlockVertices::BlockFace::RIGHT) {
						if (x < CHUNK_SIZE_X - 1) {
							BlockType block_right = blocks[x + 1][y][z].type;
							render_face = ShouldRenderFace(block, block_right);
						}
						else {
							if (right_chunk) {
								auto right_chunk_adjacent_block = right_chunk->blocks[0][y][z].type;
								render_face = ShouldRenderFace(block, right_chunk_adjacent_block);
							}
						}
					}
					else if (face == BlockVertices::BlockFace::FRONT) {
						// Logic for the front faces of blocks within a chunk
						if (z < CHUNK_SIZE_Z - 1) {
							BlockType block_front = blocks[x][y][z + 1].type;
							render_face = ShouldRenderFace(block, block_front);
						}
						// Logic for front faces of blocks at front-most edge of chunk
						else {
							if (front_chunk) {
								auto front_chunk_adjacent_block = front_chunk->blocks[x][y][0].type;
								render_face = ShouldRenderFace(block, front_chunk_adjacent_block);
							}
						}
					}
					else if (face == BlockVertices::BlockFace::BACK) {
						if (z > 0) {
							BlockType block_back = blocks[x][y][z - 1].type;
							render_face = ShouldRenderFace(block, block_back);
						}
						else {
							if (back_chunk) {
								auto back_chunk_adjacent_block = back_chunk->blocks[x][y][CHUNK_SIZE_Z - 1].type;
								render_face = ShouldRenderFace(block, back_chunk_adjacent_block);
							}
						}
					}
					else if (face == BlockVertices::BlockFace::BOTTOM) {
						// Note: Never render bottom face of bottom of chunk, it's never visible
						if (y > 0) {
							BlockType block_bottom = blocks[x][y - 1][z].type;
							render_face = ShouldRenderFace(block, block_bottom);
						}
					}
					//else if (face == BlockVertices::BlockFace::TOP) {
					else {
						if (y < CHUNK_SIZE_Y - 1) {
							BlockType block_top = blocks[x][y + 1][z].type;
							render_face = ShouldRenderFace(block, block_top);
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
							// Basically if it's not the top block, the side textures of a non-symmetrical block like grass
							// should be just dirt, aka the bottom texture, else you'd have grass on the sides going all the way down.
							if (top_block) {
								texture_coords = BlockVertices::texture_coords_face(texture_atlas_x_unit, block_data.sides_texture_num);
							}
							else {
								texture_coords = BlockVertices::texture_coords_face(texture_atlas_x_unit, block_data.bottom_texture_num);
							}
						}

						// 1) Determine how many vertices we’ve already placed
						unsigned int baseIndex = vertex_positions.size() / 3; // each vertex has 3 floats
						unsigned int base_index_transparent = transparent_vertex_positions.size() / 3;

						// 2) Copy the 4 face vertices, adding (x, y, z)
						for (int i = 0; i < 4; i++) {
							float vx = BlockVertices::vertices_face.at(face)[i * 3 + 0] + x;
							float vy = BlockVertices::vertices_face.at(face)[i * 3 + 1] + y;
							float vz = BlockVertices::vertices_face.at(face)[i * 3 + 2] + z;


							if (!ChunkHelpers::IsTransparent(block)) {
								vertex_positions.push_back(vx);
								vertex_positions.push_back(vy);
								vertex_positions.push_back(vz);
							}
							else {
								transparent_vertex_positions.push_back(vx);
								transparent_vertex_positions.push_back(vy);
								transparent_vertex_positions.push_back(vz);
							}
						}

						if (!ChunkHelpers::IsTransparent(block)) {
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
						else {
							transparent_vertex_texture_coords.insert(
								transparent_vertex_texture_coords.end(),
								texture_coords.begin(),
								texture_coords.end()
							);

							transparent_vertex_indices.push_back(base_index_transparent + 0);
							transparent_vertex_indices.push_back(base_index_transparent + 1);
							transparent_vertex_indices.push_back(base_index_transparent + 3);
							transparent_vertex_indices.push_back(base_index_transparent + 3);
							transparent_vertex_indices.push_back(base_index_transparent + 1);
							transparent_vertex_indices.push_back(base_index_transparent + 2);
						}
					}
				}
			}
		}
	}
	//timer.Stop();

	opaque_entity.model = new RawModel(vertex_positions, vertex_texture_coords, vertex_indices, true);
	if (has_transparent_blocks) {
		transparent_entity.model = new RawModel(transparent_vertex_positions, transparent_vertex_texture_coords, transparent_vertex_indices, true);
	}
}

void Chunk::LoadToGPU()
{
	opaque_entity.model->LoadToGPU();
	if (has_transparent_blocks)
		transparent_entity.model->LoadToGPU();
}

