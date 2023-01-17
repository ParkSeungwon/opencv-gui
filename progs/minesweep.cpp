#include<iostream>
#include<fstream>
#include<chrono>
#include<algorithm>
#include<random>
#include<thread>
#include"src/zgui.h"
#include"option.h"
using namespace std;

struct Interface {
	virtual void dig(int x, int y) {}
	virtual void clear(int x, int y, string s) {}
	virtual void popup(string s) {}
	virtual void run_callback(int x, int y) {}
	static Interface *pMine;
	static Interface *pWin;
};

Interface* Interface::pMine = nullptr;
Interface* Interface::pWin = nullptr;

class MineButton : public z::Button, public Interface
{
public:
	MineButton(int x, int y, int w) : z::Button{"", cv::Rect2i{x*w, y*w, w, w}}
	{
		text(" ");
		user_callback_[cv::EVENT_RBUTTONUP] = [this](int, int)
		{ if(text_ == " ") text("v");
			else if(text_ == "v") text(" ");
			*parent_ << *this;
			parent_->show();
		};
		gui_callback_[EVENT_LEAVE] = [] (int, int) {};
		gui_callback_[EVENT_ENTER] = [] (int, int) {};
		gui_callback_[cv::EVENT_LBUTTONDOWN] = [this, x, y] (int, int) {};
		gui_callback_[cv::EVENT_LBUTTONDBLCLK] = [this, x, y] (int, int) {
			if(text_ != "v") {
				pWin->run_callback(x-1, y-1);
				pWin->run_callback(x, y-1);
				pWin->run_callback(x+1, y-1);
				pWin->run_callback(x-1, y);
				pWin->run_callback(x+1, y);
				pWin->run_callback(x-1, y+1);
				pWin->run_callback(x, y+1);
				pWin->run_callback(x+1, y+1);
			}
		};
		gui_callback_[cv::EVENT_LBUTTONUP] = [this, x, y](int, int) {
			if(text_ != "v") {
				pMine->dig(x, y);
				gui_callback_[cv::EVENT_LBUTTONUP] = [] (int, int) {};
			}
		};
	}
	void clear(string s) {
		mat_ = background_color_;
		text_ = s;
		draw(background_color_, false);
		update();
	}
};


struct Popup : z::AsciiWindow
{
	Popup() : z::AsciiWindow{R"(
	W-------------------------
	|
	|    B0-------------
	|    | Failed | 
	|    |
	|
	)"}
	{
		B[0]->click([this](){popdown(0);});
	}
	void set_text(string s) {
		B[0]->text(s);
	}
};

struct Win : z::Window, Interface
{ 
	Win(int w, int h, int button_w) 
		: z::Window{"mine", cv::Rect2i{0, 0, button_w*w, button_w*h}}
	  , th{&Win::time_pass, this}
	{ 
		this->button_w = button_w;
		for(int x=0; x<w; x++) for(int y=0; y<h; y++) {
			v[x][y] = make_shared<MineButton>(x, y, button_w);
			*this + *v[x][y];
		}
		start(cv::WINDOW_AUTOSIZE, 500, 500);
		pWin = this;
		tp_ = chrono::system_clock::now();
	}
	~Win() {
		run = false;
	}
	chrono::time_point<chrono::system_clock> tp_;
	std::shared_ptr<MineButton> v[100][100] = {nullptr,};
	std::jthread th;
	bool run = true;
	Popup pop;
	int button_w = 30;
	void popup(string s) { //set popup text and show
		auto sec = chrono::duration_cast<chrono::seconds>(chrono::system_clock::now() - tp_ );
		pop.set_text ( s + " : " +  to_string(sec.count()) );
		if(s == "성공") {
			append_file(sec.count());
			best_score();
		}
		pop.popup(*this, [](int){cv::destroyAllWindows();});
	}
	void run_callback(int x, int y) {
		if(x >=0 && y >=0 && v[x][y] != nullptr)
			v[x][y]->gui_callback_[cv::EVENT_LBUTTONUP](x, y);
	}
	void clear(int x, int y, string s) {
		v[x][y]->clear(s);
	}
	void append_file(int k) {
		ofstream f{"bestscore.txt", fstream::app};
		f << (width / button_w) << ' ' << (height / button_w) << ' ' << k << '\n';
	}
	void best_score() {
		ifstream f{"bestscore.txt"};
		int w, h, score;
		vector<int> scores;
		while(f >> w >> h >> score) {
			if(w == width / button_w && h == height / button_w) {
				scores.push_back(score);
			}
		}
		std::sort(scores.begin(), scores.end(), [](int a, int b) { return a < b; });
		cout << "best score" << width / button_w << 'x' << height / button_w << endl;
		for(int i=0; i<std::min(10, int(scores.size())); i++) cout << i+1 << ". " << scores[i] << endl;
	}
	void time_pass() {
		while(run) {
			this_thread::sleep_for(1s);
			auto sec = chrono::duration_cast<chrono::seconds>(chrono::system_clock::now() - tp_ );
			cout << to_string(sec.count()) << "초 지났습니다" << endl;
		}
	}
};



