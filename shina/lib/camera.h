#pragma once
#ifndef SHINA_LIB_CAMERA_H
#define SHINA_LIB_CAMERA_H
#include "main.h"

class smCamera {
public:
	using vec3=glm::vec3;
	using mat4 = glm::mat4;
	
	GLfloat theta, phi;
	GLfloat factor;
public:
	vec3 eye, dir, uper;
	smCamera(vec3 & pos = vec3(0, 0, 3),vec3& dirp=vec3(0,0,-3)) :eye(pos), dir(dirp), uper(vec3(0, 1, 0)),
		theta(PI/2),phi(PI/2),factor(8.0f)
	{

	}
	inline vec3& getEye()
	{
		return eye;
	}
	inline vec3& getDir()
	{
		return dir;
	}
	mat4 getView()
	{
		return glm::lookAt(eye, eye + dir, uper);
	}
	//move camera
	void moveCamera(GLfloat deltaX, GLfloat deltaY)
	{
		eye.x -= (deltaX*sin(theta) + deltaY*cos(theta));
		eye.z -= ( -deltaX*cos(theta) + deltaY*sin(theta));
		//glm::vec2 tmp= glm::vec2(eye.x, eye.z);
		//glm::vec2 normal = glm::vec2(1, 0);
		//theta = acos(eye.x/sqrt(eye.x*eye.x+eye.z*eye.z));
	}
	//move the height of camera
	void moveHCamera(GLfloat Z) {
		eye.y += Z;
	}

	//rotate camera
	void rotateCamera(GLfloat rX, GLfloat ry)
	{
		
		vec3 look = eye + dir;//not change
		theta += rX;
		phi = inner<float>(0.01, (float)(PI - 0.01f), phi - ry);
		//phi = inner<float>(0.001f, (float)(PI / 2 - 0.001f), phi + ry);
		//change axis
		eye.x = look.x + factor*sin(phi)*cos(theta);
		eye.z = look.z + factor*sin(phi)*sin(theta);
		eye.y = look.y + factor*cos(phi);
		dir = look - eye;
	}
	//zoom
	void zoomCamera(GLfloat dx)
	{
		//set limit to zoom 
		GLfloat preValue = factor;
		factor = inner<float>(0.001, 2147483647, factor*dx);
		GLfloat c =factor/preValue;
		eye = eye + dir*(1-c);
		cout << factor << endl;
		dir = dir*c;
	}

	



};


#endif