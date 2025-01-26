#pragma once

#include <glm/glm.hpp>

#include "util.h"

namespace ChunkHelpers
{
	static const int CHUNK_SIZE_X = 16;
	static const int CHUNK_SIZE_Y = 256;
	static const int CHUNK_SIZE_Z = 16;

	enum AdjacentChunk {
		LEFT,
		RIGHT,
		FRONT,
		BACK,
		COUNT
	};

	inline glm::vec3 WorldCoordToChunkCoord(glm::vec3 world_coord)
	{
		int cx;
		int cz;

		// Our x origin is the top-left side of a chunk, so as we go rightward
		// in the world, the origin is to our left, which is *less* than the point's coord, 
		// so we floor it...
		// As we go leftward, the origin is still left, but is *more negative*
		// than the point's coord, so we still floor it.
		cx = std::floor(world_coord.x / CHUNK_SIZE_X);

		// Likewise for z
		cz = std::floor(world_coord.z / CHUNK_SIZE_Z);

		return glm::vec3(cx, world_coord.y, cz);
	};

	inline glm::vec3 WorldCoordToChunkLocal(glm::vec3 world_coord)
	{
		auto chunk_coord = WorldCoordToChunkCoord(world_coord);

		// The starting world block x and z of the chunk's local coordinate system
		glm::vec3 chunk_origin_world_coord(
			chunk_coord.x * CHUNK_SIZE_X,
			0,
			chunk_coord.z * CHUNK_SIZE_Z
		);

		// The block's chunk-local coordinate
		glm::vec3 chunk_local_coord(
			world_coord.x - chunk_origin_world_coord.x,
			world_coord.y,
			world_coord.z - chunk_origin_world_coord.z
		);

		return chunk_local_coord;
	};

	inline glm::vec3 ChunkLocalToWorldCoord(glm::vec3 chunk_coord, glm::vec3 block_local_coord)
	{
		// The starting world block x and z of the chunk's local coordinate system
		glm::vec3 chunk_origin_world_coord(
			chunk_coord.x * CHUNK_SIZE_X,
			0,
			chunk_coord.z * CHUNK_SIZE_Z
		);

		// The 0,0 origin of our coordinate system is the *top-left* of the chunk.
		// So moving down and right to the bottom-right of the chunk 
		// *always* means an increase in both x and z.
		glm::vec3 block_world_coord(
			chunk_origin_world_coord.x + block_local_coord.x,
			chunk_coord.y,
			chunk_origin_world_coord.z + block_local_coord.z
		);

		return block_world_coord;
	};
};