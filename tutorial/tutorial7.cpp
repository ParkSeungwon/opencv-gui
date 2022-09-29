#include<iostream>
#include"src/zgui.h"
using namespace std;

struct NewWin : z::AsciiWindow
{
	NewWin() : z::AsciiWindow{R"(
    WNew------------------
		|  B0----------------
		|  |A new window|
		|)"}
	{
		organize_accordingto_zindex();
		B[0]->click([this](){quit(7);});
	}
};

struct Win : z::AsciiWindow
{
	Win() : z::AsciiWindow{R"(
	  WMultiple Window example------------
		|
		|  B0---------------
		|  |New Window|
		|)"}
	{
		start();
		B[0]->click([this]() { if(w.open()==7) cout << "closed" << endl; });
	}
	NewWin w;
};

int main() {
	Win win;
	win.loop();
}

