#include "lib\shader.h"
#include "lib\camera.h"
#include "lib\model.h"
#include "lib\light.h"
#include "lib\mouse.h"
#include "lib\render.h"
#include "lib\keyboard.h"
#include "lib\shader.h"
#include "lib\sphere.h"
using namespace std;
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

glm::vec3 cameraPosition(0.f, 50.f, 30.f);
glm::vec3 cameraDir(0, 0, -30);

static void smInit()
{
	glewInit();
	glEnable(GL_DEPTH_TEST);
	elementShader = new smShader("files//element.vert", "files//element.frag");
	texShader = new smShader("files//tex.vert", "files//tex.frag");
	shadowShader = new smShader("files//shadow.vert", "files//shadow.frag");
	camera = new smCamera(cameraPosition,cameraDir);
	mouse = new smMouse();
	keyBoard = new smKeyBoard();

	paticleShader = new smShader("files//paticle.vert", "files//paticle.frag");



	glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
}
unsigned int cnt = 0;

static void smTimer(int id)
{
	cout << camera->eye.x << " " << camera->eye.y << " " << camera->eye.z << "***" << endl;
	//cout << "1" << endl;
	if (keyBoard->getKey((keyMap)'w') == true)
	{
		//myworld.getScenes()[0].cloudCollection[0].sphereCollection[0].translate(-0.1,0,0);
		camera->moveCamera(-50, 0);
		//sphere->translate(-0.1, 0, 0);
		//(camera->getEye()).z -= 5;
	}
	if (keyBoard->getKey((keyMap)'s') == true)
	{
		camera->moveCamera(50, 0);
	}
	if (keyBoard->getKey((keyMap)'a') == true)
	{
		camera->moveCamera(0, -50);
	}
	if (keyBoard->getKey((keyMap)'d') == true)
	{
		camera->moveCamera(0, 50);
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
//object sss;
void build() {
	static scene tmp;
	//Sphere *a = new Sphere(0.05, 1, glm::vec4(1.0f, 0.0f, 1.0f,1.0f));
	//a->init();
	tmp.push_back(object().load("city//test3.obj").scale(0.05,0.05,0.05));
	//tmp.push_back(cloud().push_back(*sphere).push_back(*a));
	//testCircles.push_back(sphere(10, 2, "fuck", glm::vec4(1, 0, 0, 1)));
	//cout<<testCircles.particlesCollection[0].draw()<<endl;


	//tmp.push_back(object().load("fly//fly.obj").translate(0,50,0));


	//tmp.push_back(object().load("city//test1.obj").unitize());
	myworld.push_back(tmp);
	myworld.push_back(smLight(0, glm::vec3 (55.f, 500.f, 23.f), glm::vec3(.7f, .7f, .7f), glm::vec3(.2f, .2f, .2f), 1.f));
	//myworld.push_back(smLight(0, glm::vec3(-24.f, 32.f, 18.f), glm::vec3(.7f, .7f, .7f), glm::vec3(.1f, .1f, .1f), 1.f));
}
static void smMouseFunc(int x, int y) {
	mouse->now.x = float(x);
	mouse->now.y = float(y);

	glutPostRedisplay();
}
static void smDrag(int x, int y) {
	/*
	if (mouse->state[GLUT_LEFT_BUTTON] == GLUT_DOWN)
	camera->rotateCamera(float(x - mouse->pre.x) / 2000, float(y - mouse->pre.y) / 2000);
	if (mouse->state[GLUT_RIGHT_BUTTON] == GLUT_DOWN)
	camera->moveCamera(float(x - mouse->pre.x) / 20, float(y - mouse->pre.y) / 20);
	*/
	mouse->pre.x = float(x);
	mouse->pre.y = float(y);
	//glutPostRedisplay();
}
static void smClick(int button, int state, int x, int y) {
	mouse->state[button] = state;
	mouse->pre.x = float(x);
	mouse->pre.y = float(y);
	if (button == GLUT_LEFT_BUTTON&&state == GLUT_DOWN) {
		mouse->state[GLUT_LEFT_BUTTON] = GLUT_DOWN;
	}

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
	if (cAscii == 'a' || cAscii == 's' || cAscii == 'd' || cAscii == 'w' || cAscii == 'j' || cAscii == 'k' || cAscii == 'l' || cAscii == 'i')
	{
		keyBoard->getKey((keyMap)cAscii) = true;
	}
	//cout << "Down  " << cAscii << " " << x << " " << y << endl;
}
static void smKeyUp(unsigned char cAscii, int x, int y)
{
	if (cAscii == 'a' || cAscii == 's' || cAscii == 'd' || cAscii == 'w' || cAscii == 'j' || cAscii == 'k' || cAscii == 'l' || cAscii == 'i')
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

	glutTimerFunc(100,smTimer,1);
	//glutSpecialFunc(smSpecialDown);
	//glutSpecialUpFunc(smSpecialUp);

	build();
	glutMainLoop();
	return 0;


}
