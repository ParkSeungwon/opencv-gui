#include<opencv2/opencv.hpp>
#include"zgui.h"
using namespace std;

z::Image::Image(cv::Rect2i r) : Widget{r}
{ }

cv::Mat &z::Image::operator=(const cv::Mat &r)
{
	cv::Mat m;
	if(r.channels() == 1) cv::cvtColor(r, m, cv::COLOR_GRAY2BGR);
	else if(r.channels() == 4) cv::cvtColor(r, m, cv::COLOR_BGRA2BGR);
	else m = r;
	if(m.size() == mat_.size()) m.copyTo(mat_);
	else cv::resize(m, mat_, mat_.size());
	return mat_;
}
