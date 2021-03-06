#pragma once
#ifndef SHINA_LIB_MODEL_H
#define SHINA_LIB_MODEL_H

#include "main.h"
#include "shader.h"
#include "camera.h"
#include "light.h"
#include "sphere.h"
#include "rectangle.h"
#include <atlimage.h>
#include "fluid.h"
#include "fire.h"

class texture;
class object;
class scene;

extern float explo_x;
extern float explo_y;
extern float explo_z;

//s2 is dest, s1 is src
void copy_imag(const char* s1, const char* s2);

class Bitmap
{
public:
	string path;
	int sizeX, sizeY;
	unsigned char *data;
};


class object {
private:
	using vec3 = glm::vec3;
	std::vector<GLfloat> pos, color, normals;
	std::vector<glm::vec3> posRe;
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



	object(bool _diy = false, vec3 _ka = vec3(0, 0, 0), float _ks = 0, vec3 _kd = vec3(0, 0, 0))
		:diy(_diy), ka(_ka), ks(_ks), kd(_kd)
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

	glm::mat4 getModel() {
		return model;
	}

	vector<glm::vec3> returnPos() {

		for (int i = 0; i < pos.size(); i += 3) {
			float tem[3];
			tem[0] = pos[i];
			tem[1] = pos[i + 1];
			tem[2] = pos[i + 2];
			posRe.push_back(glm::make_vec3(tem));
		}

		return posRe;
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


	void direct_scale(GLfloat scale)
	{
		GLuint  i;
		GLfloat maxx, minx, maxy, miny, maxz, minz;
		GLfloat cx, cy, cz, w, h, d;
		//GLfloat scale;

		/* get the max/mins */
		maxx = minx = pos[0];
		maxy = miny = pos[1];
		maxz = minz = pos[2];
		for (i = 0; i < pos.size(); i += 3) {
			if (maxx < pos[i + 0])
				maxx = pos[i + 0];
			if (minx > pos[i + 0])
				minx = pos[i + 0];

			if (maxy < pos[i + 1])
				maxy = pos[i + 1];
			if (miny > pos[i + 1])
				miny = pos[i + 1];

			if (maxz < pos[i + 2])
				maxz = pos[i + 2];
			if (minz > pos[i + 2])
				minz = pos[i + 2];
		}

		/* calculate model width, height, and depth */
		w = maxx - minx;
		h = maxy - miny;
		d = maxz - minz;

		/* calculate center of the model */
		cx = (maxx + minx) / 2.0;
		cy = (maxy + miny) / 2.0;
		cz = (maxz + minz) / 2.0;

		GLfloat max;
		if (w > h)
		{
			if (w > d)
				max = w;
			else
				max = d;
		}
		else
		{
			if (h > d)
				max = h;
			else
				max = d;
		}
		cout << "translate:" << endl;
		cout << "x: " << cx << endl;
		cout << "y: " << cy << endl;
		cout << "z: " << cz << endl;
		/* translate around center then scale */
		for (i = 0; i < pos.size(); i += 3) {
			//transform
			pos[i + 0] -= cx;
			pos[i + 1] -= cy;
			pos[i + 2] -= cz;
			//scale
			pos[i + 0] *= scale;
			pos[i + 1] *= scale;
			pos[i + 2] *= scale;
		}
	}

	object* load(const char *filename)
	{
		FILE* file = fopen(filename, "r");
		if (file == NULL)
		{
			cout << "obj name error!" << endl;
			return this;
		}

		int v_num = 0;
		int vn_num = 0;
		char buf[256];
		float num1, num2, num3;

		vector<float>pos;
		vector<float>color;
		vector<float>normal;

		vector<string> texName;
		vector<string> texDir;
		int line = 0;
		while (fscanf(file, "%s", buf) != EOF)
		{
			line++;

			switch (buf[0])
			{
			case 'v':
			{
				switch (buf[1])
				{
				case '\0':
					fscanf(file, "%f %f %f", &num1, &num2, &num3);
					pos.push_back(num1);
					pos.push_back(num2);
					pos.push_back(num3);
					v_num++;
					break;
				case 'n':
					fscanf(file, "%f %f %f", &num1, &num2, &num3);
					normal.push_back(num1);
					normal.push_back(num2);
					normal.push_back(num3);
					vn_num++;
					break;
				default:
					fgets(buf, sizeof(buf), file);
					break;
				}
				break;
			}
			case 'f':
			{

				int v0, t0, n0, v1, t1, n1, v2, t2, n2;
				v0 = v1 = v2 = 0;
				n0 = n1 = n2 = 0;
				t0 = t1 = t2 = 0;
				fscanf(file, "%s", buf);

				// type: v//vn
				if (sscanf(buf, "%d//%d", &v0, &n0) == 2)
				{
					fscanf(file, "%d//%d", &v1, &n1);
					fscanf(file, "%d//%d", &v2, &n2);
					//v
					this->pushPos(pos[v0 * 3 - 3], pos[v0 * 3 - 2], pos[v0 * 3 - 1]);
					this->pushPos(pos[v1 * 3 - 3], pos[v1 * 3 - 2], pos[v1 * 3 - 1]);
					this->pushPos(pos[v2 * 3 - 3], pos[v2 * 3 - 2], pos[v2 * 3 - 1]);
					//vt
					if (active != -1)
					{
						this->pushColor(surface[active].kd.r, surface[active].kd.g, surface[active].kd.b);
						this->pushColor(surface[active].kd.r, surface[active].kd.g, surface[active].kd.b);
						this->pushColor(surface[active].kd.r, surface[active].kd.g, surface[active].kd.b);
					}
					else
					{
						this->pushColor();
						this->pushColor();
						this->pushColor();
					}
					//vn
					this->pushNormal(normal[n0 * 3 - 3], normal[n0 * 3 - 2], normal[n0 * 3 - 1]);
					this->pushNormal(normal[n1 * 3 - 3], normal[n1 * 3 - 2], normal[n1 * 3 - 1]);
					this->pushNormal(normal[n2 * 3 - 3], normal[n2 * 3 - 2], normal[n2 * 3 - 1]);
					//if the face is not a triangle, regard it as the combination of several triangles
					//assign old vertex to new and add one more vertex
					v1 = v2;
					n1 = n2;
					t1 = t2;

					while (fscanf(file, "%d//%d", &v2, &n2) == 2)
					{
						this->pushPos(pos[v0 * 3 - 3], pos[v0 * 3 - 2], pos[v0 * 3 - 1]);
						this->pushPos(pos[v1 * 3 - 3], pos[v1 * 3 - 2], pos[v1 * 3 - 1]);
						this->pushPos(pos[v2 * 3 - 3], pos[v2 * 3 - 2], pos[v2 * 3 - 1]);

						if (active != -1)
						{
							this->pushColor(surface[active].kd.r, surface[active].kd.g, surface[active].kd.b);
							this->pushColor(surface[active].kd.r, surface[active].kd.g, surface[active].kd.b);
							this->pushColor(surface[active].kd.r, surface[active].kd.g, surface[active].kd.b);
						}
						else
						{
							this->pushColor();
							this->pushColor();
							this->pushColor();
						}

						this->pushNormal(normal[n0 * 3 - 3], normal[n0 * 3 - 2], normal[n0 * 3 - 1]);
						this->pushNormal(normal[n1 * 3 - 3], normal[n1 * 3 - 2], normal[n1 * 3 - 1]);
						this->pushNormal(normal[n2 * 3 - 3], normal[n2 * 3 - 2], normal[n2 * 3 - 1]);

						v1 = v2;
						n1 = n2;
						t1 = t2;
					}
				}
				// type: v/vt/vn
				else if (sscanf(buf, "%d/%d/%d", &v0, &t0, &n0) == 3)
				{
					fscanf(file, "%d/%d/%d", &v1, &t1, &n1);
					fscanf(file, "%d/%d/%d", &v2, &t2, &n2);

					this->pushPos(pos[v0 * 3 - 3], pos[v0 * 3 - 2], pos[v0 * 3 - 1]);
					this->pushPos(pos[v1 * 3 - 3], pos[v1 * 3 - 2], pos[v1 * 3 - 1]);
					this->pushPos(pos[v2 * 3 - 3], pos[v2 * 3 - 2], pos[v2 * 3 - 1]);

					if (active != -1)
					{
						this->pushColor(surface[active].kd.r, surface[active].kd.g, surface[active].kd.b);
						this->pushColor(surface[active].kd.r, surface[active].kd.g, surface[active].kd.b);
						this->pushColor(surface[active].kd.r, surface[active].kd.g, surface[active].kd.b);
					}
					else
					{
						this->pushColor();
						this->pushColor();
						this->pushColor();
					}

					this->pushNormal(normal[n0 * 3 - 3], normal[n0 * 3 - 2], normal[n0 * 3 - 1]);
					this->pushNormal(normal[n1 * 3 - 3], normal[n1 * 3 - 2], normal[n1 * 3 - 1]);
					this->pushNormal(normal[n2 * 3 - 3], normal[n2 * 3 - 2], normal[n2 * 3 - 1]);

					//add more
					v1 = v2;
					n1 = n2;
					t1 = t2;
					while (fscanf(file, "%d/%d/%d", &v2, &t2, &n2) == 3)
					{
						this->pushPos(pos[v0 * 3 - 3], pos[v0 * 3 - 2], pos[v0 * 3 - 1]);
						this->pushPos(pos[v1 * 3 - 3], pos[v1 * 3 - 2], pos[v1 * 3 - 1]);
						this->pushPos(pos[v2 * 3 - 3], pos[v2 * 3 - 2], pos[v2 * 3 - 1]);

						if (active != -1)
						{
							this->pushColor(surface[active].kd.r, surface[active].kd.g, surface[active].kd.b);
							this->pushColor(surface[active].kd.r, surface[active].kd.g, surface[active].kd.b);
							this->pushColor(surface[active].kd.r, surface[active].kd.g, surface[active].kd.b);
						}
						else
						{
							this->pushColor();
							this->pushColor();
							this->pushColor();
						}

						this->pushNormal(normal[n0 * 3 - 3], normal[n0 * 3 - 2], normal[n0 * 3 - 1]);
						this->pushNormal(normal[n1 * 3 - 3], normal[n1 * 3 - 2], normal[n1 * 3 - 1]);
						this->pushNormal(normal[n2 * 3 - 3], normal[n2 * 3 - 2], normal[n2 * 3 - 1]);

						v1 = v2;
						n1 = n2;
						t1 = t2;
					}
				}
				// type: v/vt
				else if (sscanf(buf, "%d/%d", &v0, &t0) == 2)
				{
					//cout << line << endl;
					fscanf(file, "%d/%d", &v1, &t1);
					fscanf(file, "%d/%d", &v2, &t2);
					//cout << v0 << " " << v1 << " " << v2 << endl;

					this->pushPos(pos[v0 * 3 - 3], pos[v0 * 3 - 2], pos[v0 * 3 - 1]);
					this->pushPos(pos[v1 * 3 - 3], pos[v1 * 3 - 2], pos[v1 * 3 - 1]);
					this->pushPos(pos[v2 * 3 - 3], pos[v2 * 3 - 2], pos[v2 * 3 - 1]);

					if (active != -1)
					{
						this->pushColor(surface[active].kd.r, surface[active].kd.g, surface[active].kd.b);
						this->pushColor(surface[active].kd.r, surface[active].kd.g, surface[active].kd.b);
						this->pushColor(surface[active].kd.r, surface[active].kd.g, surface[active].kd.b);
					}
					else
					{
						this->pushColor();
						this->pushColor();
						this->pushColor();
					}

					glm::vec3 edge1(pos[v1 * 3 - 3] - pos[v0 * 3 - 3],
						pos[v1 * 3 - 2] - pos[v0 * 3 - 2],
						pos[v1 * 3 - 1] - pos[v0 * 3 - 1]);
					glm::vec3 edge2(pos[v2 * 3 - 3] - pos[v1 * 3 - 3],
						pos[v2 * 3 - 2] - pos[v1 * 3 - 2],
						pos[v2 * 3 - 1] - pos[v1 * 3 - 1]);
					glm::vec3 norm(edge2.y *edge1.z - edge1.y*edge2.z,
						edge2.z*edge1.x - edge1.z*edge2.x,
						edge2.x*edge1.y - edge1.x*edge2.y);

					this->pushNormal(norm.x, norm.y, norm.z);
					this->pushNormal(norm.x, norm.y, norm.z);
					this->pushNormal(norm.x, norm.y, norm.z);

					//add more
					v1 = v2;
					n1 = n2;
					t1 = t2;
					while (fscanf(file, "%d/%d", &v2, &t2) == 2)
					{
						this->pushPos(pos[v0 * 3 - 3], pos[v0 * 3 - 2], pos[v0 * 3 - 1]);
						this->pushPos(pos[v1 * 3 - 3], pos[v1 * 3 - 2], pos[v1 * 3 - 1]);
						this->pushPos(pos[v2 * 3 - 3], pos[v2 * 3 - 2], pos[v2 * 3 - 1]);

						if (active != -1)
						{
							this->pushColor(surface[active].kd.r, surface[active].kd.g, surface[active].kd.b);
							this->pushColor(surface[active].kd.r, surface[active].kd.g, surface[active].kd.b);
							this->pushColor(surface[active].kd.r, surface[active].kd.g, surface[active].kd.b);
						}
						else
						{
							this->pushColor();
							this->pushColor();
							this->pushColor();
						}

						glm::vec3 edge1(pos[v1 * 3 - 3] - pos[v0 * 3 - 3],
							pos[v1 * 3 - 2] - pos[v0 * 3 - 2],
							pos[v1 * 3 - 1] - pos[v0 * 3 - 1]);
						glm::vec3 edge2(pos[v2 * 3 - 3] - pos[v1 * 3 - 3],
							pos[v2 * 3 - 2] - pos[v1 * 3 - 2],
							pos[v2 * 3 - 1] - pos[v1 * 3 - 1]);
						glm::vec3 norm(edge2.y *edge1.z - edge1.y*edge2.z,
							edge2.z*edge1.x - edge1.z*edge2.x,
							edge2.x*edge1.y - edge1.x*edge2.y);

						this->pushNormal(norm.x, norm.y, norm.z);
						this->pushNormal(norm.x, norm.y, norm.z);
						this->pushNormal(norm.x, norm.y, norm.z);

						v1 = v2;
						n1 = n2;
						t1 = t2;
					}
				}
				// type: v
				else if (sscanf(buf, "%d", &v0) == 1)
				{
					fscanf(file, "%d", &v1);
					fscanf(file, "%d", &v2);

					this->pushPos(pos[v0 * 3 - 3], pos[v0 * 3 - 2], pos[v0 * 3 - 1]);
					this->pushPos(pos[v1 * 3 - 3], pos[v1 * 3 - 2], pos[v1 * 3 - 1]);
					this->pushPos(pos[v2 * 3 - 3], pos[v2 * 3 - 2], pos[v2 * 3 - 1]);

					if (active != -1)
					{
						this->pushColor(surface[active].kd.r, surface[active].kd.g, surface[active].kd.b);
						this->pushColor(surface[active].kd.r, surface[active].kd.g, surface[active].kd.b);
						this->pushColor(surface[active].kd.r, surface[active].kd.g, surface[active].kd.b);
					}
					else
					{
						this->pushColor();
						this->pushColor();
						this->pushColor();
					}

					glm::vec3 edge1(pos[v1 * 3 - 3] - pos[v0 * 3 - 3],
						pos[v1 * 3 - 2] - pos[v0 * 3 - 2],
						pos[v1 * 3 - 1] - pos[v0 * 3 - 1]);
					glm::vec3 edge2(pos[v2 * 3 - 3] - pos[v1 * 3 - 3],
						pos[v2 * 3 - 2] - pos[v1 * 3 - 2],
						pos[v2 * 3 - 1] - pos[v1 * 3 - 1]);
					glm::vec3 norm(edge2.y *edge1.z - edge1.y*edge2.z,
						edge2.z*edge1.x - edge1.z*edge2.x,
						edge2.x*edge1.y - edge1.x*edge2.y);

					this->pushNormal(norm.x, norm.y, norm.z);
					this->pushNormal(norm.x, norm.y, norm.z);
					this->pushNormal(norm.x, norm.y, norm.z);

					//add more
					v1 = v2;
					n1 = n2;
					t1 = t2;
					while (fscanf(file, "%d", &v2) > 0)
					{
						this->pushPos(pos[v0 * 3 - 3], pos[v0 * 3 - 2], pos[v0 * 3 - 1]);
						this->pushPos(pos[v1 * 3 - 3], pos[v1 * 3 - 2], pos[v1 * 3 - 1]);
						this->pushPos(pos[v2 * 3 - 3], pos[v2 * 3 - 2], pos[v2 * 3 - 1]);

						if (active != -1)
						{
							this->pushColor(surface[active].kd.r, surface[active].kd.g, surface[active].kd.b);
							this->pushColor(surface[active].kd.r, surface[active].kd.g, surface[active].kd.b);
							this->pushColor(surface[active].kd.r, surface[active].kd.g, surface[active].kd.b);
						}
						else
						{
							this->pushColor();
							this->pushColor();
							this->pushColor();
						}

						glm::vec3 edge1(pos[v1 * 3 - 3] - pos[v0 * 3 - 3],
							pos[v1 * 3 - 2] - pos[v0 * 3 - 2],
							pos[v1 * 3 - 1] - pos[v0 * 3 - 1]);
						glm::vec3 edge2(pos[v2 * 3 - 3] - pos[v1 * 3 - 3],
							pos[v2 * 3 - 2] - pos[v1 * 3 - 2],
							pos[v2 * 3 - 1] - pos[v1 * 3 - 1]);
						glm::vec3 norm(edge2.y *edge1.z - edge1.y*edge2.z,
							edge2.z*edge1.x - edge1.z*edge2.x,
							edge2.x*edge1.y - edge1.x*edge2.y);

						this->pushNormal(norm.x, norm.y, norm.z);
						this->pushNormal(norm.x, norm.y, norm.z);
						this->pushNormal(norm.x, norm.y, norm.z);

						v1 = v2;
						n1 = n2;
						t1 = t2;
					}
				}
				break;
			}

			case 'm':
			{
				std::ifstream min;
				string path = string(filename);
				unsigned int tmp = path.find_last_of('/');
				for (unsigned int i = path.length() - 1; i > tmp; i--)
					path.pop_back();
				fscanf(file, "%s", buf);
				string s(buf);
				path += buf;

				min.open(path);
				if (min.is_open() == false)continue;
				while (min >> s) {
					if (s == "newmtl") {
						surface.push_back(Material());
						min >> s;
						surface[surface.size() - 1].name = s;
					}
					else if (s == "Kd") {
						min >> num1 >> num2 >> num3;
						surface[surface.size() - 1].kd = glm::vec3(num1, num2, num3);
					}
					else if (s == "Ka") {
						min >> num1 >> num2 >> num3;
						surface[surface.size() - 1].ka = glm::vec3(num1, num2, num3);
					}
					else if (s == "Ks") {
						min >> num1 >> num2 >> num3;
						surface[surface.size() - 1].ks = glm::vec3(num1, num2, num3);
					}
					else {
						min.getline(buf, 256);
					}
				}
				break;
			}
			case 'u':
			{
				fscanf(file, "%s", buf);
				active = -1;
				string s(buf);
				for (unsigned int i = 0; i < surface.size(); i++) {
					if (surface[i].name == s) {
						active = i;
						break;
					}
				}
				break;
			}
			default:
			{
				fgets(buf, sizeof(buf), file);
				break;
			}
			}
		}
		cout << filename << " load finished!\n";
		return this;
	}

	void shadow();
	void show();
	inline void loadIdentity()
	{
		this->model = glm::mat4();
	}
	inline object* translate(GLfloat x, GLfloat y, GLfloat z)
	{
		this->model = glm::translate(this->model, vec3(x, y, z));
		return this;
	}
	inline object* scale(GLfloat x, GLfloat y, GLfloat z)
	{
		this->model = glm::scale(this->model, vec3(x, y, z));
		return this;
	}
	inline object* rotate(GLfloat angel, vec3 axis)
	{
		this->model = glm::rotate(this->model, angel, axis);
		return this;
	}


};

class texture {
public:
	using vec3 = glm::vec3;
	bool hide = false;
	//增加了一个材质类，有材质信息和图片信恿
	class Material {
	public:
		GLuint texName;
		std::string name;//材质名字
		glm::vec3 kd, ka, ks;//材质信息
		Bitmap src;//图片信息
		bool is_src;//是否有图片，有则使用图片画，否则用材质画
		bool diy = false;

