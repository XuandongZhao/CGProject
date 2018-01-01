#ifndef SHINA_LIB_MAIN_H
#define SHINA_LIB_MAIN_H

#define _CRT_SECURE_NO_WARNINGS
//remind to use radians
#define GLM_FORCE_RADIANS
#define FREEGLUT_STATIC

#include <cassert>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <list>
#include <map>
#include <ctime>
#include <glfw/glew.h>
#include <gl/glaux.h> 
#include <gl/freeglut.h>

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifndef SHINA_Lib
#ifdef _DEBUG
#define MY_Lib(name) name "d.lib"
#else
#define MY_Lib(name) name ".lib"
#endif
#endif
#pragma comment(lib, MY_Lib("freeglut_static"))
#pragma comment(lib,"glew32.lib")



#define _KEY_F1 0x01
#define _KEY_F2 0x02
#define _KEY_F3 0x03
#define _KEY_F4 0x04
#define _KEY_F5 0x05
#define _KEY_F6 0x06
#define _KEY_F7 0x07
#define _KEY_F8 0x08
#define _KEY_F9 0x09
#define _KEY_F10 0x0A
#define _KEY_F11 0x0B
#define _KEY_F12 0x0C
#define _KEY_LEFT 0x64
#define _KEY_UP 0x65
#define _KEY_RIGHT 0x66
#define _KEY_DOWN 0x67
#define _KEY_PAGEUP 0x68
#define _KEY_PAGEDOWN 0x69
#define _KEY_HOME 0x6A
#define _KEY_END 0x6B
#define _KEY_INSERT 0x6C
#define _KEY_SHIFT 0x70
#define _KEY_CTRL 0x72
#define _KEY_ALT 0x74


#define IS_SPHERE 0
#define IS_TEXTURE 1


using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::queue;
using std::list;


#ifndef  M_PI
#define M_PI
const float PI = 3.14159;
#endif // ! M_PI

#define SHADOW_WIDTH 8192
#define SHADOW_HEIGHT 8192
extern int SCR_WIDTH;
extern int SCR_HEIGHT;
class scene;
class smCamera;
class smLight;

extern smCamera *camera;



template <typename T>
T *toArray(vector<T> *v) {
	return &((*v)[0]);
}

template <typename T>
T inner(T a, T b, T value)
{
	return value < a ? a : (value > b ? b : value);
}


template <typename T>
T min(T x, T y) {
	return x < y ? x : y;
}
template <typename T>
T max(T x, T y) {
	return x > y ? x : y;
}

template <typename T>
T interpolate(float prop, T x, T y) {
	return x*prop + y*(1 - prop);
}



#endif