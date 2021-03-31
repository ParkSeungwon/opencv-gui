#include<iostream>
#include<opencv2/opencv.hpp>
#include"zgui.h"
using namespace std;

z::PopupInterface::PopupInterface(z::Window *p)
{
	window_ptr_ = p;
}

int z::PopupInterface::open(int flag, int x, int y)
{
	window_ptr_->start(flag);
	if(x >= 0 && y >= 0) cv::moveWindow(window_ptr_->title(), x, y);
	while(!closed_) {//popup quit by quit(int) function
		try { cv::getWindowProperty(window_ptr_->title(), 0); }//for x button close
		catch(...) { break; }
		if(int key = cv::waitKey(10); key != -1) window_ptr_->keyboard_callback(key);
	}
	closed_ = false;
	return result_;
}

void z::PopupInterface::quit(int r)
{
	result_ = r;
	closed_ = true;
	window_ptr_->close();
}

z::Popup::Popup(string title, cv::Rect2i r) : z::Window{title, r}, PopupInterface{this}
{ }

z::AsciiPopup::AsciiPopup(const char *p, int unit_width, int unit_height, int margin)
	: z::AsciiWindow(p, unit_width, unit_height, margin), PopupInterface{this}
{ }
