#include "lib\shader.h"
#include "lib\camera.h"
#include "lib\model.h"
#include "lib\light.h"
#include "lib\mouse.h"
#include "lib\render.h"
#include "lib\keyboard.h"
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
object* test;

static void smInit()
{
	glewInit();
	glEnable(GL_DEPTH_TEST);
	elementShader = new smShader("files//element.vert", "files//element.frag");
	texShader = new smShader("files//tex.vert", "files//tex.frag");
	shadowShader = new smShader("files//shadow.vert", "files//shadow.frag");
	camera = new smCamera(glm::vec3(0.0f, 0.0f, 8.0f));
	mouse = new smMouse();
	keyBoard = new smKeyBoard();
	
	glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
}
unsigned int cnt = 0;

static void smTimer(int id)
{
	cout << "1" << endl;
	if (keyBoard->getKey((keyMap)'w') == true)
	{
		cout << "2" << endl;
		myworld.sceneCollection[0].objCollection[0].rotate(20,glm::vec3(0,1,0));
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
//object sss;
void build() {
	static scene tmp;
	//test = new object();
	
	tmp.push_back(object().load("source//castle.obj"));

	tmp.push_back(object().load("source//ground.obj"));
	tmp.push_back(texture().load("source//pic.obj"));
	myworld.push_back(tmp);
	myworld.push_back(smLight(0, glm::vec3 (24.f, 32.f, 18.f), glm::vec3(.7f, .7f, .7f), glm::vec3(.1f, .1f, .1f), 1.f));
	//myworld.push_back(smLight(0, glm::vec3(-24.f, 32.f, 18.f), glm::vec3(.7f, .7f, .7f), glm::vec3(.1f, .1f, .1f), 1.f));
}
static void smMouseFunc(int x, int y) {
	mouse->now.x = float(x);
	mouse->now.y = float(y);

	glutPostRedisplay();
}
static void smDrag(int x, int y) {
	if (mouse->state[GLUT_LEFT_BUTTON] == GLUT_DOWN)
		camera->rotateCamera(float(x - mouse->pre.x) / 200, float(y - mouse->pre.y) / 200);
	if (mouse->state[GLUT_RIGHT_BUTTON] == GLUT_DOWN)
		camera->moveCamera(float(x - mouse->pre.x) / 2, float(y - mouse->pre.y) / 2);

	mouse->pre.x = float(x);
	mouse->pre.y = float(y);
	glutPostRedisplay();
}
static void smClick(int button, int state, int x, int y) {
	mouse->state[button] = state;
	mouse->pre.x = float(x);
	mouse->pre.y = float(y);
	if (button == GLUT_LEFT_BUTTON&&state == GLUT_DOWN) {
		mouse->state[GLUT_LEFT_BUTTON] = GLUT_DOWN;
	}

	glutPostRedisplay();
}
static void smWheel(int wheel, int dir, int x, int y) {
	if (dir > 0)camera->zoomCamera(.8f);
	if (dir < 0)camera->zoomCamera(1.2f);

	glutPostRedisplay();
}

static void smKeyDown(unsigned char cAscii, int x, int y)
{
	static keyMap mykeys;
	if (cAscii == 'a' || cAscii == 's' || cAscii == 'd' || cAscii == 'w')
	{
		keyBoard->getKey((keyMap)cAscii) = true;
	}
	//cout << "Down  " << cAscii << " " << x << " " << y << endl;
}
static void smKeyUp(unsigned char cAscii, int x, int y)
{
	if (cAscii == 'a' || cAscii == 's' || cAscii == 'd' || cAscii == 'w')
	{
		keyBoard->getKey((keyMap)cAscii) = false;
	}
	//cout << "Up  " << cAscii << " " << x << " " << y << endl;
}

int main(int argc,char*argv[])
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
