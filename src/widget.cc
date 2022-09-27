#include<fstream>
#include<opencv2/opencv.hpp>
#include"zgui.h"
using namespace std;

cv::Ptr<cv::freetype::FreeType2> z::Widget::ft2_;
const cv::Vec3b z::Widget::background_color_ = {200, 200, 200};
const cv::Vec3b z::Widget::widget_color_ = {220, 220, 220};
const cv::Vec3b z::Widget::highlight_color_ = {240, 240, 240};
const cv::Vec3b z::Widget::click_color_ = {180, 180, 180};

void z::Widget::shade_rect(cv::Rect2i r, int shade, cv::Vec3b color, cv::Vec3b ul, cv::Vec3b lr) {
	cv::rectangle(mat_, r, ul, -1);
	cv::rectangle(mat_, {{r.x + shade, r.y + shade}, r.br()}, lr, -1);
	cv::rectangle(mat_, {r.x + shade, r.y + shade, r.width - 2 * shade,
			r.height - 2 * shade}, color, -1);
}

z::Widget::Widget(cv::Rect_<int> r)
	: Rect_<int>{r}
	, mat_(r.height, r.width)
{
	mat_ = background_color_;
	if(!ft2_) {
		ft2_ = cv::freetype::createFreeType2();
		ifstream f{"font.dat"};
		string s;
		getline(f, s);
		ft2_->loadFontData(s, 0); // difficult 프린트시 bug
    //ft2_->loadFontData("/usr/share/fonts/truetype/nanum/NanumBarunGothic.ttf", 0); // 한자 불완전
		//ft2_->loadFontData("/usr/share/fonts/opentype/noto/NotoSansCJK-Regular.ttc", 0); // difficult 프린트시 bug
		//ft2_->loadFontData("/usr/share/fonts/opentype/noto/NotoSerifCJK-Regular.ttc", 0); // difficult 프린트시 bug
    //ft2_->loadFontData("/home/zeta/Documents/Un.ttf", 0);
    //ft2_->loadFontData("/usr/share/fonts/truetype/baekmuk/batang.ttf", 0);
	}
}

z::Widget& z::Widget::operator=(const z::Widget &r) 
{/// copy rectangle property of widget r
	x = r.x; y = r.y; width = r.width; height = r.height;
	return *this;
}

bool z::Widget::focus() const {
	return focus_;
}
void z::Widget::focus(bool tf) {
	focus_ = tf;
	if(!tf) {
		if(gui_callback_.find(EVENT_LEAVE) != gui_callback_.end()) {
			gui_callback_[EVENT_LEAVE](x, y);
			update();
		}
		if(user_callback_.find(EVENT_LEAVE) != user_callback_.end())
			user_callback_[EVENT_LEAVE](x, y);
	} else {
		if(gui_callback_.find(EVENT_ENTER) != gui_callback_.end()) {
			gui_callback_[EVENT_ENTER](x, y);
			update();
		} 
		if(user_callback_.find(EVENT_ENTER) != user_callback_.end())
			user_callback_[EVENT_ENTER](x, y);
	}
}

void z::Widget::event_callback(int event, int x, int y) 
{
	if(gui_callback_.find(event) != gui_callback_.end()) {
		gui_callback_[event](x, y);
		update();
	}
	if(user_callback_.find(event) != user_callback_.end())
		user_callback_[event](x, y);
}

void z::Widget::resize(cv::Rect2i r) 
{/// resize \ref mat_ and rectangle of this widget
	*this = r;
	cv::resize(mat_, mat_, {r.width, r.height});
}

void z::Widget::update() 
{/// copy this widget's current graphical state to parent window and show it.
	if(parent_ != nullptr) {
		*parent_ << *this;
		show();
	}
}

void z::Widget::show() {
	if(parent_ != nullptr) parent_->show();
}

void z::Widget::hide() {
	hidden_ = true;
}

std::string z::source_loc(const std::source_location &loc) {
	std::stringstream ss;
	ss << '[' << loc.file_name() << ':' << loc.line() << "] [" << loc.function_name() << "]\n";
	return ss.str();
}

