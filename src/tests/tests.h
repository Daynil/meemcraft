#include <string>
#include <format>
#include <functional>

#include "blocks/shared.h"
#include "util.h"

// uncomment to disable assert()
// #define NDEBUG
#include <cassert>

// TODO: add support for types other than vec3
template<typename T>
inline void Assert(
	T actual,
	T expected,
	bool print_success = false,
	std::string test_name = "Test"
)
{
	auto val_printer = [actual, expected]() {
		print(std::format("Actual  : {0}, {1}, {2}", actual.x, actual.y, actual.z));
		print(std::format("Expected: {0}, {1}, {2}", expected.x, expected.y, expected.z));
	};

	if (actual != expected) {
		print(std::format("{0} failed!", test_name));
		val_printer();
		// Program breaks here
		assert(actual == expected);
	}
	else if (print_success) {
		print(std::format("{0} passed!", test_name));
		val_printer();
	}
}

inline void RunTests()
{
	bool print_success = false;
	// Chunk local to world coords
	//auto actual = ChunkHelpers::ChunkLocalToWorldCoord(glm::vec3(-2, 0, 0), glm::vec3(13, 0, 10));
	//auto expected = glm::vec3(-19, 0, 10);
	//Assert(actual, expected, print_success);	
	// 
	//actual = ChunkHelpers::ChunkLocalToWorldCoord(glm::vec3(1, 0, -1), glm::vec3(4, 0, 2));
	//expected = glm::vec3(20, 0, -14);
	//Assert(actual, expected, print_success);


	// World to chunk local coords
	auto actual = ChunkHelpers::WorldCoordToChunkLocal(glm::vec3(-19, 0, 10));
	auto expected = glm::vec3(13, 0, 10);
	Assert(actual, expected, print_success);

	actual = ChunkHelpers::WorldCoordToChunkLocal(glm::vec3(29, 0, 30));
	expected = glm::vec3(13, 0, 14);
	Assert(actual, expected, print_success);

	actual = ChunkHelpers::WorldCoordToChunkLocal(glm::vec3(20, 0, -14));
	expected = glm::vec3(4, 0, 2);
	Assert(actual, expected, print_success);

	// World to chunk coords
	//auto actual = ChunkHelpers::WorldCoordToChunkCoord(glm::vec3(2, 0, 2));
	//auto expected = glm::vec3(0, 0, 0);
	//Assert(actual, expected, print_success);


	//actual = ChunkHelpers::WorldCoordToChunkCoord(glm::vec3(2, 0, -2));
	//expected = glm::vec3(0, 0, -1);
	//	//Assert(actual, expected, print_success);

	//actual = ChunkHelpers::WorldCoordToChunkCoord(glm::vec3(-2, 0, -2));
	//expected = glm::vec3(-1, 0, -1);
	//Assert(actual, expected, print_success);

	//actual = ChunkHelpers::WorldCoordToChunkCoord(glm::vec3(-2, 0, 2));
	//expected = glm::vec3(-1, 0, 0);
	//Assert(actual, expected, print_success);

	//actual = ChunkHelpers::WorldCoordToChunkCoord(glm::vec3(-6, 0, 10));
	//expected = glm::vec3(-1, 0, 0);
	//Assert(actual, expected, print_success);

	//actual = ChunkHelpers::WorldCoordToChunkCoord(glm::vec3(20, 0, -20));
	//expected = glm::vec3(1, 0, -2);
	//Assert(actual, expected, print_success);

	//actual = ChunkHelpers::WorldCoordToChunkCoord(glm::vec3(40, 0, 40));
	//expected = glm::vec3(2, 0, 2);
	//Assert(actual, expected, print_success);

	//actual = ChunkHelpers::WorldCoordToChunkCoord(glm::vec3(-40, 0, -40));
	//expected = glm::vec3(-3, 0, -3);
	//Assert(actual, expected, print_success);
}