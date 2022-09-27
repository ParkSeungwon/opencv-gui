#include<filesystem>
#include<iostream>
#include<opencv2/freetype.hpp>
#include"zgui.h"
using namespace std;

int main() {
	cv::Ptr<cv::freetype::FreeType2> ft2_ = cv::freetype::createFreeType2();
	//ft2_->loadFontData("/usr/share/fonts/truetype/nanum/NanumBarunGothic.ttf", 0);
	ft2_->loadFontData("/usr/share/fonts/opentype/noto/NotoSansCJK-Regular.ttc", 0);
	cv::Mat m = cv::imread("Lenna.png");
	ft2_->putText(m, "difficult", {10, 0}, 40, {0, 0, 0}, -1, 4, false);
	cv::imshow("win", m);
	while(1) {
	int k = cv::waitKey();
	cout << "waitKey result : " << k << endl;
	k = cv::waitKeyEx();
	cout << "waitKeyEx result : " << k << endl;
	}
}
