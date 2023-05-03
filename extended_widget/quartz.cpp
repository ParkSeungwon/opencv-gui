#include"quartz.h"
using namespace std;

struct Q : z::AsciiWindow
{
  Q() 
	: z::AsciiWindow
		{ R"(
			WCalculator-------------------------------------
			|
			|    Z0--Z1--Z2--Z3--Z4--Z5--Z6--Z7--Z8--Z9-- 
			|    ||  ||  ||  ||  ||  ||  ||  ||  ||  ||    
			|    |   |   |   |   |   |   |   |   |   | 
			|    |   |   |   |   |   |   |   |   |   | 
			|    
			|    B>-----B?-----B@-----Z:-----
			|    |AC|   |%|    ||     ||    
			|    |      |      |      |    
			|    |      |      |      |
			|    B7-----B8-----B9-----B:-----
			|    |7|    |8|    |9|    |-|
			|    |      |      |      |
			|    |      |      |      |
			|    B4-----B5-----B6-----B;-----
			|    |4|    |5|    |6|    |+|
			|    |      |      |      |
			|    |      |      |      |
			|    B1-----B2-----B3-----B<-----
			|    |1|    |2|    |3|    |=|
			|    |      |      |      |
			|    |      |      |      |
			|    B0------------B=-----|    
			|    |0|           |.|    |
			|    |             |      |
			|    |             |      |
			|
			|)",10 ,15 ,0
		}
	{
		for(int i=0; i<10; i++) 
		{ q[i].resize(*Z[i]);
			*this + q[i];
		}
		*this + bt;
		bt.resize(*Z[10]); // resize just change widget's matrix dimension
		bt.draw(); // need to redraw with resized matrix
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
	}

	Quartz q[10];
	z::Button bt{"\u00d7", {0,0,1,1}};
	long current = 0, store;;
	char mode = ' ';
	void enter(int i) {
		if(q[1].number() == 10) {
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
			if(k == 0 && i != 9) q[i].number(10);
			else {
				q[i].number(k % 10);
				k /= 10;
			}
		}

		if(minus) {
			for(int i=9; i>=0; i--) if(q[i].number() == 10) {
				q[i].number(11);
				break;
			}
		}
	}
};

int main()
{
	Q win;
	win.loop();
}


