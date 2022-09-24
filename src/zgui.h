#pragma once
#include<map>
#include<type_traits>
#include<functional>
#include<spdlog/spdlog.h>
#include<source_location>
#include<opencv2/highgui.hpp>
#include<opencv2/freetype.hpp>
#include<opencv2/imgproc.hpp>
#include<list>
#include<hangul.h>
#define EVENT_ENTER 8000
#define EVENT_LEAVE 8001
#define EVENT_KEYBOARD 8002

namespace z {

std::string source_loc(const std::source_location &location = std::source_location::current());
class Window;

class Widget : public cv::Rect_<int>
{//base class for all widgets
public:
	Widget(cv::Rect_<int> r);
	Widget &operator=(const Widget &r);
	bool focus() const;
	virtual void focus(bool);
	virtual void event_callback(int event, int x, int y);
	virtual void show();
	virtual std::string type() const { return "Widget"; }
	void resize(cv::Rect2i r);
	/// map<int : event, function(x, y)>. library will draw change in widget graphics after gui_callback
	std::map<int, std::function<void(int, int)>> gui_callback_;
	/// map<int : event, function( x, y)>. library will assume user_callback is not related to graphical change. 
	/// So it will not draw any change in widget graphics after user_callback is called.
	/// If you changed widget graphics during user_callback you have to manually change graphics inside the callback.
	/// call update().
	std::map<int, std::function<void(int, int)>> user_callback_;
	cv::Mat3b mat_;///< Matrix that contains widget shape.
	void update();
	Window *parent_ = nullptr;///< pointer to parent window that is containing this widget
	virtual void on_register() {} ///< will be called when widget is added to a window
	virtual bool is_window() const {return false;} 
	int zIndex() {return zIndex_;}
	void zIndex(int v) {zIndex_ = v; }
	void hidden(bool v) {hidden_ = v;}
	void activated(bool v) {activated_ = v;}
	bool hidden() const {return hidden_;}
	bool activated() const {return activated_;}
	void hide();
	float alpha() const {return alpha_;}
	void alpha(float f) {alpha_ = f;}
	void shade_rect(cv::Rect2i r, int shade = 3, cv::Vec3b color = widget_color_,
			cv::Vec3b upper_left = highlight_color_, cv::Vec3b lower_right = click_color_);

protected:
	float alpha_ = 1;
	bool hidden_ = false, activated_ = true;
	static const cv::Vec3b background_color_, widget_color_, highlight_color_, click_color_;
	bool focus_ = false;
	static cv::Ptr<cv::freetype::FreeType2> ft2_;
	int zIndex_ = 0;
};

class Label : public Widget
{
public:
	Label(std::string text, cv::Rect2i r);
	void text(std::string s);
	std::string text() const;
protected:
	std::string text_;
};

class Button : public Widget
{
public:
	Button(std::string text, cv::Rect_<int> r);
	void click(std::function<void()> f);
	void text(std::string s);
	void repaint(cv::Vec3b color);
protected:
	std::string text_;
	void label();
private:
};

class CheckBox : public Widget
{
public:
	CheckBox(cv::Rect2i r);
	bool checked() const;
	void checked(bool);
	void on_change(std::function<void(bool)> f);
protected:
	bool checked_ = false;
private:
	void click(int, int);
};

/// CJK font, libhangul needed
class TextInput : public Widget
{
public:
	TextInput(cv::Rect2i r);
	std::string value() const;
	void value(std::string s);
	void enter(std::function<void(std::string)> f);
	void move_cursor(bool right);
	std::string type() const {return "TextInput";}
protected:
	std::string fore_, back_, editting_;
	void show_cursor(), draw();
	static HangulInputContext* hic_;
	static HanjaTable* table_;
	static bool hangul_mode_;// = false;

private:
	void flush(), del(), hanja(), backspace(), hangul();
	void popup(std::vector<std::string> v);
	void key_event(int key, int);
	const cv::Vec3b white = cv::Vec3b{255, 255, 255};
};

struct Wrapped
{
	cv::Rect2i rect;
	int fontsize;
	std::string title;
};

template<class T> class TwoD : public std::vector<std::vector<T>>
{
public:
	void push_back(T a) {
		tmp_.push_back(a);
	}
	void done() {
		std::vector<std::vector<T>>::push_back(std::move(tmp_));
	}
protected:
	std::vector<T> tmp_;
};

template<class T> auto to_number(std::string n) {
	if constexpr(std::is_same_v<int, T>) return stoi(n);
	else if constexpr(std::is_same_v<float, T>) return stof(n);
	else if constexpr(std::is_same_v<double, T>) return stod(n);
}

/** Window class houses all widgets including other windows */
class Window : public Widget
{
public:
	Window(std::string title, cv::Rect_<int> r);	
	void show(); ///< draw mat_
	void popup(Window &w, std::function<void(int)> f = [](int){});
	void popdown(int value);
	std::string type() const {return "Window"; }
	int open(int flag = cv::WINDOW_AUTOSIZE, int x = -1, int y = -1);
	void quit(int r);
	void focus(bool v);
	Window &operator+(Widget &w);
	Window &operator-(Widget &w);
	Window &operator<<(Widget &r);
	Window &operator>>(Widget &r);
	int loop();
	std::vector<Widget*>::iterator begin(), end();
	void close();
	void start(int flag = cv::WINDOW_AUTOSIZE | cv::WINDOW_KEEPRATIO);
	void keyboard_callback(int key, int level = 0);
	//void update(const Widget &r);
	std::string title() const;
	//void resize(cv::Rect2i r);
	void tie2(std::string title, int font, TextInput &t, Button &b, const std::vector<std::string> &v);
	template<class T> auto tie(TextInput &t, Button &b1, Button &b2, T start = 0, T step = 1) {
		b1.text("\u25b2"); b2.text("\u25bc");
		if(t.value() == "") t.value(std::to_string(start));
		*this << b1; *this << b2; *this << t;
		b1.click([&, step](){t.value(std::to_string(to_number<T>(t.value()) + step)); *this << t; show();});
		b2.click([&, step](){t.value(std::to_string(to_number<T>(t.value()) - step)); *this << t; show();});
		return [&t]() { return to_number<T>(t.value()); };
	}
	template<class... T> void tabs(int xpos, int ypos, T&... wins) {
		static TwoD<z::Window*> v;
		static TwoD<std::shared_ptr<z::Button>> bts;
		static std::vector<std::shared_ptr<z::Widget>> panels;
		const int button_width = 100;
		panels.push_back(std::make_shared<z::Widget>(cv::Rect2i{0,0,1,1}));
		static std::vector<int> max_zindex;
		int k = sizeof...(wins);
		(v.push_back(&wins), ...);
		v.done();
		v.back().front()->zIndex(2);
		panels.back()->zIndex(1);
		max_zindex.push_back(2);
		int sz = v.size(), shift = 0, max_w = 0, max_h = 0;
		for(auto *pw : v.back()) {
			pw->scrolled_rect_ = {0,0,0,0};
			pw->x = xpos; pw->y = ypos + 40;
			if(pw->width > max_w) max_w = pw->width;
			if(pw->height > max_h) max_h = pw->height;
			auto p = std::make_shared<z::Button>(pw->title(), 
					cv::Rect2i{xpos + shift++ * button_width, ypos, button_width, 30});
			*this + *p + *pw; 
			bts.push_back(p);
			p->click([this, sz, pw]() {
				int z = max_zindex[sz-1];
				panels[sz-1]->zIndex(z+1);
				pw->zIndex(z+2); 
				max_zindex[sz - 1] = z + 2;
				organize_accordingto_zindex();
				show();
			});
		}
		bts.done();
		panels.back()->resize(cv::Rect2i{xpos, ypos + 40, max_w, max_h});
		*this + *panels.back(); 
	}