		GLuint vao, svao;
		GLuint vboHandles[4],
			positionBufferHandle, colorBufferHandle, normalBufferHandle;
		GLuint spositionBufferHandle;

		GLuint coordBufferHandle;

		Material()
		{
			this->diy = false;
			glGenVertexArrays(1, &vao);
			glGenVertexArrays(1, &svao);
			glGenBuffers(4, vboHandles);
			positionBufferHandle = vboHandles[0];
			colorBufferHandle = vboHandles[3];
			coordBufferHandle = vboHandles[1];
			normalBufferHandle = vboHandles[2];
			glGenBuffers(1, &spositionBufferHandle);
		}
		~Material()
		{
			glDeleteVertexArrays(1, &vao);
			glDeleteVertexArrays(1, &svao);
			glDeleteBuffers(4, vboHandles);

		}

		//这个函数从texture类放到material类里靿
		bool pic(const char *fileName)
		{
			src.path = fileName;
			// load picture to bitmap
			CImage *img = new CImage;
			if (!fileName)
			{
				cout << "Warning: image open failed!\nMaterial name is " << name << endl;
				return false;
			}
			HRESULT hr = img->Load(fileName);
			if (!SUCCEEDED(hr))
			{
				cout << "Warning: load image failed!\nMaterial name is " << name << endl;
				return false;
			}
			cout << "ffff: " << fileName << endl;
			src.sizeX = img->GetWidth();
			src.sizeY = img->GetHeight();
			if (img->GetPitch() < 0)
			{
				src.data = (unsigned char *)img->GetBits() + (img->GetPitch()*(img->GetHeight() - 1));
			}
			else
			{
				src.data = (unsigned char *)img->GetBits();
			}
			return true;
		}
	};
	class Group {
	public:
		vector<float> pos;
		vector<float> coord;
		vector<float> color;
		vector<float> normal;
		Material material;
		bool colorFilled = false;
		void write(const char* name)
		{
			char* obj_name = new char[strlen(name) + 5];
			char* mtl_name = new char[strlen(name) + 5];

			strcpy(obj_name, name);
			strcpy(mtl_name, name);

			strcat(obj_name, ".obj");
			strcat(mtl_name, ".mtl");

			std::ofstream obj(obj_name);
			std::ofstream mtl(mtl_name);
			//obj
			obj << "mtllib " << mtl_name << endl;
			for (int i = 0; i < pos.size() / 3; i++)
			{
				obj << "v " << pos[3 * i] << " " << pos[3 * i + 1] << " " << pos[3 * i + 2] << endl;
				obj << "vt " << coord[2 * i] << " " << coord[2 * i + 1] << endl;
				obj << "vn " << normal[3 * i] << " " << normal[3 * i + 1] << " " << normal[3 * i + 2] << endl;
			}
			obj << "usemtl Default" << endl;
			for (int i = 1; i < pos.size() / 3 + 1; i += 3)
			{
				obj << "f " << i << "/" << i << "/" << i << " " << i + 1 << "/" << i + 1 << "/" << i + 1 << " " << i + 2 << "/" << i + 2 << "/" << i + 2 << endl;
			}

			obj.flush();
			//mtl
			mtl << "newmtl Default" << endl;
			mtl << "Ka " << material.ka.r << " " << material.ka.g << " " << material.ka.b << endl;
			mtl << "Kd " << material.kd.r << " " << material.kd.g << " " << material.kd.b << endl;
			mtl << "Ks " << material.ks.r << " " << material.ks.g << " " << material.ks.b << endl;
			if (material.is_src)
			{
				int index = material.src.path.find_last_of(".");
				string type = material.src.path.substr(index);
				string image_name = name + type;
				cout << image_name << endl;

				copy_imag(material.src.path.c_str(), image_name.c_str());
				mtl << "map_Kd " << image_name << endl;
			}
			mtl.flush();
			mtl.close();
			obj.close();
			cout << "write " << name << " finished!\n";
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
		float *getColor() {
			return toArray<float>(&color);
		}
		void fillColor()
		{
			if (!colorFilled)
			{
				for (int i = 0; i < pos.size(); i++)
				{
					if (i % 3 == 0)
					{
						color.push_back(material.kd.r);
					}
					else if (i % 3 == 1)
					{
						color.push_back(material.kd.g);
					}
					else {
						color.push_back(material.kd.b);
					}
				}
				colorFilled = true;
			}
		}
	};

