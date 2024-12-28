#include "particle.h"

void Particle::ProvideRenderData(Shader& shader)
{
	shader.setVec4("u_color", color);
}
