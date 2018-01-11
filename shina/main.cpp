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
#include "lib\explosion.h"
#include "lib\fluid.h"
#include "lib\halfCross.h"
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
Fluid *fuild;
halfPlaneCross halfCollition;
rectangle* withCamera;

cubeBox*skyBox;


glm::vec3 cameraPosition(0.f, 20.f, 0.f);
glm::vec3 cameraDir(0, 0, -30);

const float fireX = -3953.31, fireY = 731.094, fireZ = 8089.3;
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

const glm::vec4 flyVec(41.89, 25.342, -3017.63, 1.0);


void initParticle(particle * m, const char*name)
{
	if (m->rec == nullptr)
	{
		m->rec = new rectangle();

		m->rec->setShape(0.2, 0.6);
		m->rec->fill(name);
		m->which = IS_RECTANGLE;
		m->initialColor = initialColor;
		m->fadeColor = fadeColor;
		//m->sphere = new Sphere(0.05, 45, glm::vec4(1.f, 0.f, 0.f, 1.f));
		//m->which = IS_SPHERE;
	}
	//cout << "fuck" << endl;
	int random = rand() % 10000;
	m->isDead = false;
	glm::vec4 firePosition = (obFly->getModel())*glm::vec4(fireX + randNumber[random] * 200 - 100, fireY + randNumber[(random * 2) % 10000] * 200 - 100, fireZ, 1.f);
	//firePosition = (obFly->getModel())[0] * firePosition;

	//for (int i = 0; i < 4; i++)
	//{
	//	for (int j = 0; j < 4; j++)
	//	{
	//		cout << (obFly->getModel())[i][j]<<"***";
	//	}
	//	cout << endl;
	//}


	glm::vec4 fireVec = (obFly->getModel())*flyVec;
	glm::vec4 yuan = (obFly->getModel())*glm::vec4(0, 0, 0, 1);
	//multiVec4((obFly->getModel()), flyVec, fireVec);
	//cout << endl;
	//cout << fireVec.x << " " << fireVec.y << " " << fireVec.z << " " << fireVec.w << endl;
	//cout << yuan.x << " " << yuan.y << " " << yuan.z << " " << yuan.w << endl;
	fireVec = yuan - fireVec;



	fireVec /= sqrt(fireVec.x*fireVec.x + fireVec.y*fireVec.y + fireVec.z*fireVec.z);
	fireVec *= (randNumber[(random * 4) % 10000] * 20);
	//cout << fireVec.x << " " << fireVec.y << " " << fireVec.z << endl;

	float x = firePosition.x;// +randNumber[random] * 200 - 100;
	float y = firePosition.y;
	float z = firePosition.z;// +randNumber[(random * 2) % 10000] * 0.6 - 0.3;

	float pos[3] = { x,y,z };


	//float speed[3] = { 0,0,randNumber[(random * 3) % 10000] * 10 };
	float aspeed[3] = { 0,0,0 };
	m->setPosition(pos[0], pos[1], pos[2]);
	m->setSpeed(fireVec.x, fireVec.y, fireVec.z);
	m->setAccerator(aspeed[0], aspeed[1], aspeed[2]);

	m->setAngle(randNumber[(random * 4) % 10000] * 3, randNumber[(random * 5) % 10000] * 3, randNumber[(random * 6) % 10000] * 3);
	(m->rec)->color = glm::vec4(initialColor, 1.f);


	m->lifetime = randNumber[(random * 8) % 10000] * 3;
	m->fullLife = m->lifetime;
	m->deci = 0.05;

}
/*
@return true for dead
*/



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
	skyBox = new cubeBox();
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
bool isFirstView = false;
glm::vec3 eyeStart;
int count = 0;
glm::vec3 eyeEnd;

void cameramove()
{
	if (keyBoard->getKey((keyMap)'d') == true)
	{
		camera->moveCamera(-40, 0);
		withCamera->translate(40, 0, 0);
		
	}
	if (keyBoard->getKey((keyMap)'a') == true)
	{

		camera->moveCamera(40, 0);
		withCamera->translate(-40, 0, 0);
	}
	if (keyBoard->getKey((keyMap)'s') == true)
	{

		camera->moveCamera(0, -40);
		withCamera->translate(0, 0, 40);
	}
	if (keyBoard->getKey((keyMap)'w') == true)
	{
		camera->moveCamera(0, 40);
		withCamera->translate(0, 0, -40);
	}

	if (keyBoard->getKey((keyMap)'q') == true)
	{
		int ans = halfCollition.calc();
		cout << ans << endl;
		camera->moveHCamera(30);
		withCamera->translate(0, 30, 0);
	}
	if (keyBoard->getKey((keyMap)'e') == true)
	{
		int ans = halfCollition.calc();
		cout << ans << endl;
		camera->moveHCamera(-30);
		withCamera->translate(0, -30, 0);
	}

	if (keyBoard->getKey((keyMap)'j') == true)
	{
		int ans = halfCollition.calc();
		cout << ans << endl;
		obFly->translate(-25, 0, 0);
		obFly->rotate(0.02, glm::vec3(0, 1, 0));
	}
	if (keyBoard->getKey((keyMap)'l') == true)
	{
		int ans = halfCollition.calc();
		cout << ans << endl;
		obFly->translate(25, 0, 0);
		obFly->rotate(-0.02, glm::vec3(0, 1, 0));
	}
	if (keyBoard->getKey((keyMap)'i') == true)
	{
		int ans = halfCollition.calc();
		cout << ans << endl;
		obFly->translate(0, 0, -70);
	}
	if (keyBoard->getKey((keyMap)'k') == true)
	{
		int ans = halfCollition.calc();
		cout << ans << endl;
		obFly->translate(0, 0, 70);
	}
	if (keyBoard->getKey((keyMap)'u') == true)
	{
		int ans = halfCollition.calc();
		cout << ans << endl;
		obFly->translate(0, 50, 0);
	}
	if (keyBoard->getKey((keyMap)'o') == true)
	{
		int ans = halfCollition.calc();
		cout << ans << endl;
		obFly->translate(0, -50, 0);
	}
	if (keyBoard->getKey((keyMap)'m') == true)
	{
		int ans = halfCollition.calc();
		cout << ans << endl;
		obFly->rotate(0.03, glm::vec3(1, 0, 0));
	}
	if (keyBoard->getKey((keyMap)'.') == true)
	{
		int ans = halfCollition.calc();
		cout << ans << endl;
		obFly->rotate(-0.03, glm::vec3(1, 0, 0));
	}

}

