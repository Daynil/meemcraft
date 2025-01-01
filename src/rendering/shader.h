#pragma once
#include <glad/glad.h>

#include <string>
#include <array>

#include <glm/glm.hpp>


class Shader
{
public:
	// OpenGL program ID
	unsigned int ID;

	// Constructor reads and builds the shaders
	Shader(const std::string& vertexPath, const std::string& fragmentPath);
	// Combined shaders - use #shader vertex and #shader fragment
	// as the delimeter.
	Shader(const std::string& vertexFragPath);

	// Activates the shader
	void activate();
	void deactivate();

	void deleteShader();
	// Utility functions to set uniforms
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setVec3(const std::string& name, glm::vec3& value) const;
	void setVec4(const std::string& name, glm::vec4& value) const;
	void setMat4(const std::string& name, glm::f32* value) const;

private:
	std::string VertexSource;
	std::string FragmentSource;

	// Parses the shader source files to strings
	void parseShaders(const std::string& vertexPath, const std::string& fragmentPath);
	void parseShaders(const std::string& filePath);

	unsigned int compileShader(unsigned int type, const std::string& source);
	// Check for shader compilation/linking errors and log to cout ifa ny
	void checkCompileErrors(unsigned int shader, std::string type);

	void createProgram();
	int checkGetUniform(const std::string& name) const;
};
