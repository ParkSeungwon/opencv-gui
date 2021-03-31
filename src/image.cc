#include<opencv2/opencv.hpp>
#include"zgui.h"
using namespace std;

z::Image::Image(cv::Rect2i r) : Widget{r}
{ }

cv::Mat &z::Image::operator=(const cv::Mat &r)
{
	if(r.size() == mat_.size()) mat_ = r;
	else cv::resize(r, mat_, mat_.size());
	return mat_;
}
