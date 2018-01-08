#include "lib\render.h"
#include "lib\sphere.h"
extern smShader *elementShader;
extern smShader *texShader;
extern smShader *shadowShader;
extern smShader *paticleShader;
extern smShader * texParticleShader;

void smRender::render(world & myworld,smCamera& camera)
{
	if (useShadow)
	{
		texShader->use();
		texShader->setInt("u_lightNum", myworld.getLights().size());

		for (auto& light : myworld.getLights())
		{

			//cout << "use shadow" << endl;
			glm::mat4 lightProjection, lightView;
			//glm::mat4 lightMatrix;
			lightProjection = glm::perspective(60.f, (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, 8.f, 10000.0f);
			lightView = glm::lookAt(light->pos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
			light->lightMatrix = lightProjection * lightView;
			shadowShader->use();
			shadowShader->setMat4("u_lightMatrix", light->lightMatrix);

			glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
			glBindFramebuffer(GL_FRAMEBUFFER, light->frameHandle);
			glClear(GL_DEPTH_BUFFER_BIT);
			for (auto& s : myworld.getScenes())
				s->shadow();
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

		}

	}

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 projection = glm::perspective(45.f, (float)SCR_WIDTH / (float)SCR_HEIGHT, 1.f, 8096.0f);
	glm::mat4 view = glm::lookAt(camera.eye, camera.eye + camera.dir, glm::vec3(0.0, 1.0, 0.0));
	elementShader->use();
	elementShader->setMat4("u_projection", projection);
	elementShader->setMat4("u_view", view);
	elementShader->setVec3("u_eyePos", camera.eye);

	for (int i = 0; i <  myworld.getLights().size(); i++) {
		char tmp[64];
		sprintf(tmp, "u_lightsMatrix[%d]", i);
		elementShader->setMat4(tmp, myworld.getLights()[i]->lightMatrix);
		sprintf(tmp, "u_lightInfo[%d].u_lightPos", i);
		elementShader->setVec3(tmp, myworld.getLights()[i]->pos);
		sprintf(tmp, "u_lightInfo[%d].u_lightDiff", i);
		elementShader->setVec3(tmp, myworld.getLights()[i]->diffuse);
		sprintf(tmp, "u_lightInfo[%d].u_lightAmb", i);
		elementShader->setVec3(tmp, myworld.getLights()[i]->ambient);
		sprintf(tmp, "u_lightInfo[%d].u_lightSpec", i);
		elementShader->setFloat(tmp, myworld.getLights()[i]->specular);
	}


	texShader->use();
	texShader->setMat4("u_projection", projection);
	texShader->setMat4("u_view", view);
	texShader->setVec3("u_eyePos", camera.eye);


	for (int i = 0; i < myworld.getLights().size(); i++) {
		char tmp[64];
		sprintf(tmp, "u_lightsMatrix[%d]", i);
		texShader->setMat4(tmp, myworld.getLights()[i]->lightMatrix);
		sprintf(tmp, "u_lightInfo[%d].u_lightPos", i);
		texShader->setVec3(tmp, myworld.getLights()[i]->pos);
		sprintf(tmp, "u_lightInfo[%d].u_lightDiff", i);
		texShader->setVec3(tmp, myworld.getLights()[i]->diffuse);
		sprintf(tmp, "u_lightInfo[%d].u_lightAmb", i);
		texShader->setVec3(tmp, myworld.getLights()[i]->ambient);
		sprintf(tmp, "u_lightInfo[%d].u_lightSpec", i);
		texShader->setFloat(tmp, myworld.getLights()[i]->specular);
	}

	paticleShader->use();
	paticleShader->setMat4("u_projection", projection);
	paticleShader->setMat4("u_view", view);
	texParticleShader->use();
	texParticleShader->setMat4("u_projection", projection);
	texParticleShader->setMat4("u_view", view);


	for (int i = 0; i < myworld.getLights().size(); i++) {
		glActiveTexture(GL_TEXTURE1 + i);
		glBindTexture(GL_TEXTURE_2D, myworld.getLights()[i]->textureHandle);
	}
	
	for (auto& light : myworld.getLights())
	{
		for (auto& s : myworld.getScenes()) 
		{
			s->show(light, myworld.getLights().size());
		}
	}
}