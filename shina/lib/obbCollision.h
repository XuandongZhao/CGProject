#ifndef COLLISION_H
#define COLLISION_H
#include <glm\glm.hpp>
#include <vector>
const float PI = 3.14159;

struct obb_box {
	glm::vec3 center;//中心点
	glm::vec3 half;//半长
	glm::vec3 x_axis, y_axis, z_axis;//坐标轴
};

class obbCollision {
public:
	obbCollision();
	~obbCollision();
	/*
	功能：雅各比计算特征值 3*3
	返回: void
	参数:
	a: 原始矩阵 返回为特征值
	p: 特征向量
	eps: 精度
	T: 迭代次数
	*/
	void jacobi(glm::mat3 &a, glm::mat3 &p, double eps, int T);

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
	obb_box __gen_obb_box(const std::vector<glm::vec3>& vertexs, const glm::mat3 &p);

	/*
	功能: 生成obb盒子 public
	返回：obb盒结构
	参数：
	vertexs : 定点坐标
	*/
	obb_box gen_obb_box(const std::vector<glm::vec3>& vertexs);

	/*
	功能：检测物体是否碰撞
	返回：布尔值
	参数：
	两个obb盒结构
	*/
	bool check_collision(const obb_box& a, const obb_box& b);
};

#endif // !COLLISION_H

