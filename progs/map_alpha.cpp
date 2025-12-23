#include<filesystem>
#include<vector>
#include<fstream>
#include<iostream>
#include<opencv2/opencv.hpp>
#include<algorithm>
#include"option.h"
using namespace std;

int main(int ac, char** av)
{
	CMDoption co
	{ {"map", "map file name", "newzeland.png"}
	, {"r", "red", 255}
	, {"g", "green", 255}
	, {"b", "blue", 255}
	, {"a", "alpha", 255}
	, {"to_r", "change red to this, if -1 do not touch", -1}
	, {"to_g", "change green to this, if -1 do not touch", -1}
	, {"to_b", "change blue to this, if -1 do not touch", -1}
	, {"to_a", "change alpha to this, if -1 do not touch", -1}
	, {"out", "output file", "wrote.png"}
	};
	if(!co.args(ac, av)) return 0;
	cv::Mat img = cv::imread(co.get<const char*>("map"), cv::IMREAD_UNCHANGED);
	int r = co.get<int>("r"), g = co.get<int>("g"), b = co.get<int>("b"), alpha = co.get<int>("a");
	int to_r = co.get<int>("to_r"), to_g = co.get<int>("to_g"), to_b = co.get<int>("to_b")
		, to_a = co.get<int>("to_a");
	to_r = to_r == -1 ? r : to_r;
	to_g = to_g == -1 ? g : to_g;
	to_b = to_b == -1 ? b : to_b;
	to_a = to_a == -1 ? alpha : to_a;

	for(int i=0; i<img.rows; i++) for(int j=0; j<img.cols; j++) {
        // You can now access the pixel value with cv::Vec3b
		auto &a = img.at<cv::Vec4b>(i, j);
		if(a[0] == b && a[1] == g && a[2] == r && a[3] == alpha) {
			a[0] = to_b; a[1] = to_g; a[2] = to_r; a[3] = to_a;
		}
	}
	cv::imwrite(co.get<const char*>("out"), img);
}