	vector<Group> group;
	glm::mat4 model;


	bool diy = false;
	bool fake = false;

	glm::mat4 getModel() {
		return model;
	}


	texture(bool diy = false,
		glm::vec3 d = glm::vec3(0.0, 0.0, 0.0), glm::vec3 a = glm::vec3(0.0, 0.0, 0.0),
		float s = 0) {

		this->diy = diy;
	}

	texture* load(const char *filename);
	void shadow();
	void show(int lights);
	void direct_scale(float scale);
	void direct_translate(float x, float y, float z);
	void write(const char* name)
	{
		for (int i = 0; i < group.size(); i++)
		{
			string tmp = name;
			tmp += "_";
			std::stringstream newstr;
			newstr << i;
			tmp += newstr.str();
			group[i].write(tmp.c_str());
		}
	}
	void print()
	{
		for (size_t i = 0; i < group.size(); i++)
		{
			cout << "group: " << i << endl;
			cout << "v num: " << group[i].pos.size() / 3 << endl;
			cout << "vt num: " << group[i].coord.size() / 2 << endl;
			cout << "vn num: " << group[i].normal.size() / 3 << endl;
			cout << "material name: " << group[i].material.name << endl;
		}
	}
	inline void loadIdentity()
	{
		this->model = glm::mat4();
	}
	inline texture* setFake(bool fake)
	{
		this->fake = fake;
		return this;
	}

