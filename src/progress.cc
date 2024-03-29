#include<opencv2/opencv.hpp>
#include"zgui.h"
using namespace std;

z::Progress::Progress(cv::Rect2i r) : z::Widget{r}
{
	cv::rectangle(mat_, {0, height / 3, width, height / 3}, click_color_, -1);
}

void z::Progress::value(int val)
{/// @param val 0 ~ 1000. indicates how much progressed
	value_ = val;
	cv::rectangle(mat_, {0, height / 3, width, height / 3}, click_color_, -1);
	shade_rect({0, height / 3, val * width /1000, height / 3}, 2);
}

int z::Progress::value() const
{
	return value_;
}
