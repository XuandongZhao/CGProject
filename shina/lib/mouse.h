#pragma once
#ifndef SHINA_LIB_MOUSE_H
#define SHINA_LIB_MOUSE_H
#include "mouse.h"
class smMouse {
public:
	using vec2 = glm::vec2;
	int state[3];
	vec2 pre, now;
public:
	smMouse()
	{
		state[0]=state[1]=state[2]=1;
	}
};


#endif