#include<map>
#include<type_traits>
#include<functional>
#include<opencv2/highgui.hpp>
#include<opencv2/freetype.hpp>
#include<opencv2/imgproc.hpp>
#define EVENT_ENTER 8000
#define EVENT_LEAVE 8001
#define EVENT_KEYBOARD 8002

namespace z {

class Window;

class Widget : public cv::Rect_<int>
{//base class for all widgets
public:
	Widget(cv::Rect_<int> r);
	bool is_updated();
	//void register_callback(int event, std::function<void(int,int)> f);
	bool focus();
	void focus(bool);
	void resize(cv::Rect2i r);
	std::map<int, std::function<void(int, int)>> gui_callback_;//int : event, x, y
	std::map<int, std::function<void(int, int)>> user_callback_;
	cv::Mat3b mat_;//widget shape
	void update();
	Window *parent_;

protected:
	static const cv::Vec3b background_color_, widget_color_, highlight_color_, click_color_;
	void shade_rect(cv::Rect2i r, int shade = 3, cv::Vec3b color = widget_color_,
			cv::Vec3b upper_left = highlight_color_, cv::Vec3b lower_right = click_color_);
	bool focus_ = false;
	static cv::Ptr<cv::freetype::FreeType2> ft2_;
};

class Label : public Widget
{
public:
	Label(std::string text, cv::Rect2i r);
	void text(std::string s);
	std::string text();
protected:
	std::string text_;
};

class Button : public Widget
{
public:
	Button(std::string text, cv::Rect_<int> r);
	void click(std::function<void()> f);
	void text(std::string s);
protected:
	std::string text_;
	void label();
private:
	void repaint(cv::Vec3b color);
};

class CheckBox : public Widget
{
public:
	CheckBox(cv::Rect2i r);
	bool checked();
	void checked(bool);
	void on_change(std::function<void(bool)> f);
protected:
	bool checked_ = false;
private:
	void click(int, int);
};

class TextInput : public Widget
{
public:
	TextInput(cv::Rect2i r);
	std::string value();
	void value(std::string s);
	void enter(std::function<void(std::string)> f);
protected:
	std::string value_;
private:
	void key_event(int key, int);
	const cv::Vec3b white = cv::Vec3b{255, 255, 255};
};

struct Wrapped
{
	cv::Rect2i rect;
	int fontsize;
	std::string title;
};

class Window : public Widget
{
public:
	Window(std::string title, cv::Rect_<int> r);
	void show();
	void popup(Window &w, std::function<void(int)> f);
	void popdown(int value);
	Window &operator+(Widget &w);
	Window &operator-(Widget &w);
	Window &operator<<(Widget &r);
	int loop();
	std::vector<Widget*>::iterator begin(), end();
	void close();
	void start(int flag = cv::WINDOW_AUTOSIZE | cv::WINDOW_KEEPRATIO);
	void keyboard_callback(int key);
	void update(const Widget &r);
	std::string title();
	void resize(cv::Rect2i r);
	void tie(std::string title, int font, TextInput &t, Button &b, std::vector<std::string> v, int x = -1, int y = -1);
	void tie2(std::string title, int font, TextInput &t, Button &b, std::vector<std::string> v);
	void tie(TextInput &t, Button &b1, Button &b2, double start = 0, double step = 1);
	template<class... T> void tie(T&... checks)
	{//radio button
		static std::vector<z::CheckBox*> v;
		int k = sizeof...(checks);
		int sz = v.size();
		(v.push_back(&checks), ...);
		for(int i=sz; i < sz + k; i++) v[i]->on_change([i, k, sz, this](bool) {
					for(int j=sz; j < sz + k; j++) {
						if(i != j) v[j]->checked(false);
						else v[j]->checked(true);
						*this << *v[j];
					}
				});
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
protected:
	std::string title_;
	std::vector<Widget*> widgets_, backup_;
	void draw_wrapped(const Wrapped &wr);
private:
	z::Window *popup_on_ = nullptr;
	std::function<void(int)> popup_exit_func_;
	std::vector<Wrapped> wrapped_;
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
	int value();
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
private:
	int get_size(char c);
	bool parse_widget_area(int y, int x);
	void parse_art();
	int uw_, uh_, margin_;
	std::vector<std::string> art_, parsed_;
};

class InnerPopup : public AsciiWindow
{
public:
	InnerPopup(cv::Rect2i r);

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

struct Popup : Window, PopupInterface
{
	Popup(std::string title, cv::Rect2i r);
};

struct AsciiPopup : AsciiWindow, PopupInterface
{
	AsciiPopup(const char *p, int uw = 10, int uh = 15, int margin = 1);
};

}
