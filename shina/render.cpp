#include "lib\render.h"
#include "lib\sphere.h"
extern smShader *elementShader;
extern smShader *texShader;
extern smShader *shadowShader;
extern smShader *paticleShader;
extern Sphere *sphere;

void smRender::render(world & myworld,smCamera& camera)
{
	for (auto& light : myworld.getLights())
	{
		if (useShadow)
		{
			//cout << "use shadow" << endl;
			glm::mat4 lightProjection, lightView;
			//glm::mat4 lightMatrix;
			lightProjection = glm::perspective(60.f, (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, 8.f, 10000.0f);
			lightView = glm::lookAt(light.pos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
			light.lightMatrix = lightProjection * lightView;
			shadowShader->use();
			shadowShader->setMat4("u_lightMatrix", light.lightMatrix);

			glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
			glBindFramebuffer(GL_FRAMEBUFFER, light.frameHandle);
			glClear(GL_DEPTH_BUFFER_BIT);
			for (auto& s :myworld.getScenes())
				s.shadow();
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glm::mat4 projection = glm::perspective(45.f, (float)SCR_WIDTH / (float)SCR_HEIGHT, 1.f, 8096.0f);
		glm::mat4 view = glm::lookAt(camera.eye, camera.eye + camera.dir, glm::vec3(0.0, 1.0, 0.0));
		elementShader->use();
		elementShader->setMat4("u_projection", projection);
		elementShader->setMat4("u_view", view);
		elementShader->setMat4("u_lightMatrix", light.lightMatrix);
		elementShader->setVec3("u_eyePos", camera.eye);
		elementShader->setVec3("u_lightPos", light.pos);
		//cout << "posx:" << pos.x << " " << pos.y << " " << pos.z << endl;
		elementShader->setVec3("u_lightDiff", light.diffuse);
		elementShader->setVec3("u_lightAmb", light.ambient);
		elementShader->setFloat("u_lightSpec", light.specular);
		texShader->use();
		texShader->setMat4("u_projection", projection);
		texShader->setMat4("u_view", view);
		texShader->setMat4("u_lightMatrix", light.lightMatrix);
		texShader->setVec3("u_eyePos", camera.eye);
		texShader->setVec3("u_lightPos", light.pos);
		texShader->setVec3("u_lightDiff", light.diffuse);
		texShader->setVec3("u_lightAmb", light.ambient);
		texShader->setFloat("u_lightSpec", light.specular);
		paticleShader->use();
		paticleShader->setMat4("u_projection", projection);
		paticleShader->setMat4("u_view", view);
		//paticleShader->setMat4("u_lightMatrix", light.lightMatrix);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, light.textureHandle);
		for (auto& s : myworld.getScenes()) 
		{
			s.show(light);
		}
	}


	
}