#include "lib\shader.h"
#include "lib\camera.h"
#include "lib\model.h"
#include "lib\light.h"
#include "lib\mouse.h"
#include "lib\render.h"
#include "lib\keyboard.h"
#include "lib\shader.h"
#include "lib\sphere.h"

#include "lib\obbCollision.h"
#include <time.h> 

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

Sphere *sphere;
cloud temC;
obbCollision *obbcam = new obbCollision;
obb_box *camerabox;
/** 用来设置粒子的属性值 */
float x, y, z, vx, vy, vz, ax, ay, az, sizei, lifetime, deci;
int r, g, b;


cloud testCloud;

glm::vec3 cameraPosition(0.f, 50.f, 30.f);
glm::vec3 cameraDir(0, 0, -30);

void initParticle(particle & m)
{
	//cout << "init" << endl;
	m.isDead = false;
	float pos[3] = { rand()%10,50,rand()%10};
	float speed[3] = { rand()%10-5,rand() % 10-5,rand() % 10-5 };
	float aspeed[3] = { rand()%2-0.5,rand() % 2-0.5,rand() % 2 -0.5};
	m.setPosition(pos[0], pos[1], pos[2]);
	m.setSpeed(speed[0], speed[1], speed[2]);
	m.setAccerator(aspeed[0], aspeed[1], aspeed[2]);
	m.lifetime = 10;
	m.deci = 1;
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

	//temC.totalCloudInfo.Create(5000);
	//initCloudInfo();
	//obb_box build1 = obbcam->gen_obb_box(object::getPos)


}
unsigned int cnt = 0;

static void smTimer(int id)
{
	cout << camera->eye.x << " " << camera->eye.y << " " << camera->eye.z << "***" << endl;
	camerabox->center = glm::vec3(camera->eye.x, camera->eye.y, camera->eye.z);
	camerabox->half = glm::vec3(1., 1., 1.);
	camerabox->x_axis = glm::vec3(1., 0, 0);
	camerabox->z_axis = glm::vec3(0, 0, 1.);
	//cout << "1" << endl;
	if (keyBoard->getKey((keyMap)'d') == true)
	{
		for (size_t i = 0; i < 8; i++)
		{

		}
		//myworld.getScenes()[0].cloudCollection[0].sphereCollection[0].translate(-0.1,0,0);
		camera->moveCamera(-5, 0);
		//sphere->translate(-0.1, 0, 0);
		//(camera->getEye()).z -= 5;
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
	//glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	//glClear(GL_COLOR_BUFFER_BIT);


	render.render(myworld, *camera);
	//sphere->show();

	//testCircles.show(*camera);
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

	object obj1;
	obj1.load("city//tem//1//test1.obj");
	tmp.push_back(obj1.scale(0.05, 0.05, 0.05));
	tmp.push_back(temC);
	//testCircles.push_back(sphere(10, 2, "fuck", glm::vec4(1, 0, 0, 1)));
	//cout<<testCircles.particlesCollection[0].draw()<<endl;.push_back(*sphere)

	//tmp.push_back(object().load("fly//fly.obj").translate(0,50,0));

	testCloud.initParticle = initParticle;
	testCloud.isDead = isDead;
	testCloud.maxSize = 1000;
	
	testCloud.init();
	tmp.push_back(testCloud);


	myworld.push_back(tmp);
	myworld.push_back(smLight(0, glm::vec3(55.f, 500.f, 23.f), glm::vec3(.7f, .7f, .7f), glm::vec3(.2f, .2f, .2f), 1.f));
	//myworld.push_back(smLight(0, glm::vec3(-24.f, 32.f, 18.f), glm::vec3(.7f, .7f, .7f), glm::vec3(.1f, .1f, .1f), 1.f));
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

	//glutPostRedisplay();
}
static void smClick(int button, int state, int x, int y) {
	mouse->state[button] = state;
	mouse->pre.x = float(x);
	mouse->pre.y = float(y);
	//if (button == GLUT_LEFT_BUTTON&&state == GLUT_DOWN) {
	//	mouse->state[GLUT_LEFT_BUTTON] = GLUT_DOWN;
	//}

	//glutPostRedisplay();
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
	//cout << "Down  " << cAscii << " " << x << " " << y << endl;
}
static void smKeyUp(unsigned char cAscii, int x, int y)
{
	if (cAscii == 'a' || cAscii == 's' || cAscii == 'd' || cAscii == 'w' || cAscii == 'q' || cAscii == 'e' || cAscii == 'j' || cAscii == 'k' || cAscii == 'l' || cAscii == 'i')
	{
		keyBoard->getKey((keyMap)cAscii) = false;
	}
	//cout << "Up  " << cAscii << " " << x << " " << y << endl;
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
	//glutSpecialFunc(smSpecialDown);
	//glutSpecialUpFunc(smSpecialUp);

	build();
	glutMainLoop();
	return 0;


}
