#include"src/zgui.h"
#include<iostream>
using namespace std;

z::Canvas::Canvas(cv::Rect2i r) : z::Widget(r)
{
	mat_ = cv::Vec3b{255, 255, 255};
	user_callback_[cv::EVENT_LBUTTONDOWN] = [this](int xpos, int ypos){pen_down_ = true; cout << xpos -x << ' ' << ypos-y << endl;};
	user_callback_[cv::EVENT_LBUTTONUP] = [this](int, int){pen_down_ = false;};
	gui_callback_[cv::EVENT_MOUSEMOVE] = [this](int xpos, int ypos){
		if(pen_down_) {
			mat_[ypos-y][xpos-x] = draw_color_;
			update();
		}
	};
}
