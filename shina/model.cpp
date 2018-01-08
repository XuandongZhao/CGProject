#include "lib\model.h"


extern smShader *elementShader;
extern smShader *texShader;
extern smShader *shadowShader;
extern world myworld;

void texture::shadow() {
	//if (pos.size() == 0)return;
	if (fake)
	{
		return;
	}

	shadowShader->use();
	shadowShader->setMat4("u_modelMatrix", model);
	for (Group &graph : group)
	{
		if (graph.material.name == "_4")
		{
			continue;
		}
		glBindVertexArray(graph.material.svao);
		glBindBuffer(GL_ARRAY_BUFFER, graph.material.spositionBufferHandle);
		glBufferData(GL_ARRAY_BUFFER, graph.pos.size() * 4, graph.getPos(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
		glBindVertexArray(0);

		glBindVertexArray(graph.material.svao);
		glDrawArrays(GL_TRIANGLES, 0, graph.pos.size() / 3);
	}


}


void texture::show(int lights) {

	for (Group &graph : group)
	{
		if (graph.material.name == "_4")
		{
			continue;
		}
		if (graph.pos.size() == 0)	continue;

		if (graph.material.is_src)
		{
			texShader->use();
			glBindVertexArray(graph.material.vao);
			glBindBuffer(GL_ARRAY_BUFFER, graph.material.positionBufferHandle);
			glBufferData(GL_ARRAY_BUFFER, graph.pos.size() * 4, graph.getPos(), GL_STATIC_DRAW);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
			glBindBuffer(GL_ARRAY_BUFFER, graph.material.coordBufferHandle);
			glBufferData(GL_ARRAY_BUFFER, graph.coord.size() * 4, graph.getCoord(), GL_STATIC_DRAW);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), NULL);
			glBindBuffer(GL_ARRAY_BUFFER, graph.material.normalBufferHandle);
			glBufferData(GL_ARRAY_BUFFER, graph.normal.size() * 4, graph.getNormal(), GL_STATIC_DRAW);
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
			glBindVertexArray(0);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, graph.material.texName);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, graph.material.src.sizeX, graph.material.src.sizeY, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, graph.material.src.data);
			texShader->setInt("u_enLight", !fake);
			texShader->setMat4("u_modelMatrix", model);
			glm::mat4x4 inv = glm::transpose(glm::inverse(model));
			elementShader->setMat4("u_normalMatrix", inv);
			for (int i = 0; i < lights; i++) {
				char tmp[64];
				sprintf(tmp, "u_shadowMap[%d]", i);
				texShader->setInt(tmp, i + 1);
			}
			texShader->setInt("u_textureMap", 0);
			glBindVertexArray(graph.material.vao);
			glDrawArrays(GL_TRIANGLES, 0, graph.pos.size() / 3);
		}
		else {
			if (graph.pos.size() == 0)continue;

			elementShader->use();
			glBindVertexArray(graph.material.vao);
			glBindBuffer(GL_ARRAY_BUFFER, graph.material.positionBufferHandle);
			glBufferData(GL_ARRAY_BUFFER, graph.pos.size() * 4, graph.getPos(), GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
			graph.fillColor();

			glBindBuffer(GL_ARRAY_BUFFER, graph.material.colorBufferHandle);
			glBufferData(GL_ARRAY_BUFFER, graph.color.size() * 4, graph.getColor(), GL_STATIC_DRAW);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
			glBindBuffer(GL_ARRAY_BUFFER, graph.material.normalBufferHandle);
			glBufferData(GL_ARRAY_BUFFER, graph.normal.size() * 4, graph.getNormal(), GL_STATIC_DRAW);
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
			glBindVertexArray(0);

			elementShader->setInt("u_enLight", !fake);
			elementShader->setMat4("u_modelMatrix", model);
			glm::mat4x4 inv = glm::transpose(glm::inverse(model));
			elementShader->setMat4("u_normalMatrix", inv);
			for (int i = 0; i < lights; i++) {
				char tmp[64];
				sprintf(tmp, "u_shadowMap[%d]", i);
				elementShader->setInt(tmp, i + 1);
			}
			glBindVertexArray(graph.material.vao);
			glDrawArrays(GL_TRIANGLES, 0, graph.pos.size() / 3);


		}
	}


}
texture* texture::load(const char*filename)
{
	FILE* file = fopen(filename, "r");
	if (file == NULL)
	{
		cout << "obj name error!" << endl;
		return this;
	}
	int active = 0;
	int v_num = 0;
	int vn_num = 0;
	int vt_num = 0;
	char buf[256];
	float num1, num2, num3;

	vector<float>pos;
	vector<float>coord;
	vector<float>normal;

	int line = 0;
	while (fscanf(file, "%s", buf) != EOF)
	{
		line++;
		switch (buf[0])
		{
		case 'v':
		{
			//cout << "This is v and line is " << line << endl;
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
			case 't':
				num3 = 0;
				fscanf(file, "%f %f %f", &num1, &num2, &num3);
				coord.push_back(num1);
				coord.push_back(num2);
				coord.push_back(num3);
				vt_num++;
				break;
			default:
				fgets(buf, sizeof(buf), file);
				break;
			}
			break;
		}
		case 'f':
		{
			//cout << "This is f and line is " << line << endl;
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
				this->group[active].pushPos(pos[v0 * 3 - 3], pos[v0 * 3 - 2], pos[v0 * 3 - 1]);
				this->group[active].pushPos(pos[v1 * 3 - 3], pos[v1 * 3 - 2], pos[v1 * 3 - 1]);
				this->group[active].pushPos(pos[v2 * 3 - 3], pos[v2 * 3 - 2], pos[v2 * 3 - 1]);

				//vt
				this->group[active].pushCoord();
				this->group[active].pushCoord();
				this->group[active].pushCoord();

				//vn
				this->group[active].pushNormal(normal[n0 * 3 - 3], normal[n0 * 3 - 2], normal[n0 * 3 - 1]);
				this->group[active].pushNormal(normal[n1 * 3 - 3], normal[n1 * 3 - 2], normal[n1 * 3 - 1]);
				this->group[active].pushNormal(normal[n2 * 3 - 3], normal[n2 * 3 - 2], normal[n2 * 3 - 1]);

				//if the face is not a triangle, regard it as the combination of several triangles
				//assign old vertex to new and add one more vertex
				v1 = v2;
				n1 = n2;
				t1 = t2;

				while (fscanf(file, "%d//%d", &v2, &n2) == 2)
				{
					//v
					this->group[active].pushPos(pos[v0 * 3 - 3], pos[v0 * 3 - 2], pos[v0 * 3 - 1]);
					this->group[active].pushPos(pos[v1 * 3 - 3], pos[v1 * 3 - 2], pos[v1 * 3 - 1]);
					this->group[active].pushPos(pos[v2 * 3 - 3], pos[v2 * 3 - 2], pos[v2 * 3 - 1]);

					//vt
					this->group[active].pushCoord();
					this->group[active].pushCoord();
					this->group[active].pushCoord();
					//vn
					this->group[active].pushNormal(normal[n0 * 3 - 3], normal[n0 * 3 - 2], normal[n0 * 3 - 1]);
					this->group[active].pushNormal(normal[n1 * 3 - 3], normal[n1 * 3 - 2], normal[n1 * 3 - 1]);
					this->group[active].pushNormal(normal[n2 * 3 - 3], normal[n2 * 3 - 2], normal[n2 * 3 - 1]);

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

				this->group[active].pushPos(pos[v0 * 3 - 3], pos[v0 * 3 - 2], pos[v0 * 3 - 1]);
				this->group[active].pushPos(pos[v1 * 3 - 3], pos[v1 * 3 - 2], pos[v1 * 3 - 1]);
				this->group[active].pushPos(pos[v2 * 3 - 3], pos[v2 * 3 - 2], pos[v2 * 3 - 1]);

				this->group[active].pushCoord(coord[t0 * 3 - 3], coord[t0 * 3 - 2]);
				this->group[active].pushCoord(coord[t1 * 3 - 3], coord[t1 * 3 - 2]);
				this->group[active].pushCoord(coord[t2 * 3 - 3], coord[t2 * 3 - 2]);

				this->group[active].pushNormal(normal[n0 * 3 - 3], normal[n0 * 3 - 2], normal[n0 * 3 - 1]);
				this->group[active].pushNormal(normal[n1 * 3 - 3], normal[n1 * 3 - 2], normal[n1 * 3 - 1]);
				this->group[active].pushNormal(normal[n2 * 3 - 3], normal[n2 * 3 - 2], normal[n2 * 3 - 1]);

				//add more
				v1 = v2;
				n1 = n2;
				t1 = t2;
				while (fscanf(file, "%d/%d/%d", &v2, &t2, &n2) == 3)
				{
					this->group[active].pushPos(pos[v0 * 3 - 3], pos[v0 * 3 - 2], pos[v0 * 3 - 1]);
					this->group[active].pushPos(pos[v1 * 3 - 3], pos[v1 * 3 - 2], pos[v1 * 3 - 1]);
					this->group[active].pushPos(pos[v2 * 3 - 3], pos[v2 * 3 - 2], pos[v2 * 3 - 1]);

					this->group[active].pushCoord(coord[t0 * 3 - 3], coord[t0 * 3 - 2]);
					this->group[active].pushCoord(coord[t1 * 3 - 3], coord[t1 * 3 - 2]);
					this->group[active].pushCoord(coord[t2 * 3 - 3], coord[t2 * 3 - 2]);

					this->group[active].pushNormal(normal[n0 * 3 - 3], normal[n0 * 3 - 2], normal[n0 * 3 - 1]);
					this->group[active].pushNormal(normal[n1 * 3 - 3], normal[n1 * 3 - 2], normal[n1 * 3 - 1]);
					this->group[active].pushNormal(normal[n2 * 3 - 3], normal[n2 * 3 - 2], normal[n2 * 3 - 1]);

					v1 = v2;
					n1 = n2;
					t1 = t2;
				}
			}
			// type: v/vt
			else if (sscanf(buf, "%d/%d", &v0, &t0) == 2)
			{
				fscanf(file, "%d/%d", &v1, &t1);
				fscanf(file, "%d/%d", &v2, &t2);

				this->group[active].pushPos(pos[v0 * 3 - 3], pos[v0 * 3 - 2], pos[v0 * 3 - 1]);
				this->group[active].pushPos(pos[v1 * 3 - 3], pos[v1 * 3 - 2], pos[v1 * 3 - 1]);
				this->group[active].pushPos(pos[v2 * 3 - 3], pos[v2 * 3 - 2], pos[v2 * 3 - 1]);

				this->group[active].pushCoord(coord[t0 * 3 - 3], coord[t0 * 3 - 2]);
				this->group[active].pushCoord(coord[t1 * 3 - 3], coord[t1 * 3 - 2]);
				this->group[active].pushCoord(coord[t2 * 3 - 3], coord[t2 * 3 - 2]);

				glm::vec3 edge1(pos[v1 * 3 - 3] - pos[v0 * 3 - 3],
					pos[v1 * 3 - 2] - pos[v0 * 3 - 2],
					pos[v1 * 3 - 1] - pos[v0 * 3 - 1]);
				glm::vec3 edge2(pos[v2 * 3 - 3] - pos[v1 * 3 - 3],
					pos[v2 * 3 - 2] - pos[v1 * 3 - 2],
					pos[v2 * 3 - 1] - pos[v1 * 3 - 1]);
				glm::vec3 norm(edge2.y *edge1.z - edge1.y*edge2.z,
					edge2.z*edge1.x - edge1.z*edge2.x,
					edge2.x*edge1.y - edge1.x*edge2.y);
				
				this->group[active].pushNormal(norm.x, norm.y, norm.z);
				this->group[active].pushNormal(norm.x, norm.y, norm.z);
				this->group[active].pushNormal(norm.x, norm.y, norm.z);

				//add more
				v1 = v2;
				n1 = n2;
				t1 = t2;
				while (fscanf(file, "%d/%d", &v2, &t2) == 2)
				{
					this->group[active].pushPos(pos[v0 * 3 - 3], pos[v0 * 3 - 2], pos[v0 * 3 - 1]);
					this->group[active].pushPos(pos[v1 * 3 - 3], pos[v1 * 3 - 2], pos[v1 * 3 - 1]);
					this->group[active].pushPos(pos[v2 * 3 - 3], pos[v2 * 3 - 2], pos[v2 * 3 - 1]);

					this->group[active].pushCoord(coord[t0 * 3 - 3], coord[t0 * 3 - 2]);
					this->group[active].pushCoord(coord[t1 * 3 - 3], coord[t1 * 3 - 2]);
					this->group[active].pushCoord(coord[t2 * 3 - 3], coord[t2 * 3 - 2]);

					glm::vec3 edge1(pos[v1 * 3 - 3] - pos[v0 * 3 - 3],
						pos[v1 * 3 - 2] - pos[v0 * 3 - 2],
						pos[v1 * 3 - 1] - pos[v0 * 3 - 1]);
					glm::vec3 edge2(pos[v2 * 3 - 3] - pos[v1 * 3 - 3],
						pos[v2 * 3 - 2] - pos[v1 * 3 - 2],
						pos[v2 * 3 - 1] - pos[v1 * 3 - 1]);
					glm::vec3 norm(edge2.y *edge1.z - edge1.y*edge2.z,
						edge2.z*edge1.x - edge1.z*edge2.x,
						edge2.x*edge1.y - edge1.x*edge2.y);

					this->group[active].pushNormal(norm.x, norm.y, norm.z);
					this->group[active].pushNormal(norm.x, norm.y, norm.z);
					this->group[active].pushNormal(norm.x, norm.y, norm.z);

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

				this->group[active].pushPos(pos[v0 * 3 - 3], pos[v0 * 3 - 2], pos[v0 * 3 - 1]);
				this->group[active].pushPos(pos[v1 * 3 - 3], pos[v1 * 3 - 2], pos[v1 * 3 - 1]);
				this->group[active].pushPos(pos[v2 * 3 - 3], pos[v2 * 3 - 2], pos[v2 * 3 - 1]);

				this->group[active].pushCoord();
				this->group[active].pushCoord();
				this->group[active].pushCoord();

				glm::vec3 edge1(pos[v1 * 3 - 3] - pos[v0 * 3 - 3],
					pos[v1 * 3 - 2] - pos[v0 * 3 - 2],
					pos[v1 * 3 - 1] - pos[v0 * 3 - 1]);
				glm::vec3 edge2(pos[v2 * 3 - 3] - pos[v1 * 3 - 3],
					pos[v2 * 3 - 2] - pos[v1 * 3 - 2],
					pos[v2 * 3 - 1] - pos[v1 * 3 - 1]);
				glm::vec3 norm(edge2.y *edge1.z - edge1.y*edge2.z,
					edge2.z*edge1.x - edge1.z*edge2.x,
					edge2.x*edge1.y - edge1.x*edge2.y);

				this->group[active].pushNormal(norm.x, norm.y, norm.z);
				this->group[active].pushNormal(norm.x, norm.y, norm.z);
				this->group[active].pushNormal(norm.x, norm.y, norm.z);

				//add more
				v1 = v2;
				n1 = n2;
				t1 = t2;
				while (fscanf(file, "%d", &v2) > 0)
				{
					this->group[active].pushPos(pos[v0 * 3 - 3], pos[v0 * 3 - 2], pos[v0 * 3 - 1]);
					this->group[active].pushPos(pos[v1 * 3 - 3], pos[v1 * 3 - 2], pos[v1 * 3 - 1]);
					this->group[active].pushPos(pos[v2 * 3 - 3], pos[v2 * 3 - 2], pos[v2 * 3 - 1]);

					this->group[active].pushCoord();
					this->group[active].pushCoord();
					this->group[active].pushCoord();

					glm::vec3 edge1(pos[v1 * 3 - 3] - pos[v0 * 3 - 3],
						pos[v1 * 3 - 2] - pos[v0 * 3 - 2],
						pos[v1 * 3 - 1] - pos[v0 * 3 - 1]);
					glm::vec3 edge2(pos[v2 * 3 - 3] - pos[v1 * 3 - 3],
						pos[v2 * 3 - 2] - pos[v1 * 3 - 2],
						pos[v2 * 3 - 1] - pos[v1 * 3 - 1]);
					glm::vec3 norm(edge2.y *edge1.z - edge1.y*edge2.z,
						edge2.z*edge1.x - edge1.z*edge2.x,
						edge2.x*edge1.y - edge1.x*edge2.y);

					this->group[active].pushNormal(norm.x, norm.y, norm.z);
					this->group[active].pushNormal(norm.x, norm.y, norm.z);
					this->group[active].pushNormal(norm.x, norm.y, norm.z);

					v1 = v2;
					n1 = n2;
					t1 = t2;
				}
			}
			break;
		}
		//mtllib
		case 'm':
		{
			std::ifstream min;
			string path = string(filename);
			unsigned int tmp = path.find_last_of('/');
			for (unsigned int i = path.length() - 1; i > tmp; i--)
				path.pop_back();
			fscanf(file, "%s", buf);
			string s(buf);
			string relative_path = path;

			string abs_path;
			path += buf;

			min.open(path);
			if (min.is_open() == false)
			{
				cout << "ERROR: CANNOT OPEN MTLLIB " << path << endl;
				exit(1);
			}
			while (min >> s) {
				if (s == "newmtl") {
					group.push_back(Group());
					min >> s;
					group[group.size() - 1].material.name = s;
					group[group.size() - 1].material.is_src = false;
				}
				else if (s == "Kd") {
					min >> num1 >> num2 >> num3;
					group[group.size() - 1].material.kd = glm::vec3(num1, num2, num3);
				}
				else if (s == "Ka") {
					min >> num1 >> num2 >> num3;
					group[group.size() - 1].material.ka = glm::vec3(num1, num2, num3);
				}
				else if (s == "Ks") {
					min >> num1 >> num2 >> num3;
					group[group.size() - 1].material.ks = glm::vec3(num1, num2, num3);
				}
				else if (s == "map_Kd") {
					min >> s;
					//cout << s << endl;
					bool success;
					abs_path = relative_path + s;
					success = group[group.size() - 1].material.pic(abs_path.c_str());
					group[group.size() - 1].material.is_src = success;
					if (success)
					{
						glGenTextures(1, &group[group.size() - 1].material.texName);
					}
				}
				else {
					min.getline(buf, 256);
				}
			}
			break;
		}
		case 'u':
		{
			active = 0;
			fscanf(file, "%s", buf);
			string s(buf);
			for (unsigned int i = 0; i < group.size(); i++) {
				if (group[i].material.name == s) {
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
	//print();
	return this;
}
void object::shadow()
{
	//cout << "shadow" << endl;
	if (pos.size() == 0)return;

	shadowShader->use();
	shadowShader->setMat4("u_modelMatrix", model);

	glBindVertexArray(svao);
	glBindBuffer(GL_ARRAY_BUFFER, spositionBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, pos.size() * 4, getPos(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
	glBindVertexArray(0);

	glBindVertexArray(svao);
	glDrawArrays(GL_TRIANGLES, 0, pos.size() / 3);
}
void object::show() {
	if (pos.size() == 0)return;

	elementShader->use();
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, pos.size() * 4, getPos(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
	glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle);
	//cout << color.size() * 4 << endl;
	glBufferData(GL_ARRAY_BUFFER, color.size() * 4, getColor(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
	glBindBuffer(GL_ARRAY_BUFFER, normalBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * 4, getNormal(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
	glBindVertexArray(0);

	if (diy) {
		elementShader->setVec3("u_lightDiff", kd);
		elementShader->setVec3("u_lightAmb", ka);
		elementShader->setFloat("u_lightSpec", ks);
	}

	elementShader->setMat4("u_modelMatrix", model);
	elementShader->setInt("u_shadowMap", 0);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, pos.size() / 3);
}


void scene::shadow()
{
	for (auto & i : objCollection)
	{
		i->shadow();
	}

	for (auto & i : texCollection)
	{
		i->shadow();
	}

}

void scene::show(smLight * light,int lights)
{
	for (auto &e : objCollection) {
		e->show();
	}

	for (auto &t : texCollection) {
		t->show(lights);
	}

	for (auto & t : cloudCollection)
	{
		t->show(lights);
	}
	for (auto & t : fluidCollection)
	{
		t->show();
	}
}
