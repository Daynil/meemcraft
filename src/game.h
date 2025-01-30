#pragma once

#include <map>
#include <string>
#include <vector>

#include "thread_pool.hpp"

#include "rendering/texture.h"
#include "entity.h"
#include "rendering/shader.h"
#include "raw_model.h"
#include "rendering/renderer.h"
#include "rendering/rendering_manager.h"
#include "particle_manager.h"
#include "rendering/text_renderer.h"
#include "blocks/block.h"
#include "util.h"
#include "map_generation/map_generator.h"
#include "blocks/chunk.h"
#include "blocks/chunk_manager.h"


enum GameState {
	GAME_ACTIVE,
	GAME_MENU,
	DEBUG
};

enum Direction {
	UP,
	RIGHT,
	DOWN,
	LEFT
};

struct Collision {
	bool occured;
	Direction dir;
	glm::vec2 diff;
};


class Game
{
public:
	//GameState State = GAME_ACTIVE;
	GameState State = DEBUG;

	Renderer* renderer;
	TextRenderer* text_renderer;
	RenderingManager* rendering_manager;

	std::vector<Block*> Blocks;

	//Player* player;
	Camera* camera;
	MapGenerator* map_generator;

	ThreadPool thread_pool;

	ParticleManager* particle_manager;
	ChunkManager* chunk_manager;

	int seed = 123457;

	int LevelWidth;
	int LevelHeight;

	int lives = 3;
	int level = 1;

	float left_stick_x = 0;
	float left_stick_y = 0;

	float right_stick_x = 0;
	float right_stick_y = 0;

	float mouse_x_offset = 0;
	float mouse_y_offset = 0;

	float yaw = 0;
	float pitch = 0;
	float roll = 0;

	bool keyboard_keys[1024] = { false };
	bool keyboard_keys_processed[1024] = { false };

	bool gamepad_keys[1024] = { false };
	bool gamepad_keys_processed[1024] = { false };

	int last_visible_north_block = 0;
	int last_visible_south_block = 0;
	int last_visible_east_block = 0;
	int last_visible_west_block = 0;

	FPSCounter fps_counter;
	float fps = 0;

	Game(int levelWidth, int levelHeight, Renderer* renderer);
	~Game();

	void Init();

	void ResetGame();
	void LoadLevel(int offset_x, int offset_z);

	void CheckLastVisibleChunkCoord();

	// Game loop
	void ProcessInput(float dt);
	void Update(float dt);
	void Render();
	void CheckCollisions();
	//Collision CheckBallCollision(Ball& one, Entity& two);
	Collision CheckCollision(Entity& one, Entity& two);
};