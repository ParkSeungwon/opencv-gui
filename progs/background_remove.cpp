#include<vector>
#include<iostream>
#include<opencv2/opencv.hpp>
#include<algorithm>
#include"option.h"
using namespace std;

cv::Mat img;
vector<pair<int, int>> visited;
void change_alpha(int x, int y, int leap) {
	visited.push_back({x, y});
	for(int i : {-1, 1}) for(int j : {-1, 1}) if(x + i < img.cols && x + i >= 0 && y + i >= 0 && y + i < img.rows) {
		auto &b = img.at<cv::Vec4b>(x, y);
		auto &a = img.at<cv::Vec4b>(x + i, y + j);
		if(abs(a[0] - b[0]) < leap && abs(a[1] - b[1]) < leap && abs(a[2] - b[2]) < leap) a[3] = 0;
		if(find(visited.begin(), visited.end(), pair{x+i, y+j}) == visited.end())
			change_alpha(x + i, y + j, leap);
	}
}

int main(int ac, char** av)
{
	CMDoption co
	{ {"input", "input file name", "in.png"}
	, {"x", "seed point x", 0}
	, {"y", "seed point y", 0}
	, {"leap", "margin leap", 10}
	, {"out", "output file", "out.png"}
	};
	if(!co.args(ac, av)) return 0;
	img = cv::imread(co.get<const char*>("input"), cv::IMREAD_UNCHANGED);

	change_alpha(co.get<int>("x"), co.get<int>("y"), co.get<int>("leap"));
	cv::imwrite(co.get<const char*>("out"), img);
}



