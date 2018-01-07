#include "lib\shader.h"
#include "lib\camera.h"
#include "lib\model.h"
#include "lib\light.h"
#include "lib\mouse.h"
#include "lib\render.h"
#include "lib\keyboard.h"
#include "lib\shader.h"
#include "lib\sphere.h"
#include "lib\collision.h"
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

vector<collosion> objV;
collosion fly;
collosion flyPC;
collosion cameraEye;
vector<object> totalObj;
object * flyP;
texture *obFly;
texture *obPlane;
object * obMissile;

cloud testCloud;



glm::vec3 cameraPosition(0.f, 50.f, 30.f);
glm::vec3 cameraDir(0, 0, -30);

float fireX, fireY = 50, fireZ;
typedef struct
{
	int globaltimer;
	bool thebeginning;
	bool takeoff;
	bool control;
	bool gameover;
	bool launch;
}GameProg;
GameProg gameprog;

glm::vec3 initialColor(1.f, 0.996f, 0.3569f);
glm::vec3 fadeColor(0.396f, 0.0824f, 0.04705f);
GLfloat randNumber[10000];


void initParticle(particle * m)
{
	if (m->rec == nullptr)
	{
		m->rec = new rectangle();

		m->rec->setShape(0.08, 0.15);
		m->rec->fill("source//file.jpg");
		m->which = IS_RECTANGLE;
		//m->sphere = new Sphere(0.05, 45, glm::vec4(1.f, 0.f, 0.f, 1.f));
		//m->which = IS_SPHERE;
	}
	//cout << "fuck" << endl;
	int random = rand() % 10000;
	m->isDead = false;

	float x = fireX + randNumber[random] * 0.6 - 0.3;
	float y = fireY;
	float z = fireZ + randNumber[(random * 2) % 10000] * 0.6 - 0.3;

	float pos[3] = { x,y,z };


	float speed[3] = { 0,randNumber[(random * 3) % 10000] * 3,0 };
	float aspeed[3] = { 0,-0.05,0 };
	m->setPosition(pos[0], pos[1], pos[2]);
	m->setSpeed(speed[0], speed[1], speed[2]);
	m->setAccerator(aspeed[0], aspeed[1], aspeed[2]);

	m->setAngle(randNumber[(random * 4) % 10000], randNumber[(random * 5) % 10000], randNumber[(random * 6) % 10000]);
	(m->rec)->color = glm::vec4(initialColor, 1.f);


	m->lifetime = randNumber[(random * 8) % 10000];
	m->fullLife = m->lifetime;
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
		randNumber[i] = rand() / (double(RAND_MAX));
	}



	glewInit();
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_BLEND);
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
std::vector<glm::vec3> posRe;
glm::mat4 model;
void caculate(glm::vec3 pos, double result[]) {
	for (int j = 0; j < 4; j++) {
		result[j] = 0;
		for (int k = 0; k<3; k++)
			result[j] += ((double)(model[k][j])) * (double)(pos[k]);
		result[j] += ((double)(model[3][j]));
	}
	for (int j = 0; j < 3; j++)
		result[j] /= result[3];
}
void testCollision(collosion& fly, int type) {
	if (type == 0)
		posRe = fly.top;
	else posRe = fly.top2;

	for (int i = 0; i < posRe.size(); i++) {
		int last = 1;
		glm::mat4 tem;
		double result[4];
		for (int j = 0; j < 4; j++) {
			result[j] = 0;
			for (int k = 0; k<3; k++)
				result[j] += ((double)(model[k][j])) * (double)(posRe[i][k]);
			result[j] += ((double)(model[3][j])) * last;
		}
		for (int j = 0; j < 3; j++) {
			if (type == 0)
				fly.topR[i][j] = result[j] / result[3];
			else fly.topR2[i][j] = result[j] / result[3];
		}

	}

	if (type == 0)
		fly.gen_obb_box(fly.topR, type);
	else
		fly.gen_obb_box(fly.topR2, type);
}
static void dealEye() {
	std::vector<glm::vec3> posEye;
	glm::vec3 temV;
	int flag[3] = { 0 };
	for (int i = 0; i < 2; i++) {
		flag[0] = i - 1;
		temV[0] = (double)(camera->eye[0]) + flag[0] * 40;
		for (int j = 0; j < 2; j++) {
			flag[1] = j - 1;
			temV[1] = (double)(camera->eye[1]) + flag[1] * 40;
			for (int k = 0; k < 2; k++) {
				flag[2] = k - 1;
				temV[2] = (double)(camera->eye[2]) + flag[2] * 40;
				posEye.push_back(temV);
			}
		}
	}
	cameraEye.topR = posEye;
	cameraEye.gen_obb_box(cameraEye.topR);
}

