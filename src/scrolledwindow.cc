#include<opencv2/opencv.hpp>
#include"zgui.h"
using namespace std;

void z::Window::scroll_to(cv::Rect2i r)
{
	if((*this & r) == r) scrolled_rect_ = r;
}

void z::Window::set_ul(cv::Point2i p) {
	scroll_to({p, scrolled_rect_.br()});
}

void z::Window::set_br(cv::Point2i p) {
	scroll_to({scrolled_rect_.tl(), p});
}

z::Handle::Handle() : Widget{{0,0,widget_size_,widget_size_}}, vh_{*this}, hh_{*this} 
{
	zIndex(100);
	mat_ = click_color_;
	gui_callback_[EVENT_ENTER] = gui_callback_[EVENT_LEAVE] = gui_callback_[cv::EVENT_LBUTTONUP] 
		= std::bind(&z::Handle::routine, this, std::placeholders::_1, std::placeholders::_2);
	gui_callback_[cv::EVENT_LBUTTONDOWN] = [this](int, int) { 
		mouse_down_ = true; 
		*parent_ >> *this >> hh_ >> vh_;
		show();
	};
	gui_callback_[cv::EVENT_MOUSEMOVE] = [this](int xpos, int ypos) { // scrolled_rect_.x y 가 더해진 값
		if(mouse_down_) {
			parent_->move_widget(*this, {xpos - widget_size_ / 2, ypos - widget_size_ / 2});
			parent_->set_br({xpos + widget_size_ / 2, ypos + widget_size_ / 2});
			show();
		}
	};
	gui_callback_[cv::EVENT_LBUTTONDBLCLK] = [this] (int, int) {
		if(parent_->scrolled_rect_ == *parent_) parent_->scrolled_rect_ = scroll_backup_;
		else {
			scroll_backup_ = parent_->scrolled_rect_;
			parent_->scrolled_rect_ = *parent_;
		}
		*parent_ >> *this >> hh_ >> vh_;
		routine(0 ,0);
	};
}

void z::Handle::routine(int, int) {
	mouse_down_ = false;
	position_widgets();
	show_widgets();
}

void z::Handle::show_widgets()
{
	hidden(false); hh_.hidden(false); vh_.hidden(false);
	*parent_ << hh_ << vh_ << *this;
	show();
}

void z::Handle::position_widgets()
{
	auto pt = parent_->scrolled_rect_.br();
	parent_->move_widget(hh_, {0, pt.y - hh_.widget_height_});
	parent_->move_widget(vh_, {pt.x - vh_.widget_width_, 0});
	parent_->move_widget(*this, {pt.x - widget_size_, pt.y - widget_size_});
	hh_.draw(); 
	vh_.draw();
}

void z::Handle::on_register() {
	if(parent_->scrolled_rect_ == cv::Rect2i{0,0,0,0}) {
		x = parent_->br().x - widget_size_;
		y = parent_->br().y - widget_size_;
	} else {
		x = parent_->scrolled_rect_.br().x - widget_size_;
		y = parent_->scrolled_rect_.br().y - widget_size_;
	}
	*parent_ + vh_;
	*parent_ + hh_;
}

z::VHandle::VHandle(Handle &h) : z::Widget{{0,0,1,1}}, handle_{h}
{
	zIndex(99);
	gui_callback_[cv::EVENT_LBUTTONUP] = [this] (int, int ypos) {
		auto &r = parent_->scrolled_rect_;
		float ratio = float{r.height} / parent_->height;
		int half = ratio * parent_->height / 2;
		r.y = std::max(0, parent_->height * (ypos - r.y) / r.height - half) ;
		r.y = std::min(parent_->height - r.height, r.y);
		draw();
		*parent_ >> handle_ >> handle_.hh_;
		parent_->move_widget(handle_, {handle_.x, r.br().y - handle_.widget_size_});
		parent_->move_widget(handle_.hh_, {0, r.br().y - handle_.hh_.widget_height_});
		handle_.hh_.draw(); 
		handle_.hidden(false); handle_.hh_.hidden(false);
		*parent_ << *this << handle_ << handle_.hh_;
		show();
	};
}

void z::VHandle::on_register() 
{ // resize mat_, rect, equal to fullsize window height -> change x position
	resize({parent_->scrolled_rect_.br().x - widget_width_, 0, widget_width_, parent_->height - 30});
	draw();
}

void z::VHandle::draw()
{ // draw mat_ according to proportion
	mat_ = click_color_;
	auto r = parent_->scrolled_rect_;
	float occupying_ratio = float{r.height} / parent_->height;
	float occupying_start = float{r.y} / parent_->height;
	starty_ = r.y + occupying_start * r.height;
	endy_ = starty_ + occupying_ratio * r.height;
	shade_rect({ 0, r.y + occupying_start * r.height, widget_width_, occupying_ratio * r.height});
}

z::HHandle::HHandle(Handle &h) : z::Widget{{0,0,1,1}}, handle_{h}
{
	zIndex(99);
	gui_callback_[cv::EVENT_LBUTTONUP] = [this] (int xpos, int) {
		auto &r = parent_->scrolled_rect_;
		float ratio = float{r.width} / parent_->width;
		int half = ratio * parent_->width / 2;
		r.x = std::max(0, parent_->width * (xpos - r.x) / r.width - half) ;
		r.x = std::min(parent_->width - r.width, r.x);
		draw();
		*parent_ >> handle_.vh_ >> handle_;
		show();
		parent_->move_widget(handle_, {r.br().x - handle_.widget_size_, r.br().y - handle_.widget_size_});
		parent_->move_widget(handle_.vh_, {r.br().x - handle_.vh_.widget_width_, 0});
		handle_.vh_.draw();
		handle_.hidden(false); handle_.vh_.hidden(false);
		*parent_ << *this << handle_.vh_ << handle_;
		show();
	};
}

void z::HHandle::on_register()
{
	resize({0, parent_->scrolled_rect_.br().y - widget_height_, parent_->width - 30, widget_height_});
	draw();
}

void z::HHandle::draw()
{
	mat_ = click_color_;
	auto r = parent_->scrolled_rect_;
	float occupying_ratio = float{r.width} / parent_->width;
	float occupying_start = float{r.x} / parent_->width;
	shade_rect({r.x + occupying_start * r.width, 0, occupying_ratio * r.width, widget_height_});
}
