#include "renderer.h"
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

void Renderer::prepare_entity(Entity& entity)
{
	entity.shader->activate();

	float aspectRatio = display->displayWidth / display->displayHeight;

	// This directly translates display pixel size to NDC
	glm::mat4 projection = glm::perspective(glm::radians(camera->FOV), aspectRatio, camera->NEAR_PLANE, camera->FAR_PLANE);
	entity.shader->setMat4("projection", glm::value_ptr(projection));

	glm::mat4 view;
	view = glm::lookAt(camera->cameraPos, camera->cameraPos + camera->cameraFront, camera->cameraUp);
	entity.shader->setMat4("view", glm::value_ptr(view));

	glBindVertexArray(entity.model->VAO_ID);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, entity.texture->textureID);
}

void Renderer::cleanup_entity(Entity& entity)
{
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glBindVertexArray(0);
	entity.shader->deactivate();
}

// Non-shared things each entity needs to render
void Renderer::render(Entity& entity)
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

//void Renderer::renderold(Entity& entity, Shader& shader)
//{
//	glBindVertexArray(entity.model->VAO_ID);
//	glEnableVertexAttribArray(0);
//	glEnableVertexAttribArray(1);
//	shader.activate();
//
//	float aspectRatio = display->displayWidth / display->displayHeight;
//
//	// This directly translates display pixel size to NDC
//	glm::mat4 projection = glm::perspective(glm::radians(camera->FOV), aspectRatio, camera->NEAR_PLANE, camera->FAR_PLANE);
//	shader.setMat4("projection", glm::value_ptr(projection));
//
//	// Apply entity positions and transformations
//	glm::mat4 translate = glm::translate(glm::mat4(1.0f), entity.position);
//
//	glm::vec3 eulerAngles(glm::radians(entity.rotation.x), glm::radians(entity.rotation.y), glm::radians(entity.rotation.z));
//	glm::mat4 rotation = glm::toMat4(glm::quat(eulerAngles));
//
//	glm::mat4 scale = glm::scale(glm::mat4(1.0f), entity.scale);
//
//	glm::mat4 transform = translate * rotation * scale;
//	shader.setMat4("transform", glm::value_ptr(transform));
//
//	glm::mat4 view;
//	view = glm::lookAt(camera->cameraPos, camera->cameraPos + camera->cameraFront, camera->cameraUp);
//	shader.setMat4("view", glm::value_ptr(view));
//
//	entity.ProvideRenderData(shader);
//
//	glActiveTexture(GL_TEXTURE0);
//
//	glBindTexture(GL_TEXTURE_2D, entity.texture->textureID);
//
//	glDrawElements(GL_TRIANGLES, entity.model->vertex_count, GL_UNSIGNED_INT, 0);
//
//	shader.deactivate();
//	glDisableVertexAttribArray(0);
//	glDisableVertexAttribArray(1);
//	glBindVertexArray(0);
//}

void Renderer::prepare()
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
