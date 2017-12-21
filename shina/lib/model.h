#pragma once
#ifndef SHINA_LIB_MODEL_H
#define SHINA_LIB_MODEL_H

#include "main.h"
#include "shader.h"
#include "camera.h"
#include "light.h"
#include <atlimage.h>




class texture;
class object;
class scene;

class Bitmap {
public:
	int sizeX, sizeY;
	unsigned char *data;
};


class object {
private:
	using vec3 = glm::vec3;
	std::vector<GLfloat> pos, color, normals;
	std::string name;
	class Material {
	public:
		std::string name;
		glm::vec3 kd, ka, ks;
	};
	vector<Material> surface;
	int active = -1;
	glm::vec3 kd, ka;
	float ks;
	glm::mat4 model;
public:
	bool diy = false;
	GLuint vao, svao;
	GLuint vboHandles[3],
		positionBufferHandle, colorBufferHandle, normalBufferHandle;
	GLuint spositionBufferHandle;

	

	object(bool _diy = false, vec3 _ka=vec3(0,0,0), float _ks=0, vec3 _kd=vec3(0,0,0))
		:diy(_diy),ka(_ka),ks(_ks),kd(_kd)
	{
		pos.clear();
		color.clear();
		normals.clear();
		glGenVertexArrays(1, &vao);
		glGenVertexArrays(1, &svao);
		glGenBuffers(3, vboHandles);
		positionBufferHandle = vboHandles[0];
		colorBufferHandle = vboHandles[1];
		normalBufferHandle = vboHandles[2];
		glGenBuffers(1, &spositionBufferHandle);

	}
	virtual ~object()
	{
		glDeleteVertexArrays(1, &vao);
		glDeleteVertexArrays(1, &svao);
		glDeleteBuffers(3, vboHandles);
		glDeleteBuffers(1, &spositionBufferHandle);
	}
	void pushPos(float pos1 = 0, float pos2 = 0, float pos3 = 0) {
		pos.push_back(pos1);
		pos.push_back(pos2);
		pos.push_back(pos3);
	}
	void pushColor(float color1 = 1, float color2 = 1, float color3 = 1) {
		color.push_back(color1);
		color.push_back(color2);
		color.push_back(color3);
	}
	void pushNormal(float normal1, float normal2, float normal3) {
		normals.push_back(normal1);
		normals.push_back(normal2);
		normals.push_back(normal3);
	}
	float *getPos() {
		return toArray<float>(&pos);
	}
	float *getColor() {
		return toArray<float>(&color);
	}
	float *getNormal() {
		return toArray<float>(&normals);
	}
	int pointNum() {
		return pos.size() / 3;
	}
	object operator= (object e) {
		if (vao != e.vao)glDeleteVertexArrays(1, &vao);
		if (svao != e.svao)glDeleteVertexArrays(1, &svao);
		if (positionBufferHandle != e.positionBufferHandle ||
			colorBufferHandle != e.colorBufferHandle ||
			normalBufferHandle != e.normalBufferHandle)glDeleteBuffers(3, vboHandles);
		if (spositionBufferHandle != e.spositionBufferHandle)glDeleteBuffers(1, &spositionBufferHandle);

		pos = e.pos;
		color = e.color;
		normals = e.normals;
		surface = e.surface;
		active = e.active;
		kd = e.kd;
		ka = e.ka;
		ks = e.ks;
		model = e.model;
		diy = e.diy;

		vao = e.vao;
		svao = e.svao;
		vboHandles[0] = positionBufferHandle = e.positionBufferHandle;
		vboHandles[1] = colorBufferHandle = e.colorBufferHandle;
		vboHandles[2] = normalBufferHandle = e.normalBufferHandle;
		spositionBufferHandle = e.spositionBufferHandle;

		return e;
	}
	object& load(const char *filename)
	{
		std::ifstream fin;

		fin.open(filename);
		if (fin.is_open() == FALSE)return *this;//NULL;

		string op;
		float num1, num2, num3;

		vector<float>pos;
		vector<float>color;
		vector<float>normal;

