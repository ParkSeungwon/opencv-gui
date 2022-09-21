#include<filesystem>
#include<iostream>
#include"zgui.h"
using namespace std;

int main() {
	cv::Mat m = cv::imread("Lenna.png");
	cv::imshow("win", m);
	while(1) {
	int k = cv::waitKey();
	cout << "waitKey result : " << k << endl;
	k = cv::waitKeyEx();
	cout << "waitKeyEx result : " << k << endl;
	}
}