struct Mine : public Interface {
	Mine() {
		for(int x=0; x<100; x++) for(int y=0; y<100; y++) mine[x][y] = 'x';
		pMine = this;
	}
	void setup(int w, int h) {
		for(int x=0; x<w; x++) for(int y=0; y<h; y++) mine[x][y] = ' ';
	}
	char mine[100][100];

	bool check_bomb(int x, int y) {
		if(x < 0 || y < 0) return 0;
		if(mine[x][y] == '*') return 1;
		return 0;
	}

	int count_bomb(int x, int y) {
		if(x< 0 || y<0) return -1;
		return check_bomb(x -1, y-1) + check_bomb(x, y-1) + check_bomb(x+1, y-1) + check_bomb(x-1, y) 
			+ check_bomb(x+1, y) + check_bomb(x-1, y+1) + check_bomb(x, y+1) + check_bomb(x+1, y+1);
	}

	void dig(int x, int y) {
		static int opened = 0;
		if(x<0 || y<0) return;
		cout << x << ' ' << y << endl;
		if(mine[x][y] == 'x' || mine[x][y] == 'v') return;
		if(mine[x][y] == ' ') {
			mine[x][y] = 'v';
			int bomb_count = count_bomb(x, y);
			if(bomb_count == 0) {
				dig(x-1, y-1);
				dig(x, y-1);
				dig(x+1, y-1);
				dig(x-1, y);
				dig(x+1, y);
				dig(x-1, y+1);
				dig(x, y+1);
				dig(x+1, y+1);
				pWin->clear(x, y, "");
			} else pWin->clear(x, y, to_string(bomb_count));
		} else {
			pWin->clear(x, y, string{""} + mine[x][y]);
			pWin->popup("실패");
		}
		if(to_open_ == ++opened) pWin->popup("성공");
	}

	int scatter_bomb(int x, int y) {
		int count = x * y / 6;
		random_device rd;
		uniform_int_distribution<> dix{0, x-1};
		uniform_int_distribution<> diy{0, y-1};
		for(int i=0; i<count; i++) {
			while(true) {
				char &c = mine[dix(rd)][diy(rd)];
				if(c != '*') { c = '*'; break; }
			} 
		}
		to_open_ = x * y - count;
		return count;
	}

	int to_open_;
};



int main(int ac, char** av) {
	CMDoption co{
		{"width", "width size", 40},
		{"height", "height size", 30},
		{"button size", "button size", 40}
	};
	if(!co.args(ac, av)) return 0;
	int w = co.get<int>("width");
	int h = co.get<int>("height");
	int bu = co.get<int>("button size");
	Mine mine;
	mine.setup(w, h);
	mine.scatter_bomb(w, h);
	Win win{w, h, bu};
	win.loop();
}

