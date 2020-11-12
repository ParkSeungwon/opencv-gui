#include<opencv2/opencv.hpp>
#include"button.h"
using namespace std;
using namespace placeholders;

z::TextInput::TextInput(cv::Rect2i r) : z::Widget{r}
{
	mat_ = white;
	gui_callback_[EVENT_KEYBOARD] = bind(&z::TextInput::key_event, this, _1, _2);
}

void z::TextInput::key_event(int key, int)
{
	//if(key == 13) return user_callback_[EVENT_KEYBOARD](0, 0);
	if(key == 8 && !value_.empty()) value_.pop_back();
	else if(key >= 0x20 && key <= 0x7e) value_ += key;
	else return;
	mat_ = white;
	ft2_->putText(mat_, value_, {0, 0}, height * 0.8, {0, 0, 0}, -1, 4, false);
}

void z::TextInput::enter(function<void(string)> f)
{
	user_callback_[EVENT_KEYBOARD] = [this, f](int val, int){ if(val == 13) f(value());};
}

string z::TextInput::value()
{
	return value_;
}

