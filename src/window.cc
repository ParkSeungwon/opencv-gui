#include<opencv2/opencv.hpp>
#include<ranges>
#include"zgui.h"
using namespace std;

void mouse_callback(int event, int x, int y, int flags, void *ptr)
{//get mouse event
	z::Window *p = (z::Window*)ptr;
	x += p->scrolled_rect_.x;
	y += p->scrolled_rect_.y;
	z::Widget *pw = nullptr;
	for(auto w : *p) { // assumption : zIndex increase
		if(w->contains({x, y}) && w->activated()) pw = w;//set 
		else if(w->focus()) {//leave event
			if(w->gui_callback_.find(EVENT_LEAVE) != w->gui_callback_.end()) {
				w->gui_callback_[EVENT_LEAVE](x, y);
				w->update();
			}
			if(w->user_callback_.find(EVENT_LEAVE) != w->user_callback_.end())
				w->user_callback_[EVENT_LEAVE](x, y);
			w->focus(false);
		}
	}
	if(!pw) return;

	//if(event == cv::EVENT_MOUSEWHEEL) cout << cv::getMouseWheelDelta(flags) << endl;
	if(pw->is_window()) mouse_callback(event, x - pw->x, y - pw->y, flags, pw);
	else if(event == cv::EVENT_MOUSEMOVE) {
		if(!pw->focus()) {//enter event
			pw->focus(true);
			if(pw->gui_callback_.find(EVENT_ENTER) != pw->gui_callback_.end()) {
				pw->gui_callback_[EVENT_ENTER](x, y);
				pw->update();
			} 
			if(pw->user_callback_.find(EVENT_ENTER) != pw->user_callback_.end())
				pw->user_callback_[EVENT_ENTER](x, y);
		} else {//move event
			if(pw->gui_callback_.find(event) != pw->gui_callback_.end()) {
				pw->gui_callback_[event](x, y);
				pw->update();
			}
			if(pw->user_callback_.find(event) != pw->user_callback_.end())
				pw->user_callback_[event](x, y);
		}
	} else {//all other event
		if(pw->gui_callback_.find(event) != pw->gui_callback_.end()) {
			pw->gui_callback_[event](x, y);
			pw->update();
		}
		if(pw->user_callback_.find(event) != pw->user_callback_.end())
			pw->user_callback_[event](x, y);
	}
}

z::Window::Window(string title, cv::Rect2i r) : z::Widget{r}
{
	title_ = title;
	scrolled_rect_ = r;
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
	w.parent_ = this;
	w.on_register();
	return *this;
}

z::Window& z::Window::operator-(z::Widget &w)
{
	widgets_.erase(std::remove(widgets_.begin(), widgets_.end(), &w));
	mat_(w) = background_color_;
	w.parent_ = nullptr;
	std::for_each(widgets_.begin(), widgets_.end(), [&w, this](auto *a) {
		if((w & *a) != cv::Rect2i{0,0,0,0}) *this << *a;
	});
	return *this;
}

void z::Window::move_widget(z::Widget &w, cv::Point2i p)
{ // move a widget that is already mounted
	if(p.x + w.width > width) p.x = width - w.width;
	if(p.y + w.height > height) p.y = height - w.height;
	if(p.x < 0 ) p.x = 0;
	if(p.y < 0 ) p.y = 0;
	w.x = p.x;
	w.y = p.y;
}

//void z::Window::resize(cv::Rect2i r)
//{
//	z::Widget::resize(r);
//	for(const auto &a : widgets_) a->mat_.copyTo(mat_(*a));
//}

string z::Window::title()
{
	return title_;
}

//void z::Window::update(const z::Widget &r) 
//{
//	r.mat_.copyTo(mat_(r));
//}

void z::Window::copy_widget_to_mat(const z::Widget &r)
{
	//if(!contains(r.tl())) return;
	cv::Point2i pt{std::min(r.br().x, br().x), std::min(r.br().y, br().y)};
	cv::Rect2i rect{r.tl(), pt};
	if(r.alpha() == 1) r.mat_({0, 0, rect.width, rect.height}).copyTo(mat_(rect));
	else cv::addWeighted(r.mat_({0, 0, rect.width, rect.height}), r.alpha(), mat_(rect), 1-r.alpha(), 0, mat_(rect));
}

void z::Window::organize_accordingto_zindex()
{ //for mouse event 
	std::sort(widgets_.begin(), widgets_.end(), [](auto a, auto b) { return a->zIndex() < b->zIndex();});
	std::for_each(widgets_.begin(), widgets_.end(), 
			[this](auto *a) {if(!a->hidden()) copy_widget_to_mat(*a); });
}

