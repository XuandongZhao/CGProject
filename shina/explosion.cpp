#include "lib/explosion.h"

void randInit()
{
	srand((unsigned int)time(NULL));
	for (int i = 0; i < 10000; i++)
	{
		Explosion_randNumber[i] = rand() / (double(RAND_MAX));
	}
}

void ExplosionInit(cloud explosion[], int max, int time)
{
	Explosion_maxSize = max;
	randInit();
	explosion[0].initParticle = up;
	explosion[0].isDead = NULL;
	explosion[0].maxSize = Explosion_maxSize / 6;
	explosion[0].init();
	explosion[0].time = time;

	explosion[1].initParticle = down;
	explosion[1].isDead = NULL;
	explosion[1].maxSize = Explosion_maxSize / 6;
	explosion[1].init();
	explosion[1].time = time;

	explosion[2].initParticle = left;
	explosion[2].isDead = NULL;
	explosion[2].maxSize = Explosion_maxSize / 6;
	explosion[2].init();
	explosion[2].time = time;

	explosion[3].initParticle = right;
	explosion[3].isDead = NULL;
	explosion[3].maxSize = Explosion_maxSize / 6;
	explosion[3].init();
	explosion[3].time = time;

	explosion[4].initParticle = back;
	explosion[4].isDead = NULL;
	explosion[4].maxSize = Explosion_maxSize / 6;
	explosion[4].init();
	explosion[4].time = time;

	explosion[5].initParticle = front;
	explosion[5].isDead = NULL;
	explosion[5].maxSize = Explosion_maxSize / 6;
	explosion[5].init();
	explosion[5].time = time;

	int num = Explosion_maxSize / 6;
	for (int i = 0; i < num; i++)
	{
		explosion[0].particleCollection[i]->isDead = true;
		explosion[1].particleCollection[i]->isDead = true;
		explosion[2].particleCollection[i]->isDead = true;
		explosion[3].particleCollection[i]->isDead = true;
		explosion[4].particleCollection[i]->isDead = true;
		explosion[5].particleCollection[i]->isDead = true;
	}
}