static void smTimer(int id)
{
	fuild->update();
	cameramove();
	glutTimerFunc(100, smTimer, id);
	glutPostRedisplay();
}

static void smDisplay() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	render.render(myworld, *camera);
	withCamera->show();
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

	vector<string>path;
	path.push_back("source//skybox//skyFront.obj");
	path.push_back("source//skybox//skyBack.obj");
	path.push_back("source//skybox//skyLeft.obj");
	path.push_back("source//skybox//skyRight.obj");
	path.push_back("source//skybox//skyTop.obj");
	path.push_back("source//skybox//skyBottom.obj");
	skyBox->init(path);
	tmp.push_back(skyBox);

	texture* city = new texture();

	city->load("city//newshanghai//newshanghai.obj");
	//city->load("city//lzj//shanghai.obj");
	for (auto &i : city->group)
	{
		bool ok = false;
		cout << i.material.name << endl;
		cout << i.material.name.find("house") << endl;
		if (i.material.name.find("house") == string::npos)
		{
			continue;
		}

		vector<glm::vec3>input;
		for (int j = 0; j < i.pos.size(); j += 3)
		{
			input.push_back(glm::vec3(i.pos[j],i.pos[j+1], i.pos[j + 2]));
			//cout << i.pos[j] << " " << i.pos[j + 1] << " " << i.pos[j + 2] << endl;
		}
		halfCollition.addConvexHull(input);

	}

	withCamera = new rectangle();
	withCamera->fill("source//water.bmp");
	vector<glm::vec3>testPosition;
	testPosition.push_back(glm::vec3(-10, -10, -10));
	testPosition.push_back(glm::vec3(-10, 10, -10));
	testPosition.push_back(glm::vec3(10, 10, -10));
	testPosition.push_back(glm::vec3(10, -10, -10));
	testPosition.push_back(glm::vec3(-10, -10, 10));
	testPosition.push_back(glm::vec3(-10, 10, 10));
	testPosition.push_back(glm::vec3(10, 10, 10));
	testPosition.push_back(glm::vec3(10, -10, 10));
	//withCamera->scale(100,100,100);
	





	tmp.push_back(city);
	obFly = new texture();
	obFly->load("fly//flyD.obj");
	obFly->rotate(-2.36, glm::vec3(0, 1, 0));
	//obFly->translate(550, 0, 2000);
	obFly->scale(0.05, 0.05, 0.05);
	

	obMissile = new object();
	obMissile->load("fly//flyD.obj");
	obMissile->rotate(-2.36, glm::vec3(0, 1, 0));
	obMissile->translate(550, 0, 2000);
	fly.setPos(obMissile->returnPos());
	obb_box flyBox=fly.gen_obb_box(obMissile->returnPos());
	vector<glm::vec3>pos=flyBox.getPosition();
	cout << "DaoDan" << endl;
	for (int i = 0; i < 8; i++)
	{
		cout << pos[i].x << " " << pos[i].y << " " << pos[i].z << endl;
	}
	halfCollition.addtestObject(0, pos, &(obFly->model));

	tmp.push_back(obFly);
	fly.initTop();
	obPlane = new texture();
	obPlane->load("fly//flyP.obj");
	obPlane->rotate(-2.36, glm::vec3(0, 1, 0));
	obPlane->translate(550, 0, 2000);

	flyPC.initTopP();
	obPlane->scale(0.05, 0.05, 0.05);
	tmp.push_back(obPlane);

	dealBox();


	gameprog.globaltimer = 0;
	gameprog.thebeginning = true;
	gameprog.takeoff = false;
	gameprog.control = false;
	gameprog.gameover = false;
	gameprog.launch = false;

	testCloud.initParticle = initParticle;

	testCloud.maxSize = 2000;
	testCloud.init();
	tmp.push_back(&testCloud);


	fuild = new Fluid(100, 100, 200, 1, 40, 0, "source//water.bmp");
	tmp.push_back(fuild);
	fuild->translate(-3000, -60, 3000);
	fuild->rotate(PI / 2, glm::vec3(-1, 0, 0));

	// sky
	tmp.push_back((new texture())->load("source//skyFront.obj"));

	myworld.push_back(&tmp);
	myworld.push_back(new smLight(0, glm::vec3(55.f, 250.f, 23.f), glm::vec3(.7f, .7f, .7f), glm::vec3(.2f, .2f, .2f), 1.f));
	//myworld.push_back(new smLight(1, glm::vec3(0.f, 50.f, 0.f), glm::vec3(.7f, .7f, .7f), glm::vec3(.2f, .2f, .2f), 1.f));
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
