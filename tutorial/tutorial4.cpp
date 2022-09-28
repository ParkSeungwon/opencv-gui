#include<iostream>
#include"src/zgui.h"
using namespace std;

struct Win : z::Window 
{
	struct Tab1 : z::AsciiWindow {
		Tab1() : z::AsciiWindow{R"(
			WTab1----------------
			|
			|
			|   L0-------------
			|   |TAB1|
			|
			|)"}
		{ 
			organize_accordingto_zindex(); 
		}
	} tab1;
	struct Tab2 : z::AsciiWindow{
		Tab2() : z::AsciiWindow{R"(
		  WTab2------------------
			|
			|
			|   L0--------------
			|   |TAB2|
			|
			|)"}
		{ 
			organize_accordingto_zindex();
		}
	} tab2;
	Win() : z::Window{"Tab Example", {0, 0, 300, 200}}
	{
		tabs(10, 10, tab1, tab2);
		start();
	}
};


int main() {
	Win win;
	win.loop();
}