//up
void up(particle *m,const char* name)
{
	if (m->rec == nullptr)
	{
		m->rec = new rectangle();
		m->rec->setShape(0.08, 0.15);
		m->rec->fill(name);
		m->which = IS_RECTANGLE;
	}
	int ran = rand() % 10000;
	m->isDead = false;
	float x = Explosion_x + Explosion_randNumber[ran] * 2 * Explosion_xdis - Explosion_xdis;
	float y = Explosion_up;
	float z = Explosion_z + Explosion_randNumber[(ran * 2) % 10000] * 2 * Explosion_zdis - Explosion_zdis;
	float pos[3] = { x,y,z };


	float speed[3] = { Explosion_randNumber[(ran * 3) % 10000] * 6 - 3,Explosion_randNumber[(ran * 4) % 10000] * 3,Explosion_randNumber[(ran * 5) % 10000] * 6 - 3 };
	float aspeed[3] = { 0,-0.05,0 };
	m->setPosition(pos[0], pos[1], pos[2]);
	m->setSpeed(speed[0], speed[1], speed[2]);
	m->setAccerator(aspeed[0], aspeed[1], aspeed[2]);
	m->setAngle(Explosion_randNumber[(ran * 6) % 10000], Explosion_randNumber[(ran * 7) % 10000], Explosion_randNumber[(ran * 8) % 10000]);
	(m->rec)->color = glm::vec4(Explosion_initialColor, 1.f);


	m->lifetime = Explosion_randNumber[(ran * 9) % 10000];
	m->fullLife = m->lifetime;
	m->deci = 0.05;
}
//down
void down(particle * m, const char* name)
{
	if (m->rec == nullptr)
	{
		m->rec = new rectangle();
		m->rec->setShape(0.08, 0.15);
		m->rec->fill(name);
		m->which = IS_RECTANGLE;
	}
	int ran = rand() % 10000;
	m->isDead = false;
	float x = Explosion_x + Explosion_randNumber[ran] * 2 * Explosion_xdis - Explosion_xdis;
	float y = Explosion_down;
	float z = Explosion_z + Explosion_randNumber[(ran * 2) % 10000] * 2 * Explosion_zdis - Explosion_zdis;
	float pos[3] = { x,y,z };


	float speed[3] = { Explosion_randNumber[(ran * 3) % 10000] * 6 - 3,-Explosion_randNumber[(ran * 4) % 10000] * 3,Explosion_randNumber[(ran * 5) % 10000] * 6 - 3 };
	float aspeed[3] = { 0,0.05,0 };
	m->setPosition(pos[0], pos[1], pos[2]);
	m->setSpeed(speed[0], speed[1], speed[2]);
	m->setAccerator(aspeed[0], aspeed[1], aspeed[2]);
	m->setAngle(Explosion_randNumber[(ran * 6) % 10000], Explosion_randNumber[(ran * 7) % 10000], Explosion_randNumber[(ran * 8) % 10000]);
	(m->rec)->color = glm::vec4(Explosion_initialColor, 1.f);


	m->lifetime = Explosion_randNumber[(ran * 9) % 10000];
	m->fullLife = m->lifetime;
	m->deci = 0.05;
}
//left
void left(particle * m, const char* name)
{
	if (m->rec == nullptr)
	{
		m->rec = new rectangle();
		m->rec->setShape(0.08, 0.15);
		m->rec->fill(name);
		m->which = IS_RECTANGLE;
	}
	int ran = rand() % 10000;
	m->isDead = false;
	float x = Explosion_left;
	float y = Explosion_y + Explosion_randNumber[ran] * 2 * Explosion_ydis - Explosion_ydis;
	float z = Explosion_z + Explosion_randNumber[(ran * 2) % 10000] * 2 * Explosion_zdis - Explosion_zdis;
	float pos[3] = { x,y,z };


	float speed[3] = { -Explosion_randNumber[(ran * 3) % 10000] * 3,Explosion_randNumber[(ran * 4) % 10000] * 6 - 3,Explosion_randNumber[(ran * 5) % 10000] * 6 - 3 };
	float aspeed[3] = { -0.005,0,0 };
	m->setPosition(pos[0], pos[1], pos[2]);
	m->setSpeed(speed[0], speed[1], speed[2]);
	m->setAccerator(aspeed[0], aspeed[1], aspeed[2]);
	m->setAngle(Explosion_randNumber[(ran * 6) % 10000], Explosion_randNumber[(ran * 7) % 10000], Explosion_randNumber[(ran * 8) % 10000]);
	(m->rec)->color = glm::vec4(Explosion_initialColor, 1.f);


	m->lifetime = Explosion_randNumber[(ran * 9) % 10000];
	m->fullLife = m->lifetime;
	m->deci = 0.05;
}
//right
void right(particle * m, const char* name)
{
	if (m->rec == nullptr)
	{
		m->rec = new rectangle();
		m->rec->setShape(0.08, 0.15);
		m->rec->fill(name);
		m->which = IS_RECTANGLE;
	}
	int ran = rand() % 10000;
	m->isDead = false;
	float x = Explosion_right;
	float y = Explosion_y + Explosion_randNumber[ran] * 2 * Explosion_ydis - Explosion_ydis;
	float z = Explosion_z + Explosion_randNumber[(ran * 2) % 10000] * 2 * Explosion_zdis - Explosion_zdis;
	float pos[3] = { x,y,z };


	float speed[3] = { Explosion_randNumber[(ran * 3) % 10000] * 3,Explosion_randNumber[(ran * 4) % 10000] * 6 - 3,Explosion_randNumber[(ran * 5) % 10000] * 6 - 3 };
	float aspeed[3] = { 0.005,0,0 };
	m->setPosition(pos[0], pos[1], pos[2]);
	m->setSpeed(speed[0], speed[1], speed[2]);
	m->setAccerator(aspeed[0], aspeed[1], aspeed[2]);
	m->setAngle(Explosion_randNumber[(ran * 6) % 10000], Explosion_randNumber[(ran * 7) % 10000], Explosion_randNumber[(ran * 8) % 10000]);
	(m->rec)->color = glm::vec4(Explosion_initialColor, 1.f);


	m->lifetime = Explosion_randNumber[(ran * 9) % 10000];
	m->fullLife = m->lifetime;
	m->deci = 0.05;
}
//behind
void back(particle * m, const char* name)
{
	if (m->rec == nullptr)
	{
		m->rec = new rectangle();
		m->rec->setShape(0.08, 0.15);
		m->rec->fill(name);
		m->which = IS_RECTANGLE;
	}
	int ran = rand() % 10000;
	m->isDead = false;
	float x = Explosion_x + Explosion_randNumber[ran] * 2 * Explosion_xdis - Explosion_xdis;
	float y = Explosion_y + Explosion_randNumber[(ran * 2) % 10000] * 2 * Explosion_ydis - Explosion_ydis;
	float z = Explosion_back;
	float pos[3] = { x,y,z };


	float speed[3] = { Explosion_randNumber[(ran * 3) % 10000] * 6 - 3,Explosion_randNumber[(ran * 4) % 10000] * 6 - 3,-Explosion_randNumber[(ran * 5) % 10000] * 3 };
	float aspeed[3] = { 0,0,-0.005 };
	m->setPosition(pos[0], pos[1], pos[2]);
	m->setSpeed(speed[0], speed[1], speed[2]);
	m->setAccerator(aspeed[0], aspeed[1], aspeed[2]);
	m->setAngle(Explosion_randNumber[(ran * 6) % 10000], Explosion_randNumber[(ran * 7) % 10000], Explosion_randNumber[(ran * 8) % 10000]);
	(m->rec)->color = glm::vec4(Explosion_initialColor, 1.f);


	m->lifetime = Explosion_randNumber[(ran * 9) % 10000];
	m->fullLife = m->lifetime;
	m->deci = 0.05;
}
//front
void front(particle * m, const char* name)
{
	if (m->rec == nullptr)
	{
		m->rec = new rectangle();
		m->rec->setShape(0.08, 0.15);
		m->rec->fill(name);
		m->which = IS_RECTANGLE;
	}
	int ran = rand() % 10000;
	m->isDead = false;
	float x = Explosion_x + Explosion_randNumber[ran] * 2 * Explosion_xdis - Explosion_xdis;
	float y = Explosion_y + Explosion_randNumber[(ran * 2) % 10000] * 2 * Explosion_ydis - Explosion_ydis;
	float z = Explosion_front;
	float pos[3] = { x,y,z };


	float speed[3] = { Explosion_randNumber[(ran * 3) % 10000] * 6 - 3,Explosion_randNumber[(ran * 4) % 10000] * 6 - 3,Explosion_randNumber[(ran * 5) % 10000] * 3 };
	float aspeed[3] = { 0,0,0.005 };
	m->setPosition(pos[0], pos[1], pos[2]);
	m->setSpeed(speed[0], speed[1], speed[2]);
	m->setAccerator(aspeed[0], aspeed[1], aspeed[2]);
	m->setAngle(Explosion_randNumber[(ran * 6) % 10000], Explosion_randNumber[(ran * 7) % 10000], Explosion_randNumber[(ran * 8) % 10000]);
	(m->rec)->color = glm::vec4(Explosion_initialColor, 1.f);


	m->lifetime = Explosion_randNumber[(ran * 9) % 10000];
	m->fullLife = m->lifetime;
	m->deci = 0.05;
}

void ExplosionPos(double x_left, double x_right, double y_up, double y_down, double z_front,double z_back)
{
	Explosion_back = z_back;
	Explosion_front = z_front;
	Explosion_left = x_left;
	Explosion_right = x_right;
	Explosion_up = y_up;
	Explosion_down = y_down;

	Explosion_xdis = x_right - x_left;
	Explosion_ydis = y_up - y_down;
	Explosion_zdis = z_front - z_back;

	Explosion_x = (x_right + x_left) / 2;
	Explosion_y = (y_up + y_down) / 2;
	Explosion_z = (z_front + z_back) / 2;
}
