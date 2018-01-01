#include "lib/sphere.h"
#include "lib/shader.h"



extern smShader * paticleShader;

Sphere::Sphere(GLfloat _radius = 1.f, GLfloat _step = 2.f,glm::vec4& _color=glm::vec4(1.0f,0.f,0.f,1.0f)) :
	radius(_radius),step(_step),color(_color)
{
		
	
}

void Sphere::init()
{
	assert(radius > 0);
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	GLfloat r1, r2, h1, h2, cc, ss;
	for (GLfloat i = -90.f; i <= 90.f; i += step)
	{
		r1 = cos(PI*i / 180.0)*radius;
		r2 = cos(PI*(i + step) / 180.0)*radius;
		h1 = sin(PI*i / 180)*radius;
		h2 = sin(PI*(i + step) / 180)*radius;
		for (GLfloat j = 0.f; j <= 360.f; j += step)
		{
			cc = cos(j*PI / 180);
			ss = -sin(j*PI / 180);
			pushPos(r2*cc, h2, r2*ss);
			pushPos(r1*cc, h1, r1*ss);
		}


	}





	
}

void Sphere::show()
{
	//cout << "1" <<pos.size()/3<< endl;
	//cout << "&&&" << vao << " " << vbo << endl;
	paticleShader->use();
	
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(GLfloat), getPos(), GL_STREAM_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	paticleShader->setVec4("v_color", color);
	paticleShader->setMat4("u_modelMatrix", model);

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, pos.size()/3);
}

