#pragma once
#ifndef SHINA_LIB_FIRE_H
#define SHINA_LIB_FIRE_H
#include "main.h"
#include <atlimage.h>
extern glm::mat4 *daoDanModel;
extern GLfloat randNumber[10000];
class fire {
	class Bitmap {
	public:
		int sizeX, sizeY;
		unsigned char *data;
	}src;
	void pic(const char *fileName) {
		CImage *img = new CImage;
		if (!fileName) {
			return;
		}
		HRESULT hr = img->Load(fileName);
		cout << fileName << endl;
		if (!SUCCEEDED(hr)) {
			return;
		}
		cout << "fuckyou" << endl;
		src.sizeX = img->GetWidth();
		src.sizeY = img->GetHeight();
		if (img->GetPitch()<0)src.data = (unsigned char *)img->GetBits() + (img->GetPitch()*(img->GetHeight() - 1));
		else src.data = (unsigned char *)img->GetBits();
	}


	enum{MAX_CNT=100000};
	struct rec {
		GLfloat vx, vy, vz;
		GLfloat fullLife;
		GLfloat lifeTime;
		GLfloat decrease;
	};
	GLfloat *pos = nullptr;
	rec*particles = nullptr;
	GLfloat*fireX, *fireY, *fireZ;
	int cnt;
	GLuint vao, vbo, texName;

	void initFireParticle(int index)
	{
		//cout << "iiii" << endl;
		int random = rand() % (5000);
		glm::vec4 firePosition = (*daoDanModel)*glm::vec4(*fireX + randNumber[random] * 200 - 100, *fireY + randNumber[(random * 2) % 10000] * 200 - 100, *fireZ, 1.f);
		glm::vec4 fireVec = (*daoDanModel)*flyVec;
		glm::vec4 yuan = (*daoDanModel)*glm::vec4(0, 0, 0, 1);
		fireVec = yuan - fireVec;
		fireVec /= sqrt(fireVec.x*fireVec.x + fireVec.y*fireVec.y + fireVec.z*fireVec.z);
		fireVec *= (randNumber[(random * 4) % 10000] * 20);

		pos[index * 7] =  firePosition.x;
		pos[index * 7 + 1] =  firePosition.y;
		pos[index * 7 + 2] =  firePosition.z;
		pos[index * 7 + 3] =  initialColor.r;
		pos[index * 7 + 4] =  initialColor.g;
		pos[index * 7 + 5] =  initialColor.b;
		pos[index * 7 + 6] = 1;

		particles[index].lifeTime =  particles[index].fullLife = randNumber[(random * 8) % 10000] * 3;
		particles[index].decrease = 0.1;
		particles[index].vx = fireVec.x;
		particles[index].vy = fireVec.y;
		particles[index].vz = fireVec.z;

	}
	void update()
	{
		for (int i = 0; i < cnt; i++)
		{
			pos[i * 7] += particles[i].vx;
			pos[i * 7 + 1] += particles[i].vy;
			pos[i * 7 + 2] += particles[i].vz;
			pos[i * 7 + 3] = interpolate<float>(particles[i].lifeTime / particles[i].fullLife, initialColor.r, fadeColor.r);
			pos[i * 7 + 4] = interpolate<float>(particles[i].lifeTime / particles[i].fullLife, initialColor.g, fadeColor.g);
			pos[i * 7 + 5] = interpolate<float>(particles[i].lifeTime / particles[i].fullLife, initialColor.b, fadeColor.b);
			pos[i * 7 + 6] -= particles[i].decrease;
			particles[i].lifeTime -= particles[i].decrease;
			if (particles[i].lifeTime < 0||pos[i*7+6]<=0)
			{
				initFireParticle(i);
			}
		}
	}
	

public:
	glm::vec4 flyVec;
	glm::vec3 initialColor;
	glm::vec3 fadeColor;

	fire(int cnt,glm::mat4 &model)
	{
		assert(cnt > 0 && cnt < MAX_CNT);
		this->cnt = cnt;
		pos = new GLfloat[7 * (cnt)];
		particles = new rec[cnt];
	}
	void useFade(glm::vec3 initialColor, glm::vec3 fadeColor)
	{
		this->initialColor = initialColor;
		this->fadeColor = fadeColor;
	}
	void initFire(string fireJPG,const float*fireX,const float *fireY,const float *fireZ)
	{
		this->fireX =(GLfloat*) fireX;
		this->fireY = (GLfloat*)fireY;
		this->fireZ = (GLfloat*)fireZ;
		for (int i = 0; i < cnt; i++)
		{
			initFireParticle(i);
		}
		pic(fireJPG.c_str());
		glGenBuffers(1, &vbo);
		glGenVertexArrays(1, &vao);
		glGenTextures(1, &texName);
	}
	void show();
	
	/*void initRandom()
	{
		srand((unsigned int)time(NULL));
		for (int i = 0; i < MAX_CNT; i++)
		{
			randNumber[i] = rand() / (double(RAND_MAX));
		}
	}*/

};


#endif
