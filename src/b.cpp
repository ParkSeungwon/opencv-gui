#include<CvPlot/imp/cvplot.ipp>
#include<mutex>
#include<thread>
#include<iostream>
#include"asyncq.h"
#include"zgui.h"
using namespace std;
using namespace placeholders;

template<class T, int N, int V> requires (N >= 2 * V)//buffer should be more than 2 times
class RotateView : public array<T, N>
{//to show graph of V values without stopping (values are added dynamically)
public:
	void push_back(T val) {
		if(N - pos_ < V) (*this)[V - N + pos_ - 1] = val;
		(*this)[pos_++] = val;
		if(pos_ == N) pos_ = V;
	}
protected:
	int pos_ = 0;
};

class Pop : public z::Popup
{
public:
	Pop(string title, cv::Rect2i r) : Popup(title, r) {
		*this + sl_ + bt_ + la_;
		bt_.click([this]() { quit(sl_.value()); });
	}
protected:
	z::Slider sl_{{50, 50, 200, 30}, 0, 30, 1};
	z::Button bt_{"ok", {150, 100, 40, 30}};
	z::Label la_{"What do you want?", {50, 20, 200, 30}};
};

class Mat2 : public cv::Mat
{
	public:
		Mat2(const cv::Mat &r) {
			r.copyTo(*this);
		}
		Mat2 &operator=(const cv::Mat &r) {
			r.copyTo(*this);
			return *this;
		}
};

class Win : public z::Window
{
public:
	Win(string title, cv::Rect2i r) : z::Window{title, r} {
		input_.enter([](string s){cout << s << endl;});
		popup_.click([this]() {cout << pop_win_.open() << endl;});
		quit_.click([this](){ close();});
		sl_.on_change( [this](int val) { sl_label_.text(std::to_string(val)); *this << sl_label_; });
		chk_.on_change([this](bool checked) { lb_.text(checked ? "green" : "blue"); *this << lb_; });
		*this + popup_ + quit_ + click_ + chk_ + input_ + img_ + sl_ + lb_ + sl_label_;
		start();//main thread imshow or namedWindow should come first
		th_ = thread{&Win::run, this};
	}
	~Win() {
		run_ = false;
		th_.join();
	}
protected:
	
	std::thread th_;
	RotateView<double, 4000, 1000> v_;
	cv::Mat mat2_;
	Pop pop_win_{"yes no", {0, 0, 300, 300}};
	z::Button popup_{"Popup", {100, 100, 90, 30}}, quit_{"Quit", {200, 100, 80, 30}},
		click_{"click", {300, 100, 90, 30}};
	z::CheckBox chk_{{100, 200, 20, 20}};
	z::TextInput input_{{100, 300, 140, 30}};
	z::Image img_{{250, 200, 800, 500}};
	z::Slider sl_{{10, 10, 300, 30}, 0, 100, 1};
	z::Label lb_{"0", {130, 200, 70, 30}}, sl_label_{"", {320, 10, 40, 30}};
	//WQueue<Mat2, 1000> q1_{[this](cv::Mat a) { img_ = a; *this << img_;}};
	//WQueue<z::Image, 1000> q2_{[this](z::Image a) {*this << a; }};
	//WQueue<CVPLOT_LIBRARY_INTERFACE CvPlot::Axes, 1000> q_{[this](CVPLOT_LIBRARY_INTERFACE  CvPlot::Axes a) { q1_.push_back(a.render(500, 800)); }};
private:
	bool run_ = true;
	void run() {
		double x = 0;
		for(int i=0; i<2000; i++, x+=0.01) v_.push_back(sin(x));
		for(int i = 0; run_ && !chk_.checked(); i++, x+=0.01) {
			auto a = CvPlot::plot({&v_[i], sl_.value() * 10}, "-b");
			img_ = a.render(500, 800);
			*this << img_;//this should be executed after main win.start()
			v_.push_back(sin(x));
			if(i == 3000) i = -1;
			this_thread::sleep_for(10ms);
		}
	}
};

struct PopWin : z::AsciiWindow, z::PopupInterface
{
	PopWin() : z::AsciiWindow{R"(
	WInput-------------------------------------
	|
	|   L0--------------------------
	|   |Will you accept?|
	|   L1--------------------------
	|   |This is it.|
	|
	|   B0------  B1-------
	|   |Yes|     |No|
	|
	|)"}, z::PopupInterface(this)
	{
		B[0]->click([this](){quit(1);});
		B[1]->click([this](){quit(0);});
	}
};

struct Mywin : z::AsciiWindow {
	Mywin() : z::AsciiWindow{ R"(
		WThis is a test------------------------------------------
		|
		|     L0------------------    B3---------     S1-
		|     |Test Label|            |This is|       |0 100 1|
		|     S0----------------- L1---               |
		|     |0 100 1|           |50|                |
		|                                             |
		|     B0------- B1------- B2-----B4-------    |
		|     |Popup|   |Cancel|  |Quit| |123|        |
    |                                             |
		|
		|     C0 L2--------  C3 C4 C5                         
		|     || |check|     || || ||
		|     C1 L3----- 
		|     || |우리는|
		|     C2 L4----          C6
		|     || |사나이|     ||
		|                        
		|     T0--------B5-  T1------------B6-
		|     ||        ||   ||            B7-
		|
		|     I0---------------------  T2----------B8-
		|     |11.jpg|                 ||          ||
		|     |
		|     |
		|     |
		|     |
		|     |                         P0-------------------
		|     |                         ||
		|     |
		|     |                          T3------------
		|     |                          |hello|
		|     |
		|)", 14, 21 }//no tab inside
	{
		tie("choose 2", 30, *T[0], *B[5], {"text", "hello", "fjdkl", "fjdk", "우리는 사나이 진짜"});
		L[0]->text("\u2776"); update(*L[0]);
		//C[0]->on_change([this](bool checked) {L[2]->text( checked ? "On" : "Off"); *this << *L[2];});
		tie(*T[1], *B[6], *B[7], 0, 1);
		//*I[0] = cv::imread("/home/zeta/Pictures/11.jpg"); update(*I[0]);
		*this + bt_; update(bt_);
		P[0]->value(50); update(*P[0]);
		tie("choose one", 30, *T[2], *B[8], {"hello", "tehi", "is"});
		update(*B[8]);
		tie(*C[0], *C[1], *C[2]);
		tie(*C[3], *C[4], *C[5]);
		wrap("hlll", 25, 10, *C[3], *C[4], *C[5]);
		wrap("hsll", 25, 10, *B[0], *B[1], *B[2], *B[4]);
		wrap("check", 25, 10, *C[0], *C[1], *C[2], *L[2], *L[3], *L[4]);
		start();//namedWindow should be called before updating (*this << *I[0])

		S[1]->on_change([this](int val) {
				L[1]->text(to_string(100-val)); update(*L[1]);
				P[0]->value(100-val); update(*P[0]);
				show();
				});
		B[0]->click([this]() {cout << (pop.open() ? "Yes" : "No") << endl;});
		B[1]->click([this]() {L[0]->text(to_string(S[0]->value())); update(*L[0]);});
		B[2]->click([this]() {close();});
		B[3]->click([this]() {cout << "3" << endl;});
		B[4]->click([this]() {cout << cv::getWindowProperty(pop.title(), cv::WND_PROP_VISIBLE) << endl;});
	}

	z::Button bt_{"by", {0,0, 50, 50}};
	PopWin pop;
};

int main()
{
	Mywin win;
	return win.loop();
}

