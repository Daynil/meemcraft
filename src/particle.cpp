#include "particle.h"

void Particle::ProvideRenderData()
{
	shader->setVec4("u_color", color);
}
