#include<stdlib.h>
#include"lib\fluid.h"
#include "lib/shader.h"

extern smShader * texParticleShader;
extern smShader *texShader;
extern smShader *shadowShader;

Fluid::Fluid(long n, long m, float d, float t, float c, float mu, const char* name)
{
	pic(name);
	width = n; //宽度
	height = m;  //高度
	long count = n * m;  //网格点个数

	buffer[0] = new Vector3D[count];  //缓冲区1
	buffer[1] = new Vector3D[count];  //缓冲区2
	renderBuffer = 0;  //渲染缓冲区

	normals = new Vector3D[count]; //法线
	tangent = new Vector3D[count]; //切线
	pos = new float[9 * 2 * (height - 1)*(width - 1)];
	coord = new float[6 * 2 * (height - 1)*(width - 1)];
	normal = new float[9 * 2 * (height - 1)*(width - 1)];

	for (int i = 0; i < 3; i++) {
		indices[i] = new int[2 * (n - 1)*(m - 1)];
	}
	for (int i = 0; i < 6; i++) {
		texcoords[i % 3][i / 3] = new float[2 * (n - 1)*(m - 1)];
	}

	// Precompute constants for Equation (15.25).  
	//预先计算：流体表面方程的多项式系数
	float f1 = c * c * t * t / (d * d);
	float f2 = 1.0F / (mu * t + 2);
	k1 = (4.0F - 8.0F * f1) * f2;
	k2 = (mu * t - 2) * f2;
	k3 = 2.0F * f1 * f2;

	// Initialize buffers.  
	//初始化缓冲区
	long a = 0;

	for (long j = 0; j < m; j++)
	{
		float y = d * j;
		for (long i = 0; i < n; i++)
		{
			if (i == 0 || j == 0 || i == n - 1 || j == m - 1)buffer[0][a].Set(d * i, y, 0.0F);
			else {
				int r = rand() % 2;
				if (r == 0)buffer[0][a].Set(d * i, y, -5.f);
				else buffer[0][a].Set(d * i, y, -55.0F);
			}
			buffer[1][a] = buffer[0][a];
			normals[a].Set(0.0F, 0.0F, 2.0F * d);
			tangent[a].Set(2.0F * d, 0.0F, 0.0F);
			a++;
		}
	}

	a = 0;
	float w = 1.0f / width;
	float h = 1.0f / height;
	for (int i = 0; i < n - 1; i++) {
		for (int j = 0; j < m - 1; j++) {
			long t = j * m + i;
			indices[0][a] = t;
			indices[1][a] = t + 1;
			indices[2][a] = t + n;

			texcoords[0][0][a] = i * w;
			texcoords[0][1][a] = j * h;

			texcoords[1][0][a] = (i + 1)*w;
			texcoords[1][1][a] = j * h;

			texcoords[2][0][a] = i * w;
			texcoords[2][1][a] = (j + 1)*h;

			a++;
		}
	}

	for (int i = 1; i < n; i++) {
		for (int j = 1; j < m; j++) {
			long t = j * m + i;
			indices[0][a] = t;
			indices[1][a] = t - 1;
			indices[2][a] = t - n;

			texcoords[0][0][a] = i * w;
			texcoords[0][1][a] = j * h;

			texcoords[1][0][a] = (i - 1)*w;
			texcoords[1][1][a] = j * h;

			texcoords[2][0][a] = i * w;
			texcoords[2][1][a] = (j - 1)*h;

			a++;
		}
	}
	glGenVertexArrays(1, &vao);
	glGenVertexArrays(1, &svao);
	glGenBuffers(3, vboHandles);
	positionBufferHandle = vboHandles[0];
	coordBufferHandle = vboHandles[1];
	normalBufferHandle = vboHandles[2];
	glGenTextures(1, &texName);
	glGenBuffers(1, &spositionBufferHandle);


}

Fluid::~Fluid()
{
	//delete[] tangent;
	//delete[] normal;
	delete[] buffer[1];
	delete[] buffer[0];
}

void Fluid::Evaluate(void)
{
	// Apply Equation (15.25).  
	//调用流体表面方程
	for (long j = 1; j < height - 1; j++)
	{
		//当前顶点位移
		const Vector3D *crnt = buffer[renderBuffer] + j * width;
		//前一顶点位移
		Vector3D *prev = buffer[1 - renderBuffer] + j * width;

		//z(i,j,k+1) = k1 * z(i,j,k) + k2 * z(i,j,k-1) +
		//     k3 * (z(i+1,j,k) + z(i-1,j,k) + z(i,j+1,k) + z(i,j-1,k)

		for (long i = 1; i < width - 1; i++)
		{
			prev[i].z = k1 * crnt[i].z + k2 * prev[i].z +
				k3 * (crnt[i + 1].z + crnt[i - 1].z +
					crnt[i + width].z + crnt[i - width].z);
		}
	}

	renderBuffer = 1 - renderBuffer;

	for (long j = 1; j < height - 1; j++)
	{
		const Vector3D *next = buffer[renderBuffer] + j * width;
		Vector3D *nrml = normals + j * width;
		Vector3D *tang = tangent + j * width;

		for (long i = 1; i < width - 1; i++)
		{
			nrml[i].x = next[i - 1].z - next[i + 1].z;
			nrml[i].y = next[i - width].z - next[i + width].z;
			tang[i].z = next[i + 1].z - next[i - 1].z;
		}
	}

}
void Fluid::getData()
{
	for (int i = 0; i < 2 * (height - 1)*(width - 1); i++) {
		normal[i * 9] = normals[indices[0][i]].x;
		normal[i * 9 + 1] = normals[indices[0][i]].y;
		normal[i * 9 + 2] = normals[indices[0][i]].z;

		coord[i * 6] = texcoords[0][0][i];
		coord[i * 6 + 1] = texcoords[0][1][i];

		pos[i * 9] = buffer[renderBuffer][indices[0][i]].x;
		pos[i * 9 + 1] = buffer[renderBuffer][indices[0][i]].y;
		pos[i * 9 + 2] = buffer[renderBuffer][indices[0][i]].z;

		normal[i * 9 + 3] = normals[indices[1][i]].x;
		normal[i * 9 + 4] = normals[indices[1][i]].y;
		normal[i * 9 + 5] = normals[indices[1][i]].z;

		coord[i * 6 + 2] = texcoords[1][0][i];
		coord[i * 6 + 3] = texcoords[1][1][i];

		pos[i * 9 + 3] = buffer[renderBuffer][indices[1][i]].x;
		pos[i * 9 + 4] = buffer[renderBuffer][indices[1][i]].y;
		pos[i * 9 + 5] = buffer[renderBuffer][indices[1][i]].z;

		normal[i * 9 + 6] = normals[indices[2][i]].x;
		normal[i * 9 + 7] = normals[indices[2][i]].y;
		normal[i * 9 + 8] = normals[indices[2][i]].z;

		coord[i * 6 + 4] = texcoords[2][0][i];
		coord[i * 6 + 5] = texcoords[2][1][i];

		pos[i * 9 + 6] = buffer[renderBuffer][indices[2][i]].x;
		pos[i * 9 + 7] = buffer[renderBuffer][indices[2][i]].y;
		pos[i * 9 + 8] = buffer[renderBuffer][indices[2][i]].z;

	}
}

