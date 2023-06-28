#include"src/zgui.h"
using namespace std;

struct Quartz : z::AsciiWindow
{
  Quartz() 
	: z::AsciiWindow
		{ R"(
			WCalculator--------------------------------
			|
			| Q0--Q1--Q2--Q3--Q4--Q5--Q6--Q7--Q8--Q9-- 
			| ||  ||  ||  ||  ||  ||  ||  ||  ||  |0|    
			| |   |   |   |   |   |   |   |   |   | 
			| |   |   |   |   |   |   |   |   |   | 
			| 
			| B>-----B?-----B@-----Z0----- V0--------
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
			| |      |      |      |
			| B1-----B2-----B3-----B<-----
			| |1|    |2|    |3|    |=|
			| |      |      |      |
			| |      |      |      |
			| B0------------B=-----|    
			| |0|           |.|    |
			| |             |      |
			| |             |      |
			|
			|)",10 ,15 ,0
		}
	{
		*this + bt;
		bt.resize(*Z[0]); // if the original mat_ of bt_ is too small resize can't properly reflect contents.
		
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

	z::Button bt{"\u00d7", {0,0,100,100}};
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


