#include<opencv2/opencv.hpp>
#include"zgui.h"
using namespace std;

z::WButton::WButton(cv::Rect2i r) : z::Widget{r}
{
	mat_ = widget_color_;
	gui_callback_[cv::EVENT_LBUTTONUP] = [this](int xpos, int) {
		if(xpos - x < width / 2) mat_ = cv::Vec3b{0,0,255};
		else mat_ = cv::Vec3b{255,0,0};
		update();
	};
}