	inline texture* translate(GLfloat x, GLfloat y, GLfloat z)
	{
		this->model = glm::translate(this->model, vec3(x, y, z));
		return this;
	}
	inline texture* scale(GLfloat x, GLfloat y, GLfloat z)
	{
		this->model = glm::scale(this->model, vec3(x, y, z));
		return this;
	}
	inline texture* rotate(GLfloat angel, vec3 axis)
	{
		this->model = glm::rotate(this->model, angel, axis);
		return this;
	}

};


/*
0 sphere
1 texture
*/

struct particle {

	texture*tex = nullptr;
	rectangle*rec = nullptr;
	glm::vec3 initialColor;
	glm::vec3 fadeColor;

	bool fade = false;
	int draw_time;
	int which;
	float fullLife;
	float x, y, z, vx, vy, vz, ax, ay, az, sizei, lifetime, deci;
	float rx, ry, rz, wx, wy, wz, bx, by, bz;//rΪѽתއ׈,wۍbؖҰΪއ̙׈ۍއݓ̙׈
	bool isDead;
	float xScale, yScale, zScale, xFactor, yFactor, zFactor;

	particle() :x(0), y(0), z(0), vx(0), vy(0), vz(0), ax(0), ay(0), az(0), sizei(0), lifetime(0), deci(0), rx(0), ry(0), rz(0), wx(0), wy(0), wz(0), bx(0), by(0), bz(0), isDead(false), which(-1), tex(nullptr), xScale(1), yScale(1), zScale(1), xFactor(1), yFactor(1), zFactor(1), draw_time(0)
	{
		initialColor = glm::vec3(1.f, 1.f, 1.f);
		fadeColor = glm::vec3(1.f, 1.f, 1.f);
	}

