#pragma once
#ifndef SHINA_LIB_LIGHT_H
#define SHINA_LIB_LIGHT_H
#include "main.h"
#include "model.h"


class smLight {
public:
	using vec3 = glm::vec3;
	using mat4 = glm::mat4;
	//vec3 position;
	GLuint lightId;

	glm::vec3 pos;
	GLuint frameHandle, textureHandle;

	glm::mat4 lightMatrix;


public :
	vec3 diffuse, ambient;
	GLfloat specular;
	smLight(GLuint id,vec3 position=vec3(0,0,0),vec3 diff= vec3(0, 0, 0),vec3 amb= vec3(0, 0, 0),GLfloat spe= 0):
		lightId(id),pos(position),diffuse(diff),ambient(amb),specular(spe)
	{
		glGenFramebuffers(1, &frameHandle);
		glGenTextures(1, &textureHandle);

		glBindTexture(GL_TEXTURE_2D, textureHandle);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		glBindFramebuffer(GL_FRAMEBUFFER, frameHandle);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureHandle, 0);

		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, NULL);
	}
	smLight(const smLight &l) {
		pos = l.pos;
		diffuse = l.diffuse;
		ambient = l.ambient;
		specular = l.specular;
		textureHandle = l.textureHandle;
		frameHandle = l.frameHandle;
		lightId = l.lightId;
	}

};


#endif
