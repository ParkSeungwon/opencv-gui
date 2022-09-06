#include<opencv2/opencv.hpp>
#include"zgui.h"
using namespace std;

z::ScrolledWindow::ScrolledWindow(string title, cv::Rect2i r) : Window(title, r)
{ }

void z::ScrolledWindow::scroll_to(cv::Rect2i r)
{
	if((*this & r) == r) {
		scrolled_rect_ = r;
		show();
	}
}

void z::ScrolledWindow::show()
{
	if(scrolled_rect_ != cv::Rect2i{0,0,0,0}) cv::imshow(title_, mat_(scrolled_rect_));
	else z::Window::show();
}

void z::ScrolledWindow::set_ul(cv::Point2i p) {
	scroll_to({p.x, p.y, width, height});
}

void z::ScrolledWindow::set_br(cv::Point2i p) {
	scroll_to({{x, y}, p});
}

z::Handle::Handle() : Widget{{0,0,widget_size_,widget_size_}} {
	zIndex(100);
	gui_callback_[cv::EVENT_LBUTTONDOWN] = [this](int, int) { mouse_down_ = true; };
	gui_callback_[EVENT_ENTER] = [this](int, int) { shade_rect({0, 0, width, height}, 3, highlight_color_); };
	gui_callback_[EVENT_LEAVE] = [this](int, int) { shade_rect({0, 0, width, height}, 3, click_color_); };
	gui_callback_[cv::EVENT_MOUSEMOVE] = [this](int x, int y) { 
		if(mouse_down_) {
			mat_ = background_color_;
			*win_ << *this;
			win_->set_br(this->br());
			resize({win_->scrolled_rect_.br().x - widget_size_, 
					win_->scrolled_rect_.br().y - widget_size_, widget_size_, widget_size_});
			mat_ = cv::Vec3b{255, 0, 0};
			*win_ << *this;
			cout << win_->scrolled_rect_.width << ' ' << win_->scrolled_rect_.height << endl;
			win_->show();
			mat_ = click_color_;
		}
	};
	gui_callback_[cv::EVENT_LBUTTONUP] = [this](int, int) { mouse_down_ = false; };
}

void z::Handle::on_register(z::Window *win) {
	win_ = dynamic_cast<z::ScrolledWindow*>(win);
	if(win_->scrolled_rect_ == cv::Rect2i{0,0,0,0}) resize({win_->br().x - widget_size_, 
			win_->br().y - widget_size_, widget_size_, widget_size_});
	else resize({win_->scrolled_rect_.br().x - widget_size_, 
			win_->scrolled_rect_.br().y - widget_size_, widget_size_, widget_size_});
	mat_ = click_color_;
	*win << *this;
}
