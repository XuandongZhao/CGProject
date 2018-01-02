#include "lib\shader.h"
#include "lib\camera.h"
#include "lib\model.h"
#include "lib\light.h"
#include "lib\mouse.h"
#include "lib\render.h"
#include "lib\keyboard.h"
#include "lib\shader.h"
#include "lib\sphere.h"
#include <iostream>

smCamera*camera;
smMouse* mouse;
smKeyBoard* keyBoard;

smRender render;
world myworld("myworld");


int SCR_WIDTH;
int SCR_HEIGHT;
smShader *elementShader;
smShader *texShader;
smShader *shadowShader;
smShader * paticleShader;

cloud testCloud,Explosion;

glm::vec3 cameraPosition(0.f, 50.f, 30.f);
glm::vec3 cameraDir(0, 0, -30);
void initExplosion(particle & m)
{
	if (m.sphere == nullptr)
	{
		//m.tex = new texture();
		//m.tex->load("source//pic.obj");
		//m.which = IS_TEXTURE;
		m.sphere = new Sphere();
		//m.sphere = new Sphere(50, 45, glm::vec4(1.f, 0.f, 0.f, 1.f));
		m.which = IS_SPHERE;
	}
	m.setScale(1, 1, 1);
	m.isDead = false;

	float pos[3] = { 0,50,0 };

	//float speed[3] = { (rand() / (double)RAND_MAX - 0.5)*5,(rand() / (double)RAND_MAX)*10,(rand() / (double)RAND_MAX - 0.5)*5 };
	float speed[3] = { 0,0,0 };
	float aspeed[3] = { fabs(rand() / (double)RAND_MAX - 0.5)*0.01,(rand() / (double)RAND_MAX)*0.1,(rand() / (double)RAND_MAX - 0.5)*0.01 };
	if (speed[0] * aspeed[0] < 0)
	{
		aspeed[0] = -aspeed[0];
	}

	if (speed[2] * aspeed[2] < 0)
	{
		aspeed[2] = -aspeed[2];
	}
	
	float r[3] = { (rand() / (double)RAND_MAX - 0.5) * 180,(rand() / (double)RAND_MAX - 0.5) * 180,(rand() / (double)RAND_MAX - 0.5) * 180 };
	//float w[3] = { (rand() / (double)RAND_MAX - 0.5) * 6,(rand() / (double)RAND_MAX - 0.5) * 6,(rand() / (double)RAND_MAX - 0.5) * 6 };
	//float b[3] = { (rand() / (double)RAND_MAX - 0.5) * 1,(rand() / (double)RAND_MAX - 0.5) * 1,(rand() / (double)RAND_MAX - 0.5) * 1 };
	
	m.setPosition(pos[0], pos[1], pos[2]);
	m.setSpeed(speed[0], speed[1], speed[2]);
	m.setAccerator(aspeed[0], aspeed[1], aspeed[2]);

	m.SetAngle(r[0], r[1], r[2]);
	//m.setW(w[0], w[1], w[2]);
	//m.setB(b[0], b[1], b[2]);

	m.lifetime = 5;

	m.deci = 0.1 + (rand() / (double(RAND_MAX)))*0.1;
}

bool ExplosionDead(particle & m)
{
	if (m.x < -0.3 || m.x > 0.3 || m.y > 51 || m.y < 50 || m.z > 0.3 || m.z < -0.3)
		return true;
	return false;
}

void initParticle(particle & m)
{
	if (m.tex == nullptr)
	{
		m.tex = new texture();
		m.tex->load("fly//fly.obj");
		m.which = IS_TEXTURE;
		//m.sphere = new Sphere(50, 45, glm::vec4(1.f, 0.f, 0.f, 1.f));
		//m.which = IS_SPHERE;
	}
	m.isDead = false;
	float x = rand() / (double(RAND_MAX)) - 0.5;
	float z = rand() / (double(RAND_MAX)) - 0.5;
	float pos[3] = { x,50,z };


	float speed[3] = { (rand() / (double(RAND_MAX)) - 0.5)*0.1,(rand() / (double(RAND_MAX)))*0.5,(rand() / (double(RAND_MAX)) - 0.5)*0.1 };
	float aspeed[3] = { 0,0.005,0 };
	m.setPosition(pos[0], pos[1], pos[2]);
	m.setSpeed(speed[0], speed[1], speed[2]);
	m.setAccerator(aspeed[0], aspeed[1], aspeed[2]);

	
	if (fabs(x) < fabs(z))
	{
		m.lifetime = 2.0*fabs(z) / 0.5;
	}
	else {
		m.lifetime = 2.0*fabs(x) / 0.5;
	}
	m.deci = 0.1 + (rand() / (double(RAND_MAX)))*0.1;
}

/*
@return true for dead
*/
bool isDead(particle & m)
{
	if (m.y < 50)
	{
		return true;
	}
	return false;
}


