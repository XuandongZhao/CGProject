#include "lib\halfCross.h"
Point base;

double cross_product(Point p0, Point p1, Point p2)
{
	return (p1.x - p0.x)*(p2.y - p0.y) - (p2.x - p0.x)*(p1.y - p0.y);
}
double dis(Point p1, Point p2)
{
	return sqrt((p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y));
}
bool com(const Point &p1, const Point &p2)
{
	double temp = cross_product(base, p1, p2);
	if (fabs(temp) < 1e-6)
	{
		return dis(base, p1) < dis(base, p2);
	}
	else
	{
		return temp > 0;
	}
}

void halfPlaneCross::graham_scan(vector<Point> input, vector<Point> & ans)
{
	ans.clear();
	int top = 2;
	int index = 0;
	for (int i = 1; i < input.size(); ++i)//选出Y坐标最小的点，若Y坐标相等，选择X坐标小的点
	{
		if (input[i].y < input[index].y || (input[i].y == input[index].y && input[i].x < input[index].x))
		{
			index = i;
		}
	}
	std::swap(input[0], input[index]);
	ans.push_back(input[0]);
	base = ans[0];
	//按极角排序
	std::sort(input.begin() + 1, input.end(), ::com);
	ans.push_back(input[1]);
	ans.push_back(input[2]);
	for (int i = 3; i < input.size(); ++i)
	{
		while (top > 0 && cross_product(ans[top - 1], input[i], ans[top]) >= 0)
		{
			--top;
			ans.pop_back();
		}
		ans.push_back(input[i]);
		++top;
	}
}

double crs(Point u, Point v) { return u.x*v.y - u.y*v.x; }

Point itr(line x, line y) {
	double t = crs(y.v, x.p - y.p) / crs(x.v, y.v);
	x.p.x += t*x.v.x; x.p.y += t*x.v.y; return x.p;
}

bool check(line x, line y, line t) {
	//cout <<"iiii"<< crs(t.v, itr(x, y) - t.p) << endl;
	return crs(t.v, itr(x, y) - t.p)<-1e-6;
}
bool _hPCompLines(const line & x, const line& y)
{
	return x.k < y.k || x.k == y.k && crs(x.v, y.p - x.p)<0;
}
bool halfPlaneCross::calc()
{
	vector<Point>input;
	vector<Point>output;
	for (int i = 0; i < 10; i++)
	{
		if (!testObjects[i].valid)	continue;
		input.clear();
		output.clear();
		vector<line>ll;
		ll.push_back(line());
		cout << "test object" << endl;
		double height = 0x3f3f3f3f3f3f3f3f;
		for (int j = 0; j < 8; j++)
		{
			glm::vec4 tmp = (*testObjects[i].modelPtr)*glm::vec4(testObjects[i].data[j],1.f);
			input.push_back(Point(tmp.x,tmp.z));
			height = min(height, (double)tmp.y);
			cout << tmp.x << " " << tmp.z << endl;
		}
		graham_scan(input, output);
		for (int j = 0; j < output.size(); j++)
		{
			cout << output[j].x << " "<<output[j].y<<endl;
		}
		for (int j = 0; j < output.size(); j++)
		{
			line tmp;
			tmp.p = output[j];
			tmp.v = output[(j + 1) % (output.size())] - output[j];
			tmp.k = atan2(tmp.v.y, tmp.v.x);
			ll.push_back(tmp);
		}
		for (int j = 0; j < solids.size(); j++)
		{
			if (height > solids[j].height)
			{
				continue;
			}
			vector<line>l = ll;
			for (int k = 0; k < solids[j].data.size(); k++)
			{
				l.push_back(solids[j].data[k]);
			}
			//cout << l.size() << endl;
			sort(l.begin()+1, l.end(), ::_hPCompLines);
			/*for (int k = 1; k < l.size(); k++)
			{
				cout << l[k].k <<" "<<l[k].p.x<<" "<<l[k].p.y<<" "<<l[k].v.x<< l[k].v.y<< endl;
			}
			cout << "&&&&" << endl;*/

			int cc = 1;
			for (int k = 2;k < l.size(); k++)
				if (l[k].k != l[cc].k) l[++cc] = l[k];

			int head = 1, tail = 2; q[1] = l[1]; q[2] = l[2];
			for (int k = 3; k <= cc; k++) {
				while (head<tail && check(q[tail - 1], q[tail], l[k])) tail--;
				while (head<tail && check(q[head + 1], q[head], l[k])) head++;
				q[++tail] = l[k];
			}
			/*cout << "OOOOOOOOOOOOO" << endl;
			for (int k = head; k < tail; k++)
			{
				cout << q[k].k << " " << q[k].p.x << " " << q[k].p.y << " " << q[k].v.x << " " << q[k].v.y << endl;
			}
			cout << "TTTTTTTTTTTTT" << endl;*/
			while (head<tail && check(q[tail - 1], q[tail], q[head])) tail--;
			while (head<tail && check(q[head + 1], q[head], q[tail])) head++;
			/*cout << "OOOOOOOOOOOOO" << endl;
			for (int k = head; k < tail; k++)
			{
				cout << q[k].k << " " << q[k].p.x << " " << q[k].p.y << " " << q[k].v.x <<" "<< q[k].v.y << endl;
			}
			cout << "TTTTTTTTTTTTT" << endl;
			*/
			int n = 0;
			for (int k = head; k<tail; k++) p[++n] = itr(q[k], q[k + 1]);
			p[++n] = itr(q[head], q[tail]);
			double ans = 0; p[n + 1] = p[1];

			/*cout << "$$$$$$$$$$$$$" << endl;
			for (int k = 1; k <= n+1; k++)
			{
				cout << p[k].x << " " << p[k].y << endl;
			}
			cout << "^^^^^^^^^^^^^^" << endl;*/

			for (int k = 1; k <= n; k++) ans += crs(p[k], p[k + 1]);
			cout << "area" << ans << endl;
			if (ans > 0)
			{
				return true;
			}
		}
		
	}

	return false;
	
}
