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
	
	vector<glm::vec3> returnPos(int k) {
		
		for (int i = 0; i < pos.size(); i++)
			posRe.push_back((glm::vec3)(pos[i] - 0));
			
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
	/*
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
	*/

	GLfloat unitize()
	{
		GLuint  i;
		GLfloat maxx, minx, maxy, miny, maxz, minz;
		GLfloat cx, cy, cz, w, h, d;
		GLfloat scale;

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
		/* calculate unitizing scale factor */
		scale = 2.0 / max;

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

		return scale;
	}
	
	object& load(const char *filename)
	{
		FILE* file = fopen(filename, "r");
		if (file == NULL)
		{
			cout << "obj name error!" << endl;
			return *this;
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
					pos.push_back(num3);
					pos.push_back(num2);
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
					cout << line << endl;
					fscanf(file, "%d/%d", &v1, &t1);
					fscanf(file, "%d/%d", &v2, &t1);
					cout << v0 << " " << v1 << " " << v2 << endl;

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
				else if(sscanf(buf, "%d", &v0) == 1)
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
		cout << line << endl;
		cout << filename << " load finished!\n";
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
