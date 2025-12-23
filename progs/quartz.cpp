#include"src/zgui.h"
using namespace std;

struct Quartz : z::AsciiWindow
{
  Quartz() 
	: z::AsciiWindow
		{ R"(
			WCalculator-------------------------------------
			|
			| Q0--Q1--Q2--Q3--Q4--Q5--Q6--Q7--Q8--Q9--   
			| ||  ||  ||  ||  ||  ||  ||  ||  ||  |0|     
			| |   |   |   |   |   |   |   |   |   |       
			| |   |   |   |   |   |   |   |   |   |       
			|                                             
			| B>-----B?-----B@-----Z0----- V0-----------  
			| |AC|   |%|    ||     ||      ||             
			| |      |      |      |       |              
			| |      |      |      |       |              
			| B7-----B8-----B9-----B:----- |              
			| |7|    |8|    |9|    |-|     |            
			| |      |      |      |       |
			| |      |      |      |       |
			| B4-----B5-----B6-----B;----- |
			| |4|    |5|    |6|    |+|     |
			| |      |      |      |       |
			| |      |      |      |       |
			| B1-----B2-----B3-----B<----- |
			| |1|    |2|    |3|    |=|     |
			| |      |      |      |       L0 C0- L1 C1-
			| |      |      |      |       |R|||  |G||| 
			| B0------------B=-----|       L2 C2- L3 C3-  
			| |0|           |.|    |       |B|||  |K||v|
			| |             |      |       Z1-----------
			| |             |      |       || 
			|
      |  G0--------------------------------
      |  |0 220 5 5|
      |  |
      |  |
      |  |
      |  |
      |  |
      |  |
      |  |
      |  |
      |  |
      |  |
      |  |
      |  |
      |  |
      |  |
      |  |
      |  |
      |  |
      |  |
      |
      |  S0--------------------------------
      |  |0 220 1|
      |
			|)",10 ,15 ,0
		}
	{
		*this + bt + bt2;
		bt.resize(*Z[0]); // if the original mat_ of bt_ is too small resize can't properly reflect contents.
		bt2.resize(*Z[1]);
		bt2.draw();
    //resize({0, 0, width, height + G[0]->width});
    //G[0]->resize({G[0]->x, G[0]->y, G[0]->width, G[0]->width});
    S[0]->on_change([this](int v) {G[0]->value(v);});

		auto f1 = tie_with_callback
		( [this](int i)
			{ cv::Vec3b color[4] = {{0, 0, 255}, {0, 255, 0}, {255, 0, 0}, {0, 0, 0}};
			  V[0]->draw_color(color[i]);
			}
		, *C[0], *C[1], *C[2], *C[3]);

		B[16]->text("\u00f7");
		B[10]->text("\u2212");
		organize_accordingto_zindex();
		start();
		for(int i=0; i<10; i++) B[i]->click([this, i]() { enter(i);});
		B[10]->click([this]() { // -
			store = current;
			mode = '-';
			current = 0;
		});
		B[11]->click([this]() {// + press
			store = current;
			mode = '+';
			current = 0;
		});
		B[12]->click([this]() { // = press
			switch(mode) {
				case '-': current = store - current; break;
				case '+': current += store; break;
				case '*': current *= store; break;
				case '/': current = store / current; break;
				default: return;
			}
			set(current);
		});
		B[14]->click([this]() {// AC
			current = 0;
			mode = ' ';
			set(0);
		});
		B[15]->click([this]() {
			current /= 100;
			set(current);
		});
		B[16]->click([this]() {// /
			store = current;
			mode = '/';
			current = 0;
		});
		bt.click([this]() {// *
			store = current;
			mode = '*';
			current = 0;
		});
		bt2.click([this]() {
			V[0]->clear();
		});
	}

	z::Button bt{"\u00d7", {0,0,100,100}};
	z::Button bt2{"clear", {0, 0, 300, 300}};
	long current = 0, store;;
	char mode = ' ';
	void enter(int i) {
		if(Q[1]->number() == 10) {
			current *= 10;
			current += i;
			set(current);
		}
	}
	void set(int k) {
		bool minus = false;
		if(k < 0) {
			k = -k;
			minus = true;
		}

		for(int i=9; i>=0; i--) {
			if(k == 0 && i != 9) Q[i]->number(10);
			else {
				Q[i]->number(k % 10);
				k /= 10;
			}
		}

		if(minus) {
			for(int i=9; i>=0; i--) if(Q[i]->number() == 10) {
				Q[i]->number(11);
				break;
			}
		}
	}
};

int main()
{
	Quartz win;
	win.loop();
}


