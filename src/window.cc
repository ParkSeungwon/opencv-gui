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

	//if(event == cv::EVENT_MOUSEWHEEL) cout << cv::getMouseWheelDelta(flags) << endl;
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
	w.parent_ = this;
	return *this;
}

z::Window& z::Window::operator-(z::Widget &w)
{
	widgets_.erase(std::remove(widgets_.begin(), widgets_.end(), &w));
	mat_(w) = background_color_;
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

void z::Window::popup(z::Window &w, std::function<void(int)> f) 
{
	popup_on_ = &w;
	if(x == 0 && y == 0) {
		x = (w.width - width) / 2;
		y = (w.height - height) / 2;
	}
	w.backup_ = move(w.widgets_); //disable callback
	w.shade_rect(*this);
	for(auto &a : widgets_) {
		a->x += x;
		a->y += y;
		w + *a;
	}
	w.show();
	popup_exit_func_ = f;
}

void z::Window::popdown(int value)
{
	if(popup_on_ == nullptr) return;
	for(auto &a : widgets_) {
		a->x -= x;
		a->y -= y;
	}
	popup_on_->mat_ = background_color_;
	popup_on_->widgets_.clear();
	for(auto *p : popup_on_->backup_) *popup_on_ + *p;
	for(const auto &a : popup_on_->wrapped_) popup_on_->draw_wrapped(a);
	popup_on_->show();
	popup_on_ = nullptr;
	popup_exit_func_(value);
}

void z::Window::draw_wrapped(const Wrapped &wr)
{
	cv::rectangle(mat_, wr.rect, {100,100,100}, 1);
	int base = 0;
	if(wr.title != "") {
		auto sz = ft2_->getTextSize(wr.title, wr.fontsize, -1, &base);
		cv::line(mat_, {wr.rect.x + 20, wr.rect.y}, {wr.rect.x + sz.width + 40, wr.rect.y}, background_color_, 1);
		ft2_->putText(mat_, wr.title, {wr.rect.x + 30, wr.rect.y - 5 - wr.fontsize / 2}, wr.fontsize, {0,0,0}, -1, 4, false);
	}
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