	void organize_accordingto_zindex();
	void move_widget(Widget &w, cv::Point2i p);
	void on_register();
	bool is_window() const {return true;}
	template<class... T> auto tie(T&... checks)
	{//radio button
		static std::vector<z::CheckBox*> v;
		int k = sizeof...(checks);
		int sz = v.size();
		(v.push_back(&checks), ...);
		for(int i=sz; i < sz + k; i++) v[i]->on_change([i, k, sz, this](bool) {
					for(int j=sz; j < sz + k; j++) {
						if(i != j) v[j]->checked(false);
						else v[j]->checked(true);
						v[j]->update();
					}
				});
		return [sz, k, &v]() {
			for(int i=sz; i<sz+k; i++) if(v[i]->checked()) return i - sz;
		};
	}
	template<class... T> void wrap(const char* title, int font, int N, const T&... widgets)
	{//N : margin, font : font height
		std::vector<int> xs, ys;
		(xs.push_back(widgets.x), ...);
		(xs.push_back(widgets.br().x), ...);
		(ys.push_back(widgets.y), ...);
		(ys.push_back(widgets.br().y), ...);
		auto p = std::minmax_element(xs.begin(), xs.end());
		auto q = std::minmax_element(ys.begin(), ys.end());
		cv::Point2i ul = {*p.first -N, *q.first -N};
		wrapped_.push_back({cv::Rect2i{ul, cv::Point{*p.second + N, *q.second + N}}, font, title});
		draw_wrapped(wrapped_.back());
	}
	cv::Rect2i scrolled_rect_ = cv::Rect2i{0,0,0,0};
	std::vector<Widget*> widgets_, backup_;
	void draw_all_wrapped();
	void scroll_to(cv::Rect2i r);
	void set_ul(cv::Point2i p);
	void set_br(cv::Point2i p);
protected:
	std::string title_;
	void draw_wrapped(const Wrapped &wr);
	bool closed_ = false;
	int result_ = -1;
private:
	z::Window *popup_on_ = nullptr;
	std::function<void(int)> popup_exit_func_;
	std::vector<Wrapped> wrapped_;
	void copy_widget_to_mat(const Widget &r);
};

class Handle;

class VHandle : public Widget
{
public:
	VHandle(Handle&);
	const static int widget_width_ = 10;
	void draw();
private:
	Handle &handle_;
	bool mouse_down_ = false;
	void on_register();
	int starty_, endy_;
	friend class Handle;
};

class HHandle : public Widget
{
public:
	HHandle(Handle&);
	const static int widget_height_ = 10;
	void draw();
private:
	Handle &handle_;
	bool mouse_down_ = false;
	Window *scwin_;
	void on_register();
	int startx_, endx_;
	friend class Handle;
};

class Handle : public Widget
{
public:
	Handle();
	const static int widget_size_ = 30;
	VHandle vh_;
	HHandle hh_;
private:
	bool mouse_down_ = false;
	Window *scwin_;
	cv::Rect2i scroll_backup_ = {0,0,0,0};
	void on_register();
	void position_widgets();
	void show_widgets();
	void routine(int, int);
};

struct Line {
	std::string fore, editting, back;
	bool new_line = false;
};

class TextInput2 : public TextInput
{
public:
	using iter = std::list<Line>::iterator;
	TextInput2(cv::Rect2i r);
	void down_stream(iter it, int level = 0);///< call with inserted node iterator
	void up_stream(iter it);
	bool empty() const;
	Line line() const;
	void line(Line l);
protected:
	void focus(bool tf);
	void set_iter(iter it);
	TextInput2 *prev_ = nullptr, *next_ = nullptr;
	std::list<Line> *contents_ptr_ = nullptr;
	bool end_new_line_ = false;
	iter it_;
	//std::list<std::array<std::string, 3>> *contents_ = nullptr;
private:
	void up(), down(), new_line();
	bool is_end() const { return contents_ptr_->end() == it_;}
	friend class TextBox;
};

class TextBox : public Window
{
public:
	TextBox(cv::Rect2i r, int lines);
	void set_max_character(int max);
	std::string type() const {return "TextBox";}
protected:
	int top_line_index_ = 0, focus_line_ = 0;
	std::list<Line> contents_;
	std::vector<std::shared_ptr<TextInput2>> inputs_;
	void draw();
private:
	Line line() const;
	void line(Line l);
	bool empty() const;
};

class Image : public Widget
{
public:
	Image(cv::Rect2i r);
	cv::Mat &operator=(const cv::Mat &r);
};

class Slider : public Widget
{
public:
	Slider(cv::Rect2i r, int start, int stop, int step);
	int value();
	void value(int);
	void on_change(std::function<void(int)> f);//front int = value, second int = discard
	void draw();
protected:
	int value_, start_, end_, step_, logical_length_, physical_length_;
	bool hold_ = false;
private:
	std::function<void(int)> on_change_;
	void key_event(int key, int);
	void move(int x, int y);
	void ldown(int x, int y);
	void lup(int x, int y);
	int to_pos(int val), to_val(int pos);
	bool user_hold_ = false;
};

class Progress : public Widget
{
public:
	Progress(cv::Rect2i r);
	void value(int val);
	int value() const;
protected:
	int value_ = 0;
};

class AsciiWindow : public Window
{
public:
	AsciiWindow(const char *asciiart, int unit_width = 10, int unit_height = 15, 
			int margin = 1);//, int x = 0, int y = 0);
	void title(std::string t) { title_ = t; }
protected:
	std::vector<std::shared_ptr<Slider>> S;
	std::vector<std::shared_ptr<Button>> B;
	std::vector<std::shared_ptr<TextInput>> T;
	std::vector<std::shared_ptr<CheckBox>> C;
	std::vector<std::shared_ptr<Label>> L;
	std::vector<std::shared_ptr<Image>> I;
	std::vector<std::shared_ptr<Progress>> P;
	std::vector<std::shared_ptr<Widget>> Z;
	std::vector<std::shared_ptr<TextBox>> E;
private:
	int get_size(char c);
	bool parse_widget_area(int y, int x);
	void parse_art();
	int uw_, uh_, margin_;
	std::vector<std::string> art_, parsed_;
};

class PopupInterface {
public:
	PopupInterface(Window *p);
	int open(int flag = cv::WINDOW_AUTOSIZE, int x = -1, int y = -1);
	void quit(int r);
protected:
	bool closed_ = false;
	int result_ = -1;
private:
	Window *window_ptr_ = nullptr;
};

class WButton : public Widget
{
public:
	WButton(cv::Rect2i r);
};

}
