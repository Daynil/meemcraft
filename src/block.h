#pragma once

#include "string"

#include <glm/glm.hpp>

#include "entity.h"
#include "shader_s.h"


class Block : public Entity
{
public:
	std::string type;

	Block(std::string type, glm::vec3 position, bool symmetrical = true);
};