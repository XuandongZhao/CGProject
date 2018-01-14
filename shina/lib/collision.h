#ifndef COLLISION_H
#define COLLISION_H
#include"model.h"

#ifndef  M_PI
#define M_PI
const float PI = 3.14159;
#endif // ! M_PI


struct obb_box {
	glm::vec3 center;//中心点
	glm::vec3 half;//半长
	glm::vec3 x_axis, y_axis, z_axis;//坐标轴

	vector<glm::vec3>getPosition()
	{
		vector<glm::vec3>pos;
		pos.resize(8);

		pos[0] = -this->half.x * this->x_axis - this->half.y * this->y_axis - this->half.z * this->z_axis + center;
		pos[1] = -this->half.x * this->x_axis - this->half.y * this->y_axis + this->half.z * this->z_axis + center;
		pos[2] = -this->half.x * this->x_axis + this->half.y * this->y_axis - this->half.z * this->z_axis + center;
		pos[3] = -this->half.x * this->x_axis + this->half.y * this->y_axis + this->half.z * this->z_axis + center;
		pos[4] = this->half.x * this->x_axis - this->half.y * this->y_axis - this->half.z * this->z_axis + center;
		pos[5] = this->half.x * this->x_axis - this->half.y * this->y_axis + this->half.z * this->z_axis + center;
		pos[6] = this->half.x * this->x_axis + this->half.y * this->y_axis - this->half.z * this->z_axis + center;
		pos[7] = this->half.x * this->x_axis + this->half.y * this->y_axis + this->half.z * this->z_axis + center;
		return pos;
	}
};

class collosion {
private:
	/*
	功能：雅各比计算特征值 3*3
	返回: void
	参数:
	a: 原始矩阵 返回为特征值
	p: 特征向量
	eps: 精度
	T: 迭代次数
	*/
	void jacobi(glm::mat3 &a, glm::mat3 &p, double eps, int t);
	/*
	功能：施密特正交化 3*3
	返回：void
	参数：
	p:特征向量矩阵
	a:特征值
	*/
	void schmidtOrthogonal(glm::mat3 &p, glm::vec3 &a);
	/*
	功能：计算协方差矩阵 3*3
	返回：协方差矩阵
	参数：
	vertexs：顶点数组
	*/
	glm::mat3 cal_cov_mat(const std::vector<glm::vec3>& vertexs);
	/*
	功能：生成obb盒 private
	返回：obb盒结构
	参数：
	vertexs：定点坐标
	p：归一化的坐标系
	*/
	obb_box __gen_obb_box(const std::vector<glm::vec3> vertexs, const glm::mat3 &p);
public:
	obb_box a;
	obb_box a2;
	std::vector<glm::vec3> posRe;
	std::vector<glm::vec3> topR;
	std::vector<glm::vec3> topR2;
	std::vector<glm::vec3> top;
	std::vector<glm::vec3> top2;
	collosion() {};
	collosion(char* filePath) {
		object obj1;
		obj1.load(filePath);
		obj1.scale(0.05, 0.05, 0.05);
		gen_obb_box(obj1.returnPos());
	}
	void setPos(const std::vector<glm::vec3>& vertexs) {
		posRe = vertexs;
	}
	void div20() {
		a.center /= 20;
		a.half /= 20;
	}

	void initTopP() {

		float temV[3];
		float temV1[3] = { 789.8995,22.1202,170.0430 };
		top.push_back(glm::make_vec3(temV1));
		float temV2[3] = { 806.1168,22.1202,153.6148 };
		top.push_back(glm::make_vec3(temV2));
		float temV3[3] = { 812.7761,22.1202,160.4098 };
		top.push_back(glm::make_vec3(temV3));
		float temV8[3] = { 796.5589,22.1202,176.8380 };
		top.push_back(glm::make_vec3(temV8));
		float temV4[3] = { 789.8995,29.3808,170.0430 };
		top.push_back(glm::make_vec3(temV4));

		float temV5[3] = { 796.5589,29.3808,176.8380 };
		top.push_back(glm::make_vec3(temV5));
		float temV6[3] = { 812.7761,29.3808,160.4098 };
		top.push_back(glm::make_vec3(temV6));
		float temV7[3] = { 806.1168,29.3808,153.6148 };
		top.push_back(glm::make_vec3(temV7));
		topR = top;

		float temV11[3] = { 789.8995,22.1202,170.0430 };
		top2.push_back(glm::make_vec3(temV1));
		float temV21[3] = { 806.1168,22.1202,153.6148 };
		top2.push_back(glm::make_vec3(temV2));
		float temV31[3] = { 812.7761,22.1202,160.4098 };
		top2.push_back(glm::make_vec3(temV3));
		float temV81[3] = { 796.5589,22.1202,176.8380 };
		top2.push_back(glm::make_vec3(temV8));
		float temV41[3] = { 789.8995,29.3808,170.0430 };
		top2.push_back(glm::make_vec3(temV4));

		float temV51[3] = { 796.5589,29.3808,176.8380 };
		top2.push_back(glm::make_vec3(temV5));
		float temV61[3] = { 812.7761,29.3808,160.4098 };
		top2.push_back(glm::make_vec3(temV6));
		float temV71[3] = { 806.1168,29.3808,153.6148 };
		top2.push_back(glm::make_vec3(temV7));
		topR2 = top2;
	}

	void initTop() {
		float temV[3];
		for (int i = 0; i<3; i++)
			temV[i] = a.center[i] + a.half[i];
		top.push_back(glm::make_vec3(temV));
		temV[0] = a.center[0] - a.half[0];
		temV[1] = a.center[1] + a.half[1];
		temV[2] = a.center[2] + a.half[2];
		top.push_back(glm::make_vec3(temV));

		temV[0] = a.center[0] + a.half[0];
		temV[1] = a.center[1] - a.half[1];
		temV[2] = a.center[2] + a.half[2];
		top.push_back(glm::make_vec3(temV));

		temV[0] = a.center[0] + a.half[0];
		temV[1] = a.center[1] + a.half[1];
		temV[2] = a.center[2] - a.half[2];
		top.push_back(glm::make_vec3(temV));

		temV[0] = a.center[0] - a.half[0];
		temV[1] = a.center[1] - a.half[1];
		temV[2] = a.center[2] + a.half[2];
		top.push_back(glm::make_vec3(temV));

		temV[0] = a.center[0] + a.half[0];
		temV[1] = a.center[1] - a.half[1];
		temV[2] = a.center[2] - a.half[2];
		top.push_back(glm::make_vec3(temV));

		temV[0] = a.center[0] - a.half[0];
		temV[1] = a.center[1] + a.half[1];
		temV[2] = a.center[2] - a.half[2];
		top.push_back(glm::make_vec3(temV));

		temV[0] = a.center[0] - a.half[0];
		temV[1] = a.center[1] - a.half[1];
		temV[2] = a.center[2] - a.half[2];
		top.push_back(glm::make_vec3(temV));
		topR = top;
	}
	/*
	功能: 生成obb盒子 public
	返回：obb盒结构
	参数：
	vertexs : 定点坐标
	*/
	obb_box gen_obb_box(const std::vector<glm::vec3>& vertexs, int type = 0);
	/*
	功能：检测物体是否碰撞
	返回：布尔值
	参数：
	两个obb盒结构
	*/
	bool check_collision(collosion obj2, int type = 0);
};
#endif // !COLLISION_H

