#include<opencv2/opencv.hpp>
#include"zgui.h"
using namespace std;
using namespace placeholders;

z::Slider::Slider(cv::Rect2i r, int start, int end, int step) : z::Widget{r}
{
	start_ = start; end_ = end; step_ = step;
	value((end - start) / 2);
	logical_length_ = end_ - start_;
	physical_length_ = max(width, height) - 20;
	draw();
	gui_callback_[cv::EVENT_LBUTTONUP] = bind(&z::Slider::lup, this, _1, _2);
	gui_callback_[cv::EVENT_LBUTTONDOWN] = bind(&z::Slider::ldown, this, _1, _2);
	gui_callback_[cv::EVENT_MOUSEMOVE] = bind(&z::Slider::move, this, _1, _2);
	gui_callback_[EVENT_KEYBOARD] = bind(&z::Slider::key_event, this, _1, _2);
}

void z::Slider::draw()
{
	mat_ = background_color_;
	int r = min(width, height) / 3;
	if(width > height) {
		cv::line(mat_, {10, height / 2}, {width - 11, height / 2}, {100, 100, 100}, 2);
		cv::circle(mat_, {to_pos(value_), height / 2}, r, widget_color_, -1);
		cv::ellipse(mat_, {to_pos(value_), height / 2}, {r,r}, 135, 0, 180, highlight_color_, 2);
		cv::ellipse(mat_, {to_pos(value_), height / 2}, {r,r}, 315, 0, 180, click_color_, 2);
	} else {
		cv::line(mat_, {width / 2, 10}, {width / 2, height - 11}, {100, 100, 100}, 2);
		cv::circle(mat_, {width / 2, to_pos(value_)}, r, widget_color_, -1);
		cv::ellipse(mat_, {width / 2, to_pos(value_)}, {r,r}, 135, 0, 180, highlight_color_, 2);
		cv::ellipse(mat_, {width / 2, to_pos(value_)}, {r,r}, 315, 0, 180, click_color_, 2);
	}
}

int z::Slider::to_pos(int val)
{
	int r = (val - start_) * physical_length_ / logical_length_ + 10;
	return width > height ? r : height - r;
}

int z::Slider::to_val(int pos)
{
	 return ((width > height ? pos : height - pos) - 10) * logical_length_ / physical_length_ + start_;
}

void z::Slider::value(int v)
{
	if(v < start_) value_ = start_;
	else if(v > end_) value_ = end_;
	else value_ = v;
	//draw();
}

int z::Slider::value()
{
	return value_;
}

void z::Slider::ldown(int x, int y)
{
	int pos = to_pos(value_);
	if(width > height) {
		if(pos - 10 < x - this->x && x - this->x < pos + 10) hold_ = true;
	} else if(pos - 10 < y - this->y && y - this->y < pos + 10) hold_ = true;
}

void z::Slider::move(int x, int y)
{
	if(hold_) {
		value(width > height ? to_val(x - this->x) : to_val(y - this->y));
		draw();
	}
}

void z::Slider::lup(int x, int y)
{
	if(hold_) hold_ = false;
	else {
		value(width > height ? to_val(x - this->x) : to_val(y - this->y));
		draw();
	}
}

void z::Slider::key_event(int key, int)
{
	switch(key) {
		case 81: value(value_ - step_); break;//left arrow key
		case 83: value(value_ + step_); break;//right arrow key
		case 82: value(value_ + step_); break;//up arrow key
		case 84: value(value_ - step_); break;//down 
	}
	draw();
}

void z::Slider::on_change(function<void(int)> f)
{
	user_callback_[cv::EVENT_LBUTTONDOWN] = [this](int, int) {user_hold_ = true;};
	user_callback_[cv::EVENT_MOUSEMOVE] = [this, f](int, int) { if(user_hold_) f(value()); };
	user_callback_[cv::EVENT_LBUTTONUP] = [this, f](int, int) { f(value()); user_hold_ = false; };
	user_callback_[EVENT_KEYBOARD] = [this, f](int, int) { f(value()); };
}