int speed = 80;
char key[8];
bool isFirstView = true;
glm::vec3 eyeStart;
int count = 0;
glm::vec3 eyeEnd;

void cameramove()
{
	if (keyBoard->getKey((keyMap)'d') == true)
	{
		camera->moveCamera(-50, 0);
		//fireX++;
	}
	if (keyBoard->getKey((keyMap)'a') == true)
	{
		camera->moveCamera(50, 0);
		//fireX--;
	}
	if (keyBoard->getKey((keyMap)'s') == true)
	{
		camera->moveCamera(0, -50);
		fireZ++;
	}
	if (keyBoard->getKey((keyMap)'w') == true)
	{
		camera->moveCamera(0, 50);
		fireZ--;
	}

	if (keyBoard->getKey((keyMap)'q') == true)
	{
		camera->moveHCamera(20);
		fireY += 0.1;
	}
	if (keyBoard->getKey((keyMap)'e') == true)
	{
		camera->moveHCamera(-20);
		fireY -= 0.1;
	}
	if (keyBoard->getKey((keyMap)'0') == true)
	{
		gameprog.takeoff = true;
		gameprog.thebeginning = false;
		isFirstView = true;
	}
}

static void smTimer(int id)
{
	count++;
	if (keyBoard->getKey((keyMap)'z') == true)
	{
		isFirstView = !isFirstView;
	}
	if (count == 1) {
		camera->eye[0] = -2192.02;
		camera->eye[1] = 3392.93;
		camera->eye[2] = 17740.6;
		eyeStart = camera->eye;
		eyeEnd = eyeStart;
		eyeEnd[2] -= 100;
	}
	if (isFirstView) {
		double result[4] = { 0 };

		model = obFly->getModel();
		caculate(eyeStart, result);
		camera->eye[0] = result[0];
		camera->eye[1] = result[1];
		camera->eye[2] = result[2];

		caculate(eyeEnd, result);
		camera->dir[0] = result[0] - camera->eye[0];
		camera->dir[1] = result[1] - camera->eye[1];
		camera->dir[2] = result[2] - camera->eye[2];
	}
	else {
		cameramove();
	}
	if (gameprog.takeoff) {
		isFirstView = true;
		if (gameprog.globaltimer >= 0 && gameprog.globaltimer <= 200) {
			obPlane->translate(0, 0, -80);
			obFly->translate(0, 0, -80);
		}
		else if (gameprog.globaltimer <= 235 && gameprog.globaltimer > 200) {
			obPlane->translate(0, 100, -100);
			obPlane->rotate(-0.01, glm::vec3(-1, 0, 0));
			obFly->translate(0, 100, -100);
			obFly->rotate(-0.01, glm::vec3(-1, 0, 0));
		}
		else if (gameprog.globaltimer > 235 && gameprog.globaltimer <= 270) {
			obPlane->translate(0, 0, -200);
			obPlane->rotate(0.01, glm::vec3(-1, 0, 0));
			obFly->translate(0, 0, -200);
			obFly->rotate(0.01, glm::vec3(-1, 0, 0));
		}
		else if (gameprog.globaltimer > 270 && gameprog.globaltimer <= 350) {
			obPlane->translate(0, 0, -200);
			obFly->translate(0, 0, -200);
		}
		else if (gameprog.globaltimer > 350) {
			gameprog.control = true;
			gameprog.takeoff = false;
		}
		gameprog.globaltimer++;
	}
	else if (gameprog.control) {
		if (keyBoard->getKey((keyMap)'l') == true)
		{
			obFly->translate(-50, 0, 0);
			obFly->rotate(0.05, glm::vec3(0, 1, 0));
		}
		if (keyBoard->getKey((keyMap)'j') == true)
		{
			obFly->translate(50, 0, 0);
			obFly->rotate(-0.05, glm::vec3(0, 1, 0));
		}
		if (keyBoard->getKey((keyMap)'i') == true)
		{
			obFly->translate(0, 0, -80);
		}
		if (keyBoard->getKey((keyMap)'k') == true)
		{
			obFly->translate(0, 0, 80);
		}
		if (keyBoard->getKey((keyMap)'u') == true)
		{
			obFly->translate(0, 50, 0);
		}
		if (keyBoard->getKey((keyMap)'o') == true)
		{
			obFly->translate(0, -50, 0);
		}
		if (keyBoard->getKey((keyMap)'m') == true)
		{
			obFly->rotate(0.03, glm::vec3(1, 0, 0));
		}
		if (keyBoard->getKey((keyMap)'.') == true)
		{
			obFly->rotate(-0.03, glm::vec3(1, 0, 0));
		}
		if (keyBoard->getKey((keyMap)'z') == true)
		{
			isFirstView = !isFirstView;
		}
		if (!gameprog.launch) {
			if (keyBoard->getKey((keyMap)'j') == true)
			{
				obPlane->translate(-50, 0, 0);
				obPlane->rotate(0.05, glm::vec3(0, 1, 0));
				obFly->translate(-50, 0, 0);
				obFly->rotate(0.05, glm::vec3(0, 1, 0));
			}
			if (keyBoard->getKey((keyMap)'l') == true)
			{
				obPlane->translate(50, 0, 0);
				obPlane->rotate(-0.05, glm::vec3(0, 1, 0));
				obFly->translate(50, 0, 0);
				obFly->rotate(-0.05, glm::vec3(0, 1, 0));
			}
			if (keyBoard->getKey((keyMap)'k') == true)
			{
				obPlane->translate(0, 0, -80);
				obFly->translate(0, 0, -80);
			}
			if (keyBoard->getKey((keyMap)'i') == true)
			{
				obPlane->translate(0, 0, 80);
				obFly->translate(0, 0, 80);
			}
			if (keyBoard->getKey((keyMap)'u') == true)
			{
				obPlane->translate(0, 50, 0);
				obFly->translate(0, 50, 0);
			}
			if (keyBoard->getKey((keyMap)'o') == true)
			{
				obPlane->translate(0, -50, 0);
				obFly->translate(0, 50, 0);
			}
			if (keyBoard->getKey((keyMap)'m') == true)
			{
				obPlane->rotate(0.03, glm::vec3(1, 0, 0));
				obFly->rotate(0.03, glm::vec3(1, 0, 0));
			}
			if (keyBoard->getKey((keyMap)'.') == true)
			{
				obPlane->rotate(-0.03, glm::vec3(1, 0, 0));
				obFly->rotate(-0.03, glm::vec3(1, 0, 0));
			}
		}
		if (keyBoard->getKey((keyMap)' ') == true)
		{
			gameprog.launch = true;
			gameprog.gameover = true;
		}
	}
	if (gameprog.gameover)
	{
		if (gameprog.globaltimer <= 400 && gameprog.globaltimer > 350) {
			obPlane->translate(0, 100, -150);
			obPlane->rotate(-0.01, glm::vec3(-1, 0, 0));
		}
		else if (gameprog.globaltimer >= 400 && gameprog.globaltimer <= 550)
		{
			obPlane->translate(0, 0, -200);
		}
		else if (gameprog.globaltimer > 550)
		{
			gameprog.gameover = false;
		}
		gameprog.globaltimer++;
	}
	model = obFly->getModel();
	testCollision(fly, 0);
	model = obPlane->getModel();
	testCollision(flyPC, 0);
	testCollision(flyPC, 1);
	dealEye();
	for (int i = 0; i < objV.size(); i++) {

		if (fly.check_collision(objV[i]))
			//cout << "daodan pengzhuang:" << i << endl;
		if (flyPC.check_collision(objV[i])
			|| flyPC.check_collision(objV[i], 1)) {
			//cout << "feiji penghzuang" << i << endl;
		}
		if (cameraEye.check_collision(objV[i])) {
			//cout << "xiangji pengzhuang" << i << endl;
		}
	}
	if (flyPC.check_collision(cameraEye)
		|| flyPC.check_collision(cameraEye, 1)) {
		//cout << "xiangji pengzhuang feiji" << endl;
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


void dealBox() {
	char boxFileName[50];
	strcpy(boxFileName, "city//tem//6//test6.obj");
	for (int i = 1; i <= 8; i++) {
		object obj1;
		boxFileName[11] = i + '0';
		boxFileName[18] = i + '0';
		obj1.load(boxFileName);
		obj1.scale(0.05, 0.05, 0.05);
		collosion temC;
		temC.gen_obb_box(obj1.returnPos());
		temC.div20();
		objV.push_back(temC);
	}
}

void build() {
	static scene tmp;

	tmp.push_back((new texture())->load("city//test3.obj")->scale(0.05, 0.05, 0.05));
	obFly = new texture();
	obFly->load("fly//flyD.obj");
	obFly->rotate(-2.36, glm::vec3(0, 1, 0));
	obFly->translate(550, 0, 1500);
	obFly->scale(0.05, 0.05, 0.05);

	obMissile = new object();
	obMissile->load("fly//flyD.obj");
	obMissile->rotate(-2.36, glm::vec3(0, 1, 0));
	obMissile->translate(550, 0, 1500);
	fly.setPos(obMissile->returnPos());
	fly.gen_obb_box(obMissile->returnPos());

	tmp.push_back(obFly);
	fly.initTop();
	obPlane = new texture();
	obPlane->load("fly//flyP.obj");
	obPlane->rotate(-2.36, glm::vec3(0, 1, 0));
	obPlane->translate(550, 0, 1500);

	flyPC.initTopP();
	obPlane->scale(0.05, 0.05, 0.05);
	tmp.push_back(obPlane);

	dealBox();

	//testCloud.initParticle = initParticle;
	//testCloud.isDead = isDead;

	//testCloud.maxSize = 4000;

	//testCloud.init();
	//tmp.push_back(&testCloud);

	gameprog.globaltimer = 0;
	gameprog.thebeginning = true;
	gameprog.takeoff = false;
	gameprog.control = false;
	gameprog.gameover = false;
	gameprog.launch = false;
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
	if (cAscii == 'a' || cAscii == 's' || cAscii == 'd' || cAscii == 'w' || cAscii == 'q' || cAscii == 'e' || cAscii == 'j' || cAscii == 'k' || cAscii == 'l' || cAscii == 'i'
		|| cAscii == 'f' || cAscii == 'g' || cAscii == 'h' || cAscii == 't' || cAscii == 'u' || cAscii == 'o' || cAscii == ' ' || cAscii == 'm' || cAscii == '.' || cAscii == '0'
		|| cAscii == 'z')
	{
		keyBoard->getKey((keyMap)cAscii) = true;
	}
}
static void smKeyUp(unsigned char cAscii, int x, int y)
{
	if (cAscii == 'a' || cAscii == 's' || cAscii == 'd' || cAscii == 'w' || cAscii == 'q' || cAscii == 'e' || cAscii == 'j' || cAscii == 'k' || cAscii == 'l' || cAscii == 'i'
		|| cAscii == 'f' || cAscii == 'g' || cAscii == 'h' || cAscii == 't' || cAscii == 'u' || cAscii == 'o' || cAscii == ' ' || cAscii == 'm' || cAscii == '.' || cAscii == '0'
		|| cAscii == 'z')
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
