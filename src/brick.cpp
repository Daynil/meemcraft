#include "brick.h"

void Brick::ProvideRenderData(Shader& shader)
{
	shader.setVec4("u_color", color);
}
