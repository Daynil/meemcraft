#include "game.h"

#include <vector>

#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
#include <format>

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <raudio.h>
#include "perlin_noise.hpp"

#include "block_data.h"
#include "resource_manager.h"
#include "text_renderer.h"


void Game::Init()
{
	BlockLoader block_loader;
	block_loader.LoadBlocks();


	//for (int i = 0; i < BlockType::BLOCK_COUNT; i++)
	//{
	//	std::string type = BlockTypeString[i];
	//	ResourceManager::LoadTexture(type, Texture(RESOURCES_PATH "textures/" + type + ".png", true));
	//}

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
	LoadLevel();
}

void Game::LoadLevel()
{
	const siv::PerlinNoise::seed_type seed = 123456u;
	const siv::PerlinNoise perlin{ seed };

	for (int y = 0; y < 5; ++y)
	{
		for (int x = 0; x < 5; ++x)
		{
			const double noise = perlin.octave2D_01((x * 0.01), (y * 0.01), 4);

			std::cout << noise << '\t';
		}

		std::cout << '\n';
	}

	Blocks.clear();

	for (int i = 0; i < BlockType::BLOCK_COUNT; i++)
	{
		Blocks.push_back(Block(
			(BlockType)i,
			glm::vec3(i, 0, i)
		));
	}
}


void Game::ProcessInput(float dt)
{
	if (State == GAME_MENU) {
		if (keyboard_keys[GLFW_KEY_ENTER] && !keyboard_keys_processed[GLFW_KEY_ENTER]) {
			State = GAME_ACTIVE;
			keyboard_keys_processed[GLFW_KEY_ENTER] = true;
		}

		if (keyboard_keys[GLFW_KEY_W] && !keyboard_keys_processed[GLFW_KEY_W]) {
			level += 1;
			if (level == 11)
				level = 10;
			LoadLevel();
			keyboard_keys_processed[GLFW_KEY_W] = true;
		}
		if (keyboard_keys[GLFW_KEY_S] && !keyboard_keys_processed[GLFW_KEY_S]) {
			level -= 1;
			if (level == 0)
				level = 1;
			LoadLevel();
			keyboard_keys_processed[GLFW_KEY_S] = true;
		}
		return;
	}

	float player_movement = 0;
	float should_release = false;

	// Keyboard controls
	if (keyboard_keys[GLFW_KEY_W] || keyboard_keys[GLFW_KEY_UP]) {
		camera->Move(dt, glm::vec2(0.0f, -1.0f));
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
	if (left_stick_x != 0 || left_stick_y != 0)
		camera->Move(dt, glm::vec2(left_stick_x, left_stick_y));

	if (gamepad_keys[GLFW_GAMEPAD_BUTTON_X])
		should_release = true;
}

void Game::Update(float dt)
{
	fps = fps_counter.Update();
	//CheckCollisions();
	//particle_manager->Update(dt);

	//player->Update(dt);
}


void Game::Render()
{
	renderer->prepare();

	/*renderer->render(Entity(&ResourceManager::GetRawModel("quad"), &ResourceManager::GetTexture("background"), glm::vec3(0, 0, 0), glm::vec3(0), glm::vec3(LevelWidth, LevelHeight, 0)), ResourceManager::GetShader("entity"));*/

	for (auto& block : Blocks)
	{
		renderer->render(block, ResourceManager::GetShader("entity"));
	}

	//renderer->render(*player, ResourceManager::GetShader("entity_tinted"));

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



Game::~Game()
{
	//delete player;
	delete camera;
	delete text_renderer;
}