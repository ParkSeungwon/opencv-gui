#include<filesystem>
#include<iostream>
#include"zgui.h"
using namespace std;

int main() {
	cv::Rect2i r1{0, 0, 10, 10};
	cv::Rect2i r2{5, 5, 10, 10};
	r1 = r2;
	auto r = r1 & r2;
	//cout << r.x << ' ' << r.y << ' ' << r.width << ' ' << r.height << endl;
	cout << r1.x << ' ' << r1.y << ' ' << r1.width << ' ' << r1.height << endl;

}
