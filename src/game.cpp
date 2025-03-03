#include "game.h"

#include <vector>

#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
#include <format>
#include <future>

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <raudio.h>

#include "blocks/block_data.h"
#include "resource_manager.h"
#include "rendering/text_renderer.h"
#include "map_generation/map_generator.h"
#include "rendering/rendering_manager.h"
#include "shared.h"

void Game::Init()
{
	BlockLoader block_loader;
	block_loader.LoadBlocks();
	ResourceManager::texture_atlas_x_unit = block_loader.texture_atlas_x_unit;

	ResourceManager::LoadShader("entity", Shader(RESOURCES_PATH "shaders/entity.shader"));
	ResourceManager::LoadShader("entity_tinted", Shader(RESOURCES_PATH "shaders/entity_tinted.shader"));
	ResourceManager::LoadShader("particle", Shader(RESOURCES_PATH "shaders/particle.shader"));
	ResourceManager::LoadShader("text", Shader(RESOURCES_PATH "shaders/text.shader"));

	InitAudioDevice();
	//ResourceManager::LoadRSound("brick", RESOURCES_PATH "bleep.mp3");
	ResourceManager::LoadRSound("brick", RESOURCES_PATH "solid.wav");
	ResourceManager::LoadRSound("brick_solid", RESOURCES_PATH "solid.wav");
	ResourceManager::LoadRSound("powerup", RESOURCES_PATH "powerup.wav");
	ResourceManager::LoadRSound("paddle", RESOURCES_PATH "bleep.wav");
	ResourceManager::LoadRSound("laser", RESOURCES_PATH "laser.wav");

	text_renderer = new TextRenderer(LevelWidth, LevelHeight, RESOURCES_PATH "PressStart2P.ttf");

	camera = new Camera();
	renderer->init(camera);
	rendering_manager = new RenderingManager(renderer, renderer->display);
	rendering_manager->Init(camera);

	map_generator = new MapGenerator(rendering_manager, 123457);
	chunk_manager = new ChunkManager(&thread_pool, map_generator, camera);

	//particle_manager = new ParticleManager(
	//	&ResourceManager::GetRawModel("quad"),
	//	&ResourceManager::GetTexture("particle"),
	//	&ResourceManager::GetShader("particle"),
	//	ball
	//);
	//particle_manager->Init(800);

	ResetGame();
}

void Game::ResetGame()
{
	LoadLevel(0, 0);
}

void Game::LoadLevel(int offset_x, int offset_z)
{
	//Blocks.clear();

	chunk_manager->ClearChunks();

	//Blocks.push_back(new Block(BlockType::GRASS_BLOCK, glm::vec3(0, 0, -2)));
	Blocks.push_back(new Block(BlockType::SELECTED, glm::vec3(0, 0, 13)));

	auto t = ChunkHelpers::ChunkLocalToWorldCoord(
		glm::vec3(-9, 0, 9), glm::vec3(0, 73, 13)
	);

	print(std::format("Test block world coord: {0}, {1}, {2}", t.x, t.y, t.z));
	//id.x == -10 && id.y == 9 && x == 15 && z == 12 && y == 73
	Blocks.push_back(new Block(
		BlockType::SELECTED,
		glm::vec3(t.x, t.y - 55, t.z)
	));

	chunk_manager->RefreshChunksCenteredAt(glm::vec2(0, 0));
}

bool first_gen = true;
void Game::CheckLastVisibleChunkCoord()
{
	auto last_viz_z = camera->cameraPos.z + (ChunkManager::VIEW_DIST_CHUNKS * ChunkHelpers::CHUNK_SIZE_X);
	int last_viz_cz = last_viz_z / ChunkHelpers::CHUNK_SIZE_Z;

	//if (last_viz_cz > last_visible_south_block && first_gen) {
	//	//first_gen = false;
	//	last_visible_south_block = last_viz_cz;
	//	print("New last vis cz" + std::to_string(last_viz_cz));
	//	chunk_manager->RefreshChunksCenteredAt(glm::vec2(camera->cameraPos.x, camera->cameraPos.z));
	//}
	//print(std::format("Last visible chunk world z coord: {0}", last_viz_cz));

	//auto chunk_coord = ChunkHelpers::WorldCoordToChunkCoord(camera->cameraPos);
	//print(std::format("Chunk world coord {0}", vec_to_string(chunk_coord)));

	//auto chunk_local_coord = ChunkHelpers::WorldCoordToChunkLocal(camera->cameraPos);
	//print(std::format("Block in chunk coord {0}", vec_to_string(chunk_local_coord)));
}


