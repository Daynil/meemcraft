#pragma once
#include <string>
#include <vector>

#include "rendering/texture.h"
#include "raw_model.h"

class Model
{
public:
	RawModel* raw_model;
	Texture* texture;

	Model(RawModel* raw_model, Texture* texture) : raw_model(raw_model), texture(texture) {};
};