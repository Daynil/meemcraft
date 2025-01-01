#include "shader.h"

#include <fstream>
#include <sstream>
#include <iostream>


Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
{
	parseShaders(vertexPath, fragmentPath);
	createProgram();
}

Shader::Shader(const std::string& vertexFragPath)
{
	parseShaders(vertexFragPath);
	createProgram();
}

void Shader::parseShaders(const std::string& vertexPath, const std::string& fragmentPath)
{
	// 1. retrieve the vertex/fragment source code from filePath
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	// ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// open files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		// read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// close file handlers
		vShaderFile.close();
		fShaderFile.close();
		// convert stream into string
		VertexSource = vShaderStream.str();
		FragmentSource = fShaderStream.str();
	}
	catch (std::ifstream::failure& e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
	}
}

void Shader::parseShaders(const std::string& vertexFragPath)
{
	std::ifstream stream(vertexFragPath);

	enum class ShaderType
	{
		None = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::None;
	while (std::getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
			{
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos)
			{
				type = ShaderType::FRAGMENT;
			}
		}
		else
		{
			ss[(int)type] << line << "\n";
		}
	}

	VertexSource = ss[0].str();
	FragmentSource = ss[1].str();
}

unsigned int Shader::compileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	// OpenGL expects a raw C string not C++ std string.
	const char* src = source.c_str();
	// Pretty straightforward boilerplate, only weird bit is it wants a pointer to a pointer
	// for the src, that's why we use ampersand with something already a pointer.
	// `nullptr` just indicates we want to pass the whole string.
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	checkCompileErrors(id, type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT");

	return id;
}

void Shader::createProgram()
{
	ID = glCreateProgram();
	unsigned int vs = compileShader(GL_VERTEX_SHADER, VertexSource);
	unsigned int fs = compileShader(GL_FRAGMENT_SHADER, FragmentSource);

	// This combines the individual shaders into one shader program
	glAttachShader(ID, vs);
	glAttachShader(ID, fs);
	glLinkProgram(ID);

	checkCompileErrors(ID, "PROGRAM");
	//glValidateProgram(ID);

	// Delete intermediates now that we're done
	glDeleteShader(vs);
	glDeleteShader(fs);
}

void Shader::checkCompileErrors(unsigned int shader, std::string type)
{
	int success;
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			glDeleteShader(shader);
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
}

int Shader::checkGetUniform(const std::string& name) const
{
	int location = glGetUniformLocation(ID, name.c_str());
	if (location == -1)
	{
		std::cout << "ERROR::Could not find uniform: " << name << std::endl;
	}

	return location;
}

void Shader::activate()
{
	glUseProgram(ID);
}
void Shader::deactivate()
{
	glUseProgram(0);
}
void Shader::deleteShader()
{
	glDeleteProgram(ID);
}

void Shader::setBool(const std::string& name, bool value) const
{
	glUniform1i(checkGetUniform(name.c_str()), (int)value);
}
void Shader::setInt(const std::string& name, int value) const
{
	glUniform1i(checkGetUniform(name.c_str()), value);
}
void Shader::setFloat(const std::string& name, float value) const
{
	glUniform1f(checkGetUniform(name.c_str()), value);
}
void Shader::setVec3(const std::string& name, glm::vec3& value) const
{
	glUniform3f(checkGetUniform(name.c_str()), value.x, value.y, value.z);
}
void Shader::setVec4(const std::string& name, glm::vec4& value) const
{
	glUniform4f(checkGetUniform(name.c_str()), value.x, value.y, value.z, value.w);
}
void Shader::setMat4(const std::string& name, glm::f32* value) const
{
	glUniformMatrix4fv(checkGetUniform(name.c_str()), 1, GL_FALSE, value);
}