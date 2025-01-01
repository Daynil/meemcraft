#pragma once

#include <map>
#include <vector>

#include "block_data.h"
#include "block.h"
#include "entity.h"
#include "camera.h"
#include "shader.h"
#include "display.h"
#include "renderer.h"

// Batch rendering manager
// Everything that needs to be rendered each frame needs to be stored here.
// Text does this in its own renderer.
class RenderingManager
{
public:
	Camera* camera;
	Display* display;
	Renderer* renderer;

	// Batch-rendered blocks - each block group has the same model and texture
	std::map<BlockType, std::vector<Block*>> block_groups;
	// Individually-rendered entities - everythign else 
	// (can break out into groups when there are enough eventually).
	std::vector<Entity*> entities;

	RenderingManager(Renderer* renderer, Display* display) : renderer(renderer), display(display) {};

	void Init(Camera* p_camera);
	void Render();
	void ProcessBlock(Block* block);
	void ProcessEntity(Entity* entity);
};