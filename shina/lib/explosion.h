#pragma once
#include"model.h"
//随机数
static double Explosion_randNumber[10000];
//爆炸盒
static double Explosion_left = -0.3, Explosion_right = 0.3, Explosion_up = 50, Explosion_down = 49.4, Explosion_front = 0.3, Explosion_back = -0.3;
//爆炸距离
static double Explosion_xdis = 0.6, Explosion_ydis = 0.6, Explosion_zdis = 0.6;
static double Explosion_x = 0, Explosion_y = 49.7, Explosion_z = 0;
static double Explosion_r = 0.5;
//最大粒子数
static int Explosion_maxSize = 60;
static glm::vec3 Explosion_initialColor(0.2f, 0.2f, 0.2f);

void up(particle * m,const char* name);
void down(particle * m,const char* name);
void right(particle * m, const char* name);
void left(particle * m,const char* name);
void front(particle * m,const char* name);
void back(particle * m,const char* name);
void randInit();
void ExplosionInit(cloud explosion[], int max, int time);
void ExplosionPos(double x_left, double x_right, double y_up, double y_down, double z_front, double z_back);