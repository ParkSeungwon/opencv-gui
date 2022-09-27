#include<opencv2/opencv.hpp>
#include"zgui.h"
using namespace std;

void z::Window::scroll_to(cv::Rect2i r)
{ /// use this to scroll or resize window.
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
		width = height = std::min(std::min(parent_->width, parent_->height), 100); // careful, bug prone..
																																							 // deceive event area
		show();
	};
	gui_callback_[cv::EVENT_MOUSEMOVE] = [this](int xpos, int ypos) { // scrolled_rect_.x y 가 더해진 값
		if(mouse_down_) {
			parent_->move_widget(*this, {xpos - width / 2, ypos - height / 2});
			parent_->set_br({xpos + width / 2, ypos + height / 2});
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
	width = height = widget_size_; // make sure that no drawing is performed during width, height change
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
	gui_callback_[cv::EVENT_LBUTTONDOWN] = [this] (int, int ypos) {
		mouse_down_ = true;
		press_y_ = ypos;
		*parent_ >> handle_ >> handle_.hh_;
	};
	gui_callback_[cv::EVENT_MOUSEMOVE] = [this](int, int ypos) {
		if(mouse_down_) {
			int delta = scroll_delta( ypos - press_y_);
			press_y_ = ypos + delta;
			draw();
			update();
		}
	};
	gui_callback_[EVENT_LEAVE] = gui_callback_[cv::EVENT_LBUTTONUP] = [this] (int, int ypos) { 
		mouse_down_ = false; 
		handle_.position_widgets();
		handle_.show_widgets();
	};
	gui_callback_[cv::EVENT_LBUTTONDBLCLK] = [this] (int, int ypos) {
		scroll_window(ypos);
		draw();
		*parent_ >> handle_ >> handle_.hh_;
		handle_.position_widgets();
		handle_.show_widgets();
	};
}

int z::VHandle::scroll_delta(int yd) {
	auto &r = parent_->scrolled_rect_;
	int prev = r.y;
	r.y += yd;
	r.y = std::max(0, r.y);
	r.y = std::min(parent_->height - r.height, r.y);
	return r.y - prev;
}

void z::VHandle::scroll_window(int ypos) 
{
	auto &r = parent_->scrolled_rect_;
	float ratio = float{r.height} / parent_->height;
	int half = ratio * parent_->height / 2;
	r.y = std::max(0, parent_->height * (ypos - r.y) / r.height - half) ;
	r.y = std::min(parent_->height - r.height, r.y);
}

void z::VHandle::on_register() 
{ // resize mat_, rect, equal to fullsize window height -> change x position
	resize({parent_->scrolled_rect_.br().x - widget_width_, 0, widget_width_, parent_->height - handle_.widget_size_});
	draw();
}

void z::VHandle::draw()
{ // draw mat_ according to proportion
	mat_ = click_color_;
	shade_rect({{ 0, starty()}, cv::Point2i{widget_width_, endy()}});
}
int z::VHandle::starty() const
{
	auto r = parent_->scrolled_rect_;
	float occupying_start = float{r.y} / parent_->height;
	return r.y + occupying_start * r.height;
}
int z::VHandle::endy() const
{
	auto r = parent_->scrolled_rect_;
	float occupying_ratio = float{r.height} / parent_->height;
	return starty() + occupying_ratio * r.height;
}

z::HHandle::HHandle(Handle &h) : z::Widget{{0,0,1,1}}, handle_{h}
{
	zIndex(99);
	gui_callback_[cv::EVENT_LBUTTONDOWN] = [this] (int, int ypos) {
		mouse_down_ = true;
		press_x_ = ypos;
		*parent_ >> handle_ >> handle_.vh_;
	};
	gui_callback_[cv::EVENT_MOUSEMOVE] = [this](int xpos, int) {
		if(mouse_down_) {
			int delta = scroll_delta( xpos - press_x_);
			press_x_ = xpos + delta;
			draw();
			update();
		}
	};
	gui_callback_[EVENT_LEAVE] = gui_callback_[cv::EVENT_LBUTTONUP] = [this] (int, int) { 
		mouse_down_ = false; 
		handle_.position_widgets();
		handle_.show_widgets();
	};
	gui_callback_[cv::EVENT_LBUTTONDBLCLK] = [this] (int, int xpos) {
		scroll_window(xpos);
		draw();
		*parent_ >> handle_ >> handle_.vh_;
		handle_.position_widgets();
		handle_.show_widgets();
	};
}

int z::HHandle::scroll_delta(int xd) {
	auto &r = parent_->scrolled_rect_;
	int prev = r.x;
	r.x += xd;
	r.x = std::max(0, r.x);
	r.x = std::min(parent_->width - r.width, r.x);
	return r.x - prev;
}

void z::HHandle::scroll_window(int xpos) {
	auto &r = parent_->scrolled_rect_;
	float ratio = float{r.width} / parent_->width;
	int half = ratio * parent_->width / 2;
	r.x = std::max(0, parent_->width * (xpos - r.x) / r.width - half) ;
	r.x = std::min(parent_->width - r.width, r.x);
}

void z::HHandle::on_register()
{
	resize({0, parent_->scrolled_rect_.br().y - widget_height_, parent_->width - handle_.widget_size_, widget_height_});
	draw();
}

void z::HHandle::draw()
{
	mat_ = click_color_;
	shade_rect({{startx(), 0}, cv::Point2i{endx(), widget_height_}});
}
int z::HHandle::startx() const
{
	auto r = parent_->scrolled_rect_;
	float occupying_start = float{r.x} / parent_->width;
	return r.x + occupying_start * r.width;
}

int z::HHandle::endx() const
{
	auto r = parent_->scrolled_rect_;
	float occupying_ratio = float{r.width} / parent_->width;
	return startx() + occupying_ratio * r.width;
}
