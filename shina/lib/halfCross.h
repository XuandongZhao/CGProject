#pragma once
#ifndef SHINA_LIB_HALFCROSS_H
#define SHINA_LIB_HALFCROSS_H
#include "main.h"
class halfPlaneCross {
private:
	struct objectToTest {
		glm::mat4 * modelPtr;
		vector<glm::vec3>data;
		bool valid;
	};
	struct solidObject {
		vector<line>data;
		double height;
	};
	objectToTest testObjects[10];//°üÎ§ºÐs
	vector<solidObject> solids;
	line q[10000];
	Point p[10000];
	int cnt;
	vector<Point> change;

	void graham_scan(vector<Point> input, vector<Point> & ans);


public:
	halfPlaneCross() {}
	void addConvexHull(const vector<glm::vec3>& data)
	{
		vector<Point>ans, input;
		solidObject solid;
		double height = -10000;
		for (auto &i : data)
		{
			height = max(height, (double)i.y);
			input.push_back(Point(i.x, i.z));
		}
		solid.height = height;
		graham_scan(input, ans);
		cout << "Convex Hull" << endl;
		cout << height << endl;
		for (int i = 0; i < ans.size(); i++)
		{
			cout << ans[i].x << " " << ans[i].y << endl;
		}
		for (int i = 0; i < ans.size(); i++)
		{
			line tmp;
			tmp.p = ans[i];
			tmp.v = ans[(i + 1) % (ans.size())] - ans[i];
			tmp.k = atan2(tmp.v.y, tmp.v.x);
			solid.data.push_back(tmp);
		}
		solids.push_back(solid);
		cnt = solid.data.size();
	}
	void addtestObject(int id, const vector<glm::vec3>&input, glm::mat4* model)
	{
		assert(id >= 0 && id < 10);
		assert(input.size() == 8);
		assert(model != nullptr);
		testObjects[id].data = input;
		testObjects[id].modelPtr = model;
		testObjects[id].valid = true;
	}
	int calc();
};



#endif