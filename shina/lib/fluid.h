#pragma once
#ifndef SHINA_LIB_FLUID_H
#define SHINA_LIB_FLUID_H
#include "VectorClasses.h"  
#include "main.h"

#include <atlimage.h>


class Fluid
{
private:

	long            width; //宽
	long            height; //高

	Vector3D        *buffer[2]; //缓冲区
	Vector3D		*normals, *tangent;
	long            renderBuffer;  //当前渲染的缓冲区

	int             *indices[3]; //索引
	float			*texcoords[3][2]; //纹理坐标
	float           k1, k2, k3; //多项式系数
	float			*coord, *pos, *normal;

	GLuint vao, svao, texName;
	GLuint vboHandles[3],
		positionBufferHandle, coordBufferHandle, normalBufferHandle;
	GLuint spositionBufferHandle;
	class Bitmap {
	public:
		int sizeX, sizeY;
		unsigned char *data;
	}src;


	inline void pic(const char *fileName) {
		CImage *img = new CImage;
		if (!fileName) {
			return;
		}
		HRESULT hr = img->Load(fileName);
		if (!SUCCEEDED(hr)) {
			return;
		}
		src.sizeX = img->GetWidth();
		src.sizeY = img->GetHeight();
		if (img->GetPitch()<0)src.data = (unsigned char *)img->GetBits() + (img->GetPitch()*(img->GetHeight() - 1));
		else src.data = (unsigned char *)img->GetBits();
	}

public:

	glm::mat4 model;

	//n:网格宽 m：网格高 d：网格点之间的距离
	//t:时间 c：波速 mu：阻力系数

	Fluid(long n, long m, float d, float t, float c, float mu, const char*name);
	~Fluid();

	void Evaluate(void);
	void show(int lights);
	void getData();
	void shadow();
	inline void update()
	{
		static int count = 0;
		count |= 1;
		if (count & 1) {
			count &= 0;
			Evaluate();
			getData();
		}

	}
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
};

#endif