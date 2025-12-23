#include<filesystem>
#include<vector>
#include<fstream>
#include<iostream>
#include<cassert>
#include<opencv2/opencv.hpp>
#include<algorithm>
#include"option.h"
using namespace std;

int main(int ac, char** av)
{
	CMDoption co
	{ {"input", "input file name", "input.png"}
	, {"output", "output file name", "output.png"}
  , {"background", "background file", "/home/zeta/256G/Project/SDL2/units.png"}
	, {"x", "dest x", 0}
	, {"y", "dest y", 0}
	, {"width", "image width", -1}
	, {"height", "image height", -1}
	, {"sx", "src x", 0}
	, {"sy", "src y", 0}
	};
	if(!co.args(ac, av)) return 0;

	cv::Mat m1 = cv::imread(co.get<const char*>("input"), cv::IMREAD_UNCHANGED);
	cv::Mat m2 = cv::imread(co.get<const char*>("background"), cv::IMREAD_UNCHANGED);
	int x = co.get<int>("x");
	int y = co.get<int>("y");
	int w = co.get<int>("width");
	int h = co.get<int>("height");
	int sx = co.get<int>("sx");
	int sy = co.get<int>("sy");
	cout << x << ' ' << y << endl;
	if(w == -1) w = m1.cols;
	if(h == -1) h = m1.rows;
	m1(cv::Rect(sx, sy, w, h)).copyTo(m2(cv::Rect(x, y, w, h)));
	cv::imwrite(co.get<const char*>("output"), m2);
	cv::imshow("win", m2);
	cv::waitKey();
}

// ford 115 344
// eliz 96 323
// degol 83 298
// kuz 82 328