void Game::ProcessInput(float dt)
{
	if (State == DEBUG) {
		if (keyboard_keys[GLFW_KEY_G] && !keyboard_keys_processed[GLFW_KEY_G]) {
			//map_generator->Reseed(random_int(1, 123459));
			LoadLevel(0, 0);

			keyboard_keys_processed[GLFW_KEY_G] = true;
		}

		if (keyboard_keys[GLFW_KEY_SPACE] && !keyboard_keys_processed[GLFW_KEY_SPACE]) {
			auto world = glm::vec3(int(camera->cameraPos.x), int(camera->cameraPos.y), int(camera->cameraPos.z));
			auto chunk = ChunkHelpers::WorldCoordToChunkCoord(world);
			auto local = ChunkHelpers::WorldCoordToChunkLocal(world);

			print(std::format("Placing selection at world: {0}", vec_to_string(world)));
			print(std::format("Placing selection at chunk: {0}", vec_to_string(chunk)));
			print(std::format("Placing selection at local: {0}", vec_to_string(local)));
			//id.x == -10 && id.y == 9 && x == 15 && z == 12 && y == 73
			Blocks.push_back(new Block(
				BlockType::SELECTED,
				world
			));

			keyboard_keys_processed[GLFW_KEY_SPACE] = true;
		}

		if (keyboard_keys[GLFW_KEY_C] && !keyboard_keys_processed[GLFW_KEY_C]) {
			Blocks.clear();

			keyboard_keys_processed[GLFW_KEY_C] = true;
		}
	}

	if (State == GAME_MENU) {
		if (keyboard_keys[GLFW_KEY_ENTER] && !keyboard_keys_processed[GLFW_KEY_ENTER]) {
			State = GAME_ACTIVE;
			keyboard_keys_processed[GLFW_KEY_ENTER] = true;
		}

		if (keyboard_keys[GLFW_KEY_W] && !keyboard_keys_processed[GLFW_KEY_W]) {
			level += 1;
			if (level == 11)
				level = 10;
			//LoadLevel();
			keyboard_keys_processed[GLFW_KEY_W] = true;
		}
		if (keyboard_keys[GLFW_KEY_S] && !keyboard_keys_processed[GLFW_KEY_S]) {
			level -= 1;
			if (level == 0)
				level = 1;
			//LoadLevel();
			keyboard_keys_processed[GLFW_KEY_S] = true;
		}
		return;
	}

	float player_movement = 0;
	float should_release = false;

	// Keyboard controls
	if (keyboard_keys[GLFW_KEY_W] || keyboard_keys[GLFW_KEY_UP]) {
		camera->Move(dt, glm::vec2(0.0f, -1.0f));
		CheckLastVisibleChunkCoord();
		//print(std::format("Player pos: {0}, {1}", camera->cameraPos.x, camera->cameraPos.z));
	}
	if (keyboard_keys[GLFW_KEY_A] || keyboard_keys[GLFW_KEY_LEFT]) {
		camera->Move(dt, glm::vec2(-1.0f, 0.0f));
	}
	if (keyboard_keys[GLFW_KEY_S] || keyboard_keys[GLFW_KEY_DOWN]) {
		camera->Move(dt, glm::vec2(0.0f, 1.0f));
	}
	if (keyboard_keys[GLFW_KEY_D] || keyboard_keys[GLFW_KEY_RIGHT]) {
		camera->Move(dt, glm::vec2(1.0f, 0.0f));
	}

	// Gamepad controls
	if (left_stick_x != 0 || left_stick_y != 0) {
		camera->Move(dt, glm::vec2(left_stick_x, left_stick_y));
		CheckLastVisibleChunkCoord();
	}

	if (gamepad_keys[GLFW_GAMEPAD_BUTTON_X])
		should_release = true;
}

void Game::Update(float dt)
{
	chunk_manager->ProcessChunks();
	//chunk_manager->UploadCompletedChunks();
	fps = fps_counter.Update();
	//CheckCollisions();
	//particle_manager->Update(dt);

	//player->Update(dt);
}


void Game::Render()
{
	for (auto& block : Blocks)
	{
		rendering_manager->ProcessBlock(block);
	}

	for (auto& p_chunk : chunk_manager->chunks)
	{
		rendering_manager->ProcessChunk(p_chunk.second);
	}

	if (State == DEBUG && map_generator->texture) {
		map_generator->DrawNoisemap(camera->cameraPos);
	}

	rendering_manager->Render();

	text_renderer->RenderText("FPS: " + std::format("{:.2f}", fps), glm::vec2(5.0f, 5.0f), 0.4f);

	if (State == GAME_MENU) {
		text_renderer->RenderText("Press Enter or Start to start", glm::vec2(140.0f, LevelHeight / 2.0f), 0.4f);
		text_renderer->RenderText("Press W or S to select level", glm::vec2(210.0f, (LevelHeight / 2.0f) + 30.0f), 0.3f);
	}
}

Direction VectorDirection(glm::vec2 target)
{
	glm::vec2 compass[] = {
		glm::vec2(0.0f, 1.0f),	// up
		glm::vec2(1.0f, 0.0f),	// right
		glm::vec2(0.0f, -1.0f),	// down
		glm::vec2(-1.0f, 0.0f)	// left
	};
	float max = 0.0f;
	unsigned int best_match = -1;
	for (unsigned int i = 0; i < 4; i++)
	{
		float dot_product = glm::dot(glm::normalize(target), compass[i]);
		if (dot_product > max)
		{
			max = dot_product;
			best_match = i;
		}
	}
	return (Direction)best_match;
}


Collision Game::CheckCollision(Entity& one, Entity& two)
{
	// Simple collision check if both are square colliders
	bool x_overlap = (one.position.x + one.scale.x) >= two.position.x && one.position.x <= (two.position.x + two.scale.x);
	bool y_overlap = (one.position.y + one.scale.y) >= two.position.y && one.position.y <= (two.position.y + two.scale.y);
	return Collision{ x_overlap && y_overlap, UP, glm::vec2(0) };
}



Game::Game(int levelWidth, int levelHeight, Renderer* renderer) : LevelWidth(levelWidth), LevelHeight(levelHeight), renderer(renderer), thread_pool(GetOptimalThreadCount())
{
}

Game::~Game()
{
	//delete player;
	delete camera;
	delete text_renderer;
	delete map_generator;
	delete rendering_manager;
	delete chunk_manager;
}