	inline void setCoor(float x, float y, float z)
	{
		this->x = x; this->y = y; this->z = z;
	}
	inline void setPosition(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
	inline void setAccerator(float ax, float ay, float az)
	{
		this->ax = ax;
		this->ay = ay;
		this->az = az;
	}
	inline void setSpeed(float vx, float vy, float vz)
	{
		this->vx = vx;
		this->vy = vy;
		this->vz = vz;
	}
	inline void setAngle(float rx, float ry, float rz)
	{
		this->rx = rx;
		this->ry = ry;
		this->rz = rz;
	}
	inline void setW(float wx, float wy, float wz)
	{
		this->wx = wx;
		this->wy = wy;
		this->wz = wz;
	}
	inline void setB(float bx, float by, float bz)
	{
		this->bx = bx;
		this->by = by;
		this->bz = bz;
	}
	inline void setScale(float xScale, float yScale, float zScale)
	{
		this->xScale = xScale;
		this->yScale = yScale;
		this->zScale = zScale;
	}
	inline void setFactor(float xFactor, float yFactor, float zFactor)
	{
		this->xFactor = xFactor;
		this->yFactor = yFactor;
		this->zFactor = zFactor;
	}
	inline void update()
	{
		x += vx * deci;
		y += vy * deci;
		z += vz * deci;
		vx += ax * deci;
		vy += ay * deci;
		vz += az * deci;
		rx += wx * deci;
		ry += wy * deci;
		rz += wz * deci;

		rx += wx * deci;
		ry += wy * deci;
		rz += wz * deci;
		wx += bx * deci;
		wy += by * deci;
		wz += bz * deci;

		xScale *= (xFactor);
		yScale *= (yFactor);
		zScale *= (zFactor);

		switch (which)
		{
		case IS_TEXTURE:
		{
			assert(tex != nullptr);
			break;
		}
		case IS_RECTANGLE:
		{
			assert(rec != nullptr);
			rec->color.r = interpolate<float>(lifetime / fullLife, initialColor.r, fadeColor.r);
			rec->color.g = interpolate<float>(lifetime / fullLife, initialColor.g, fadeColor.g);
			rec->color.b = interpolate<float>(lifetime / fullLife, initialColor.b, fadeColor.b);

			if (fade)
			{
				rec->color.a -= 0.5*deci;
			}
			break;
		}
		default:
		{
			assert(0);
		}

		}



		lifetime -= deci;
		if (lifetime < 0)
		{
			isDead = true;
		}
	}

	inline void init()
	{
		switch (which)
		{
		case IS_TEXTURE:
		{
			assert(tex != nullptr);
			break;
		}
		case IS_RECTANGLE:
		{
			assert(rec != nullptr);
			break;
		}
		default:
		{
			assert(0);
		}

		}
	}

	inline void show(int lights)
	{
		if (!isDead)
		{
			switch (which)
			{
			case IS_TEXTURE:
			{
				assert(tex != nullptr);
				tex->loadIdentity();
				tex->translate(x, y, z);
				tex->rotate(rx, glm::vec3(1, 0, 0));
				tex->rotate(ry, glm::vec3(0, 1, 0));
				tex->rotate(rz, glm::vec3(0, 0, 1));
				tex->scale(xScale, yScale, zScale);
				tex->show(lights);
				break;
			}
			case IS_RECTANGLE:
			{
				assert(rec != nullptr);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE);
				rec->loadIdentity();
				rec->translate(x, y, z);
				rec->rotate(rx, glm::vec3(1, 0, 0));
				rec->rotate(ry, glm::vec3(0, 1, 0));
				rec->rotate(rz, glm::vec3(0, 0, 1));

				rec->scale(xScale, yScale, zScale);

				rec->show();
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glDisable(GL_BLEND);
				break;
			}
			default:
			{
				assert(0);
			}

			}

		}


	}

};

class cloud
{
private:


public:
	int maxSize = -1;
	int time = MAXINT;
	const char* imag = "source//file.jpg";
	//void(*initParticle)(particle*) = nullptr;
	//void(*initParticle)(particle* m, const char* name) = nullptr;
	bool(*isDead)(particle*) = nullptr;
	void initParticle(particle *m, const char* name)
	{

		if (m->draw_time == 1)
		{
			return;
		}
		m->tex->hide = false;
		m->draw_time++;
		int random = rand() % 10000;
		m->isDead = false;
		float aspeed[3] = { 0,-30,0 };
		float x = explo_x;
		float y = explo_y;
		float z = explo_z;
		m->setPosition(x, y, z);
		//m->setPosition(0,200,-400);
		cout << "explox" << explo_x << endl;
		cout << "exploy" << explo_y << endl;
		cout << "exploz" << explo_z << endl;
		m->setSpeed(1.f * rand() / RAND_MAX * 200 - 100, -1.f * rand() / RAND_MAX * 100, -1.f * rand() / RAND_MAX * 150);
		m->setAccerator(aspeed[0], aspeed[1], aspeed[2]);
		//m->setAngle();
		m->lifetime = random * 2;
		m->fullLife = m->lifetime;
		m->deci = 0.05;
	}