void Fluid::shadow()
{
	shadowShader->use();
	shadowShader->setMat4("u_modelMatrix", model);
	glBindVertexArray(svao);
	glBindBuffer(GL_ARRAY_BUFFER, spositionBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, 9 * 2 * (height - 1)*(width - 1) * 4, pos, GL_STREAM_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
	glBindVertexArray(0);

	glBindVertexArray(svao);
	glDrawArrays(GL_TRIANGLES, 0, 3 * 2 * (height - 1)*(width - 1) * 4);
}

void Fluid::show(int lights)
{
	texShader->use();
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, 9 * 2 * (height - 1)*(width - 1) * 4, pos, GL_STREAM_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
	glBindBuffer(GL_ARRAY_BUFFER, coordBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, 6 * 2 * (height - 1)*(width - 1) * 4, coord, GL_STREAM_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), NULL);
	glBindBuffer(GL_ARRAY_BUFFER, normalBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, 9 * 2 * (height - 1)*(width - 1) * 4, normal, GL_STREAM_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texName);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, src.sizeX, src.sizeY, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, src.data);
	texShader->setInt("u_enLight", true);
	texShader->setMat4("u_modelMatrix", model);
	glm::mat4x4 inv = glm::transpose(glm::inverse(model));
	texShader->setMat4("u_normalMatrix", inv);
	for (int i = 0; i < lights; i++) {
		char tmp[64];
		sprintf(tmp, "u_shadowMap[%d]", i);
		texShader->setInt(tmp, i + 1);
	}
	texShader->setInt("u_textureMap", 0);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 3 * 2 * (height - 1)*(width - 1) * 4);
	/*
	texParticleShader->use();
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, 9 * 2 * (height - 1)*(width - 1) * 4, pos, GL_STREAM_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
	glBindBuffer(GL_ARRAY_BUFFER, coordBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, 6 * 2 * (height - 1)*(width - 1) * 4, coord, GL_STREAM_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), NULL);

	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texName);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, src.sizeX, src.sizeY, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, src.data);


	texParticleShader->setInt("u_textureMap", 1);
	texParticleShader->setMat4("u_modelMatrix", model);

	texParticleShader->setVec4("v_color", glm::vec4(1,1,1,1));
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 3 * 2 * (height - 1)*(width - 1) * 4);

	*/
}


//
//void Fluid::draw()
//{
//	glEnable(GL_TEXTURE_2D);
//	glBindTexture(GL_TEXTURE_2D, texture);  //选择纹理texture[status]     
//	glBegin(GL_TRIANGLES);
//
//	for (int i = 0; i < 2 * (height - 1)*(width - 1); i++) {
//		/*	glNormal3f(normal[(int)indices[i].x].x,
//		normal[(int)indices[i].x].y,
//		normal[(int)indices[i].x].z);*/
//		glTexCoord2f(texcoords[0][0][i], texcoords[0][1][i]);
//		glVertex3f(buffer[renderBuffer][indices[0][i]].x,
//			buffer[renderBuffer][indices[0][i]].y,
//			buffer[renderBuffer][indices[0][i]].z);
//
//		/*	glNormal3f(normal[(int)indices[i].y].x,
//		normal[(int)indices[i].y].y,
//		normal[(int)indices[i].y].z);*/
//		glTexCoord2f(texcoords[1][0][i], texcoords[1][1][i]);
//		glVertex3f(buffer[renderBuffer][indices[1][i]].x,
//			buffer[renderBuffer][indices[1][i]].y,
//			buffer[renderBuffer][indices[1][i]].z);
//
//		/*	glNormal3f(normal[(int)indices[i].z].x,
//		normal[(int)indices[i].z].y,
//		normal[(int)indices[i].z].z);*/
//		glTexCoord2f(texcoords[2][0][i], texcoords[2][1][i]);
//		glVertex3f(buffer[renderBuffer][indices[2][i]].x,
//			buffer[renderBuffer][indices[2][i]].y,
//			buffer[renderBuffer][indices[2][i]].z);
//
//	}
//
//	glEnd();
//	glDisable(GL_TEXTURE_2D);
//}
//
