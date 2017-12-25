#pragma once
#ifndef SHINA_LIB_SPHERE_H
#define SHINA_LIB_SPHERE_H

#include "main.h"

class Sphere
{
public:
	Sphere(GLfloat radius, GLfloat _step,glm::vec4 color);
	~Sphere()
	{
		
	}

	void init();
	void show();

	inline void loadIdentity()
	{
		this->model = glm::mat4();
	}

	inline void translate(GLfloat x, GLfloat y, GLfloat z)
	{
		this->model = glm::translate(this->model, glm::vec3(x, y, z));
	}
	inline void scale(GLfloat x, GLfloat y, GLfloat z)
	{
		this->model = glm::scale(this->model, glm::vec3(x, y, z));
	}
	inline void rotate(GLfloat angel, glm::vec3 axis)
	{
		this->model = glm::rotate(this->model, angel, axis);
	}
	

private:
	GLuint vao, vbo;
	GLfloat radius;
	vector<GLfloat> pos;
	GLfloat step;
	glm::vec4 color;

	glm::mat4 model;
	
	GLfloat *getPos() {
		return toArray<GLfloat>(&pos);
	}
	void pushPos(float pos1 = 0, float pos2 = 0, float pos3 = 0) {
		pos.push_back(pos1);
		pos.push_back(pos2);
		pos.push_back(pos3);
	}
	
};

#endif // SPHERE_H