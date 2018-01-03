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
smShader * texParticleShader;

cloud testCloud;

glm::vec3 cameraPosition(0.f, 50.f, 30.f);
glm::vec3 cameraDir(0, 0, -30);

float fireX, fireY=50, fireZ;

glm::vec3 initialColor(0.9765f, 1.f, 0.396f);
glm::vec3 fadeColor(0.396f, 0.0824f, 0.04705f);
GLfloat randNumber[10000];

void initParticle(particle * m)
{
	if (m->rec == nullptr)
	{
		m->rec = new rectangle();
		m->rec->setShape(0.1, 0.3);
		m->rec->fill("source//file.jpg");
		m->which = IS_RECTANGLE;
		//m->sphere = new Sphere(0.05, 45, glm::vec4(1.f, 0.f, 0.f, 1.f));
		//m->which = IS_SPHERE;
	}
	//cout << "fuck" << endl;
	int random = rand() % 10000;
	m->isDead = false;
	float x = fireX + randNumber[random] - 0.5;
	float y = fireY;
	float z = fireZ + randNumber[(random*2)%10000] - 0.5;
	float pos[3] = { x,y,z };


	float speed[3] = { 0,randNumber[(random*3) % 10000],0 };
	float aspeed[3] = { 0,0.005,0 };
	m->setPosition(pos[0], pos[1], pos[2]);
	m->setSpeed(speed[0], speed[1], speed[2]);
	m->setAccerator(aspeed[0], aspeed[1], aspeed[2]);
	m->setAngle(randNumber[(random * 4) % 10000], randNumber[(random * 5) % 10000], randNumber[(random * 6) % 10000]);
	(m->rec)->color = glm::vec4(initialColor,1.f);


	m->lifetime = 1+randNumber[(random * 8) % 10000];
	m->fullLife = m->lifetime;
	/*if (fabs(x-fireX) < fabs(z-fireZ))
	{
		m->lifetime = 2.0*fabs(fabs(z-fireZ)-0.5) / 0.5;
	}
	else {
		m->lifetime = 2.0*fabs(fabs(x-fireX)-0.5) / 0.5;
	}*/
	m->deci = 0.05;
}

/*
@return true for dead
*/
bool isDead(particle * m)
{

	return false;
}


static void smInit()
{

	srand((unsigned int)time(NULL));
	for (int i = 0; i < 10000; i++)
	{
		randNumber[i]=rand() / (double(RAND_MAX));
	}



	glewInit();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	elementShader = new smShader("files//element.vert", "files//element.frag");
	texShader = new smShader("files//tex.vert", "files//tex.frag");
	shadowShader = new smShader("files//shadow.vert", "files//shadow.frag");
	camera = new smCamera(cameraPosition, cameraDir);
	mouse = new smMouse();
	keyBoard = new smKeyBoard();

	paticleShader = new smShader("files//paticle.vert", "files//paticle.frag");
	texParticleShader = new smShader("files//rectangle.vert", "files//rectangle.frag");

	glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
}
unsigned int cnt = 0;

static void smTimer(int id)
{
	if (keyBoard->getKey((keyMap)'d') == true)
	{
		//camera->moveCamera(-5, 0);
		fireX++;
	}
	if (keyBoard->getKey((keyMap)'a') == true)
	{
		//camera->moveCamera(5, 0);
		fireX--;
	}
	if (keyBoard->getKey((keyMap)'s') == true)
	{
		//camera->moveCamera(0, -5);
		fireZ++;
	}
	if (keyBoard->getKey((keyMap)'w') == true)
	{
		//camera->moveCamera(0, 5);
		fireZ--;
	}

	if (keyBoard->getKey((keyMap)'q') == true)
	{
		//camera->moveHCamera(10);
		fireY+=0.1;
	}
	if (keyBoard->getKey((keyMap)'e') == true)
	{
		//camera->moveHCamera(-10);
		fireY-=0.1;
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

	tmp.push_back((new texture())->load("city//test3.obj")->scale(0.05, 0.05, 0.05));
	testCloud.initParticle = initParticle;
	testCloud.isDead = isDead;
	testCloud.maxSize = 3000;
	testCloud.init();
	tmp.push_back(&testCloud);


	myworld.push_back(&tmp);
	myworld.push_back(new smLight(0, glm::vec3(55.f, 500.f, 23.f), glm::vec3(.7f, .7f, .7f), glm::vec3(.2f, .2f, .2f), 1.f));
	
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