	std::vector<particle*> particleCollection;

	cloud()
	{

	}
	~cloud() {}
	inline cloud* push_back(particle * e)
	{
		particleCollection.push_back(e);
		return this;
	}



	void show(int lights)
	{

		time--;
		for (auto & i : particleCollection)
		{

			i->update();
			if (i->isDead && time > 0)
			{
				initParticle(i, imag);
			}
			else if (isDead != nullptr)
			{
				if (isDead(i))
				{
					initParticle(i, imag);
				}

			}

			i->show(lights);
		}
	}

	void tex2cloud(texture *tex)
	{
		int j = 0;
		for (int i = 0; i < tex->group.size(); i++, j++)
		{
			/*
			if (tex->group[i].material.name == "_0131_Silver_1")
			{
			j--;
			continue;
			}
			*/

			particleCollection.push_back(new particle());
			particleCollection[j]->which = IS_TEXTURE;
			particleCollection[j]->tex = new texture();
			particleCollection[j]->tex->group.push_back(tex->group[i]);
			particleCollection[j]->isDead = false;
			particleCollection[j]->tex->hide = false;

		}
		//time = 30;
		maxSize = j;
		cout << "texture to cloud finished!\n";
	}

	void hit_happen()
	{
		for (int i = 0; i < maxSize; i++)
		{
			particleCollection[i]->isDead = true;
			particleCollection[i]->tex->hide = false;
		}
	}

