#pragma once
#ifndef SHINA_LIB_RENDER_H
#define SHINA_LIB_RENDER_H
#include "main.h"
#include "camera.h"
#include "model.h"
#include "light.h"
class smRender {
private:
	bool useShadow;

public :
	smRender(bool shadow = true) :useShadow(shadow) {}
	void render(world & myworld, smCamera & camera);

};

#endif
