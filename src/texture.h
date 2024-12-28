#pragma once
#include <string>

class Texture
{
public:
	unsigned int textureID;

	Texture(std::string texturePath, bool alpha);

	void Delete();
};