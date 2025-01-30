#include "renderer.h"
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include "shader.h"


void Renderer::init(Camera* p_camera)
{
	camera = p_camera;
}

void Renderer::bind_vertex_data(Entity& entity)
{
	glBindVertexArray(entity.model->VAO_ID);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
}

void Renderer::bind_texture(Entity& entity)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, entity.texture->textureID);
}

void Renderer::prepare_entity(Entity& entity)
{
	entity.shader->activate();

	if (entity.is_transparent) {
		glDepthMask(GL_FALSE);
	}

	float aspectRatio = display->displayWidth / display->displayHeight;

	// This directly translates display pixel size to NDC
	glm::mat4 projection = glm::perspective(glm::radians(camera->FOV), aspectRatio, camera->NEAR_PLANE, camera->FAR_PLANE);
	entity.shader->setMat4("projection", glm::value_ptr(projection));

	glm::mat4 view;
	view = glm::lookAt(camera->cameraPos, camera->cameraPos + camera->cameraFront, camera->cameraUp);
	entity.shader->setMat4("view", glm::value_ptr(view));

	bind_texture(entity);
	bind_vertex_data(entity);
}

void Renderer::cleanup_entity(Entity& entity)
{
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glBindVertexArray(0);
	if (entity.is_transparent) {
		glDepthMask(GL_TRUE);
	}
	entity.shader->deactivate();
}

// Non-shared things each entity needs to render
void Renderer::render_entity(Entity& entity)
{
	// Apply entity positions and transformations
	glm::mat4 translate = glm::translate(glm::mat4(1.0f), entity.position);

	glm::vec3 eulerAngles(glm::radians(entity.rotation.x), glm::radians(entity.rotation.y), glm::radians(entity.rotation.z));
	glm::mat4 rotation = glm::toMat4(glm::quat(eulerAngles));

	glm::mat4 scale = glm::scale(glm::mat4(1.0f), entity.scale);

	glm::mat4 transform = translate * rotation * scale;
	entity.shader->setMat4("transform", glm::value_ptr(transform));

	entity.ProvideRenderData();

	glDrawElements(GL_TRIANGLES, entity.model->vertex_count, GL_UNSIGNED_INT, 0);
}

void Renderer::prepare_chunk(Entity& entity)
{
	entity.shader->activate();

	if (entity.is_transparent) {
		glDepthMask(GL_FALSE);
	}

	float aspectRatio = display->displayWidth / display->displayHeight;

	// This directly translates display pixel size to NDC
	glm::mat4 projection = glm::perspective(glm::radians(camera->FOV), aspectRatio, camera->NEAR_PLANE, camera->FAR_PLANE);
	entity.shader->setMat4("projection", glm::value_ptr(projection));

	glm::mat4 view;
	view = glm::lookAt(camera->cameraPos, camera->cameraPos + camera->cameraFront, camera->cameraUp);
	entity.shader->setMat4("view", glm::value_ptr(view));

	bind_texture(entity);
}

void Renderer::render_chunk(Entity& entity)
{
	bind_vertex_data(entity);

	// Apply entity positions and transformations
	glm::mat4 translate = glm::translate(glm::mat4(1.0f), entity.position);

	glm::vec3 eulerAngles(glm::radians(entity.rotation.x), glm::radians(entity.rotation.y), glm::radians(entity.rotation.z));
	glm::mat4 rotation = glm::toMat4(glm::quat(eulerAngles));

	glm::mat4 scale = glm::scale(glm::mat4(1.0f), entity.scale);

	glm::mat4 transform = translate * rotation * scale;
	entity.shader->setMat4("transform", glm::value_ptr(transform));

	entity.ProvideRenderData();

	glDrawElements(GL_TRIANGLES, entity.model->vertex_count, GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void Renderer::prepare()
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
