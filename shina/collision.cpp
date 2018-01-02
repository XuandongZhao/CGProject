#include "lib/collision.h"
#include <cmath>
#include <algorithm>

void collosion::jacobi(glm::mat3 &a, glm::mat3 &p, double eps=0.00001, int t = 10000)
{
	int T = t;
	int i, j, k;
	double max = a[0][1];
	int row = 0;
	int col = 0;
	double Ptemp[3][3];
	int ite_num = 0;
	while (1)
	{
		max = fabs(a[0][1]);
		row = 0;
		col = 1;
		for (i = 0; i < 3; i++)
			for (j = 0; j < 3; j++)
				if (i != j && fabs(a[i][j])>max)
				{
					max = fabs(a[i][j]);
					row = i;
					col = j;
				}
		if (max < eps)
		{
			return ;
		}

		if (ite_num>T)
		{
			return ;
		}

		double theta;
		if (a[row][row] == a[col][col])
			theta = PI / 4;
		else
			theta = 0.5*atan(2 * a[row][col] / (a[row][row] - a[col][col]));
		double aii = a[row][row];
		double ajj = a[col][col];
		double aij = a[row][col];
		double sin_theta = sin(theta);
		double cos_theta = cos(theta);
		double sin_2theta = sin(2 * theta);
		double cos_2theta = cos(2 * theta);
		a[row][row] = aii*cos_theta*cos_theta + ajj*sin_theta*sin_theta + aij*sin_2theta;
		a[col][col] = aii*sin_theta*sin_theta + ajj*cos_theta*cos_theta - aij*sin_2theta;
		a[row][col] = 0.5*(ajj - aii)*sin_2theta + aij*cos_2theta;
		a[col][row] = a[row][col];
		for (k = 0; k < 3; k++)
		{
			if (k != row && k != col)
			{
				double arowk = a[row][k];
				double acolk = a[col][k];
				a[row][k] = arowk * cos_theta + acolk * sin_theta;
				a[k][row] = a[row][k];
				a[col][k] = acolk * cos_theta - arowk * sin_theta;
				a[k][col] = a[col][k];
			}
		}
		if (ite_num == 0)
		{
			p[row][row] = cos_theta;
			p[row][col] = -sin_theta;
			p[col][row] = sin_theta;
			p[col][col] = cos_theta;
		}
		else
		{
			for (k = 0; k < 3; k++)
			{
				double pki = p[k][row];
				double pkj = p[k][col];
				p[k][row] = pki*cos_theta + pkj*sin_theta;
				p[k][col] = pkj*cos_theta - pki*sin_theta;
			}
		}
		ite_num++;
	}
}

void collosion::schmidtOrthogonal(glm::mat3 &p, glm::vec3 &a) {
	int max_index = 0;
	if (a[1] >= a[0] && a[1] >= a[2]) max_index = 1;
	if (a[2] >= a[0] && a[2] >= a[1]) max_index = 2;
	p = glm::transpose(p);
	glm::vec3 v0 = p[max_index];
	glm::vec3 v1 = p[(max_index+1)%3];
	glm::vec3 v2 = p[(max_index+2)%3];
	v0 = glm::normalize(v0);
	v1 -= glm::dot(v1, v0)*v0;
	v1 = glm::normalize(v1);
	v2 = glm::normalize(glm::cross(v0, v1));
	p[0] = v0;
	p[1] = v1;
	p[2] = v2;
	p = glm::transpose(p);
}

glm::mat3 collosion::cal_cov_mat(const vector<glm::vec3>& vertexs){
	glm::mat3 cov_mat;
	//Compute the average x,y,z  
	glm::vec3 avg(0.0f, 0.0f, 0.0f);
	for (int i = 0; i<vertexs.size(); i++) {
		avg += vertexs[i];
	}
	avg /= vertexs.size();
	//Compute the covariance   
	for (int c = 0; c<3; c++)
	{
		for (int r = c; r<3; r++)
		{
			cov_mat[r][c] = 0.0f;
			float sum = 0.0f; 
			for (int i = 0; i<vertexs.size(); i++)
			{
				sum += (vertexs[i][r] - avg[r])*(vertexs[i][c] - avg[c]);
			}
			sum /= vertexs.size();
			cov_mat[r][c] = cov_mat[c][r] = sum;
		}
	}
	return cov_mat;
}

obb_box collosion::__gen_obb_box(const std::vector<glm::vec3> vertexs, const glm::mat3 &p) {
	
	obb_box obb;
	glm::mat3 tp = glm::transpose(p);
	obb.x_axis = tp[0];
	obb.y_axis = tp[1];
	obb.z_axis = tp[2];
	glm::vec3 max_point(-INFINITY,-INFINITY,-INFINITY), min_point(INFINITY,INFINITY,INFINITY);
	for (auto i : vertexs) {
		max_point.x = max(max_point.x, glm::dot(obb.x_axis, i));
		max_point.y = max(max_point.y, glm::dot(obb.y_axis, i));
		max_point.z = max(max_point.z, glm::dot(obb.z_axis, i));
		min_point.x = min(min_point.x, glm::dot(obb.x_axis, i));
		min_point.y = min(min_point.y, glm::dot(obb.y_axis, i));
		min_point.z = min(min_point.z, glm::dot(obb.z_axis, i));
	}
	glm::vec3 center = (max_point - min_point) / 2.0f + min_point;
	obb.center = center.x*obb.x_axis + center.y*obb.y_axis + center.z*obb.z_axis;
	obb.half = (max_point - min_point) / 2.0f;
	return obb;
}

obb_box collosion::gen_obb_box(const std::vector<glm::vec3>& vertexs) {
	glm::mat3 cov = cal_cov_mat(vertexs);
	glm::mat3 p;
	jacobi(cov, p);
	glm::vec3 evalue(cov[0][0], cov[1][1], cov[2][2]);
	schmidtOrthogonal(p, evalue);
	a = __gen_obb_box(vertexs, p);
	return a;
}

bool collosion::check_collision(collosion obj2) {
	obb_box b = obj2.a;
	glm::vec3 T = a.center - b.center;
	glm::vec3 axis[15];
	//?и┤???ив
	axis[0] = a.x_axis;
	axis[1] = a.y_axis;
	axis[2] = a.z_axis;
	axis[3] = b.x_axis;
	axis[4] = b.y_axis;
	axis[5] = b.z_axis;
	int k = 6;
	for (int i = 0; i < 3; i++) {
		for (int j = 3; j < 6; j++) {
			axis[k++] = glm::normalize(glm::cross(axis[i], axis[j]));
		}
	}
	glm::vec3 half_a[3] = {
		a.half.x*a.x_axis,
		a.half.y*a.y_axis,
		a.half.z*a.z_axis
	};
	glm::vec3 half_b[3] = {
		b.half.x*b.x_axis,
		b.half.y*b.y_axis,
		b.half.z*b.z_axis
	};

	using glm::dot;
	for (int i = 0; i < 15; i++) {
		glm::vec3 L = axis[i];
		float tmp = 0;
		for (int j = 0; j < 3; j++) {
			tmp += fabs(dot(half_a[j], L)) + fabs(dot(half_b[j], L));
		}
		bool no_intersect = fabs(dot(T, L)) > tmp;
		if (no_intersect) return false;
	}
	return true;
}