		while (fin >> op) {
			if (op == "v") {
				fin >> num1 >> num2 >> num3;
				pos.push_back(num1);
				pos.push_back(num3);
				pos.push_back(num2);
			}
			else if (op == "vc") {
				fin >> num1 >> num2 >> num3;
				color.push_back(num1);
				color.push_back(num2);
				color.push_back(num3);
			}
			else if (op == "vn") {
				fin >> num1 >> num2 >> num3;
				normal.push_back(num1);
				normal.push_back(num2);
				normal.push_back(num3);
			}
			else if (op == "f") {
				string v1, v2, v3;
				int tmp1 = 0, tmp2 = 0, tmp3 = 0;

				fin >> v1 >> v2 >> v3;
				num1 = (float)atoi(v1.c_str() + tmp1);
				num2 = (float)atoi(v2.c_str() + tmp2);
				num3 = (float)atoi(v3.c_str() + tmp3);
				this->pushPos(pos[(int)num1 * 3 - 3], pos[(int)num1 * 3 - 2], pos[(int)num1 * 3 - 1]);
				this->pushPos(pos[(int)num2 * 3 - 3], pos[(int)num2 * 3 - 2], pos[(int)num2 * 3 - 1]);
				this->pushPos(pos[(int)num3 * 3 - 3], pos[(int)num3 * 3 - 2], pos[(int)num3 * 3 - 1]);

				tmp1 = v1.find_first_of('/', tmp1) + 1;
				tmp2 = v2.find_first_of('/', tmp2) + 1;
				tmp3 = v3.find_first_of('/', tmp3) + 1;
				if (tmp1 == 0 || tmp2 == 0 || tmp3 == 0) {
					if (active == -1) {
						this->pushColor(1, 1, 1);
						this->pushColor(1, 1, 1);
						this->pushColor(1, 1, 1);
					}
					else {
						this->pushColor(surface[active].kd.r, surface[active].kd.g, surface[active].kd.b);
						this->pushColor(surface[active].kd.r, surface[active].kd.g, surface[active].kd.b);
						this->pushColor(surface[active].kd.r, surface[active].kd.g, surface[active].kd.b);
					}
				}
				else {
					num1 = (float)atoi(v1.c_str() + tmp1);
					num2 = (float)atoi(v2.c_str() + tmp2);
					num3 = (float)atoi(v3.c_str() + tmp3);
					this->pushColor(color[(int)num1 * 3 - 3], color[(int)num1 * 3 - 2], color[(int)num1 * 3 - 1]);
					this->pushColor(color[(int)num2 * 3 - 3], color[(int)num2 * 3 - 2], color[(int)num2 * 3 - 1]);
					this->pushColor(color[(int)num3 * 3 - 3], color[(int)num3 * 3 - 2], color[(int)num3 * 3 - 1]);
				}

				tmp1 = v1.find_first_of('/', tmp1) + 1;
				tmp2 = v2.find_first_of('/', tmp2) + 1;
				tmp3 = v3.find_first_of('/', tmp3) + 1;
				if (tmp1 == 0 || tmp2 == 0 || tmp3 == 0) {
					int p1 = atoi(v1.c_str());
					int p2 = atoi(v2.c_str());
					int p3 = atoi(v3.c_str());
					glm::vec3 edge1(pos[p2 * 3 - 3] - pos[p1 * 3 - 3],
						pos[p2 * 3 - 2] - pos[p1 * 3 - 2],
						pos[p2 * 3 - 1] - pos[p1 * 3 - 1]);
					glm::vec3 edge2(pos[p3 * 3 - 3] - pos[p2 * 3 - 3],
						pos[p3 * 3 - 2] - pos[p2 * 3 - 2],
						pos[p3 * 3 - 1] - pos[p2 * 3 - 1]);
					glm::vec3 norm(edge2.y *edge1.z - edge1.y*edge2.z,
						edge2.z*edge1.x - edge1.z*edge2.x,
						edge2.x*edge1.y - edge1.x*edge2.y);
					this->pushNormal(norm.x, norm.y, norm.z);
					this->pushNormal(norm.x, norm.y, norm.z);
					this->pushNormal(norm.x, norm.y, norm.z);
				}
				else {
					num1 = (float)atoi(v1.c_str() + tmp1);
					num2 = (float)atoi(v2.c_str() + tmp2);
					num3 = (float)atoi(v3.c_str() + tmp3);
					this->pushNormal(normal[(int)num1 * 3 - 3], normal[(int)num1 * 3 - 2], normal[(int)num1 * 3 - 1]);
					this->pushNormal(normal[(int)num2 * 3 - 3], normal[(int)num2 * 3 - 2], normal[(int)num2 * 3 - 1]);
					this->pushNormal(normal[(int)num3 * 3 - 3], normal[(int)num3 * 3 - 2], normal[(int)num3 * 3 - 1]);
				}
			}
			else if (op == "mtllib") {
				std::ifstream min;
				string path = string(filename);
				unsigned int tmp = path.find_last_of('/');
				for (unsigned int i = path.length() - 1; i > tmp; i--)
					path.pop_back();
				fin >> op;
				path += op;

				min.open(path);
				if (min.is_open() == FALSE)continue;
				while (min >> op) {
					if (op == "newmtl") {
						surface.push_back(Material());
						min >> op;
						surface[surface.size() - 1].name = op;
					}
					else if (op == "Kd") {
						min >> num1 >> num2 >> num3;
						surface[surface.size() - 1].kd = glm::vec3(num1, num2, num3);
					}
					else if (op == "Ka") {
						min >> num1 >> num2 >> num3;
						surface[surface.size() - 1].ka = glm::vec3(num1, num2, num3);
					}
					else if (op == "Ks") {
						min >> num1 >> num2 >> num3;
						surface[surface.size() - 1].ks = glm::vec3(num1, num2, num3);
					}
					else {
						char *buf = new char[256];
						min.getline(buf, 256);
						delete buf;
					}
				}
			}
			else if (op == "usemtl") {
				fin >> op;
				active = -1;
				for (unsigned int i = 0; i < surface.size(); i++) {
					if (surface[i].name == op) {
						active = i;
						break;
					}
				}
			}
			else {
				char *buf = new char[256];
				fin.getline(buf, 256);
				delete buf;
			}
		}
		return *this;
	}

