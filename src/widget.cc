#include<opencv2/opencv.hpp>
#include"zgui.h"
using namespace std;

cv::Ptr<cv::freetype::FreeType2> z::Widget::ft2_;
const cv::Vec4b z::Widget::background_color_ = {200, 200, 200, 255};
const cv::Vec4b z::Widget::widget_color_ = {220, 220, 220, 255};
const cv::Vec4b z::Widget::highlight_color_ = {240, 240, 240, 255};
const cv::Vec4b z::Widget::click_color_ = {180, 180, 180, 255};

void z::Widget::shade_rect(cv::Rect2i r, int shade, cv::Vec4b color, cv::Vec4b ul, cv::Vec4b lr) {
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
    ft2_->loadFontData("/usr/share/fonts/truetype/nanum/NanumBarunGothic.ttf", 0);
    //ft2_->loadFontData("/home/zeta/Documents/Un.ttf", 0);
	}
}

bool z::Widget::focus() {
	return focus_;
}
void z::Widget::focus(bool tf) {
	focus_ = tf;
}

void z::Widget::resize(cv::Rect2i r) {
	*this = r;
	cv::resize(mat_, mat_, {r.width, r.height});
}

void z::Widget::drop_alpha() {
	std::vector<cv::Mat1b> channels;
	cv::split(mat_, channels);
	channels[3].copyTo(alpha_channel_);
	channels.pop_back();
	cv::merge(channels, mat_);
}

void z::Widget::add_alpha() {
	std::vector<cv::Mat1b> channels;
	cv::split(mat_, channels);
	channels.push_back(alpha_channel_);
	cv::merge(channels, mat_);
}
