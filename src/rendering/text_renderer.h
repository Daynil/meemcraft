#pragma once

#include <map>
#include <string>
#include <glm/glm.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

struct Character {
	// ID handle of the glyph texture
	unsigned int TextureID;
	// Size of glyph
	glm::ivec2   Size;
	// Offset from baseline to left/top of glyph
	glm::ivec2   Bearing;
	// Offset to advance to next glyph
	unsigned int Advance;
};

class TextRenderer
{
public:
	std::map<char, Character> characters;
	float screen_width, screen_height;

	TextRenderer(float screen_width, float screen_height, std::string font_path);

	void LoadCharacters(std::string font_path);

	void RenderText(std::string text, glm::vec2 position, float scale, glm::vec3 color = glm::vec3(1.0f));

private:
	unsigned int textVAO, textVBO;
};