z::Window& z::Window::operator<<(z::Widget &r)
{ // copy to mat_
	if(r.hidden()) return *this;
	copy_widget_to_mat(r);
	std::for_each
	( ++std::find(widgets_.begin(), widgets_.end(), &r)
	, widgets_.end()
	, [this, &r] (Widget *a) 
		{ if((r & *a) != cv::Rect2i{0,0,0,0} && a->zIndex() > r.zIndex()) *this << *a;
		}
	);
	return *this;
}

z::Window& z::Window::operator>>(z::Widget &r)
{ // remove from mat_
	r.hidden(true);
	if(!contains(r.tl())) return *this;
	cv::Point2i pt{std::min(r.br().x, br().x), std::min(r.br().y, br().y)};
	cv::rectangle(mat_, {r.tl(), pt}, background_color_, cv::FILLED);
	for(z::Widget *a : widgets_) 
		if((r & *a) !=  cv::Rect2i{0,0,0,0}) *this << *a;
	draw_all_wrapped();
	//std::for_each
	//( widgets_.begin()
	//, widgets_.end()
	//, [this, &r] (Widget *a) 
	//	{ if((r & *a) != cv::Rect2i{0,0,0,0} && !r.hidden()) *this << *a;
	//	}
	//);
	return *this;
}

void z::Window::draw_all_wrapped()
{
	for(auto a : wrapped_) draw_wrapped(a);
}

void z::Window::show()
{
	if(parent_ == nullptr) cv::imshow(title_, mat_(scrolled_rect_));
	else {
		*parent_ << *this;
		parent_->show();
	}
}

void z::Window::close()
{
	cv::destroyWindow(title_);
}

void z::Window::popup(z::Window &w, std::function<void(int)> f) 
{// popup 시 widget을 빼는 것이 문제
	z::Window *p = &w;
	while(p->parent_ != nullptr) {
		if(x != 0 || y != 0) {
			x += p->x;
			y += p->y;
		}
		p = p->parent_;
	}
	auto r = p->scrolled_rect_;
	static z::Widget panel{{0,0,1,1}};
	panel.resize(r);
	panel.zIndex(1000);
	panel.alpha(0.3);
	panel.mat_ = cv::Vec3b{0,0,0};
	zIndex(1002);
	popup_on_ = p;
	if(x == 0 && y == 0) {
		x = r.x + (r.width - width) / 2;
		y = r.y + (r.height - height) / 2;
	}
	if(p->width < x + width) x = std::max(0, p->width - width);
	if(p->height < y + height) y = std::max(0, p->height - height);
	organize_accordingto_zindex();
	*p + panel + *this;
	p->organize_accordingto_zindex();
	p->show();
	popup_exit_func_ = f;
}

void z::Window::popdown(int value)
{
	if(popup_on_ == nullptr) return;
	popup_on_->widgets_.erase(popup_on_->widgets_.end() - 2, popup_on_->widgets_.end());
	popup_on_->mat_ = background_color_;
	popup_on_->organize_accordingto_zindex();
	popup_on_->draw_all_wrapped();
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
	organize_accordingto_zindex();
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
	for(z::Widget* p : *this) if(p->is_window()) 
		dynamic_cast<z::Window*>(p)->keyboard_callback(key);
	else if(p->focus()) {
		if(p->gui_callback_.find(EVENT_KEYBOARD) != p->gui_callback_.end()) {
			p->gui_callback_[EVENT_KEYBOARD](key, 0);
			p->update();
		}
		if(p->user_callback_.find(EVENT_KEYBOARD) != p->user_callback_.end())
			p->user_callback_[EVENT_KEYBOARD](key, 0);
	}
}

int z::Window::open(int flag, int x, int y)
{
	start(flag);
	if(x >= 0 && y >= 0) cv::moveWindow(title(), x, y);
	while(!closed_) {//popup quit by quit(int) function
		try { cv::getWindowProperty(title(), 0); }//for x button close
		catch(...) { break; }
		if(int key = cv::waitKey(10); key != -1) keyboard_callback(key);
	}
	closed_ = false;
	return result_;
}

void z::Window::quit(int r)
{
	result_ = r;
	closed_ = true;
	close();
}

void z::Window::on_register()
{
	if(title() != "") {
		Wrapped w{{0, 10, width, height - 10}, 20, title()};
		wrapped_.push_back(w);
		draw_wrapped(w);
	}
}
