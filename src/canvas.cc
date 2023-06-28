#include"src/zgui.h"
#include<iostream>
using namespace std;

z::Canvas::Canvas(cv::Rect2i r) : z::Widget(r)
{
	static cv::Point2i before{-1, -1};
	mat_ = cv::Vec3b{255, 255, 255};
	user_callback_[cv::EVENT_LBUTTONDOWN] = [this](int, int){pen_down_ = true;};
	user_callback_[cv::EVENT_LBUTTONUP] = [this](int, int){pen_down_ = false; before.x = -1;};
	gui_callback_[cv::EVENT_MOUSEMOVE] = [this](int xpos, int ypos)
	{ if(pen_down_) 
		{ cv::Point2i p{xpos - x, ypos - y};
			if(before.x != -1) cv::line(mat_, before, p, draw_color_, 2);
			before = p;
			update();
		}
	};
}

void z::Canvas::clear()
{
	mat_ = cv::Vec3b{255, 255, 255};
	update();
}
