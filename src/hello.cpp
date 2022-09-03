#include<iostream>
#include<thread>
#include"zgui.h"
using namespace std;

struct Win2 : z::PopupInterface, z::AsciiWindow
{ Win2() : z::PopupInterface{this}, z::AsciiWindow{R"(
	Wt---------------------------
	|
	|      B0------
	|      |Hello|
	|)", 15, 22, 1}
	{
		B[0]->click([this](){ popdown(0);});
	}
} ;

struct Win : z::AsciiWindow, z::PopupInterface
{ Win() : z::PopupInterface{this}, z::AsciiWindow{R"(
		WHello------------------------------------------------
		|
		|    B0-------------
		|    | Hello World |
		|    B1------------
		|    | 우리는 |
		|
		|
		|
		|
		|
		|
		|)", 12, 18, 2}
	{ win3.title("sss");
		start();
		B[0]->click([this](){cout << "Hello" << endl; if(win2.open()) cv::destroyAllWindows();});
		B[1]->click([this](){win3.popup(*this, [](int i){cout << i << endl;});});
	}

	Win2 win2, win3;
};

int main()
{
	Win win;
	win.loop();
}

