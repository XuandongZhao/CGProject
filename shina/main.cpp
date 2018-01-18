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

float explo_x, explo_y, explo_z;
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
smShader* fireShader;

vector<collosion> objV;
collosion fly;
collosion flyPC;

collosion cameraEye;
vector<object> totalObj;
object * flyP;
texture *obFly;
texture *testPlane;
texture *testDao;
texture *obPlane;
object * obMissile;

Fluid *fuild;
cubeBox*skyBox;
halfPlaneCross halfCollition;
rectangle* withCamera;

glm::vec3 cameraPosition(0.f, 20.f, 0.f);
glm::vec3 cameraDir(0, 0, -30);

const float fireX = 1.78550000000007, fireY = -0.0475999999999992, fireZ = -1.727499999999992;
const glm::vec3 fire_l = glm::vec3(7.297400000000039, -1.5888000000000027, -5.379500000000007);
const glm::vec3 fire_r = glm::vec3(5.965800000000058, -1.5888000000000027, -6.905599999999993);

glm::mat4 *daoDanModel;
GLfloat planeSpeed = 1;
GLfloat daoDanSpeed = 5;
GLfloat turnAngle = 0.025;

typedef struct
{
	int globaltimer;
	int hittimer;
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

const glm::vec4 flyVec(-4.027699999999982, 0.3669000000000011, 3.903599999999983, 1.0);

//explosion part
bool planehit = false;
bool daohit = false;
bool hit_once = false;
cloud PtestExplosion;
cloud DtestExplosion;

fire* fire_d;
fire* fire_p_l;
fire* fire_p_r;
fire* fire_explot;

bool isFirst = 0;


void Dcrash()
{
	obFly->hide = true;
	cout << "daodan crash" << endl;
	DtestExplosion.hit_happen();
}

void Pcrash()
{
	obPlane->hide = true;
	obFly->hide = true;
	cout << "plane crash" << endl;
	PtestExplosion.hit_happen();
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
	elementShader = new smShader("files//element.vert", "files//element.frag");
	texShader = new smShader("files//tex.vert", "files//tex.frag");
	shadowShader = new smShader("files//shadow.vert", "files//shadow.frag");
	camera = new smCamera(cameraPosition, cameraDir);
	mouse = new smMouse();
	keyBoard = new smKeyBoard();

	paticleShader = new smShader("files//paticle.vert", "files//paticle.frag");
	texParticleShader = new smShader("files//rectangle.vert", "files//rectangle.frag");
	fireShader = new smShader("files//fire.vert", "files//fire.frag", "files//fire.geo");


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
		temV[0] = ((double)camera->eye[0]) + flag[0] * 10;
		for (int j = 0; j < 2; j++) {
			flag[1] = j - 1;
			temV[1] = (double)(camera->eye[1]) + flag[1] * 10;
			for (int k = 0; k < 2; k++) {
				flag[2] = k - 1;
				temV[2] = (double)(camera->eye[2]) + flag[2] * 10;
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
	float a = 10;
	if (keyBoard->getKey((keyMap)'d') == true)
	{
		camera->moveCamera(-a, 0);

	}
	if (keyBoard->getKey((keyMap)'a') == true)
	{
		camera->moveCamera(a, 0);
	}
	if (keyBoard->getKey((keyMap)'s') == true)
	{
		camera->moveCamera(0, -a);
	}
	if (keyBoard->getKey((keyMap)'w') == true)
	{
		camera->moveCamera(0, a);
	}

	if (keyBoard->getKey((keyMap)'q') == true)
	{
		camera->moveHCamera(a);
	}
	if (keyBoard->getKey((keyMap)'e') == true)
	{
		camera->moveHCamera(-a);
	}

}

glm::vec3 getCenter(std::vector<glm::vec3> top) {
	glm::vec3 center;
	center[0] = 0;
	center[1] = 0;
	center[2] = 0;
	for (int i = 0; i < 8; i++) {
		center[0] += top[i][0];
	}
	center[0] /= 8;
	for (int i = 0; i < 8; i++) {
		center[1] += top[i][1];
	}
	center[1] /= 8;
	for (int i = 0; i < 8; i++) {
		center[2] += top[i][2];
	}
	center[2] /= 8;
	return center;
}

static void smTimer(int id)
{
	//cout << "dao hit" << daohit << endl;
	//cout << "plane hit" << planehit << endl;
	if (daohit == true && hit_once == false || keyBoard->getKey((keyMap)'b') == true)
	{
		cout << "hit" << endl;
		explo_x = fly.a.center.x;
		explo_y = fly.a.center.y + 20;
		explo_z = fly.a.center.z;
		Dcrash();
		hit_once = true;
	}
	if (planehit == true && hit_once == false || keyBoard->getKey((keyMap)'b') == true)
	{
		cout << "hit" << endl;
		explo_x = fly.a.center.x;
		explo_y = fly.a.center.y + 20;
		explo_z = fly.a.center.z;
		Pcrash();
		hit_once = true;
	}

	fuild->update();
	model = obFly->getModel();
	testCollision(fly, 0);
	//cout<<"&&&"<<halfCollition.calc()<<endl;
	model = obPlane->getModel();
	testCollision(flyPC, 0);
	testCollision(flyPC, 1);
	glm::vec3 flyC = getCenter(fly.topR);
	glm::vec3 flyPCC = getCenter(flyPC.topR);

	if (gameprog.thebeginning) {
		if (keyBoard->getKey((keyMap)'0') == true)
		{
			gameprog.takeoff = true;
			gameprog.thebeginning = false;
			isFirstView = true;
		}
	}
	else if (gameprog.takeoff) {
		isFirstView = true;
		if (gameprog.globaltimer >= 0 && gameprog.globaltimer <= 50) {
			obPlane->translate(-0.7*planeSpeed, 0, 0.7*planeSpeed);
			obFly->translate(-0.7*planeSpeed, 0, 0.7*planeSpeed);
		}
		else if (gameprog.globaltimer <= 80 && gameprog.globaltimer > 50) {
			obPlane->translate(-0.9*planeSpeed, 0.6*planeSpeed, 0.9*planeSpeed);
			obPlane->rotate(-0.01, glm::vec3(1, 0, 1));
			obFly->translate(-0.9*planeSpeed, 0.6*planeSpeed, 0.9*planeSpeed);
			obFly->rotate(-0.01, glm::vec3(1, 0, 1));
		}
		else if (gameprog.globaltimer > 80 && gameprog.globaltimer <= 110) {
			obPlane->translate(-0.9*planeSpeed, 0.6*planeSpeed, 0.9*planeSpeed);
			obPlane->rotate(0.01, glm::vec3(1, 0, 1));
			obFly->translate(-0.9*planeSpeed, 0.6*planeSpeed, 0.9*planeSpeed);
			obFly->rotate(0.01, glm::vec3(1, 0, 1));
		}
		else if (gameprog.globaltimer > 110) {
			gameprog.control = true;
			gameprog.takeoff = false;
		}
		gameprog.globaltimer++;
	}
	else if (gameprog.control) {
		obFly->translate(-planeSpeed, 0, planeSpeed);
		if (keyBoard->getKey((keyMap)'j') == true)
		{
			obFly->rotate(turnAngle, glm::vec3(0, 1, 0));
		}
		if (keyBoard->getKey((keyMap)'l') == true)
		{
			obFly->rotate(-turnAngle, glm::vec3(0, 1, 0));
		}
		if (keyBoard->getKey((keyMap)'i') == true)
		{
			obFly->translate(-planeSpeed, 0, planeSpeed);
		}
		if (keyBoard->getKey((keyMap)'k') == true)
		{
			obFly->translate(planeSpeed, 0, -planeSpeed);
		}
		if (keyBoard->getKey((keyMap)'u') == true)
		{
			obFly->translate(0, 0.5*planeSpeed, 0);
		}
		if (keyBoard->getKey((keyMap)'o') == true)
		{
			obFly->translate(0, -planeSpeed, 0);
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
		if (keyBoard->getKey((keyMap)' ') == true)
		{
			gameprog.launch = true;
			gameprog.gameover = true;
		}
		if (!gameprog.launch) {
			obPlane->translate(-planeSpeed, 0, planeSpeed);
			if (keyBoard->getKey((keyMap)'j') == true)
			{
				obPlane->rotate(turnAngle, glm::vec3(0, 1, 0));
			}
			if (keyBoard->getKey((keyMap)'l') == true)
			{
				obPlane->rotate(-turnAngle, glm::vec3(0, 1, 0));
			}
			if (keyBoard->getKey((keyMap)'i') == true)
			{
				obPlane->translate(-planeSpeed, 0, planeSpeed);
			}
			if (keyBoard->getKey((keyMap)'k') == true)
			{
				obPlane->translate(planeSpeed, 0, -planeSpeed);
			}
			if (keyBoard->getKey((keyMap)'u') == true)
			{
				obPlane->translate(0, 0.5*planeSpeed, 0);
			}
			if (keyBoard->getKey((keyMap)'o') == true)
			{
				obPlane->translate(0, -planeSpeed, 0);
			}
			if (keyBoard->getKey((keyMap)'m') == true)
			{
				obPlane->rotate(0.03, glm::vec3(1, 0, 0));

			}
			if (keyBoard->getKey((keyMap)'.') == true)
			{
				obPlane->rotate(-0.03, glm::vec3(1, 0, 0));
			}
		}
	}
	if (gameprog.gameover)
	{
		if (gameprog.globaltimer <= 135 && gameprog.globaltimer > 110) {
			obPlane->translate(-2 * planeSpeed, 2 * planeSpeed, 2 * planeSpeed);
			obPlane->rotate(-0.01, glm::vec3(1, 0, 1));
		}
		if (gameprog.globaltimer > 135)
		{
			obPlane->hide = true;
		}
		gameprog.globaltimer++;
	}
	model = obFly->getModel();
	testCollision(fly, 0);
	model = obPlane->getModel();
	testCollision(flyPC, 0);
	testCollision(flyPC, 1);
	dealEye();



	//碰撞检测
	/*用法：fly：导弹
	flyPC：飞机
	fly.a.center为中心点
	飞机同理
	*/
	int flag = 0;
	for (int i = 0; i < objV.size(); i++) {
		if (fly.check_collision(objV[i]))
		{
			flag = 2;
			
		}
		if (flyPC.check_collision(objV[i])
			|| flyPC.check_collision(objV[i], 1)) {
			flag = 1;
		}
		if (cameraEye.check_collision(objV[i])) {
			cout << "xiangji pengzhuang" << i << endl;
		}
	}

	if (flag == 1 || halfCollition.calc() == 2) {
		//cout << "feiji he daodan dou pengzhang lou le" << endl;
		obPlane->translate(planeSpeed, 0, -planeSpeed);
		obFly->translate(planeSpeed, 0, -planeSpeed);
		fire_d->hide = true;
		fire_p_r->hide = true;
		fire_p_l->hide = true;
		gameprog.hittimer++;
		//cout << "feiji penghzuang" << i << endl;
		planehit = true;
	}
	else if (flag == 2 || halfCollition.calc() == 1) {
		
		//cout << "daodan pengzhuang lou le" << endl;
		obFly->translate(planeSpeed, 0, -planeSpeed);
		gameprog.hittimer++;
		//cout << "daodan pengzhuang:" << i << endl;
		fire_d->hide = true;
		//
		//myworld.popLight();
		//fire_p_r = new fire(20000, (obPlane->model), 1.5, 0.1);
		/*fire_p_r->flyVec = glm::vec4(-19.718799999999987, -0.1495999999999995, 18.768, 1);
		fire_p_r->useFade(initialColor, fadeColor);
		fire_p_r->initFire("source//file.jpg", &fire_r.x, &fire_r.y, &fire_r.z);
		tmp.push_back(fire_p_r);*/
		if (!isFirst)
		{
			myworld.sceneCollection[0]->push_back(fire_explot);
			myworld.push_back(new smLight(1, glm::vec3(-55.f, 250.f, -23.f), glm::vec3(.7f, .7f, .7f), glm::vec3(.2f, .2f, .2f), 1.f));
			//myworld.push_back(new smLight(1,glm::vec3(explo_x,explo_y, explo_z),glm::vec3(.7f, .7f, .7f), glm::vec3(.2f, .2f, .2f), 1.f));
			isFirst = true;
		}
		

		daohit = true;
	}
	if (gameprog.hittimer>30)
	{
		exit(0);
	}
	if (flyPC.check_collision(cameraEye)
		|| flyPC.check_collision(cameraEye, 1)) {

		cout << "xiangji pengzhuang feiji" << endl;

		gameprog.takeoff = true;
		gameprog.thebeginning = false;
		isFirstView = true;
	}
	if (isFirstView) {
		count++;
		if (count == 1) {
			camera->eye[0] = 40.14;//841.02;
			camera->eye[1] = 26.23;//64.03;
			camera->eye[2] = -40.44;//137.6;

			eyeStart = camera->eye;
			eyeEnd = eyeStart;
			eyeEnd[2] += 100;
			eyeEnd[0] -= 100;
		}
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
	strcpy(boxFileName, "city//building//test5.obj");
	for (int i = 1; i <= 5; i++) {
		object obj1;
		boxFileName[20] = i + '0';
		cout << boxFileName << endl;
		obj1.load(boxFileName);
		collosion temC;
		temC.gen_obb_box(obj1.returnPos());
		objV.push_back(temC);
	}
}

void build() {
	static scene tmp;
	skyBox = new cubeBox();
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
	boundBox*cityBox1 = new boundBox();
	boundBox*cityBox2 = new boundBox();
	cityBox1->load("city//cityaround//cityaround//city_box1.obj");
	cityBox2->load("city//cityaround//cityaround//CAD_BOX2.obj");
	for (auto &i : cityBox1->group)
	{
		bool ok = false;
		vector<glm::vec3>input;
		for (int j = 0; j < i.pos.size(); j += 3)
		{
			input.push_back(glm::vec3(i.pos[j], i.pos[j + 1], i.pos[j + 2]));
		}
		halfCollition.addConvexHull(input);
	}
	for (auto &i : cityBox2->group)
	{
		bool ok = false;
		vector<glm::vec3>input;
		for (int j = 0; j < i.pos.size(); j += 3)
		{
			input.push_back(glm::vec3(i.pos[j], i.pos[j + 1], i.pos[j + 2]));
		}
		halfCollition.addConvexHull(input);
	}

	withCamera = new rectangle();
	withCamera->fill("source//water.bmp");
	tmp.push_back(city);

	obFly = new texture();
	obFly->load("city//newmissile.obj");
	obFly->direct_scale(1);
	obFly->translate(800, 21, 171);

	obMissile = new object();
	obMissile->load("city//newmissile.obj");
	obMissile->direct_scale(1);
	obMissile->translate(800, 21, 171);
	fly.setPos(obMissile->returnPos());
	obb_box flyBox = fly.gen_obb_box(obMissile->returnPos());
	vector<glm::vec3>pos = flyBox.getPosition();
	halfCollition.addtestObject(0, pos, &(obFly->model));
	tmp.push_back(obFly);

	testPlane = new texture();
	testPlane->load("city//newplane.obj");
	testPlane->direct_scale(1);
	testPlane->direct_translate(0, -20, 0);
	PtestExplosion.tex2cloud(testPlane);
	PtestExplosion.isDead = NULL;
	tmp.push_back(&PtestExplosion);

	testDao = new texture();
	testDao->load("city//newmissile.obj");
	testDao->direct_scale(1);
	testDao->direct_translate(0, -20, 0);
	DtestExplosion.tex2cloud(testDao);
	DtestExplosion.isDead = NULL;
	tmp.push_back(&DtestExplosion);

	//d=fire with dao dan
	fire_d = new fire(10000, (obFly->model), 1, 0.1);
	daoDanModel = &(obFly->model);
	fire_d->flyVec = flyVec;
	fire_d->useFade(initialColor, fadeColor);

	fire_d->initFire("source//file.jpg", &fireX, &fireY, &fireZ);
	tmp.push_back(fire_d);




	fly.initTop();
	obPlane = new texture();
	obPlane->load("city//newplane.obj");
	obPlane->direct_scale(1);
	obPlane->translate(800, 24, 171);



	flyPC.initTopP();
	pos = flyPC.top;
	halfCollition.addtestObject(1, pos, &(obPlane->model));
	tmp.push_back(obPlane);


	fire_p_l = new fire(20000, (obPlane->model), 1.5, 0.1);
	fire_p_l->flyVec = glm::vec4(-19.718799999999987, -0.1495999999999995, 18.768, 1);
	fire_p_l->useFade(initialColor, fadeColor);
	fire_p_l->initFire("source//file.jpg", &fire_l.x, &fire_l.y, &fire_l.z);
	tmp.push_back(fire_p_l);

	fire_p_r = new fire(20000, (obPlane->model), 1.5, 0.1);
	fire_p_r->flyVec = glm::vec4(-19.718799999999987, -0.1495999999999995, 18.768, 1);
	fire_p_r->useFade(initialColor, fadeColor);
	fire_p_r->initFire("source//file.jpg", &fire_r.x, &fire_r.y, &fire_r.z);
	tmp.push_back(fire_p_r);

	dealBox();


	gameprog.globaltimer = 0;
	gameprog.thebeginning = true;
	gameprog.takeoff = false;
	gameprog.control = false;
	gameprog.gameover = false;
	gameprog.launch = false;


	fuild = new Fluid(100, 100, 200, 1, 40, 0, "source//water.bmp");
	tmp.push_back(fuild);
	fuild->translate(-3000, -70, 3000);
	fuild->rotate(PI / 2, glm::vec3(-1, 0, 0));

	// sky
	tmp.push_back((new texture())->load("source//skyFront.obj"));

	myworld.push_back(&tmp);
	//myworld.push_back(new smLight(1, glm::vec3(-55.f, 250.f, -23.f), glm::vec3(.7f, .7f, .7f), glm::vec3(.2f, .2f, .2f), 1.f));
	myworld.push_back(new smLight(0, glm::vec3(55.f, 250.f, 23.f), glm::vec3(.7f, .7f, .7f), glm::vec3(.2f, .2f, .2f), 1.f));
	//myworld.push_back(new smLight(1, glm::vec3(0.f, 50.f, 0.f), glm::vec3(.7f, .7f, .7f), glm::vec3(.2f, .2f, .2f), 1.f));

	fire_explot = new fire(20000, (obFly->model), 1.5, 0.1);
	fire_explot->flyVec = glm::vec4(-1, -5, -1, 1);
	fire_explot->useFade(initialColor, fadeColor);
	fire_explot->initFire("source//file.jpg", &explo_x, &explo_y, &explo_z);
	//tmp.push_back(fire_explot);
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
		camera->rotateCamera((-mouse->pre.x + float(x)) / 400, (-mouse->pre.y + float(y)) / 400);
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
		|| cAscii == 'z' || cAscii == 'b')
	{
		keyBoard->getKey((keyMap)cAscii) = true;
	}
}
static void smKeyUp(unsigned char cAscii, int x, int y)
{
	if (cAscii == 'a' || cAscii == 's' || cAscii == 'd' || cAscii == 'w' || cAscii == 'q' || cAscii == 'e' || cAscii == 'j' || cAscii == 'k' || cAscii == 'l' || cAscii == 'i'
		|| cAscii == 'f' || cAscii == 'g' || cAscii == 'h' || cAscii == 't' || cAscii == 'u' || cAscii == 'o' || cAscii == ' ' || cAscii == 'm' || cAscii == '.' || cAscii == '0'
		|| cAscii == 'z' || cAscii == 'b')
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