static void smInit()
{
	srand((unsigned int)time(NULL));
	glewInit();
	glEnable(GL_DEPTH_TEST);
	elementShader = new smShader("files//element.vert", "files//element.frag");
	texShader = new smShader("files//tex.vert", "files//tex.frag");
	shadowShader = new smShader("files//shadow.vert", "files//shadow.frag");
	camera = new smCamera(cameraPosition, cameraDir);
	mouse = new smMouse();
	keyBoard = new smKeyBoard();

	paticleShader = new smShader("files//paticle.vert", "files//paticle.frag");

	glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
}
unsigned int cnt = 0;

static void smTimer(int id)
{
	if (keyBoard->getKey((keyMap)'d') == true)
	{
		camera->moveCamera(-5, 0);
	}
	if (keyBoard->getKey((keyMap)'a') == true)
	{
		camera->moveCamera(5, 0);
	}
	if (keyBoard->getKey((keyMap)'s') == true)
	{
		camera->moveCamera(0, -5);
	}
	if (keyBoard->getKey((keyMap)'w') == true)
	{
		camera->moveCamera(0, 5);
	}

	if (keyBoard->getKey((keyMap)'q') == true)
	{
		camera->moveHCamera(10);
	}
	if (keyBoard->getKey((keyMap)'e') == true)
	{
		camera->moveHCamera(-10);
	}


	if (keyBoard->getKey((keyMap)'j') == true)
	{
		camera->rotateCamera(-0.1, 0);
	}
	if (keyBoard->getKey((keyMap)'l') == true)
	{
		camera->rotateCamera(0.1, 0);
	}
	if (keyBoard->getKey((keyMap)'i') == true)
	{
		camera->rotateCamera(0, -0.1);
	}
	if (keyBoard->getKey((keyMap)'k') == true)
	{
		camera->rotateCamera(0, 0.1);
	}


	glutTimerFunc(100, smTimer, id);
	glutPostRedisplay();
}

static void smDisplay() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	render.render(myworld, *camera);
	glutSwapBuffers();
}
static void smReshape(int w, int h) {
	SCR_WIDTH = w;
	SCR_HEIGHT = h;
}
void build() {
	static scene tmp;
	texture obj;
	obj.load("city//test3.obj");

	tmp.push_back(obj.scale(0.05, 0.05, 0.05));
	//testCloud.initParticle = initParticle;
	//testCloud.isDead = isDead;
	//testCloud.maxSize = 10;
	//testCloud.init();
	//tmp.push_back(testCloud);

	/*爆炸测试*/
	/*Explosion.initParticle = initExplosion;
	Explosion.isDead = ExplosionDead;
	Explosion.maxSize = 100;
	Explosion.init();
	tmp.push_back(Explosion);*/

	myworld.push_back(tmp);
	myworld.push_back(smLight(0, glm::vec3(55.f, 500.f, 23.f), glm::vec3(.7f, .7f, .7f), glm::vec3(.2f, .2f, .2f), 1.f));
	
}
static void smMouseFunc(int x, int y) {
	mouse->now.x = float(x);
	mouse->now.y = float(y);

	glutPostRedisplay();
}
static void smDrag(int x, int y) {
	float getx;
	float gety;
	if (abs(-mouse->pre.x + float(x))<100 && abs(-mouse->pre.y + float(y))<100)
		camera->rotateCamera((-mouse->pre.x + float(x)) / 40, (-mouse->pre.y + float(y)) / 40);
	mouse->pre.x = float(x);
	mouse->pre.y = float(y);
}
static void smClick(int button, int state, int x, int y) {
	mouse->state[button] = state;
	mouse->pre.x = float(x);
	mouse->pre.y = float(y);
}
static void smWheel(int wheel, int dir, int x, int y) {
	if (dir > 0)camera->zoomCamera(.95f);
	if (dir < 0)camera->zoomCamera(1.05f);

	glutPostRedisplay();
}

static void smKeyDown(unsigned char cAscii, int x, int y)
{
	static keyMap mykeys;
	if (cAscii == 'a' || cAscii == 's' || cAscii == 'd' || cAscii == 'w' || cAscii == 'q' || cAscii == 'e' || cAscii == 'j' || cAscii == 'k' || cAscii == 'l' || cAscii == 'i')
	{
		keyBoard->getKey((keyMap)cAscii) = true;
	}
}
static void smKeyUp(unsigned char cAscii, int x, int y)
{
	if (cAscii == 'a' || cAscii == 's' || cAscii == 'd' || cAscii == 'w' || cAscii == 'q' || cAscii == 'e' || cAscii == 'j' || cAscii == 'k' || cAscii == 'l' || cAscii == 'i')
	{
		keyBoard->getKey((keyMap)cAscii) = false;
	}
}

int main(int argc, char*argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE);

	glutInitWindowSize(800, 600);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("shina mashiro");
	smInit();

	glutDisplayFunc(smDisplay);
	glutReshapeFunc(smReshape);


	glutPassiveMotionFunc(smMouseFunc);
	glutMotionFunc(smDrag);
	glutMouseFunc(smClick);
	glutMouseWheelFunc(smWheel);

	glutKeyboardFunc(smKeyDown);
	glutKeyboardUpFunc(smKeyUp);

	glutTimerFunc(100, smTimer, 1);

	build();
	glutMainLoop();
	return 0;


}