	void shadow();
	void show();
	inline void loadIdentity()
	{
		this->model = glm::mat4();
	}
	inline void translate(GLfloat x, GLfloat y, GLfloat z)
	{
		this->model = glm::translate(this->model, vec3(x, y, z));
	}
	inline void scale(GLfloat x, GLfloat y, GLfloat z)
	{
		this->model = glm::scale(this->model, vec3(x, y, z));
	}
	inline void rotate(GLfloat angel, vec3 axis)
	{
		this->model = glm::rotate(this->model, angel, axis);
	}


};

class texture {
private:
	using vec3 = glm::vec3;
	vector<float> pos;
	vector<float> coord;
	vector<float> normal;

	glm::vec3 kd, ka;
	float ks;

	glm::mat4 model;
public:
	bool diy = false;
	Bitmap src;

	GLuint vao, svao, texName;
	GLuint vboHandles[3],
		positionBufferHandle, coordBufferHandle, normalBufferHandle;
	GLuint spositionBufferHandle;

	texture(bool diy = false,
		glm::vec3 d = glm::vec3(0.0, 0.0, 0.0), glm::vec3 a = glm::vec3(0.0, 0.0, 0.0),
		float s = 0) {
		pos.clear();
		coord.clear();
		normal.clear();
		glGenVertexArrays(1, &vao);
		glGenVertexArrays(1, &svao);
		glGenBuffers(3, vboHandles);
		positionBufferHandle = vboHandles[0];
		coordBufferHandle = vboHandles[1];
		normalBufferHandle = vboHandles[2];
		glGenBuffers(1, &spositionBufferHandle);
		glGenTextures(1, &texName);

		this->diy = diy;
		kd = d;
		ka = a;
		ks = s;
	}
	void pushPos(float pos1 = 0, float pos2 = 0, float pos3 = 0) {
		pos.push_back(pos1);
		pos.push_back(pos2);
		pos.push_back(pos3);
	}
	void pushCoord(float coord1 = 0.f, float coord2 = 0.f) {
		coord.push_back(coord1);
		coord.push_back(coord2);
	}
	void pushNormal(float normal1, float normal2, float normal3) {
		normal.push_back(normal1);
		normal.push_back(normal2);
		normal.push_back(normal3);
	}
	float *getPos() {
		return toArray<float>(&pos);
	}
	float *getCoord() {
		return toArray<float>(&coord);
	}
	float *getNormal() {
		return toArray<float>(&normal);
	}

	texture& load(const char *filename);
	void shadow();
	void show();

	void pic(const char *fileName);
	
	inline void loadIdentity()
	{
		this->model = glm::mat4();
	}
	
	inline void translate(GLfloat x, GLfloat y, GLfloat z)
	{
		this->model = glm::translate(this->model, vec3(x, y, z));
	}
	inline void scale(GLfloat x, GLfloat y, GLfloat z)
	{
		this->model = glm::scale(this->model, vec3(x, y, z));
	}
	inline void rotate(GLfloat angel, vec3 axis)
	{
		this->model = glm::rotate(this->model, angel, axis);
	}
};


class scene {
private:
	using objVector = std::vector<object>;
	using textureVector = std::vector<texture>;
	
	std::string name;
	bool active;

	
	
public:
	objVector objCollection;
	textureVector texCollection;
	scene() :active(true)
	{

	}
	virtual ~scene() {}
	void push_back(object& e)
	{
		objCollection.push_back(e);
	}
	void push_back(texture& e)
	{
		texCollection.push_back(e);
	}
	void shadow();
	void show(smLight& light);

};

class world {
private:
	using sceneVector = std::vector<scene>;
	using lightVector = std::vector<smLight>;
	
	std::string name;
public:
	sceneVector sceneCollection;
	lightVector lightCollection;
	world(std::string tname):name(tname){}
	virtual ~world(){}
	void push_back(scene& e)
	{
		sceneCollection.push_back(e);
	}
	void push_back(smLight& e)
	{
		lightCollection.push_back(e);
	}
	sceneVector& getScenes()
	{
		return sceneCollection;
	}
	lightVector& getLights()
	{
		return lightCollection;
	}
};


#endif
