#include<opencv2/opencv.hpp>
#include"zgui.h"
using namespace std;
using namespace placeholders;

//const cv::Vec3b z::Widget::background_color_ = cv::Vec3b{200, 200, 200};

z::Button::Button(string text, cv::Rect_<int> r) : z::Widget{r}
{
	text_ = text;
	repaint(widget_color_);
	gui_callback_[EVENT_ENTER] = [this](int, int){repaint(highlight_color_);};
	gui_callback_[EVENT_LEAVE] = [this](int, int){repaint(widget_color_);};
	gui_callback_[cv::EVENT_LBUTTONDOWN] = [this](int, int){repaint(click_color_);};
	gui_callback_[cv::EVENT_LBUTTONUP] = [this](int, int){repaint(widget_color_);};
}

void z::Button::label()
{
	int baseline = 0;
	auto sz = ft2_->getTextSize(text_, height * 0.8, -1, &baseline);
	int pos = (width - sz.width) / 2;
	ft2_->putText(mat_, text_, {pos, 0}, height * 0.8, {0,0,0}, -1, 4, false);
}

void z::Button::click(function<void()> f) 
{
	user_callback_[cv::EVENT_LBUTTONUP] = [f](int, int) {f();};
}

void z::Button::repaint(cv::Vec3b color)
{
	shade_rect({0, 0, width, height}, 3, color);
	label();
}

void z::Button::text(string s)
{
	text_ = s;
	shade_rect({0, 0, width, height}, 3, widget_color_);
	label();
}
