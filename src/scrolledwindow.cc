#include<opencv2/opencv.hpp>
#include"zgui.h"
using namespace std;

z::ScrolledWindow::ScrolledWindow(string title, cv::Rect2i r) : Window(title, r)
{ }

void z::ScrolledWindow::scroll_to(cv::Rect2i r)
{
	if((*this & r) == r) {
		scrolled_rect_ = r;
		//move_widget(handle_, {r.br().x - handle_.widget_size_, r.br().y - handle_.widget_size_});
		show();
	}
}

void z::ScrolledWindow::show()
{
	cout << "show" << endl;
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
	mat_ = cv::Vec3b{255,0,0}; //click_color_;
	gui_callback_[cv::EVENT_LBUTTONDOWN] = [this](int, int) { mouse_down_ = true; };
	gui_callback_[EVENT_ENTER] = [this](int, int) { shade_rect({0, 0, width, height}, 3, highlight_color_); };
	gui_callback_[EVENT_LEAVE] = [this](int, int) { shade_rect({0, 0, width, height}, 3, click_color_); };
	gui_callback_[cv::EVENT_MOUSEMOVE] = [this](int xpos, int ypos) { 
		if(mouse_down_) {
			scwin_->move_widget(*this, {xpos - scwin_->scrolled_rect_.x - widget_size_ / 2, 
					ypos - scwin_->scrolled_rect_.y - widget_size_ / 2});
			scwin_->set_br({xpos + widget_size_ / 2, ypos + widget_size_ / 2});
			cout << "xy pos "  << xpos << ' ' << ypos << endl;
			cout << scwin_->scrolled_rect_.width << ' ' << scwin_->scrolled_rect_.height << endl;
			scwin_->show();
		}
	};
	gui_callback_[cv::EVENT_LBUTTONUP] = [this](int, int) { mouse_down_ = false; };
}

void z::Handle::on_register() {
	scwin_ = dynamic_cast<z::ScrolledWindow*>(parent_);
	if(scwin_->scrolled_rect_ == cv::Rect2i{0,0,0,0}) {
		x = scwin_->br().x - widget_size_;
		y = scwin_->br().y - widget_size_;
	} else {
		x = scwin_->scrolled_rect_.br().x - widget_size_;
		y = scwin_->scrolled_rect_.br().y - widget_size_;
	}
}
