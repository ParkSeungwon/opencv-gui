#include<opencv2/opencv.hpp>
#include"zgui.h"
using namespace std;
using namespace placeholders;

z::CheckBox::CheckBox(cv::Rect2i r) : z::Widget{r}
{
	int k = min(width, height);
	cv::resize(mat_, mat_, {k, k});
	x += (width - k) / 2;
	y += (height - k) / 2;
	width = height = k;
	shade_rect({0, 0, k, k}, 2, background_color_, click_color_, highlight_color_);
	gui_callback_[cv::EVENT_LBUTTONUP] = bind(&CheckBox::click, this, _1, _2);
}

void z::CheckBox::click(int, int) 
{
	checked_ = !checked_;
	checked(checked_);
}

bool z::CheckBox::checked()
{
	return checked_;
}

void z::CheckBox::checked(bool t)
{
	checked_ = t;
	cv::rectangle(mat_, {2, 2}, {width - 2, width - 2},
			checked_ ? cv::Scalar{0, 255, 0} : background_color_, -1); 
}

void z::CheckBox::on_change(function<void(bool)> f)
{
	user_callback_[cv::EVENT_LBUTTONUP] = [this, f](int,int) { f(checked()); };
}

