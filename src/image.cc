#include<opencv2/opencv.hpp>
#include"zgui.h"
using namespace std;

z::Image::Image(cv::Rect2i r) : Widget{r}
{ }

cv::Mat &z::Image::operator=(const cv::Mat &r)
{
//	int depth = r.depth();
//	switch(depth) {
//		case cv::CV_8U:
//	}
	cv::Mat m;
	if(r.channels() == 1) cv::cvtColor(r, m, cv::COLOR_GRAY2BGR);
	else if(r.channels() == 4) cv::cvtColor(r, m, cv::COLOR_BGRA2BGR);
	else m = r;
	int depth = m.depth();
	double a = 1, b = 0;
	switch(depth) {
  	case CV_16U: a = 1. / 255; break;
		case CV_16S: a = 1. / 255; b = 128; break;
		case CV_32S: a = 1. / 255 / 255; b = 128; break;
		case CV_32F: 
		case CV_64F: a = 255;
	}
	if(depth != CV_8U) m.convertTo(m, CV_8U, a, b);
	if(m.size() == mat_.size()) m.copyTo(mat_);
	else cv::resize(m, mat_, mat_.size());
	return mat_;
}
