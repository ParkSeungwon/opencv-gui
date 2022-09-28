#include<opencv2/opencv.hpp>
#include"zgui.h"
using namespace std;
using namespace placeholders;

//const cv::Vec3b z::Widget::background_color_ = cv::Vec3b{200, 200, 200};

z::Button::Button(string text, cv::Rect_<int> r) : z::Widget{r}
{/// Button with text at position and size of r
	text_ = text;
	draw();
	gui_callback_[EVENT_ENTER] = [this](int, int){draw(highlight_color_);};
	gui_callback_[EVENT_LEAVE] = [this](int, int){draw(widget_color_);};
	gui_callback_[cv::EVENT_LBUTTONDOWN] = [this](int, int){draw(click_color_);};
	gui_callback_[cv::EVENT_LBUTTONUP] = [this](int, int){draw(widget_color_);};
}

void z::Button::draw(cv::Vec3b color, bool repaint)
{/// @param color background color
 /// @param rapaint if repaint is false, do not draw background and just draw text
	int baseline = 0;
	auto sz = ft2_->getTextSize(text_, height * 0.8, -1, &baseline);
	int pos = (width - sz.width) / 2;
	if(repaint) shade_rect({0, 0, width, height}, 3, color);
	ft2_->putText(mat_, text_, {pos, 0}, height * 0.8, {0,0,0}, -1, 4, false);
}

void z::Button::click(function<void()> f) 
{/// sets the function to be executed when the button is clicked.
	user_callback_[cv::EVENT_LBUTTONUP] = [f](int, int) {f();};
}


void z::Button::text(string s)
{/// sets the button text
	text_ = s;
	draw();
}
