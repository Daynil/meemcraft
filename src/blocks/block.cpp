#include "block.h"

#include "resource_manager.h"

Block::Block(BlockType type, glm::vec3 p_position) : type(type), data(ResourceManager::GetBlockData(type))
{
	position = p_position;
	rotation = glm::vec3(0);
	scale = glm::vec3(1);

	// TODO: update transparency to compile time
	if (type == BlockType::WATER || type == BlockType::SELECTED) {
		is_transparent = true;
	}

	float texture_atlas_x_unit = ResourceManager::texture_atlas_x_unit;

	std::vector<float> vertex_positions;
	std::vector<float> vertex_texture_coords;
	std::vector<unsigned int> vertex_indices;

	for (int i = 0; i < BlockVertices::BlockFace::FACES_COUNT; i++)
	{
		BlockVertices::BlockFace face = (BlockVertices::BlockFace)i;
		std::vector<float> texture_coords;

		if (data.symmetrical || face == BlockVertices::BlockFace::TOP) {
			texture_coords = BlockVertices::texture_coords_face(texture_atlas_x_unit, data.top_texture_num);
		}
		else if (face == BlockVertices::BlockFace::BOTTOM) {
			texture_coords = BlockVertices::texture_coords_face(texture_atlas_x_unit, data.bottom_texture_num);
		}
		else {
			// Not sure if we need to consider this in non-chunk scenarios?
			// Basically if it's not the top block, the side textures of a non-symmetrical block like grass
			// should be just dirt, aka the bottom texture, else you'd have grass on the sides going all the way down.
			//if (top_block) {
			if (true) {
				texture_coords = BlockVertices::texture_coords_face(texture_atlas_x_unit, data.sides_texture_num);
			}
			else {
				texture_coords = BlockVertices::texture_coords_face(texture_atlas_x_unit, data.bottom_texture_num);
			}
		}

		// 1) Determine how many vertices we’ve already placed
		unsigned int baseIndex = vertex_positions.size() / 3; // each vertex has 3 floats

		// 2) Copy the 4 face vertices, adding (x, y, z)
		for (int i = 0; i < 4; i++) {
			float vx = BlockVertices::vertices_face.at(face)[i * 3 + 0];
			float vy = BlockVertices::vertices_face.at(face)[i * 3 + 1];
			float vz = BlockVertices::vertices_face.at(face)[i * 3 + 2];


			vertex_positions.push_back(vx);
			vertex_positions.push_back(vy);
			vertex_positions.push_back(vz);

			//if (!IsTransparentBlock(block)) {
			//	vertex_positions.push_back(vx);
			//	vertex_positions.push_back(vy);
			//	vertex_positions.push_back(vz);
			//}
			//else {
			//	transparent_vertex_positions.push_back(vx);
			//	transparent_vertex_positions.push_back(vy);
			//	transparent_vertex_positions.push_back(vz);
			//}
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

		//if (!IsTransparentBlock(block)) {
		//	vertex_texture_coords.insert(
		//		vertex_texture_coords.end(),
		//		texture_coords.begin(),
		//		texture_coords.end()
		//	);

		//	vertex_indices.push_back(baseIndex + 0);
		//	vertex_indices.push_back(baseIndex + 1);
		//	vertex_indices.push_back(baseIndex + 3);
		//	vertex_indices.push_back(baseIndex + 3);
		//	vertex_indices.push_back(baseIndex + 1);
		//	vertex_indices.push_back(baseIndex + 2);
		//}
		//else {
		//	unsigned int base_index_transparent = transparent_vertex_positions.size() / 3;

		//	transparent_vertex_texture_coords.insert(
		//		transparent_vertex_texture_coords.end(),
		//		texture_coords.begin(),
		//		texture_coords.end()
		//	);

		//	transparent_vertex_indices.push_back(base_index_transparent + 0);
		//	transparent_vertex_indices.push_back(base_index_transparent + 1);
		//	transparent_vertex_indices.push_back(base_index_transparent + 3);
		//	transparent_vertex_indices.push_back(base_index_transparent + 3);
		//	transparent_vertex_indices.push_back(base_index_transparent + 1);
		//	transparent_vertex_indices.push_back(base_index_transparent + 2);
		//}
	}

	model = new RawModel(vertex_positions, vertex_texture_coords, vertex_indices);
	texture = &ResourceManager::GetTexture("block_atlas");
	shader = &ResourceManager::GetShader("entity");
}
