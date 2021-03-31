#include<opencv2/opencv.hpp>
#include"zgui.h"
using namespace std;

void mouse_callback(int event, int x, int y, int flags, void *ptr)
{//get mouse event
	z::Window *p = (z::Window*)ptr;
	z::Widget *pw = nullptr;
	for(auto w : *p) {
		if(w->contains({x, y})) pw = w;//set 
		else if(w->focus()) {//leave event
			if(w->gui_callback_.find(EVENT_LEAVE) != w->gui_callback_.end()) {
				w->gui_callback_[EVENT_LEAVE](x, y);
				*p << *w;
			}
			if(w->user_callback_.find(EVENT_LEAVE) != w->user_callback_.end())
				w->user_callback_[EVENT_LEAVE](x, y);
			w->focus(false);
		}
	}
	if(!pw) return;

	if(event == cv::EVENT_MOUSEMOVE) {
		if(!pw->focus()) {//enter event
			pw->focus(true);
			if(pw->gui_callback_.find(EVENT_ENTER) != pw->gui_callback_.end()) {
				pw->gui_callback_[EVENT_ENTER](x, y);
				*p << *pw;
			} 
			if(pw->user_callback_.find(EVENT_ENTER) != pw->user_callback_.end())
				pw->user_callback_[EVENT_ENTER](x, y);
		} else {//move event
			if(pw->gui_callback_.find(event) != pw->gui_callback_.end()) {
				pw->gui_callback_[event](x, y);
				*p << *pw;
			}
			if(pw->user_callback_.find(event) != pw->user_callback_.end())
				pw->user_callback_[event](x, y);
		}
	} else {//all other event
		if(pw->gui_callback_.find(event) != pw->gui_callback_.end()) {
			pw->gui_callback_[event](x, y);
			*p << *pw;
		}
		if(pw->user_callback_.find(event) != pw->user_callback_.end())
			pw->user_callback_[event](x, y);
	}
}

z::Window::Window(string title, cv::Rect2i r) : z::Widget{r}
{
	title_ = title;
}

vector<z::Widget*>::iterator z::Window::begin() {
	return widgets_.begin();
}
vector<z::Widget*>::iterator z::Window::end() {
	return widgets_.end();
}

z::Window& z::Window::operator+(z::Widget &w)
{
	//lock_guard<mutex> lck{mtx_};
	widgets_.push_back(&w);
	w.mat_.copyTo(mat_(w));
	return *this;
}

void z::Window::resize(cv::Rect2i r)
{
	z::Widget::resize(r);
	for(const auto &a : widgets_) a->mat_.copyTo(mat_(*a));
}

string z::Window::title()
{
	return title_;
}

void z::Window::update(const z::Widget &r) 
{
	r.mat_.copyTo(mat_(r));
}

z::Window& z::Window::operator<<(z::Widget &r)
{
	//lock_guard<mutex> lck{mtx_};
	r.mat_.copyTo(mat_(r));
	show();
	return *this;
}

void z::Window::show()
{
	cv::imshow(title_, mat_);
}

void z::Window::close()
{
	cv::destroyWindow(title_);
}

void z::Window::start(int flag)
{
	cv::namedWindow(title_, flag);
	cv::setMouseCallback(title_, mouse_callback, this);
	show();
}

int z::Window::loop()
{//get keyboard event
	for(int key; (key = cv::waitKey()) != -1;) keyboard_callback(key);//destroy window make waitkey return -1
	return 0;
}

void z::Window::keyboard_callback(int key)
{
	for(z::Widget* p : *this) if(p->focus()) {
		if(p->gui_callback_.find(EVENT_KEYBOARD) != p->gui_callback_.end()) {
			p->gui_callback_[EVENT_KEYBOARD](key, 0);
			*this << *p;
		}
		if(p->user_callback_.find(EVENT_KEYBOARD) != p->user_callback_.end())
			p->user_callback_[EVENT_KEYBOARD](key, 0);
	}
}