	void init()
	{
		assert(initParticle != nullptr);
		assert(maxSize > 0);
		for (int i = 0; i < maxSize; i++)
		{
			particle* newParticle = new particle();
			initParticle(newParticle, imag);
			newParticle->init();
			particleCollection.push_back(newParticle);
		}
	}


};

class cubeBox {
private:



	GLuint textureID;
	class Bitmap {
	public:
		int sizeX, sizeY;
		unsigned char *data;
	}src[6];

	void pic(const char *fileName, int i) {
		CImage *img = new CImage;
		if (!fileName) {
			return;
		}
		HRESULT hr = img->Load(fileName);
		if (!SUCCEEDED(hr)) {
			return;
		}
		cout << fileName << endl;
		src[i].sizeX = img->GetWidth();
		src[i].sizeY = img->GetHeight();
		if (img->GetPitch()<0)src[i].data = (unsigned char *)img->GetBits() + (img->GetPitch()*(img->GetHeight() - 1));
		else src[i].data = (unsigned char *)img->GetBits();
	}

	texture* box[6];
	glm::mat4 view;


public:
	cubeBox() {}
	void init(vector<string>&faces);
	void show(int lights);
};


class scene {
private:
	using objPtrVector = std::vector<object*>;
	using texturePtrVector = std::vector<texture*>;
	using cloudPtrVector = std::vector<cloud*>;
	using fluidPtrVector = std::vector<Fluid*>;
	std::string name;
	bool active;



public:
	cubeBox * skyBox = nullptr;
	objPtrVector objCollection;
	texturePtrVector texCollection;
	cloudPtrVector cloudCollection;
	fluidPtrVector fluidCollection;
	vector<fire*>fireCollection;


	scene() :active(true)
	{

	}
	virtual ~scene() {}
	inline scene* push_back(object* e)
	{
		objCollection.push_back(e);
		return this;
	}
	inline scene* push_back(texture* e)
	{
		texCollection.push_back(e);
		return this;
	}
	inline scene* push_back(cloud* e)
	{
		cloudCollection.push_back(e);
		return this;
	}
	inline scene* push_back(Fluid*e)
	{
		fluidCollection.push_back(e);
		return this;
	}
	inline scene* push_back(cubeBox*e)
	{
		skyBox = e;
		return this;
	}
	inline scene*push_back(fire*e)
	{
		fireCollection.push_back(e);
		return this;
	}
	void shadow();
	void show(smLight* light, int lights);

};



class world {
private:
	using scenePtrVector = std::vector<scene*>;
	using lightPtrVector = std::vector<smLight*>;

	std::string name;
public:
	scenePtrVector sceneCollection;
	lightPtrVector lightCollection;

	world(std::string tname) :name(tname) {}
	virtual ~world() {}
	void push_back(scene* e)
	{
		sceneCollection.push_back(e);
	}
	void push_back(smLight* e)
	{
		lightCollection.push_back(e);
	}
	scenePtrVector& getScenes()
	{
		return sceneCollection;
	}
	lightPtrVector& getLights()
	{
		return lightCollection;
	}
	void popLight()
	{
		lightCollection.pop_back();
	}
};


class boundBox {
	using vec3 = glm::vec3;
public:

