#include<iostream>
#include<thread>
#include<random>
#include"zgui.h"
using namespace std;

void dig(int x, int y);
struct Interface {
	virtual void dig(int x, int y) {}
	virtual void clear(int x, int y, string s) {}
	virtual void popup(string s) {}
	static Interface *pMine;
	static Interface *pWin;
};

Interface* Interface::pMine = nullptr;
Interface* Interface::pWin = nullptr;

class MineButton : public z::Button, Interface
{
public:
	MineButton(int x, int y) : z::Button{"", cv::Rect2i{x*30, y*30, 30, 30}}
	{
		text(" ");
		user_callback_[cv::EVENT_RBUTTONUP] = [this](int, int)
		{ if(text_ == " ") text("v");
			else if(text_ == "v") text(" ");
		};
		gui_callback_[EVENT_LEAVE] = [] (int, int) {};
		gui_callback_[EVENT_ENTER] = [] (int, int) {};
		gui_callback_[cv::EVENT_LBUTTONDOWN] = [this, x, y] (int, int) {};
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
		label();
		update();
	}
};


struct Popup : z::AsciiWindow
{
	Popup() : z::AsciiWindow{R"(
	Wt-----------------------------
	|
	|    B0---------------
	|    | Failed | 
	|
	)"}
	{
		B[0]->click([this](){popdown(0);});
	}
	void set_text(string s) {
		B[0]->text(s);
		z::Widget::update();
	}
};

struct Win : z::Window, Interface
{ 
	Win(int w, int h) : z::Window{"mine", cv::Rect2i{0, 0, 30*w, 30*h}}
	{ 
		for(int x=0; x<w; x++) for(int y=0; y<h; y++) {
			v[x][y] = make_shared<MineButton>(x, y);
			*this + *v[x][y];
		}
		start();
		pWin = this;
	}

	std::shared_ptr<MineButton> v[100][100] = {nullptr,};
	Popup pop;
	void popup(string s) { //set popup text and show
		pop.set_text(s);
		pop.popup(*this, [](int i){});
	}
	void clear(int x, int y, string s) {
		v[x][y]->clear(s);
	}
};



class Mine : public Interface {
public:
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
		if(to_open_ == ++opened) pWin->popup("성공");
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
	if(ac < 3) {
		cout << "run with width height parameters." << endl;
		return 1;
	}
	int w = stoi(av[1]);
	int h = stoi(av[2]);
	Mine mine;
	mine.setup(w, h);
	mine.scatter_bomb(w, h);
	Win win{w, h};
	win.loop();
}

