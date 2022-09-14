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
	gui_callback_[cv::EVENT_LBUTTONDOWN] = [this](int, int) { 
		mouse_down_ = true; 
		*parent_ >> *this >> hh_ >> vh_;
		show();
	};
	gui_callback_[EVENT_ENTER] = [this](int, int) {  };
	gui_callback_[EVENT_LEAVE] = [this](int, int) { 
		//mat_ = click_color_; 
		hidden(false);
		mouse_down_ = false;
		*parent_ << *this; show(); 
	};
	gui_callback_[cv::EVENT_MOUSEMOVE] = [this](int xpos, int ypos) { // scrolled_rect_.x y 가 더해진 값
		if(mouse_down_) {
			scwin_->move_widget(*this, {xpos - widget_size_ / 2, ypos - widget_size_ / 2});
			scwin_->set_br({xpos + widget_size_ / 2, ypos + widget_size_ / 2});
			show();
		}
	};
	gui_callback_[cv::EVENT_LBUTTONUP] = [this](int, int) { 
		mouse_down_ = false; 
		auto r = scwin_->scrolled_rect_;
		scwin_->move_widget(hh_, {0, r.br().y - hh_.widget_height_});
		scwin_->move_widget(vh_, {r.br().x - vh_.widget_width_, 0});
		hh_.draw(); 
		vh_.draw();
		hidden(false); hh_.hidden(false); vh_.hidden(false);
		*scwin_ << hh_ << vh_ << *this;
		show();
	};
	gui_callback_[cv::EVENT_LBUTTONDBLCLK] = [this] (int, int) {
		mouse_down_ = false;
		if(scwin_->scrolled_rect_ == *scwin_) scwin_->scrolled_rect_ = scroll_backup_;
		else {
			scroll_backup_ = scwin_->scrolled_rect_;
			scwin_->scrolled_rect_ = *scwin_;
		}
		auto pt = scwin_->scrolled_rect_.br();
		*scwin_ >> *this >> hh_ >> vh_;
		hidden(false); hh_.hidden(false); vh_.hidden(false);
		scwin_->move_widget(*this, {pt.x - widget_size_, pt.y - widget_size_});
		scwin_->move_widget(hh_, {0, pt.y - hh_.widget_height_});
		scwin_->move_widget(vh_, {pt.x - vh_.widget_width_ ,0});
		hh_.draw(); vh_.draw();
		*scwin_ << hh_ << vh_ << *this;
		show();
	};
}

void z::Handle::on_register() {
	scwin_ = dynamic_cast<z::Window*>(parent_);
	if(scwin_->scrolled_rect_ == cv::Rect2i{0,0,0,0}) {
		x = scwin_->br().x - widget_size_;
		y = scwin_->br().y - widget_size_;
	} else {
		x = scwin_->scrolled_rect_.br().x - widget_size_;
		y = scwin_->scrolled_rect_.br().y - widget_size_;
	}
	*parent_ + vh_;
	*parent_ + hh_;
}

z::VHandle::VHandle(Handle &h) : z::Widget{{0,0,1,1}}, handle_{h}
{
	zIndex(99);
	gui_callback_[cv::EVENT_LBUTTONUP] = [this] (int, int ypos) {
		auto &r = scwin_->scrolled_rect_;
		float ratio = float{r.height} / scwin_->height;
		int half = ratio * scwin_->height / 2;
		r.y = std::max(0, scwin_->height * (ypos - r.y) / r.height - half) ;
		r.y = std::min(scwin_->height - r.height, r.y);
		draw();
		*scwin_ >> handle_ >> handle_.hh_;
		scwin_->move_widget(handle_, {handle_.x, r.br().y - handle_.widget_size_});
		scwin_->move_widget(handle_.hh_, {0, r.br().y - handle_.hh_.widget_height_});
		handle_.hh_.draw(); 
		handle_.hidden(false); handle_.hh_.hidden(false);
		*scwin_ << *this << handle_ << handle_.hh_;
		show();
	};
}

void z::VHandle::on_register() 
{ // resize mat_, rect, equal to fullsize window height -> change x position
	scwin_ = dynamic_cast<z::Window*>(parent_);
	resize({scwin_->scrolled_rect_.br().x - widget_width_, 0, widget_width_, scwin_->height - 30});
	draw();
}

void z::VHandle::draw()
{ // draw mat_ according to proportion
	mat_ = click_color_;
	auto r = scwin_->scrolled_rect_;
	float occupying_ratio = float{r.height} / scwin_->height;
	float occupying_start = float{r.y} / scwin_->height;
	starty_ = r.y + occupying_start * r.height;
	endy_ = starty_ + occupying_ratio * r.height;
	shade_rect({ 0, r.y + occupying_start * r.height, widget_width_, occupying_ratio * r.height});
}

z::HHandle::HHandle(Handle &h) : z::Widget{{0,0,1,1}}, handle_{h}
{
	zIndex(99);
	gui_callback_[cv::EVENT_LBUTTONUP] = [this] (int xpos, int) {
		auto &r = scwin_->scrolled_rect_;
		float ratio = float{r.width} / scwin_->width;
		int half = ratio * scwin_->width / 2;
		r.x = std::max(0, scwin_->width * (xpos - r.x) / r.width - half) ;
		r.x = std::min(scwin_->width - r.width, r.x);
		draw();
		*scwin_ >> handle_.vh_ >> handle_;
		show();
		scwin_->move_widget(handle_, {r.br().x - handle_.widget_size_, r.br().y - handle_.widget_size_});
		scwin_->move_widget(handle_.vh_, {r.br().x - handle_.vh_.widget_width_, 0});
		handle_.vh_.draw();
		handle_.hidden(false); handle_.vh_.hidden(false);
		*scwin_ << *this << handle_.vh_ << handle_;
		show();
	};
}

void z::HHandle::on_register()
{
	scwin_ = dynamic_cast<z::Window*>(parent_);
	resize({0, scwin_->scrolled_rect_.br().y - widget_height_, scwin_->width - 30, widget_height_});
	draw();
}

void z::HHandle::draw()
{
	mat_ = click_color_;
	auto r = scwin_->scrolled_rect_;
	float occupying_ratio = float{r.width} / scwin_->width;
	float occupying_start = float{r.x} / scwin_->width;
	shade_rect({r.x + occupying_start * r.width, 0, occupying_ratio * r.width, widget_height_});
}