	class Material {
	public:
		std::string name;//材质名字
		glm::vec3 kd, ka, ks;//材质信息
		Bitmap src;
		bool is_src;


		Material()
		{

		}
		~Material()
		{

		}

	};
	class Group {
	public:
		vector<float> pos;
		vector<float> coord;
		vector<float> color;
		vector<float> normal;
		Material material;
		string group_name;
		bool colorFilled = false;
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
	};
	vector<Material>mtl;
	vector<Group> group;
	glm::mat4 model;

	glm::mat4 getModel() {
		return model;
	}




	boundBox* load(const char *filename);
	void print()
	{
		for (size_t i = 0; i < group.size(); i++)
		{
			cout << "group: " << i << endl;
			cout << "v num: " << group[i].pos.size() / 3 << endl;
			cout << "vt num: " << group[i].coord.size() / 2 << endl;
			cout << "vn num: " << group[i].normal.size() / 3 << endl;
			cout << "material name: " << group[i].material.name << endl;
		}
	}

	boundBox() {}
};

class textureGroup {
private:
	using vec3 = glm::vec3;
	//增加了一个材质类，有材质信息和图片信恿
	class Material {
	public:
		GLuint texName;
		std::string name;//材质名字
		glm::vec3 kd, ka, ks;//材质信息
		Bitmap src;//图片信息
		bool is_src;//是否有图片，有则使用图片画，否则用材质画
		bool diy = false;

		GLuint vao, svao;
		GLuint vboHandles[4],
			positionBufferHandle, colorBufferHandle, normalBufferHandle;
		GLuint spositionBufferHandle;

		GLuint coordBufferHandle;

		Material()
		{
			this->diy = false;
			glGenVertexArrays(1, &vao);
			glGenVertexArrays(1, &svao);
			glGenBuffers(4, vboHandles);
			positionBufferHandle = vboHandles[0];
			colorBufferHandle = vboHandles[3];
			coordBufferHandle = vboHandles[1];
			normalBufferHandle = vboHandles[2];
			glGenBuffers(1, &spositionBufferHandle);
		}
		~Material()
		{
			glDeleteVertexArrays(1, &vao);
			glDeleteVertexArrays(1, &svao);
			glDeleteBuffers(4, vboHandles);

		}

		//这个函数从textureGroup类放到material类里靿
		bool pic(const char *fileName)
		{
			// load picture to bitmap
			CImage *img = new CImage;
			if (!fileName)
			{
				cout << "Warning: image open failed!\nMaterial name is " << name << endl;
				return false;
			}
			HRESULT hr = img->Load(fileName);
			if (!SUCCEEDED(hr))
			{
				cout << "Warning: load image failed!\nMaterial name is " << name << endl;
				return false;
			}
			cout << fileName << endl;
			src.sizeX = img->GetWidth();
			src.sizeY = img->GetHeight();
			if (img->GetPitch() < 0)
			{
				src.data = (unsigned char *)img->GetBits() + (img->GetPitch()*(img->GetHeight() - 1));
			}
			else
			{
				src.data = (unsigned char *)img->GetBits();
			}
			return true;
		}
	};
	class Group {
	public:
		vector<float> pos;
		vector<float> coord;
		vector<float> color;
		vector<float> normal;
		Material material;
		bool colorFilled = false;
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
		float *getColor() {
			return toArray<float>(&color);
		}
		void fillColor()
		{
			if (!colorFilled)
			{
				for (int i = 0; i < pos.size(); i++)
				{
					if (i % 3 == 0)
					{
						color.push_back(material.kd.r);
					}
					else if (i % 3 == 1)
					{
						color.push_back(material.kd.g);
					}
					else {
						color.push_back(material.kd.b);
					}
				}
				colorFilled = true;
			}
		}
	};
	vector<Group> group;
	glm::mat4 model;

public:
	bool diy = false;
	bool fake = false;
	bool use = true;

	glm::mat4 getModel() {
		return model;
	}


	textureGroup(bool diy = false,
		glm::vec3 d = glm::vec3(0.0, 0.0, 0.0), glm::vec3 a = glm::vec3(0.0, 0.0, 0.0),
		float s = 0) {

		this->diy = diy;
	}

	textureGroup* load(const char *filename);
	void shadow();
	void show(int lights);
	void print()
	{
		for (size_t i = 0; i < group.size(); i++)
		{
			cout << "group: " << i << endl;
			cout << "v num: " << group[i].pos.size() / 3 << endl;
			cout << "vt num: " << group[i].coord.size() / 2 << endl;
			cout << "vn num: " << group[i].normal.size() / 3 << endl;
			cout << "material name: " << group[i].material.name << endl;
		}
	}
	inline void loadIdentity()
	{
		this->model = glm::mat4();
	}
	inline textureGroup* setFake(bool fake)
	{
		this->fake = fake;
		return this;
	}

	inline textureGroup* translate(GLfloat x, GLfloat y, GLfloat z)
	{
		this->model = glm::translate(this->model, vec3(x, y, z));
		return this;
	}
	inline textureGroup* scale(GLfloat x, GLfloat y, GLfloat z)
	{
		this->model = glm::scale(this->model, vec3(x, y, z));
		return this;
	}
	inline textureGroup* rotate(GLfloat angel, vec3 axis)
	{
		this->model = glm::rotate(this->model, angel, axis);
		return this;
	}

};



#endif