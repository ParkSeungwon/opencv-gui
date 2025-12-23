#include<filesystem>
#include<vector>
#include<fstream>
#include<iostream>
#include<cassert>
#include<opencv2/opencv.hpp>
#include<algorithm>
using namespace std;

int main(int ac, char** av)
{
	vector<string> v;
	for(const filesystem::path &p : filesystem::directory_iterator("./unit_icons"))
		v.push_back(p.filename());
	assert(v.size() > 0);
	sort(v.begin(), v.end());
	int height = (v.size() -1) / 12 * 100 + 100;
	cout << height << endl;
	cv::Mat m = cv::Mat::zeros(height, 1200, CV_8UC4);
	cout << m.cols << ' ' << m.rows << endl;
	ofstream f{"dimension.txt"};
	for(int y=0, i=0; y < height; y+=100) {
		for(int x=0; x<1200; x+=100) {
			cv::Mat tmp = cv::imread("./unit_icons/" + v[i], cv::IMREAD_UNCHANGED);
			cout << v[i] << ' ' << x << ' ' << y << endl;
			tmp.copyTo(m(cv::Rect(x, y, 100, 100)));
			f << v[i++] << ' ' << x  << ' ' << y  << endl;
			if(i == v.size()) break;
		}
	}
	cv::imwrite("dimension.png", m);
}
